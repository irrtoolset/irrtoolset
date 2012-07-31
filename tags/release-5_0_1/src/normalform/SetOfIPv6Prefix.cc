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

#pragma implementation

#include "config.h"
#include "irrutil/debug.hh"
#include "SetOfIPv6Prefix.hh"
#include <cstdio>
#include <cassert>
#include <ostream>

using namespace std;

void SetOfIPv6Prefix::insert(const MPPrefixRanges& b) { 
    ipv6_addr_t rngs;
    MPPrefixRanges::const_iterator p;
    
   if (_universal)
      return;

    //iterator
    for (p = b.begin(); p != b.end(); ++p) {
      rngs = p->get_range();

      if (not_)
         members.remove(p->get_ipaddr(), p->get_length(), rngs); 
      else
         members.insert(p->get_ipaddr(), p->get_length(), rngs); 
   }

}

void SetOfIPv6Prefix::remove(const MPPrefixRanges& b) { 
   ipv6_addr_t rngs;
   MPPrefixRanges::const_iterator p;

   if (_universal) {
      _universal = 0;
      not_ = 1;
   }

   for (p = b.begin(); p != b.end(); ++p) {
      rngs = p->get_range();
      if (not_)
         members.insert(p->get_ipaddr(), p->get_length(), rngs);
      else
         members.remove(p->get_ipaddr(), p->get_length(), rngs);
   }

}

void SetOfIPv6Prefix::operator= (const MPPrefixRanges& b) {
   clear();
   insert(b);
}

void SetOfIPv6Prefix::do_print (ostream& stream) {
   stream << *this;
}

ostream& operator<<(ostream& stream, SetOfIPv6Prefix& set) {
   if (set._universal)
      stream << "UNIVERSAL";
   else  {
      if (set.not_)
      	 stream << "NOT";

      stream << set.members;
   }
   return stream;
}

void SetOfIPv6Prefix::operator |= (const SetOfIPv6Prefix& b) { // union
   IPv6RadixSet c;

   if (_universal || b.isEmpty())
      ; // done
   else
      if (b._universal) {
	 members.clear();
	 _universal = 1;
	 not_ = 0;
      } else {
	 switch (not_*2+b.not_) {
	 case 0: // not = 0, b.not = 0 => this UNION b = members UNION b.members
	    members |= b.members;
	    break;
	 case 1: // not = 0, b.not = 1 => this UNION b = members MINUS b.members
	    c = members;
	    members = b.members;
	    members -= c;
	    not_ = 1;
 	    break;
	 case 2: // not = 1, b.not = 0 => this UNION b = b.members MINUS members
	    members -= b.members;
 	    break;
	 case 3: // not = 1, b.not = 1 => this UNION b = members INTERSECTION b.members
	    members &= b.members;
	    break;
	 default:
	    assert(0);
	 }
	 if (isEmpty() && not_) { // case 1,2,3 can cause this
	    _universal = 1;
	    not_ = 0;
	 }
      }
}

void SetOfIPv6Prefix::operator &= (const SetOfIPv6Prefix& b) { // intersection
   IPv6RadixSet c;

   if (isEmpty() || b._universal)
      ; // done
   else
      if (b.isEmpty()) {
	 members.clear();
	 _universal = 0;
	 not_ = 0;
      } else 
	 if (_universal) {
	    _universal = b._universal;
	    not_ = b.not_;
	    members = b.members;
	 } else {
	    switch (not_*2+b.not_) {
	    case 0: // not = 0, b.not = 0 => this INTSCTN b = members INTSCTN b.members
	       members &= b.members;
	       break;
	    case 1: // not = 0, b.not = 1 => this INTSCTN b = members MINUS b.members
	       members -= b.members;
	       break;
	    case 2: // not = 1, b.not = 0 => this INTSCTN b = b.members MINUS members
	       c = members;
	       members = b.members;
	       members -= c;	    
	       not_ = 0;
	       break;
	    case 3: // not = 1, b.not = 1 => this INTSCTN b = members UNION b.members
	       members |= b.members;
	       break;
	    default:
	       assert(0);
	    }
	 }
}
void SetOfIPv6Prefix::operator ~  () { // complement
   if (_universal) {
      assert(members.isEmpty());
      _universal = 0;
   } else 
      if (isEmpty()) {
	 assert(! not_);
	 _universal = 1;
      } else
	 not_ = ! not_;
}
int  SetOfIPv6Prefix::operator == (const SetOfIPv6Prefix& other) const { // equivalance
   return (_universal == other._universal 
	   && not_ == other.not_
	   && members == other.members);
}

void  SetOfIPv6Prefix::operator = (const SetOfIPv6Prefix& other) { // assignment
   members.clear();
   not_ = other.not_;
   _universal = other._universal;
   members = other.members;
}
