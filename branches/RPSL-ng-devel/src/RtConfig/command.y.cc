
/*  A Bison parser, made from command.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse commandparse
#define yylex commandlex
#define yyerror commanderror
#define yylval commandlval
#define yychar commandchar
#define yydebug commanddebug
#define yynerrs commandnerrs
#define	TKN_IP	257
#define	TKN_ASNUM	258
#define	TKN_ERROR	259
#define	TKN_STR	260
#define	TKN_INT	261
#define	TKN_DNSNAME	262
#define	TKN_FILTER	263
#define	TKN_WORD	264
#define	KW_IMPORT	265
#define	KW_EXPORT	266
#define	KW_EXPORT_GROUP	267
#define	KW_IMPORT_GROUP	268
#define	KW_PRINT_PREFIXES	269
#define	KW_PRINT_PREFIX_RANGES	270
#define	KW_PRINT_SUPER_PREFIX_RANGES	271
#define	KW_ASPATH_ACCESS_LIST	272
#define	KW_ACCESS_LIST	273
#define	KW_CONFIGURE_ROUTER	274
#define	KW_STATIC2BGP	275
#define	KW_SET	276
#define	KW_CISCO_MAP_NAME	277
#define	KW_JUNOS_POLICY_NAME	278
#define	KW_CISCO_PREFIX_ACL_NO	279
#define	KW_CISCO_ASPATH_ACL_NO	280
#define	KW_CISCO_PKTFILTER_ACL_NO	281
#define	KW_CISCO_COMMUNITY_ACL_NO	282
#define	KW_CISCO_ACCESS_LIST_NO	283
#define	KW_SOURCE	284
#define	KW_PREFERENCECEILING	285
#define	KW_CISCO_MAX_PREFERENCE	286
#define	KW_NETWORKS	287
#define	KW_V6NETWORKS	288
#define	KW_DEFAULT	289
#define	KW_CISCO_MAP_INC	290
#define	KW_CISCO_MAP_START	291
#define	KW_PKT_FILTER	292
#define	KW_OUTBOUND_PKT_FILTER	293
#define	KW_BCC_VERSION	294
#define	KW_BCC_MAX_PREFERENCE	295
#define	KW_BCC_ADVERTISE_NETS	296
#define	KW_BCC_ADVERTISE_ALL	297
#define	KW_BCC_FORCE_BACK	298
#define	KW_BCC_MAX_PREFIXES	299

#line 1 "command.y"

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

/***** ALL CONFIGS EXCEPT CISCO TEMP DISABLED !!! *****/

#include "config.h"

#include <ostream>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdarg>
#include "RtConfig.hh"
#include "f_cisco.hh"
#include "f_bcc.hh" 
#include "f_junos.hh" 
#include "irr/irr.hh"
#include "util/trace.hh"
#include "rpsl/rpsl.hh"

extern "C" {
#if HAVE_MEMORY_H && 0
#   include <memory.h>
#endif
#ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
#endif
}

extern int yyerror(char *);
extern int yylex();
extern int yylineno;

int xx_eof = 0;


#line 89 "command.y"
typedef union {
   int i;
   char *val;
   /*IPAddr *ip;*/
   MPPrefix *ip;
   ASt as;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		115
#define	YYFLAG		-32768
#define	YYNTBASE	48

#define YYTRANSLATE(x) ((unsigned)(x) <= 299 ? yytranslate[x] : 73)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    46,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    47,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     7,    10,    12,    14,    16,    18,    20,
    22,    24,    26,    28,    30,    32,    34,    36,    38,    40,
    42,    44,    46,    48,    50,    52,    54,    56,    62,    68,
    72,    76,    80,    84,    88,    91,    94,    98,   101,   105,
   108,   111,   118,   125,   130,   135,   140,   145,   150,   155,
   160,   165,   170
};

static const short yyrhs[] = {    -1,
    48,    49,     0,    50,    46,     0,     1,    46,     0,    46,
     0,    51,     0,    52,     0,    53,     0,    54,     0,    55,
     0,    56,     0,    57,     0,    58,     0,    59,     0,    60,
     0,    61,     0,    62,     0,    63,     0,    64,     0,    65,
     0,    66,     0,    67,     0,    68,     0,    69,     0,    70,
     0,    71,     0,    72,     0,    11,     4,     3,     4,     3,
     0,    12,     4,     3,     4,     3,     0,    13,     4,    10,
     0,    14,     4,    10,     0,    15,     6,     9,     0,    16,
     6,     9,     0,    17,     6,     9,     0,    19,     9,     0,
    18,     9,     0,    35,     4,     4,     0,    20,     8,     0,
    21,     4,     3,     0,    33,     4,     0,    34,     4,     0,
    38,     6,     4,     3,     4,     3,     0,    39,     6,     4,
     3,     4,     3,     0,    22,    23,    47,     6,     0,    22,
    36,    47,     7,     0,    22,    37,    47,     7,     0,    22,
    25,    47,     7,     0,    22,    26,    47,     7,     0,    22,
    27,    47,     7,     0,    22,    28,    47,     7,     0,    22,
    29,    47,     7,     0,    22,    31,    47,     7,     0,    22,
    30,    47,     6,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   143,   147,   153,   154,   157,   160,   161,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
   175,   176,   178,   179,   180,   188,   189,   192,   204,   216,
   226,   236,   251,   266,   281,   285,   289,   294,   300,   306,
   311,   316,   323,   330,   344,   351,   358,   364,   370,   376,
   382,   433,   442
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","TKN_IP",
"TKN_ASNUM","TKN_ERROR","TKN_STR","TKN_INT","TKN_DNSNAME","TKN_FILTER","TKN_WORD",
"KW_IMPORT","KW_EXPORT","KW_EXPORT_GROUP","KW_IMPORT_GROUP","KW_PRINT_PREFIXES",
"KW_PRINT_PREFIX_RANGES","KW_PRINT_SUPER_PREFIX_RANGES","KW_ASPATH_ACCESS_LIST",
"KW_ACCESS_LIST","KW_CONFIGURE_ROUTER","KW_STATIC2BGP","KW_SET","KW_CISCO_MAP_NAME",
"KW_JUNOS_POLICY_NAME","KW_CISCO_PREFIX_ACL_NO","KW_CISCO_ASPATH_ACL_NO","KW_CISCO_PKTFILTER_ACL_NO",
"KW_CISCO_COMMUNITY_ACL_NO","KW_CISCO_ACCESS_LIST_NO","KW_SOURCE","KW_PREFERENCECEILING",
"KW_CISCO_MAX_PREFERENCE","KW_NETWORKS","KW_V6NETWORKS","KW_DEFAULT","KW_CISCO_MAP_INC",
"KW_CISCO_MAP_START","KW_PKT_FILTER","KW_OUTBOUND_PKT_FILTER","KW_BCC_VERSION",
"KW_BCC_MAX_PREFERENCE","KW_BCC_ADVERTISE_NETS","KW_BCC_ADVERTISE_ALL","KW_BCC_FORCE_BACK",
"KW_BCC_MAX_PREFIXES","'\\n'","'='","input_stream","input","input_line","import_line",
"export_line","export_group_line","import_group_line","print_prefixes_line",
"print_prefix_ranges_line","print_super_prefix_ranges_line","access_list_line",
"aspath_access_list_line","default_line","configure_router_line","static2bgp_line",
"networks_line","v6networks_line","pkt_filter_line","outbound_pkt_filter_line",
"cisco_map_name_line","cisco_map_inc_line","cisco_map_start_line","cisco_access_list_no_line",
"preferenceCeiling_line","source_line", NULL
};
#endif

static const short yyr1[] = {     0,
    48,    48,    49,    49,    49,    50,    50,    50,    50,    50,
    50,    50,    50,    50,    50,    50,    50,    50,    50,    50,
    50,    50,    50,    50,    50,    50,    50,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
    64,    65,    66,    67,    68,    69,    70,    70,    70,    70,
    70,    71,    72
};

static const short yyr2[] = {     0,
     0,     2,     2,     2,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     5,     5,     3,
     3,     3,     3,     3,     2,     2,     3,     2,     3,     2,
     2,     6,     6,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4
};

static const short yydefact[] = {     1,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     5,
     2,     0,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,     4,     0,     0,     0,     0,     0,
     0,     0,    36,    35,    38,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    40,    41,     0,     0,
     0,     3,     0,     0,    30,    31,    32,    33,    34,    39,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    37,     0,     0,     0,     0,    44,    47,    48,    49,    50,
    51,    53,    52,    45,    46,     0,     0,    28,    29,     0,
     0,    42,    43,     0,     0
};

static const short yydefgoto[] = {     1,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    44
};

static const short yypact[] = {-32768,
     0,   -44,    -1,     1,     2,     3,    -2,     4,    17,    15,
    16,    18,     5,    22,    23,    24,    25,    26,    30,-32768,
-32768,   -38,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    27,    28,    31,    32,    34,
    35,    45,-32768,-32768,-32768,    37,   -10,     8,     9,    10,
    13,    14,    19,    20,    21,    29,-32768,-32768,    58,    59,
    60,-32768,    61,    65,-32768,-32768,-32768,-32768,-32768,-32768,
    64,    66,    67,    68,    70,    71,    73,    74,    75,    76,
-32768,    69,    77,    81,    82,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    83,    84,-32768,-32768,    86,
    87,-32768,-32768,    91,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768
};


#define	YYLAST		91


static const short yytable[] = {   114,
     2,    45,    46,    50,    47,    48,    49,    72,    56,    51,
     3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,    52,    53,    54,    55,    67,    68,    69,    73,
    74,    70,    15,    16,    17,    71,    81,    18,    19,    80,
    75,    76,    77,    78,    57,    20,    58,    59,    60,    61,
    62,    63,    64,    79,    82,    83,    84,    65,    66,    85,
    86,    91,    92,    93,    94,    87,    88,    89,    95,    96,
     0,   106,    97,    98,    99,    90,   100,   101,   102,   107,
   103,   104,   105,   108,   109,     0,   110,   111,   112,   113,
   115
};

static const short yycheck[] = {     0,
     1,    46,     4,     6,     4,     4,     4,    46,     4,     6,
    11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
    21,    22,     6,     9,     9,     8,     4,     4,     4,     3,
     3,     6,    33,    34,    35,     6,    47,    38,    39,     3,
    10,    10,     9,     9,    23,    46,    25,    26,    27,    28,
    29,    30,    31,     9,    47,    47,    47,    36,    37,    47,
    47,     4,     4,     4,     4,    47,    47,    47,     4,     6,
    -1,     3,     7,     7,     7,    47,     7,     7,     6,     3,
     7,     7,     7,     3,     3,    -1,     4,     4,     3,     3,
     0
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/local/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 143 "command.y"
{
   if (opt_prompt)
      std::cout << opt_prompt;
;
    break;}
case 2:
#line 147 "command.y"
{
   if (opt_prompt)
      std::cout << opt_prompt;
;
    break;}
case 4:
#line 154 "command.y"
{
  yyerrok;
;
    break;}
case 28:
#line 192 "command.y"
{
   /*
   cout << "!" << endl
        << "!LINE " << yylineno << " -- import" << endl
        << "!" << endl;
   */
   rtConfig->importP(yyvsp[-3].as, yyvsp[-2].ip, yyvsp[-1].as, yyvsp[0].ip);
   delete yyvsp[-2].ip;
   delete yyvsp[0].ip;
;
    break;}
case 29:
#line 204 "command.y"
{
   /*
   cout << "!" << endl
        << "!LINE " << yylineno << " -- export" << endl
        << "!" << endl;
   */
   rtConfig->exportP(yyvsp[-3].as, yyvsp[-2].ip, yyvsp[-1].as, yyvsp[0].ip);
   delete yyvsp[-2].ip;
   delete yyvsp[0].ip;
;
    break;}
case 30:
#line 216 "command.y"
{
   /* 
   cout << "!" << endl
        << "!LINE " << yylineno << " -- export" << endl
        << "!" << endl;
   */
   rtConfig->exportGroup(yyvsp[-1].as, yyvsp[0].val);
;
    break;}
case 31:
#line 226 "command.y"
{
   /* 
   cout << "!" << endl
        << "!LINE " << yylineno << " -- export" << endl
        << "!" << endl;
   */
   rtConfig->importGroup(yyvsp[-1].as, yyvsp[0].val);
;
    break;}
case 32:
#line 236 "command.y"
{
   char *p = yyvsp[-1].val;
   while (p = strchr(p, '\\')) {
      if (*(p+1) == 'n') {
	 *p = '%';
	 *(p+1) = '\n';
      }
      if (*(p+1) == 't') {
	 *p = '%';
	 *(p+1) = '\t';
      }
   }
   rtConfig->printPrefixes(yyvsp[0].val, yyvsp[-1].val);
;
    break;}
case 33:
#line 251 "command.y"
{
   char *p = yyvsp[-1].val;
   while (p = strchr(p, '\\')) {
      if (*(p+1) == 'n') {
	 *p = '%';
	 *(p+1) = '\n';
      }
      if (*(p+1) == 't') {
	 *p = '%';
	 *(p+1) = '\t';
      }
   }
   rtConfig->printPrefixRanges(yyvsp[0].val, yyvsp[-1].val);
;
    break;}
case 34:
#line 266 "command.y"
{
   char *p = yyvsp[-1].val;
   while (p = strchr(p, '\\')) {
      if (*(p+1) == 'n') {
	 *p = '%';
	 *(p+1) = '\n';
      }
      if (*(p+1) == 't') {
	 *p = '%';
	 *(p+1) = '\t';
      }
   }
   rtConfig->printSuperPrefixRanges(yyvsp[0].val, yyvsp[-1].val);
;
    break;}
case 35:
#line 281 "command.y"
{
   rtConfig->accessList(yyvsp[0].val);
;
    break;}
case 36:
#line 285 "command.y"
{
   rtConfig->aspathAccessList(yyvsp[0].val);
;
    break;}
case 37:
#line 289 "command.y"
{
   rtConfig->deflt(yyvsp[-1].as, yyvsp[0].as);
;
    break;}
case 38:
#line 294 "command.y"
{
   rtConfig->configureRouter(yyvsp[0].val);
   free(yyvsp[0].val);
;
    break;}
case 39:
#line 300 "command.y"
{
   rtConfig->static2bgp(yyvsp[-1].as, yyvsp[0].ip);
   delete yyvsp[0].ip;
;
    break;}
case 40:
#line 306 "command.y"
{
   rtConfig->networks(yyvsp[0].as);
;
    break;}
case 41:
#line 311 "command.y"
{
   rtConfig->IPv6networks(yyvsp[0].as);
;
    break;}
case 42:
#line 316 "command.y"
{
   rtConfig->packetFilter(yyvsp[-4].val, yyvsp[-3].as, yyvsp[-2].ip, yyvsp[-1].as, yyvsp[0].ip);
   delete yyvsp[-2].ip;
   delete yyvsp[0].ip;
;
    break;}
case 43:
#line 323 "command.y"
{
   rtConfig->outboundPacketFilter(yyvsp[-4].val, yyvsp[-3].as, yyvsp[-2].ip, yyvsp[-1].as, yyvsp[0].ip);
   delete yyvsp[-2].ip;
   delete yyvsp[0].ip;
;
    break;}
case 44:
#line 330 "command.y"
{
   strcpy(CiscoConfig::mapNameFormat, yyvsp[0].val);
   Trace(TR_INPUT) << "RtConfig: cisco_map_name '"
		   << CiscoConfig::mapNameFormat << "'" << std::endl;
;
    break;}
case 45:
#line 344 "command.y"
{
   CiscoConfig::mapIncrements = yyvsp[0].i;
   Trace(TR_INPUT) << "RtConfig: cisco_map_increment_by '" 
		   << CiscoConfig::mapIncrements << "'" << std::endl;
;
    break;}
case 46:
#line 351 "command.y"
{
   CiscoConfig::mapNumbersStartAt = yyvsp[0].i;
   Trace(TR_INPUT) << "RtConfig: cisco_map_first_no '" 
		   << CiscoConfig::mapNumbersStartAt << "'" << std::endl;
;
    break;}
case 47:
#line 358 "command.y"
{
   if (yyvsp[0].i > 0)
      prefixMgr.setNextID(yyvsp[0].i);
   Trace(TR_INPUT) << "RtConfig: cisco_prefix_access_list_no '"
		   << yyvsp[0].i << "'" << std::endl;
;
    break;}
case 48:
#line 364 "command.y"
{
   if (yyvsp[0].i > 0)
      aspathMgr.setNextID(yyvsp[0].i);
   Trace(TR_INPUT) << "RtConfig: cisco_aspath_access_list_no '"
		   << yyvsp[0].i << "'" << std::endl;
;
    break;}
case 49:
#line 370 "command.y"
{
   if (yyvsp[0].i > 0)
      pktFilterMgr.setNextID(yyvsp[0].i);
   Trace(TR_INPUT) << "RtConfig: cisco_pktfilter_access_list_no '"
		   << yyvsp[0].i << "'" << std::endl;
;
    break;}
case 50:
#line 376 "command.y"
{
   if (yyvsp[0].i > 0)
      communityMgr.setNextID(yyvsp[0].i);
   Trace(TR_INPUT) << "RtConfig: cisco_pktfilter_access_list_no '"
		   << yyvsp[0].i << "'" << std::endl;
;
    break;}
case 51:
#line 382 "command.y"
{
   if (yyvsp[0].i > 0) {
      communityMgr.setNextID(yyvsp[0].i);
      pktFilterMgr.setNextID(yyvsp[0].i);
      aspathMgr.setNextID(yyvsp[0].i);
      prefixMgr.setNextID(yyvsp[0].i);
   }
   Trace(TR_INPUT) << "RtConfig: cisco_pktfilter_access_list_no '"
		   << yyvsp[0].i << "'" << std::endl;
;
    break;}
case 52:
#line 433 "command.y"
{
   if (yyvsp[0].i >= 0)
      RtConfig::preferenceCeiling = yyvsp[0].i;

   Trace(TR_INPUT) << "RtConfig: preferenceCeiling '"
		   << RtConfig::preferenceCeiling << "'" << std::endl;
;
    break;}
case 53:
#line 442 "command.y"
{
   irr->SetSources(yyvsp[0].val);
   Trace(TR_INPUT) << "RtConfig: database order is changed to'"
		   << yyvsp[0].val << "'" << std::endl;
;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/local/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 450 "command.y"


int yyerror(char *s) {
     std::cerr << "Error in template file at line " << yylineno
               << ": " << s
               << std::endl;
     return(0);
}

