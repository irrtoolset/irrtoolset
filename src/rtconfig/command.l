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

%option outfile="command.l.cc"
%option prefix="command"

/* Definitions */
%x USEFUL_LINE
%x USELESS_LINE

DNAME          [[:alnum:]_-]+
QUAD           [0-9A-Fa-f]{1,4}
IPV6           {QUAD}(:{QUAD}){7}
IPV6DC         (({QUAD}:){0,6}{QUAD})?::({QUAD}(:{QUAD}){0,6})?

%{
#include "config.h"
#include <iostream>
#include "rtconfig.hh"
#include "command.y.hh"
#include "irr/irr.hh"

#if HAVE_LIBREADLINE && HAVE_LIBHISTORY 
#include <readline/readline.h> 
#include <readline/history.h> 
extern int rtconfig_input(char *, int); 
#endif // HAVE_LIBREADLINE and HAVE_LIBHISTORY 
                        
#define yylval commandlval
#define LEXER_RETURN(x)  return(x)

#define yylineno commandlineno
#ifndef HAVE_YYLINENO
int yylineno;
#endif // HAVE_YYLINENO

typedef struct _KeyWord {
   const char *val;
   int num;
} KeyWord;

static KeyWord keywords[] = {
"import",                  KW_IMPORT,
"export",                  KW_EXPORT,
"exportGroup",             KW_EXPORT_GROUP,
"importGroup",             KW_IMPORT_GROUP,
"default",                 KW_DEFAULT,
"printPrefixes",           KW_PRINT_PREFIXES,
"printPrefixRanges",       KW_PRINT_PREFIX_RANGES,
"printSuperPrefixRanges",  KW_PRINT_SUPER_PREFIX_RANGES,
"access_list",             KW_ACCESS_LIST,
"aspath_access_list",      KW_ASPATH_ACCESS_LIST,
"configureRouter",         KW_CONFIGURE_ROUTER,
"static2bgp",              KW_STATIC2BGP,
"networks",                KW_NETWORKS,
"v6networks",              KW_V6NETWORKS,
"pkt_filter",              KW_INBOUND_PKT_FILTER,
"inbound_pkt_filter",      KW_INBOUND_PKT_FILTER,
"outbound_pkt_filter",     KW_OUTBOUND_PKT_FILTER,
"set",                     KW_SET,
"cisco_map_name",          KW_CISCO_MAP_NAME,
"junos_policy_name",       KW_JUNOS_POLICY_NAME,
"cisco_map_increment_by",  KW_CISCO_MAP_INC,
"cisco_map_first_no",      KW_CISCO_MAP_START,
"cisco_access_list_no",    KW_CISCO_ACCESS_LIST_NO,
"cisco_prefix_acl_no",     KW_CISCO_PREFIX_ACL_NO,
"junos_prefix_acl_no",     KW_JUNOS_PREFIX_ACL_NO,
"cisco_aspath_acl_no",     KW_CISCO_ASPATH_ACL_NO,
"junos_aspath_acl_no",     KW_JUNOS_ASPATH_ACL_NO,
"cisco_pktfilter_acl_no",  KW_CISCO_PKTFILTER_ACL_NO,
"junos_pktfilter_acl_no",  KW_JUNOS_PKTFILTER_ACL_NO,
"cisco_community_acl_no",  KW_CISCO_COMMUNITY_ACL_NO,
"junos_community_acl_no",  KW_JUNOS_COMMUNITY_ACL_NO,
"junos_community_set_no",  KW_JUNOS_COMMUNITY_SET_NO,
"cisco_max_preference",    KW_PREFERENCECEILING,
"sources",                 KW_SOURCE,
NULL,        -1
};

static int get_keyword_num(char *string) {
   int i;

   for (i = 0; keywords[i].val; i++)
      if (!strcmp(keywords[i].val, string))
	 break;

   return(keywords[i].num);
}

extern "C" {
int yywrap () {
   return 1;
}
}

#if HAVE_LIBREADLINE && HAVE_LIBHISTORY
#define YY_INPUT(buf,result,max_size)	\
   result = rtconfig_input (buf, max_size);
#endif // HAVE_LIBREADLINE && HAVE_LIBHISTORY

%}

%%

%{
/* Rules */
%}

^@[Rr]t[Cc]onfig {
   BEGIN(USEFUL_LINE);
}

\n {
   yylineno++;
   ECHO;
   /* simply skip this line, it is not interesting to us */
   LEXER_RETURN((int) *yytext);
}

. {
   ECHO;
   /* simply skip this line, it is not interesting to us */
   BEGIN(USELESS_LINE);
}

<USELESS_LINE>.* {
   ECHO;
   /* simply skip this line, it is not interesting to us */
}

<USELESS_LINE>\n {
   yylineno++;
   ECHO;
   /* simply skip this line, it is not interesting to us */
   BEGIN(INITIAL);
   LEXER_RETURN((int) *yytext);
}


<USEFUL_LINE>[Aa][Ss]([0-9]+|[0-9]+\.[0-9]+) {
   const char *dot = strchr(yytext,'.');
   if (dot)
      yylval.as = atoi(yytext+2)<<16 | atoi(dot+1);
   else
      yylval.as = atoi(yytext+2);
   LEXER_RETURN(TKN_ASNUM);
}

<USEFUL_LINE>[0-9]+(\.[0-9]+){3,3} {
   yylval.ip = new MPPrefix(yytext);
   LEXER_RETURN(TKN_IP);
}

<USEFUL_LINE>{IPV6} {
   yylval.ip = new MPPrefix(yytext);
   LEXER_RETURN(TKN_IP);
}

<USEFUL_LINE>{IPV6DC} {
   yylval.ip = new MPPrefix(yytext);
   LEXER_RETURN(TKN_IP);
}

<USEFUL_LINE>{DNAME}("."{DNAME})+ {
   yylval.val = strdup(yytext);
   LEXER_RETURN(TKN_DNSNAME);
}

<USEFUL_LINE>[0-9]+ {
   yylval.i = atoi(yytext);
   LEXER_RETURN(TKN_INT);
}

<USEFUL_LINE>\"([^\"]|\\\")*\" { 
   yylval.val = yytext + 1; 
   *(yytext + yyleng - 1) = 0;
   LEXER_RETURN(TKN_STR);
}

<USEFUL_LINE>filter\ .* {
   yylval.val = yytext + 7; 
   LEXER_RETURN(TKN_FILTER);
}

<USEFUL_LINE>[ \t,]+ { 
   /* Skip white space */
}

<USEFUL_LINE>\n {
   yylineno++;
   BEGIN(INITIAL);
   LEXER_RETURN((int) *yytext);
}

<USEFUL_LINE>= {
   LEXER_RETURN((int) *yytext);
}

<USEFUL_LINE>. {
   LEXER_RETURN(TKN_ERROR);
}

<USEFUL_LINE>[A-Za-z_0-9-]+ {
   int code = get_keyword_num(yytext);
   if (code != -1)
      LEXER_RETURN(code);

   yylval.val = yytext;
   LEXER_RETURN(TKN_WORD);
}

%%

/* User Code if any */
