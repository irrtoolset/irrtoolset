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
//
//  Questions concerning this software should be directed to 
//  irrtoolset@cs.usc.edu.
//
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include <cstdio>
#include "config.h"
#include "FilterOfCommunity.hh"
#include "irrutil/debug.hh"
#include <ostream>

using namespace std;

ostream& operator<<(ostream& os, CommunityConjunct& set) {
   CommunitySet *cs;
   bool and_ = false;

   if (!set.pe->empty()) 
      os << "community == {" << *set.pe << "}";
   else {
      if (!set.n->empty()) {
	 os << "not community(" << *set.n << ")";
	 and_ = true;
      }

      for (Pix pi = set.p->first(); pi; set.p->next(pi)) {
	 if (and_)
	    os << " AND ";
	 else
	    and_ = true;
	 os << "community(";
	 community_print(os, (*set.n)(pi));
	 os << ")";
      }

      for (cs = set.ne.head(); cs; cs = set.ne.next(cs)) {
	 if (and_)
	    os << " AND ";
	 else 
	    and_ = true;
	 os << "NOT community == {" << *cs << "}";
      }
   }

   return os;
}

void CommunityConjunct::reduce() {
   CommunitySet t1, t2;

   // become empty if p INT n is non empty
   if (!p->isEmpty() && !n->isEmpty()) {
      t1 = *p;
      t2 = *n;
      t1 &= t2;
      if (! t1.empty()) {
	 make_empty();
	 return;
      }
   }

   // become empty if pe INT n is non empty
   if (!pe->isEmpty() && !n->isEmpty()) {
      t1 = *pe;
      t2 = *n;
      t1 &= t2;
      if (! t1.empty()) {
	 make_empty();
	 return;
      }
   }

   // become empty if pe is in ne
   CommunitySet *cs;
   for (cs = ne.head(); cs; cs = ne.next(cs))
      if (*pe == *cs) {
	 make_empty();
	 return;
      }

   // become empty if pe does not contain p
   if (!pe->isEmpty() && ! (*p <= *pe)) { // <= is the set contained operator
      make_empty();
      return;
   }

   // now if there is pe, the others has to be empty
   // since our conjuct can only match one community string
   if (!pe->empty()) {
      p->clear();
      n->clear();
      ne.clear();
      return;
   }

   // delete duplicates in ne
   CommunitySet *cs2, *cs3;
   for (cs = ne.head(); cs; cs = ne.next(cs))
      for (cs2 = ne.next(cs); cs2; cs2 = cs3) {
	 cs3 = ne.next(cs2);
	 if (*cs == *cs2) {
	    ne.remove(cs2);
	    delete cs2;
	 }
      }

   // delete members of ne if communities in n or p make them redundant
   Pix px;
   for (cs = cs2 = ne.head(); cs; cs = cs2) {
      cs2 = ne.next(cs2);
      // delete cs if a community in n is also in cs
      for (px = n->first(); px; n->next(px))
	 if (cs->contains((*n)(px))) {
	    ne.remove(cs);
	    delete cs;
	    cs = NULL;
	    break;
	 }
      if (cs)
	 // delete cs if a community in p is not in cs
	 for (px = p->first(); px; p->next(px))
	    if (! cs->contains((*n)(px))) {
	       ne.remove(cs);
	       delete cs;
	       break;
	    }
   }
}

void CommunityConjunct::do_and(CommunityConjunct &b) {
   // assume *this and b are both simplified 

   if (! pe->empty() && ! b.pe->empty()) {
      if (*pe == *b.pe) {
	 b.make_empty();
	 return;
      }
      else {
	 b.make_empty();
	 make_empty();
	 return;
      }
   }

   if (!pe->empty()) { // note that b.pe has to be empty
      p = b.p;
      n = b.n;
      ne.splice(b.ne);

      b.p = NULL;
      b.n = NULL;

      b.make_empty();
      reduce();
      return;
   }
    
   (*pe) |= (*b.pe);
   (*p)  |= (*b.p);
   (*n)  |= (*b.n);
   ne.splice(b.ne);

   b.make_empty();
   reduce();  
}

bool CommunityConjunct::implies(const CommunityConjunct &b) { // this contains b or equals b
   // assumes *this and b are reduced
   if (! pe->empty())
      return *pe == *b.pe;

   if (! b.pe->empty()) {
      for (CommunitySet *csp = ne.head(); csp; csp = ne.next(csp))
	 if (*csp == *b.pe)
	    return false;

      if (! (*p <= *b.pe))
	 return false;

      CommunitySet tmp(*b.pe);
      tmp &= *n;
      
      return tmp.empty();
   }

   if (! (*p <= *b.p && *n <= *b.n))
      return false;

   if (ne.size() > b.ne.size())
      return false;

   CommunitySet *csp, *csp2;
   for (csp = ne.head(); csp; csp = ne.next(csp)) {
      for (csp2 = b.ne.head(); csp2; csp2 = b.ne.next(csp2))
	 if (*csp == *csp2)
	    break;
      if (! csp2)
	 return false;
   }
   
   return true;
}

int CommunityConjunct::operator ==(const CommunityConjunct &b) {
   return *p == *b.p && *n == *b.n && *pe == *b.pe && ne == b.ne;
}

// ============================================================

void FilterOfCommunity::do_print(ostream& stream) {
   stream << *this;
}

ostream& operator<<(ostream& stream, FilterOfCommunity& set) {
   CommunityConjunct *cptr;

   for (cptr = set.conjuncts.head(); cptr;  ) {
      stream << *cptr;
      cptr = set.conjuncts.next(cptr);
      if (cptr)
	 stream << " OR ";
   }

   return stream;
}

void FilterOfCommunity::operator &= (FilterOfCommunity& b) { // intersection
   CommunityConjunct *cp1, *cp2, *cp3, *cp4;

   if (isEmpty() || b.is_universal()) {
      b.make_empty();
      return;
   }

   if (b.isEmpty()) { // result is empty
      make_empty();
      return;
   }

   if (is_universal()) { // become b
      conjuncts.splice(b.conjuncts);
      b.make_empty();
      _universal = false;
      return;
   }

   // cartesian product conjuncts of *this and b
   FilterOfCommunity result;
   for (cp1 = conjuncts.head(); cp1; cp1 = conjuncts.next(cp1))
      for (cp2 = b.conjuncts.head(); cp2; cp2 = b.conjuncts.next(cp2)) {
	 cp3 = new CommunityConjunct(*cp1);
	 cp4 = new CommunityConjunct(*cp2);
	 cp3->do_and(*cp4);
	 if (!cp3->isEmpty())
	    result.conjuncts.append(cp3);
	 else
	    delete cp3;
	 delete cp4;
      }

   make_empty();
   b.make_empty();

   if (!result.isEmpty()) {
      conjuncts.splice(result.conjuncts);
      reduce();
   }
}

void FilterOfCommunity::operator |= (FilterOfCommunity& b) { // union
   if (b.isEmpty() || is_universal()) {
      b.make_empty();
      return;
   }

   if (b.is_universal()) { // result is universal
      b.make_empty();
      make_universal();
      return;
   }

   if (isEmpty()) { // become b
      conjuncts.splice(b.conjuncts);
      b.make_empty();
      return;
   }

   conjuncts.splice(b.conjuncts);
   b.make_empty();
   reduce();
}

void FilterOfCommunity::operator ~() { // complement
   if (isEmpty() || is_universal()) {
      _universal = ! _universal;
      return;
   }

   CommunityConjunct *cp1;
   FilterOfCommunity tmp;
   FilterOfCommunity result;

   result.make_universal();
   for (cp1 = conjuncts.head(); cp1; cp1 = conjuncts.next(cp1)) {
      tmp.NegateCommunityConjunct(cp1);
      result &= tmp;
   }
 
   make_empty();
   conjuncts.splice(result.conjuncts);
}

void FilterOfCommunity::NegateCommunityConjunct(CommunityConjunct *cp) {
   CommunityConjunct *cp2;
   Pix pi;
   CommunitySet *CSptr;

   make_empty();

   // handle pe, easy
   if (!cp->pe->empty()) { 
      cp2 = new CommunityConjunct;
      cp2->ne.append(cp->pe);
      cp->pe = NULL;
      conjuncts.append(cp2);
   }
   // handle p, easy
   while ((pi = cp->p->first())) {
      cp2 = new CommunityConjunct;
      cp2->n->add((*cp->p)(pi));
      conjuncts.append(cp2);
      cp->p->del((*cp->p)(pi));
   }
   // handle n, easy
   while ((pi = cp->n->first())) {
      cp2 = new CommunityConjunct;
      cp2->p->add((*cp->n)(pi));
      conjuncts.append(cp2);
      cp->n->del((*cp->n)(pi));
   }
   // handle ne, easy
   while ((CSptr = cp->ne.head())) {
      cp2 = new CommunityConjunct;
      cp2->pe = CSptr;
      conjuncts.append(cp2);
      cp->ne.remove(CSptr);
   }

   //   reduce();
}

void FilterOfCommunity::reduce() {
   // this does not reduce to minimum possible, but adequate in practise

   if (conjuncts.isEmpty() || conjuncts.isSingleton()) 
      // covers (empty() || _universal)
      return;

   CommunityConjunct *cp1, *cp2, *cp3;

   // delete redundant conjuncts
   for (cp1 = conjuncts.head(); cp1; cp1 = conjuncts.next(cp1))
      for (cp2 = conjuncts.head(); cp2; cp2 = cp3) {
	 cp3 = conjuncts.next(cp2);
	 if (cp1 != cp2 && cp1->implies(*cp2)) {
	    conjuncts.remove(cp2);
	    delete cp2;
	 }
      }

   // check to see if *this is universal
   FilterOfCommunity result;
   for (cp1 = conjuncts.head(); cp1; cp1 = conjuncts.next(cp1)) {
      cp2 = new CommunityConjunct(*cp1);
      result.NegateCommunityConjunct(cp2);
      delete cp2;
      for (cp2 = result.conjuncts.head(); 
	   cp2; 
	   cp2 = result.conjuncts.next(cp2)) {
	 for (cp3 = conjuncts.head(); cp3; cp3 = conjuncts.next(cp3))
	    if (cp3->implies(*cp2))
	       break;
	 if (! cp3)
	    break;
      }
      if (!cp2) { // yes, *this is universal
	 make_universal();
	 return;
      }
   }
}

int  FilterOfCommunity::operator == (FilterOfCommunity& b) { // equivalance
   return _universal == b._universal && conjuncts == b.conjuncts;
}

void FilterOfCommunity::operator =  (FilterOfCommunity& b) { // assignment
   make_empty();
   conjuncts = b.conjuncts;
   _universal = b._universal;
}
