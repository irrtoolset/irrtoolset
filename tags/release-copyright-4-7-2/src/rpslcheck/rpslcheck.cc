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

#include <config.h>
#include <istream.h>
#include "rpsl/object.hh"
#include "util/rusage.hh"
#include "util/debug.hh"
#include "util/trace.hh"
#include "util/Argv.hh"
#include "util/version.hh"
#ifdef IRR_NEEDED
#include "irr/irr.hh"
#include "irr/rawhoisc.hh"
#endif // IRR_NEEDED
#include "rpsl/schema.hh"

Rusage ru;
bool opt_stats                   = false;
bool opt_rusage                  = false;
char *opt_prompt                 = "RtConfig> ";
bool opt_echo                    = false;
#ifdef DEBUG
bool opt_debug_rpsl              = false;
#endif // DEBUG

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
#ifdef IRR_NEEDED
     {"-h", ARGV_FUNC, (char *) &IRR::ArgvHost,      (char *) NULL,
      "Host name of the RAWhoisd server"},
     {"-p", ARGV_FUNC, (char *) &IRR::ArgvPort,      (char *) NULL,
      "Port number of the RAWhoisd server"},
     {"-s", ARGV_FUNC, (char *) &IRR::ArgvSources,   (char *) NULL,
      "Order of databases"},
     {"-ignore_errors", ARGV_FUNC, (char *)&IRR::IgnoreErrors, (char *)NULL,
      "Ignore IRR error and warning messages"},
     {"-report_errors", ARGV_FUNC, (char *)&IRR::ReportErrors, (char *)NULL,
      "Print IRR error and warning messages"},
#endif // IRR_NEEDED
     {"-rusage", ARGV_BOOL, (char *) NULL,           (char *) &opt_rusage,
      "On termination print resource usage"},
     {"-stats", ARGV_BOOL, (char *) NULL,            (char *) &opt_stats,
      "On termination print class statistics"},
     {"-prompt", ARGV_STRING,  (char *) NULL,        (char *) &opt_prompt,
      "Prompt"},
     
     {"-echo", ARGV_BOOL, (char *) NULL,           (char *) &opt_echo,
      "Echo each object parsed"},
#ifdef DEBUG
     {"-debug_rpsl", ARGV_BOOL, (char *) NULL,     (char *) &opt_debug_rpsl,
      "Turn on bison debugging. Intended for developers."},
#endif // DEBUG
     {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL,
      (char *) NULL}
   };
  
#ifdef IRR_NEEDED
   IRR::handleEnvironmentVariables(envp);
#endif // IRR_NEEDED

   if (ParseArgv(&argc, argv, argTable, ARGV_NO_LEFTOVERS) != ARGV_OK) {
      cerr << endl;
      exit(1);
   }

#ifdef IRR_NEEDED
   irr = IRR::newClient();
#endif // IRR_NEEDED

   // have a prompt only if the input is coming from a tty
   if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
      opt_prompt = NULL;
}


main(int argc, char **argv, char **envp) {
   schema.initialize();
   init_and_set_options(argc, argv, envp);
   schema.beHarsh();

   // opt_echo = 1;

#ifdef DEBUG
   extern int rpsldebug;
   if (opt_debug_rpsl)
      rpsldebug = 1;
#endif // DEBUG

   Object *o;
   bool code = true;
   int count = 0;
   while (cin) {
      o = new Object;
      code = o->read();
      if (code)
	 code = o->scan();

      if (opt_echo && code)
	 cout << *o;

      delete o;
   }

   if (opt_stats)
      schema.printClassStats();

   if (opt_prompt)
      cout << endl;

   if (opt_rusage)
      clog << ru;
}
