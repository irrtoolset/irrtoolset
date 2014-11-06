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

#ifndef RANGELIST_HH
#define RANGELIST_HH

#include "config.h"
#include <iostream>
#include "List.hh"

#define RANGE_TOP ~(unsigned int)0

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
      Range(unsigned int l, unsigned int h)   : low(l),     high(h)      {}

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
   RangeList(unsigned int low) {
      ranges.prepend(new Range(low, low));
   }
   RangeList(unsigned int low, unsigned int high) {
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

   void add(unsigned int low) {
      add(low, low);
   }
   void add(unsigned int low, unsigned int high) {
      if (low > high) {
	 unsigned int tmp = low;
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
