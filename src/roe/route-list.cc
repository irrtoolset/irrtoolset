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


#include "config.h"
#include "route-list.hh"
#include "object.hh"
#include <fstream.h>
#include "roe.hh"
#include "util/rusage.hh"
#include "irr/irr.hh"

// Added by wlee@isi.edu
extern IRR *whois;
extern IRR *RLWhois;


char *RouteList::format(RouteList::Route *vr) {
   static char line[256];
   char *c;

   for (c = line; c < line + vr->indentation; ++c)
      *c = ' ';

   vr->route.get_text(line + vr->indentation);
   for (c = line + strlen(line); c < line + 31; ++c)
      *c = ' ';
   *c = 0;
	 
   if (vr->reg_status & VR_REGISTERED_NO)
      strcat(line, " NotReg");
   else if (vr->reg_status & VR_REGISTERED_MULTI)
      strcat(line, "       ");
   else if (vr->reg_status & VR_REGISTERED_SINGLY)
      strcat(line, "       ");
   else 
      strcat(line, "  ---  ");


   if (vr->rtd_status & VR_ROUTED_NO)
      strcat(line, " NotRtd");
   else if (vr->rtd_status & VR_ROUTED_YES)
      strcat(line, "       ");
   else 
      strcat(line, "  ---  ");

   if (vr->hmd_status & VR_HOMED_OTHER)
      strcat(line, " OtrHmd");
   else if (vr->hmd_status & VR_HOMED_MULTI_ME)
      strcat(line, " MltHmd");
   else if (vr->hmd_status & VR_HOMED_MULTI_OTHER)
      strcat(line, " MHmOtr");
   else if (vr->hmd_status & VR_HOMED_ME)
      strcat(line, "       ");
   else 
      strcat(line, "  ---  ");

   strcat(line, "    ");

   RouteList::Route::db_as* p;
   for (p = vr->db_as_l.head(); p; p = vr->db_as_l.next(p))
      sprintf(line + strlen(line), "  %s:AS%d", p->db, p->as);

   return line;
}

void RouteList::display() {
   matching_reg = 0;
   matching_rtd = 0;
   matching_hmd = 0;

   matching_reg |= roe_show_reg_no            ? VR_REGISTERED_NO       : 0;
   matching_reg |= roe_show_reg_single        ? VR_REGISTERED_SINGLY   : 0;
   matching_reg |= roe_show_reg_multi         ? VR_REGISTERED_MULTI    : 0;
   matching_reg |= roe_show_reg_undet         ? VR_REGISTERED_UNDTRMND : 0;
             
   matching_rtd |= roe_show_routed_no         ? VR_ROUTED_NO           : 0;
   matching_rtd |= roe_show_routed_yes        ? VR_ROUTED_YES          : 0;
   matching_rtd |= roe_show_routed_undet      ? VR_ROUTED_UNDTRMND     : 0;
           
   matching_hmd |= roe_show_homed_you         ? VR_HOMED_ME            : 0;
   matching_hmd |= roe_show_homed_other       ? VR_HOMED_OTHER         : 0;
   matching_hmd |= roe_show_homed_multi_me    ? VR_HOMED_MULTI_ME      : 0;
   matching_hmd |= roe_show_homed_multi_other ? VR_HOMED_MULTI_OTHER   : 0;
   matching_hmd |= roe_show_homed_undet       ? VR_HOMED_UNDTRMND      : 0;

   index = 0;
   indent = 0;
   
   //cerr << "measure_now before display" << endl;
   //cerr << ru << endl;
   if (vrdisplayed_size != rlist.size() + 1) {
      vrdisplayed_size = rlist.size() + 1;
      if (vrdisplayed)
	 delete [] vrdisplayed;
      vrdisplayed = new (RouteList::Route*)[vrdisplayed_size];
   }

   tcl_Eval(".routes.list delete 0 end");

   display_i(&radix);

   vrdisplayed[index] = (RouteList::Route *) NULL;

   if (index) { // if there is at least one route, select it
      tcl_Eval(".routes.list selection set 0");
      select(0);
   }
   //cerr << "measure_now after display" << endl;
   //cerr << ru << endl;
}

void RouteList::display_i(RadixNode *rdnode) {
   int oldindent = indent;

   if (!rdnode)
      return;

   if (rdnode->state == RADIX_FULL) {
      RouteList::Route *vr = (Route *) rdnode->data;

      int potential_IBGP = 0;
      if (!roe_show_potential_IBGP) {
	 RouteList::Route *vr1 = vr;
	 RadixNode *parent = rdnode;

	 while (vr1 && parent && ! potential_IBGP && 
		vr1->reg_status & VR_REGISTERED_NO &&
		vr1->rtd_status & VR_ROUTED_YES) {
	    for (parent = parent->prnt; 
		 parent && parent->state != RADIX_FULL; 
		 parent = parent->prnt)
	       ;
	    if (parent) {
	       vr1 = (Route *)parent->data;
	       if (vr1)
		  if (vr1->reg_status & VR_REGISTERED_YES)
		     potential_IBGP = 1;
	    }
	 }
      }

      if (vr->reg_status & matching_reg
	  && vr->rtd_status & matching_rtd
	  && vr->hmd_status & matching_hmd
	  && ! potential_IBGP) {

	 vr->display_index = index;
	 vr->indentation   = indent;
	 vrdisplayed[index++] = vr;

	 tcl_Eval(".routes.list insert end {%s}", format(vr));
	 indent++;
      } else
	 vr->display_index = -1;
   }

   display_i(rdnode->left);
   display_i(rdnode->rght);
   indent = oldindent;
}

void RouteList::select(int index) {
   char buffer[1024];
   char buffer2[64];
   char *buffer3;
   char rc;
   int code;
   int bytes_read;
   RouteList::Route::db_as *p;

   RouteList::Route *vr = vrdisplayed[index];

   tcl_Eval("destroy_children .buttonbar");

   get_registration_now(vr);

   for (p = vr->db_as_l.head(); p; p = vr->db_as_l.next(p)) {
      tcl_Eval("button .buttonbar.a%s:%d -text {%s %d} -command { roe_show_object %s %d %s}", 
	       p->db, p->as, p->db, p->as, p->db, p->as, 
	       vr->route.get_text(buffer2));
      tcl_Eval("pack .buttonbar.a%s:%d  -side left", p->db, p->as);
   }

   p = vr->db_as_l.head();
   if (p)
      object.display(p->db, p->as, vr->route.get_text(buffer2));
   else
      object.clear();
}

void RouteList::get_registration_now(RouteList::Route *vr) {
   char buffer[64];
   char *response = NULL;

   if ((vr->status & VR_IRR_QUERIED) || (vr->status & VR_IRR_DONT_QUERY))
      return;

   // Modified by wlee@isi.edu
   //   whois->QueryKillResponse("!ufo=0");
   whois->setFullObjectFlag(false);
   //   whois->QueryResponse(response, "!r%s,o", vr->route.get_text(buffer));
   whois->getSourceOrigin(response, vr->route.get_text(buffer)); 

   if (!response) 
     {
     tcl_Eval("showWarning { No %s on IRR!}", buffer);
     return;
     }
   parse_registration_response(response, vr);
   delete [] response;

   if (vr->display_index != -1) {
      sprintf(buffer, "%d", vr->display_index);
      tcl_Eval(".routes.list delete %s", buffer);
      tcl_Eval(".routes.list insert %s {%s}", buffer, routelist.format(vr));
   }
}

// response contains all the (database,AS#) pair having the route vr->route
// For example, query sent to server:
//   !ufo=0
//   !r128.223.0.0/16,o
// Response from the server
//   MCI  AS3582
//   RADB  AS3582
void RouteList::parse_registration_response(char *response, 
					    RouteList::Route *vr) {
   char dbase[64], *line, *next;
   char *as;
   RouteList::Route::db_as *p, *fp;

   vr->status |= VR_IRR_QUERIED;

   next = response;

   while (*next) {
      line = next;
      extern char *my_strchr(char *s, int c);
      if (next = my_strchr(next, '\n')) {
	 *next = 0;
	 next++;
      }

      strcpy(dbase, strtok(line, " \t"));
      for (as = strtok(NULL, " \t"); as; as = strtok(NULL, " \t")) {
	 p = new RouteList::Route::db_as(dbase, atoi(as + 2));
	 vr->db_as_l.append(p);
      }      
   }

   if (vr->db_as_l.isEmpty())
      vr->reg_status = VR_REGISTERED_NO;
   else if (vr->db_as_l.isSingleton())
      vr->reg_status = VR_REGISTERED_SINGLY;
   else
      vr->reg_status = VR_REGISTERED_MULTI;

   fp = vr->db_as_l.head();
   if (fp) {
      int multi_homed = 0;
      int homed_to_me = 0;
      for (p = fp; p; p = vr->db_as_l.next(p)) {
       	 if (p->as != fp->as)       multi_homed = 1;
	 if (p->as == routelist.as) homed_to_me = 1;
      }
	 
      if (multi_homed && homed_to_me)
	 vr->hmd_status = VR_HOMED_MULTI_ME;
      else if (multi_homed)
	 vr->hmd_status = VR_HOMED_MULTI_OTHER;
      else if (homed_to_me)
	 vr->hmd_status = VR_HOMED_ME;
      else
	 vr->hmd_status = VR_HOMED_OTHER;
   }
}

// Make it global -- wlee@isi.edu
int irr_pending_replies;

RouteList::Route *RouteList::get_registration_response() {
   char dbase[64], buffer[1024], *response, *line, *next;
   char *as;
   char last_as[16];
   RouteList::Route *vr, *p;

   if (!routelist.vr_response_next)
      return NULL;
   for (vr = routelist.vr_response_next;
	vr && (vr->status & VR_IRR_DONT_QUERY);
	vr = routelist.rlist.next(vr))
      ;      

   if (vr) {
      for (p = routelist.rlist.next(vr);
	   p && (p->status & VR_IRR_DONT_QUERY);
	   p = routelist.rlist.next(p))
	 ;      
      routelist.vr_response_next = p;
      
      //      if (! RLWhois->Response(response))
      if (!RLWhois->getSourceOrigin(response)) {
	 return vr;
      }

      tcl_Eval(".statusbar.right configure -text {Pending Replies: %d}", 
	       --irr_pending_replies);

      if (! (vr->status & VR_IRR_QUERIED))
	 parse_registration_response(response, vr);
      delete [] response;
   } else
      routelist.vr_response_next = NULL;

   return vr;
}

RouteList::Route *RouteList::ask_registration() {
   RouteList::Route *vr;
   char buffer[64];

   if (!routelist.vr_query_next)
      return NULL;

   // Added by wlee@isi.edu
   //   if (!RLWhois.WriteReady()) return NULL;

   for (vr = routelist.vr_query_next;
	vr && (vr->status & VR_IRR_DONT_QUERY);
	vr = routelist.rlist.next(vr))
      ;      

   if (vr) {
      routelist.vr_query_next = routelist.rlist.next(vr);

      //      RLWhois->Query("!r%s,o", vr->route.get_text(buffer));
      RLWhois->querySourceOrigin(vr->route.get_text(buffer));

   } else 
      routelist.vr_query_next = NULL;

   return vr;
}


void RouteList::get_registrations(ClientData clientdata) {
   char buffer[1024];
   RouteList::Route *vr;

   if (! routelist.vr_response_next && ! routelist.vr_query_next)
      return;

   //   if (! RLWhois->PendingData()) {
   if (!RLWhois->readReady()) {

      tcl_Eval("update");
      Tk_DoWhenIdle(get_registrations, (ClientData) NULL);
      return;
   }

// Modified by wlee
// Send query before reading would improve the performance, but some servers
// couldn't handle bursty queries.  Read before sending queries again would be the right way.
//   ask_registration();

   if (vr = get_registration_response()) {
      if (vr->display_index != -1) {
	 sprintf(buffer, "%d", vr->display_index);
	 tcl_Eval(".routes.list delete %s", buffer);
	 tcl_Eval(".routes.list insert %s {%s}", buffer, routelist.format(vr));
      }

      if (vr->display_index == 0)
	 routelist.select(0);

      tcl_Eval("update");
   }

   // Moved by wlee
   ask_registration();

   Tk_DoWhenIdle(get_registrations, (ClientData) NULL);
}

#define PENDING_REQ_WINDOW 1

void RouteList::get_registrations_init(RouteList::Route *first) {
   RouteList::Route *vr1 = (RouteList::Route *) 1;
   RouteList::Route *vr;

   // Modified by wlee@isi.edu
   //   if (!RLWhois->is_open()) {
   //      RLWhois->QueryKillResponse("!ufo=0");
   //   }
   RLWhois->setFullObjectFlag(false);

   for (vr = first; vr; vr = rlist.next(vr))
      if (! (vr->status & VR_IRR_DONT_QUERY))
	 irr_pending_replies++;

   tcl_Eval(".statusbar.right configure -text {Pending Replies: %d}", 
	    irr_pending_replies);

   if (! vr_query_next && first) {
      vr_response_next = first;
      vr_query_next = first;
// ??? A quick hack for ripe
      if (IRR::GetProtocol() == IRR::ripe || IRR::GetProtocol() == IRR::bird)
         vr1 = ask_registration();
      else
	 // Quick hack end here
	 for (int i = 0; i < PENDING_REQ_WINDOW && vr1; ++i)
	    vr1 = ask_registration();

      // Changed by wlee@isi.edu
      //      get_registrations((ClientData) NULL);
      Tk_DoWhenIdle(get_registrations, (ClientData) NULL);
   }
}

void RouteList::init(void) {
  vrdisplayed = NULL;
  vrdisplayed_size = 0;
  as = 0;
  vr_query_next = NULL;
  vr_response_next = NULL;
  matching_reg = 0;
  matching_rtd = 0;
  matching_hmd = 0;
  if (irr_radix)
    {
    delete irr_radix;
    irr_radix = NULL;
    }
  real_routes.clear();
  rlist.unlink();
  radix.clear();
  irr_pending_replies = 0; 
}

void RouteList::load(ASt _as) {
  // Modified by wlee@isi.edu
  as = _as;
  //   strncpy(as, _as, sizeof(as));

   const PrefixRanges *registered_routes = whois->expandAS(as);

   if (!registered_routes) 	{
	tcl_Eval("showWarning { No object for AS%d}", as);
        return;
   }
	
   // Added by wlee
   if (registered_routes->isEmpty()) {
     tcl_Eval("showWarning { No route for AS%d}", as);
     return;
   }

   RouteList::Route *vr;
   for (int i = registered_routes->low(); 
	i < registered_routes->fence();
	i++)
     radix.insert(new RouteList::Route((*registered_routes)[i]));

   // below code is less eficient than above commented code
   // however user interface looks better
   // undo this once expand returns a RadixSet
   for (RadixNode *nd = &radix; nd; nd = nd->next())
      if (nd->state == RADIX_FULL)
	 rlist.append((Route *) nd->data);

   get_registrations_init(rlist.head());
}

void RouteList::read_bgp_dump(char *fname) {
   char r[1024];
   char *status;
   Pix rpix;
   RouteList::Route *vr, *tail;
   ifstream dump(fname);

   tail = rlist.tail();

   extern int readbgpdump(RadixSet &);
   extern FILE *bgpin;
   bgpin = fopen(fname, "r");
   if (! bgpin) {
      cerr << "File " << fname << " not found." << endl;
      return;
   }
   readbgpdump(real_routes);
   fclose(bgpin);

   for (vr = rlist.head(); vr; vr = rlist.next(vr))
      if (real_routes.contains(vr->route))
         vr->rtd_status = VR_ROUTED_YES;
      else
	 if (! (vr->rtd_status & VR_ROUTED_YES))
	    vr->rtd_status = VR_ROUTED_NO;

   if (!define_irr_radix()) return;

   PrefixRange pr;
   RadixSet::PrefixRangeIterator prIterator(&real_routes);
   for (bool b = prIterator.first(pr); b; b = prIterator.next(pr)) {
      if (! radix.find(&pr)){
	 vr = new RouteList::Route(pr);
	 vr->rtd_status = VR_ROUTED_YES;
	 
	 if (irr_radix->left || irr_radix->rght) { // has a node other than 0/0
	    if (! irr_radix->find(&vr->route)) {
	       vr->reg_status = VR_REGISTERED_NO;
	       vr->status = VR_IRR_DONT_QUERY;
	    }
	 } else {
	    vr->reg_status = VR_REGISTERED_UNDTRMND;
	 }
	 rlist.append(vr);
	 radix.insert(vr);
      }
   }

   display();
   
   if (tail)
      get_registrations_init(rlist.next(tail));
}

// Modified by wlee from void to bool
bool RouteList::define_irr_radix() {
   //cerr << "measure_now before expand(IRR_ROUTES)" << endl;
   //cerr << ru << endl;
   if (! irr_radix) {
     tcl_Eval("popup_message {This will take 3 to 5 mins to finish, during which, your roe will freeze.  Please be patient!}");

     const PrefixRanges *irr_routes = whois->expandRSSet("RS-ANY");

     // Added by wlee
     if (!irr_routes) return false;

     //cerr << "measure_now after expand(IRR_ROUTES)" << endl;
     //cerr << ru << endl;

     irr_radix = new Radix<Route>;

     for (int i = irr_routes->low(); i < irr_routes->fence(); i++) {
       irr_radix->insert(new RouteList::Route((*irr_routes)[i]));
     }

   }
   //cerr << "measure_now after define_irr_radix()" << endl;
   //cerr << ru << endl;

   return true;
}

void RouteList::get_more_specifics(char *selection) {
   RouteList::Route *vr;
   Route *tail;

   if (!define_irr_radix()) return;

   tail = rlist.tail();

   for (char *s = strtok(selection, " \t"); s; s = strtok(NULL, " \t")) {
      vr = vrdisplayed[atoi(s)];
      RadixNode *rdnode = irr_radix->find(&vr->route);
      if (rdnode) {
	 get_more_specifics_add(rdnode->left);
	 get_more_specifics_add(rdnode->rght);
      }
   }

   //cerr << "measure_now after get_more_specifics_add(t)" << endl;
   //cerr << ru << endl;
   if (tail != rlist.tail()) {
      display();

      if (tail)
	 get_registrations_init(rlist.next(tail));
   }
}

void RouteList::get_more_specifics_add(RadixNode *rdnode) {
   RouteList::Route *vr;

   if (!rdnode)
      return;

   PrefixRange *prefixRange = &((RouteList::Route *)rdnode->data)->route;

   if (rdnode->state == RADIX_FULL) { // is it actually a route in IRR
      RadixNode *node = radix.find(prefixRange); 

      if (!node || node->state == RADIX_EMPTY) { // is not in radix
	 vr = new RouteList::Route(*prefixRange);
	 rlist.append(vr);
	 if (!node)
	    radix.insert(vr);
	 else {
	    node->state |= RADIX_FULL;
	    node->data  = vr;
	 }
      }
   }

   if (rdnode->left)
      get_more_specifics_add(rdnode->left);
   if (rdnode->rght)
      get_more_specifics_add(rdnode->rght);
}

void RouteList::get_less_specifics(char *selection) {
   RouteList::Route *vr;
   Route *tail;
   RadixNode *irr_node;
   RadixNode *node;

   if (!define_irr_radix()) return;

   tail = rlist.tail();

   for (char *s = strtok(selection, " \t"); s; s = strtok(NULL, " \t")) {
      vr = vrdisplayed[atoi(s)];
      irr_node = irr_radix->find(&vr->route);
      node     = radix.find(&vr->route);
      if (irr_node && node) {
	 // add the parents of irr_node
	 for (irr_node = irr_node->prnt, node = node->prnt; 
	      irr_node; 
	      irr_node = irr_node->prnt, node = node->prnt) {
	    if (irr_node->state == RADIX_FULL 
		&& node->state == RADIX_EMPTY) { 
	       // the node exists in irr_radix but not in radix, so add it
	       PrefixRange *prefixRange = &((RouteList::Route *)irr_node->data)->route;
	       vr = new RouteList::Route(*prefixRange);
	       rlist.append(vr);
	       node->state |= RADIX_FULL;
	       node->data  = vr;
	    }
	 }
      }
   }

   if (tail != rlist.tail()) {
      display();

      if (tail)
	 get_registrations_init(rlist.next(tail));
   }
}
