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

#include "config.h"
#include "rangelist.hh"
#include <ostream>

using namespace std;

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

// static member initialization
char *RangeList::prefix_str = "AS";
bool  RangeList::useOrList = false;

ostream& operator<<(ostream& out, const RangeList &no) {
   RangeList::Range *pi, *qi;

   if (no.universal())
      out << ".";
   else if (no.empty())
      out << "E";
   else
      if (no.useOrList)
	 for (pi = no.ranges.head(); pi; ) {
	    out << no.prefix_str << pi->low;
	    if (pi->low != pi->high)
	       out << "-" << no.prefix_str << pi->high;
	    pi = no.ranges.next(pi);
	    if (pi)
	       out << "|";
	 }
      else
	 for (pi = no.ranges.head(); pi; pi = no.ranges.next(pi)) {
	    out << " " << no.prefix_str << pi->low;
	    if (pi->low != pi->high)
	       out << "-" << no.prefix_str << pi->high;
	 }

   return out;
}

RangeList& RangeList::operator+=(RangeList& no) { // destroys no
   Range *pi, *qi, *r;

   pi = ranges.head();
   qi = no.ranges.head();
   while (pi && qi) {
      while (pi && pi->low <= qi->low)
	 pi = ranges.next(pi);

      if (pi)
	 while (qi && qi->low < pi->low) {
	    r = qi;
	    qi = no.ranges.next(qi);
	    no.ranges.remove(r);
	    ranges.insertBefore(pi, r);
	 }
   }

   // add remaining ranges of no
   ranges.splice(no.ranges);

   for (pi = ranges.head(); pi; ) {
      qi = ranges.next(pi);
      if (qi && pi->high >= qi->low - 1) {
	 pi->high = MAX(qi->high, pi->high);
	 ranges.remove(qi);
	 delete qi;
      } else
	 pi = qi;
   }

   return *this;
}

void RangeList::complement() {
   Range *pi = ranges.head();
   Range *qi;

   if (universal()) {
      clear();
      return;
   }

   if (empty()) {
      ranges.append(new Range(0, RANGE_TOP));
      return;
   }

   if (pi->low != 0)
      ranges.prepend(new Range(0, (*pi).low-1));

   for (qi = ranges.next(pi); 
	qi; 
	pi = qi, qi = ranges.next(qi)) {
      pi->low  = pi->high + 1;
      pi->high = qi->low  - 1;
   }

   if (pi->high != RANGE_TOP) {
      pi->low  = pi->high + 1;
      pi->high = RANGE_TOP;
   } else {
      ranges.remove(pi);
      delete pi;
   }
}

int RangeList::contains(const Range r) const {
   for (Range *i = ranges.head(); i; i = ranges.next(i))
      if (i->low <= r.low && i->high >= r.high)
	 return 1;

   return 0;
}
