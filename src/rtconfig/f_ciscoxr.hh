//  $Id: f_cisco.hh 319 2010-03-19 22:19:39Z nick $
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

#ifndef F_CISCOXR_H
#define F_CISCOXR_H


#include "f_cisco.hh"
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
class SetOfIPv6Prefix;
class regexp_nf;

//class ListNodeOf2Ints : public ListNode {
//public:
//   int start;
//   int end;
//   int flag;    // flag is used for exact community matching
//
//   ListNodeOf2Ints() {}
//   ListNodeOf2Ints(int s, int e, int f) : start(s), end(e), flag(f) {}
//   ListNodeOf2Ints(const ListNodeOf2Ints& b) : 
//      start(b.start), end(b.end), flag(b.flag) {}
//};
//
//class ListOf2Ints : public List<ListNodeOf2Ints> {
//public:
//   ListOf2Ints(int flag) : List<ListNodeOf2Ints>() {
//      // create a list whose end is less than its start
//      // so it wont print anything!
//      add(2,1);
//   }
//  ListOf2Ints() : List<ListNodeOf2Ints>() {}
//  void add(int s, int e, int flag = 0) {
//     append((new ListNodeOf2Ints(s, e, flag)));
//  }
//};

class CiscoXRConfig : public RtConfig {
public:
   CiscoXRConfig() : RtConfig() {
      lastMapName[0] = 0;
      routeMapGenerated = false;
      prefixListGenerated = false;
      distributeListNo = -1;
      routeMapID = 1;
   }
   // REIMPLEMENTED
   void importP(ASt as, MPPrefix* addr, ASt peerAS, MPPrefix* peerAddr);
   void exportP(ASt as, MPPrefix* addr, ASt peerAS, MPPrefix* peerAddr);
   void exportGroup(ASt as, char *pset);
   void importGroup(ASt as, char *pset);
   void deflt(ASt as, ASt peerAS);
   void static2bgp(ASt as, MPPrefix* addr);
   void networks(ASt as);
   void IPv6networks(ASt as);
   void inboundPacketFilter(char *ifname, ASt as, MPPrefix* addr, ASt peerAS, MPPrefix* peerAddr);
   void outboundPacketFilter(char *ifname, ASt as, MPPrefix* addr, ASt peerAS, MPPrefix* peerAddr);

public:
   // options
   static char mapName[80]; 
   static char mapNameFormat[80];
   static bool forcedInboundMatchIP;
   static bool useAclCaches;
   static bool printRouteMap;
   static bool compressAcls;
   static bool usePrefixLists;
   static bool eliminateDupMapParts;
   static bool forceTilda;
   static bool emptyLists;
   static int  mapIncrements;
   static int  mapNumbersStartAt;
   static int  mapCount;

private:
   char lastMapName[80];
   bool routeMapGenerated;
   bool prefixListGenerated;
   int  distributeListNo;
   int  routeMapID;
   bool reSplittable;
   bool hasTilda;
   bool inTilda;

private:
   static bool  firstCommunityList;
   const char  *returnPermitOrDeny(bool);
   ListOf2Ints *printRoutes(SetOfPrefix& nets);
   ListOf2Ints *printRoutes(SetOfIPv6Prefix& nets);
   ListOf2Ints *printPrefixList(SetOfPrefix& nets);
   ListOf2Ints *printPrefixList(SetOfIPv6Prefix& nets);
   ListOf2Ints *printCommunities(FilterOfCommunity& cm);
   ListOf2Ints *printASPaths(regexp_nf& path);
   void         printREASno(std::ostream& out, const RangeList &no);
   int          printRE_(std::ostream& os, const regexp& r);
   void         printRE(std::ostream& os, const regexp& r, int aclID, bool permit);

   int          printPacketFilter(SetOfPrefix &set);
   int          printPacketFilter(SetOfIPv6Prefix &set);
   inline void  printCommunity(std::ostream &os, unsigned int i);
   void         printCommunityList(std::ostream &os, ItemList *args);
   void         printActions(std::ostream &os, PolicyActionList *action, ItemAFI *afi);
   int          print(NormalExpression *ne, PolicyActionList *actn, int import_flag, ItemAFI *afi);
   bool         printNeighbor(int import, ASt asno, ASt peerAS, char *neighbor, bool peerGroup, ItemAFI *peer_afi, ItemAFI *filter_afi);
   void printAccessList(SetOfPrefix& nets) {
      bool save = useAclCaches;
      useAclCaches = false;
      printRoutes(nets);
      useAclCaches = save;
   }
   void printAccessList(SetOfIPv6Prefix& nets) {
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
};

//template <class T>
//class AccessListManager {
//private :
//   class Node : public ListNode {
//   public:
//      Node()              : data(),       lof2i()        {}
//      Node(const Node &b) : data(b.data), lof2i(b.lof2i) {}
//      Node(T& _data)      : data(_data),  lof2i()        {}
//
//   public:
//      T data;
//      ListOf2Ints lof2i;
//   };
//
//   List<Node> cache;
//   int next_id;
//
//public:
//   AccessListManager() : cache(), next_id(1) {
//   }
//
//   AccessListManager(int firstID) : cache(), next_id(firstID) {
//   }
//
//   ListOf2Ints* search(T& _data) {
//      for (Node *p = cache.head(); p; p = cache.next(p))
//	 if (p->data == _data)
//	    return &(p->lof2i);
//      return NULL;
//   }
//
//   ListOf2Ints* add(T& _data) {
//      Node *p = new Node(_data);
//      cache.append(p);
//      return &(p->lof2i);
//   }
//
//   void setNextID(int id) {
//      next_id = id;
//   }
//
//   int newID() {
//      return next_id++;
//   }
//   int lastID() {
//      return next_id - 1;
//   }
//};

extern AccessListManager<regexp_nf>         aspathMgr;
extern AccessListManager<SetOfPrefix>       prefixMgr;
extern AccessListManager<SetOfIPv6Prefix>   ipv6prefixMgr;
extern AccessListManager<SetOfPrefix>       pktFilterMgr;
extern AccessListManager<SetOfIPv6Prefix>   ipv6pktFilterMgr;
extern AccessListManager<FilterOfCommunity> communityMgr;


#endif   // F_CISCOXR_H
