//  $Id$
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
#include "RadixSet.hh"
#include "route-list.hh"

RadixNode *RadixNode::insert(void *dt) {
   unsigned int mask = 1 << 31;
   RadixNode *current = this;
   unsigned int  prefix = ((RouteList::Route *)dt)->route.get_ipaddr();
   unsigned char length = ((RouteList::Route *)dt)->route.get_length();

   for (int i = 0; i < length; ++i, mask = mask >> 1) {
      if (! (prefix & mask)) {
	 if (! current->left) 
	    current->left = new RadixNode(current);
	 current = current->left;
      }
      else { 
	 if (! current->rght) 
	    current->rght = new RadixNode(current);
	 current = current->rght;
      }
   }

   if (current->state != RADIX_FULL) {
      current->state = RADIX_FULL;
      current->data  = dt;
   }

   return current;
}

RadixNode *RadixNode::find(PrefixRange *dt) {
   unsigned int mask = 1 << 31;
   RadixNode *current = this;
   unsigned int  prefix = dt->get_ipaddr();
   unsigned char length = dt->get_length();

   for (int i = 0; i < length; ++i, mask = mask >> 1) {
      if (! (prefix & mask)) {
	 if (! current->left) 
	    return NULL;
	 current = current->left;
      }
      else { 
	 if (! current->rght) 
	    return NULL;
	 current = current->rght;
      }
   }

   return current;
}

inline RadixNode *RadixNode::next_i() {
   RadixNode *current = this;

   if (current->left)
      return current->left;

   if (current->rght)
      return current->rght;
      
   while (current->prnt) {
      while (current->prnt && current == current->prnt->left) { 
	 // I was a left child
	 current = current->prnt;
	 if (current->rght)
	    return current->rght;
      }
      while (current->prnt && current == current->prnt->rght)
	 // I was a rght child
	 current = current->prnt;
   }
   return NULL;
}

RadixNode *RadixNode::next() {
   RadixNode *current;
   for (current = this->next_i(); 
	current && current->state == RADIX_EMPTY;
	current = current->next_i())
      ;
   return current;
}
