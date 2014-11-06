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
 * All rights reserved. 
//    Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//    in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in
//    all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//    THE SOFTWARE.
 */

#ifndef RE2DFA
#define RE2DFA

#include "config.h"

#define MAX_AS ~(unsigned int)0-1

/*
 * The following is a compile time limit on the number of NFA states
 * in an AS path regular expression. We use the fact that most expressions
 * of interest compile to an NFA with a small number of states. We assign
 * each NFA state a bit in a bit mask; then we can represent the target
 * states of an NFA transition as a bitmap.
 */

/* Change this in the compile options file */
#ifndef RD_MAXASPSTATES
//#define RD_MAXASPSTATES 64		/* Multiples of bytes */
#define RD_MAXASPSTATES 8000		/* Multiples of bytes */
#endif /* RD_MAXASPSTATES */

#define RD_NBBY 8		/* Number of bits per byte */
#define RD_ASPSIZE	(RD_MAXASPSTATES/RD_NBBY)
#define STATES_MASK(x)	unsigned char (x)[RD_ASPSIZE]

/*
 * Data structures for converting from regular expressions to DFA.
 * We use doubly linked lists for the list of states, arcs and final
 * states, since we frequently perform element insertion/deletion
 * and list splicing.
 */

/*
 * Doubly linked list data structure for all lists. 
 */

typedef struct _rd_dq {
    struct _rd_dq	*rq_forw;	/* Pointer to forw element */
    struct _rd_dq	*rq_back;	/* Pointer to back element */
    void		*rq_self;	/* Pointer to encaps data struct */
} rd_dq;

/*
 * The basic finite machine data structure. It contains a singly
 * linked list of states, and a linked list of start and final states.
 * This structure is used to represent both the NFA and the DFA.
 */

typedef struct _rd_fm {
    struct _rd_state 	*rf_start;	/* Start state of FM */
    rd_dq 		rf_states;	/* States of the FM */
    rd_dq		rf_final;	/* NFA's final states - unused in DFA */

   /* Cengiz added these fields to support ^AS1 and AS1$ like expressions */
    char                bolexp;         /* if set m is starts with ^ */
    char                eolexp;         /* if set m is ends in $ */
} rd_fm;

/*
 * Each state itself contains a linked list of the arcs that
 * emanate from it. It also contains linked list anchor structs.
 *
 * Notes: 
 *	The rs_bit field is used in three different ways:
 *		- for an NFA state, it denotes the assigned bit number.
 *		- for a un-minimized DFA, it is used to number states to
 *			implement Myhill-Nerode
 *		- used for printing DFA.
 *	The rs_flags field is used in two different ways:
 *		- during Myhill-Nerode, it marks delete-able states.
 *		- in the final DFA, it marks final and reject states.
 */

typedef struct _rd_state {
    rd_dq		rs_arcs;	/* Arcs out of this state */
    rd_dq		rs_states;	/* Queue block for states */
    rd_dq		rs_final;	/* Queue block for final states */
    union {
	struct {
	    unsigned short rsus_bit;	/* See Notes above */
	    unsigned short rsus_flags;	/* See Notes above */
	} rsu_s;
	STATES_MASK(rsu_mask);		/* If DFA state, which NFA state set */
    } rs_rsu;

#define rs_bit rs_rsu.rsu_s.rsus_bit
#define rs_flags rs_rsu.rsu_s.rsus_flags
#define rs_mask rs_rsu.rsu_mask
   
   /* Cengiz added this field to make rd_duplicate_dfa more efficient */
   struct _rd_state *new_address;
} rd_state;

/*
 * Possible flags for state.
 */

#define RSF_DELETE	0x1		/* This state can be deleted */
#define RSF_FINAL	0x2		/* Final state */
#define RSF_REJECT	0x4		/* Reject state */
#define RSF_MARKED	0x8		/* marked as reachable */

/*
 * Each arc contains the AS number range for which it is to be
 * traversed and a pointer to the state at which the arc terminates.
 *
 * Note: ra_low and ra_high MUST be unsigned int (we use this fact in
 * rd_complete_ranges and rd_complete_arcs).
 */

typedef struct _rd_arc {
    unsigned int	ra_low;		/* Low end of range */
    unsigned int	ra_high;	/* High end of range */
    rd_dq		ra_arcs;	/* Queue block for arcs */
    union {
	struct _rd_state	*rau_to;	/* target state of DFA arc */
	STATES_MASK(rau_mask);			/* target states of NFA arc */
    } ra_rau;

#define ra_to 		ra_rau.rau_to
#define ra_mask 	ra_rau.rau_mask
} rd_arc;

/*
 * Data structures used for Myhill-Nerode minimization.
 */

/*
 * This is a single element of the 2-dimensional array. This contains
 * a bit to indicate if the corresponding pair of states is distinguishable
 * (in the Myhill-Nerode sense) and a pointer to a list of state pairs
 * whose fate is tied to this pair of states.
 */

typedef struct _rd_notsame {
    int			rt_mark;	/* 1 if pair is distinguishable */
    struct _rd_pair	*rt_pair;	/* List of state pairs */
} rd_notsame;

/*
 * This struct contains pairs of states in the rt_pair linked list.
 * During construction of this list, we maintain the invariant that
 * rp_first's bit number is always less than that of rp_second.
 */

typedef struct _rd_pair {
    struct _rd_state	*rp_first;	/* Lower numbered state in pair */
    struct _rd_state	*rp_second;	/* Higher numbered state in pair */
    struct _rd_pair	*rp_next;	/* Next in list of states */
} rd_pair;

/*
 * Externs.
 */

extern void rd_init();

extern rd_arc *rd_alloc_range(unsigned int, unsigned int);
extern rd_dq  *rd_alloc_range_list(rd_arc *);
extern rd_dq  *rd_alloc_range_list_empty();
extern void rd_complement_range_list(rd_dq *);
extern void rd_insert_range(rd_dq *, rd_arc *);

extern rd_fm *rd_singleton(rd_dq *);
extern rd_fm *rd_alternate(rd_fm *, rd_fm *);
extern rd_fm *rd_concatenate(rd_fm *, rd_fm *);
extern rd_fm *rd_zero_or_more(rd_fm *);
extern rd_fm *rd_one_or_more(rd_fm *);
extern rd_fm *rd_zero_or_one(rd_fm *);

extern void rd_minimize(rd_fm *);
extern rd_fm *rd_ntod(rd_fm *);

extern void rd_print_nfa(rd_fm *);
extern void rd_print_dfa(rd_fm *);

/*
 * Macros for doubly-linked list insertion, deletion and other operations.
 * For uniformity, ALL queue operations take rd_dq pointers as arguments.
 */

#define RDQ_SET_EMPTY(q)  						\
{									\
    (q)->rq_forw = ((q)->rq_back = (q)); 				\
}

#define RDQ_ISEMPTY(q)	  						\
    (((q)->rq_forw == (q)) && ((q)->rq_back == (q)))

#define RDQ_INIT(q, r)	{						\
    RDQ_SET_EMPTY((q));							\
    (q)->rq_self = (r);							\
}

#define RDQ_INSERT_AFTER(q, e) {					\
    (e)->rq_forw = (q)->rq_forw;					\
    (e)->rq_back = (q);							\
    (q)->rq_forw->rq_back = (e);					\
    (q)->rq_forw = (e);							\
}

#define RDQ_INSERT_BEFORE(q, e) {					\
    (e)->rq_back = (q)->rq_back;					\
    (e)->rq_forw = (q);							\
    (q)->rq_back->rq_forw = (e);					\
    (q)->rq_back = (e);							\
}

#define RDQ_UNLINK(q) {							\
    (q)->rq_forw->rq_back = (q)->rq_back;				\
    (q)->rq_back->rq_forw = (q)->rq_forw;				\
    RDQ_SET_EMPTY((q));							\
}

#define RDQ_UNLINK_LIST(h,j) {						\
    rd_dq *XXrq;							\
    XXrq = (h)->rq_forw;						\
    while (XXrq->rq_self != (j)) {					\
        RDQ_UNLINK(XXrq);						\
        XXrq = (h)->rq_forw;						\
    }									\
}

#define RDQ_UNLINK_LIST_FREE_ELMS(h,j) {				\
    rd_dq *XXrq;							\
    XXrq = (h)->rq_forw;						\
    while (XXrq->rq_self != (j)) {	                                \
        RDQ_UNLINK(XXrq);                 				\
	free(XXrq->rq_self);                                            \
        XXrq = (h)->rq_forw;						\
    }									\
}

/*
 * Splices two lists together, whose list heads are given by q1 and q2.
 * The resulting head is q1, and q2 is returned as the empty list.
 */

#define RDQ_SPLICE(q1, q2) {						\
    if (!RDQ_ISEMPTY(q2)) {						\
        (q2)->rq_forw->rq_back = (q1)->rq_back;				\
        (q1)->rq_back->rq_forw = (q2)->rq_forw;				\
        (q2)->rq_back->rq_forw = (q1);					\
        (q1)->rq_back = (q2)->rq_back;					\
        RDQ_SET_EMPTY((q2));						\
    }									\
}

/*
 * Macros for looping through a doubly linked list, starting from the head.
 * Given a head, element pointer and an element type.
 */

#define RDQ_LIST_START(h,j,e,t)	{					\
    rd_dq *XXrq;							\
    for (XXrq = (h)->rq_forw; XXrq->rq_self != (j); XXrq = XXrq->rq_forw) {\
        (e) = (t *) XXrq->rq_self;					\

#define RDQ_LIST_END(h,j,e,t)	} }

/*
 * Sets the element "e" to the head of the list, or NULL if the list
 * is empty. This is used to create while loops when using RDQ_LIST_START
 * may not be appropriate (e.g. when we need to do some processing on
 * a list element and then delete each element of the list).
 */

#define RDQ_LIST_HEAD(q,e,t)						\
    ((e) = (RDQ_ISEMPTY(q)) ? NULL : (t *) ((q)->rq_forw->rq_self))



/*
 * Macros for machine related predicates.
 */

#define RD_IS_FINAL(s)	      					\
    (!RDQ_ISEMPTY(&((s)->rs_final)))

#define RD_ACCEPTS_EMPTY_STRING(f)				\
    (RD_IS_FINAL((f)->rf_start))




/*
 * Cengiz Alaettinoglu's additions
 */

extern int rd_isEmpty_dfa(rd_fm *fm);
extern int rd_is_universal_dfa(rd_fm *fm);
extern rd_fm *rd_empty_string_machine();
extern rd_fm *rd_empty_set_machine(); 
extern rd_fm *rd_empty_set_dfa();
extern rd_fm *rd_duplicate_dfa(rd_fm *fm);
extern void rd_complement_dfa(rd_fm *fm);
extern void rd_free_nfa(rd_fm *fm); /* works for dfa and nfa */
#define rd_free_dfa rd_free_nfa
extern rd_fm *rd_intersect_dfa(rd_fm *fm1, rd_fm *fm2);
extern int rd_equal_dfa(rd_fm *fm1, rd_fm *fm2);
extern void rd_dton(rd_fm *fm);
extern rd_fm *rd_make_bol(rd_fm *fm);
extern rd_fm *rd_make_eol(rd_fm *fm);

#endif /* RE2DFA */
