/*  
//  $Id$
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
//  Author(s): WeeSan Lee <wlee@ISI.EDU>
*/

%{

#if !defined(IDENT_OFF)
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7) 
static char ident[] = 
   "@(#)$Id$";
#endif
#endif

#include <string.h>
#include "bgp_aspath.hh"

#undef YY_DECL
#define YY_DECL int loadBgpDump(BgpASPath &rBgpASPath)


static int iBgpASPathCol = 0;
static int iCol = 1;
static int iRow = 1;

static int iFirstPeer = 1;
static ASt tFirstPeer = 0;
static ASt tCandidateLastPeer = 0;

%}

%x AS_PATH
%x USEFUL_LINE

%%
.  { 
   iCol++;
}

\n {
   iCol = 1;
   iRow++;
}

Path {
   iBgpASPathCol = iCol - 1;   // (iCol - 1) should be a space
   BEGIN USEFUL_LINE;
}

<USEFUL_LINE>. {
   if (++iCol == iBgpASPathCol) {
      iFirstPeer = 1;
      tFirstPeer = 0;
      tCandidateLastPeer = 0;
      BEGIN AS_PATH;
   }
}

<USEFUL_LINE>\n {
   iCol = 1;
   iRow++;
}

<AS_PATH>[ ][1-9][0-9]*[ ] {  // Match a number not starting with a zero
   yyless(yyleng - 1);        // Push back the trailer blank
   if (iFirstPeer) {
     //      cerr << iRow << ':' << iCol << " - " << yytext + 1 << " ";
      iFirstPeer = 0;
      tFirstPeer = atoi(yytext + 1);
      rBgpASPath.addFirst(tFirstPeer);
   } else {
      tCandidateLastPeer = atoi(yytext + 1);
   }
}

<AS_PATH>. ;

<AS_PATH>\n {
  //   cerr << tCandidateLastPeer << endl;
   if (tCandidateLastPeer) 
      rBgpASPath.addLast(tFirstPeer, tCandidateLastPeer);
   iCol = 1;
   iRow++;
   BEGIN USEFUL_LINE;
}


%%

int yywrap(void) 
{
   return 1;
}

#ifdef BGP_DUMP_DEBUG

void main(void)
{
   while (yylex()) ;
}

#endif // BGP_DUMP_DEBUG
