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
#include <cstdlib>
#include <cstdarg>

extern "C" {
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif
}
#include <iostream>
extern "C" {
#include <tcl.h>
#include <tk.h>
}
#include <cstring>

#include "roe.hh"
#include "route-list.hh"
#include "object.hh"
#include "schedule.hh"
#include "irr/irr.hh"
#include "roe_config.hh"
#include <cstdarg>

int roe_show_reg_no            = 1;
int roe_show_reg_single        = 1;
int roe_show_reg_multi         = 1;
int roe_show_reg_undet         = 1;
int roe_show_routed_no         = 1;
int roe_show_routed_yes        = 1;
int roe_show_routed_undet      = 1;
int roe_show_homed_you         = 1;
int roe_show_homed_other       = 1;
int roe_show_homed_multi_me    = 1;
int roe_show_homed_multi_other = 1;
int roe_show_homed_undet       = 1;
int roe_show_potential_IBGP    = 1;
int roe_show_routes_hint       = 1;

// katie@ripe.net
#define DEFAULT_GEOMETRY "1000x500+20+20"

/*
 * Global variables. The Interpreter and a window that is not used :-)
 */

Tk_Window w;                    /* The main window for the application.  If
                                 * NULL then the application no longer
                                 * exists. */
Tcl_Interp *interp;      /* Interpreter for this application. */

#include <cstdarg>
// Added by wlee@isi.edu
extern IRR *whois;

inline void tcl_error_handle() {
   if (*interp->result)
      cerr << interp->result << endl;
   exit(1);
}
 
void tcl_Eval(char *format, ...) {
   char buffer[1024*16];

   va_list ap;
   va_start(ap, format);
   vsprintf(buffer, format, ap);
   va_end(ap);

   if (!Tcl_Eval(interp, buffer) == TCL_OK) 
      tcl_error_handle();
}

void tcl_EvalFile(char *file) {
   if (!Tcl_EvalFile(interp, file) == TCL_OK) 
      tcl_error_handle();
}

void tcl_delete_matching_lines(char *w, char *expr) {
   char position[64];
   strcpy(position, "1.0");
   while (tcl_Eval("%s search -nocase -regexp {%s} %s", w, expr, position),
	  interp->result && *interp->result) {
      strncpy(position, interp->result, sizeof(position));
      tcl_Eval("%s delete {%s linestart} {%s lineend + 1 char}", 
	       w, position, position);
   }
}

void tcl_delete_blank_lines(char *w) {
   tcl_delete_matching_lines(w, "^[ \t]*$");
}

// From route_list.cc
extern int irr_pending_replies;

int roe_statusbar_hint(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  if (argc != 1) {
    Tcl_SetResult(interp, "wrong # of args", TCL_STATIC);
    return TCL_ERROR;
  }

  if (irr_pending_replies && !routelist.get_response_next())
    tcl_Eval("Balloon:show .statusbar.right "
	     "{%d route objects not found from IRR}", 
	     irr_pending_replies);

  return TCL_OK;
}

static int roe_routes_hint_index = -1;

int roe_routes_hint_reset(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  roe_routes_hint_index = -1;
  return TCL_OK;
}

int roe_routes_hint(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  if (argc != 4) {
    Tcl_SetResult(interp, "wrong # of args", TCL_STATIC);
    return TCL_ERROR;
  }

  if (roe_show_routes_hint) {
    int index = atoi(argv[1]);
    int x = atoi(argv[2]);
    int y = atoi(argv[3]);
    if ((roe_routes_hint_index != index) && (index >= 0)) {
      char buffer1[64], buffer2[64];
    //    Prefask_map(routelist(index)->route).get_address(buffer);
      PrefixRange &prefask = routelist(index)->route;
      unsigned int prefix = prefask.get_ipaddr();
      unsigned int mask   = prefask.get_mask();
      unsigned int prefix_max = prefix | ~mask;
      roe_routes_hint_index = index;
      tcl_Eval("Balloon:show .routes.list {%s - %s} %d %d", 
	       int2quad(buffer1, prefix), int2quad(buffer2, prefix_max), x, y);
    }
  }

  return TCL_OK;
}

int roe_file_new_as(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  switch (argc)
    {
    case 1:
      if (routelist.get_response_next())
	tcl_Eval("showWarning {You can't do this at this moment, there are still some pending data!}");
      else
	tcl_Eval("new_as_command");
      break;
    case 2:
      routelist.init();
      routelist.load(atoi(argv[1] + 2));
      routelist.display();
      break;
    default:
      Tcl_SetResult(interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
    }

  return TCL_OK;
}

int roe_show_routes(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
   if (argc != 1) {
      Tcl_SetResult(interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }
   
   routelist.display();

   return TCL_OK;
}

int roe_select_route(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
   if (argc != 2) {
      Tcl_SetResult(interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   if (argv[1][0]) routelist.select(atoi(argv[1]));

   return TCL_OK;
}

int roe_show_object(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {

   if (argc != 4) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   // Modified by wlee@isi.edu
   object.display(argv[1], atoi(argv[2]), argv[3]);
   //   object.display(argv[1], argv[2], argv[3]);

   return TCL_OK;
}

int roe_read_bgp_dump(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {

   if (argc != 2) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   routelist.read_bgp_dump(argv[1]);

   return TCL_OK;
}

int roe_object_delete(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {

   if (argc != 2) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   object.del(atoi(argv[1]));

   return TCL_OK;
}

int roe_object_add(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) {

   if (argc != 2) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   object.add(atoi(argv[1]));

   return TCL_OK;
}

int roe_object_update(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {

   if (argc != 2) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   object.upd(atoi(argv[1]));

   return TCL_OK;
}

int roe_object_schedule(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {

   if (argc != 1) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   schedule.add();

   return TCL_OK;
}

int roe_select_transaction(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {

   if (argc != 2) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   schedule.select(atoi(argv[1]));

   return TCL_OK;
}

int roe_cancel_transaction(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {

   if (argc != 2) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   if (*(argv[1]))
      schedule.cancel(atoi(argv[1]));

   return TCL_OK;
}

int roe_add_selected(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {

   if (argc != 2) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   schedule.add_selected(argv[1]);

   return TCL_OK;
}

int roe_del_selected(ClientData clientData, Tcl_Interp *interp, 
		int argc, char **argv) {

   if (argc != 2) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   schedule.del_selected(argv[1]);

   return TCL_OK;
}

int roe_del_selected_matching_source(ClientData clientData, Tcl_Interp *interp, 
		int argc, char **argv) {

   if (argc != 3) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   schedule.del_selected_matching_source(argv[1], argv[2]);

   return TCL_OK;
}

int roe_config_tmpl(ClientData clientData, Tcl_Interp *interp, 
	       int argc, char **argv) {

   if (argc != 1) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   configure.config_tmpl();

   return TCL_OK;
}

int roe_config_tmpl_apply(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {

   if (argc != 1) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   configure.tmpl_apply();

   return TCL_OK;
}

int roe_config_irr(ClientData clientData, Tcl_Interp *interp, 
	       int argc, char **argv) {

   if (argc != 1) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   configure.config_irr();

   return TCL_OK;
}

int roe_config_irr_apply(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {

   if (argc != 1) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   configure.irr_apply();

   return TCL_OK;
}

int roe_config_revert(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
   
   if (argc != 1) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   configure.revert();

   return TCL_OK;
}

int roe_config_load(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
   
   if (argc != 1) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   configure.load();

   return TCL_OK;
}

int roe_config_save(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
   
   if (argc != 1) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   configure.save();

   return TCL_OK;
}

int roe_get_more_specifics(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
   
   if (argc != 2) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   routelist.get_more_specifics(argv[1]);

   return TCL_OK;
}

int roe_get_less_specifics(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
   
   if (argc != 2) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   routelist.get_less_specifics(argv[1]);

   return TCL_OK;
}

int roe_update_irr(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
   
   if (argc != 1) {
      Tcl_SetResult (interp, "wrong # of args", TCL_STATIC);
      return TCL_ERROR;
   }

   schedule.update_irr();

   return TCL_OK;
}

void tcl_create_commands() {
#define tcl_CreateCommand(x) \
   Tcl_CreateCommand(interp, #x, x, \
                     (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

   tcl_CreateCommand(roe_statusbar_hint);
   tcl_CreateCommand(roe_routes_hint_reset);
   tcl_CreateCommand(roe_routes_hint);
   tcl_CreateCommand(roe_file_new_as);
   tcl_CreateCommand(roe_show_routes);
   tcl_CreateCommand(roe_select_route);
   tcl_CreateCommand(roe_show_object);
   tcl_CreateCommand(roe_read_bgp_dump);
   tcl_CreateCommand(roe_object_add);
   tcl_CreateCommand(roe_object_delete);
   tcl_CreateCommand(roe_object_update);
   tcl_CreateCommand(roe_object_schedule);
   tcl_CreateCommand(roe_select_transaction);
   tcl_CreateCommand(roe_cancel_transaction);
   tcl_CreateCommand(roe_add_selected);
   tcl_CreateCommand(roe_del_selected);
   tcl_CreateCommand(roe_del_selected_matching_source);
   tcl_CreateCommand(roe_config_tmpl);
   tcl_CreateCommand(roe_config_tmpl_apply);
   tcl_CreateCommand(roe_config_irr);
   tcl_CreateCommand(roe_config_irr_apply);
   tcl_CreateCommand(roe_config_revert);
   tcl_CreateCommand(roe_config_load);
   tcl_CreateCommand(roe_config_save);
   tcl_CreateCommand(roe_get_more_specifics);
   tcl_CreateCommand(roe_get_less_specifics);
   tcl_CreateCommand(roe_update_irr);
}

void tcl_link_vars() {
#define tcl_LinkIntVar(x) \
   Tcl_LinkVar(interp, #x, (char *) &x, TCL_LINK_INT);

   tcl_LinkIntVar(roe_show_reg_no);
   tcl_LinkIntVar(roe_show_reg_single);
   tcl_LinkIntVar(roe_show_reg_multi);
   tcl_LinkIntVar(roe_show_reg_undet);
   tcl_LinkIntVar(roe_show_routed_no);
   tcl_LinkIntVar(roe_show_routed_yes);
   tcl_LinkIntVar(roe_show_routed_undet);
   tcl_LinkIntVar(roe_show_homed_you);
   tcl_LinkIntVar(roe_show_homed_other);
   tcl_LinkIntVar(roe_show_homed_multi_me);
   tcl_LinkIntVar(roe_show_homed_multi_other);
   tcl_LinkIntVar(roe_show_homed_undet);
   tcl_LinkIntVar(roe_show_potential_IBGP);
   tcl_LinkIntVar(roe_show_routes_hint);
}

RouteList routelist;
RoeObject object;
Schedule schedule;

void roe_initialize_GUI() {
   /*
    * If a display was specified, put it into the DISPLAY
    * environment variable so that it will be available for
    * any sub-processes created by us.
    */
   
   interp = Tcl_CreateInterp();
   if (display != NULL) {
      Tcl_SetVar2(interp, "env", "DISPLAY", display, TCL_GLOBAL_ONLY);
   }

   /*
    * Initialize the Tk application and arrange to map the main window
    * after the startup script has been executed, if any.  This way
    * the script can withdraw the window so it isn't ever mapped
    * at all.
    */

#ifdef HAVE_TK_CREATEMAINWINDOW
   w = Tk_CreateMainWindow(interp, display, "roe", "roe");
   if (w == NULL) {
      fprintf(stderr, "%s\n", interp->result);
      exit(1);
   }
   Tk_SetClass(w, "Roe");
   Tcl_AppInit(interp);
#else // HAVE_TK_CREATEMAINWINDOW
   Tcl_AppInit(interp);
   w = Tk_MainWindow(interp);
   Tk_SetClass(w, "Roe");
   tcl_Eval("wm title . roe");
#endif // HAVE_TK_CREATEMAINWINDOW
   
   /*
    * Set the geometry of the main window, if requested.  Put the
    * requested geometry into the "geometry" variable.
    */
   // modified a bit by katie@ripe.net to avoid spontaneus resizing
   if (geometry == NULL) 
      geometry = DEFAULT_GEOMETRY;
   Tcl_SetVar(interp, "geometry", geometry ,TCL_GLOBAL_ONLY);
   tcl_Eval("wm geometry . %s", geometry);
   
   /*
    * Set the "tcl_interactive" variable.
    */
   Tcl_SetVar(interp, "tcl_interactive", "0", TCL_GLOBAL_ONLY);

   tcl_create_commands();
   tcl_link_vars();
#ifdef LOAD_FILE
   tcl_EvalFile("fileselect.tk");
#else // LOAD_FILE
   extern char *fileselect_tk;
   tcl_Eval("%s", fileselect_tk);
#endif // LOAD_FILE

#ifdef LOAD_FILE
   tcl_EvalFile("utils.tk");
#else // LOAD_FILE
   extern char *utils_tk;
   tcl_Eval("%s", utils_tk);
#endif // LOAD_FILE

#ifdef LOAD_FILE
   tcl_EvalFile("layout.tk");
#else // LOAD_FILE
   extern char *layout_tk;
   tcl_Eval("%s", layout_tk);
#endif // LOAD_FILE
   char sources[256];
   strncpy(sources, whois->GetSources(), sizeof(sources));
   if (! *sources) 
      strcpy(sources, "radb,ripe,ans,cw,canet");
   for (char *s = strtok(sources, ","); s; s = strtok(NULL, ","))
      tcl_Eval(".menubar.selection.menu.sources add command -label %s -command { roe_del_selected_matching_source [.routes.list curselection] %s}", s, s);

   // added by wlee@isi.edu
   tcl_Eval("update");
}

/*
 * The Tcl_AppInit() function as suggested by J. Ousterhout.
 */

int Tcl_AppInit(Tcl_Interp *interp) {
   if (Tcl_Init(interp) == TCL_ERROR) {
      fprintf (stderr, "%s\n", interp->result);
      exit(1);
   }

   if (Tk_Init(interp) == TCL_ERROR) {
      fprintf (stderr, "%s\n", interp->result);
      return TCL_ERROR;
   }

#ifdef HAVE_BLT
   if (Blt_Init(interp) != TCL_OK) {
      return TCL_ERROR;
      fprintf (stderr, "%s\n", interp->result);
   }
#endif

#ifdef NEVER
   if (roe_Init(interp) == TCL_ERROR) {
      return TCL_ERROR;
   }
#endif

    return TCL_OK;
}
