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

#ifndef Community_H
#define Community_H

#include "config.h"
#include <iostream>
#include "dataset/unsigned.OXPSet.h"
#include "rpsl/List.hh"

class Item;

const int COMMUNITY_INTERNET            = 0;
const int COMMUNITY_NO_EXPORT           = 0xFFFFFF01u;
const int COMMUNITY_NO_ADVERTISE        = 0xFFFFFF02u;
const int COMMUNITY_NO_EXPORT_SUBCONFED = 0xFFFFFF03u;

inline void community_print(std::ostream &os, unsigned int i) {
   if (i == COMMUNITY_INTERNET)
      os << "INTERNET";
   else if (i == COMMUNITY_NO_EXPORT)
      os << "NO_EXPORT";
   else if (i == COMMUNITY_NO_EXPORT_SUBCONFED)
      os << "NO_EXPORT_SUBCONFED";
   else if (i == COMMUNITY_NO_ADVERTISE)
      os << "NO_ADVERTISE";
   else {
      int high = i >> 16;
      int low  = i & 0xFFFF;
      if (high == 0 || high == 0xFFFF)
	 os << i;
      else
	 os << high << ":" << low;
   }
}

class CommunitySet : public unsignedOXPSet, public ListNode {
   friend std::ostream& operator<<(std::ostream &os, CommunitySet& cs);
public:

   CommunitySet() : unsignedOXPSet(), ListNode(), mark(0) {};
   CommunitySet(Item *item) : unsignedOXPSet(), mark(0) {
      addCommunity(item);
   }
   CommunitySet(int community) : unsignedOXPSet(), mark(0) {
      add(community);
   }
   CommunitySet(const CommunitySet& b) : unsignedOXPSet(b), ListNode(),
					 mark(b.mark) {};
   void addCommunity(Item *item);

   int operator==(CommunitySet& b) {
      return unsignedOXPSet::operator==(b) && mark == b.mark;
   }
   void operator=(const CommunitySet& b) {
      *((unsignedOXPSet *) this) = (unsignedOXPSet) b;
      mark = b.mark;
   }
   int isEmpty() {
      return empty();
   }

private:
   friend class JunosConfig; // this uses the below mark field during caching
   int mark;
};

#endif // Community_H
