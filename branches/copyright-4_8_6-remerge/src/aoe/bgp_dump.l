/*
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
