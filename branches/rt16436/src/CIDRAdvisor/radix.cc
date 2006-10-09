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
//  info-ra@isi.edu.
//
//  Author(s): Cengiz Alaettinoglu (cengiz@isi.edu)
//  Modified By: John Mehringer <mehringe@isi.edu>


#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream.h>
#include <iomanip.h>

#include "irr/irr.hh"
#include "irr/autnum.hh"

#include "radix.hh"


// Global Vars
static addr * aggregates;
static addr * root_aggregates = NULL;
static addr * components;
static addr * root_components = NULL;
static addr * prefixes;
static addr * root_prefixes = NULL;
static int weight = 0; // used to find the number of prefixes in a subtree


//-------------------------------------------------------
//
//  addr::addr(unsigned int p, unsigned int i, IRR * irr) 
//    Set prefix = p and length = l 
//-------------------------------------------------------
addr::addr(unsigned int p, unsigned int l, IRR * irr) {
  char buffer[32];
  
  Prefix * bgp_prefix = new Prefix(p,l);
  
  prefix = p;
  length = l;

  bgproute = new BGPRoute(INVALID_AS, bgp_prefix, irr);
  components = NULL;
  next = NULL;
}

//-------------------------------------------------------
//
//  addr::addr(const addr & original)
//    Clone an addr (address) 
//    Note: Doesn't copy over components or Next pointers
//-------------------------------------------------------
addr::addr(const addr & original) {
  BGPRoute * temp_bgproute = NULL;
  
  prefix = original.prefix;
  length = original.length;
  
  temp_bgproute = original.bgproute;
  
  if (temp_bgproute != NULL)
    bgproute = new BGPRoute(*temp_bgproute);
  else
    bgproute = NULL;
  
  components = NULL;
  next = NULL;      
}

//-------------------------------------------------------
// void 
// addr::findOrigins(IRR * irr) 
//   Finds originating ASes for addr 
//   and puts them in origin
//-------------------------------------------------------
void
addr::findOrigin(IRR * irr) {
  char buffer[32];
  if (prefix && length) {
    if (bgproute->origin)
      delete bgproute->origin;
    bgproute->origin = new Origin(getStringPrefix(buffer), irr);    
  }
}

//-------------------------------------------------------
// char *
// addr::getStringPrefix(char * buffer); 
//   Returns a string of the addr route ie. "10.0.0.0/24"
//-------------------------------------------------------
char *
addr::getStringPrefix(char * buffer) { 

  sprintf(buffer, "%d.%d.%d.%d/%d", 
         (prefix >> 24) & 0xFF, (prefix >> 16) & 0xFF,
         (prefix >> 8)  & 0xFF, prefix & 0xFF, length);
   return buffer;
}

//##########################################################################

//------------------------------------------------------------------
// radix_node *
// radix_create_node(radix_node * parent, unsigned int prefix,
//                   unsigned int prefixlen, ASt origin, int state)
//
//   Create a new radix node with the passed in parameters
//     Should probably be moved to the radix_node class as a 
//     type of constructor function.
//
//      Used in CIDRAdvisor.cc  main() to create the root node of
//      a radix tree
//------------------------------------------------------------------
radix_node * radix_create_node(radix_node * parent, unsigned int prefix,
                               unsigned int prefixlen, ASt origin, int state) {
  radix_node * new_node;    

  new_node = new radix_node;
  new_node->leftc = NULL;
  new_node->rightc = NULL;
  new_node->parent  = parent;
  new_node->prefix = prefix;
  new_node->length = prefixlen;

   // Copy origin over to new radix node
   /* if(origin) {
	ListNodePix *p = new ListNodePix(origin);
	(new_node->origin).append(p->l);
    }
    */
  if (origin != INVALID_AS)
    new_node->origin->add(origin);    
    
  new_node->state = state;
  return new_node;
}


//------------------------------------------------------------------
// int 
// radix_insert(unsigned int prefix, unsigned int length,
//                       ASt origin, radix_node * root)
//
//------------------------------------------------------------------
int radix_insert(unsigned int prefix, unsigned int length,
                          ASt origin, radix_node *root) {
  int i = 0;
  int count = 0; // used to count the # of addr prefixes inserted
  unsigned int mask,addr_bit = 0;
  radix_node *cur_node = root;

  mask = 1 << 31;
  while(i < length ) {

    addr_bit = prefix & mask;

    if (!addr_bit) {
      if(cur_node->leftc) {
        cur_node = cur_node->leftc;
      } else {
        cur_node->leftc = radix_create_node(cur_node, 0, i+1, INVALID_AS, EMPTY);
        cur_node = cur_node->leftc;
      }
    } else { 
      if(cur_node->rightc) {
        cur_node = cur_node->rightc;
      } else {
        cur_node->rightc = radix_create_node(cur_node,0,i+1,INVALID_AS,EMPTY);
        cur_node = cur_node->rightc;
      }
    }
	
    if(cur_node->state == FULL) { // less specific or same prefix present
      if(i + 1 == length)  // same prefix present and so count is 0 
        return(count);                
      else  // this is a more specific prefix
        return(MORE_SPECIFIC);  
    }

    mask = mask >> 1;
    ++i;
  }

  cur_node->prefix = prefix;

  if (origin) {
    cur_node->origin->add(origin);
  } 
    
    
  cur_node->state = FULL;
  if(cur_node->leftc) {
    weight = 0;
    count -= wt_of_subtree(cur_node->leftc);
    radix_delete_tree(cur_node->leftc);
  }
  if(cur_node->rightc) {
    weight = 0;
    count -= wt_of_subtree(cur_node->rightc);
    radix_delete_tree(cur_node->rightc);
  }
  cur_node->leftc = NULL;
  cur_node->rightc = NULL;
  
  if(count == 0)
    ++count;
    
  return(count);
}    





int wt_of_subtree(radix_node* root) {
  if(root->state == FULL) ++weight;
  if(root->leftc)  wt_of_subtree(root->leftc);
  if(root->rightc) wt_of_subtree(root->rightc);
  return(weight);
}





void radix_delete_node(radix_node *del_node) {
    delete del_node;
}





void radix_delete_tree(radix_node *del_node) {

    if(del_node->leftc)
	radix_delete_tree(del_node->leftc);
    if(del_node->rightc)
	radix_delete_tree(del_node->rightc);

    delete del_node;
}





int radix_delete(unsigned int prefix,unsigned int length,radix_node *root) {
    unsigned int mask,addr_bit = 0;
    int i = 0;
    radix_node *cur_node = root;

    mask = 1 << 31;

    while(i < length) {
	
	addr_bit = prefix & mask;
	
	if(!addr_bit) 
	    if(!cur_node->leftc)
		return(FALSE); // returns FALSE if element not present
	    else
		cur_node = cur_node->leftc;
	else
	    if(!cur_node->rightc)
		return(FALSE);
	    else
		cur_node = cur_node->rightc;

	mask = mask >> 1;
	++i;
    }

    if(cur_node->state == FULL || cur_node->state == AGGREGATE || cur_node->state == COMPONENT) {
	cur_node->state = EMPTY;
	return(TRUE);
    }
    return(FALSE);
}





int radix_search(unsigned int prefix,unsigned int length,radix_node *root) {
  unsigned int mask,addr_bit = 0;
  int i = 0;
  radix_node *cur_node = root;

  mask = 1 << 31;

  while(i < length) {

    if(cur_node->state == FULL) 
      return(MORE_SPECIFIC);

    addr_bit = prefix & mask;

    if(!addr_bit) 
      if(!cur_node->leftc)
        return(FALSE);
      else 
        cur_node = cur_node->leftc;
    else 
      if(!cur_node->rightc)
        return(FALSE);
      else
        cur_node = cur_node->rightc;
	
    mask = mask >> 1;
    ++i;
  }
  if(cur_node->state == AGGREGATE)
    return(AGGREGATE);
  else if(cur_node->state == FULL)
    return(TRUE);
  else
    return(FALSE);
}




//-------------------------------------------------------
//  void 
//  radix_aggregate(radix_node *root) 
//    
//-------------------------------------------------------
void radix_aggregate(radix_node * root) {
  radix_node * cur_node;

  cur_node = root;
  if(cur_node->leftc) {
    radix_aggregate(cur_node->leftc);
  }
  if(cur_node->rightc) {
    radix_aggregate(cur_node->rightc);
  }

  if(!cur_node->leftc || !cur_node->rightc) {
    return;
  }
    
  if((cur_node->leftc->state == FULL || cur_node->leftc->state == AGGREGATE) &&
     (cur_node->rightc->state == FULL || cur_node->rightc->state == AGGREGATE)) {
    if(cur_node->leftc->state == AGGREGATE) 
      cur_node->leftc->state = EMPTY;
    else
      cur_node->leftc->state = COMPONENT;

    if(cur_node->rightc->state == AGGREGATE) 
      cur_node->rightc->state = EMPTY;
    else
      cur_node->rightc->state = COMPONENT;
        
    cur_node->state = AGGREGATE;
    cur_node->prefix = cur_node->leftc->prefix;

   //*ListNodePix *p;
   // *  for(p = (cur_node->leftc->origin).head(); p; p = (cur_node->leftc->origin).next(p->l)) 
   //*     if(!((cur_node->origin).match(p->pix))) {
   //*       ListNodePix *origin_list = new ListNodePix(p->pix);
   //*      (cur_node->origin).append(origin_list->l);
   //*    }
   ///---------------------------------*******************
    for (ItemASNO * asln = cur_node->leftc->origin->list.head(); asln;
                    asln = cur_node->leftc->origin->list.next(asln)) {
     // cerr << "radix_aggreagte left has origin of " << asln->asno;

      if (! cur_node->origin->contains(asln->asno))
        cur_node->origin->add(asln->asno);   
    }                   
            
    for (ItemASNO * asln = cur_node->rightc->origin->list.head(); asln;
                    asln = cur_node->rightc->origin->list.next(asln)) {
      if (! cur_node->origin->contains(asln->asno)) {
        cur_node->origin->add(asln->asno);   
      }
    }    
  }
}




//-------------------------------------------------------
//  addr * 
//  get_components(radix_node *root) 
//    
//-------------------------------------------------------
addr * get_components(radix_node *root) {
  radix_node * cur_node;
  char prefix_quad[16];
  char prefix[20];

  cur_node = root;
  if(cur_node->leftc) {
    get_components(cur_node->leftc);
  }
  
  if(cur_node->rightc) {
    get_components(cur_node->rightc);
  }
    
  if (cur_node->state == COMPONENT) {
    if (! components) { 
      components = new addr; 
    } else {
	    components->next = new addr;
	    components = components->next;
    }

    if(! root_components) {
      root_components = components;
    }
    components->prefix = cur_node->prefix;
    components->length = cur_node->length;
    components->next = NULL;
    components->components = NULL;

    int2dd(prefix_quad,cur_node->prefix);
    sprintf(prefix,"%s/%d",prefix_quad,cur_node->length);

    for (ItemASNO * asln = cur_node->origin->list.head(); asln;
                    asln = cur_node->origin->list.next(asln)) {
      if (! components->bgproute->origin->contains(asln->asno))
        components->bgproute->origin->add(asln->asno);   
    }    
  }
  return root_components;
}






//-------------------------------------------------------
//  void
//  init_components() 
//    
//-------------------------------------------------------
void init_components() {
  components = NULL;
  root_components = NULL;
}




//-------------------------------------------------------
//  addr * 
//  radix_get_aggregates (radix_node * root)
//    
//-------------------------------------------------------
addr * radix_get_aggregates (radix_node * root) {
  radix_node *cur_node;
  char prefix_quad[16];
  char prefix[25];

  cur_node = root;
  //- Check left and right nodes first
  if(cur_node->state != AGGREGATE && cur_node->leftc)
    radix_get_aggregates(cur_node->leftc);
  if(cur_node->state != AGGREGATE && cur_node->rightc)
    radix_get_aggregates(cur_node->rightc);

  if(cur_node->state == AGGREGATE) {
    if(!aggregates) {
      aggregates = new addr; 
    } else {
      aggregates->next = new addr;
      aggregates = aggregates->next;
    }

    if(!root_aggregates) {
      root_aggregates = aggregates;
    }
    aggregates->prefix = cur_node->prefix;
    aggregates->length = cur_node->length;
    aggregates->next = NULL;

    int2dd(prefix_quad,cur_node->prefix);
    sprintf(prefix,"%s/%d",prefix_quad,cur_node->length);
   
    for (ItemASNO * asln = cur_node->origin->list.head(); asln;
                    asln = cur_node->origin->list.next(asln)) {
      if (! aggregates->bgproute->origin->contains(asln->asno))
        aggregates->bgproute->origin->add(asln->asno);   
    }    

    init_components();
    aggregates->components = get_components(cur_node);
  }
  return root_aggregates;
}




//-------------------------------------------------------
//  addr *
//  radix_get_aggs(radix_node * root)
//
//  Initializes aggregate variables and calls recursive 
//  function radix_get_aggregates();
//    
//-------------------------------------------------------
addr *
radix_get_aggs(radix_node * root) {

  //- init_agg_variables()
  aggregates = NULL;
  root_aggregates = NULL;

  return radix_get_aggregates(root);
}





void init_nonagg_variables() {
    prefixes = NULL;
    root_prefixes = NULL;
}




//-------------------------------------------------------
//  addr *
//  radix_nonaggregates(radix_node *root)
//    
//-------------------------------------------------------
addr * radix_nonaggregates(radix_node *root) {
  radix_node *cur_node;
  char prefix_quad[16],prefix[25];

  cur_node = root;
  if(cur_node->leftc) 
    radix_nonaggregates(cur_node->leftc); 
  if(cur_node->rightc) 
    radix_nonaggregates(cur_node->rightc); 

  if(cur_node->state == FULL) {
    if(!prefixes)
      prefixes = new addr;
    else {
      prefixes->next = new addr;
      prefixes = prefixes->next;
    }

    if(!root_prefixes)
      root_prefixes = prefixes;
	
    prefixes->prefix = cur_node->prefix;
    prefixes->length = cur_node->length;
    prefixes->next   = NULL;

    int2dd(prefix_quad,cur_node->prefix);
    sprintf(prefix,"%s/%d",prefix_quad,cur_node->length);
   
   /*
   
        prefixes->rt.nlri.pix = Prefask_map.add_entry(prefix);

	for(ListNodePix *p = (cur_node->origin).head() ; p ; p = (cur_node->origin).next(p->l)) {
	    ListNodePix *list = new ListNodePix(p->pix);
	    (prefixes->rt).origin.append(list->l);
	}
   
   */
    for (ItemASNO * asln = cur_node->origin->list.head(); asln;
                    asln = cur_node->origin->list.next(asln)) {
      if (! prefixes->bgproute->origin->contains(asln->asno))
        prefixes->bgproute->origin->add(asln->asno);   
    }    
    prefixes->components = NULL;
  }
  return(root_prefixes);
}





addr *radix_get_nonaggs(radix_node *root) {
    addr *nonaggs;

    init_nonagg_variables();
    nonaggs = radix_nonaggregates(root);
    return(nonaggs);
}





char* int2dd(char *buffer, unsigned int i) {
   sprintf(buffer, "%d.%d.%d.%d", 
           (i >> 24) & 0xFF, (i >> 16) & 0xFF, (i >> 8)  & 0xFF, i & 0xFF);
   return buffer;
}








