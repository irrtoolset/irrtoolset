// $Id$
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

#include "config.h"
#include <cstdio>
#include <ctype.h>

#include "bgproute.hh"
#include "gnug++/SetOfUInt.hh"
#include "normalform/community.hh"
#include "normalform/RadixSet.hh"
#include "re2dfa/regexp_nf.hh"

int BGPROUTE_MAX_ASPATH_LENGTH = 8;


//-------------------------------------------------------
//
//  NLRI::NLRI()
//
//-------------------------------------------------------
NLRI::NLRI() {
  prefix = NULL;
  dontcare();
}

//-------------------------------------------------------
//  NLRI::NLRI(char * new_prefix)
//
//-------------------------------------------------------
NLRI::NLRI(char * new_prefix) {

}


//------------------------------------------------------
//
//  NLRI::NLRI(Prefix * new_prefix)
//
//-------------------------------------------------------
NLRI::NLRI(Prefix * new_prefix) {
  prefix = new Prefix(*new_prefix);
  _dontcare = false;
}

//------------------------------------------------------
//
//  NLRI::NLRI(Prefix * new_prefix)
//
//-------------------------------------------------------
NLRI::NLRI(const NLRI & original) {
  if (original.is_dontcare()) {
    prefix = (Prefix*) NULL;
    dontcare();
  } else {
    prefix = new Prefix(original.get_text());
    _dontcare = false;
  }
}

//------------------------------------------------------
//
//  NLRI::~NLRI()
//
//-------------------------------------------------------
NLRI::~NLRI() {
  delete prefix;
}

//------------------------------------------------------
//
//  NLRI::getPrefix()
//
//-------------------------------------------------------   
Prefix * 
NLRI::getPrefix() {
  return prefix;
}

//------------------------------------------------------
//
//  NLRI::get_text()
//
//-------------------------------------------------------
char *
NLRI::get_text() const {
  return prefix->get_text();
}

//#######################################################

//------------------------------------------------------
//  List<ItemASNO> *
//  Origin::getRouteOrigins(char * prefix, IRR * whois)
//  
//    Looks up the originators of a given route prefix.
//      and formats it into a list of ASt. Then returns a
//      pointer to the list
//
//-------------------------------------------------------
List<ItemASNO> *
Origin::getRouteOrigins(char * prefix, IRR * whois) {
  char * response = NULL;
  char as_string[64];
  char * response_index;
  int string_index;
  List<ItemASNO> * as_list = new List<ItemASNO>;
  ASt origin_as;
  bool on_list;
  
  // Get list of possible orgins
  whois->getSourceOrigin(response, prefix);
  
  // response is a string with the form
  //  Source ASnumber
  //  For example:
  //  RADB AS4200
  //
  // The folowing code parses that string and
  // trys to extract all ASxxxx numbers from it
  // and sticks them in the as_list
  response_index = response;
  while (response_index && ((*response_index) != '\0')) {
    if (((*(response_index + 1)) != '\0') &&
        (toupper(*response_index) == 'A') && 
        (toupper(*(response_index + 1)) == 'S')) {
      response_index += 2;
      string_index = 0;
      while(isdigit(*response_index)) {
        if (string_index < 63) {
          as_string[string_index] = *response_index;
          string_index++;
          response_index++;
        } else {
          cerr << "Warning -- Origin::getRouteOrigins(char * prefix, IRR * whois)" << endl;
          cerr << "        -- as_string overflow" << endl;
        }
      }
      as_string[string_index] = '\0';
      
      //- Add origin AS to as_list if it is not
      //-  already on the as_list
      origin_as = atoi(as_string);
      on_list = false;
      
      for (ItemASNO * asln = as_list->head(); asln;
                      asln = as_list->next(asln)) {
        if (asln->asno == origin_as)
          on_list = true;
      }
      
      if (! on_list && origin_as)
        as_list->append(new ItemASNO(origin_as));
      
    }
    if (*response_index != '\0')
      response_index++;           
  }
  
  return as_list;
}

//------------------------------------------------------
//
//  Origin::Origin(IPAddr * ip)
//
//-------------------------------------------------------
Origin::Origin() {
  clear();
}

//------------------------------------------------------
//
//  Origin::Origin(char * prefix, IRR * whois)
//
//-------------------------------------------------------
Origin::Origin(char * prefix, IRR * whois) {
  List<ItemASNO> * temp_list;
  
  clear();
  temp_list = getRouteOrigins(prefix,whois);
  for (ItemASNO * asln = temp_list->head(); asln;
                  asln = temp_list->next(asln)) {
    add(asln->asno);
  }
}

//------------------------------------------------------
//
//  Origin::Origin(ASt single_origin)
//
//-------------------------------------------------------
Origin::Origin(ASt single_origin) {
  clear();
  add(single_origin);
}

//------------------------------------------------------
//
//  Origin::Origin(const Origin & original)
//    Clone the original
//  
//-------------------------------------------------------
Origin::Origin(const Origin & original) {
  clear();
  for (ItemASNO * asln = original.list.head(); asln;
                  asln = original.list.next(asln)) {
    add(asln->asno);
  }
}

//------------------------------------------------------
//
//  Origin::clear() 
//    Clear the list
//-------------------------------------------------------
void
Origin::clear() {
  list.clear();
  size = 0;
}


//------------------------------------------------------
//  void
//  Origin::add(ASt as)
//
//-------------------------------------------------------
void
Origin::add(ASt as) {
  append(as);
}

//------------------------------------------------------
//
//  Origin::prepend(ASt)
//  
//  Adds a new as to the front of Origin list
//
//------------------------------------------------------- 
void
Origin::prepend(ASt as) {
  list.prepend(new ItemASNO(as));
  size++;
}

//------------------------------------------------------
//  void
//  Origin::append(ASt)
//  
//  Adds a new as to the back of Origin list
//
//------------------------------------------------------- 
void
Origin::append(ASt as) {
  list.append(new ItemASNO(as));
  size++;
}


//------------------------------------------------------
//  bool
//  Origin::contains(ASt as)
//
//  Checks to see if as is contained in the Origin list
//
//-------------------------------------------------------   
bool
Origin::contains(ASt as) {
  for (ItemASNO * asln = list.head(); asln; asln = list.next(asln)) {
    if (asln->asno == as)
      return true;
    //cerr << "Origin::contains -- checking AS" << asln->asno << endl;
  }
  return false;
}

//------------------------------------------------------
//  int
//  Origin::getSize()
//
//  Returns size of the origin list
//-------------------------------------------------------   
int
Origin::getSize() {
  return size;
}

//#######################################################


//-------------------------------------------------------
//
//  ASPath::ASPath()
//  Creates an ASPath with no limit to the length of the 
//  path
//
//-------------------------------------------------------
ASPath::ASPath() {  
  max_length = NO_LIMIT;
  clear();
}

//------------------------------------------------------
//
//  ASPath::ASPath()
//  
//
//-------------------------------------------------------
ASPath::ASPath(int max) {  
  max_length = max;
  clear();
}

//------------------------------------------------------
//
//  ASPath::ASPath(ASt new_as)
//  
//  Create a new ASPath with origin
//  as the first AS in the list
//
//-------------------------------------------------------
ASPath::ASPath(ASt new_as, int max) {  
  max_length = max;
  clear();
  prepend(new_as);
}


//------------------------------------------------------
//
//  ASPath::ASPath(const ASpath & original)
//  
//  Creates a new ASPath which is a
//  duplicate of original
//
//------------------------------------------------------- 
ASPath::ASPath(const ASPath & original) {
  max_length = original.getMaxPathLength();  
  for (ItemASNO * asln = original.path_list.head(); asln;
                  asln = original.path_list.next(asln)) { 
    append(asln->asno);
  }
}

//------------------------------------------------------
//
//  ASPath::~ASPath() 
//  
//
//------------------------------------------------------- 
ASPath::~ASPath() {
  //delete path_list;
}

//------------------------------------------------------
//
//  ASPath::clear()
//  
//  
//
//-------------------------------------------------------
void
ASPath::clear() {
  path_list.clear();
}

//------------------------------------------------------
//
//  ASPath::prepend(ASt)
//  
//  Adds a new as to the ASPath list
//
//------------------------------------------------------- 
void
ASPath::prepend(ASt as) {
  if ((getPathLength() < max_length) || (max_length == NO_LIMIT)) 
    path_list.prepend(new ItemASNO(as));
}

//------------------------------------------------------
//
//  ASPath::append(ASt)
//  
//  Adds a new as to the ASPath list
//
//------------------------------------------------------- 
void
ASPath::append(ASt as) {
  if ((getPathLength() < max_length) || (max_length == NO_LIMIT)) 
    path_list.append(new ItemASNO(as));
}

//------------------------------------------------------
//  void
//  ASPath::remove(ASt)
//  
//  Removes an AS from the ASPath list
//
//------------------------------------------------------
void
ASPath::remove(ASt as) {
  List<ItemASNO> temp_list;
  for (ItemASNO * asln = path_list.head(); asln;
                  asln = path_list.next(asln)) { 
    if (asln->asno != as)
      temp_list.append(new ItemASNO(as));
  }
  
  path_list.clear();
  for (ItemASNO * asln = temp_list.head(); asln;
                  asln = temp_list.next(asln)) {
    append(asln->asno);
  }
}

//------------------------------------------------------
//
//  ASPath::getPathLength()
//  
//  
//
//------------------------------------------------------- 
int 
ASPath::getPathLength() const { 
  return path_list.size(); 
}

//------------------------------------------------------
//
//  ASPath::getMaxPathLength() const 
//  
//
//------------------------------------------------------- 
int
ASPath::getMaxPathLength() const { 
  return max_length;
}

//------------------------------------------------------
//
//  ASPath::loopCheck(ASt as)
//  
//  Checks the ASPath for a loop with as
//
//------------------------------------------------------- 
bool
ASPath::loopCheck(ASt as) {
  for (ItemASNO * asln = path_list.head(); asln; asln = path_list.next(asln))
    if (asln->asno == as)
      return true;
  return false;
}


//------------------------------------------------------
//
// ASPath::print() 
//  
//  Prints out the AS path
//
//------------------------------------------------------- 
void 
ASPath::print(ASt first) {
  cout << "AS" << first;
  for (ItemASNO * asln = path_list.head(); asln; asln = path_list.next(asln))
    cout << " AS" << asln->asno;
  cout << endl;
}

//------------------------------------------------------
//
// ASPath::atMaxRouteLength();
//  
//
//-------------------------------------------------------
bool 
ASPath::atMaxRouteLength() {
  return getPathLength() >= max_length;
}

//------------------------------------------------------
// bool
// ASPath::contains(ASt as);
//  
//
//-------------------------------------------------------
bool 
ASPath::contains(ASt as) {
  for (ItemASNO * asln = path_list.head(); asln; asln = path_list.next(asln))
    if (asln->asno == as)
      return true;
  return false;
}

//#######################################################


//------------------------------------------------------
//  
// BGPCommunity::BGPCommunity(int i)
//
//------------------------------------------------------
BGPCommunity::BGPCommunity(int i) {
  id = i;
}

//------------------------------------------------------
//  
// BGPCommunity::BGPCommunity(const BGPCommunity &original)
//
//------------------------------------------------------
BGPCommunity::BGPCommunity(const BGPCommunity &original) {
  id = original.id;
}



//------------------------------------------------------
// int 
// BGPCommunitySet::parseItem(Item * community_entry) const
//  Parses a PTree to convert the ptree to a community integer
//
//-------------------------------------------------------
int 
BGPCommunitySet::parseItem(Item * community_entry) const {
  if (typeid(*community_entry) == typeid(ItemINT))
    return ((ItemINT *)community_entry)->i;
    
  else if (typeid(*community_entry) == typeid(ItemWORD)) {
    if (!strcasecmp(((ItemWORD *)community_entry)->word, "no_advertise"))
      return COMMUNITY_NO_ADVERTISE;
    else if (!strcasecmp(((ItemWORD *)community_entry)->word, "no_export"))
	    return COMMUNITY_NO_EXPORT;
    else if (!strcasecmp(((ItemWORD *)community_entry)->word,"no_export_subconfed"))
      return COMMUNITY_NO_EXPORT_SUBCONFED;
    else
      return COMMUNITY_INTERNET;
  } else if (typeid(*community_entry) == typeid(ItemList)) {
	    int high = ((ItemINT *) ((ItemList *) community_entry)->head())->i;
	    int low  = ((ItemINT *) ((ItemList *) community_entry)->tail())->i;
	    return (high << 16) + low;
  } else
    cerr << "Warning: Ignoring non-community value " << *community_entry << endl;
}
//------------------------------------------------------
//
// BGPCommunitySet::BGPCommunitySet()
//    Creates a BGP community 
//      of maximum size MAX_COMMUNITIES
//
//-------------------------------------------------------
BGPCommunitySet::BGPCommunitySet() {
}


//------------------------------------------------------
//
// BGPCommunitySet::BGPCommunitySet()
//    Clones the passed in BGPCommunitySet - original 
//
//-------------------------------------------------------
BGPCommunitySet::BGPCommunitySet(const BGPCommunitySet &original) {
  int as, path;
  
  for (BGPCommunity * bgpcommunity = original.community_set.head(); bgpcommunity;
                    bgpcommunity = original.community_set.next(bgpcommunity)) {
    add(bgpcommunity);
  }
}

//------------------------------------------------------
//
// BGPCommunitySet::~BGPCommunitySet();
//  
//
//-------------------------------------------------------
BGPCommunitySet::~BGPCommunitySet() {
  //delete community_set;
}
 

//------------------------------------------------------
//
// BGPCommunitySet::clear()
//  
//
//-------------------------------------------------------
void
BGPCommunitySet::clear() {
  community_set.clear();
}


//------------------------------------------------------
//
// BGPCommunitySet::add(int new_member)
//  
//
//-------------------------------------------------------
void
BGPCommunitySet::add(int new_member) {
  community_set.append(new BGPCommunity(new_member));
}


//------------------------------------------------------
//
// BGPCommunitySet::add(BGPCommmunity * new_member)
//  
//
//-------------------------------------------------------
void
BGPCommunitySet::add(BGPCommunity * new_member) {
  community_set.append(new BGPCommunity(*new_member));
}


//------------------------------------------------------
//
// BGPCommunitySet::add(Item * new_member)
//  
//
//-------------------------------------------------------
void
BGPCommunitySet::add(Item * new_member) {
  add(parseItem(new_member));
}


//------------------------------------------------------
// bool
// BGPCommunitySet::contains(int test_member) 
//    Tests to see if a single community entry 
//    is contained with in the community set.  
//
//-------------------------------------------------------
bool
BGPCommunitySet::contains(int test_member) const {
  for (BGPCommunity * bgpcommunity = community_set.head(); bgpcommunity;
                    bgpcommunity = community_set.next(bgpcommunity)) {
     if (bgpcommunity->id == test_member)
       return true;
  }
  return false;
}

//------------------------------------------------------
// bool
// BGPCommunitySet::contains(Item * test_member) 
//    Tests to see if a single community entry 
//    is contained with in the community set.  
//
//  Requires: BGPCommunitySet::contains(int test_member)
//            BGPCommunitySet::parseItem(Item * community_entry)
//-------------------------------------------------------
bool
BGPCommunitySet::contains(Item * test_member) const {
  return (contains(parseItem(test_member)));
}


//------------------------------------------------------
// bool
// BGPCommunitySet::containsSubset(FilterRPAttribute * subset) const 
//    Tests to see if a community set is a  
//    subset of the BGPCommunitySet set.
//
//  Requires: BGPCommunitySet::contains(Item * test_member);  
//
//-------------------------------------------------------
bool
BGPCommunitySet::containsSubset(const FilterRPAttribute * subset) {
  bool result = true;
  for (Item * member = subset->args->head(); member; 
       member = subset->args->next(member)) {
    result = result && contains(member);
  }
  return result;
}


//------------------------------------------------------
// bool
// BGPCommunitySet::equalsSet(FilterRPAttribute * equalset) const 
//    Tests to see if a single community entry 
//    is contained with in the community set.
//
//  Requires: BGPCommunitySet::contains(Item * test_member);  
//
//-------------------------------------------------------
bool
BGPCommunitySet::equalsSet(const FilterRPAttribute * test_set) {
  bool result = true;
  int members = 0;
  for (Item * member = test_set->args->head();  member; 
       member = test_set->args->next(member)) {
    members++;
    result = result && contains(member);
  }
  return result && (community_set.size() == members);
}


//#######################################################


//------------------------------------------------------
//
//  BGPRoute::BGPRoute(ASt source_as, Prefix * prefix, IRR * irr)
//  
//  This method sets up a blank bgproute route.  
//
//-------------------------------------------------------
BGPRoute::BGPRoute() {
  nlri = new NLRI;
  origin = new Origin;
  source = INVALID_AS;
  aspath = new ASPath;
  action_aspath = new ASPath;
  bgpcommunityset = new BGPCommunitySet;
}


//------------------------------------------------------
//
//  BGPRoute::BGPRoute(ASt source_as, Prefix * prefix, IRR * irr)
//  
//  This method sets up a bgproute route.  It finds the
//  origins of a prefix and sets up a max aspath length.
//
//-------------------------------------------------------
BGPRoute::BGPRoute(ASt source_as, Prefix * prefix, IRR * irr) {
  nlri = new NLRI(prefix);  
  origin = new Origin(prefix->get_text(),irr);  
  source = source_as;
  aspath = new ASPath(BGPROUTE_MAX_ASPATH_LENGTH);
  action_aspath = new ASPath;
  bgpcommunityset = new BGPCommunitySet;
}


//------------------------------------------------------
//
//  BGPRoute::BGPRoute(ASt source_as, ASt origin_as);
//  
//  This method starts finding the AS path of 
//  a BGP route. 
//
//-------------------------------------------------------
BGPRoute::BGPRoute(ASt source_as, ASt origin_as) {
  nlri = new NLRI;
  origin = new Origin(origin_as);
  source = source_as;
  aspath = new ASPath(BGPROUTE_MAX_ASPATH_LENGTH);
  action_aspath = new ASPath;
  bgpcommunityset = new BGPCommunitySet;
}

//------------------------------------------------------
//
//  BGPRoute::BGPRoute(const BGPRoute &bgproute)
//  
//
//-------------------------------------------------------
BGPRoute::BGPRoute(const BGPRoute &bgproute) {
  nlri = new NLRI(*(bgproute.nlri));
  origin = new Origin(*(bgproute.origin));
  source = bgproute.source;
  aspath = new ASPath(*(bgproute.aspath));
  action_aspath = new ASPath(*(bgproute.action_aspath));
  bgpcommunityset = new BGPCommunitySet(*(bgproute.bgpcommunityset));
}
//------------------------------------------------------
//
// BGPRoute::~BGPRoute() 
//  
//
//-------------------------------------------------------
BGPRoute::~BGPRoute() {
  delete nlri;
  delete origin;
  delete aspath;
  delete action_aspath;
  delete bgpcommunityset;
}

//------------------------------------------------------
//
// BGPRoute::print()
//  
//
//-------------------------------------------------------
void 
BGPRoute::print() {
  aspath->print(source);
}


//------------------------------------------------------
//
// BGPRoute::atMaxRouteLength();
//  
//
//-------------------------------------------------------
bool 
BGPRoute::atMaxRouteLength() {
  return aspath->atMaxRouteLength();
}


//------------------------------------------------------
//
// BGPRoute::loopCheck(ASt as) 
//  
//
//-------------------------------------------------------
bool 
BGPRoute::loopCheck(ASt as) {
  return aspath->loopCheck(as);
}


//------------------------------------------------------
//
//  BGPRoute::add(ASt as)
//  
//  Adds an as to the aspath &  action_aspath
//
//------------------------------------------------------- 
void
BGPRoute::add(ASt as) {
  aspath->prepend(as);
  action_aspath->prepend(as);
}

//------------------------------------------------------
//  void
//  BGPRoute::remove(ASt as)
//  
//  Removes an AS from the aspath & action_aspath
//
//------------------------------------------------------- 
void
BGPRoute::remove(ASt as) {
  aspath->remove(as);
  action_aspath->remove(as);
}


//------------------------------------------------------
//  bool 
//  BGPRoute::evaluate(const PrefixRanges * prefixranges)
//
//  Check for nlri->is_dontcare() before using
//  
//------------------------------------------------------- 
bool 
BGPRoute::evaluate(const PrefixRanges * prefixranges) {
  RadixTree * radixtree;
  Prefix * origin_prefix;
  
  if (nlri->is_dontcare()) {
    return false;
  }
    
  if (!prefixranges || prefixranges->isEmpty()) {
    return false;
  } else {  // Cycle through all prefix ranges and compare our origin prefix
    origin_prefix = nlri->getPrefix();    
    for (int i = prefixranges->low(); i < prefixranges->fence(); prefixranges->next(i)) {
      //RadixTree(u_int _addr, u_int _leng, u_int64_t _rngs)
      // Create radix tree to do comparisons
      radixtree = new RadixTree((*prefixranges)[i].get_ipaddr(),
                                (*prefixranges)[i].get_length(),
                                (*prefixranges)[i].get_range());
                                    
      if (radixtree->contains(origin_prefix->get_ipaddr(),
                              origin_prefix->get_length(),
                              origin_prefix->get_range())) {
            return true;
      }
      delete radixtree;
    }
    return false;
  }
} 

//------------------------------------------------------
//  bool 
//  BGPRoute::evaluate(const FilterPRFXList * prefixranges)
//  Evaluate to see if a match between origin prefix
//  and current route filter
//
//  Identical to evaluate(const PrefixRanges * prefixranges)
//  execpt passing in a FilterPRFXList which inherits 
//  PrefixRanges
//  
//------------------------------------------------------- 
bool 
BGPRoute::evaluate(const FilterPRFXList * prefixranges) {
  RadixTree * radixtree;
  Prefix * origin_prefix;
  
  if (nlri->is_dontcare()) {
    return false;
  }
    
  if (!prefixranges || prefixranges->isEmpty()) {
    return false;
  } else {  // Cycle through all prefix ranges and compare our origin prefix
    origin_prefix = nlri->getPrefix();
    for (int i = prefixranges->low(); i < prefixranges->fence(); prefixranges->next(i)) {
      //RadixTree(u_int _addr, u_int _leng, u_int64_t _rngs)
      // Create radix tree to do comparisons
      radixtree = new RadixTree((*prefixranges)[i].get_ipaddr(),
                                (*prefixranges)[i].get_length(),
                                (*prefixranges)[i].get_range());
                 
      if (radixtree->contains(origin_prefix->get_ipaddr(),
                              origin_prefix->get_length(),
                              origin_prefix->get_range())) {
        return true;
      }
      delete radixtree;
    }
    return false;
  }
} 


//------------------------------------------------------
//
// BGPRoute::evaluate(const FilterRPAttribute *filter)
//  
//  A continuation of isMatching for the 
//    FilterRPAttribute put here for better 
//    readability
//
//-------------------------------------------------------
bool 
BGPRoute::evaluate(const FilterRPAttribute *filter) {
   static const AttrRPAttr *comm_rp_attr = (AttrRPAttr *) NULL;
   static const AttrMethod *comm_contains = (AttrMethod *) NULL;
   static const AttrMethod *comm_fcall = (AttrMethod *) NULL;
   static const AttrMethod *comm_equals = (AttrMethod *) NULL;

   if (!comm_rp_attr)
	 comm_rp_attr = schema.searchRPAttr("community");
   if (!comm_contains)
	 comm_contains = comm_rp_attr->searchMethod("contains");
   if (!comm_fcall)
	 comm_fcall = comm_rp_attr->searchMethod("()");
   if (!comm_equals)
	 comm_equals = comm_rp_attr->searchMethod("==");
   
   if (filter->rp_attr != comm_rp_attr) {
      cerr << "Warning: unimplemented method " << filter->rp_attr->name << "."
	   << filter->rp_method->name << " in filter, assuming no match for it.\n";
      return false;
   }
   if (filter->rp_method != comm_contains
       && filter->rp_method != comm_fcall
       && filter->rp_method != comm_equals) {
      cerr << "Warning: unimplemented method " << filter->rp_attr->name << "."
	   << filter->rp_method->name << " in filter, assuming no match for it.\n";
      return false;
   }

  if (filter->rp_method == comm_equals) {
     // Check if bgpcommunityset is equal to the filter community
    return bgpcommunityset->equalsSet(filter);
  } else
     // Check if it is a subset of the ptree
    return bgpcommunityset->containsSubset(filter); 
    
  return false;
}


//------------------------------------------------------
//
// BGPRoute::isMatching(Filter * filter, IRR *irr)
//  
//  Evaluate AS filter to see if it will accept our traffic
//
//-------------------------------------------------------
bool 
BGPRoute::isMatching(ASt PeerAS, Filter * filter, IRR * irr) {

  if (typeid(*filter) == typeid(FilterOR)) {
    return (isMatching(PeerAS,((FilterOR *) filter)->f1, irr) ||
            isMatching(PeerAS,((FilterOR *) filter)->f2, irr));
  } 

  if (typeid(*filter) == typeid(FilterAND)) {
   return (isMatching(PeerAS,((FilterAND *) filter)->f1, irr) &&
           isMatching(PeerAS,((FilterAND *) filter)->f2, irr));
  }

  if (typeid(*filter) == typeid(FilterNOT)) {
    return ! isMatching(PeerAS,((FilterNOT *) filter)->f1, irr);
  }

  if (typeid(*filter) == typeid(FilterASNO)) {
    return origin->contains(((FilterASNO *) filter)->asno);
  }

  if (typeid(*filter) == typeid(FilterASNAME)) {
    const SetOfUInt * as_set;
    as_set = irr->expandASSet(((FilterASNAME *) filter)->asname);
    if (!as_set || as_set->isEmpty()) {
      return false;
    } else { 
      // Check if AS number is in the origin set
      for (Pix p = as_set->first(); p; as_set->next(p)) {
        if (origin->contains((*as_set)(p))) {
          return true;
        }
      }
      return false;
    }
  }

  if (typeid(*filter) == typeid(FilterRSNAME)) {
    const PrefixRanges * rs_set;        
    if (nlri->is_dontcare()) {
      return false;
    } else {
      // Expand route set to PrefixRanges
      rs_set = irr->expandRSSet(((FilterRSNAME *) filter)->rsname);
      return evaluate(rs_set);
    }
  }
  
  if (typeid(*filter) == typeid(FilterPeerAS)) {
    return origin->contains(PeerAS);
     /*AutNumPeeringIterator itr(autnum);
     for (const Peering *peering = itr.first(); peering; peering = itr.next()){
        if (origin->contains(peering->peerAS))
	   return true;
     }   
     return false;*/
  }
  
  if (typeid(*filter) == typeid(FilterMS)) {
    // More Specific
    return isMatching(PeerAS,((FilterMS *)filter)->f1,irr);
  }

  if (typeid(*filter) == typeid(FilterANY)) {
    return true;
  }

  if (typeid(*filter) == typeid(FilterASPath)) {
    regexp_nf * evaluate_path = new regexp_nf(((FilterASPath *)filter)->re);
    return evaluate_path->match(action_aspath->path_list);
  }
   
  if (typeid(*filter) == typeid(FilterPRFXList)) {
    // PTreeFilterPRFXList inherits PrefixRanges
   /* if (
    
    PrefixRanges::contains(PrefixRange &b) */
    return evaluate((FilterPRFXList *)filter);
  }

  if (typeid(*filter) == typeid(FilterRPAttribute)) {
    // Evaluated in the BGPRoute::evaluate method 
    // to make code more readable
    return evaluate((FilterRPAttribute *) filter);
  }
  
  assert(!"Unknown Filter");
  return false;
}

//------------------------------------------------------
//
// BGPRoute::executeAction(PTreeAction * action, ActionDictionary * type)
//  
//
//-------------------------------------------------------
void 
BGPRoute::executeAction(PolicyActionList * action_list, ActionDictionary * type) {
#define UNIMPLEMENTED_METHOD \
   cerr << "Warning: unimplemented method " \
	<< *action->rp_attr->name << "." << action->rp_method->name << endl

  PolicyAction *action;
  
  for (action = action_list->head(); action; action = action_list->next(action)) {
    if (action->rp_attr == type->dctn_rp_pref) {
	   if (action->rp_method == type->dctn_rp_pref_set) {
        pref.set(((ItemINT *) action->args->head())->i);
      } else 
        UNIMPLEMENTED_METHOD;
        
    } else if (action->rp_attr == type->dctn_rp_dpa) {
      if (action->rp_method == type->dctn_rp_dpa_set) {
        dpa.dpa = ((ItemINT *) action->args->head())->i;
      } else 
        UNIMPLEMENTED_METHOD;
        
    } else if (action->rp_attr == type->dctn_rp_med) {
      if (action->rp_method == type->dctn_rp_med_set) {
        Item *item = action->args->head();
        if (typeid(*item) == typeid(ItemINT))
	       med.set(((ItemINT *) item)->i);
      } else 
        UNIMPLEMENTED_METHOD;
        
    } else if (action->rp_attr == type->dctn_rp_community) {
      if (action->rp_method == type->dctn_rp_community_setop) {
        bgpcommunityset->clear();
        
        for (Item * community_member = action->args->head(); community_member;
             community_member = action->args->next(community_member)) 
          bgpcommunityset->add(community_member);

      } else if (action->rp_method == type->dctn_rp_community_append ||
                 action->rp_method == type->dctn_rp_community_appendop) {
        for (Item * community_member = action->args->head(); community_member;
             community_member = action->args->next(community_member)) 
          bgpcommunityset->add(community_member);
          
      } else
        UNIMPLEMENTED_METHOD;
        
    } else if (action->rp_attr == type->dctn_rp_aspath) {
      if (action->rp_method == type->dctn_rp_aspath_prepend) {
        for (Item *plnd = action->args->head(); plnd;
             plnd = action->args->next(plnd))
          action_aspath->prepend(((ItemINT *) plnd)->i);
      } else
        UNIMPLEMENTED_METHOD;
    } else 
      cerr << "Warning: unimplemented attribute " << *action->rp_attr << endl;
  }
}

