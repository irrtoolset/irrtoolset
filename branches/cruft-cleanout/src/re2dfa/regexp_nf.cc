//  $Id$
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
//
//  Copyright (c) 1994 by the University of Southern California
//  All rights reserved.
//
//  Permission to use, copy, modify, and distribute this software and its
//  documentation in source and binary forms for lawful non-commercial
//  purposes and without fee is hereby granted, provided that the above
//  copyright notice appear in all copies and that both the copyright
//  notice and this permission notice appear in supporting documentation,
//  and that any documentation, advertising materials, and other materials
//  related to such distribution and use acknowledge that the software was
//  developed by the University of Southern California, Information
//  Sciences Institute. The name of the USC may not be used to endorse or
//  promote products derived from this software without specific prior
//  written permission.
//
//  THE UNIVERSITY OF SOUTHERN CALIFORNIA DOES NOT MAKE ANY
//  REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY
//  PURPOSE.  THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
//  TITLE, AND NON-INFRINGEMENT.
//
//  IN NO EVENT SHALL USC, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT, TORT,
//  OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH, THE USE
//  OR PERFORMANCE OF THIS SOFTWARE.
//
//  Questions concerning this software should be directed to 
//  ratoolset@isi.edu.
//
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include "config.h"
#include <cstring>
#include <cassert>
#include <map>

#include "irrutil/debug.hh"
#include "regexp_nf.hh"
#include "dataset/SetOfUInt.hh"
#include "irr/irr.hh"

using namespace std;

bool regexp_nf::expand_as_macros = true;

///// regexp_empty_set ///////////////////////////////////////////////////////

regexp* regexp_nf::dup() const { 
   return new regexp_nf(*this);
}

///// regexp_nf ///////////////////////////////////////////////////////

bool regexp_nf::operator==(regexp_nf& b) {
   return rd_equal_dfa(dfa(), b.dfa());
}

bool regexp_nf::is_universal() {
   return rd_is_universal_dfa(dfa());
}

bool regexp_nf::isEmpty() {
   return rd_isEmpty_dfa(dfa());
}

bool regexp_nf::isEmptyStr() {
   rd_init();
   rd_fm* m = rd_empty_string_machine();
   m = rd_ntod(m); 
   rd_minimize(m);
   bool code = rd_equal_dfa(dfa(), m);
   rd_free_dfa(m);
   return code;
}

rd_fm* regexp_nf::dfa(ASt peerAS) {
   if (!m) {
      rd_init();
      m = re2nfa(this, peerAS);
      m = rd_make_bol(m);
      m = rd_make_eol(m);
      m = rd_ntod(m); 
      rd_minimize(m);
   }
   return m;
}

ostream& operator<<(ostream& s, const regexp_nf& rs) {
   regexp_nf::RegexpConjunct *rc;
   regexp_nf::RegexpConjunct::ReInt *ri;
   bool complex = false;
   bool needOr  = false;

   // simple re, print the regexp
   for (rc = rs.rclist.head(); rc; rc = rs.rclist.next(rc)) {
      if (rc->regexs.isSingleton()) {
	 ri = rc->regexs.head();
	 if (! ri->negated) {
	    if (needOr)
	       s << " | ";
	    else
	       needOr = true;
	    s << *ri->re;
	 } else
	    complex = true;
      } else
	 complex = true;
   }

   if (complex) {
      // complex re, print the dfa
      regexp *reg = rs.construct();
      s << "^" << *reg << "$";
      delete reg;
   }

   return s;
}
   
////////////////////////////// re2nfa ////////////////////

rd_fm *regexp_nf::do_tildaplus_as(rd_fm *result, int as, bool star) const {
   rd_dq *rdr = rd_alloc_range_list_empty();
   rd_insert_range(rdr, rd_alloc_range(as, as));
   rd_fm* m;
   m = rd_singleton(rdr);
   if (star)
      m = rd_zero_or_more(m);
   else
      m = rd_one_or_more(m);
   if (! result)
      result = m;
   else
      result = rd_alternate(result, m);
   
   return result;
}

rd_fm *regexp_nf::do_tildaplus(regexp_symbol *sym, ASt peerAS, bool star) const {
   RangeList::Range *r;
   int i;
   assert(!sym->complemented);
      
   rd_fm *result = NULL;
   for (r = sym->asnumbers.ranges.head(); 
	r; 
	r = sym->asnumbers.ranges.next(r))
      for (i = r->low; i <= r->high; ++i)
	 result = do_tildaplus_as(result, i, star);

   for (Pix p = sym->asSets.first(); 
	p; 
	sym->asSets.next(p)) {
      SymID sid = (SymID) sym->asSets(p);
      if (symbols.isPeerAS(sid)) {
	 assert(peerAS != RE_INVALID_AS);
	 result = do_tildaplus_as(result, peerAS, star);
	 if (expand_as_macros)
	    sym->asnumbers.add(peerAS, peerAS);
      } else {
	 sid = symbols.resolvePeerAS(sid, peerAS);
	 const SetOfUInt* as_set = irr->expandASSet(sid);
      
	 if (as_set) {
	    for (Pix pi = as_set->first(); pi; as_set->next(pi)) {
	       int as1 = (*as_set)(pi);
	       result = do_tildaplus_as(result, as1, star);
	       if (expand_as_macros)
		  sym->asnumbers.add(as1, as1);
	    }
	 }
      }
   }

   if (expand_as_macros)
      sym->asSets.clear();

   return result;
}

rd_fm* regexp_nf::re2nfa(regexp *re, ASt peerAS) const {
   if (typeid(*re) == typeid(regexp_empty_set))
      return rd_empty_set_machine();

   if (typeid(*re) == typeid(regexp_bol)) {
      rd_fm* m = rd_empty_string_machine();
      m->bolexp = 1;
      return m;
   }

   if (typeid(*re) == typeid(regexp_eol)) {
      rd_fm* m = rd_empty_string_machine();
      m->eolexp = 1;
      return m;
   }

   if (typeid(*re) == typeid(regexp_empty_str))
      return rd_empty_string_machine();

   if (typeid(*re) == typeid(regexp_cat))
      return rd_concatenate(re2nfa(((regexp_cat *) re)->left, peerAS),
			    re2nfa(((regexp_cat *) re)->right, peerAS));

   if (typeid(*re) == typeid(regexp_or))
      return rd_alternate(re2nfa(((regexp_or *) re)->left, peerAS), 
			  re2nfa(((regexp_or *) re)->right, peerAS));

   if (typeid(*re) == typeid(regexp_star))
      return rd_zero_or_more(re2nfa(((regexp_star *) re)->left, peerAS));

   if (typeid(*re) == typeid(regexp_question))
      return rd_zero_or_one(re2nfa(((regexp_question *) re)->left, peerAS));

   if (typeid(*re) == typeid(regexp_plus))
      return rd_one_or_more(re2nfa(((regexp_plus *) re)->left, peerAS));

   if (typeid(*re) == typeid(regexp_tildaplus)) {
      assert(typeid(*((regexp_tildaplus *) re)->left) ==  typeid(regexp_symbol));
      return do_tildaplus((regexp_symbol *) ((regexp_tildaplus *) re)->left, peerAS, false);
   }

   if (typeid(*re) == typeid(regexp_tildastar)) {
      assert(typeid(*((regexp_tildastar *) re)->left) ==  typeid(regexp_symbol));
      return do_tildaplus((regexp_symbol *) ((regexp_tildastar *) re)->left, peerAS, true);
   }

   if (typeid(*re) == typeid(regexp_symbol)) {
      rd_dq *rdr = rd_alloc_range_list_empty();

      RangeList::Range *r;
      for (r = ((regexp_symbol *) re)->asnumbers.ranges.head(); 
	   r; 
	   r = ((regexp_symbol *) re)->asnumbers.ranges.next(r))
	 rd_insert_range(rdr, rd_alloc_range(r->low, r->high));

      for (Pix p = ((regexp_symbol *) re)->asSets.first(); 
	   p; 
	   ((regexp_symbol *) re)->asSets.next(p)) {
	 SymID sid = (SymID) ((regexp_symbol *) re)->asSets(p);
	 if (symbols.isPeerAS(sid)) {
	    assert(peerAS != RE_INVALID_AS);
	    rd_insert_range(rdr, rd_alloc_range(peerAS, peerAS));
	    if (expand_as_macros)
	       (((regexp_symbol *) re)->asnumbers).add(peerAS, peerAS);
	 } else {
	    sid = symbols.resolvePeerAS(sid, peerAS);
	    const SetOfUInt* as_set = irr->expandASSet(sid);
      
	    if (as_set) {
	       for (Pix pi = as_set->first(); pi; as_set->next(pi)) {
		  int as1 = (*as_set)(pi);
		  rd_insert_range(rdr, rd_alloc_range(as1, as1));
		  if (expand_as_macros)
		     (((regexp_symbol *) re)->asnumbers).add(as1, as1);
	       }
	    }
	 }
      }

      if (expand_as_macros)
	 ((SetOfSymID &) ((regexp_symbol *) re)->asSets).clear();

      if (((regexp_symbol *) re)->complemented)
	 rd_complement_range_list(rdr);

      rd_fm* m;
      if (RDQ_ISEMPTY(rdr)) {
	 free(rdr);
	 m = rd_empty_set_machine(); 
      } else 
	 m = rd_singleton(rdr);

      return m;
   }

   if (typeid(*re) == typeid(regexp_nf)) {
      assert(((regexp_nf *) re)->rclist.isSingleton());
      RegexpConjunct *rc= ((regexp_nf *) re)->rclist.head();
      assert(rc->regexs.isSingleton());
      regexp_nf::RegexpConjunct::ReInt *ri = rc->regexs.head();
      return re2nfa(ri->re, peerAS);
   }

   assert(1);
   rd_fm* m;
   return m;
}

//////////////////////////////////////// NF //////////////////////////////

void regexp_nf::become_universal() {
   rclist.clear();

   if (m)
      rd_free_dfa(m);
   m = rd_empty_set_dfa();
   rd_complement_dfa(m);
}

void regexp_nf::become_empty() {
   rclist.clear();

   if (m)
      rd_free_dfa(m);
   m = rd_empty_set_dfa();
}

regexp_nf::regexp_nf(const regexp_nf& s) {
   RegexpConjunct *rc, *rc2;

   for (rc = s.rclist.head(); rc; rc = s.rclist.next(rc)) {
      rc2 = new RegexpConjunct(*rc);
      rclist.append(rc2);
   }
   
   m = NULL;
   m = rd_duplicate_dfa(s.m);
}

regexp_nf::RegexpConjunct::RegexpConjunct(const RegexpConjunct &s) {
   mark    = s.mark;

   RegexpConjunct::ReInt *ri, *ri2;

   for (ri = s.regexs.head(); ri; ri = s.regexs.next(ri)) {
      ri2 = new RegexpConjunct::ReInt(*ri);
      regexs.append(ri2);
   }
}

void regexp_nf::do_or(regexp_nf &b) {
   if (b.isEmpty() || is_universal())
      return;

   if (b.is_universal()) {
      become_universal();
      return;
   }
   
   rd_fm *_m   = rd_duplicate_dfa(m);
   rd_fm *_b_m = rd_duplicate_dfa(b.m);

   rd_init();
   rd_dton(_m);
   rd_dton(_b_m);

   _m = rd_alternate(_m, _b_m);
   _m = rd_ntod(_m);
   rd_minimize(_m);

   if (rd_equal_dfa(m, _m)) // union is same as us
      ;
   else if (rd_equal_dfa(b.m, _m)) { // union is same as b
      rclist.clear();
      rclist.splice(b.rclist);
      b.become_empty();
   } else { // union is new!
      rclist.splice(b.rclist); 
      b.become_empty();
   }

   rd_free_dfa(m); /* works with dfa too */ 
   m = _m;

   if (rd_is_universal_dfa(m))
      rclist.clear();
}

void regexp_nf::do_and(regexp_nf &b) {
   if (b.is_universal() || isEmpty())
      return;

   if (b.isEmpty()) {
      become_empty();
      return;
   }

   rd_fm *m3 = rd_intersect_dfa(m, b.m);

#if 0   
   if (rd_equal_dfa(m, m3)) // intersection is same as us
      ;
   else if (rd_equal_dfa(b.m, m3)) { // intersection is same as b
      rclist.clear();
      rclist.splice(b.rclist);
      b.become_empty();
   } else { // intersection is new!
      do_and_terms(b);
      b.become_empty();
   }
#else
      do_and_terms(b);
      b.become_empty();
#endif

   rd_free_dfa(m); /* works with dfa too */ 
   m = m3;

   if (rd_isEmpty_dfa(m))
      rclist.clear();
}

void regexp_nf::do_and_terms(regexp_nf &b) {
   List<RegexpConjunct> tmp;
   RegexpConjunct *rc1, *rc2, *rc3, *tmp2;

   if (rclist.isEmpty()) {
      rclist.splice(b.rclist);
      return;
   }

   for (rc1 = rclist.head(); rc1; rc1 = rclist.next(rc1))
      for (rc2 = b.rclist.head(); rc2; rc2 = b.rclist.next(rc2)) {
	 rc3 = new RegexpConjunct;
	 tmp2 = new RegexpConjunct(*rc1);
	 rc3->regexs.splice(tmp2->regexs);
	 delete tmp2;
	 tmp2 = new RegexpConjunct(*rc2);
	 rc3->regexs.splice(tmp2->regexs);
	 delete tmp2;
	 tmp.append(rc3);
      }

   rclist.clear();
   b.rclist.clear();
   rclist.splice(tmp);
}

void regexp_nf::do_not() {
   if (is_universal()) {
      become_empty();
      return;
   }
    
   if (isEmpty()) {
      become_universal();
      return;
   }
   
   // complement machine
   rd_complement_dfa(m);

   // complement terms
   regexp_nf tmp, tmp2;
   RegexpConjunct *rc1, *rc2;
   RegexpConjunct::ReInt *ri1, *ri2;

   tmp.become_universal();
   for (rc1 = rclist.head(); rc1; rc1 = rclist.next(rc1)) {
      tmp2.become_empty();
      for (ri1 = rc1->regexs.head(); ri1; ri1 = rc1->regexs.next(ri1)) {
	 ri2 = new RegexpConjunct::ReInt(*ri1);
	 ri2->negated = ~ri2->negated;
	 rc2 = new RegexpConjunct;
	 rc2->regexs.append(ri2);
	 tmp2.rclist.append(rc2);
      }
      tmp.do_and_terms(tmp2);
   }

   tmp2.rclist.clear();
   rclist.clear();
   rclist.splice(tmp.rclist);	 
}

////////////////////////////// fsa to regexp conversion /////////////////////

#define state rd_state

struct int2 {
   int2() {}
   int2(state* ii, state* jj) : i(ii), j(jj) {}
   state *i, *j;

   struct less {
      int operator() (const int2& a, const int2& b) const {
	 return a.i <  b.i ||  a.i == b.i && a.j <  b.j;
      }
   };
};

void pmap(map<int2, regexp*, int2::less> &fmtore_map) {
   map<int2, regexp*, int2::less>::iterator pi;

   for (pi = fmtore_map.begin(); pi != fmtore_map.end(); ++pi)
      cerr << "map " << (*pi).first.i << " " << (*pi).first.j 
	   << " " << *(*pi).second << "\n";
   
}

regexp* regexp_nf::construct() const {
   map<int2, regexp*, int2::less> fmtore_map;
   map<int2, regexp*, int2::less>::iterator pi, qi, si;

   regexp *prefix, *suffix, *middle;
   rd_state *stt;
   rd_arc *arc;
   regexp_symbol *r_sym;

   // initialize fmtore_map from m
   RDQ_LIST_START(&(m->rf_states), m, stt, rd_state) {
      RDQ_LIST_START(&(stt->rs_arcs), stt, arc, rd_arc) {
	 int2 i2(stt, arc->ra_to);

	 pi = fmtore_map.find(i2);
	 if (pi == fmtore_map.end())
	    fmtore_map[i2] = r_sym = new regexp_symbol;
	 else
	    r_sym = (regexp_symbol *) (*pi).second;
	 r_sym->add(arc->ra_low, arc->ra_high);
	 
      } RDQ_LIST_END(&(stt->rs_arcs), stt, arc, rd_arc);
   } RDQ_LIST_END(&(m->states), m, stt, rd_state);

   // make two states;
   state start;
   state final;

   fmtore_map[int2(&start, m->rf_start)] = new regexp_empty_str;

   RDQ_LIST_START(&(m->rf_final), m, stt, rd_state) {
      fmtore_map[int2(stt, &final)] = new regexp_empty_str;
   } RDQ_LIST_END(&(m->rf_final), m, stt, rd_state);

   RDQ_LIST_START(&(m->rf_states), m, stt, rd_state) {// eliminate each state
      // pmap(fmtore_map);
      // make self looping middle
      if ((pi = fmtore_map.find(int2(stt, stt))) != fmtore_map.end()) {
	 middle = (*pi).second;
	 middle = buildStar(middle);
	 fmtore_map.erase(pi);
      } else
	 middle = new regexp_empty_str;

//      cerr << "Eliminating " << next[i].value() << " middle " << *middle << "\n";

      for (pi = fmtore_map.begin(); pi != fmtore_map.end(); ) {
	 if ((*pi).first.j == stt) {
	    prefix = buildCat((*pi).second, middle->dup());
//	    cerr << "arc from " << (*pi).first.i << " q " << *q << "\n";
	    for (qi = fmtore_map.lower_bound(int2(stt, 0));
		 qi != fmtore_map.end() && (*qi).first.i == stt;
		 ++qi) {
	       suffix = buildCat(prefix->dup(), (*qi).second->dup());
//	       cerr << " into " << (*qi).first.j << " p " << *p << "\n";
	       if ((si = fmtore_map.find(int2((*pi).first.i, (*qi).first.j))) != fmtore_map.end())
		  (*si).second = buildOr((*si).second, suffix);
	       else
		  fmtore_map[int2((*pi).first.i, (*qi).first.j)] = suffix;
	    }
	    delete prefix;
	    si = pi;
	    ++pi;
	    fmtore_map.erase(si);
	 } else
	    ++pi;
      }
      for (qi = fmtore_map.lower_bound(int2(stt, 0));
	   qi != fmtore_map.end() && (*qi).first.i == stt; 
	 ) {
	 delete (*qi).second;
	 si = qi;
	 ++qi;
	 fmtore_map.erase(si);
      }
	 
     delete middle;
   } RDQ_LIST_END(&(m->states), m, stt, rd_state);

   // check for empty string
   if (RD_ACCEPTS_EMPTY_STRING(m))
      return buildQuestion(fmtore_map[int2(&start, &final)]);

   return fmtore_map[int2(&start, &final)];
}

regexp* regexp_nf::buildCat(regexp *l, regexp *r) const {
   assert(l);
   assert(r);

   if (typeid(*l) == typeid(regexp_empty_set)) {
      delete r;
      return l;
   }

   if (typeid(*r) == typeid(regexp_empty_set)) {
      delete l;
      return r;
   }

   if (typeid(*l) == typeid(regexp_empty_str)) {
      delete l;
      return r;
   }
   
   if (typeid(*r) == typeid(regexp_empty_str)) {
      delete r;
      return l;
   }

   regexp_cat* re = new regexp_cat(l, r);

   return re;
}

regexp* regexp_nf::buildOr(regexp *l, regexp *r) const {
   assert(l);
   assert(r);

   if (typeid(*l) == typeid(regexp_empty_set)) {
      delete l;
      return r;
   }

   if (typeid(*r) == typeid(regexp_empty_set)) {
      delete r;
      return l;
   }

   if (typeid(*l) == typeid(regexp_empty_str)) {
      delete l;
      return buildQuestion(r);
   }

   if (typeid(*r) == typeid(regexp_empty_str)) {
      delete r;
      return buildQuestion(l);
   }

   if (*l == *r) {
      delete r;
      return l;
   }

   regexp_or* re = new regexp_or(l, r);
   return re;
}

regexp* regexp_nf::buildStar(regexp *l) const {
   assert(l);

   if (typeid(*l) == typeid(regexp_empty_set))
      return l;

   if (typeid(*l) == typeid(regexp_empty_str))
      return l;

   if (typeid(*l) == typeid(regexp_star))
      return l;

   regexp_star* re;

   if (typeid(*l) == typeid(regexp_question)) {
      re = new regexp_star(((regexp_question *) l)->left);
      ((regexp_question *) l)->left = NULL;
      delete l;
   } else
       re = new regexp_star(l);

   return re;
}

regexp* regexp_nf::buildQuestion(regexp *l) const {
   assert(l);

   if (typeid(*l) == typeid(regexp_empty_set))
      return l;

   if (typeid(*l) == typeid(regexp_empty_str))
      return l;

   if (typeid(*l) == typeid(regexp_star))
      return l;

   if (typeid(*l) == typeid(regexp_question))
      return l;

   regexp_question* re = new regexp_question(l);
   return re;
}
