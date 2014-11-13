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

#ifndef F_JUNOS_H
#define F_JUNOS_H

#include "config.h"
#include "rpsl/List.hh"
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
   static bool load_replace;
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

   const char  *returnPermitOrDeny(bool);
   inline void  printCommunity(std::ostream &os, unsigned int i);
   int          printCommunityList(std::ostream &os, ItemList *args);
   int          printCommunitySet(std::ostream &os, CommunitySet *set, bool exact);
   void         printActions(std::ostream &os, PolicyActionList *action, ItemAFI *afi,std::ostringstream &lastCout);
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
extern AccessListManager<CommunitySet>      communityMgr2;

#endif   // F_JUNOS_H
