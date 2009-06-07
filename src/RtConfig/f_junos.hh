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

#ifndef F_JUNOS_H
#define F_JUNOS_H

#include "config.h"
#include "util/List.hh"
#include "normalform/FilterOfASPath.hh"
#include "irr/irr.hh"
#include "irr/autnum.hh"

#include <sstream>

class FilterOfCommunity;
class RangeList;
class regexp;
class PolicyAction;
class PolicyActionList;
class PTree;
class ItemList;
class NormalExpression;
class SetOfPrefix;
class regexp_nf;
class CommunitySet;

class JunosConfig : public RtConfig {
public:
   JunosConfig() : RtConfig() {
      lastMapName[0] = 0;
      distributeListNo = -1;
      routeMapID = 1;
   }
   void importP(ASt as, MPPrefix* addr, ASt peerAS, MPPrefix* peerAddr);
   void exportP(ASt as, MPPrefix* addr, ASt peerAS, MPPrefix* peerAddr);
   void exportGroup(ASt as, char *pset);
   void importGroup(ASt as, char *pset);
   void static2bgp(ASt as, MPPrefix* addr);
   void networks(ASt as);
   void IPv6networks(ASt as);

public:
   // options
   static char mapName[80]; 
   static char mapNameFormat[80];
   static bool useAclCaches;
   static bool printRouteMap;
   static bool compressAcls;
   static bool usePrefixLists;
   static int  mapIncrements;
   static int  mapNumbersStartAt;
   static int  mapCount;
   static bool setMEDtoIGP;
   static bool exportStatics;

private:
   char lastMapName[80];
   int  distributeListNo;
   int  routeMapID;
   bool reSplittable;

private:
   ListOf2Ints *printRoutes(SetOfIPv6Prefix& nets);
   ListOf2Ints *printRoutes(SetOfPrefix& nets);
   ListOf2Ints *printCommunities(FilterOfCommunity& cm);
   ListOf2Ints *printASPaths(FilterOfASPath& path);

   const char  *returnPermitOrDeny(int, bool);
   inline void  printCommunity(std::ostream &os, unsigned int i);
   int          printCommunityList(std::ostream &os, ItemList *args);
   int          printCommunitySet(std::ostream &os, CommunitySet *set, bool exact);
   void         printActions(std::ostream &os, PolicyActionList *action, ItemAFI *afi);
   void         printMartians();
   int          print(NormalExpression *ne, PolicyActionList *actn, int import_flag, ItemAFI *afi);
   int          printDeclarations(NormalExpression *ne, PolicyActionList *actn, int import_flag);
   bool         printNeighbor(int import, ASt asno, ASt peerAS, char *neighbor, 
			      bool peerGroup, ItemAFI *peer_afi, ItemAFI *filter_afi);
   void printAccessList(SetOfIPv6Prefix& nets) {
      bool save = useAclCaches;
      useAclCaches = false;
      printRoutes(nets);
      useAclCaches = save;
   }
   void printAccessList(SetOfPrefix& nets) {
      bool save = useAclCaches;
      useAclCaches = false;
      printRoutes(nets);
      useAclCaches = save;
   }
   void printAspathAccessList(FilterOfASPath& path) {
      bool save = useAclCaches;
      useAclCaches = false;
      printASPaths(path);
      useAclCaches = save;
   }
   void printREASno(std::ostream& out, const RangeList &no);
   int printRE_(std::ostream& os, const regexp& r, bool &hasBOL, bool &hasEOL);
   int printRE(std::ostream& os, const regexp& r);
};

extern AccessListManager<regexp_nf>         aspathMgr;
extern AccessListManager<SetOfPrefix>       prefixMgr;
extern AccessListManager<SetOfIPv6Prefix>   ipv6prefixMgr;
extern AccessListManager<SetOfPrefix>       pktFilterMgr;
extern AccessListManager<FilterOfCommunity> communityMgr;

#endif   // F_JUNOS_H
