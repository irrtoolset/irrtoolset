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
#include <cstring>
#include "schedule.hh"
#include "roe_tcl.hh"
#include "route-list.hh"
#include "roe_config.hh"
#include "irr/irr.hh"
#include "irr/route.hh"  // For class Route

// Added by wlee@isi.edu
extern IRR *whois;

void Schedule::add() {
   tcl_delete_blank_lines(".object.text");
   tcl_Eval(".object.text get 1.0 end");
   add_internal(interp->result);
   select(schedule.size() - 1);
   tcl_Eval(".object.text configure -state disabled");
}

void Schedule::add_internal(char *text) {
   extern Transaction *roe_parser_trns;
   extern int roe_lexer_input_size;
   extern char *roe_lexer_input;
   extern int roeparse();
   Transaction *trns = new Transaction;
   Transaction *tr;
   char index[16];

   strncpy(trns->text, text, sizeof(trns->text));

   roe_lexer_input = trns->text;
   roe_lexer_input_size = strlen(trns->text);
   roe_parser_trns = trns;
   roeparse();
   
   int c1, c2, c3;

   if (*trns->source && *trns->as && *trns->route) {

      for (tr = schedule.head(); tr; tr = schedule.next(tr)) {
	 c1 = strcmp(tr->source, trns->source);
	 c2 = strcmp(tr->as,     trns->as);
	 c3 = strcmp(tr->route,  trns->route);

	 if (! (c1 < 0
		|| c1 == 0 && c2 < 0
		|| c1 == 0 && c2 == 0 && c3 < 0))
	    break;
      }

      if (tr) {
	 schedule.insertBefore(tr, trns);
	 if (c1 == 0 && c2 == 0 && c3 == 0) { // already scheduled
	    trns->display_position = tr->display_position;
	    sprintf(index, "%d", trns->display_position);
	    tcl_Eval(".schedule.list delete %s", index);
	    schedule.remove(tr);
	    delete tr;
	 } else {
	    trns->display_position = schedule.size() - 1;
	    strcpy(index, "end");
	 }
      } else {
	 schedule.append(trns);
	 trns->display_position = schedule.size() - 1;
	 strcpy(index, "end");
      }

      tcl_Eval(".schedule.list insert %s {%s %s %s %s (%s)}", index, 
	       trns->op_code == OP_DEL ? "Del" : "   ",
	       trns->source, trns->route, trns->as, trns->mnt_by);
   }
}

void Schedule::select(int index) {
   tcl_Eval(".object.text configure -state normal");

   tcl_Eval(".object.text delete 1.0 end");
   tcl_Eval("destroy_children .buttonbar");
   tcl_Eval(".schedule.list selection set %d", index);

   Transaction *tr;

   for (tr = schedule.head(); 
	tr && tr->display_position != index; 
	tr = schedule.next(tr))
      ;

   if (tr)
      tcl_Eval(".object.text insert 1.0 {%s}", tr->text);

   tcl_Eval(".object.text configure -state disabled");
   tcl_Eval(".object.buttons.delete configure -state disabled");
   tcl_Eval(".object.buttons.update configure -state disabled");
   tcl_Eval(".object.buttons.schedule configure -state disabled");
}

void Schedule::cancel(int index) {
   tcl_Eval(".object.text configure -state normal");
   tcl_Eval(".object.text delete 1.0 end");
   tcl_Eval("destroy_children .buttonbar");

   tcl_Eval(".schedule.list delete %d", index);

   for (Transaction *tr = schedule.head(); tr; )
      if (tr->display_position == index) {
	 Transaction *trns = schedule.next(tr);
	 schedule.remove(tr);
	 delete tr;
	 tr = trns;
      } else {
	 if (tr->display_position > index)
	    tr->display_position--;
	 tr = schedule.next(tr);
      }
   tcl_Eval(".object.text configure -state disabled");
}

void Schedule::add_selected(char *selection) {
   RouteList::Route *r;
   char buffer[64];

   for (char *s = strtok(selection, " \t"); s; s = strtok(NULL, " \t")) {
      r = routelist(atoi(s));

	  asnum_string(buffer, routelist.as);
	  tcl_Eval("set AS %s", buffer);
      tcl_Eval("set ROUTE %s", r->route.get_text(buffer));
      tcl_Eval("set DATE %s", configure.date);
      tcl_Eval("subst \"%s\"", configure.add_template);
      if (interp->result && *interp->result)
	 add_internal(interp->result);
   }
}

void Schedule::del_selected(char *selection) {
   RouteList::Route *r;
   RouteList::Route::db_as *p;
   Route *route = NULL;
   char buffer[64];

   for (char *s = strtok(selection, " \t"); s; s = strtok(NULL, " \t")) {
      r = routelist(atoi(s));
      for (p = r->db_as_l.head(); p; p = r->db_as_l.next(p)) {
	 // Modified by wlee@isi.edu
	 //	 whois->QueryKillResponse("!ufo=1");
	 //	 whois->QueryKillResponse("!uF=0");
	 whois->setFullObjectFlag(true);
	 whois->setFastResponseFlag(false);
	 whois->SetSources(p->db);
	 // Modified by wlee@isi.edu
	 /*
	 int code = whois->QueryResponse(response, "!mrt,%s-%s", 
					 r->route.get_text(buffer),
					 p->as);
	 */
	 whois->getRoute(route, r->route.get_text(), p->as);

	 whois->SetSources();

         // Modified by wlee@isi.edu
	 //	 if (code)
	 if (route) {
		asnum_string(buffer, routelist.as);
	    tcl_Eval("set AS %s", buffer);
	    tcl_Eval("set ROUTE %s", r->route.get_text());
	    tcl_Eval("set DATE %s", configure.date);
	    tcl_Eval("subst \"%.*s%s\"", 
		     route->size, route->contents, configure.del_template);
	    if (interp->result && *interp->result)
	       add_internal(interp->result);
	    tcl_Eval("update");
	    delete route;
    	 }
      }
   }
}

void Schedule::del_selected_matching_source(char *selection, char *dbase) {
   RouteList::Route *r;
   RouteList::Route::db_as *p;
   Route *route = NULL;
   char buffer[64];

   for (char *s = strtok(selection, " \t"); s; s = strtok(NULL, " \t")) {
      r = routelist(atoi(s));
      for (p = r->db_as_l.head(); p; p = r->db_as_l.next(p)) {
	 if (!strcmp(dbase, p->db)) {
	    // Modified by wlee@isi.edu
	    //	 whois->QueryKillResponse("!ufo=1");
	    //	 whois->QueryKillResponse("!uF=0");
	    whois->setFullObjectFlag(true);
	    whois->setFastResponseFlag(false);
	    whois->SetSources(p->db);
	    // Modified by wlee@isi.edu
	    /*
	    int code = whois->QueryResponse(response, "!mrt,%s-%s", 
					    r->route.get_text(buffer),
					    p->as);
	    */
	    whois->getRoute(route, r->route.get_text(), p->as);

	    whois->SetSources();

	    // Modified by wlee@isi.edu
	    if (route) {
		   asnum_string(buffer, routelist.as);
	       tcl_Eval("set AS %s", buffer);
	       tcl_Eval("set ROUTE %s", r->route.get_text());
	       tcl_Eval("set DATE %s", configure.date);
	       tcl_Eval("subst \"%.*s%s\"", 
			route->size, route->contents, configure.del_template);
	       if (interp->result && *interp->result)
		  add_internal(interp->result);
	       tcl_Eval("update");
	       delete route;
	    }
	 }
      }
   }
}

void Schedule::update_irr() {
   char current_source[64];
   char current_mnt_by[64];
   Transaction *tr, *tr_next;

#ifdef LOAD_FILE
   tcl_EvalFile("update_irr.tk");
#else // LOAD_FILE
   extern char *update_irr_tk;
   tcl_Eval("%s", update_irr_tk);
#endif // LOAD_FILE

   while (tr = schedule.head()) {
      strncpy(current_source, tr->source, sizeof(current_source));
      strncpy(current_mnt_by, tr->mnt_by, sizeof(current_mnt_by));

      tcl_Eval("update_irr_setup %s %s",  current_source, current_mnt_by);
      tcl_Eval(".update_irr_%s_%s.cmd.label configure -text {Update command for %s:}", current_source, current_mnt_by, current_source);
      tcl_Eval(".update_irr_%s_%s.cmd.entry insert 0 {%s}", current_source, current_mnt_by, configure.dbfind(current_source));

      for (; tr; tr = tr_next) {
	 tr_next = schedule.next(tr);
	 if (! strncmp(current_source, tr->source, sizeof(current_source))
	     && ! strncmp(current_mnt_by, tr->mnt_by, sizeof(current_mnt_by))) {
	    tcl_Eval(".update_irr_%s_%s.body.text insert end {\n%s\n}", current_source, current_mnt_by, tr->text);
	    schedule.remove(tr);
	    delete tr;
	 }
      }
   }

   tcl_Eval(".schedule.list delete 0 end");
}
