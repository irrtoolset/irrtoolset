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
#include <ostream>
#include <cstdio>
#include "rpsl_filter.hh"
#include "rpsl_attr.hh"
#include "regexp.hh"

using namespace std;

//// printing ////////////////////////////////////////////////////////

ostream &Filter::print(ostream &out) const {
   return out;
}

ostream &FilterMS::print(ostream &out) const {
   out << *f1;
   switch (code) {
   case 0:
      out << "^-";
      break;
   case 1:
      out << "^+";
      break;
   case 2:
      if (n == m)
	 out << "^" << n;
      else
	 out << "^" << n << "-" << m;
   }
   return out;
}

ostream &FilterOR::print(ostream &out) const {
   out << *f1 << " or " << *f2;
   return out;
}

ostream &FilterAND::print(ostream &out) const {
   out << *f1 << " and " << *f2;
   return out;
}

ostream &FilterEXCEPT::print(ostream &out) const {
   out << *f1 << " except " << *f2;
   return out;
}

ostream &FilterNOT::print(ostream &out) const {
   out << "not " << *f1;
   return out;
}

ostream &FilterASNO::print(ostream &out) const {
   out << "AS" << asno;
   return out;
}

ostream &FilterASNAME::print(ostream &out) const {
   out << asname;
   return out;
}

ostream &FilterRSNAME::print(ostream &out) const {
   out << rsname;
   return out;
}

ostream &FilterRTRSNAME::print(ostream &out) const {
   out << rtrsname;
   return out;
}

ostream &FilterFLTRNAME::print(ostream &out) const {
   out << fltrname;
   return out;
}

ostream &FilterANY::print(ostream &out) const {
   out << "ANY";
   return out;
}

ostream &FilterPeerAS::print(ostream &out) const {
   out << "peerAS";
   return out;
}

ostream &FilterASPath::print(ostream &out) const {
   out << "<" << *re << ">";
   return out;
}

ostream &FilterPRFXList::print(ostream &out) const {
   out << "{";
   int i = low();
   if (i < fence()) {
      out << (*this)[i].get_text();
      for (++i; i < fence(); ++i)
	 out << ", " << (*this)[i].get_text();
   }
   out << "}";

   return out;
}

ostream &FilterMPPRFXList::print(ostream &out) const {
   MPPrefixRanges::const_iterator p;
   out << "{";

   p = begin();
   if (p != end()) {
     out << *p;
     ++p;
     for (; p != end(); ++p) {
       out << ", ";
       out << *p ;
     }
   }

   out << "}";

   return out;
}

FilterPRFXList* FilterMPPRFXList::get_v4() {

   MPPrefixRanges::const_iterator p;
   FilterPRFXList *list_v4 = new FilterPRFXList;

   for (p = begin(); p != end(); ++p) {
     if (p->ipv4)
       list_v4->add_high(*(p->ipv4));
   }
   if (list_v4->isEmpty())
     return NULL;
   else 
     return list_v4;
}


FilterMPPRFXList* FilterMPPRFXList::get_v6() {
   MPPrefixRanges::const_iterator p;
   FilterMPPRFXList *list_v6 = new FilterMPPRFXList;

   for (p = begin(); p != end(); ++p) {
     if (p->ipv6) 
       list_v6->push_back(*p);
   }
   if (list_v6->begin() == list_v6->end())
     return NULL;
   else 
     return list_v6;
}

ostream &FilterV6EXCLUDE::print(ostream &out) const {
   MPPrefixRanges::const_iterator p;
   out << "{";

   for (p = prfxs->begin(); p != prfxs->end(); ++p) {
     out << *p ;
     out << ',';
   }

   out << "}";

   return out;
}

ostream &FilterAFI::print(ostream &out) const {
   //out << (AddressFamily &) *afi_item;
   for (Item *item = afi_list->head(); item; item = afi_list->next(item))
     out << (AddressFamily &) ((ItemAFI &) *item);
     
   out << " ";
   out << *f;

   return out;
}

ostream &FilterV6HAVE_COMPONENTS::print(ostream &out) const {
   MPPrefixRanges::const_iterator p;
   out << "{";

   for (p = prfxs->begin(); p != prfxs->end(); ++p) {
     out << *p ;
     out << ',';
   }

   out << "}";

   return out;
}

ostream &FilterRPAttribute::print(ostream &out) const {
   out << rp_attr->name;
   if (rp_method->isOperator)
      out << " " << (rp_method->name + 8) << " " << *args;
   else
      out << "." << rp_method->name << "(" << *args << ")";

   return out;
}

ostream &FilterHAVE_COMPONENTS::print(ostream &out) const {
   out << "HAVE-COMPONENTS " << *prfxs;
   return out;
}

ostream &FilterEXCLUDE::print(ostream &out) const {
   out << "EXCLUDE " << *prfxs;
   return out;
}

ostream &FilterRouter::print(ostream &out) const {

   out << ip->get_ip_text();

   return out;
}

ostream &FilterRouterName::print(ostream &out) const {
   out << name;
   return out;
}

