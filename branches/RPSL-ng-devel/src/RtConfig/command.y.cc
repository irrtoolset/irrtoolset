/* A Bison parser, made from command.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse commandparse
#define yylex commandlex
#define yyerror commanderror
#define yylval commandlval
#define yychar commandchar
#define yydebug commanddebug
#define yynerrs commandnerrs
# define	TKN_IP	257
# define	TKN_ASNUM	258
# define	TKN_ERROR	259
# define	TKN_STR	260
# define	TKN_INT	261
# define	TKN_DNSNAME	262
# define	TKN_FILTER	263
# define	TKN_WORD	264
# define	KW_IMPORT	265
# define	KW_EXPORT	266
# define	KW_EXPORT_GROUP	267
# define	KW_IMPORT_GROUP	268
# define	KW_PRINT_PREFIXES	269
# define	KW_PRINT_PREFIX_RANGES	270
# define	KW_PRINT_SUPER_PREFIX_RANGES	271
# define	KW_ASPATH_ACCESS_LIST	272
# define	KW_ACCESS_LIST	273
# define	KW_CONFIGURE_ROUTER	274
# define	KW_STATIC2BGP	275
# define	KW_SET	276
# define	KW_CISCO_MAP_NAME	277
# define	KW_JUNOS_POLICY_NAME	278
# define	KW_CISCO_PREFIX_ACL_NO	279
# define	KW_CISCO_ASPATH_ACL_NO	280
# define	KW_CISCO_PKTFILTER_ACL_NO	281
# define	KW_CISCO_COMMUNITY_ACL_NO	282
# define	KW_CISCO_ACCESS_LIST_NO	283
# define	KW_SOURCE	284
# define	KW_PREFERENCECEILING	285
# define	KW_CISCO_MAX_PREFERENCE	286
# define	KW_NETWORKS	287
# define	KW_V6NETWORKS	288
# define	KW_DEFAULT	289
# define	KW_CISCO_MAP_INC	290
# define	KW_CISCO_MAP_START	291
# define	KW_PKT_FILTER	292
# define	KW_OUTBOUND_PKT_FILTER	293
# define	KW_BCC_VERSION	294
# define	KW_BCC_MAX_PREFERENCE	295
# define	KW_BCC_ADVERTISE_NETS	296
# define	KW_BCC_ADVERTISE_ALL	297
# define	KW_BCC_FORCE_BACK	298
# define	KW_BCC_MAX_PREFIXES	299

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


#line 88 "command.y"
#ifndef YYSTYPE
typedef union {
   int i;
   char *val;
   /*IPAddr *ip;*/
   MPPrefix *ip;
   ASt as;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		115
#define	YYFLAG		-32768
#define	YYNTBASE	48

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 299 ? yytranslate[x] : 73)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      46,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    47,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     7,    10,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
      40,    42,    44,    46,    48,    50,    52,    54,    56,    62,
      68,    72,    76,    80,    84,    88,    91,    94,    98,   101,
     105,   108,   111,   118,   125,   130,   135,   140,   145,   150,
     155,   160,   165,   170
};
static const short yyrhs[] =
{
      -1,    48,    49,     0,    50,    46,     0,     1,    46,     0,
      46,     0,    51,     0,    52,     0,    53,     0,    54,     0,
      55,     0,    56,     0,    57,     0,    58,     0,    59,     0,
      60,     0,    61,     0,    62,     0,    63,     0,    64,     0,
      65,     0,    66,     0,    67,     0,    68,     0,    69,     0,
      70,     0,    71,     0,    72,     0,    11,     4,     3,     4,
       3,     0,    12,     4,     3,     4,     3,     0,    13,     4,
      10,     0,    14,     4,    10,     0,    15,     6,     9,     0,
      16,     6,     9,     0,    17,     6,     9,     0,    19,     9,
       0,    18,     9,     0,    35,     4,     4,     0,    20,     8,
       0,    21,     4,     3,     0,    33,     4,     0,    34,     4,
       0,    38,     6,     4,     3,     4,     3,     0,    39,     6,
       4,     3,     4,     3,     0,    22,    23,    47,     6,     0,
      22,    36,    47,     7,     0,    22,    37,    47,     7,     0,
      22,    25,    47,     7,     0,    22,    26,    47,     7,     0,
      22,    27,    47,     7,     0,    22,    28,    47,     7,     0,
      22,    29,    47,     7,     0,    22,    31,    47,     7,     0,
      22,    30,    47,     6,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   142,   146,   152,   153,   156,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   177,   178,   179,   187,   188,   191,   203,
     215,   225,   235,   250,   265,   280,   284,   288,   293,   299,
     305,   310,   315,   322,   329,   343,   350,   357,   363,   369,
     375,   381,   432,   441
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "TKN_IP", "TKN_ASNUM", "TKN_ERROR", 
  "TKN_STR", "TKN_INT", "TKN_DNSNAME", "TKN_FILTER", "TKN_WORD", 
  "KW_IMPORT", "KW_EXPORT", "KW_EXPORT_GROUP", "KW_IMPORT_GROUP", 
  "KW_PRINT_PREFIXES", "KW_PRINT_PREFIX_RANGES", 
  "KW_PRINT_SUPER_PREFIX_RANGES", "KW_ASPATH_ACCESS_LIST", 
  "KW_ACCESS_LIST", "KW_CONFIGURE_ROUTER", "KW_STATIC2BGP", "KW_SET", 
  "KW_CISCO_MAP_NAME", "KW_JUNOS_POLICY_NAME", "KW_CISCO_PREFIX_ACL_NO", 
  "KW_CISCO_ASPATH_ACL_NO", "KW_CISCO_PKTFILTER_ACL_NO", 
  "KW_CISCO_COMMUNITY_ACL_NO", "KW_CISCO_ACCESS_LIST_NO", "KW_SOURCE", 
  "KW_PREFERENCECEILING", "KW_CISCO_MAX_PREFERENCE", "KW_NETWORKS", 
  "KW_V6NETWORKS", "KW_DEFAULT", "KW_CISCO_MAP_INC", "KW_CISCO_MAP_START", 
  "KW_PKT_FILTER", "KW_OUTBOUND_PKT_FILTER", "KW_BCC_VERSION", 
  "KW_BCC_MAX_PREFERENCE", "KW_BCC_ADVERTISE_NETS", 
  "KW_BCC_ADVERTISE_ALL", "KW_BCC_FORCE_BACK", "KW_BCC_MAX_PREFIXES", 
  "'\\n'", "'='", "input_stream", "input", "input_line", "import_line", 
  "export_line", "export_group_line", "import_group_line", 
  "print_prefixes_line", "print_prefix_ranges_line", 
  "print_super_prefix_ranges_line", "access_list_line", 
  "aspath_access_list_line", "default_line", "configure_router_line", 
  "static2bgp_line", "networks_line", "v6networks_line", 
  "pkt_filter_line", "outbound_pkt_filter_line", "cisco_map_name_line", 
  "cisco_map_inc_line", "cisco_map_start_line", 
  "cisco_access_list_no_line", "preferenceCeiling_line", "source_line", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    48,    48,    49,    49,    49,    50,    50,    50,    50,
      50,    50,    50,    50,    50,    50,    50,    50,    50,    50,
      50,    50,    50,    50,    50,    50,    50,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    70,    70,
      70,    70,    71,    72
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     5,
       3,     3,     3,     3,     3,     2,     2,     3,     2,     3,
       2,     2,     6,     6,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     2,     0,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,     4,     0,     0,     0,     0,
       0,     0,     0,    36,    35,    38,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    40,    41,     0,
       0,     0,     3,     0,     0,    30,    31,    32,    33,    34,
      39,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    37,     0,     0,     0,     0,    44,    47,    48,    49,
      50,    51,    53,    52,    45,    46,     0,     0,    28,    29,
       0,     0,    42,    43,     0,     0
};

static const short yydefgoto[] =
{
       1,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44
};

static const short yypact[] =
{
  -32768,     0,   -44,    -1,     1,     2,     3,    -2,     4,    17,
      15,    16,    18,     5,    22,    23,    24,    25,    26,    30,
  -32768,-32768,   -38,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,    27,    28,    31,    32,
      34,    35,    45,-32768,-32768,-32768,    37,   -10,     8,     9,
      10,    13,    14,    19,    20,    21,    29,-32768,-32768,    58,
      59,    60,-32768,    61,    65,-32768,-32768,-32768,-32768,-32768,
  -32768,    64,    66,    67,    68,    70,    71,    73,    74,    75,
      76,-32768,    69,    77,    81,    82,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,    83,    84,-32768,-32768,
      86,    87,-32768,-32768,    91,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768
};


#define	YYLAST		91


static const short yytable[] =
{
     114,     2,    45,    46,    50,    47,    48,    49,    72,    56,
      51,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    52,    53,    54,    55,    67,    68,    69,
      73,    74,    70,    15,    16,    17,    71,    81,    18,    19,
      80,    75,    76,    77,    78,    57,    20,    58,    59,    60,
      61,    62,    63,    64,    79,    82,    83,    84,    65,    66,
      85,    86,    91,    92,    93,    94,    87,    88,    89,    95,
      96,     0,   106,    97,    98,    99,    90,   100,   101,   102,
     107,   103,   104,   105,   108,   109,     0,   110,   111,   112,
     113,   115
};

static const short yycheck[] =
{
       0,     1,    46,     4,     6,     4,     4,     4,    46,     4,
       6,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,     6,     9,     9,     8,     4,     4,     4,
       3,     3,     6,    33,    34,    35,     6,    47,    38,    39,
       3,    10,    10,     9,     9,    23,    46,    25,    26,    27,
      28,    29,    30,    31,     9,    47,    47,    47,    36,    37,
      47,    47,     4,     4,     4,     4,    47,    47,    47,     4,
       6,    -1,     3,     7,     7,     7,    47,     7,     7,     6,
       3,     7,     7,     7,     3,     3,    -1,     4,     4,     3,
       3,     0
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
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
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
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
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 142 "command.y"
{
   if (opt_prompt)
      cout << opt_prompt;
}
    break;
case 2:
#line 146 "command.y"
{
   if (opt_prompt)
      cout << opt_prompt;
}
    break;
case 4:
#line 153 "command.y"
{
  yyerrok;
}
    break;
case 28:
#line 191 "command.y"
{
   /*
   cout << "!" << endl
        << "!LINE " << yylineno << " -- import" << endl
        << "!" << endl;
   */
   rtConfig->importP(yyvsp[-3].as, yyvsp[-2].ip, yyvsp[-1].as, yyvsp[0].ip);
   delete yyvsp[-2].ip;
   delete yyvsp[0].ip;
}
    break;
case 29:
#line 203 "command.y"
{
   /*
   cout << "!" << endl
        << "!LINE " << yylineno << " -- export" << endl
        << "!" << endl;
   */
   rtConfig->exportP(yyvsp[-3].as, yyvsp[-2].ip, yyvsp[-1].as, yyvsp[0].ip);
   delete yyvsp[-2].ip;
   delete yyvsp[0].ip;
}
    break;
case 30:
#line 215 "command.y"
{
   /* 
   cout << "!" << endl
        << "!LINE " << yylineno << " -- export" << endl
        << "!" << endl;
   */
   rtConfig->exportGroup(yyvsp[-1].as, yyvsp[0].val);
}
    break;
case 31:
#line 225 "command.y"
{
   /* 
   cout << "!" << endl
        << "!LINE " << yylineno << " -- export" << endl
        << "!" << endl;
   */
   rtConfig->importGroup(yyvsp[-1].as, yyvsp[0].val);
}
    break;
case 32:
#line 235 "command.y"
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
}
    break;
case 33:
#line 250 "command.y"
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
}
    break;
case 34:
#line 265 "command.y"
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
}
    break;
case 35:
#line 280 "command.y"
{
   rtConfig->accessList(yyvsp[0].val);
}
    break;
case 36:
#line 284 "command.y"
{
   rtConfig->aspathAccessList(yyvsp[0].val);
}
    break;
case 37:
#line 288 "command.y"
{
   rtConfig->deflt(yyvsp[-1].as, yyvsp[0].as);
}
    break;
case 38:
#line 293 "command.y"
{
   rtConfig->configureRouter(yyvsp[0].val);
   free(yyvsp[0].val);
}
    break;
case 39:
#line 299 "command.y"
{
   rtConfig->static2bgp(yyvsp[-1].as, yyvsp[0].ip);
   delete yyvsp[0].ip;
}
    break;
case 40:
#line 305 "command.y"
{
   rtConfig->networks(yyvsp[0].as);
}
    break;
case 41:
#line 310 "command.y"
{
   rtConfig->IPv6networks(yyvsp[0].as);
}
    break;
case 42:
#line 315 "command.y"
{
   rtConfig->packetFilter(yyvsp[-4].val, yyvsp[-3].as, yyvsp[-2].ip, yyvsp[-1].as, yyvsp[0].ip);
   delete yyvsp[-2].ip;
   delete yyvsp[0].ip;
}
    break;
case 43:
#line 322 "command.y"
{
   rtConfig->outboundPacketFilter(yyvsp[-4].val, yyvsp[-3].as, yyvsp[-2].ip, yyvsp[-1].as, yyvsp[0].ip);
   delete yyvsp[-2].ip;
   delete yyvsp[0].ip;
}
    break;
case 44:
#line 329 "command.y"
{
   strcpy(CiscoConfig::mapNameFormat, yyvsp[0].val);
   Trace(TR_INPUT) << "RtConfig: cisco_map_name '"
		   << CiscoConfig::mapNameFormat << "'" << endl;
}
    break;
case 45:
#line 343 "command.y"
{
   CiscoConfig::mapIncrements = yyvsp[0].i;
   Trace(TR_INPUT) << "RtConfig: cisco_map_increment_by '" 
		   << CiscoConfig::mapIncrements << "'" << endl;
}
    break;
case 46:
#line 350 "command.y"
{
   CiscoConfig::mapNumbersStartAt = yyvsp[0].i;
   Trace(TR_INPUT) << "RtConfig: cisco_map_first_no '" 
		   << CiscoConfig::mapNumbersStartAt << "'" << endl;
}
    break;
case 47:
#line 357 "command.y"
{
   if (yyvsp[0].i > 0)
      prefixMgr.setNextID(yyvsp[0].i);
   Trace(TR_INPUT) << "RtConfig: cisco_prefix_access_list_no '"
		   << yyvsp[0].i << "'" << endl;
}
    break;
case 48:
#line 363 "command.y"
{
   if (yyvsp[0].i > 0)
      aspathMgr.setNextID(yyvsp[0].i);
   Trace(TR_INPUT) << "RtConfig: cisco_aspath_access_list_no '"
		   << yyvsp[0].i << "'" << endl;
}
    break;
case 49:
#line 369 "command.y"
{
   if (yyvsp[0].i > 0)
      pktFilterMgr.setNextID(yyvsp[0].i);
   Trace(TR_INPUT) << "RtConfig: cisco_pktfilter_access_list_no '"
		   << yyvsp[0].i << "'" << endl;
}
    break;
case 50:
#line 375 "command.y"
{
   if (yyvsp[0].i > 0)
      communityMgr.setNextID(yyvsp[0].i);
   Trace(TR_INPUT) << "RtConfig: cisco_pktfilter_access_list_no '"
		   << yyvsp[0].i << "'" << endl;
}
    break;
case 51:
#line 381 "command.y"
{
   if (yyvsp[0].i > 0) {
      communityMgr.setNextID(yyvsp[0].i);
      pktFilterMgr.setNextID(yyvsp[0].i);
      aspathMgr.setNextID(yyvsp[0].i);
      prefixMgr.setNextID(yyvsp[0].i);
   }
   Trace(TR_INPUT) << "RtConfig: cisco_pktfilter_access_list_no '"
		   << yyvsp[0].i << "'" << endl;
}
    break;
case 52:
#line 432 "command.y"
{
   if (yyvsp[0].i >= 0)
      RtConfig::preferenceCeiling = yyvsp[0].i;

   Trace(TR_INPUT) << "RtConfig: preferenceCeiling '"
		   << RtConfig::preferenceCeiling << "'" << endl;
}
    break;
case 53:
#line 441 "command.y"
{
   irr->SetSources(yyvsp[0].val);
   Trace(TR_INPUT) << "RtConfig: database order is changed to'"
		   << yyvsp[0].val << "'" << endl;
}
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
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

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 449 "command.y"


int yyerror(char *s) {
   printf ("Error in template file at line %d: %s\n", yylineno, s);
   return(0);
}

