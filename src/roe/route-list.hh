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

#ifndef ROUTE_LIST_H
#define ROUTE_LIST_H

#include "config.h"
#include <cstring>
#include "util/List.hh"
#include "roe_tcl.hh"
#include "RadixSet.hh"
#include "normalform/RadixSet.hh"
#include "rpsl/rpsl.hh"   // For ASt

class RouteList {
public:
   class Route : public ListNode {
   public:
      class db_as : public ListNode {
      public:
	 char db[64];
	 // Modified by wlee@isi.edu
	 ASt as;
      public:
	 db_as() {}
	 db_as(char *_db,  ASt _as) {
	    strncpy(db, _db, sizeof(db));
	    as = _as;
	 }
      }; // End of class db_as

   public:
      PrefixRange    route;
      
      int    display_index;
      int    indentation;

      char   reg_status;
#define VR_REGISTERED_UNDTRMND  0x01
// The route has NEVER been registered in IRR
#define VR_REGISTERED_NO        0x02
// The route has been registered in ONE database
#define VR_REGISTERED_SINGLY    0x04
// The route has been registered in multiple database
#define VR_REGISTERED_MULTI     0x08
#define VR_REGISTERED_YES       (VR_REGISTERED_SINGLY | VR_REGISTERED_MULTI)
      char   rtd_status;
#define VR_ROUTED_UNDTRMND      0x01
// After reading BGP dump, the route is found in the dump
#define VR_ROUTED_YES       	0x02
// After reading BGP dump, the route is not found in the dump
#define VR_ROUTED_NO        	0x04
      char   hmd_status;
#define VR_HOMED_UNDTRMND       0x01
// The route is origin from the AS specific in roe command line
#define VR_HOMED_ME         	0x02
// The route is origin from the AS other than specific in roe command line
#define VR_HOMED_OTHER      	0x04
// The route is origin from more than one ASes, 
// one of them is specific in roe command line
#define VR_HOMED_MULTI_ME      	0x08
// The route is origin from more than one ASes,
// none of them is from the roe command line
#define VR_HOMED_MULTI_OTHER   	0x10
      char   status;
#define VR_IRR_QUERIED          0x01
#define VR_IRR_DONT_QUERY       0x02

      List<db_as> db_as_l;

      Route() {
	 display_index = -1;
	 indentation = 0; 

	 status     = 0;
	 reg_status = VR_REGISTERED_UNDTRMND;
	 rtd_status = VR_ROUTED_UNDTRMND;
	 hmd_status = VR_HOMED_UNDTRMND;
      }

      Route(const PrefixRange &i) : route(i) {
	 display_index = -1;
	 indentation = 0; 

	 status     = 0;
	 reg_status = VR_REGISTERED_UNDTRMND;
	 rtd_status = VR_ROUTED_UNDTRMND;
	 hmd_status = VR_HOMED_UNDTRMND;
      }
   }; // End of class Route

   static void get_registrations(ClientData clientdata);

private:
   static RouteList::Route *get_registration_response();
   static RouteList::Route *ask_registration();
   void get_registration_now(RouteList::Route *vr);
   static void parse_registration_response(char *response,RouteList::Route *vr);

   List<Route> rlist;
   Radix<Route>    radix;
   Route **vrdisplayed;
   int vrdisplayed_size;

   RadixSet real_routes;

   void get_registrations_init(Route *first);
   void get_more_specifics_add(RadixNode *rdnode);
   void display_i(RadixNode *rdnode);

   char *format(Route *vr);

public:
   RouteList() : irr_radix(NULL) { init(); }

  // Modified by wlee@isi.edu
  ASt as;
  //   char as[16];

   void display();
   void select(int index);

   void init(void);
   void load(ASt _as);
   void read_bgp_dump(char *fname);
   void get_more_specifics(char *selection);
   void get_less_specifics(char *selection);

   Route *operator() (int index) {
      return vrdisplayed[index];
   }

   // Added by wlee@isi.edu
   Route *get_response_next(void) { return vr_response_next; }

private:
   int index;
   int indent;
   Route *vr_query_next;
   Route *vr_response_next;
   Radix<Route> *irr_radix;

   // Modified by wlee from void to bool
   bool define_irr_radix();

   int matching_reg;
   int matching_rtd;
   int matching_hmd;
};

extern RouteList routelist;

#endif   // ROUTE_LIST_H
