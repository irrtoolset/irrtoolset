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
//  irrtoolset@ripe.net.
//
//  Author(s): Katie Petrusha <katie@ripe.net>

#pragma interface


#include "config.h"
#include <iostream.h>
#include <cassert>
#include "IPv6RadixSet.hh"
#include "rpsl/prefix.hh"

// the following set class can perform set complement operation 
// without knowing the universal set

class SetOfIPv6Prefix {
   friend class RtConfig;
   friend class CiscoConfig;
   friend class JunosConfig;
   friend class GatedConfig;
   friend class RsdConfig;
   friend class BccConfig;

public:
   SetOfIPv6Prefix() : members() {
      _universal = 0;
      not_ = false;
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

   friend ostream& operator<<(ostream& stream, SetOfIPv6Prefix& nt);
   virtual void do_print (ostream& stream);

   int length();

   int contains(ipv6_addr_t i);

   void makeMoreSpecific(int code, int n, int m) {
      members.makeMoreSpecific(code, n, m);
      if (members.isEmpty())
	 clear();
   }

private:
   IPv6RadixSet        members;	// elements if not = false
   unsigned char    _universal;  // true if set equals universal set
   bool            not_;         // if true, set = not members

};


