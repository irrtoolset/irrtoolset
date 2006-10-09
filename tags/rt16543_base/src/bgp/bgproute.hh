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
//  and/or the International Business Machines Corporation.
//  All rights reserved.
//
//  Permission to use, copy, modify, and distribute this software and
//  its documentation in source and binary forms for lawful
//  non-commercial purposes and without fee is hereby granted, provided
//  that the above copyright notice appear in all copies and that both
//  the copyright notice and this permission notice appear in supporting
//  documentation, and that any documentation, advertising materials,
//  and other materials related to such distribution and use acknowledge
//  that the software was developed by the University of Southern
//  California, Information Sciences Institute and/or the International
//  Business Machines Corporation.  The name of the USC or IBM may not
//  be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  NEITHER THE UNIVERSITY OF SOUTHERN CALIFORNIA NOR INTERNATIONAL
//  BUSINESS MACHINES CORPORATION MAKES ANY REPRESENTATIONS ABOUT
//  THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  THIS SOFTWARE IS
//  PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
//  INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, TITLE, AND 
//  NON-INFRINGEMENT.
//
//  IN NO EVENT SHALL USC, IBM, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT,
//  TORT, OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH,
//  THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//  Questions concerning this software should be directed to 
//  ratoolset@isi.edu.
//
//  Author(s): Cengiz Alaettinoglu <cengiz@isi.edu>
//             John Mehringer <mehringe@isi.edu>

#ifndef BGPROUTE_H
#define BGPROUTE_H

#include "config.h"
#include "util/List.hh"
#include "irr/irr.hh"
#include "gnu/prefixranges.hh"

#include "bgp/actiondictionary.hh"
#include "irr/autnum.hh"
#include "rpsl/rpsl_item.hh"

#define MAX_COMMUNITIES 64
#define NO_LIMIT	0

//-------------------------------------------------------
// class NLRI
//
//  Network Layer Reachability Information
//-------------------------------------------------------
class NLRI {
private:
  bool _dontcare;
  Prefix * prefix;

public:
   NLRI();
   NLRI(char * new_prefix); // finish this
   NLRI(Prefix * new_prefix);
   NLRI(const NLRI &original);
   ~NLRI();
   
   Prefix * getPrefix();
   char * get_text() const;
   
   bool is_dontcare() const {
      return _dontcare;
   }

   void dontcare() {
      _dontcare = true;
   }
};

//-------------------------------------------------------
// class Origin
//
//-------------------------------------------------------
class Origin {
private:
   int size;
//   List<ItemASNO> * getRouteOrigins(char * prefix, IRR * whois);
   
public:
   List<ItemASNO> list;
   
   
   List<ItemASNO> * getRouteOrigins(char * prefix, IRR * whois);

   Origin();
   Origin(char * prefix, IRR * whois);
   Origin(ASt single_origin);
   Origin(const Origin & original);
   void clear();
   void add(ASt as);
   void prepend(ASt as);
   void append(ASt as);
   bool contains(ASt as);
   int getSize();
};


//-------------------------------------------------------
// class ASPath
//
//  list of ASes from one AS to 
//  another
//-------------------------------------------------------
class ASPath {
private:
   bool _dontcare;
   int max_length;
   
public:
   List<ItemASNO> path_list;

   ASPath();
   ASPath(int max);
   ASPath(ASt new_as, int max);  // Create a new ASPath with origin 
                                 // as the first AS in the list
   
   ASPath(const ASPath & original);  // Creates a new ASPath which is a 
                                     // duplicate of original 
   ~ASPath();
   void clear();               
                                
   bool loopCheck(ASt as);
   
   int getPathLength() const;
   int getMaxPathLength() const;
   
   void append(ASt as);	// add an AS to the ASPath
   void prepend(ASt as);
   void remove(ASt as);
   
   void print(ASt first);
   bool atMaxRouteLength();
   bool contains(ASt as);

};

//-------------------------------------------------------
// class Pref
//
//  Route Preference
//  Smaller is better
//-------------------------------------------------------
class Pref {
 public:
   int  pref;

 public:
   Pref() {
      clear();
   }

   void clear() {
      pref = 0;
   }

   void set(int p) {
      pref = p;
   }
};


//-------------------------------------------------------
// class Med
//
//  Multi Exit Discriminator
//-------------------------------------------------------
class Med {
 public:
   int     med;

 public:
   Med() {
      clear();
   }

   void clear() {
      med = 0;
   }

   void set(int m) {
      med = m;
   }
};

//-------------------------------------------------------
// class Dpa
//
//  Destination Preference Attribute
//-------------------------------------------------------
class Dpa {
 public:
   ASt asno;
   int pref;
   
   int dpa;

 public:
   Dpa () {
      clear();
   }

   void clear() {
      asno = 0;
      pref = 0;
   }

   void set (ASt asn, int p) {
      asno = asn;
      pref = p;
   }
};


//-------------------------------------------------------
// class BGPCommunity
//
//  Community for BGP routing 
//  purposes
//-------------------------------------------------------
class BGPCommunity : public ListNode {
public:
  int id;
  
  BGPCommunity(int i);
  BGPCommunity(const BGPCommunity &original);

};

class BGPCommunitySet {
private:
 // int index;
 // int max_size;
  
  int parseItem(Item * community_entry) const;
  
public:
  List<BGPCommunity> community_set;

  BGPCommunitySet();
  BGPCommunitySet(const BGPCommunitySet &original);
  ~BGPCommunitySet();
  
  void clear();
  void add(int new_member);
  void add(BGPCommunity * new_member);
  void add(Item * new_member);
  int getEntry(int i) const;
  bool contains(int test_member) const;
  bool contains(Item * test_member) const;
  bool containsSubset(const FilterRPAttribute * subset);
  bool equalsSet(const FilterRPAttribute * test_set);
};




//-------------------------------------------------------
//  class BGPRoute
//
//  BGPRoute is used to examing routing policy for a given 
//  path. It works by starting at the
//  destination and working its way back to to the source.
//  This is accomplished by comparing the source NLRI to 
//  the filters with in an AS and examining the next AS to
//  be taken in the path.  Each new AS is then searched back 
//  to the source.
//-------------------------------------------------------
class BGPRoute {
public:
   NLRI * nlri;  // Source Address for BGPTrace                   
   Origin * origin;    // Origin ASes of nlri
   ASt source;
   ASPath * aspath;				// Keeps track of visited ASes 
   ASPath * action_aspath;    // visited ASes & actions which modify the path
   BGPCommunitySet * bgpcommunityset;
   Pref          pref;
   Med           med;
   Dpa           dpa;

public:
   BGPRoute();
   BGPRoute(ASt source_as, Prefix * prefix, IRR * irr);
   BGPRoute(ASt source_as, ASt origin_as);
   BGPRoute(const BGPRoute &bgproute);
   
   ~BGPRoute();
   
   void print();
   bool atMaxRouteLength();
   bool loopCheck(ASt as);
   void add(ASt as);  // Add AS to aspath & action_aspath
   void remove(ASt as); // Remove AS from aspath & action_aspath
   
   bool evaluate(const PrefixRanges * prefixranges);  // Check to see if nlri is in 
                                                      // a set of PrefixRanges
   bool evaluate(const FilterPRFXList * prefixranges);  // same as above  
           
   bool evaluate(const FilterRPAttribute * filter);  // Evaluates a community to see if the 
                                                     // bgpcommunity is equal to or subset of
   
   bool isMatching(ASt PeerAS, Filter * filter, IRR * whois);  // Checks to see if filter 
                                                                          // matches origin
   void executeAction(PolicyActionList * action_list, ActionDictionary * type);
};


#endif
