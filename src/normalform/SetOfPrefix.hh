// $Id$
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
//  Author(s): Katie Petrusha <katie@ripe.net>

#pragma interface

#ifndef SetOfPrefix_H
#define SetOfPrefix_H

#include "config.h"
#include <iostream>
#include <cassert>
#include "RadixSet.hh"
#include "gnu/prefixranges.hh"
#include "rpsl/rpsl_item.hh"

// the following set class can perform set complement operation 
// without knowing the universal set
class SetOfPrefix {
   friend class RtConfig;
   friend class CiscoConfig;
   friend class JunosConfig;
   friend class GatedConfig;
   friend class RsdConfig;
   friend class BccConfig;

public:
   SetOfPrefix() : members() {
      _universal = 0;
      not_ = false;
   };
   ~SetOfPrefix() {
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
   void operator |= (const SetOfPrefix& b); // union
   void operator &= (const SetOfPrefix& b); // intersection
   int  operator == (const SetOfPrefix& b) const; // equivalance
   void operator =  (const SetOfPrefix& b); // assignment

   void operator =  (const PrefixRanges& b);
   void operator |= (const PrefixRanges& b) {
      insert(b);
   }

   void insert(const PrefixRanges& b);
   void remove(const PrefixRanges& b);

   friend std::ostream& operator<<(std::ostream& stream, SetOfPrefix& nt);
   virtual void do_print (std::ostream& stream);

   int length();

   int contains(int i);

   void makeMoreSpecific(int code, int n, int m) {
      members.makeMoreSpecific(code, n, m);
      if (members.isEmpty())
	 clear();
   }

   void restrict(ItemList *afi_list) {
     // create multicast set
     SetOfPrefix *multicast = new SetOfPrefix();
     multicast->members.insert(MulticastPrefixRange.get_ipaddr(), 
                              MulticastPrefixRange.get_length(),
                              MulticastPrefixRange.get_range()
                              );
     SetOfPrefix *unicast = new SetOfPrefix(*multicast);
     ~*unicast;

     SetOfPrefix *res = new SetOfPrefix();
     
     for (Item *afi_item = afi_list->head(); afi_item; afi_item = afi_list->next(afi_item)) {
       if (((ItemAFI *) afi_item)->is_Matching("ipv4.multicast")) {
         SetOfPrefix *set = new SetOfPrefix(*this);
         *set &= *multicast;   
         *res |= *set;
         delete set;
       } else if (((ItemAFI *) afi_item)->is_Matching("ipv4.unicast")) {
         SetOfPrefix *set = new SetOfPrefix(*this);
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
   RadixSet        members;	// elements if not = false
   unsigned char    _universal;  // true if set equals universal set
   bool            not_;         // if true, set = not members

};


#endif // SetOfPrefix_H
