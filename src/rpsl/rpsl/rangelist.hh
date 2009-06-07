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

#ifndef RANGELIST_HH
#define RANGELIST_HH

#include "config.h"
#include <iostream>
#include "util/List.hh"

#define RANGE_TOP 65535

class RangeList {
friend class regexp_nf;
friend class CiscoConfig;
friend class JunosConfig;
public:
   static const char *prefix_str;
   static bool  useOrList;
public:
   class Range : public ListNode {
   public:
      unsigned int low;
      unsigned int high;
      
      Range()               {}
      Range(const Range& r) : low(r.low), high(r.high) {}
      Range(int l, int h)   : low(l),     high(h)      {}

      bool operator==(const Range &b) const {
	 return low == b.low && high == b.high;
      }
      bool operator!=(const Range &b) const {
	 return ! (*this == b);
      }
   };

private:
   List<Range> ranges;

public:
   RangeList(int low) {
      ranges.prepend(new Range(low, low));
   }
   RangeList(int low, int high) {
      ranges.prepend(new Range(low, high));
   }
   RangeList(const RangeList& n) {
      for (Range *pi = n.ranges.head(); pi; pi = n.ranges.next(pi))
	 ranges.append(new Range(*pi));
   }
   RangeList() {}
   // ~RangeList() {} compiler generated is fine

   void complement();		                 // this destroys *this
   RangeList& operator+=(RangeList& no);         // this destroys no

   RangeList& operator=(const RangeList& no) {
      clear();
      for (Range *pi = no.ranges.head(); pi; pi = no.ranges.next(pi))
	 ranges.append(new Range(*pi));
      return *this;
   }   

   void add(int low) {
      add(low, low);
   }
   void add(int low, int high) {
      if (low > high) {
	 int tmp = low;
	 low = high;
	 high = tmp;
      }
      RangeList no(low, high);
      *this += no;
   }
   void clear() {
      ranges.clear();
   }
   int empty() const {
      return ranges.isEmpty();
   }
   int universal() const {
      return (ranges.isSingleton()
	      && ranges.head()->low == 0 
	      && ranges.head()->high == RANGE_TOP);
   }

   int is_singleton() const {
      Range *h = ranges.head();

      return (ranges.isSingleton()
	      && ((h->low == 0 && h->high == RANGE_TOP) 
		  || (h->low == h->high)));
   }

   int contains(const RangeList::Range r) const;

   bool operator==(const RangeList &b) const {
      if (ranges.size() != b.ranges.size())
	 return false;
      Range *i, *j;
      for (i = ranges.head(), j = b.ranges.head(); 
	   i && j;
	   i = ranges.next(i), j = b.ranges.next(j))
	 if (*i != *j)
	    return false;
      return true;
   }
   bool operator!=(const RangeList &b) const {
      return ! (*this == b);
   }

   friend std::ostream& operator<<(std::ostream&, const RangeList &no);
};
#endif   // RANGELIST_HH
