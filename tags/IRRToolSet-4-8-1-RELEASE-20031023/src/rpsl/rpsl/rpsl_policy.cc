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
#include <cstdio>
#include "rpsl_policy.hh"
#include "rpsl_attr.hh"

//// printing ////////////////////////////////////////////////////////

ostream& Policy::print(ostream &out) const {
   return out;
}

ostream &PolicyAction::print(ostream &out) const {
   out << rp_attr->name;
   if (rp_method->isOperator)
      out << " " << (rp_method->name + 8) << " " << *args;
   else
      out << "." << rp_method->name << "(" << *args << ")";

   return out;
}

ostream &PolicyActionList::print(ostream &out) const {
   if (!isEmpty()) {
      out << "action ";
      for (PolicyAction *nd = head(); nd; nd = next(nd))
	 out << *nd << "; ";
   }
      
   return out;
}

ostream &PolicyPeeringAction::print(ostream &out) const {
   out << *peering << "\n";
   if (!action->isEmpty())
      out << "       \t" << *action << "\n";
   return out;
}

ostream &PolicyFactor::print(ostream &out) const {
   for (PolicyPeeringAction *pa = peeringActionList->head();
	pa;
	pa = peeringActionList->next(pa))
      out << "\tfrom/to " << *pa << "\n";

   out << "       \taccept/announce " << *filter <<";";

   return out;
}

ostream &PolicyTerm::print(ostream &out) const {
   bool indent = !isEmpty() && !isSingleton();
   if (indent) 
      out << "{\n";

   for (PolicyFactor *pf = head(); pf; pf = next(pf)) {
      if (indent)
	 out << "   ";
      out << *pf << "\n";
   }
   if (indent) 
      out << "}";

   return out;
}

ostream &PolicyRefine::print(ostream &out) const {
   out << *left << " refine " << *right;
   return out;
}
   
ostream &PolicyExcept::print(ostream &out) const {
   out << *left << " except " << *right;
   return out;
}
   

ostream &PolicyPeering::print(ostream &out) const {
   if (prngSet)
      out << prngSet;
   else {
      out << *peerASes;
      if (peerRtrs)
	 out << " " << *peerRtrs;
      if (localRtrs)
	 out << " at " << *localRtrs;
   }

   return out;
}

