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
#include <cstdlib>

extern "C" {
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif
#include <tcl.h>
#include <tk.h>
}
#include <iostream.h>
#include <cstring>
#include <cctype>

#include "util/net.hh"      // for class ipAddr
#include "util/debug.hh"
#include "util/trace.hh"
#include "util/rusage.hh"
#include "util/version.hh"
#include "util/Argv.hh"
#include "irr/irr.hh"
#include "roe_tcl.hh"
#include "route-list.hh"
#include "roe_config.hh"
// Added by wlee@isi.edu
#include "irr/rawhoisc.hh"
#include "irr/ripewhoisc.hh"
#include "irr/birdwhoisc.hh"
#include "rpsl/rpsl.hh"     // For ASt
#include "rpsl/schema.hh"   // For schema

char *dispatcher;
char *programName = "";

// Modified by wlee@isi.edu
IRR *whois;
// roe needs another rawhoisd connection
IRR *RLWhois;

bool opt_rusage        = false;
Rusage ru(clog, &opt_rusage);
Configure configure;

char *opt_my_as        = NULL;
char *opt_rcfile       = ".roerc";
char opt_default_rcfile[256] = "";
char *display  = NULL;
char *geometry = NULL;
ASt myAS;


void usage(char *argv[]) {
   cerr << "Usage: " << argv[0] << " to be written later" << endl;
   exit(1);
}

int start_tracing(char *dst, char *key, char *nextArg) {
   if (nextArg) {
      trace.enable(nextArg);
      return 1; // return 1 to signify nextArg is used by us
   }
   return 0; 
}

int start_debugging(char *dst, char *key, char *nextArg) {
   if (nextArg) {
      Debug(dbg.enable(atoi(nextArg)));
      return 1; // return 1 to signify nextArg is used by us
   }
   return 0;
}

void init_and_set_options (int argc, char **argv, char **envp) {
   ArgvInfo argTable[] = {
      // RAToolSet common arguments
      // key, type, src, dst, help
      {"-T", ARGV_FUNC, (char *) &start_tracing,      (char *) NULL, 
       "Start tracing the next argument"},
      {"-D", ARGV_FUNC, (char *) &start_debugging,    (char *) NULL, 
       "Start debugging the next argument"},
      {"-version", ARGV_FUNC, (char *) &version,      (char *) NULL,
       "Show version"},
      {"-rusage", ARGV_BOOL, (char *) NULL,          (char *) &opt_rusage,
       "On termination print resource usage"},

      IRR_COMMAND_LINE_OPTIONS,

      // roe specific arguments
      {"-as",  ARGV_STRING,    (char *) NULL,    (char *) &opt_my_as,
       "AS number of the aut-num object to use."},
      {"-rcfile", ARGV_STRING, (char *)NULL, (char *)&opt_rcfile,
       "roe resource file"},
      {"-display", ARGV_STRING, (char *)NULL, (char *)&display,
       "X display"},
      {"-geometry", ARGV_STRING, (char *)NULL, (char *)&geometry,
       "Geometry for xterm"},

      {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL, (char *) NULL}
   };

   IRR::handleEnvironmentVariables(envp);

   // Extra env. variable checking other than defaults
   char *p = getenv("HOME");
   if (p) 
     {
     strcpy(opt_default_rcfile, p);
     strcat(opt_default_rcfile, "/.roerc");
     opt_rcfile = opt_default_rcfile;
     }

   if (ParseArgv(&argc, argv, argTable, 0) != ARGV_OK) {
      cerr << endl;
      exit(1);
   }

   // if there are remaining arguments
   // the first one is my as number
   switch (argc) {
   case 2 :
      opt_my_as = argv[1];
      break;
   case 1:
      break;
   default:
      cerr << "Wrong number of arguments..." << endl;
      exit(-1);
   }

  if (! (whois = IRR::newClient())) {
      cerr << "Unknown protocol!" << endl;
      exit(1);
   }

   if (! (RLWhois = IRR::newClient())) {
      cerr << "Unknown protocol!" << endl;
      exit(1);
   }

   if (opt_my_as)
     myAS = atoi(opt_my_as + 2);
   else {
      ipAddr myip;
      myip.setToMyHost();
      if ((myAS = whois->getOrigin("%s/32", myip.getIpaddr())) == 0)
	{
	cerr << "roe could not guess your AS number, "
	        "please specify an AS number on the command line." 
	     << endl;
	delete whois;
	delete RLWhois;
	exit(1);
	}
   }      
}


main (int argc, char **argv, char **envp) {

   schema.initialize();

   init_and_set_options(argc, argv, envp);

   configure.load();

   // Invoke tcl and initialize GUI
   roe_initialize_GUI();

   routelist.load(myAS);
   routelist.display();

   Tk_MainLoop();
   Tcl_DeleteInterp(interp);

   delete whois;
   delete RLWhois;
}
