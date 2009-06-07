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
#include "rangelist.hh"
#include <ostream>

using namespace std;

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

// static member initialization
const char *RangeList::prefix_str = "AS";
bool  RangeList::useOrList = false;

ostream& operator<<(ostream& out, const RangeList &no) {
   RangeList::Range *pi;

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
