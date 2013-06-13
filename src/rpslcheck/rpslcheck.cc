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
//	       Katie Petrusha <katie@ripe.net>

#include <config.h>
#include <iostream>
#include "rpsl/object.hh"
#include "irrutil/rusage.hh"
#include "irrutil/debug.hh"
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
#ifdef DTAG
bool opt_asn16                   = false;
#endif /* DTAG */
bool opt_asdot                   = false;
char *opt_my_as			 = NULL;
#ifdef ENABLE_DEBUG
bool opt_debug_rpsl              = false;
#endif // ENABLE_DEBUG

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

#ifdef DTAG
     {"-asn16", ARGV_BOOL, (char *) NULL, (char *) &opt_asn16,
      "translate 32bit AS numbers into AS23456."},
#endif /* DTAG */

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
#ifdef ENABLE_DEBUG
     {"-debug_rpsl", ARGV_BOOL, (char *) NULL,     (char *) &opt_debug_rpsl,
      "Turn on bison debugging. Intended for developers."},
#endif // ENABLE_DEBUG

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

#ifdef ENABLE_DEBUG
   extern int rpsldebug;
   if (opt_debug_rpsl)
      rpsldebug = 1;
#endif // ENABLE_DEBUG

   Object *o;
   bool code = true;
   int myAS;
   
   while (opt_my_as || cin ) {
       if (opt_my_as) {
          // if the first two characters of the ASN are "as", then ignore them
#ifdef DTAG
          while (isspace(*opt_my_as) && *opt_my_as != '\0') opt_my_as++;
          if (strncasecmp(opt_my_as, "as", 2) == 0)
            opt_my_as += 2;
#else
          if (strcasestr(opt_my_as, "as") == opt_my_as)
            opt_my_as += 2;
#endif /* DTAG */

          const char *dot = strchr(opt_my_as,'.');
          if (dot)
             myAS = atoi(opt_my_as)<<16 | atoi(dot+1);
          else
	     myAS = atoi(opt_my_as);
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
