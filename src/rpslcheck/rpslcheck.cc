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
//	       Katie Petrusha <katie@ripe.net>

#include <config.h>
#include <iostream>
#include "rpsl/object.hh"
#include "util/rusage.hh"
#include "util/debug.hh"
#include "irrutil/trace.hh"
#include "irrutil/Argv.hh"
#include "irrutil/version.hh"
#include "irr/irr.hh"
#include "irr/rawhoisc.hh"
#include "rpsl/schema.hh"

using namespace std;

Rusage ru;
bool opt_stats                   = false;
bool opt_rusage                  = false;
char *opt_prompt                 = (char *)"rpslcheck> ";
bool opt_echo                    = false;
bool opt_asdot                   = false;
char *opt_my_as			 = NULL;
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
     
     IRR_COMMAND_LINE_OPTIONS,

     {"-asdot", ARGV_BOOL, (char *) NULL, (char *) &opt_asdot,
      "print AS numbers in asdot format."},

     {"-rusage", ARGV_BOOL, (char *) NULL,           (char *) &opt_rusage,
      "On termination print resource usage"},
     {"-stats", ARGV_BOOL, (char *) NULL,            (char *) &opt_stats,
      "On termination print class statistics"},
     {"-prompt", ARGV_STRING,  (char *) NULL,        (char *) &opt_prompt,
      "Prompt"},
     {"-as",  ARGV_STRING,     (char *) NULL, (char *) &opt_my_as, 
      "AS number of the aut-num object to parse"},
     
     {"-echo", ARGV_BOOL, (char *) NULL,           (char *) &opt_echo,
      "Echo each object parsed"},
#ifdef DEBUG
     {"-debug_rpsl", ARGV_BOOL, (char *) NULL,     (char *) &opt_debug_rpsl,
      "Turn on bison debugging. Intended for developers."},
#endif // DEBUG

     {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL,
      (char *) NULL}
   };
  
   IRR::handleEnvironmentVariables(envp);

   if (ParseArgv(&argc, argv, argTable, ARGV_NO_LEFTOVERS) != ARGV_OK) {
      std::cerr << std::endl;
      exit(1);
   }

   if (! (irr = IRR::newClient())) {
      std::cerr << "Unknown protocol!" << std::endl;
      exit(1);
   }

   // have a prompt only if the input is coming from a tty
   if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
      opt_prompt = NULL;
}


int main(int argc, char **argv, char **envp) {
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
   int myAS;
   
   while (opt_my_as || cin ) {
       if (opt_my_as) {
          const char *dot = strchr(opt_my_as,'.');
          if (dot)
             myAS = atoi(opt_my_as + 2)<<16 | atoi(dot+1);
          else
	     myAS = atoi(opt_my_as + 2);
	  const AutNum *autnum = irr->getAutNum(myAS);
          if (!autnum)	{
          	std::cerr << "Error: no object for AS " << myAS << std::endl;
		exit(1);
	  }
	  o = new Object((Object &) *autnum);
          o->scan();
       }
       else {
	  o = new Object;
          code = o->read();
          if (code)
             code = o->scan();
       }

          if (opt_echo && code)
             std::cout << *o;

          if (!o->has_error && code)
          std::cout << "Congratulations no errors were found" << std::endl;

	  delete o;

	  if (opt_my_as)	{
		break;
	  }

   }

   if (opt_stats)
      schema.printClassStats();

   if (opt_prompt)
      std::cout << std::endl;

   if (opt_rusage)
      clog << ru;
}
