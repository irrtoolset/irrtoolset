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
