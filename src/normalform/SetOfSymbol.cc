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

#if (__GNUC__ > 2)
#include <ostream>
#else
#include <ostream.h>
#endif
#include <cstdio>
#include <cassert>
#include "config.h"
#include "SetOfSymbol.hh"
#include "gnug++/SetOfUInt.hh"
#include "util/debug.hh"

using namespace std;

SetOfSymbol::SetOfSymbol(SetOfSymbol &a) {
   _universal = a._universal;
   for (Pix i = a.conjuncts.first(); i; a.conjuncts.next(i))
      conjuncts.add(new SymbolConjunct(*a.conjuncts(i)));
}

ostream& operator<<(ostream& stream, SetOfSymbol& set) {
   Pix i, k;
   SymbolConjunctPtr conjunct;

   for (i = set.conjuncts.first(); i;  ) {
      conjunct = set.conjuncts(i);

      stream << "(" << *conjunct << ")";

      set.conjuncts.next(i);
      if (i)
	 stream << " and ";
   }

   return stream;
}

void SetOfSymbol::clear() {
   Pix i, j;

   for (i = j = conjuncts.first(); i;  i = j) {
      conjuncts.next(j);
      delete conjuncts(i);
   }

   conjuncts.clear();
   _universal = 0;
}

// assumes *ORDERED* bag implementation
// this may not be reducing to the minimum number of conjuncts
void SetOfSymbol::reduce() {
   Pix i, j, k;
   SetOfSymID s, notS;
   
   // { s1, s2, s3 } INTERSECTION { s1, s2 } is { s1, s2 }
   for (i = conjuncts.first(); i; conjuncts.next(i))
      for (j = i, conjuncts.next(j), k = j; j; j = k) {
	 conjuncts.next(k);
	 if (*conjuncts(i) <= *conjuncts(j)) {
	    delete conjuncts(j);
	    conjuncts.del_this(j);
	 }
      }

   // for each singleton conjunct { s1 }, insert ~s1 into S
   SymbolConjunct S;
   for (i = conjuncts.first(); 
	i;
	conjuncts.next(i))
      if (conjuncts(i)->length() == 1)
	 S.negatedMerge(*conjuncts(i));
   
   // if { s1, s2, s3 } SUBSETEQ s, then the result is empty
   // E.g. { s1, s2, s3 } INT { ~s1 } INT { ~s2 } INT { ~s3 } = empty
   // since s = { ~~s1, ~~s2, ~~s3 }
   // E.g. { 690 } INT { ~690 } = empty
   for (i = conjuncts.first(); i; conjuncts.next(i))
      if (*conjuncts(i) <= S) {
	 clear();
	 return;
      } else
	 conjuncts(i)->remove(S);
}

void SetOfSymbol::operator &= (SetOfSymbol& b) { // intersection
   SymbolConjunctPtr conjunct;
   Pix i;

   if (empty() || b._universal)
      return;

   if (b.empty()) { // result is empty
      clear();
      _universal = 0;
      return;
   }

   // append conjuncts of b into conjuncts of *this
   splice(b);

   if (_universal)
      _universal = b._universal;
   else
      reduce();
}

void SetOfSymbol::splice(SetOfSymbol& b) {
   Pix i;
   SymbolConjunct *conjunct;
   // append conjuncts of b into conjuncts of *this
   while (i = b.conjuncts.first()) {
      conjunct = b.conjuncts(i);
      b.conjuncts.del_this(i);
      conjuncts.add(conjunct);
   }
}

void SetOfSymbol::operator |= (SetOfSymbol& b) { // union
   SymbolConjunctPtr conjunct;
   Pix i, j;
   SetOfSymbol tmp;

   if (_universal || b.empty())
      return; 

   if (b._universal) { // result is universal
      clear();
      _universal = 1;
      return;
   }

   if (empty()) { // become b
      splice(b);
      return;
   }

   // copy conjuncts into conjuncts of tmp
   tmp.splice(*this);

   // cartesian product b with tmp result into *this
   for (i = tmp.conjuncts.first(); i; tmp.conjuncts.next(i)) {
      for (j = b.conjuncts.first(); j; b.conjuncts.next(j)) {
	 conjunct = new SymbolConjunct(*tmp.conjuncts(i));
	 conjunct->merge(*b.conjuncts(j));
	 if (! conjunct->isUniversal()) 
	    conjuncts.add(conjunct);
	 else 
	    delete conjunct;
      }
   }
   
   // check to see whether we became universal
   if (conjuncts.empty())
      _universal = 1;
   else
      reduce();
}

void SetOfSymbol::operator ~  () { // complement
   SymbolConjunctPtr conjunct;
   Pix i, j;
   SetOfSymbol tmp, tmp2;

   if (_universal) {
      assert(conjuncts.empty());
      _universal = 0;
      return;
   } 
   if (empty()) {
      _universal = 1;
      return;
   }

   // copy conjuncts into conjuncts of tmp
   tmp.splice(*this);


   SymID sid;
   bool negated;
   for (i = tmp.conjuncts.first(); i; tmp.conjuncts.next(i)) {
      tmp2.clear();
      for (tmp.conjuncts(i)->first(sid, negated); 
	   sid; 
	   tmp.conjuncts(i)->next(sid, negated)) {
	 // negate here
	 conjunct = new SymbolConjunct(sid, !negated); 
	 tmp2.conjuncts.add(conjunct);
      }
      *this |= tmp2;
   }
}   

void SetOfSymbol::operator =  (SetOfSymbol& b) { // assignment
   SymbolConjunctPtr conjunct;
   Pix i;

   clear();
   _universal = b._universal;

   // copy conjuncts of b into conjuncts
   for (i = b.conjuncts.first(); i; b.conjuncts.next(i)) {
      conjunct = new SymbolConjunct(*b.conjuncts(i));
      conjuncts.add(conjunct);
   }

}

void SetOfSymbol::operator =  (const SetOfSymID& b) { // assignment
   SymbolConjunctPtr conjunct;
   clear();
   _universal = 0;

   conjunct = new SymbolConjunct(b);
   conjuncts.add(conjunct);
}

void SetOfSymbol::operator =  (const SetOfUInt& b) { // assignment
   for (Pix p = b.first(); p; b.next(p))
      add(b(p));
}

void SetOfSymbol::add(ASt as) {
   char buffer[64];
   asnum_string(buffer, as);
   add(symbols.symID(buffer));
}

void SetOfSymbol::add(SymID sid) {
   if (conjuncts.empty()) {
      SymbolConjunct *conjunct = new SymbolConjunct(sid);
      conjuncts.add(conjunct);
   } else {
      SymbolConjunct *conjunct = conjuncts(conjuncts.first());
      conjunct->add(sid);
   }
}

// Below code assumes *ORDERED* bag implementation
int  SetOfSymbol::operator == (SetOfSymbol& b) { // equivalance
   Pix i, j, lastj;
   int lastlength, found;
   
   if (_universal != b._universal)
      return 0;

   if (_universal == 1)
      return 1;

   if (conjuncts.length() != b.conjuncts.length())
      return 0;

   // for each conjunct find a matching conjunct in b
   lastj = (Pix) 0;
   lastlength = -1;
   for (i = conjuncts.first(); i; conjuncts.next(i)) {
      found = 0;

      if (lastlength != conjuncts(i)->length()) {
	 lastlength = conjuncts(i)->length();
	 lastj = b.conjuncts.seek(conjuncts(i), lastj);
      }

      for (j = lastj; 
	   j && lastlength == b.conjuncts(j)->length(); 
	   b.conjuncts.next(j))
	 if (*conjuncts(i) == *b.conjuncts(j)) {
	    found = 1;
	    break;
	 }

      if (!found)
	 return 0;
   }
   
   return 1;
}
