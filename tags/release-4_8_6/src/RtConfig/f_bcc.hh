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

#ifndef F_BCC_H
#define F_BCC_H

#include "config.h"
#include "util/List.hh"
#include "normalform/RadixSet.hh"
#include "f_cisco.hh" // included to define ListOf2Ints (should be in util)

class FilterOfCommunity;
class ostream;
class RangeList;
class regexp;
class PolicyAction;
class PolicyActionList;
class PTree;
class ItemList;
class NormalExpression;
class SetOfPrefix;
class regexp_nf;

class BccConfig : public RtConfig {
public:
   BccConfig() : RtConfig() {
      lastMapName[0] = 0;
      routeMapGenerated = false;
      prefixListGenerated = false;
      distributeListNo = -1;
      routeMapID = 1;
   }
   void importP(ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr);
   void exportP(ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr);
   void deflt(ASt as, ASt peerAS);
public:
   // options
   static char mapName[80]; 
   static char mapNameFormat[80];
   static bool forcedInboundMatchIP;
   static bool useAclCaches;
   static bool printRouteMap;
   static bool compressAcls;
   static bool usePrefixLists;
   static int  mapIncrements;
   static int  mapNumbersStartAt;
   static int  mapCount;
   static int  bcc_version;  // following are newly added
   static int  bcc_max_preference;
   static int  bcc_advertise_nets;
   static int  bcc_advertise_all;
   static int  bcc_force_back;
   static int  bcc_max_prefixes; // end of new  

   void printAccessList(SetOfPrefix& nets) {
      RadixSet::SortedPrefixRangeIterator *next_itrp = NULL;
      printRoutes(&next_itrp, nets, "network", "match Exact");
   }
private:
  char lastMapName[80];
  bool routeMapGenerated;
  bool prefixListGenerated;
  int  distributeListNo;
  int  routeMapID;
  bool reSplittable;



private:
   static bool  firstCommunityList;
   ListOf2Ints *printRoutes(RadixSet::SortedPrefixRangeIterator **itrp,SetOfPrefix& nets, char *prefix, char *suffix);// new
   int         print(NormalExpression *ne, PolicyActionList *actn, 
		     ASt asno, ASt peerAS, IPAddr *peer_addr, 
		     int import_flag); 

};


extern AccessListManager<regexp_nf>         aspathMgr;
extern AccessListManager<SetOfPrefix>       prefixMgr;
extern AccessListManager<SetOfPrefix>       pktFilterMgr;
extern AccessListManager<FilterOfCommunity> communityMgr;

#endif   // F_BCC_H




