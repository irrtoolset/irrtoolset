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
#include "autnum.hh"
#include "rpsl/schema.hh"
#include "irr/irr.hh"
#include "rpsl/prefix.hh"
#include "gnug++/SetOfUInt.hh"
#include <algorithm>

ASt AutNum::asno(void) const
{
   AttrGenericIterator<ItemASNO> itr(this, "aut-num");
   return itr()->asno;
}

void AutNum::removePeer(ASt peerAS)
{
  int newOffset = 0;
  int newLen = 0;
  int copyingLen = 0;
  char *newText = strdup(contents);
  newText[0] = 0;
  // copy the text form of the object 
  // for import policies
  for (AutNumImportIterator itr(this, peerAS); itr; itr++)
    {
    copyingLen = itr()->getOffset() - newOffset;
    memcpy(newText + newLen, contents + newOffset, copyingLen);
    newLen += copyingLen;
    newOffset = itr()->getOffset() + itr()->getLen();
    }
  copyingLen = size - newOffset;
  memcpy(newText + newLen, contents + newOffset, copyingLen);
  newLen += copyingLen;
  newText[newLen] = 0;
  // Discard all parsed form of the object
  attrs.clear();
  // Rescan the object and rebuilt the parsed tree
  free(contents);
  contents = newText;
  size = newLen;
  scan();

  // Same thing applys to the export policy line which means that 
  // another reparse!

  newText = strdup(contents);
  newText[0] = 0;
  newOffset = 0;
  newLen = 0;
  // for export policies
  for (AutNumExportIterator itr(this, peerAS); itr; itr++)
    {
    copyingLen = itr()->getOffset() - newOffset;
    memcpy(newText + newLen, contents + newOffset, copyingLen);
    newLen += copyingLen;
    newOffset = itr()->getOffset() + itr()->getLen();
    }
  copyingLen = size - newOffset;
  memcpy(newText + newLen, contents + newOffset, copyingLen);
  newLen += copyingLen;
  newText[newLen] = 0;
  // Discard all parsed form of the object
  attrs.clear();
  // Rescan the object and rebuilt the parsed tree
  free(contents);
  contents = newText;
  size = newLen;
  scan();
}

void AutNum::gatherPeerings() {
   if (peerings)
      return;

   peerings = new SortedList<Peering>;
   const AttrProtocol *bgp4 = schema.searchProtocol("BGP4");
   
   for (AttrIterator<AttrImport> itr(this, "import"); itr; itr++)
      if (itr()->fromProt == bgp4 && itr()->intoProt == bgp4)
	 gatherPeerings(itr()->policy, peerings);

   for (AttrIterator<AttrExport> itr(this, "export"); itr; itr++)
      if (itr()->fromProt == bgp4 && itr()->intoProt == bgp4)
	 gatherPeerings(itr()->policy, peerings);

   for (AttrIterator<AttrDefault> itr(this, "default"); itr; itr++)
      gatherPeerings(itr()->peering, peerings);
}

void AutNum::gatherPeerings(PolicyExpr *policy, SortedList<Peering> *peerings){
   if (typeid(*policy) == typeid(PolicyTerm)) {
      for (PolicyFactor *pf = ((PolicyTerm *) policy)->head(); 
	   pf; 
	   pf = ((PolicyTerm *) policy)->next(pf)) 
	 for (PolicyPeeringAction *pa = pf->peeringActionList->head(); 
	      pa; 
	      pa = pf->peeringActionList->next(pa))
	    gatherPeerings(pa->peering, peerings);
      return;
   }

   if (typeid(*policy) == typeid(PolicyRefine)) {
      gatherPeerings(((PolicyRefine *) policy)->left, peerings);
      gatherPeerings(((PolicyRefine *) policy)->right, peerings);
      return;
   }

   if (typeid(*policy) == typeid(PolicyExcept)) {
      gatherPeerings(((PolicyExcept *) policy)->left, peerings);
      gatherPeerings(((PolicyExcept *) policy)->right, peerings);
      return;
   }

   assert(0);
}

void AutNum::gatherPeerings(PolicyPeering *peering, 
			    SortedList<Peering> *peerings) {
   if (peering->prngSet) {
   // added by katie@ripe.net
   const PeeringSet *pset = irr->getPeeringSet(peering->prngSet);
   for (AttrIterator<AttrPeering> itr(pset, "peering"); itr; itr++)
   	gatherPeerings(itr->peering, peerings);
   
   } else {
      SetOfUInt *ases = new SetOfUInt;
      SetOfUInt *pRtrs = new SetOfUInt;
      SetOfUInt *lRtrs = new SetOfUInt;
      gatherASes(peering->peerASes, ases);
      gatherRouters(peering->peerRtrs, pRtrs);
      gatherRouters(peering->localRtrs, lRtrs);

      if (!pRtrs->empty() && ! lRtrs->empty())
	 for (Pix as = ases->first(); as; ases->next(as))
	    for (Pix pR = pRtrs->first(); pR; pRtrs->next(pR))
	       for (Pix lR = lRtrs->first(); lR; lRtrs->next(lR))
		  peerings->insertSortedNoDups(new Peering((*ases)(as), 
							   (*pRtrs)(pR), 
							   (*lRtrs)(lR)));
      else if (!pRtrs->empty())
	 for (Pix as = ases->first(); as; ases->next(as))
	    for (Pix pR = pRtrs->first(); pR; pRtrs->next(pR))
	       peerings->insertSortedNoDups(new Peering((*ases)(as), 
							(*pRtrs)(pR), 
							NullIPAddr));
      else if (! lRtrs->empty())
	 for (Pix as = ases->first(); as; ases->next(as))
	    for (Pix lR = lRtrs->first(); lR; lRtrs->next(lR))
	       peerings->insertSortedNoDups(new Peering((*ases)(as), 
							NullIPAddr, 
							(*lRtrs)(lR)));
      else 
	 for (Pix as = ases->first(); as; ases->next(as))
	    peerings->insertSortedNoDups(new Peering((*ases)(as), 
						     NullIPAddr, 
						     NullIPAddr));

      delete ases;
      delete pRtrs;
      delete lRtrs;
   }
}

void AutNum::gatherASes(Filter *f, SetOfUInt *result) {
   if (typeid(*f) == typeid(FilterASNO))
      result->add(((FilterASNO *) f)->asno);
   else if (typeid(*f) == typeid(FilterASNAME)) {
      SymID set = ((FilterASNAME *) f)->asname;
      if (!symbols.isASAny(set) && irr)
	 (*result) |= *irr->expandASSet(set);
   } else if (typeid(*f) == typeid(FilterAND)) {
      SetOfUInt *s1 = new SetOfUInt;
      SetOfUInt *s2 = new SetOfUInt;
      gatherASes(((FilterAND *) f)->f1, s1);
      gatherASes(((FilterAND *) f)->f2, s2);
      (*s1) &= (*s2);
      (*result) |= (*s1);
      delete s1; 
      delete s2;
   } else if (typeid(*f) == typeid(FilterEXCEPT)) {
      SetOfUInt *s1 = new SetOfUInt;
      SetOfUInt *s2 = new SetOfUInt;
      gatherASes(((FilterEXCEPT *) f)->f1, s1);
      gatherASes(((FilterEXCEPT *) f)->f2, s2);
      (*s1) -= (*s2);
      (*result) |= (*s1);
      delete s1; 
      delete s2;
   } else if (typeid(*f) == typeid(FilterOR)) {
      SetOfUInt *s1 = new SetOfUInt;
      SetOfUInt *s2 = new SetOfUInt;
      gatherASes(((FilterOR *) f)->f1, s1);
      gatherASes(((FilterOR *) f)->f2, s2);
      (*s1) |= (*s2);
      (*result) |= (*s1);
      delete s1; 
      delete s2;
   } else
      assert(false);
}

void AutNum::gatherRouters(Filter *f, SetOfUInt *result) {
   if (typeid(*f) == typeid(FilterANY))
      ;
   else if (typeid(*f) == typeid(FilterRouter))
      result->add(((FilterRouter *) f)->ip->get_ipaddr());
   else if (typeid(*f) == typeid(FilterRouterName)) {
      char *dns = ((FilterRouterName *) f)->name;
      if (irr) {
	 const InetRtr *rtr = irr->getInetRtr(dns);
	 if (rtr)
	    for (AttrIterator<AttrIfAddr> itr(rtr, "ifaddr"); itr; itr++)
	       result->add(itr()->ifaddr.get_ipaddr());
      } 
   } else if (typeid(*f) == typeid(FilterRTRSNAME)) {
      SymID set = ((FilterRTRSNAME *) f)->rtrsname;
      if (irr) {
	 const PrefixRanges *rtrSet = irr->expandRtrSet(set);
	 if (rtrSet) 
	    for (Pix p = rtrSet->first(); p; rtrSet->next(p))
	       result->add(((*rtrSet)(p)).get_ipaddr());
      }
   } else if (typeid(*f) == typeid(FilterAND)) {
      SetOfUInt *s1 = new SetOfUInt;
      SetOfUInt *s2 = new SetOfUInt;
      gatherASes(((FilterAND *) f)->f1, s1);
      gatherASes(((FilterAND *) f)->f2, s2);
      (*s1) &= (*s2);
      (*result) |= (*s1);
      delete s1; 
      delete s2;
   } else if (typeid(*f) == typeid(FilterEXCEPT)) {
      SetOfUInt *s1 = new SetOfUInt;
      SetOfUInt *s2 = new SetOfUInt;
      gatherASes(((FilterEXCEPT *) f)->f1, s1);
      gatherASes(((FilterEXCEPT *) f)->f2, s2);
      (*s1) -= (*s2);
      (*result) |= (*s1);
      delete s1; 
      delete s2;
   } else if (typeid(*f) == typeid(FilterOR)) {
      SetOfUInt *s1 = new SetOfUInt;
      SetOfUInt *s2 = new SetOfUInt;
      gatherASes(((FilterOR *) f)->f1, s1);
      gatherASes(((FilterOR *) f)->f2, s2);
      (*s1) |= (*s2);
      (*result) |= (*s1);
      delete s1; 
      delete s2;
   } else
      assert(false);
}
