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
#include "util/debug.hh"
#include "community.hh"
#include <iostream>
#include "rpsl/rpsl_item.hh"

using namespace std;

void CommunitySet::addCommunity(Item *item) {
   if (typeid(*item) == typeid(ItemINT))
      add(((ItemINT *)item)->i);
   else 
      if (typeid(*item) == typeid(ItemWORD))
	 if (!strcasecmp(((ItemWORD *)item)->word, "no_advertise"))
	    add(COMMUNITY_NO_ADVERTISE);
	 else if (!strcasecmp(((ItemWORD *)item)->word, "no_export"))
	    add(COMMUNITY_NO_EXPORT);
	 else if (!strcasecmp(((ItemWORD *)item)->word,"no_export_subconfed"))
	    add(COMMUNITY_NO_EXPORT_SUBCONFED);
	 else
	    add(COMMUNITY_INTERNET);
      else 
	 if (typeid(*item) == typeid(ItemList)) {
	    int high = ((ItemINT *) ((ItemList *) item)->head())->i;
	    int low  = ((ItemINT *) ((ItemList *) item)->tail())->i;
	    add((high << 16) + low);
	 } else
	    cerr << "Warning: Ignoring non-community value " << *item << endl;
}

ostream& operator<<(ostream &os, CommunitySet& cs) {
   for (Pix p = cs.first(); p;) {
      community_print(os, cs(p));
      cs.next(p);
      if (p)
	 os << ", ";
   }
   return os;
}
