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
//  irrtoolset@ripe.net.
//
//  Author(s): Katie Petrusha <katie@ripe.net>

#pragma interface


#include "config.h"
#include <iostream>
#include <cassert>
#include "IPv6RadixSet.hh"
#include "rpsl/prefix.hh"
#include "rpsl/rpsl_item.hh"

// the following set class can perform set complement operation 
// without knowing the universal set

class SetOfIPv6Prefix {
   friend class RtConfig;
   friend class CiscoConfig;
   friend class CiscoXRConfig;
   friend class JunosConfig;

public:
   SetOfIPv6Prefix() : members() {
      _universal = 0;
      not_ = false;
   };
   SetOfIPv6Prefix(const SetOfIPv6Prefix &b) {
      members = IPv6RadixSet(b.members);
      _universal = b._universal;
      not_ = b.not_;
   };
   ~SetOfIPv6Prefix() {
      members.clear();
   };

   virtual int isEmpty() const {
      return (! _universal && members.isEmpty());
   }

   virtual int universal() {
      return (_universal);
   }
   virtual bool negated() {
      return (not_);
   }

   virtual void make_universal() {
      _universal = 1;
      not_ = false;
      members.clear();
   }

   virtual void clear() {
      members.clear();
      _universal = 0;
      not_ = false;
   }

   virtual void operator ~  (); // complement
   void operator |= (const SetOfIPv6Prefix& b); // union
   void operator &= (const SetOfIPv6Prefix& b); // intersection
   int  operator == (const SetOfIPv6Prefix& b) const; // equivalance
   void operator =  (const SetOfIPv6Prefix& b); // assignment

   void operator =  (const MPPrefixRanges& b);
   void operator |= (const MPPrefixRanges& b) {
      insert(b);
   }

   void insert(const MPPrefixRanges& b);
   void remove(const MPPrefixRanges& b);

   friend std::ostream& operator<<(std::ostream& stream, SetOfIPv6Prefix& nt);
   virtual void do_print (std::ostream& stream);

   int length();

   int contains(ipv6_addr_t i);

   void makeMoreSpecific(int code, int n, int m) {
      members.makeMoreSpecific(code, n, m);
      if (members.isEmpty())
	 clear();
   }


     void restrict(ItemList *afi_list) {
     // create multicast set
     SetOfIPv6Prefix *multicast = new SetOfIPv6Prefix();
     multicast->members.insert(*(MulticastIPv6PrefixRange.get_ipaddr()),
                              MulticastIPv6PrefixRange.get_length(),
                              MulticastIPv6PrefixRange.get_range()
                              );
     SetOfIPv6Prefix *unicast = new SetOfIPv6Prefix(*multicast);
     ~*unicast;

     SetOfIPv6Prefix *res = new SetOfIPv6Prefix();
     
     for (Item *afi_item = afi_list->head(); afi_item; afi_item = afi_list->next(afi_item)) {
       if (((ItemAFI *) afi_item)->is_Matching("ipv6.multicast")) {
         SetOfIPv6Prefix *set = new SetOfIPv6Prefix(*this);
         *set &= *multicast;   
         *res |= *set;
         delete set;
       }
       else if (((ItemAFI *) afi_item)->is_Matching("ipv6.unicast")) {
         SetOfIPv6Prefix *set = new SetOfIPv6Prefix(*this);
         *set &= *unicast;
         *res |= *set;
         delete set;
       }
       // other afi's - do nothing
     }

     this->clear();
     *this = *res;
     delete res;
     delete multicast;
     delete unicast;

   }


private:
   IPv6RadixSet        members;	// elements if not = false
   unsigned char    _universal;  // true if set equals universal set
   bool            not_;         // if true, set = not members

};


