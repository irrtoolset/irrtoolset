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
//  Author(s): Cengiz Alaettinoglu (cengiz@isi.edu)
//  Modified by: John Mehringer <mehringe@isi.edu>


#ifndef RADIX_H
#define RADIX_H

#include <cstdlib>
//#include "config.hh"
//#include "Route.hh"
#include "bgp/bgproute.hh"

#define MAXPREFIXLEN 32
#define MORE_SPECIFIC 2
#define TRUE 1
#define FALSE 0

// The following define whether the components of the aggregates also have
// to be printed or not
#define NO_COMPONENTS 0
#define COMPONENTS 1

// The following define the state of the nodes of the radix tree
#define EMPTY 0
#define FULL 1
#define AGGREGATE 3
#define COMPONENT 4

// Address Class used for CIDRAdvisor
class addr {
public:
  unsigned int prefix;
  unsigned int length;
  BGPRoute * bgproute;
  //NLRI * nlri;
  //*Origin * origin;
  addr *components;
  addr *next;
  
  addr() {
    prefix = 0;
    length = 0;
   //* rt.clear();
    bgproute = new BGPRoute;
    //*origin = new Origin;
    components = NULL;
    next = NULL;
  }

  addr(unsigned int p, unsigned int l, IRR * irr);  // Set prefix = p 
                                                    // and length = l
  
  addr(const addr & original);  // Clones the original addr 
                                // Note: Doesn't copy over components or next 
                                //       they are set to NULL
                                // look at aggchk.cc append() for info on that

  ~addr() {
   //* rt.clear();
   // delete bgproute;
  }
  
  void findOrigin(IRR * irr);  // Finds originating ASes for addr and puts them in origin
  
  char * getStringPrefix(char * buffer); // Returns a string of the addr route ie. "10.0.0.0/24"
 
};


// Radix Class used for CIDRAdvisor
class radix_node {
public:
  radix_node *leftc;    // left child
  radix_node *rightc;   // right child
  radix_node *parent;   // parent
  unsigned int prefix;
  unsigned int length;          // prefix length
  Origin * origin;                // route originator 
  int state;                    // describes the state of the node
  
  radix_node() {
    leftc = NULL;
    rightc = NULL;
    parent = NULL;
    prefix = 0;
    length = 0;
    origin = new Origin;
    state = 0;
  }
  
  ~radix_node() {
//*    origin.clear();
  }
};


radix_node *radix_create_node(radix_node *,unsigned int,unsigned int,ASt,int);
int radix_insert(unsigned int,unsigned int,ASt,radix_node *);
int wt_of_subtree(radix_node*);
extern void radix_delete_node(radix_node *);
extern void radix_delete_tree(radix_node *);
extern int radix_delete(unsigned int,unsigned int,radix_node *);
extern int  radix_search(unsigned int,unsigned int,radix_node *);
extern void radix_aggregate(radix_node *);
extern addr* radix_get_aggs(radix_node *);
extern addr* radix_get_nonaggs(radix_node*);
extern char* int2dd(char *,unsigned int);
#endif


