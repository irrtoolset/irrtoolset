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
*/

/* Definitions */
%x USEFUL_LINE
%x USELESS_LINE

DNAME          [[:alnum:]_-]+
QUAD           [0-9A-F]{1,4}
IPV6           {QUAD}(:{QUAD}){7}
IPV6DC         (({QUAD}:){0,6}{QUAD})?::({QUAD}(:{QUAD}){0,6})?

%{
#include "config.h"
#include <iostream>
#include "RtConfig.hh"
#include "command.y.hh"
#include "irr/irr.hh"

#define yylval commandlval
#define LEXER_RETURN(x)  return(x)

#define HAVE_YYLINENO
#ifndef HAVE_YYLINENO
#define yylineno commandlineno
int yylineno();
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
"pkt_filter",              KW_PKT_FILTER,
"outbound_pkt_filter",     KW_OUTBOUND_PKT_FILTER,
"set",                     KW_SET,
"cisco_map_name",          KW_CISCO_MAP_NAME,
"junos_policy_name",       KW_JUNOS_POLICY_NAME,
"cisco_map_increment_by",  KW_CISCO_MAP_INC,
"cisco_map_first_no",      KW_CISCO_MAP_START,
"cisco_access_list_no",    KW_CISCO_ACCESS_LIST_NO,
"cisco_prefix_acl_no",     KW_CISCO_PREFIX_ACL_NO,
"cisco_aspath_acl_no",     KW_CISCO_ASPATH_ACL_NO,
"cisco_pktfilter_acl_no",  KW_CISCO_PKTFILTER_ACL_NO,
"cisco_community_acl_no",  KW_CISCO_COMMUNITY_ACL_NO,
"cisco_max_preference",    KW_PREFERENCECEILING,
"sources",                 KW_SOURCE,
"bcc_version_line",        KW_BCC_VERSION, 
"bcc_max_preference",      KW_BCC_MAX_PREFERENCE, 
"bcc_advertise_nets",      KW_BCC_ADVERTISE_NETS,
"bcc_advertise_all",       KW_BCC_ADVERTISE_ALL,
"bcc_force_back",          KW_BCC_FORCE_BACK,
"bcc_max_prefixes",        KW_BCC_MAX_PREFIXES,
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


#if 0
/* The use of YY_INPUT() seems to tickle a bug in the
 * lexer.  Sometmes it drops the initial character.
 * [GDT. Red Hat Linux Fedora Core 1, flex-2.5.4a-30]
 */
/* Redefine end of line as CR, CR LF or LF so that Unix, Mac, Windows,
 * MS-DOS files can be used as input.  CP/M and old MS-DOS files might
 * have 1 to 128 trailing Ctrl+Z -- we don't cope with those :-)
 */
#define YY_INPUT(BUFFER,RESULT,MAX_SIZE) \
{ \
    int ch; \
    int next_ch; \
 \
    ch = getc(yyin); \
    if (ch == EOF) { \
        RESULT = YY_NULL; \
    } \
    else { \
        if (ch == '\r') { \
            ch = '\n'; \
            next_ch = fgetc(yyin); \
            if (next_ch != EOF && next_ch != '\n') { \
                (void)ungetc(next_ch, yyin); \
            } \
        } \
        BUFFER[0] = ch; \
        RESULT = 1; \
    } \
}
#endif

%}

%%

%{
/* Rules */
%}

^@RtConfig {
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
