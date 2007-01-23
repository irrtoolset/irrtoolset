/* $Id$
// Copyright (c) 2001,2002                        RIPE NCC
//
// All Rights Reserved
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted,
// provided that the above copyright notice appear in all copies and that
// both that copyright notice and this permission notice appear in
// supporting documentation, and that the name of the author not be
// used in advertising or publicity pertaining to distribution of the
// software without specific, written prior permission.
//
// THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
// ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
// AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
// DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
// AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
 */
/*
 * RSd release 0.1:
 * Copyright (c) 1995 University of Southern California and/or
 * the International Business Machines Corporation.
 * All rights reserved. Refer to Particulars at the end of this file.
 * Portions of this code may fall under other copyrights; refer to
 * copyright notices at the end of this file.
 * 
 * Author: Ramesh Govindan, ISI
 *
 * This implementation is based on Grail, Cengiz Alaettinoglu's 
 * modifications to Grail, and on the algorithms presented in
 * Hopcroft & Ullmann.
 *
 */

#include "config.h"
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include "re2dfa.h"
#include "assert.h"

/*
 * Routines for converting an AS path based regular expression to
 * a deterministic finite state automaton. This implementation follows
 * the classical approach of first constructing a non-deterministic
 * finite automaton and then using the construction described in Chap 2
 * of Hopcroft & Ullmann to transform the non-deterministic automaton
 * to the deterministic one.
 */



/*
 * Each state of the NFA is represented by a bit in a bitmask.
 * The assumption here is that the NFA states are relatively small.
 * The size of the bit mask is compile time specifiable (increasable
 * in byte offset counts). The following structure maintains the
 * bit allocator and precomputed masks.
 */

struct _rd_bit {
    unsigned short	rb_offset;	/* Offset into byte array */
    unsigned char	rb_mask;	/* Mask within byte */
    unsigned char	rb_alloc;	/* 1 if that bit has been allocated */
    struct _rd_state	*rb_state;	/* State to which bit is allocated */
} rd_bit[RD_MAXASPSTATES];

/*
 * The following macros operate on fields in rd_bit. The fields
 * are initialized in rd_init().
 */

void debugging_hack() {
   /* this function lets me put a break point here... */
   fprintf(stderr,
           "InternalError: Too many states in AS path expression\n"
           "InternalError: Increase RD_MAXASPSTATES in src/re2dfa/re2dfa.h and rebuild.\n");
}

#define RD_ASSIGN_BIT(s) {					\
    int Xi;							\
    for (Xi = 0; Xi < RD_MAXASPSTATES; Xi++) {			\
        if (!rd_bit[Xi].rb_alloc) {			        \
            break;						\
        }							\
    }								\
    if (Xi == RD_MAXASPSTATES) {				\
	debugging_hack();	                                \
	exit(1);						\
    }								\
    (s)->rs_bit = Xi;						\
    rd_bit[Xi].rb_alloc = 1;					\
    rd_bit[Xi].rb_state = (s);					\
}

#define RD_RELEASE_BIT(s) {					\
    rd_bit[(s)->rs_bit].rb_alloc = 0;				\
    rd_bit[(s)->rs_bit].rb_state = NULL;			\
}

#define RD_SET_BIT(m, n) 					\
    ((m)[rd_bit[(n)].rb_offset] |= rd_bit[(n)].rb_mask)

#define RD_ISSET_BIT(m, n)					\
    ((m)[rd_bit[(n)].rb_offset] & rd_bit[(n)].rb_mask)

#define RD_RESET_BIT(m, n)					\
    ((m)[rd_bit[(n)].rb_offset] &= ~rd_bit[(n)].rb_mask)

#define RD_BIT_STATE(a)		(rd_bit[(a)].rb_state)

#define RD_EQUAL_BITS(a, b)					\
	(!bcmp((caddr_t) (a), (caddr_t) (b), RD_ASPSIZE))

#define RD_COPY_BITS(b1, b2) {					\
    bcopy((caddr_t) (b2), (caddr_t) (b1), RD_ASPSIZE);		\
}

#define RD_OR_BITS(b1, b2) {					\
    int Xi;							\
    for (Xi = 0; Xi < RD_ASPSIZE; Xi++) {			\
        (b1)[Xi] |= (b2)[Xi];					\
    }								\
}

/*
 * rd_init: initializes the rd_bit array.
 */

void
rd_init()
{
    static    	 first = 1;	/* Identifies code segments entered once */
    int i;			/* Current loop index */

    for (i = 0; i < RD_MAXASPSTATES; i++) {
	rd_bit[i].rb_alloc = 0;
	rd_bit[i].rb_state = NULL;
    }

    if (first) {
	first = 0;
	for (i = 0; i < RD_MAXASPSTATES; i++) {
	    rd_bit[i].rb_offset = (unsigned short) i / RD_NBBY;
	    rd_bit[i].rb_mask = (unsigned char) 0x80 >> (i % RD_NBBY);
	}
    }
}
    


/*
 * Range lists: These are constructed while parsing the AS path
 * expressions. Invariant: range list is always sorted by _low
 * values and successive ranges are disjoint.
 */

typedef struct _rd_arc rd_range; 	/* So we don't need another struct */
typedef struct _rd_dq  rd_rhead;	/* Ditto */

/*
 * Most of the allocs/frees are declared as macros so we can
 * easily Gated-ize the code.
 */

#define RD_ALLOC_RANGE(r) {					\
    (r) = (rd_range *) malloc(sizeof (rd_range));		\
    bzero((caddr_t) (r), sizeof (rd_range));			\
    RDQ_INIT(&(r)->ra_arcs, (r));				\
}

#define RD_FREE_RANGE(r)	{ free((r)); }

/*
 * Queue heads for ranges are simply the rd_dq structs.
 */

#define RD_ALLOC_RHEAD(q) {					\
    (q) = (rd_rhead *) malloc(sizeof (rd_rhead));		\
    RDQ_INIT((q), (q));						\
}

#define RD_FREE_RHEAD(q) 	{ free((q)); }

/*
 * rd_alloc_range: given two integer arguments, set the low
 * and high end of a range and return the range. 
 */

rd_range *
rd_alloc_range(unsigned int low,	
	       unsigned int high)
{
    rd_range *ra;			/* Allocated rd_range */

    RD_ALLOC_RANGE(ra);
    ra->ra_low = low;
    ra->ra_high = high;

    return ra;
}

/*
 * rd_alloc_range_list: given a single range, allocate a range head
 * to hold the range.
 */

rd_rhead *
rd_alloc_range_list(rd_range *ra)
{
    rd_rhead *rq;			/* Range head for list */

    RD_ALLOC_RHEAD(rq);
    RDQ_INSERT_BEFORE(rq, &(ra->ra_arcs));

    return rq;
}

rd_rhead *
rd_alloc_range_list_empty()
{
    rd_rhead *rq;			/* Range head for list */

    RD_ALLOC_RHEAD(rq);

    return rq;
}

/*
 * rd_insert_range: given a range list and a single range node,
 * insert the latter in order (sorted by the low element) into 
 * the former. Make sure the range list invariant is met. Return
 * the resulting range list.
 */

void
rd_insert_range(rd_rhead *rlist,
		rd_range *rtoins)
{
    register rd_dq *rq;		/* Temporary pointer to DLL struct */
    register rd_range *pra;	/* Temp. range pointer */

    /*
     * Scan the range list backwards. The first element whose low value
     * is less than that of rtoins is the one after which rtoins goes.
     * If that element (pra) overlaps with rtoins, adjust pra's range
     * accordingly and continue.
     */

    pra = NULL;
    for (rq = rlist->rq_back; rq->rq_self != rq; rq = rq->rq_back) {
	if (((rd_range *) (rq->rq_self))->ra_low <= rtoins->ra_low) {
	    pra = (rd_range *) (rq->rq_self);
	    if (pra->ra_high >= rtoins->ra_low) {
                if (pra->ra_high < rtoins->ra_high) {
                   pra->ra_high = rtoins->ra_high;
                }
		RD_FREE_RANGE(rtoins);
		rtoins = pra;
	    } else {
		RDQ_INSERT_AFTER(&(pra->ra_arcs), &(rtoins->ra_arcs));
	    }
	    break;
	}
    }

    if (!pra) {
	RDQ_INSERT_AFTER(rlist, &(rtoins->ra_arcs));
    }

    /*
     * Now delete all subsequent ranges whose low falls within
     * rtoins' range. If their high is greater than our current high,
     * set our current high to that.
     */

    rq = rtoins->ra_arcs.rq_forw;
    while (rq->rq_self != rq) {
	pra = (rd_range *) (rq->rq_self);
	if (pra->ra_low > rtoins->ra_high) {
	    break;
	}
	if (pra->ra_high > rtoins->ra_high) {
	    rtoins->ra_high = pra->ra_high;
	}
	rq = pra->ra_arcs.rq_forw;
	RDQ_UNLINK(&(pra->ra_arcs));
	RD_FREE_RANGE(pra);
    }

    return;
}

/*
 * rd_complement_range_list: given a list of ranges (fulfilling the
 * range invariant), simply complement the ranges. The range list is 
 * never empty.
 */

void
rd_complement_range_list(rd_rhead *rlist)
{
    register rd_dq *rq;			/* Temporary linked list elt ptr */
    register rd_range *ra; 		/* Element corresponding to rq */
    register rd_range *new_ra;		/* Element to be inserted into comp */

    RD_ALLOC_RANGE(new_ra);
    new_ra->ra_low = 0;

    rq = rlist->rq_forw;
    while (rq->rq_self != rq) {
	ra = (rd_range *) (rq->rq_self);
	if (ra->ra_low > new_ra->ra_low) {
	    new_ra->ra_high = ra->ra_low - 1;
	    RDQ_INSERT_BEFORE(rq, &(new_ra->ra_arcs));
	    RD_ALLOC_RANGE(new_ra);
	}
	rq = rq->rq_forw;
	new_ra->ra_low = ra->ra_high + 1;
	RDQ_UNLINK(&(ra->ra_arcs));
	RD_FREE_RANGE(ra);
    }

    if (new_ra->ra_low <= MAX_AS) {
	new_ra->ra_high = MAX_AS;
	RDQ_INSERT_BEFORE(rlist, &(new_ra)->ra_arcs);
    } else {
	RD_FREE_RANGE(new_ra);
    }

    return;
}



/*
 * RE -> NFA:
 * Finite state machine related stuff. This includes functions
 * for creating empty machines, singleton machines, for
 * concatenating (AND-ing) and alternating (OR-ing) machines,
 * and for closures.
 *
 * NOTE: We represent the multiple transitions out of the NFA
 * as a bitmask of target states to which an arc goes. We maintain
 * arc lists sorted by AS range, which are completely disjoint.
 *
 * NOTE: We maintain two invariants when building NFAs: 1) there
 * is only one start state and 2) the start state has no self loops.
 */

/*
 * Various macros for allocating and freeing data structures.
 */

#define RD_ALLOC_FM(f) {					\
    (f) = (rd_fm *) malloc(sizeof (rd_fm));			\
    bzero((caddr_t) (f), sizeof (rd_fm));			\
    RDQ_INIT(&((f)->rf_states), (f));				\
    RDQ_INIT(&((f)->rf_final), (f));				\
    f->eolexp = 0;                                              \
    f->bolexp = 0;                                              \
}

#define RD_FREE_FM(f) { 					\
    free((f)); 							\
}

#define RD_ALLOC_STATE(s) {					\
    (s) = (rd_state *) malloc(sizeof (rd_state));		\
    bzero((caddr_t) (s), sizeof (rd_state));			\
    RD_ASSIGN_BIT((s));						\
    RDQ_INIT(&((s)->rs_arcs), (s));				\
    RDQ_INIT(&((s)->rs_states), (s));				\
    RDQ_INIT(&((s)->rs_final), (s));				\
}

#define RD_FREE_STATE(s) {					\
    RD_RELEASE_BIT((s));					\
    free((s));							\
}

#define RD_ALLOC_ARC(a,l,h) {					\
    (a) = (rd_arc *) malloc(sizeof (rd_arc));		        \
    bzero((caddr_t) (a), sizeof (rd_arc));			\
    RDQ_INIT(&((a)->ra_arcs), (a));				\
    (a)->ra_low = l;						\
    (a)->ra_high = h;						\
}

#define RD_FREE_ARC(a) {					\
    free((a));							\
}

/*
 * rd_insert_arc: insert a given arc into a list of arcs.
 * We try to maintain the following invariant: the list
 * of arcs contains completely disjoint AS number ranges.
 * The parameter "arctoins" is returned unmodified.
 *
 * This is substantially similar to rd_insert_range,
 * except here we may need to break up the left and
 * right intersecting ranges and anything in between.
 */

static void
rd_insert_arc(rd_dq  *alist,		/* List of arcs */
	      rd_arc *arctoins)		/* Arc to insert */
{
    rd_dq 	*rq;		/* Current queue element in loop */
    rd_arc	*pra;		/* Arc prior to the one being inserted */
    rd_arc	*nra;		/* New arc to be inserted */
    rd_arc	*nnra;		/* Used when a three-way split is done */
    rd_arc	arc;		/* Local copy of arctoins */
    unsigned int high;		/* Temporary storage for high end of range */

    /*
     * Search the list backwards to find out arc "pra" whose low value
     * is just before that of arctoins. This is where we start inserting
     * arc. Otherwise, we start inserting arc at the head of the list.
     */

    memcpy(&arc, arctoins, sizeof(arc)); /* Struct copy */
    if (RDQ_ISEMPTY(alist)) {
	RD_ALLOC_ARC(nra, arc.ra_low, arc.ra_high);
	RD_OR_BITS(nra->ra_mask, arc.ra_mask);
	RDQ_INSERT_AFTER(alist, &(nra->ra_arcs));
	return;
    }

    for (rq = alist->rq_back; rq->rq_self != alist->rq_self; rq = rq->rq_back) {
	if (arc.ra_low >= ((rd_arc *) rq->rq_self)->ra_low) {
	    goto found;
	}
    }
    rq = alist->rq_forw;

    /*
     * Here, "arc" cannot possibly intersect any arcs prior to "pra",
     * where "pra" is the arc of "rq". Seven cases arise:
     *	  - arc(lo) < pra(lo):
     *	  - arc(lo) == pra(lo) && arc(hi) < pra(hi): 
     *	  - arc(lo) == pra(lo) && arc(hi) == pra(hi): 
     *	  - arc(lo) == pra(lo) && arc(hi) > pra(hi):
     *	  - arc(lo) > pra(lo) && arc(hi) < pra(hi):
     *	  - arc(lo) > pra(lo) && arc(hi) == pra(hi):
     *	  - arc(lo) > pra(lo) && arc(hi) > pra(hi)
     *	  - arc(lo) > pra(hi): advance pointer.
     */

  found:
    while (1) {
	if (arc.ra_low > arc.ra_high) {
	    break;
	}
	if (rq->rq_self == alist) {
	    RD_ALLOC_ARC(nra, arc.ra_low, arc.ra_high);
	    RD_OR_BITS(nra->ra_mask, arc.ra_mask);
	    RDQ_INSERT_BEFORE(alist, &(nra->ra_arcs));
	    break;
	}
	pra = (rd_arc *) rq->rq_self;
	if (arc.ra_high < pra->ra_low) {
	    RD_ALLOC_ARC(nra, arc.ra_low, arc.ra_high);
	    RD_OR_BITS(nra->ra_mask, arc.ra_mask);
	    RDQ_INSERT_BEFORE(&(pra->ra_arcs), &(nra->ra_arcs));
	    return;
	}
	if (arc.ra_low < pra->ra_low) {
	    RD_ALLOC_ARC(nra, arc.ra_low, pra->ra_low-1);
	    RD_OR_BITS(nra->ra_mask, arc.ra_mask);
	    arc.ra_low = pra->ra_low;
	    RDQ_INSERT_BEFORE(&(pra->ra_arcs), &(nra->ra_arcs));
	    continue;
	}
	if ((arc.ra_low == pra->ra_low) && (arc.ra_high < pra->ra_high)) {
	    RD_ALLOC_ARC(nra, arc.ra_high+1, pra->ra_high);
	    RD_OR_BITS(nra->ra_mask, pra->ra_mask);
	    pra->ra_high = arc.ra_high;
	    RD_OR_BITS(pra->ra_mask, arc.ra_mask);
	    RDQ_INSERT_AFTER(&(pra->ra_arcs), &(nra->ra_arcs));
	    return;
	}
	if ((arc.ra_low == pra->ra_low) && (arc.ra_high == pra->ra_high)) {
	    RD_OR_BITS(pra->ra_mask, arc.ra_mask);
	    return;
	}
	if ((arc.ra_low == pra->ra_low) && (arc.ra_high > pra->ra_high)) {
	    RD_OR_BITS(pra->ra_mask, arc.ra_mask);
	    arc.ra_low = pra->ra_high+1;
	    rq = rq->rq_forw;
	    continue;
	}
	if ((arc.ra_low > pra->ra_low) && (arc.ra_high <= pra->ra_high)) {
	    /* Possibly split into three */
	    high = pra->ra_high;
	    pra->ra_high = arc.ra_low-1;
	    RD_ALLOC_ARC(nra, arc.ra_low, arc.ra_high);
	    RD_OR_BITS(nra->ra_mask, arc.ra_mask);
	    RD_OR_BITS(nra->ra_mask, pra->ra_mask);
	    RDQ_INSERT_AFTER(&(pra->ra_arcs), &(nra->ra_arcs));
	    if (arc.ra_high < high) {
		RD_ALLOC_ARC(nnra, arc.ra_high+1, high);
		RD_OR_BITS(nnra->ra_mask, pra->ra_mask);
		RDQ_INSERT_AFTER(&(nra->ra_arcs), &(nnra->ra_arcs));
	    }
	    return;
	}
	if ((arc.ra_low > pra->ra_low) && (arc.ra_low <= pra->ra_high) &&
	    (arc.ra_high > pra->ra_high)) {
	    /* Split into three */
	    pra->ra_high = arc.ra_low-1;
	    RD_ALLOC_ARC(nra, arc.ra_low, pra->ra_high);
	    RD_OR_BITS(nra->ra_mask, arc.ra_mask);
	    RD_OR_BITS(nra->ra_mask, pra->ra_mask);
	    RDQ_INSERT_AFTER(&(pra->ra_arcs), &(nra->ra_arcs));
	    arc.ra_low = pra->ra_high+1;
	    rq = nra->ra_arcs.rq_forw;
	    continue;
	}
	if (arc.ra_low > pra->ra_high) {
	    rq = rq->rq_forw;
	    continue;
	}
#ifdef SANITY
	assert(0);
#endif /* SANITY */
    }

    return;
}

#ifdef SANITY

/*
 * rd_check_invariant ensures that the start state of the
 * specified NFA does not have self-loops.
 */

static void
rd_check_invariant(rd_fm *fm)
{
    rd_state	*start;		/* Start state of fm */
    rd_arc	*ra;		/* Current arc in loop */

    start = fm->rf_start;
    RDQ_LIST_START(&(start->rs_arcs), start, ra, rd_arc) {
	if (RD_ISSET_BIT(ra->ra_mask, start->rs_bit)) {
	    printf("FATAL ERROR: self-loop invariant check failed.\n");
	    exit(1);
	}
    } RDQ_LIST_END(&(start->rs_arcs), start, ra, rd_arc);
}

#endif /* SANITY */

/*
 * rd_singleton creates an FA containing two
 * states (a start state and a final state), with arcs given
 * by the specified ranges. This FA accepts a single token.
 *
 * Note: we make use of the fact that the range list is also
 * an arc list, so we can avoid freeing storage here.
 */

rd_fm *
rd_singleton(rd_rhead *rlist) 
{
    register rd_fm *rf;			/* The singleton machine */
    register rd_state *rs;		/* A state of the singleton machine */
    register rd_range *ra;		/* Current element of range list */

    RD_ALLOC_FM(rf);
    
    /* The final state */
    RD_ALLOC_STATE(rs);
    RDQ_INSERT_BEFORE(&(rf->rf_states), &(rs->rs_states));
    RDQ_INSERT_BEFORE(&(rf->rf_final), &(rs->rs_final));

    RDQ_LIST_START(rlist, rlist, ra, rd_arc) {
	RD_SET_BIT(ra->ra_mask, rs->rs_bit);
    } RDQ_LIST_END(rlist, rlist, ra, rd_arc);

    /* The start state */
    RD_ALLOC_STATE(rs);
    RDQ_SPLICE(&(rs->rs_arcs), rlist);
    RD_FREE_RHEAD(rlist);
    RDQ_INSERT_BEFORE(&(rf->rf_states), &(rs->rs_states));
    rf->rf_start = rs;

#ifdef SANITY
    rd_check_invariant(rf);
#endif /* SANITY */

    return rf;
}

/*
 * rd_concatenate appends fm2 to fm1. It returns the resulting fm1,
 * releasing any resources held by fm2.
 */

rd_fm *
rd_concatenate(rd_fm *fm1,	/* To this fm.... */
	       rd_fm *fm2)	/* ... concatenate this fm */
{
    rd_dq	*rq;		/* Current linked list element */
    rd_state	*rs;		/* Current state we're at */
    rd_state	*start;		/* Start state of fm2 */
    rd_arc	*ra;		/* Current arc we're at */
    int		fm2e;		/* Does fm2 accept the empty string? */

    if (fm1->eolexp) {
       if (!RD_ACCEPTS_EMPTY_STRING(fm2) || fm2->bolexp) {
	  rd_free_nfa(fm1);
	  fm1 = rd_empty_set_machine();
       }
       rd_free_nfa(fm2);
       fm2 = NULL;
       return fm1;
    }
    if (fm2->bolexp) {
       if (RD_ACCEPTS_EMPTY_STRING(fm1) && !fm1->eolexp) {
	  rd_free_nfa(fm1);
	  fm1 = fm2;
	  fm2 = NULL;
       } else {
	  rd_free_nfa(fm1);
	  rd_free_nfa(fm2);
	  fm2 = NULL;
	  fm1 = rd_empty_set_machine();
       }
       return fm1;    
    }

    fm1->eolexp = fm2->eolexp;

    /*
     * Add all arcs of fm2's start state onto fm1's final states.
     * Delete arcs of fm2's start state and delete fm2's start state.
     */

    fm2e = RD_ACCEPTS_EMPTY_STRING(fm2);
    start = fm2->rf_start;

    while (RDQ_LIST_HEAD(&(start->rs_arcs), ra, rd_arc) != NULL) {
	RDQ_LIST_START(&(fm1->rf_final), fm1, rs, rd_state) {
	    rd_insert_arc(&(rs->rs_arcs), ra);
	} RDQ_LIST_END(&(fm1->rf_final), fm1, rs, rd_state);
	RDQ_UNLINK(&(ra->ra_arcs));
	RD_FREE_ARC(ra);
    }
    RDQ_UNLINK(&(start->rs_states));
    RDQ_UNLINK(&(start->rs_final));
    RD_FREE_STATE(start);
    
    /*
     * Now, append fm2's states list to fm1. If fm2 does not accept the
     * empty string, undo fm1's final states. Copy over to fm1 fm2's 
     * final states. Release fm2.
     */

    RDQ_SPLICE(&(fm1->rf_states), &(fm2->rf_states));
    if (!fm2e) {
	RDQ_UNLINK_LIST(&(fm1->rf_final), fm1);
    }
    RDQ_SPLICE(&(fm1->rf_final), &(fm2->rf_final));
    RD_FREE_FM(fm2);

#ifdef SANITY
    rd_check_invariant(fm1);
#endif /* SANITY */

    return fm1;
}

/*
 * rd_alternate OR's fm2 to fm1. 
 * Returns the OR'd fm1 and releases fm2.
 */

rd_fm *
rd_alternate(rd_fm *fm1,			/* To this NFA.. */
	     rd_fm *fm2)			/* .. OR this one */
{
    rd_state		*start1;		/* Fm1's start state */
    rd_state		*start2;		/* Fm2's start state */
    rd_arc		*ra;			/* Current arc in loop */

    if (fm1->bolexp)
       fm2 = rd_make_bol(fm2);
    else if (fm2->bolexp) 
	  fm1 = rd_make_bol(fm1);

    if (fm1->eolexp)
       fm2 = rd_make_eol(fm2);
    else if (fm2->eolexp) 
	  fm1 = rd_make_eol(fm1);

    /*
     * Insert each arc from start2 into start1.
     * Delete the arcs from start2. Delete start2.
     * Combine fm1 and fm2's states and final states.
     */

    start1 = fm1->rf_start;
    start2 = fm2->rf_start;

    while (RDQ_LIST_HEAD(&(start2->rs_arcs), ra, rd_arc) != NULL) {
	rd_insert_arc(&(start1->rs_arcs), ra);
	RDQ_UNLINK(&(ra->ra_arcs));
	RD_FREE_ARC(ra);
    }
    RDQ_UNLINK(&(start2->rs_states));
    RDQ_UNLINK(&(start2->rs_final));
    RD_FREE_STATE(start2);

    RDQ_SPLICE(&(fm1->rf_states), &(fm2->rf_states));
    RDQ_SPLICE(&(fm1->rf_final), &(fm2->rf_final));

    RD_FREE_FM(fm2);

#ifdef SANITY
    rd_check_invariant(fm1);
#endif /* SANITY */

    return fm1;
}

/*
 * rd_zero_or_one simply makes the start state of the specified
 * FM also final states. It returns the thus modified fm.
 */

rd_fm *
rd_zero_or_one(rd_fm *fm1)
{
   if (RDQ_ISEMPTY(&(fm1->rf_start->rs_final)))
      RDQ_INSERT_BEFORE(&(fm1->rf_final), &(fm1->rf_start->rs_final));
    return fm1;
}

/*
 * rd_one_or_more inserts all the arcs out of its start state into
 * each of its final states. It returns the resulting FM.
 */

rd_fm *
rd_one_or_more(rd_fm *fm1)
{
    rd_state	*rs;		/* Current state we're at */
    rd_state	*start;		/* The start state */
    rd_arc	*ra;		/* Current arc we're at */

    /* Note that (^AS1)+ == ^AS1, and (AS1$)+ == AS1$ */
    if (fm1->bolexp || fm1->eolexp)
       return fm1;

    start = fm1->rf_start;
    RDQ_LIST_START(&(start->rs_arcs), start, ra, rd_arc) {
	RDQ_LIST_START(&(fm1->rf_final), fm1, rs, rd_state) {
	    rd_insert_arc(&(rs->rs_arcs), ra);
	} RDQ_LIST_END(&(fm1->rf_final), fm1, rs, rd_state);
    } RDQ_LIST_END(&(start->rs_arcs), start, ra, rd_arc);

#ifdef SANITY
    rd_check_invariant(fm1);
#endif /* SANITY */

    return fm1;
}

/*
 * rd_zero_or_more is simply the combination of one_or_more and
 * zero_or_one. It returns the resulting FM.
 */

rd_fm *
rd_zero_or_more(rd_fm *fm1) 
{
    return (rd_zero_or_one(rd_one_or_more(fm1)));
}



/*
 * DFA ->NFA:
 * The routine below converts a DFA to a corresponding NFA.
 */

/*
 * rd_dton() given a pointer to an FM/DFA converts that FM into
 * the corresponding NFA. 
 */

void
rd_dton(rd_fm *fm)
{
    rd_state 	*rs;		/* Current state in loop */
    rd_arc	*ra;		/* Current arc in loop */
    rd_state	*rsto;		/* State to which arc points */
    rd_state	*nstart;	/* New start state */
    rd_state	*ostart;	/* Old start state */
    int		newst = 0;	/* 1 if we need a new start state */

    /*
     * Walk through each state and assign a new bit number to it.
     * Then, walk through all the arcs and convert them to the NFA
     * arc representation. 
     */

    RDQ_LIST_START(&(fm->rf_states), fm, rs, rd_state) {
	RD_ASSIGN_BIT(rs);
    } RDQ_LIST_END(&(fm->rf_states), fm, rs, rd_state);

    RDQ_LIST_START(&(fm->rf_states), fm, rs, rd_state) {
	RDQ_LIST_START(&(rs->rs_arcs), rs, ra, rd_arc) {
	    if ((rsto = ra->ra_to) == fm->rf_start) {
		newst = 1;
	    }
	    ra->ra_to = NULL;	/* XXX */
	    RD_SET_BIT(ra->ra_mask, rsto->rs_bit);
	} RDQ_LIST_END(&(rs->rs_arcs), rs, ra, rd_arc);
    } RDQ_LIST_END(&(fm->rf_states), fm, rs, rd_state);

    /*
     * Add a new start state with the same arcs as the current
     * start state. The new start state is also a final state
     * if the current start state is a final state.
     */

    if (!newst)
	return;
    RD_ALLOC_STATE(nstart);
    ostart = fm->rf_start;
    RDQ_LIST_START(&(ostart->rs_arcs), ostart, ra, rd_arc) {
	rd_insert_arc(&(nstart->rs_arcs), ra);
    } RDQ_LIST_END(&(ostart->rs_arcs), ostart, ra, rd_arc);

    RDQ_INSERT_BEFORE(&(fm->rf_states), &(nstart)->rs_states);
    if (RD_IS_FINAL(ostart)) {
	RDQ_INSERT_BEFORE(&(fm->rf_final), &(nstart)->rs_final);
    }
    fm->rf_start = nstart;

    return;
}



/*
 * NFA->DFA:
 * The routines below are used to convert a given non-deterministic
 * finite automaton into the corresponding deterministic finite automaton.
 * The algorithm is straight out of Hopcroft and Ullmann.
 */

/*
 * Same as RD_ALLOC_STATE, but it does not allocate the state bit.
 */

#define RD_ALLOC_DFA_STATE(s) {					\
    (s) = (rd_state *) malloc(sizeof (rd_state));		\
    bzero((caddr_t) (s), sizeof (rd_state));			\
    RDQ_INIT(&((s)->rs_arcs), (s));				\
    RDQ_INIT(&((s)->rs_states), (s));				\
    RDQ_INIT(&((s)->rs_final), (s));				\
}

#define RD_FREE_DFA_STATE(s) {					\
    free((s));							\
}

/*
 * rd_free_nfa releases all the resources held by the NFA: states, arcs etc.
 * Other code assumes this function works with dfa too.
 */

void rd_free_nfa(rd_fm *nfa) 
{
    rd_state	*rs;		/* Current state in loop */
    rd_arc	*ra;		/* Current arc in loop */

    /* 
     * Go through all the states, unlink them from the start and
     * final states lists, remove all arcs.
     */

    while (RDQ_LIST_HEAD(&(nfa->rf_states), rs, rd_state) != NULL) {
	RDQ_UNLINK(&(rs->rs_final));
	while (RDQ_LIST_HEAD(&(rs->rs_arcs), ra, rd_arc) != NULL) {
	    RDQ_UNLINK(&(ra->ra_arcs));
	    RD_FREE_ARC(ra);
	}
	RDQ_UNLINK(&(rs->rs_states));
	RD_FREE_STATE(rs);
    }
    
    RD_FREE_FM(nfa);
    return;
}    

/*
 * rd_merge_arcs takes a list of DFA arc ranges and tries to collapse
 * successive arcs into one.
 */

static void
rd_merge_arcs(rd_dq *alist)
{
    rd_dq 	*rq;		/* Current arc in list */
    rd_dq	*nrq;		/* Next candidate for merge */
    rd_arc 	*ra;		/* Arc corresponding to rq */
    rd_arc	*nra;		/* Arc corresponding to nrq */
    
    rq = (rd_dq *) alist->rq_forw;
    while (rq->rq_self != alist) {
	ra = (rd_arc *) rq->rq_self;
	nrq = rq->rq_forw;
	while (nrq->rq_self != alist) {
	    nra = (rd_arc *) nrq->rq_self;
	    if ((nra->ra_low > (ra->ra_high + 1)) || 
			(nra->ra_to != ra->ra_to)) {
		break;
	    }
	    ra->ra_high = nra->ra_high;
	    RDQ_UNLINK(&(nra->ra_arcs));
	    RD_FREE_ARC(nra);
	    nrq = rq->rq_forw;
	}
	rq = rq->rq_forw;
    }
}
		
/*
 * rd_complete_arcs points missing token ranges at the specified
 * reject state. This code is like rd_complement_ranges,, except
 * that it does not delete arcs.
 */

static int
rd_complete_arcs(rd_dq *alist,		/* List of arcs to fill up */
		 rd_state *reject)	/* Reject state for arc */
{
    rd_dq	*rq;		/* Temporary linked list elt pointer */
    rd_arc	*ra;		/* Current arc corresponding to dq */
    rd_arc	*nra;		/* New arc to be inserted */
    int		ret;		/* No. of new arcs with rej. state as target */

    ret = 0;
    RD_ALLOC_RANGE(nra);
    nra->ra_low = 0;

    rq = alist->rq_forw;
    while (rq->rq_self != alist->rq_self) {
	ra = (rd_arc *) (rq->rq_self);
	if (ra->ra_low > nra->ra_low) {
	    nra->ra_high = ra->ra_low - 1;
	    nra->ra_to = reject; ret++;	   
	    RDQ_INSERT_BEFORE(rq, &(nra->ra_arcs));
	    RD_ALLOC_ARC(nra, 0, 0);
	}
	rq = rq->rq_forw;
	nra->ra_low = ra->ra_high + 1;
    }

    if (nra->ra_low <= MAX_AS) {
	nra->ra_high = MAX_AS;
	nra->ra_to = reject; ret++;
	RDQ_INSERT_BEFORE(alist, &(nra->ra_arcs));
    } else {
	RD_FREE_RANGE(nra);
    }

    return ret;
}

/*
 * rd_ntod takes a non-deterministic FA and converts it into the
 * corresponding deterministic FA. It releases the resources allocated
 * to the NFA and returns the DFA.
 */

rd_fm *
rd_ntod(rd_fm* nfa)
{
    rd_fm	*dfa;		/* The DFA under construction */
    rd_dq	unp;		/* Unprocessed states of the DFA */
    rd_state	*rs;		/* Current state in list */
    rd_state	*nrs;		/* Newly allocated state, or inner loop state */
    rd_state	*rej;		/* Reject state: all uncov trans go here */
    rd_arc	*ra;		/* Current arc in list */
    int		i;		/* Current bit number in loop */
    int		rjc;		/* Count of arcs to reject state */

    /*
     * Initialize the unprocessed state list. Set up the start
     * state of the DFA as initially unprocessed. Initialize the DFA.
     * Allocate a reject state: we will fill it up later if necessary.
     */

    RD_ALLOC_FM(dfa);
    RDQ_INIT(&unp, &unp);
    rs = nfa->rf_start;
    RD_ALLOC_DFA_STATE(nrs);
    RD_SET_BIT(nrs->rs_mask, rs->rs_bit);
    RDQ_INSERT_BEFORE(&unp, &(nrs->rs_states));
    dfa->rf_start = nrs;

    RD_ALLOC_DFA_STATE(rej); 
    rjc = 0;

    while (!RDQ_ISEMPTY(&unp)) {

	/*
	 * Remove first unprocessed state in list. Move it over
	 * to the list of DFA states.
	 */

	rs = (rd_state *) unp.rq_forw->rq_self;
	RDQ_UNLINK(&(rs->rs_states));
	RDQ_INSERT_BEFORE(&(dfa->rf_states), &(rs->rs_states));
	
	/*
	 * For each NFA states in the subset of NFA states to which
	 * this DFA state is equivalent,
	 *	- if the NFA state is a start state, the DFA state is too.
	 *	- if the NFA state is a final state, the DFA state is too.
	 *	- merge all the arcs of the NFA state into the DFA state's arcs.
	 */

	for (i = 0; i < RD_MAXASPSTATES; i++) {
	    if (RD_ISSET_BIT(rs->rs_mask, i)) {
		nrs = RD_BIT_STATE(i);
		if (RD_IS_FINAL(nrs) && !RD_IS_FINAL(rs)) {
		    RDQ_INSERT_BEFORE(&(dfa->rf_final), &(rs->rs_final));
		}
		RDQ_LIST_START(&(nrs->rs_arcs), nrs, ra, rd_arc) {
		    rd_insert_arc(&(rs->rs_arcs), ra);
		} RDQ_LIST_END(&(nrs->rs_arcs), nrs, ra, rd_arc);
	    }
	}

	/*
	 * For each arc of the DFA state, if the arc's mask corresponds
	 * to either a DFA state or an unprocessed state, set the arc's
	 * to pointer to that state. Otherwise, create a new unprocessed
	 * state and insert it into the list of unprocessed states.
	 */

	RDQ_LIST_START(&(rs->rs_arcs), rs, ra, rd_arc) {
	    RDQ_LIST_START(&(dfa->rf_states), dfa, nrs, rd_state) {
		if (RD_EQUAL_BITS(nrs->rs_mask, ra->ra_mask)) {
		    goto found;
		}
	    } RDQ_LIST_END(&(dfa->rf_states), dfa, nrs, rd_state);
	    RDQ_LIST_START(&unp, &unp, nrs, rd_state) {
		if (RD_EQUAL_BITS(nrs->rs_mask, ra->ra_mask)) {
		    goto found;
		}
	    } RDQ_LIST_END(&unp, &unp, nrs, rd_state);

	    /* New state */
	    RD_ALLOC_DFA_STATE(nrs);
	    RDQ_INSERT_AFTER(&unp, &(nrs->rs_states));
	    RD_OR_BITS(nrs->rs_mask, ra->ra_mask);
	  found:
	    ra->ra_to = nrs;
	} RDQ_LIST_END(&(rs->rs_arcs), rs, ra, rd_arc);

	/*
	 * The above process can result in adjacent arc ranges with
	 * the same target. Merge those. Also, point any uncovered
	 * AS path ranges at the reject state (we need this for
	 * minimization).
	 */
	rd_merge_arcs(&(rs->rs_arcs));
	rjc += rd_complete_arcs(&(rs->rs_arcs), rej);
    }

    /*
     * Add the reject state in. Free the NFA. If any arcs to reject
     * state have been added, add the reject state, else free it up.
     */

    if (rjc) {
	RD_ALLOC_ARC(ra, 0, 0);
	ra->ra_low = 0;
	ra->ra_high = MAX_AS;
	ra->ra_to = rej;
	rej->rs_flags |= RSF_REJECT;
	RDQ_INSERT_BEFORE(&(rej->rs_arcs), &(ra->ra_arcs));
	RDQ_INSERT_BEFORE(&(dfa->rf_states), &(rej->rs_states));
    } else {
	RD_FREE_STATE(rej);
    }

    rd_free_nfa(nfa);
    return dfa;
}



/*
 * rd_complete_dfa completes all the arcs in a DFA. This is needed
 * where the traditional ntod->minimize route is not taken.
 */

void
rd_complete_dfa(rd_fm *dfa)
{
    rd_state	*rs;		/* Current state in loop */
    rd_arc	*ra;		/* Reject state arc */
    rd_state	*rej;		/* Reject state */
    int		rjc;		/* Count of arcs pointing to rej */

    RD_ALLOC_DFA_STATE(rej);
    rjc = 0;

    RDQ_LIST_START(&(dfa->rf_states), dfa, rs, rd_state) {
	rjc += rd_complete_arcs(&(rs->rs_arcs), rej);
    } RDQ_LIST_END(&(dfa->rf_states), dfa, rs, rd_state);

    if (rjc) {
	RD_ALLOC_ARC(ra, 0, 0);
	ra->ra_low = 0;
	ra->ra_high = MAX_AS;
	ra->ra_to = rej;
	rej->rs_flags |= RSF_REJECT;
	RDQ_INSERT_BEFORE(&(rej->rs_arcs), &(ra->ra_arcs));
	RDQ_INSERT_BEFORE(&(dfa->rf_states), &(rej->rs_states));
    } else {
	RD_FREE_STATE(rej);
    }

    return;
}



/*
 * Myhill-Nerode minimization. See Hopcroft and Ullmann for a
 * detailed description of this algorithm.
 */

/*
 * The minimization algorithm maintains a 2-dimensional array of
 * state pairs. Element (i,j) of the array is set if states i and
 * j are distinguishable (that is, they accept different languages).
 * The notsame array below contains this information.
 */

/* File local */
static rd_notsame *rd_ns = NULL;	/* Array of distinguishable entries */
static int rd_nsdim = 0;		/* Dimension of above array */

#define RD_ALLOC_NOTSAME(n) {						\
    rd_ns = (rd_notsame *) malloc((n) * (n) * sizeof (rd_notsame));	\
    bzero((caddr_t) rd_ns, (n) * (n) * sizeof(rd_notsame));		\
    rd_nsdim = (n);							\
}

#define RD_FREE_NOTSAME() {						\
    free(rd_ns); 							\
    rd_ns = NULL; rd_nsdim = 0;						\
}

#define RD_NOTSAME(i, j)						\
    (&rd_ns[(i)*rd_nsdim+(j)])

#define RD_ALLOC_PAIR(p) {						\
    (p) = (rd_pair *) malloc(sizeof (rd_pair));				\
    bzero((caddr_t) (p), sizeof (rd_pair));				\
}

#define RD_FREE_PAIR(p)	{ 						\
    free((p)); 								\
}

/*
 * Range predicates that are useful in rd_distinguishable and
 * rd_put_pair.
 */

#define RD_OVERLAP(r1, r2)						\
  ((((r1)->ra_high >= (r2)->ra_low) && ((r1)->ra_high <= (r2)->ra_high)) \
|| (((r2)->ra_high >= (r1)->ra_low) && ((r2)->ra_high <= (r1)->ra_high)))

#define RD_LEFT_OF(r1, r2)						\
   ((r1)->ra_high < (r2)->ra_high)

/* The usual */
#define MIN(a, b)	((a < b) ? a : b)
#define MAX(a, b)	((a > b) ? a : b)

/*
 * rd_distinguishable traverses the arc lists of the two states
 * and, for any common symbols, checks if the pair of target states
 * is distinguishable. Return TRUE if so, else FALSE.
 * When we are called: rs1->rs_bit < rs2->rs_bit.
 */

int
rd_distinguishable(rd_state *rs1,
		   rd_state *rs2)
{
    rd_arc	*ra1;		/* Current arc in rs1 */
    rd_arc	*ra2;		/* Current arc in rs2 */
    rd_dq	*rq1;		/* Current list element in rs1 */
    rd_dq	*rq2;		/* Current list element in rs2 */

    rq1 = rs1->rs_arcs.rq_forw;
    rq2 = rs2->rs_arcs.rq_forw;

    while ((rq1->rq_self != rs1) && (rq2->rq_self != rs2)) {
	ra1 = (rd_arc *) rq1->rq_self;
	ra2 = (rd_arc *) rq2->rq_self;

	if (RD_OVERLAP(ra1, ra2) &&
	    RD_NOTSAME(MIN(ra1->ra_to->rs_bit, ra2->ra_to->rs_bit),
		   MAX(ra1->ra_to->rs_bit, ra2->ra_to->rs_bit))->rt_mark) {
	    return 1;
	}

	if (RD_LEFT_OF(ra1, ra2)) {
	    rq1 = rq1->rq_forw;
	} else {
	    rq2 = rq2->rq_forw;
	}
    }
    return 0;
}

/*
 * rd_recursively mark marks the specified pair's notsame location
 * and then recursively every location in the list for the pair.
 * We also delete the rd_pair list associated with pair's notsame
 * location. When we are called: i < j.
 */

static void
rd_recursively_mark(int i,
		    int j)
{
    rd_pair	*rp;		/* Current pair under consideration */
    rd_pair	*nrp;		/* Next pair */

    if (RD_NOTSAME(i,j)->rt_mark)
       return;

    RD_NOTSAME(i,j)->rt_mark = 1;
    rp = RD_NOTSAME(i,j)->rt_pair;
    while (rp) {
	rd_recursively_mark(rp->rp_first->rs_bit, rp->rp_second->rs_bit);
	nrp = rp->rp_next;
	RD_FREE_PAIR(rp);
	rp = nrp;
    }
    RD_NOTSAME(i,j)->rt_pair = NULL;
}

/*
 * rd_put_pair adds the specified pair of states to the list
 * of all pairs of meaningful state transitions.
 * When we are called: rs1->rs_bit < rs2->rs_bit.
 */

void
rd_put_pair(rd_state *rs1,
	    rd_state *rs2)
{
    rd_arc	*ra1;		/* Current arc in rs1 */
    rd_arc	*ra2;		/* Current arc in rs2 */
    rd_dq	*rq1;		/* Current list element in rs1 */
    rd_dq	*rq2;		/* Current list element in rs2 */
    rd_pair	*rp;		/* Pair element to be added to list */
    rd_notsame	*rt;		/* Temporary storage for notsame element */

    rq1 = rs1->rs_arcs.rq_forw;
    rq2 = rs2->rs_arcs.rq_forw;

    while ((rq1->rq_self != rs1) && (rq2->rq_self != rs2)) {
	ra1 = (rd_arc *) rq1->rq_self;
	ra2 = (rd_arc *) rq2->rq_self;

	if (RD_OVERLAP(ra1, ra2)) {
	    int n1, n2;

	    n1 = MIN(ra1->ra_to->rs_bit, ra2->ra_to->rs_bit);
	    n2 = MAX(ra1->ra_to->rs_bit, ra2->ra_to->rs_bit);
	    if ((n1 != n2) && ((n1 != rs1->rs_bit) || (n2 != rs2->rs_bit))) { 
		RD_ALLOC_PAIR(rp);
		rp->rp_first = rs1;
		rp->rp_second = rs2;
		rt = RD_NOTSAME(n1, n2);
		rp->rp_next = rt->rt_pair;
		rt->rt_pair = rp;
	    }
	}

	if (RD_LEFT_OF(ra1, ra2)) {
	    rq1 = rq1->rq_forw;
	} else {
	    rq2 = rq2->rq_forw;
	}
    }
    return;
}

/*
 * rd_adjust_arcs modifies the arcs of the specified dfa such
 * that those that end on "rs1" are deflected to end on "rs2".
 */

static void
rd_adjust_arcs(rd_fm *dfa,
	       rd_state *rs1,
	       rd_state *rs2)
{
    rd_arc	*ra;		/* Current arc under consideration */
    rd_state	*rs;		/* Current state in loop */

    RDQ_LIST_START(&(dfa->rf_states), dfa, rs, rd_state) {
	RDQ_LIST_START(&(rs->rs_arcs), rs, ra, rd_arc) {
	    if (ra->ra_to == rs1) {
		ra->ra_to = rs2;
	    }
	} RDQ_LIST_END(&(rs->rs_arcs), rs, ra, rd_arc);
    } RDQ_LIST_END(&(dfa->rf_states), dfa, rs, rd_state);
}

/*
 * rd_minimize, given a DFA, computes which states of the DFA are
 * distinguishable, and deletes one of every indistinguishable pair.
 * The resulting DFA is known to be minimal.
 */

void
rd_minimize(rd_fm *dfa)
{
    rd_dq	*rq;		/* Current queue element */
    rd_state	*rs;		/* Current state in loop */
    rd_state	*nrs;		/* Current state in inner loop */
    rd_arc	*ra;		/* Current arc in index */
    rd_pair	*rp;		/* Current pair in loop */
    int		i;		/* State bit number */
    
    /*
     * First number the states, allocate the notsame array and mark
     * (final state, non-final state) pairs as distinguishable.
     */

    i = 0;
    RDQ_LIST_START(&(dfa->rf_states), dfa, rs, rd_state) {
	rs->rs_bit = i++;
	rs->rs_flags &= ~RSF_DELETE;
    } RDQ_LIST_END(&(dfa->rf_states), dfa, rs, rd_state);
    RD_ALLOC_NOTSAME(i); 

    RDQ_LIST_START(&(dfa->rf_final), dfa, rs, rd_state) {
	RDQ_LIST_START(&(dfa->rf_states), dfa, nrs, rd_state) {
	    if (RD_IS_FINAL(nrs)) {
		continue;
	    }
	    RD_NOTSAME(MIN(rs->rs_bit, nrs->rs_bit),
		   MAX(rs->rs_bit, nrs->rs_bit))->rt_mark = 1;
	} RDQ_LIST_END(&(dfa->rf_states), dfa, nrs, rd_state);
    } RDQ_LIST_END(&(dfa->rf_states), dfa, nrs, rd_state);

    /*
     * For every pair of different, not yet distinguishable states,
     * check if they can reach a pair of distinguishable states.
     * If the pair is distinguishable, mark it and all other pairs
     * that are waiting on it: otherwise, put this pair on all
     * pairs of outgoing transitions.
     */

    RDQ_LIST_START(&(dfa->rf_states), dfa, rs, rd_state) {
	RDQ_LIST_START(&(dfa->rf_states), dfa, nrs, rd_state) {
	    if (nrs->rs_bit <= rs->rs_bit) {
		continue;	/* We traverse each pair once */
	    }
	    if (RD_NOTSAME(rs->rs_bit, nrs->rs_bit)->rt_mark) {
		continue;
	    }
	    if (rd_distinguishable(rs, nrs)) {
		rd_recursively_mark(rs->rs_bit, nrs->rs_bit);
	    } else {
		rd_put_pair(rs, nrs);
	    }
	} RDQ_LIST_END(&(dfa->rf_states), dfa, nrs, rd_state);
    } RDQ_LIST_END(&(dfa->rf_state), dfa, rs, rd_state);

    /*
     * Finally, walk through all state pairs and, if two states
     * are indistinguishable, delete the higher numbered one:
     *	- ensure that all arcs that end on the higher numbered one
     *	now end on the lower numbered one.
     *  - free the pair list of state pairs waiting on this pair
     * 	to become distinguishable.
     *  - mark the state for deletion.
     */

    RDQ_LIST_START(&(dfa->rf_states), dfa, rs, rd_state) {
	RDQ_LIST_START(&(dfa->rf_states), dfa, nrs, rd_state) {
	    if (nrs->rs_bit <= rs->rs_bit) {
		continue;
	    }
	    if (RD_NOTSAME(rs->rs_bit, nrs->rs_bit)->rt_mark) {
		continue;
	    }
	    rp = RD_NOTSAME(rs->rs_bit, nrs->rs_bit)->rt_pair;
	    while (rp) {
		rd_pair *nrp = rp->rp_next;
		RD_FREE_PAIR(rp);
		rp = nrp;
	    }
	    rd_adjust_arcs(dfa, nrs, rs);
	    nrs->rs_flags |= RSF_DELETE;
	} RDQ_LIST_END(&(dfa->rf_states), dfa, nrs, rd_state);
    } RDQ_LIST_END(&(dfa->rf_state), dfa, rs, rd_state);

    /*
     * Run through all the states deleting all states (and their
     * corresponding arcs) that have been marked above.
     */
    rq = dfa->rf_states.rq_forw;
    while (rq->rq_self != dfa) {
	rs = (rd_state *) rq->rq_self;
	rq = rq->rq_forw;
	if (!(rs->rs_flags & RSF_DELETE)) {
	    continue;
	}
	while (RDQ_LIST_HEAD(&(rs->rs_arcs), ra, rd_arc) != NULL) {
	    RDQ_UNLINK(&(ra->ra_arcs));
	    RD_FREE_ARC(ra);
	}
	RDQ_UNLINK(&(rs->rs_states));
	RDQ_UNLINK(&(rs->rs_final));
	RD_FREE_DFA_STATE(rs);
    }

    /*
     * Here too, we may have a number of adjacent arc ranges
     * with the same target. We try and merge these arc ranges.
     */

    RDQ_LIST_START(&(dfa->rf_states), dfa, rs, rd_state) {
	rd_merge_arcs(&(rs->rs_arcs));
    } RDQ_LIST_END(&(dfa->rf_states), dfa, rs, rd_state);

    // Added by wlee@isi.edu according to Ramesh's suggestion
    RD_FREE_NOTSAME();

    return; 	/* Phew! */
}



/*
 * Routines to print out various forms of the regular expression.
 */

/*
 * rd_print_nfa, given a pointer to an NFA, pretty prints the transitions
 * of the NDFA.
 */

void
rd_print_nfa(rd_fm *fm)
{
    rd_state 	*rs;			/* Current state in loop */
    rd_arc	*ra;			/* Current arc in loop */
    int		i;			/* Current loop index */

    /*
     * Print out the start and final states first, followed by the
     * arcs.
     */

    printf("NFA:\n");
    printf("\tSTART: %d", fm->rf_start->rs_bit);

    printf("\n\tFINAL:");
    RDQ_LIST_START(&(fm->rf_final), fm, rs, rd_state) {
	printf(" %d", rs->rs_bit);
    } RDQ_LIST_END(&(fm->rf_final), fm, rs, rd_state);

    printf("\n\tARCS:\n");
    RDQ_LIST_START(&(fm->rf_states), fm, rs, rd_state) {
	printf("\t\tFrom: %d\n", rs->rs_bit);
	if (RDQ_ISEMPTY(&(rs->rs_arcs))) {
	    printf("\t\t\tNo outgoing arcs\n");
	    continue;
	}
	RDQ_LIST_START(&(rs->rs_arcs), rs, ra, rd_arc) {
	    printf("\t\t\tLow: %d\tHigh: %d\tTo:", ra->ra_low, ra->ra_high);
	    for (i = 0; i < RD_MAXASPSTATES; i++) {
		if (RD_ISSET_BIT(ra->ra_mask, i)) {
		    printf(" %d", i);
		}
	    }
	    printf("\n");
	} RDQ_LIST_END(&(rs->rs_arcs), rs, ra, rd_arc);
    } RDQ_LIST_END(&(fm->rf_states), fm, rs, rd_state);

    return;
}

/*
 * rd_print_dfa: given a pointer to a DFA, pretty print the DFA.
 * Largely similar to rd_print_nfa.
 */

void
rd_print_dfa(rd_fm *fm)
{
    rd_state	*rs;			/* Current state in loop */
    rd_arc	*ra;			/* Current arc in loop */
    int 	i;			/* Bit index */

    /*
     * For convenience, first number the states from 0..n.
     * Then print out states as above.
     */

    i = 0;
    RDQ_LIST_START(&(fm->rf_states), fm, rs, rd_state) {
	rs->rs_bit = i++;
    } RDQ_LIST_END(&(fm->rf_states), fm, rs, rd_state);

    printf("DFA:\n");
    printf("\tSTART: %d", fm->rf_start->rs_bit);

    printf("\n\tFINAL:");
    RDQ_LIST_START(&(fm->rf_final), fm, rs, rd_state) {
	printf(" %d", rs->rs_bit);
    } RDQ_LIST_END(&(fm->rf_final), fm, rs, rd_state);

    printf("\n\tARCS:\n");
    RDQ_LIST_START(&(fm->rf_states), fm, rs, rd_state) {
	printf("\t\tFrom: %d\n", rs->rs_bit);
	if (RDQ_ISEMPTY(&(rs->rs_arcs))) {
	    printf("\t\t\tNo outgoing arcs\n");
	    continue;
	}
	RDQ_LIST_START(&(rs->rs_arcs), rs, ra, rd_arc) {
	    printf("\t\t\tLow: %d\tHigh: %d\tTo: %d\n", 
		   ra->ra_low, ra->ra_high, ra->ra_to->rs_bit);
	} RDQ_LIST_END(&(rs->rs_arcs), rs, ra, rd_arc);
    } RDQ_LIST_END(&(fm->rf_states), fm, rs, rd_state);

    return;
}



/*
 * Cengiz Alaettinoglu's additions
 */

/*
 * rd_empty_string_machine creates an FA containing one
 * state which is both a start state and a final state. 
 * This FA accepts the empty string.
 */

rd_fm *rd_empty_string_machine() {
    register rd_fm *rf;			/* The singleton machine */
    register rd_state *rs;		/* A state of the singleton machine */
    register rd_range *ra;		/* Current element of range list */

    RD_ALLOC_FM(rf);
    
    /* The only state */
    RD_ALLOC_STATE(rs);
    RDQ_INSERT_BEFORE(&(rf->rf_states), &(rs->rs_states));
    RDQ_INSERT_BEFORE(&(rf->rf_final), &(rs->rs_final));
    rf->rf_start = rs;

#ifdef SANITY
    rd_check_invariant(rf);
#endif /* SANITY */

    return rf;
}

rd_fm *rd_empty_set_machine() {
    register rd_fm *rf;			/* The singleton machine */
    register rd_state *rs;		/* A state of the singleton machine */
    register rd_range *ra;		/* Current element of range list */

    RD_ALLOC_FM(rf);
    
    /* The only state */
    RD_ALLOC_STATE(rs);
    RDQ_INSERT_BEFORE(&(rf->rf_states), &(rs->rs_states));
    rf->rf_start = rs;

#ifdef SANITY
    rd_check_invariant(rf);
#endif /* SANITY */

    return rf;
}

rd_fm *rd_empty_set_dfa() {
   register rd_fm *m;

   rd_init();
   m = rd_empty_set_machine();
   m = rd_ntod(m); 
   rd_minimize(m);

   return m;
}

/*
 * rd_isEmpty_dfa returns true if fm recognizes the empty language
 */

int rd_isEmpty_dfa(rd_fm *fm) {
   return RDQ_ISEMPTY(&(fm->rf_final));
}

/*
 * rd_is_universal_dfa returns true if fm recognizes the universal language
 */

int rd_is_universal_dfa(rd_fm *fm) {
   /* all states are final and no reject state iff a universal machine,
    * this holds because we have a reject state if it is not universal
    */
   register rd_state *stt;

   RDQ_LIST_START(&(fm->rf_states), fm, stt, rd_state) {
      if (!RD_IS_FINAL(stt))
	 return 0;
   } RDQ_LIST_END(&(fm->states), fm, stt, rd_state);

   return 1;
}

/*
 * duplicate the argument dfa and return it
 */
rd_fm *rd_duplicate_dfa(rd_fm *fm) {
   register rd_fm *rf;			/* The singleton machine */
   register rd_state *stt, *rs;		/* A state of the singleton machine */
   register rd_arc *arc, *new_arc;

   RD_ALLOC_FM(rf);
    
   /* duplicate the states */
   RDQ_LIST_START(&(fm->rf_states), fm, stt, rd_state) {
      RD_ALLOC_STATE(rs);

      RDQ_INSERT_BEFORE(&(rf->rf_states), &(rs->rs_states));

      if (RD_IS_FINAL(stt))
	 RDQ_INSERT_BEFORE(&(rf->rf_final), &(rs->rs_final));

      bcopy(&(stt->rs_rsu), &(rs->rs_rsu), sizeof(stt->rs_rsu));

      stt->new_address = rs;
   } RDQ_LIST_END(&(fm->states), fm, stt, rd_state);

   rf->rf_start = fm->rf_start->new_address;

   /* now copy the arcs */
   RDQ_LIST_START(&(fm->rf_states), fm, stt, rd_state) {
      RDQ_LIST_START(&(stt->rs_arcs), stt, arc, rd_arc) {

	 RD_ALLOC_ARC(new_arc, arc->ra_low, arc->ra_high);
	 new_arc->ra_to = arc->ra_to->new_address;

	 RDQ_INSERT_BEFORE(&(stt->new_address->rs_arcs), &(new_arc->ra_arcs));

      } RDQ_LIST_END(&(stt->rs_arcs), stt, arc, rd_arc);
   } RDQ_LIST_END(&(fm->states), fm, stt, rd_state);


   rf->bolexp = fm->bolexp;
   rf->eolexp = fm->eolexp;

   return rf;
}

/*
 * rd_complement_dfa makes fm recognize the negation of the language
 */

void rd_complement_dfa(rd_fm *fm) {
   register rd_state *stt;		/* A state of the singleton machine */

   /* make final states non-final and non-finals final,
    * reject state also become final
    */

   RDQ_LIST_START(&(fm->rf_final), fm, stt, rd_state) {
      stt->rs_flags |= RSF_FINAL;
   } RDQ_LIST_END(&(fm->rf_final), fm, stt, rd_state);
   
   RDQ_SET_EMPTY(&fm->rf_final);

   RDQ_LIST_START(&(fm->rf_states), fm, stt, rd_state) {

      if (stt->rs_flags & RSF_FINAL) {
	 RDQ_SET_EMPTY(&stt->rs_final);
	 stt->rs_flags &= ~RSF_FINAL;
      } else {
	 stt->rs_flags |= RSF_FINAL;
	 stt->rs_flags &= ~RSF_REJECT;
	 RDQ_INSERT_BEFORE(&(fm->rf_final), &(stt->rs_final));
      }

   } RDQ_LIST_END(&(fm->states), fm, stt, rd_state);

}

/*
 * rd_intersect_dfa takes two dfa's and returns a dfa which accepts 
 * the intersection of the two.
 */

typedef struct _rd_triple {
   struct _rd_state	*first;	        /* Lower numbered state in pair */
   struct _rd_state	*second;	/* Higher numbered state in pair */
   struct _rd_state	*third;	        /* maps the first and the second one 
					   into this state */
   rd_dq                dll;
} rd_triple;

rd_triple rd_intersect_map;

#define RD_ALLOC_TRIPLE(s) {					\
    (s) = (rd_triple *) malloc(sizeof (rd_triple));		\
    bzero((caddr_t) (s), sizeof (rd_triple));			\
    RDQ_INIT(&((s)->dll), (s));			   	        \
}

rd_state *rd_intersect_map_find(register rd_state *stt1, 
				register rd_state *stt2, 
				register rd_fm *fm1, 
				register rd_fm *fm2,
				register rd_fm *fm3) {
   register rd_state *stt3;	
   register rd_triple *triple, *new_triple;

   /* search for a "(stt1, stt2) -> stt3" mapping in the rd_intersect_map */
   for (triple = rd_intersect_map.dll.rq_forw->rq_self; 
	triple != &rd_intersect_map
	   && (triple->first < stt1 
	       || triple->first == stt1 && triple->second < stt2); 
	triple = triple->dll.rq_forw->rq_self)
	   ;

   if (triple->first == stt1 && triple->second == stt2) /* found ? */
      return triple->third;

   /* Create a state stt3 that is mapped by pair (stt1, stt2) */
   RD_ALLOC_STATE(stt3);
   RDQ_INSERT_BEFORE(&(fm3->rf_states), &(stt3->rs_states));
   /* stt3 is final if both stt1 and stt2 are */
   if (RD_IS_FINAL(stt1) && RD_IS_FINAL(stt2))
      RDQ_INSERT_BEFORE(&(fm3->rf_final), &(stt3->rs_final));
   /* stt3 is a start state if both stt1 and stt2 are */
   if (fm1->rf_start == stt1 && fm2->rf_start == stt2)
      fm3->rf_start = stt3;

   /* add "(stt1, stt2) -> stt3" mapping to the rd_intersect_map */
   RD_ALLOC_TRIPLE(new_triple);
   new_triple->first  = stt1;
   new_triple->second = stt2;
   new_triple->third  = stt3;
   RDQ_INSERT_BEFORE(&(triple->dll), &(new_triple->dll));

   return stt3;
}

rd_fm *rd_intersect_dfa(rd_fm *fm1, rd_fm *fm2) {
   rd_fm *fm3;
   rd_state *stt1, *stt2, *stt3;
   rd_arc *arc1, *arc2, *arc3;

   RD_ALLOC_FM(fm3);

   RDQ_INIT(&(rd_intersect_map.dll), &rd_intersect_map);

   /* create the start state of fm3 */
   (void) rd_intersect_map_find(fm1->rf_start, fm2->rf_start, fm1, fm2, fm3);

   /* traverse the arcs of fm1 */
   RDQ_LIST_START(&(fm1->rf_states), fm1, stt1, rd_state) {
      RDQ_LIST_START(&(stt1->rs_arcs), stt1, arc1, rd_arc) {

	 /* traverse the arcs of fm2 */
	 RDQ_LIST_START(&(fm2->rf_states), fm2, stt2, rd_state) {
	    RDQ_LIST_START(&(stt2->rs_arcs), stt2, arc2, rd_arc) {

	       /* intersect arc1 and arc2 and add it to fm3 */
	       if (MAX(arc1->ra_low, arc2->ra_low) 
		   <= MIN(arc1->ra_high, arc2->ra_high)) {
		  RD_ALLOC_ARC(arc3, 
			       MAX(arc1->ra_low, arc2->ra_low), 
			       MIN(arc1->ra_high, arc2->ra_high));
		  arc3->ra_to = rd_intersect_map_find
		     (arc1->ra_to, arc2->ra_to, fm1, fm2, fm3);
		  stt3 = rd_intersect_map_find(stt1, stt2, fm1, fm2, fm3);
		  
		  RDQ_INSERT_BEFORE(&(stt3->rs_arcs), &(arc3->ra_arcs));
	       }

	    } RDQ_LIST_END(&(stt2->rs_arcs), stt2, arc2, rd_arc)
          } RDQ_LIST_END(&(fm2->rf_states), fm2, stt2, rd_state)

     } RDQ_LIST_END(&(stt1->rs_arcs), stt1, arc1, rd_arc)
  } RDQ_LIST_END(&(fm1->rf_states), fm1, stt1, rd_state)

  RDQ_UNLINK_LIST_FREE_ELMS(&(rd_intersect_map.dll), &rd_intersect_map);

 /*  rd_reachable_dfa(fm3); */
/*   rd_complete_dfa(fm3); */
  rd_init();
     rd_dton(fm3);
     fm3 = rd_ntod(fm3);
   rd_minimize(fm3);

   return fm3;
}

typedef struct _rd_single {
   struct _rd_state	*first;	        /* Lower numbered state in pair */

   rd_dq                dll;
} rd_single;

#define RD_ALLOC_SINGLE(s) {					\
    (s) = (rd_single *) malloc(sizeof (rd_single));		\
    bzero((caddr_t) (s), sizeof (rd_single));			\
    RDQ_INIT(&((s)->dll), (s));			   	        \
}

void rd_reachable_dfa(rd_fm *fm) {
   rd_state *stt, *stt2;
   rd_arc *arc;

   rd_single reachable;
   rd_single *rd_s, *rd_s2;

   RDQ_INIT(&(reachable.dll), &reachable);
   RD_ALLOC_SINGLE(rd_s);
   rd_s->first = fm->rf_start;
   /* mark start state */
   fm->rf_start->rs_flags |= RSF_MARKED;

   RDQ_INSERT_BEFORE(&(reachable.dll), &(rd_s->dll));

   for (rd_s = reachable.dll.rq_forw->rq_self; 
	rd_s != &reachable; 
	rd_s = rd_s->dll.rq_forw->rq_self) {
      stt = rd_s->first; /* stt is a reachable states */

      /* travers the arcs of stt, a reachable states
	 and add more reachable states if arcs go to unmarked states */
      RDQ_LIST_START(&(stt->rs_arcs), stt, arc, rd_arc) {
	 
	 if (! (arc->ra_to->rs_flags & RSF_MARKED)) {
	    arc->ra_to->rs_flags |= RSF_MARKED;
	    RD_ALLOC_SINGLE(rd_s2);
	    rd_s2->first = arc->ra_to;
	    RDQ_INSERT_BEFORE(&(reachable.dll), &(rd_s2->dll));
	 }

      } RDQ_LIST_END(&(stt->rs_arcs), stt, arc, rd_arc)
   }

   /* delete unreachable states, i.e. not marked */
   for (stt = fm->rf_states.rq_forw->rq_self; stt != (rd_state *) fm; )
      if (! (stt->rs_flags & RSF_MARKED)) {
	 stt2 = stt;
	 stt = stt->rs_states.rq_forw->rq_self;
	 RDQ_UNLINK(&(stt2->rs_states));
	 if (RD_IS_FINAL(stt2))
	    RDQ_UNLINK(&(stt2->rs_final));
	 RDQ_UNLINK_LIST_FREE_ELMS(&(stt2->rs_arcs), stt2);
	 RD_FREE_STATE(stt2);
      } else
	 stt = stt->rs_states.rq_forw->rq_self;

   RDQ_UNLINK_LIST_FREE_ELMS(&(reachable.dll), &reachable);

}

int rd_equal_dfa(rd_fm *fm1, rd_fm *fm2) {
   rd_fm *fm3, *fm4;
   int result;

   fm3 = rd_duplicate_dfa(fm2);
   rd_complement_dfa(fm3);
   fm4 = rd_intersect_dfa(fm3, fm1);
   result = rd_isEmpty_dfa(fm4);
   rd_free_nfa(fm3);
   rd_free_nfa(fm4);

   if (!result)
      return 0;

   fm3 = rd_duplicate_dfa(fm1);
   rd_complement_dfa(fm3);
   fm4 = rd_intersect_dfa(fm3, fm2);
   result = rd_isEmpty_dfa(fm4);
   rd_free_nfa(fm3);
   rd_free_nfa(fm4);

   return result;   
}

rd_fm * rd_make_bol(rd_fm *fm) {
   rd_fm *fm2;

   if (fm->bolexp) /* already so */
      return fm;

   fm2 = rd_zero_or_more(
	 rd_singleton(rd_alloc_range_list(rd_alloc_range(0, MAX_AS))));
   fm = rd_concatenate(fm2, fm);

   fm->bolexp = 1;

   return fm;
}

rd_fm *rd_make_eol(rd_fm *fm) {
   rd_fm *fm2;
   
   if (fm->eolexp) /* already so */
      return fm;

   fm2 = rd_zero_or_more(
	 rd_singleton(rd_alloc_range_list(rd_alloc_range(0, MAX_AS))));
   fm = rd_concatenate(fm, fm2);

   fm->eolexp = 1;

   return fm;
}

/*  */

/* 
 * Copyright (c) 1994 by the University of Southern California
 * and/or the International Business Machines Corporation.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation in source and binary forms for lawful
 * non-commercial purposes and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both
 * the copyright notice and this permission notice appear in supporting
 * documentation, and that any documentation, advertising materials,
 * and other materials related to such distribution and use acknowledge
 * that the software was developed by the University of Southern
 * California, Information Sciences Institute and/or the International
 * Business Machines Corporation.  The name of the USC or IBM may not
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * NEITHER THE UNIVERSITY OF SOUTHERN CALIFORNIA NOR INTERNATIONAL
 * BUSINESS MACHINES CORPORATION MAKES ANY REPRESENTATIONS ABOUT
 * THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  THIS SOFTWARE IS
 * PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, TITLE, AND 
 * NON-INFRINGEMENT.
 *
 * IN NO EVENT SHALL USC, IBM, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT,
 * TORT, OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH,
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Questions concerning this software should be directed to 
 * ratoolset@isi.edu.
 *
 */



