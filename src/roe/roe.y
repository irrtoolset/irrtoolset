%{
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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include "config.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdarg>

extern "C" {
#if HAVE_MEMORY_H && 0
#   include <memory.h>
#endif
#ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
#endif
}

#include "schedule.hh"

#define yylval roelval
#define yyline roeline
#define yylinebol roelinebol
#define yyerrstart roeerrstart
#define yyerrlength roeerrlength
#define yylength roelength
#define yylineptr roelineptr
#define yytext roetext
#define yy_error_while_expecting roe_error_while_expecting

Transaction *roe_parser_trns;

int roe_parser_had_errors = 0;

extern int yyerror(char *);
extern int yylex();
char *yy_error_while_expecting = NULL;
extern char *yytext;

%}

%union {
   char *val;
}

%token <val>  TKN_ERROR        500
%token <val>  TKN_PRFMSK       501
%token <val>  TKN_ASNUM        502
%token <val>  TKN_WORD         503
%token <val>  TKN_ALPHANUMERIC 504

%token <val> ATTR_ROUTE          400
%token <val> ATTR_ORIGIN         401
%token <val> ATTR_MNT_BY         402
%token <val> ATTR_SOURCE         403
%token <val> ATTR_DELETE         404

%%

input: input_line
|
input input_line
;

input_line: policy_line '\n'
{
}
| '\n'
{ // empty line
}
| error  '\n'
{
   extern char yyline[];
   extern int  yyerrstart;
   extern int  yyerrlength;
   int i;
   cerr << "Error: while parsing " << (char *) yyline;
   cerr << "Error:               ";
   for (i = 0; i < yyerrstart; i++)
      cerr << " ";
   for (i = 0; i < yyerrlength; i++)
      cerr << "^";
   cerr << endl;
   if (yy_error_while_expecting)
      cerr << "Error: " << yy_error_while_expecting << " expected." << endl;
   yy_error_while_expecting = NULL;
}
;

policy_line: rt_route_line
| rt_origin_line
| rt_source_line
| rt_mnt_by_line
| rt_delete_line
;

rt_route_line: ATTR_ROUTE TKN_PRFMSK {
   strncpy(roe_parser_trns->route, yytext, sizeof(roe_parser_trns->route));
}
;

rt_origin_line: ATTR_ORIGIN TKN_ASNUM {
   strncpy(roe_parser_trns->as, yytext, sizeof(roe_parser_trns->as));
}
;

rt_source_line: ATTR_SOURCE TKN_WORD {
   strncpy(roe_parser_trns->source, yytext, sizeof(roe_parser_trns->source));
}
;

rt_delete_line: ATTR_DELETE {
   roe_parser_trns->op_code = OP_DEL;
}
;

rt_mnt_by_line: ATTR_MNT_BY TKN_ALPHANUMERIC {
   strncpy(roe_parser_trns->mnt_by, yytext, sizeof(roe_parser_trns->mnt_by));
}
| ATTR_MNT_BY TKN_WORD {
   strncpy(roe_parser_trns->mnt_by, yytext, sizeof(roe_parser_trns->mnt_by));
}
;

%%

int yyerror(char *s) {
   extern char yyline[];
   extern int  yyerrstart;
   extern int  yyerrlength;
   extern char *yylineptr;
   extern int yylength;

   roe_parser_had_errors = 1;

   yyerrstart = yylineptr - yyline - yylength;
   yyerrlength = yylength;
   return(0);
}

