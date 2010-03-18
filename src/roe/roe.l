/*
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
*/


/* definitions */
%x USEFUL_LINE
%x USELESS_LINE
%x SKIP_REST_OF_LINE

%{

#include "config.h"
#include <iostream.h>
#include "roe.y.hh"
// Added by wlee@isi.edu
#include <cstring>

#define yylval roelval
#define yyline roeline
#define yylinebol roelinebol
#define yyerrstart roeerrstart
#define yyerrlength roeerrlength
#define yylength roelength
#define yylineptr roelineptr

#define LEXER_RETURN(x)  return(x)
#define YY_USER_ACTION    {                 \
      if (yylinebol) {                      \
	 *yyline = 0;                       \
	 yylineptr = yyline;                \
         yyerrstart  = 0;                   \
         yyerrlength = 0;                   \
      }                                     \
      strcpy(yylineptr, yytext);            \
      yylineptr += yyleng;                  \
      yylength = yyleng;                    \
      yylinebol = !strcmp(yytext, "\n");    \
   }

char yyline[4*1024] = "";
int  yylinebol = 1;
int  yyerrstart  = 0;
int  yyerrlength = 0;
int  yylength = 0;
char *yylineptr;

int roe_lexer_input_size = 0;
char *roe_lexer_input;

typedef struct _Attribute_t {
   char *long_name;
   char *short_name;
   int  num;
} Attribute_t;

static Attribute_t useful_attrs[] = {
// attributes of route object
"route:",                     "*rt:",   ATTR_ROUTE,
"origin:",                    "*or:",   ATTR_ORIGIN,
"source:",                    "*so:",   ATTR_SOURCE,
"mnt-by:",                    "*mb:",   ATTR_MNT_BY,
"delete:",                    "*XX:",   ATTR_DELETE,
NULL,                          NULL,    TKN_ERROR
};

static int get_attr_num(char *string) {
   int i;

   for (i = 0; useful_attrs[i].long_name; i++)
      if (!strcmp(useful_attrs[i].long_name, string)
	  || !strcmp(useful_attrs[i].short_name, string))
	 break;

  return(useful_attrs[i].num);
}

extern "C" {
int yywrap () {
   return 1;
}
}

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) { \
	result = MIN(max_size, roe_lexer_input_size); \
        strncpy(buf, roe_lexer_input, result); \
	if (result < max_size) \
           buf[result] = 0; \
        roe_lexer_input_size -= result; \
}

%}

%%

%{
/* Rules */
%}

^[^ \t\n]+: {
   int token_type;

   if ((token_type = get_attr_num(yytext)) != TKN_ERROR) {
      if (token_type == ATTR_DELETE)
	 BEGIN(SKIP_REST_OF_LINE);
      else
	 BEGIN(USEFUL_LINE);
      LEXER_RETURN(token_type);
   } else
      BEGIN(USELESS_LINE);
}

\n {
   /* simply skip this line, it is not interesting to us */
}

. {
   /* simply skip this line, it is not interesting to us */
   BEGIN(USELESS_LINE);
}

<USELESS_LINE>.* {
   /* simply skip this line, it is not interesting to us */
}

<USELESS_LINE>\n {
   /* simply skip this line, it is not interesting to us */
   BEGIN(INITIAL);
}

<SKIP_REST_OF_LINE>.* {
   /* simply skip this line, it is not interesting to us */
}

<SKIP_REST_OF_LINE>\n {
   /* simply skip this line, it is not interesting to us */
   BEGIN(INITIAL);
   LEXER_RETURN((int) *yytext); 
}

<USEFUL_LINE>AS[0-9]+ {
   LEXER_RETURN(TKN_ASNUM);
}

<USEFUL_LINE>[a-zA-Z]+ {
   LEXER_RETURN(TKN_WORD);
}

<USEFUL_LINE>[a-zA-Z0-9_-]+ {
   LEXER_RETURN(TKN_ALPHANUMERIC);
}

<USEFUL_LINE>[0-9]+(\.[0-9]+){3,3}\/[0-9]+ {
   LEXER_RETURN(TKN_PRFMSK);
}

<USEFUL_LINE>\n {
   BEGIN(INITIAL);
   LEXER_RETURN((int) *yytext); 
}

<USEFUL_LINE>[ \t]+ {
   /* Skip white space */
}

<USEFUL_LINE>. {
   LEXER_RETURN(TKN_ERROR);
}
%%

/* User Code if any */

