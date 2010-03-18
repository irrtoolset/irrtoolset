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
#include <cstring>
#include <fstream.h>
#include <ctime>

#include "roe_config.hh"
#include "roe_tcl.hh"
#include "roe.hh"
#include "irr/irr.hh"

#define DEFAULT_DBUPDATE_COMMAND_COUNTS 5

// Added by wlee@isi.edu
extern IRR *whois;

void Configure::revert() {
   class DBUpdate default_dbupdate[DEFAULT_DBUPDATE_COMMAND_COUNTS+1] = {
      "ANS",   "mail auto-dbm@ans.net",
      "MCI",   "mail auto-rr@mci.net",
      "CANET", "mail auto-dbm@canet.net",
      "RIPE",  "mail auto-dbm@ripe.net",
      "RADB",  "mail auto-dbm@radb.ra.net",
      "", ""
   };

   strncpy(add_template, "route: $ROUTE\norigin: $AS\ndescr: foo\nnotify: foo@bar\nmnt-by: MAINT-FOO\nchanged: foo@bar $DATE\nsource: bar\n", sizeof(add_template));
   strncpy(upd_template, "changed: foo@bar $DATE\n", sizeof(upd_template));
   strncpy(del_template, "delete: foo@bar (via ROE)\n", sizeof(del_template));
   
   int index;
   for (index = 0; *default_dbupdate[index].source; ++index) {
      strncpy(dbupdate[index].source, default_dbupdate[index].source, 
	      sizeof(dbupdate[index].source));
      strncpy(dbupdate[index].update, default_dbupdate[index].update, 
	      sizeof(dbupdate[index].update));
   }
   *dbupdate[index].source = 0;
   *dbupdate[index].update = 0;
}

void Configure::load() {
   char s[1024], *p;
   ifstream in(opt_rcfile);
   int index = 0;
   int size;
   time_t t;

   time(&t);
   strftime(date, sizeof(date), "%y%m%d", localtime(&t));

   if (in) {
      while (in.getline(s, 1024)) {
	 if (!*s || *s == '#')
	    continue;
	 if (strncmp(s, "add_template", 12) == 0)  { // trace
	    size = atoi(s + 12) + 1;
	    in.get(add_template, size, 0);
	    continue;
	 }
	 if (strncmp(s, "upd_template", 12) == 0)  { // trace
	    size = atoi(s + 12) + 1;
	    in.get(upd_template, size, 0);
	    continue;
	 }
	 if (strncmp(s, "del_template", 12) == 0)  { // trace
	    size = atoi(s + 12) + 1;
	    in.get(del_template, size, 0);
	    continue;
	 }
	 extern char *my_strchr(char *s, int c);
	 if (p = my_strchr(s, '=')) {
	    *p = 0;
	    p++;
	    strncpy(dbupdate[index].source, s, sizeof(dbupdate[index].source));
	    strncpy(dbupdate[index].update, p, sizeof(dbupdate[index].update));
	    index++;
	 }
      }
      *dbupdate[index].source = 0;
      *dbupdate[index].update = 0;
   } else {
      revert();
      index = DEFAULT_DBUPDATE_COMMAND_COUNTS; 
   }

   char sources[256];
   char *src;
   int j;
   strncpy(sources, whois->GetSources(), sizeof(sources));
   for (src = strtok(sources, ","); 
	src && index < size; 
	src = strtok(NULL, ",")) {
      for (j = 0; j < index && strcmp(src, dbupdate[j].source); ++j)
	 ;
      if (j == index) {
	 strncpy(dbupdate[index].source, src, sizeof(dbupdate[index].source));
	 *dbupdate[index].update = 0;
	 index++;
      }
   }
   *dbupdate[index].source = 0;
   *dbupdate[index].update = 0;
}

void Configure::save() {
   ofstream out(opt_rcfile);
   int index = 0;

   if (out) {
      out << "# It is not a good idea to edit this file\n";
      out << "# If byte counts dont match, you are in trouble.\n";
      out << "add_template " << strlen(add_template) << "\n"
	  << add_template << "\n";
      out << "upd_template " << strlen(upd_template) << "\n"
	  << upd_template << "\n";
      out << "del_template " << strlen(del_template) << "\n"
	  << del_template << "\n";
      for (index = 0; *dbupdate[index].source; index++)
	 out << dbupdate[index].source << "=" << dbupdate[index].update << "\n";
   }
}

char *Configure::dbfind(char *source) {
   int i;
   for (i = 0; *dbupdate[i].source && strcmp(dbupdate[i].source, source); ++i)
      ;

   return dbupdate[i].update;
}

void Configure::config_tmpl() {
   tcl_Eval("
if {[winfo exists .config_tmpl]} {
   wm deiconify .config_tmpl
   set a 1
} else {set a 0} 
");

   if (*(interp->result) != '0') // already displaying a config window
      return;

#ifdef LOAD_FILE
   tcl_EvalFile("config_tmpl.tk");
#else // LOAD_FILE
   extern char *config_tmpl_tk;
   tcl_Eval("%s", config_tmpl_tk);
#endif // LOAD_FILE

   tcl_Eval(".config_tmpl.add_text insert 1.0 {%s}", add_template);
   tcl_Eval(".config_tmpl.del_text insert 1.0 {%s}", del_template);
   tcl_Eval(".config_tmpl.upd_text insert 1.0 {%s}", upd_template);
}

void Configure::tmpl_apply() {
   tcl_delete_blank_lines(".config_tmpl.add_text");
   tcl_delete_blank_lines(".config_tmpl.del_text");
   tcl_delete_blank_lines(".config_tmpl.upd_text");

   tcl_Eval(".config_tmpl.add_text get 1.0 end");
   strncpy(add_template, interp->result, sizeof(add_template));
   tcl_Eval(".config_tmpl.del_text get 1.0 end");
   strncpy(del_template, interp->result, sizeof(del_template));
   tcl_Eval(".config_tmpl.upd_text get 1.0 end");
   strncpy(upd_template, interp->result, sizeof(upd_template));
}

void Configure::config_irr() {
   tcl_Eval("
if {[winfo exists .config_irr]} {
   wm deiconify .config_irr
   set a 1
} else {set a 0} 
");

   if (*(interp->result) != '0') // already displaying a config window
      return;

#ifdef LOAD_FILE
   tcl_EvalFile("config_irr.tk");
#else // LOAD_FILE
   extern char *config_irr_tk;
   tcl_Eval("%s", config_irr_tk);
#endif // LOAD_FILE

   for (int i = 0; *dbupdate[i].source; ++i) 
      tcl_Eval("label .config_irr.source.label.x%s -text %s:
entry .config_irr.source.entry.x%s -relief sunken -bd 2 -width 30
pack .config_irr.source.label.x%s -side top -anchor e -ipady 3 -expand true
pack .config_irr.source.entry.x%s -side top -fill both -expand 1 -pady 1 -padx 1
.config_irr.source.entry.x%s insert 0 {%s}", 
	       dbupdate[i].source, dbupdate[i].source, dbupdate[i].source, 
	       dbupdate[i].source, dbupdate[i].source, dbupdate[i].source, 
	       dbupdate[i].update);
}

void Configure::irr_apply() {
   for (int i = 0; *dbupdate[i].source; ++i) {
      tcl_Eval(".config_irr.source.entry.x%s get", dbupdate[i].source);
      strncpy(dbupdate[i].update, interp->result, sizeof(dbupdate[i].update));
   }
}
