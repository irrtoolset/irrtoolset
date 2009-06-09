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
#include <cstring>

extern "C" {
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif
}
#include <iostream>
#include <iomanip>

#include <cstdio>

#if HAVE_LIBREADLINE && HAVE_LIBHISTORY
extern "C" {
extern char *readline (char *);
extern void add_history (char *);
}
#endif // HAVE_LIBREADLINE and HAVE_LIBHISTORY

#include "irr/irr.hh"
#include "irr/rawhoisc.hh"
#include "irrutil/debug.hh"
#include "irrutil/trace.hh"
#include "irrutil/rusage.hh"
#include "irrutil/Argv.hh"
#include "irrutil/version.hh"
#include "rpsl/schema.hh"
#include "normalform/NE.hh"
#include "re2dfa/regexp_nf.hh"

using namespace std;

#define EXPAND_ASSets         0x000001
#define EXPAND_RSSets         0x000002
#define EXPAND_AS             0x000004
#define EXPAND_NOTHING        0
#define EXPAND_ALL            (~0)

bool opt_rusage                  = false;
Rusage ru(clog, &opt_rusage);

char *opt_prompt                 = (char *)"peval> ";
int  opt_expand                  = EXPAND_ALL;
int  opt_symbolic                = 0;
bool opt_asdot                   = false;

const int SIZE = 8*1024;
char base[SIZE] = "peval: ";
char temp[SIZE];
char safe_base[SIZE];
char *filter;
char *cut;

void evaluate() {
   if (opt_expand & EXPAND_ASSets)
      regexp_nf::expandASSets();

   strcat(filter, "\n\n");
   // Was: safe_base = base;
   memcpy(safe_base, base, SIZE);

   cut = strstr(filter, "afi");
   if (cut && isspace(*(cut+3))) {
     strcat (temp, "mp-");
     strcat (temp, base);
     // Was: base = temp;
     memcpy(base, temp, SIZE);
     // Was: bzero(temp, SIZE);
     memset(temp, '\0', SIZE);
   }

   Object *o = new Object;
   o->scan(base, strlen(base));
   if (o->has_error) {
      delete o;
      // Was: base = safe_base;
      memcpy(base, safe_base, SIZE);
      return;
   }

   if (strcmp(o->type->getName(), "mp-peval") == 0) {

     AttrIterator<AttrMPPeval> itr(o, "mp-peval");

     NormalExpression *ne = 
        NormalExpression::evaluate(itr()->filter, ~0, opt_expand);

     if (ne)
        cout << *ne << endl;
     else
        cerr << "Error: Internal error." << endl;
	
     delete ne;
     delete o;

   } else {
 
   AttrIterator<AttrFilter> itr(o, "peval");

   NormalExpression *ne = 
      NormalExpression::evaluate(itr()->filter, ~0, opt_expand);

   if (ne)
      cout << *ne << endl;
   else
      cerr << "Error: Internal error." << endl;
  
   delete ne;
   delete o;

   }
   // Was: base = safe_base;
   memcpy(base, safe_base, SIZE);
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

int skipASSets(char *dst, char *key, char *nextArg) {
   opt_expand &= ~EXPAND_ASSets;
   return 0;
}

int skipRSSets(char *dst, char *key, char *nextArg) {
   opt_expand &= ~EXPAND_RSSets;
   return 0;
}

int skipAS(char *dst, char *key, char *nextArg) {
   opt_expand &= ~EXPAND_AS;
   return 0;
}

int skipAll(char *dst, char *key, char *nextArg) {
   opt_expand = EXPAND_NOTHING;
   return 0;
}

int expression(char *dst, char *key, char *nextArg) {
   if (!nextArg) return 0;

   strcpy(filter, nextArg);
   return 1;
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
      {"-rusage", ARGV_BOOL, (char *) NULL,           (char *) &opt_rusage,
       "On termination print resource usage"},
      {"-prompt", ARGV_STRING,  (char *) NULL,        (char *) &opt_prompt,
       "Prompt"},

      IRR_COMMAND_LINE_OPTIONS,

      {"-asdot", ARGV_BOOL, (char *) NULL, (char *) &opt_asdot,
       "print AS numbers in asdot format."},

      // peval specific arguments
      {"-symbolic",  ARGV_CONSTANT, (char *)1, (char *)&opt_symbolic, 
       "Symbolic"},
      {"-no-as-set", ARGV_FUNC, (char *)skipASSets, (char *)NULL,
       "Do not expand AS sets"},
      {"-no-route-set", ARGV_FUNC, (char *)skipRSSets, (char *)NULL, 
       "Do not expand route sets"},
      {"-no-as", ARGV_FUNC, (char *)skipAS, (char *)NULL,
       "Do not expand AS numbers"},
      {"-none", ARGV_FUNC, (char *)skipAll, (char *)NULL,
       "Do not expand anything"},
      {"-compressed", ARGV_BOOL, 
       (char *) NULL, (char *) &RadixSet::compressedPrint,
       "Print prefix lists using the more specific operators."},
      {"-e", ARGV_FUNC, (char *)expression, (char *)NULL,
       "Expression"},
      {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL, (char *) NULL}
   };

   IRR::handleEnvironmentVariables(envp);

   if (ParseArgv(&argc, argv, argTable, ARGV_NO_ABBREV) != ARGV_OK) {
      cerr << endl;
      exit(1);
   }

   if (! (irr = IRR::newClient())) {
      cerr << "Unknown protocol!" << endl;
      exit(1);
   }

   // if there are remaining arguments
   // the first one is the expression
   switch (argc) {
   case 2:
      strcpy(filter, argv[1]);
      break;
   case 1:
      break;
   default:
      cerr << "Wrong number of arguments..." << endl;
      exit(-1);
   }

   if (*filter) {
      evaluate();
      exit(0);
   }

   // have a prompt only if the input is coming from a tty
   if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
      opt_prompt = (char *) NULL;
}

#if HAVE_LIBREADLINE && HAVE_LIBHISTORY
/* A static variable for holding the line. */
static char *line_read = (char *)NULL;

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char *
rl_gets (char *prompt)
{
  /* If the buffer has already been allocated, return the memory
     to the free pool. */
  if (line_read)
    {
      free (line_read);
      line_read = (char *)NULL;
    }

  /* Get a line from the user. */
  line_read = readline (prompt);

  /* If the line has any text in it, save it on the history. */
  if (line_read && *line_read)
    add_history (line_read);

  return (line_read);
}
#endif // HAVE_READLINE

int main (int argc, char **argv, char **envp) {
   int newSize = strlen(base);
   filter = base + newSize;
   newSize = SIZE - newSize - 3;

   schema.initialize();
   init_and_set_options(argc, argv, envp);

   while (1) {
#if HAVE_LIBREADLINE && HAVE_LIBHISTORY
      char *line = rl_gets(opt_prompt);
      if (!line)
	 break;
      strncpy(filter, line, newSize);
      evaluate();
#else // HAVE_READLINE
      if (opt_prompt)
	 cout << opt_prompt;
      if (cin.getline(filter, newSize))
	 evaluate();
      else
	 break;
#endif
   }

   cout << endl;

   CLASS_DEBUG_MEMORY_PRINT(Set);
   CLASS_DEBUG_MEMORY_PRINT(NormalTerm);
   CLASS_DEBUG_MEMORY_PRINT(NormalExpression);
   CLASS_DEBUG_MEMORY_PRINT(SymbolConjunct);
}
