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

#ifndef F_CISCO_H
#define F_CISCO_H

#include "config.h"
#include "util/List.hh"
#include "normalform/FilterOfASPath.hh"

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

class ListNodeOf2Ints : public ListNode {
public:
   int start;
   int end;
   int flag;    // flag is used for exact community matching

   ListNodeOf2Ints() {}
   ListNodeOf2Ints(int s, int e, int f) : start(s), end(e), flag(f) {}
   ListNodeOf2Ints(const ListNodeOf2Ints& b) : 
      start(b.start), end(b.end), flag(b.flag) {}
};

class ListOf2Ints : public List<ListNodeOf2Ints> {
public:
   ListOf2Ints(int flag) : List<ListNodeOf2Ints>() {
      // create a list whose end is less than its start
      // so it wont print anything!
      add(2,1);
   }
   ListOf2Ints() : List<ListNodeOf2Ints>() {}
   void add(int s, int e, int flag = 0) {
      append((new ListNodeOf2Ints(s, e, flag)));
   }
};

class CiscoConfig : public RtConfig {
public:
   CiscoConfig() : RtConfig() {
      lastMapName[0] = 0;
      routeMapGenerated = false;
      prefixListGenerated = false;
      distributeListNo = -1;
      routeMapID = 1;
   }
   void importP(ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr);
   void exportP(ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr);
   void exportGroup(ASt as, char *pset);
   void importGroup(ASt as, char *pset);
   void deflt(ASt as, ASt peerAS);
   void static2bgp(ASt as, IPAddr* addr);
   void networks(ASt as);
   void packetFilter(char *ifname, ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr);
   void outboundPacketFilter(char *ifname, ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr);

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
   ListOf2Ints *printRoutes(SetOfPrefix& nets);
   ListOf2Ints *printPrefixList(SetOfPrefix& nets);
   ListOf2Ints *printCommunities(FilterOfCommunity& cm);
   ListOf2Ints *printASPaths(regexp_nf& path);
   void         printREASno(ostream& out, const RangeList &no);
   int          printRE_(ostream& os, const regexp& r);
   void         printRE(ostream& os, const regexp& r, int aclID, bool permit);

   int          printPacketFilter(SetOfPrefix &set);
   inline void  printCommunity(ostream &os, unsigned int i);
   void         printCommunityList(ostream &os, ItemList *args);
   void         printActions(ostream &os, PolicyActionList *action);
   int          print(NormalExpression *ne, PolicyActionList *actn, int import_flag);
   bool         printNeighbor(int import, ASt asno, char *neighbor, 
			      bool peerGroup);
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
};

template <class T>
class AccessListManager {
private :
   class Node : public ListNode {
   public:
      Node()              : data(),       lof2i()        {}
      Node(const Node &b) : data(b.data), lof2i(b.lof2i) {}
      Node(T& _data)      : data(_data),  lof2i()        {}

   public:
      T data;
      ListOf2Ints lof2i;
   };

   List<Node> cache;
   int next_id;

public:
   AccessListManager() : cache(), next_id(1) {
   }

   AccessListManager(int firstID) : cache(), next_id(firstID) {
   }

   ListOf2Ints* search(T& _data) {
      for (Node *p = cache.head(); p; p = cache.next(p))
	 if (p->data == _data)
	    return &(p->lof2i);
      return NULL;
   }

   ListOf2Ints* add(T& _data) {
      Node *p = new Node(_data);
      cache.append(p);
      return &(p->lof2i);
   }

   void setNextID(int id) {
      next_id = id;
   }

   int newID() {
      return next_id++;
   }
   int lastID() {
      return next_id - 1;
   }
};

extern AccessListManager<regexp_nf>         aspathMgr;
extern AccessListManager<SetOfPrefix>       prefixMgr;
extern AccessListManager<SetOfPrefix>       pktFilterMgr;
extern AccessListManager<FilterOfCommunity> communityMgr;

#endif   // F_CISCO_H
