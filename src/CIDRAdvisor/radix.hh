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


