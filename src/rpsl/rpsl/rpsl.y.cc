/* A Bison parser, made from rpsl.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse rpslparse
#define yylex rpsllex
#define yyerror rpslerror
#define yylval rpsllval
#define yychar rpslchar
#define yydebug rpsldebug
#define yynerrs rpslnerrs
# define	KEYW_TRUE	257
# define	KEYW_FALSE	258
# define	KEYW_ACTION	259
# define	KEYW_ACCEPT	260
# define	KEYW_ANNOUNCE	261
# define	KEYW_FROM	262
# define	KEYW_TO	263
# define	KEYW_AT	264
# define	KEYW_ANY	265
# define	KEYW_REFINE	266
# define	KEYW_EXCEPT	267
# define	KEYW_STATIC	268
# define	KEYW_NETWORKS	269
# define	KEYW_MASKLEN	270
# define	KEYW_UNION	271
# define	KEYW_RANGE	272
# define	KEYW_LIST	273
# define	KEYW_OF	274
# define	KEYW_OPERATOR	275
# define	KEYW_SYNTAX	276
# define	KEYW_SPECIAL	277
# define	KEYW_REGEXP	278
# define	KEYW_OPTIONAL	279
# define	KEYW_MANDATORY	280
# define	KEYW_INTERNAL	281
# define	KEYW_SINGLEVALUED	282
# define	KEYW_MULTIVALUED	283
# define	KEYW_LOOKUP	284
# define	KEYW_KEY	285
# define	KEYW_DELETED	286
# define	KEYW_OBSOLETE	287
# define	KEYW_PEERAS	288
# define	KEYW_PROTOCOL	289
# define	KEYW_INTO	290
# define	KEYW_ATOMIC	291
# define	KEYW_INBOUND	292
# define	KEYW_OUTBOUND	293
# define	KEYW_UPON	294
# define	KEYW_HAVE_COMPONENTS	295
# define	KEYW_EXCLUDE	296
# define	KEYW_AFI	297
# define	KEYW_TUNNEL	298
# define	TKN_ERROR	299
# define	TKN_UNKNOWN_CLASS	300
# define	TKN_EOA	301
# define	TKN_EOO	302
# define	TKN_FREETEXT	303
# define	TKN_INT	304
# define	TKN_REAL	305
# define	TKN_STRING	306
# define	TKN_TIMESTAMP	307
# define	TKN_BLOB	308
# define	TKN_IPV4	309
# define	TKN_PRFXV4	310
# define	TKN_PRFXV4RNG	311
# define	TKN_IPV6	312
# define	TKN_PRFXV6	313
# define	TKN_PRFXV6RNG	314
# define	TKN_ASNO	315
# define	TKN_ASNAME	316
# define	TKN_RSNAME	317
# define	TKN_RTRSNAME	318
# define	TKN_PRNGNAME	319
# define	TKN_FLTRNAME	320
# define	TKN_BOOLEAN	321
# define	TKN_WORD	322
# define	TKN_RP_ATTR	323
# define	TKN_DNS	324
# define	TKN_EMAIL	325
# define	TKN_3DOTS	326
# define	TKN_AFI	327
# define	ATTR_GENERIC	328
# define	ATTR_BLOBS	329
# define	ATTR_REGEXP	330
# define	ATTR_IMPORT	331
# define	ATTR_MP_IMPORT	332
# define	ATTR_EXPORT	333
# define	ATTR_MP_EXPORT	334
# define	ATTR_DEFAULT	335
# define	ATTR_MP_DEFAULT	336
# define	ATTR_FREETEXT	337
# define	ATTR_CHANGED	338
# define	ATTR_IFADDR	339
# define	ATTR_INTERFACE	340
# define	ATTR_PEER	341
# define	ATTR_MP_PEER	342
# define	ATTR_INJECT	343
# define	ATTR_V6_INJECT	344
# define	ATTR_COMPONENTS	345
# define	ATTR_V6_COMPONENTS	346
# define	ATTR_AGGR_MTD	347
# define	ATTR_AGGR_BNDRY	348
# define	ATTR_RS_MEMBERS	349
# define	ATTR_RS_MP_MEMBERS	350
# define	ATTR_RTR_MP_MEMBERS	351
# define	ATTR_RP_ATTR	352
# define	ATTR_TYPEDEF	353
# define	ATTR_PROTOCOL	354
# define	ATTR_FILTER	355
# define	ATTR_V6_FILTER	356
# define	ATTR_MP_FILTER	357
# define	ATTR_MP_PEVAL	358
# define	ATTR_PEERING	359
# define	ATTR_MP_PEERING	360
# define	ATTR_ATTR	361
# define	ATTR_MNT_ROUTES	362
# define	ATTR_MNT_ROUTES6	363
# define	ATTR_MNT_ROUTES_MP	364
# define	OP_OR	365
# define	OP_AND	366
# define	OP_NOT	367
# define	OP_MS	368
# define	TKN_OPERATOR	369

#line 1 "rpsl.y"

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
//             Katie Petrusha <katie@ripe.net>

#include "config.h"
#include "time.h"
#include "schema.hh"
#include "object.hh"
#include "regexp.hh"
#include "rptype.hh"

#ifdef DEBUG
#define YYDEBUG 1
#endif // DEBUG
#if YYDEBUG != 0
// stdio is needed for yydebug
#include <cstdio>
#endif

extern "C" {
#ifdef HAVE_REGEX_H
#include <regex.h>
#else
#include <gnu/regex.h>
#endif
}

extern void handle_error(char *, ...);
extern void handle_object_error(char *, ...);
extern void handle_warning(char *, ...);
extern int yylex();
char *token_name(int token_id);
void rpslerror(char *s, ...);
Attr *changeCurrentAttr(Attr *b);
void handleArgumentTypeError(char *attr, char *method, int position,
			     const RPType *correctType, 
			     bool isOperator = false);
const AttrMethod *searchMethod(const AttrRPAttr *rp_attr, char *method, ItemList *args);

/* argument to yyparse result of parsing should be stored here */
#define YYPARSE_PARAM object	
#define yyschema schema
#define enable_yy_parser_debugging enable_rpsl_parser_debugging

extern Object *current_object;


#line 102 "rpsl.y"
#ifndef YYSTYPE
typedef union {
   long long int      i;
   double             real;
   char              *string;
   void              *ptr;
   time_t             time;
   Item              *item;
   ItemList          *list;
   regexp            *re;
   SymID              sid;
   IPAddr            *ip;
   Prefix            *prfx;
   PrefixRange       *prfxrng;
   IPv6Addr          *ipv6;
   IPv6Prefix        *prfxv6;
   IPv6PrefixRange   *prfxv6rng;
   AddressFamily     *afi;
   RPType            *typenode;
   MPPrefix          *mpprefix;
   Tunnel            *tunnel;

   Filter                    *filter;
   FilterMS                  *moreSpecOp;
   PolicyPeering             *peering;
   PolicyPeering             *mp_peering;
   PolicyActionList          *actionList;
   PolicyAction              *actionNode;
   PolicyFactor              *policyFactor;
   PolicyTerm                *policyTerm;
   PolicyExpr                *policyExpr;
   List<PolicyPeeringAction> *peeringActionList;

   Attr               *attr;
   AttrAttr           *rpslattr;
   AttrMethod         *method;
   const AttrRPAttr   *rp_attr;
   const AttrProtocol *protocol;
   AttrProtocolOption *protocol_option;
   List<AttrProtocolOption> *protocol_option_list;

   AttrPeerOption       *peer_option;
   List<AttrPeerOption> *peer_option_list;

   List<RPTypeNode> *typelist;
   List<AttrMethod> *methodlist;
   List<WordNode>   *wordlist;

   List<AttrMntRoutes::MntPrfxPair> *listMntPrfxPair;
   AttrMntRoutes::MntPrfxPair       *mntPrfxPair;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		970
#define	YYFLAG		-32768
#define	YYNTBASE	136

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 369 ? yytranslate[x] : 308)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const short yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   135,     2,     2,     2,
     121,   122,   130,   132,   116,   117,   123,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   118,   124,
     127,     2,   128,   131,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   125,     2,   126,   134,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   119,   129,   120,   133,     2,     2,     2,
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
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     3,     6,     9,    11,    13,    15,    16,    18,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    47,    49,    51,    53,    55,    57,    59,
      61,    63,    65,    67,    69,    71,    73,    75,    77,    79,
      81,    83,    85,    87,    89,    91,    93,    96,    99,   104,
     108,   111,   114,   118,   122,   123,   125,   127,   131,   135,
     139,   141,   144,   146,   150,   152,   154,   156,   158,   160,
     162,   164,   166,   168,   170,   172,   176,   180,   182,   184,
     186,   188,   190,   192,   194,   196,   198,   200,   202,   206,
     210,   212,   214,   216,   218,   220,   222,   224,   226,   228,
     230,   232,   234,   236,   238,   240,   242,   244,   246,   248,
     250,   252,   254,   256,   258,   260,   262,   264,   266,   268,
     270,   272,   274,   276,   278,   280,   282,   284,   286,   288,
     290,   292,   294,   296,   298,   300,   302,   303,   305,   309,
     311,   315,   319,   321,   325,   327,   329,   331,   332,   334,
     335,   338,   342,   344,   348,   352,   354,   358,   360,   362,
     364,   366,   370,   372,   373,   376,   378,   381,   389,   394,
     400,   406,   408,   412,   415,   417,   421,   423,   426,   430,
     432,   434,   438,   440,   442,   444,   447,   449,   451,   453,
     455,   457,   461,   462,   464,   466,   470,   472,   474,   478,
     480,   483,   485,   488,   491,   494,   498,   502,   504,   506,
     508,   512,   514,   516,   518,   520,   522,   526,   531,   532,
     535,   538,   541,   546,   549,   556,   560,   565,   570,   574,
     579,   583,   588,   592,   595,   599,   603,   606,   610,   613,
     617,   620,   624,   626,   630,   634,   636,   640,   644,   645,
     648,   649,   652,   658,   665,   673,   677,   683,   690,   698,
     702,   704,   708,   712,   714,   718,   722,   726,   731,   735,
     740,   744,   747,   751,   755,   758,   762,   766,   771,   775,
     780,   784,   787,   789,   793,   795,   798,   802,   804,   806,
     810,   812,   814,   816,   819,   821,   823,   825,   827,   829,
     833,   834,   836,   838,   842,   844,   846,   848,   850,   854,
     856,   857,   859,   860,   863,   867,   869,   873,   877,   879,
     883,   885,   887,   889,   891,   893,   899,   905,   909,   915,
     921,   925,   926,   929,   936,   942,   946,   954,   961,   965,
     966,   969,   973,   977,   981,   985,   989,   993,   997,  1001,
    1005,  1009,  1013,  1020,  1027,  1033,  1038,  1042,  1044,  1046,
    1047,  1052,  1060,  1068,  1074,  1079,  1083,  1084,  1086,  1088,
    1092,  1097,  1099,  1101,  1103,  1105,  1111,  1117,  1122,  1126,
    1132,  1138,  1143,  1147,  1149,  1151,  1153,  1155,  1157,  1161,
    1165,  1169,  1172,  1174,  1178,  1180,  1183,  1187,  1189,  1191,
    1195,  1197,  1199,  1201,  1204,  1206,  1208,  1210,  1212,  1214,
    1218,  1219,  1221,  1223,  1227,  1229,  1231,  1236,  1240,  1241,
    1243,  1248,  1254,  1258,  1259,  1262,  1266,  1268,  1272,  1274,
    1278,  1280,  1282,  1287,  1292,  1296,  1300,  1304,  1309,  1314,
    1319,  1323,  1324,  1327,  1331,  1333,  1337,  1339,  1343,  1345,
    1347,  1352,  1357,  1363,  1367,  1368,  1370,  1371,  1373,  1378,
    1383,  1387,  1388,  1390,  1392,  1396,  1398,  1401,  1403,  1406,
    1408,  1411,  1413,  1415,  1419,  1423,  1424,  1426,  1428,  1432,
    1434,  1437,  1439,  1442,  1444,  1447,  1449,  1451,  1453,  1455,
    1459,  1463,  1468,  1473,  1477,  1479,  1482,  1486,  1491,  1498,
    1504,  1512,  1516,  1521,  1525,  1528,  1533,  1537,  1539,  1543,
    1546,  1550,  1552,  1559,  1566,  1571,  1580,  1584,  1589,  1591,
    1595,  1600,  1605,  1609,  1610,  1613,  1616,  1619,  1620,  1622,
    1624,  1628,  1632,  1637,  1644,  1651,  1660,  1662,  1664,  1666,
    1668,  1670,  1672,  1674,  1676,  1678,  1683,  1688,  1692,  1696,
    1698,  1702,  1704,  1707,  1712,  1716,  1718,  1722,  1724,  1727,
    1732,  1736,  1738,  1742,  1744,  1747,  1752,  1755,  1756,  1758,
    1762,  1764
};
static const short yyrhs[] =
{
     137,    48,     0,    46,    48,     0,     1,    48,     0,   137,
       0,    46,     0,     1,     0,     0,   138,     0,   137,   138,
       0,   142,     0,   145,     0,   141,     0,   139,     0,   222,
       0,   194,     0,   223,     0,   195,     0,   225,     0,   226,
       0,   242,     0,   243,     0,   234,     0,   237,     0,   273,
       0,   255,     0,   270,     0,   257,     0,   264,     0,   263,
       0,   285,     0,   289,     0,   282,     0,   277,     0,   281,
       0,   228,     0,   245,     0,   231,     0,   229,     0,   232,
       0,   233,     0,   295,     0,   140,     0,   296,     0,   299,
       0,   302,     0,    45,    47,     0,     1,    47,     0,    84,
      71,    50,    47,     0,    84,     1,    47,     0,    83,    47,
       0,    76,    47,     0,    74,   143,    47,     0,    74,     1,
      47,     0,     0,   144,     0,   147,     0,   144,   116,   147,
       0,    75,   146,    47,     0,    75,     1,    47,     0,   147,
       0,   146,   147,     0,   148,     0,   148,   117,   148,     0,
      61,     0,    50,     0,    51,     0,    52,     0,    53,     0,
      55,     0,    56,     0,    57,     0,    58,     0,    59,     0,
      60,     0,    55,   118,    50,     0,    70,   118,    50,     0,
      62,     0,    63,     0,    64,     0,    65,     0,    66,     0,
      67,     0,    68,     0,   150,     0,    70,     0,    71,     0,
      54,     0,   119,   143,   120,     0,   121,   166,   122,     0,
      68,     0,    61,     0,    62,     0,    63,     0,    64,     0,
      65,     0,    66,     0,    67,     0,   150,     0,     3,     0,
       4,     0,     5,     0,     6,     0,     7,     0,     8,     0,
       9,     0,    10,     0,    11,     0,    12,     0,    13,     0,
      14,     0,    15,     0,    16,     0,    17,     0,    18,     0,
      19,     0,    20,     0,    21,     0,    22,     0,    23,     0,
      24,     0,    25,     0,    26,     0,    27,     0,    32,     0,
      28,     0,    29,     0,    30,     0,    31,     0,    33,     0,
      34,     0,    35,     0,    36,     0,    37,     0,    38,     0,
      39,     0,     0,   152,     0,   152,   111,   153,     0,   153,
       0,   153,   112,   154,     0,   153,    13,   154,     0,   154,
       0,   121,   152,   122,     0,   155,     0,    61,     0,    62,
       0,     0,   158,     0,     0,    10,   158,     0,   158,   111,
     159,     0,   159,     0,   159,   112,   160,     0,   159,    13,
     160,     0,   160,     0,   121,   158,   122,     0,   161,     0,
      55,     0,    70,     0,    64,     0,   152,   156,   157,     0,
      65,     0,     0,     5,   164,     0,   165,     0,   164,   165,
       0,    69,   123,    68,   121,   143,   122,   124,     0,    69,
     115,   147,   124,     0,    69,   121,   143,   122,   124,     0,
      69,   125,   143,   126,   124,     0,   124,     0,   166,   111,
     167,     0,   166,   167,     0,   167,     0,   167,   112,   168,
       0,   168,     0,   113,   168,     0,   121,   166,   122,     0,
     169,     0,    11,     0,   127,   175,   128,     0,   181,     0,
      66,     0,   170,     0,   171,   114,     0,   171,     0,    61,
       0,    34,     0,    62,     0,    63,     0,   119,   172,   120,
       0,     0,   173,     0,   174,     0,   173,   116,   174,     0,
      56,     0,    57,     0,   175,   129,   176,     0,   176,     0,
     176,   177,     0,   177,     0,   177,   130,     0,   177,   131,
       0,   177,   132,     0,   179,   133,   132,     0,   179,   133,
     130,     0,   178,     0,   134,     0,   135,     0,   121,   175,
     122,     0,   179,     0,    61,     0,    34,     0,    62,     0,
     123,     0,   125,   180,   126,     0,   125,   134,   180,   126,
       0,     0,   180,    61,     0,   180,    34,     0,   180,   123,
       0,   180,    61,   117,    61,     0,   180,    62,     0,    69,
     123,    68,   121,   143,   122,     0,    69,   115,   147,     0,
      69,   121,   143,   122,     0,    69,   125,   143,   126,     0,
       8,   162,   163,     0,   182,     8,   162,   163,     0,     9,
     162,   163,     0,   183,     9,   162,   163,     0,   182,     6,
     166,     0,   184,   124,     0,   185,   184,   124,     0,   183,
       7,   166,     0,   186,   124,     0,   187,   186,   124,     0,
     184,   124,     0,   119,   185,   120,     0,   186,   124,     0,
     119,   187,   120,     0,   188,     0,   188,    12,   190,     0,
     188,    13,   190,     0,   189,     0,   189,    12,   191,     0,
     189,    13,   191,     0,     0,    35,   149,     0,     0,    36,
     149,     0,    78,   192,   193,   196,    47,     0,    78,   192,
     193,   305,   200,    47,     0,    78,   192,   193,    43,   306,
       1,    47,     0,    78,     1,    47,     0,    80,   192,   193,
     197,    47,     0,    80,   192,   193,   305,   202,    47,     0,
      80,   192,   193,    43,   306,     1,    47,     0,    80,     1,
      47,     0,   198,     0,   198,    12,   196,     0,   198,    13,
     196,     0,   199,     0,   199,    12,   197,     0,   199,    13,
     197,     0,   305,   200,   124,     0,   305,   119,   201,   120,
       0,   305,   202,   124,     0,   305,   119,   203,   120,     0,
     204,     6,   206,     0,   200,   124,     0,   201,   200,   124,
       0,   205,     7,   206,     0,   202,   124,     0,   203,   202,
     124,     0,     8,   215,   163,     0,   204,     8,   215,   163,
       0,     9,   215,   163,     0,   205,     9,   215,   163,     0,
     206,   111,   207,     0,   206,   207,     0,   207,     0,   207,
     112,   208,     0,   208,     0,   113,   208,     0,   121,   206,
     122,     0,   209,     0,    11,     0,   127,   175,   128,     0,
     181,     0,    66,     0,   210,     0,   211,   114,     0,   211,
       0,    61,     0,    34,     0,    62,     0,    63,     0,   119,
     212,   120,     0,     0,   213,     0,   214,     0,   213,   116,
     214,     0,    59,     0,    60,     0,    56,     0,    57,     0,
     152,   216,   217,     0,    65,     0,     0,   218,     0,     0,
      10,   218,     0,   218,   111,   219,     0,   219,     0,   219,
     112,   220,     0,   219,    13,   220,     0,   220,     0,   121,
     218,   122,     0,   221,     0,    55,     0,    58,     0,    70,
       0,    64,     0,    77,   192,   193,   190,    47,     0,    77,
     192,   193,   184,    47,     0,    77,     1,    47,     0,    79,
     192,   193,   191,    47,     0,    79,   192,   193,   186,    47,
       0,    79,     1,    47,     0,     0,    15,   166,     0,    81,
       9,   162,   163,   224,    47,     0,    81,     9,   162,     1,
      47,     0,    81,     1,    47,     0,    82,   305,     9,   215,
     163,   227,    47,     0,    82,   305,     9,   215,     1,    47,
       0,    82,     1,    47,     0,     0,    15,   206,     0,   101,
     166,    47,     0,   101,     1,    47,     0,   104,   230,    47,
       0,   104,     1,    47,     0,    43,   306,   206,     0,   103,
     206,    47,     0,   103,     1,    47,     0,   105,   162,    47,
       0,   105,     1,    47,     0,   106,   215,    47,     0,   106,
       1,    47,     0,    85,    55,    16,    50,   163,    47,     0,
      85,    55,    16,    50,     1,    47,     0,    85,    55,    16,
       1,    47,     0,    85,    55,     1,    47,     0,    85,     1,
      47,     0,    55,     0,    58,     0,     0,    44,   235,   116,
      68,     0,    86,   235,    16,    50,   163,   236,    47,     0,
      86,   235,    16,    50,   163,     1,    47,     0,    86,   235,
      16,     1,    47,     0,    86,   235,     1,    47,     0,    86,
       1,    47,     0,     0,   239,     0,   240,     0,   239,   116,
     240,     0,   149,   121,   143,   122,     0,    55,     0,    70,
       0,    64,     0,    65,     0,    87,   149,   241,   238,    47,
       0,    87,   149,    55,     1,    47,     0,    87,   149,     1,
      47,     0,    87,     1,    47,     0,    88,    68,   244,   238,
      47,     0,    88,    68,   244,     1,    47,     0,    88,    68,
       1,    47,     0,    88,     1,    47,     0,    55,     0,    58,
       0,    70,     0,    64,     0,    65,     0,   102,   246,    47,
       0,   102,     1,    47,     0,   246,   111,   247,     0,   246,
     247,     0,   247,     0,   247,   112,   248,     0,   248,     0,
     113,   248,     0,   121,   246,   122,     0,   249,     0,    11,
       0,   127,   175,   128,     0,   181,     0,    66,     0,   250,
       0,   251,   114,     0,   251,     0,    61,     0,    34,     0,
      62,     0,    63,     0,   119,   252,   120,     0,     0,   253,
       0,   254,     0,   253,   116,   254,     0,    59,     0,    60,
       0,    92,   271,   256,    47,     0,    92,     1,    47,     0,
       0,   246,     0,   256,    35,    68,   246,     0,    90,   217,
     163,   258,    47,     0,    90,     1,    47,     0,     0,    40,
     259,     0,   259,   111,   260,     0,   260,     0,   260,   112,
     261,     0,   261,     0,   121,   259,   122,     0,   262,     0,
      14,     0,    41,   119,   252,   120,     0,    42,   119,   252,
     120,     0,    94,   152,    47,     0,    94,     1,    47,     0,
      93,    38,    47,     0,    93,    39,   151,    47,     0,    93,
      39,     1,    47,     0,    93,    38,     1,    47,     0,    93,
       1,    47,     0,     0,    40,   266,     0,   266,   111,   267,
       0,   267,     0,   267,   112,   268,     0,   268,     0,   121,
     266,   122,     0,   269,     0,    14,     0,    41,   119,   172,
     120,     0,    42,   119,   172,   120,     0,    89,   157,   163,
     265,    47,     0,    89,     1,    47,     0,     0,    37,     0,
       0,   166,     0,   272,    35,   149,   166,     0,    91,   271,
     272,    47,     0,    91,     1,    47,     0,     0,   275,     0,
     276,     0,   275,   116,   276,     0,    61,     0,    61,   114,
       0,    62,     0,    62,   114,     0,    63,     0,    63,   114,
       0,    56,     0,    57,     0,    95,   274,    47,     0,    95,
       1,    47,     0,     0,   279,     0,   280,     0,   279,   116,
     280,     0,    61,     0,    61,   114,     0,    62,     0,    62,
     114,     0,    63,     0,    63,   114,     0,    56,     0,    57,
       0,    59,     0,    60,     0,    96,   278,    47,     0,    96,
       1,    47,     0,    98,    68,   283,    47,     0,    98,    69,
     283,    47,     0,    98,     1,    47,     0,   284,     0,   283,
     284,     0,    68,   121,   122,     0,    68,   121,   286,   122,
       0,    68,   121,   286,   116,    72,   122,     0,    21,   115,
     121,   286,   122,     0,    21,   115,   121,   286,   116,    72,
     122,     0,    68,     1,   122,     0,    21,   115,     1,   122,
       0,    21,     1,   122,     0,     1,   122,     0,    99,    68,
     287,    47,     0,    99,     1,    47,     0,   287,     0,   286,
     116,   287,     0,    17,   286,     0,    18,    20,   287,     0,
      68,     0,    68,   125,    50,   116,    50,   126,     0,    68,
     125,    51,   116,    51,   126,     0,    68,   125,   288,   126,
       0,    19,   125,    50,   118,    50,   126,    20,   287,     0,
      19,    20,   287,     0,    19,     1,    20,   287,     0,   149,
       0,   288,   116,   149,     0,   100,   149,   290,    47,     0,
     100,   149,     1,    47,     0,   100,     1,    47,     0,     0,
     290,   291,     0,    26,   284,     0,    25,   284,     0,     0,
     293,     0,   294,     0,   293,   116,   294,     0,     1,   116,
     294,     0,    22,   121,   287,   122,     0,    22,   121,    23,
     116,   149,   122,     0,    22,   121,    24,   116,    52,   122,
       0,    22,   121,    24,   116,    52,   116,    50,   122,     0,
      25,     0,    26,     0,    32,     0,    28,     0,    29,     0,
      30,     0,    31,     0,    33,     0,    27,     0,   107,   149,
     292,    47,     0,   107,   149,     1,    47,     0,   107,     1,
      47,     0,   108,   297,    47,     0,   298,     0,   297,   116,
     298,     0,   149,     0,   149,    11,     0,   149,   119,   172,
     120,     0,   109,   300,    47,     0,   301,     0,   300,   116,
     301,     0,   149,     0,   149,    11,     0,   149,   119,   252,
     120,     0,   110,   303,    47,     0,   304,     0,   303,   116,
     304,     0,   149,     0,   149,    11,     0,   149,   119,   212,
     120,     0,    43,   306,     0,     0,   307,     0,   306,   116,
     307,     0,    73,     0,    11,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   465,   468,   471,   475,   478,   481,   485,   490,   493,
     498,   499,   500,   501,   502,   503,   504,   505,   506,   507,
     508,   509,   510,   511,   512,   513,   514,   515,   516,   517,
     518,   519,   520,   521,   522,   523,   524,   525,   526,   527,
     528,   529,   530,   531,   532,   533,   534,   538,   547,   551,
     557,   568,   611,   620,   628,   631,   634,   638,   644,   647,
     655,   659,   665,   668,   673,   676,   679,   682,   685,   688,
     691,   694,   697,   700,   703,   706,   709,   712,   715,   718,
     721,   724,   727,   730,   733,   736,   739,   742,   745,   748,
     753,   756,   761,   764,   767,   770,   773,   776,   782,   785,
     788,   791,   794,   797,   800,   803,   806,   809,   812,   815,
     818,   821,   824,   827,   830,   833,   836,   839,   842,   845,
     848,   851,   854,   857,   860,   863,   866,   869,   872,   875,
     878,   881,   884,   887,   890,   893,   902,   905,   908,   911,
     914,   917,   920,   923,   926,   929,   932,   939,   942,   947,
     950,   955,   958,   961,   964,   967,   970,   973,   976,   979,
     982,   989,   992,   999,  1002,  1007,  1012,  1019,  1029,  1043,
    1052,  1061,  1069,  1072,  1075,  1078,  1081,  1084,  1087,  1090,
    1093,  1096,  1099,  1105,  1108,  1111,  1115,  1118,  1121,  1124,
    1127,  1130,  1135,  1138,  1143,  1147,  1154,  1157,  1162,  1165,
    1168,  1171,  1174,  1177,  1180,  1183,  1186,  1189,  1192,  1195,
    1198,  1201,  1204,  1207,  1210,  1213,  1216,  1219,  1225,  1228,
    1231,  1234,  1237,  1240,  1245,  1255,  1269,  1278,  1291,  1295,
    1301,  1305,  1313,  1320,  1324,  1330,  1337,  1341,  1349,  1354,
    1359,  1364,  1371,  1374,  1377,  1382,  1385,  1388,  1395,  1398,
    1408,  1411,  1422,  1427,  1433,  1438,  1445,  1450,  1456,  1461,
    1469,  1472,  1475,  1480,  1483,  1486,  1493,  1499,  1507,  1513,
    1523,  1528,  1532,  1538,  1543,  1547,  1555,  1559,  1565,  1569,
    1577,  1580,  1583,  1586,  1589,  1592,  1595,  1598,  1601,  1604,
    1607,  1613,  1616,  1619,  1623,  1626,  1629,  1632,  1635,  1638,
    1643,  1646,  1649,  1653,  1659,  1662,  1665,  1668,  1675,  1680,
    1687,  1690,  1695,  1698,  1703,  1706,  1709,  1712,  1715,  1718,
    1721,  1724,  1727,  1730,  1733,  1740,  1745,  1751,  1758,  1763,
    1769,  1776,  1779,  1786,  1791,  1797,  1805,  1811,  1817,  1823,
    1826,  1833,  1843,  1852,  1855,  1862,  1869,  1878,  1887,  1890,
    1899,  1902,  1913,  1916,  1922,  1928,  1934,  1943,  1946,  1951,
    1954,  1964,  1975,  1980,  1985,  1990,  1999,  2002,  2007,  2011,
    2017,  2022,  2023,  2026,  2029,  2034,  2090,  2097,  2103,  2112,
    2169,  2176,  2182,  2189,  2192,  2195,  2198,  2202,  2214,  2217,
    2224,  2227,  2230,  2233,  2236,  2239,  2242,  2245,  2248,  2251,
    2254,  2260,  2263,  2266,  2270,  2273,  2276,  2279,  2282,  2285,
    2290,  2293,  2296,  2300,  2307,  2310,  2317,  2320,  2327,  2329,
    2332,  2343,  2350,  2357,  2360,  2365,  2368,  2372,  2375,  2378,
    2381,  2384,  2387,  2390,  2397,  2401,  2408,  2411,  2414,  2419,
    2424,  2433,  2436,  2441,  2444,  2447,  2450,  2453,  2456,  2459,
    2462,  2465,  2470,  2476,  2485,  2486,  2489,  2491,  2494,  2500,
    2503,  2512,  2515,  2520,  2524,  2530,  2533,  2537,  2540,  2544,
    2547,  2551,  2554,  2559,  2562,  2571,  2574,  2579,  2583,  2589,
    2592,  2596,  2599,  2603,  2606,  2610,  2613,  2616,  2619,  2624,
    2627,  2636,  2639,  2642,  2649,  2654,  2661,  2664,  2667,  2670,
    2677,  2684,  2689,  2694,  2698,  2706,  2709,  2716,  2721,  2728,
    2735,  2742,  2749,  2756,  2763,  2771,  2782,  2789,  2796,  2800,
    2808,  2811,  2816,  2823,  2826,  2832,  2835,  2842,  2845,  2852,
    2855,  2860,  2866,  2869,  2878,  2890,  2902,  2905,  2908,  2911,
    2914,  2917,  2920,  2923,  2926,  2931,  2935,  2941,  2951,  2956,
    2960,  2966,  2969,  2972,  2979,  2983,  2987,  2993,  2996,  2999,
    3006,  3010,  3014,  3020,  3023,  3026,  3039,  3042,  3049,  3054,
    3061,  3064
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "KEYW_TRUE", "KEYW_FALSE", "KEYW_ACTION", 
  "KEYW_ACCEPT", "KEYW_ANNOUNCE", "KEYW_FROM", "KEYW_TO", "KEYW_AT", 
  "KEYW_ANY", "KEYW_REFINE", "KEYW_EXCEPT", "KEYW_STATIC", 
  "KEYW_NETWORKS", "KEYW_MASKLEN", "KEYW_UNION", "KEYW_RANGE", 
  "KEYW_LIST", "KEYW_OF", "KEYW_OPERATOR", "KEYW_SYNTAX", "KEYW_SPECIAL", 
  "KEYW_REGEXP", "KEYW_OPTIONAL", "KEYW_MANDATORY", "KEYW_INTERNAL", 
  "KEYW_SINGLEVALUED", "KEYW_MULTIVALUED", "KEYW_LOOKUP", "KEYW_KEY", 
  "KEYW_DELETED", "KEYW_OBSOLETE", "KEYW_PEERAS", "KEYW_PROTOCOL", 
  "KEYW_INTO", "KEYW_ATOMIC", "KEYW_INBOUND", "KEYW_OUTBOUND", 
  "KEYW_UPON", "KEYW_HAVE_COMPONENTS", "KEYW_EXCLUDE", "KEYW_AFI", 
  "KEYW_TUNNEL", "TKN_ERROR", "TKN_UNKNOWN_CLASS", "TKN_EOA", "TKN_EOO", 
  "TKN_FREETEXT", "TKN_INT", "TKN_REAL", "TKN_STRING", "TKN_TIMESTAMP", 
  "TKN_BLOB", "TKN_IPV4", "TKN_PRFXV4", "TKN_PRFXV4RNG", "TKN_IPV6", 
  "TKN_PRFXV6", "TKN_PRFXV6RNG", "TKN_ASNO", "TKN_ASNAME", "TKN_RSNAME", 
  "TKN_RTRSNAME", "TKN_PRNGNAME", "TKN_FLTRNAME", "TKN_BOOLEAN", 
  "TKN_WORD", "TKN_RP_ATTR", "TKN_DNS", "TKN_EMAIL", "TKN_3DOTS", 
  "TKN_AFI", "ATTR_GENERIC", "ATTR_BLOBS", "ATTR_REGEXP", "ATTR_IMPORT", 
  "ATTR_MP_IMPORT", "ATTR_EXPORT", "ATTR_MP_EXPORT", "ATTR_DEFAULT", 
  "ATTR_MP_DEFAULT", "ATTR_FREETEXT", "ATTR_CHANGED", "ATTR_IFADDR", 
  "ATTR_INTERFACE", "ATTR_PEER", "ATTR_MP_PEER", "ATTR_INJECT", 
  "ATTR_V6_INJECT", "ATTR_COMPONENTS", "ATTR_V6_COMPONENTS", 
  "ATTR_AGGR_MTD", "ATTR_AGGR_BNDRY", "ATTR_RS_MEMBERS", 
  "ATTR_RS_MP_MEMBERS", "ATTR_RTR_MP_MEMBERS", "ATTR_RP_ATTR", 
  "ATTR_TYPEDEF", "ATTR_PROTOCOL", "ATTR_FILTER", "ATTR_V6_FILTER", 
  "ATTR_MP_FILTER", "ATTR_MP_PEVAL", "ATTR_PEERING", "ATTR_MP_PEERING", 
  "ATTR_ATTR", "ATTR_MNT_ROUTES", "ATTR_MNT_ROUTES6", 
  "ATTR_MNT_ROUTES_MP", "OP_OR", "OP_AND", "OP_NOT", "OP_MS", 
  "TKN_OPERATOR", "','", "'-'", "':'", "'{'", "'}'", "'('", "')'", "'.'", 
  "';'", "'['", "']'", "'<'", "'>'", "'|'", "'*'", "'?'", "'+'", "'~'", 
  "'^'", "'$'", "object", "attribute_list", "attribute", 
  "changed_attribute", "freetext_attribute", "regexp_attribute", 
  "generic_attribute", "generic_list", "generic_non_empty_list", 
  "blobs_attribute", "blobs_list", "list_item", "list_item_0", "tkn_word", 
  "tkn_word_from_keyw", "opt_as_expr", "as_expr", "as_expr_term", 
  "as_expr_factor", "as_expr_operand", "opt_router_expr", 
  "opt_router_expr_with_at", "router_expr", "router_expr_term", 
  "router_expr_factor", "router_expr_operand", "peering", "opt_action", 
  "action", "single_action", "filter", "filter_term", "filter_factor", 
  "filter_operand", "filter_prefix", "filter_prefix_operand", 
  "opt_filter_prefix_list", "filter_prefix_list", 
  "filter_prefix_list_prefix", "filter_aspath", "filter_aspath_term", 
  "filter_aspath_closure", "filter_aspath_factor", "filter_aspath_no", 
  "filter_aspath_range", "filter_rp_attribute", 
  "import_peering_action_list", "export_peering_action_list", 
  "import_factor", "import_factor_list", "export_factor", 
  "export_factor_list", "import_term", "export_term", "import_expr", 
  "export_expr", "opt_protocol_from", "opt_protocol_into", 
  "mp_import_attribute", "mp_export_attribute", "mp_import_expr", 
  "mp_export_expr", "mp_import_term", "mp_export_term", 
  "mp_import_factor", "mp_import_factor_list", "mp_export_factor", 
  "mp_export_factor_list", "mp_import_peering_action_list", 
  "mp_export_peering_action_list", "mp_filter", "mp_filter_term", 
  "mp_filter_factor", "mp_filter_operand", "mp_filter_prefix", 
  "mp_filter_prefix_operand", "opt_mp_filter_prefix_list", 
  "mp_filter_prefix_list", "mp_filter_prefix_list_prefix", "mp_peering", 
  "opt_mp_router_expr", "opt_mp_router_expr_with_at", "mp_router_expr", 
  "mp_router_expr_term", "mp_router_expr_factor", 
  "mp_router_expr_operand", "import_attribute", "export_attribute", 
  "opt_default_filter", "default_attribute", "mp_default_attribute", 
  "opt_default_mp_filter", "filter_attribute", "mp_peval_attribute", 
  "mp_peval", "mp_filter_attribute", "peering_attribute", 
  "mp_peering_attribute", "ifaddr_attribute", "interface_address", 
  "opt_tunnel_spec", "interface_attribute", "opt_peer_options", 
  "peer_options", "peer_option", "peer_id", "peer_attribute", 
  "mp_peer_attribute", "mp_peer_id", "v6_filter_attribute", "v6_filter", 
  "v6_filter_term", "v6_filter_factor", "v6_filter_operand", 
  "v6_filter_prefix", "v6_filter_prefix_operand", 
  "opt_v6_filter_prefix_list", "v6_filter_prefix_list", 
  "v6_filter_prefix_list_prefix", "v6_components_attribute", 
  "v6_components_list", "v6_inject_attribute", "opt_v6_inject_expr", 
  "v6_inject_expr", "v6_inject_expr_term", "v6_inject_expr_factor", 
  "v6_inject_expr_operand", "aggr_bndry_attribute", "aggr_mtd_attribute", 
  "opt_inject_expr", "inject_expr", "inject_expr_term", 
  "inject_expr_factor", "inject_expr_operand", "inject_attribute", 
  "opt_atomic", "components_list", "components_attribute", 
  "opt_rs_members_list", "rs_members_list", "rs_member", 
  "rs_members_attribute", "opt_rs_mp_members_list", "rs_mp_members_list", 
  "rs_mp_member", "rs_mp_members_attribute", "rpattr_attribute", 
  "methods", "method", "typedef_attribute", "typedef_type_list", 
  "typedef_type", "enum_list", "protocol_attribute", "protocol_options", 
  "protocol_option", "opt_attr_options", "attr_options", "attr_option", 
  "attr_attribute", "mnt_routes_attribute", "mnt_routes_list", 
  "mnt_routes_list_item", "mnt_routes6_attribute", "mnt_routes6_list", 
  "mnt_routes6_list_item", "mnt_routes_mp_attribute", 
  "mnt_routes_mp_list", "mnt_routes_mp_list_item", "opt_afi_list", 
  "afi_list", "afi", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,   136,   136,   136,   136,   136,   136,   136,   137,   137,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   139,   139,
     140,   141,   142,   142,   143,   143,   144,   144,   145,   145,
     146,   146,   147,   147,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
     149,   149,   149,   149,   149,   149,   149,   149,   149,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   151,   151,   152,   152,
     153,   153,   153,   154,   154,   155,   155,   156,   156,   157,
     157,   158,   158,   159,   159,   159,   160,   160,   161,   161,
     161,   162,   162,   163,   163,   164,   164,   165,   165,   165,
     165,   165,   166,   166,   166,   167,   167,   168,   168,   168,
     169,   169,   169,   169,   169,   170,   170,   171,   171,   171,
     171,   171,   172,   172,   173,   173,   174,   174,   175,   175,
     176,   176,   177,   177,   177,   177,   177,   177,   178,   178,
     178,   178,   179,   179,   179,   179,   179,   179,   180,   180,
     180,   180,   180,   180,   181,   181,   181,   181,   182,   182,
     183,   183,   184,   185,   185,   186,   187,   187,   188,   188,
     189,   189,   190,   190,   190,   191,   191,   191,   192,   192,
     193,   193,   194,   194,   194,   194,   195,   195,   195,   195,
     196,   196,   196,   197,   197,   197,   198,   198,   199,   199,
     200,   201,   201,   202,   203,   203,   204,   204,   205,   205,
     206,   206,   206,   207,   207,   208,   208,   208,   209,   209,
     209,   209,   209,   210,   210,   211,   211,   211,   211,   211,
     212,   212,   213,   213,   214,   214,   214,   214,   215,   215,
     216,   216,   217,   217,   218,   218,   219,   219,   219,   220,
     220,   221,   221,   221,   221,   222,   222,   222,   223,   223,
     223,   224,   224,   225,   225,   225,   226,   226,   226,   227,
     227,   228,   228,   229,   229,   230,   231,   231,   232,   232,
     233,   233,   234,   234,   234,   234,   234,   235,   235,   236,
     236,   237,   237,   237,   237,   237,   238,   238,   239,   239,
     240,   241,   241,   241,   241,   242,   242,   242,   242,   243,
     243,   243,   243,   244,   244,   244,   244,   244,   245,   245,
     246,   246,   246,   247,   247,   248,   248,   248,   249,   249,
     249,   249,   249,   250,   250,   251,   251,   251,   251,   251,
     252,   252,   253,   253,   254,   254,   255,   255,   256,   256,
     256,   257,   257,   258,   258,   259,   259,   260,   260,   261,
     261,   262,   262,   262,   263,   263,   264,   264,   264,   264,
     264,   265,   265,   266,   266,   267,   267,   268,   268,   269,
     269,   269,   270,   270,   271,   271,   272,   272,   272,   273,
     273,   274,   274,   275,   275,   276,   276,   276,   276,   276,
     276,   276,   276,   277,   277,   278,   278,   279,   279,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   281,
     281,   282,   282,   282,   283,   283,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   285,   285,   286,   286,   287,
     287,   287,   287,   287,   287,   287,   287,   287,   288,   288,
     289,   289,   289,   290,   290,   291,   291,   292,   292,   293,
     293,   293,   294,   294,   294,   294,   294,   294,   294,   294,
     294,   294,   294,   294,   294,   295,   295,   295,   296,   297,
     297,   298,   298,   298,   299,   300,   300,   301,   301,   301,
     302,   303,   303,   304,   304,   304,   305,   305,   306,   306,
     307,   307
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     2,     2,     2,     1,     1,     1,     0,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     4,     3,
       2,     2,     3,     3,     0,     1,     1,     3,     3,     3,
       1,     2,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     1,     3,     1,
       3,     3,     1,     3,     1,     1,     1,     0,     1,     0,
       2,     3,     1,     3,     3,     1,     3,     1,     1,     1,
       1,     3,     1,     0,     2,     1,     2,     7,     4,     5,
       5,     1,     3,     2,     1,     3,     1,     2,     3,     1,
       1,     3,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     3,     0,     1,     1,     3,     1,     1,     3,     1,
       2,     1,     2,     2,     2,     3,     3,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     3,     4,     0,     2,
       2,     2,     4,     2,     6,     3,     4,     4,     3,     4,
       3,     4,     3,     2,     3,     3,     2,     3,     2,     3,
       2,     3,     1,     3,     3,     1,     3,     3,     0,     2,
       0,     2,     5,     6,     7,     3,     5,     6,     7,     3,
       1,     3,     3,     1,     3,     3,     3,     4,     3,     4,
       3,     2,     3,     3,     2,     3,     3,     4,     3,     4,
       3,     2,     1,     3,     1,     2,     3,     1,     1,     3,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     3,
       0,     1,     1,     3,     1,     1,     1,     1,     3,     1,
       0,     1,     0,     2,     3,     1,     3,     3,     1,     3,
       1,     1,     1,     1,     1,     5,     5,     3,     5,     5,
       3,     0,     2,     6,     5,     3,     7,     6,     3,     0,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     6,     6,     5,     4,     3,     1,     1,     0,
       4,     7,     7,     5,     4,     3,     0,     1,     1,     3,
       4,     1,     1,     1,     1,     5,     5,     4,     3,     5,
       5,     4,     3,     1,     1,     1,     1,     1,     3,     3,
       3,     2,     1,     3,     1,     2,     3,     1,     1,     3,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     3,
       0,     1,     1,     3,     1,     1,     4,     3,     0,     1,
       4,     5,     3,     0,     2,     3,     1,     3,     1,     3,
       1,     1,     4,     4,     3,     3,     3,     4,     4,     4,
       3,     0,     2,     3,     1,     3,     1,     3,     1,     1,
       4,     4,     5,     3,     0,     1,     0,     1,     4,     4,
       3,     0,     1,     1,     3,     1,     2,     1,     2,     1,
       2,     1,     1,     3,     3,     0,     1,     1,     3,     1,
       2,     1,     2,     1,     2,     1,     1,     1,     1,     3,
       3,     4,     4,     3,     1,     2,     3,     4,     6,     5,
       7,     3,     4,     3,     2,     4,     3,     1,     3,     2,
       3,     1,     6,     6,     4,     8,     3,     4,     1,     3,
       4,     4,     3,     0,     2,     2,     2,     0,     1,     1,
       3,     3,     4,     6,     6,     8,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     4,     3,     3,     1,
       3,     1,     2,     4,     3,     1,     3,     1,     2,     4,
       3,     1,     3,     1,     2,     4,     2,     0,     1,     3,
       1,     1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     6,     0,     5,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     8,    13,    42,    12,    10,    11,    15,    17,    14,
      16,    18,    19,    35,    38,    37,    39,    40,    22,    23,
      20,    21,    36,    25,    27,    29,    28,    26,    24,    33,
      34,    32,    30,    31,    41,    43,    44,    45,    47,     3,
      46,     2,     0,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   125,   126,
     127,   128,   124,   129,   130,   131,   132,   133,   134,   135,
      65,    66,    67,    68,    87,    69,    70,    71,    72,    73,
      74,    64,    77,    78,    79,    80,    81,    82,    83,    85,
      86,    54,     0,     0,    55,    56,    62,    84,     0,     0,
      60,    51,     0,     0,   250,     0,   250,     0,   250,     0,
     250,     0,     0,     0,     0,     0,    50,     0,     0,     0,
       0,     0,   357,   358,     0,     0,    91,    92,    93,    94,
      95,    96,    97,    90,     0,    98,     0,     0,     0,     0,
     163,     0,     0,   163,     0,   455,   456,     0,   418,     0,
       0,     0,     0,   145,   146,     0,     0,   139,   142,   144,
       0,   471,   472,   465,   467,   469,     0,   462,   463,     0,
     485,   486,   487,   488,   479,   481,   483,     0,   476,   477,
       0,     0,     0,     0,     0,     0,     0,     0,   180,   188,
     187,   189,   190,   183,     0,     0,   192,     0,     0,     0,
     174,   176,   179,   184,   186,   182,     0,   398,   406,   405,
     407,   408,   401,     0,   410,     0,     0,   400,     0,   392,
     394,   397,   402,   404,     0,   288,   296,   295,   297,   298,
     291,     0,   300,     0,     0,   290,     0,   282,   284,   287,
     292,   294,     0,     0,     0,     0,   162,   147,     0,     0,
     309,   310,     0,     0,     0,   551,     0,   549,   557,     0,
     555,   563,     0,   561,     0,     1,     9,    53,     0,     0,
       0,     0,    52,     0,     0,    59,    58,    61,   327,   249,
       0,     0,   255,   567,   330,     0,   259,   567,   335,     0,
     338,   571,   570,   566,   568,     0,    49,     0,   356,     0,
       0,   365,     0,     0,   378,     0,     0,   373,   374,   372,
     366,   382,     0,   383,   384,   386,   387,   385,     0,   453,
     158,   160,   159,     0,   150,   152,   155,   157,     0,   441,
     422,   321,   322,   324,   323,     0,   313,   315,   318,   320,
     423,   460,   457,     0,   417,   419,     0,   440,     0,   436,
       0,     0,   137,   435,     0,   434,     0,     0,     0,   474,
     466,   468,   470,   473,     0,   490,   480,   482,   484,   489,
       0,   493,     0,     0,     0,     0,   494,     0,   506,     0,
       0,     0,   511,     0,   522,     0,     0,   342,     0,    54,
       0,    54,   177,   196,   197,     0,   193,   194,     0,   213,
     212,   214,     0,   215,   218,   208,   209,     0,   199,   201,
     207,   211,   341,     0,   173,     0,   185,   389,   395,   414,
     415,     0,   411,   412,     0,     0,   388,     0,   391,     0,
     403,   347,   285,   306,   307,   304,   305,     0,   301,   302,
       0,     0,   346,     0,   281,     0,   293,   344,     0,   343,
     349,   149,   148,   348,   351,   312,   311,   350,   547,     0,
       0,   536,   537,   544,   539,   540,   541,   542,   538,   543,
       0,   528,   529,   552,   192,   548,     0,   558,   410,   554,
       0,   564,   300,   560,     0,    75,    76,    88,    89,    57,
      63,   251,     0,     0,     0,     0,   242,     0,     0,     0,
     260,     0,     0,     0,     0,     0,   245,     0,     0,     0,
     263,     0,     0,   331,     0,     0,    48,   355,     0,     0,
     364,     0,   163,   377,     0,     0,     0,   367,   368,   381,
       0,     0,     0,     0,     0,     0,     0,   171,   164,   165,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   459,
       0,   416,   439,   438,   437,   143,   138,   141,   140,   464,
     478,   504,     0,     0,     0,     0,   491,   495,   492,   509,
     507,     0,     0,     0,     0,     0,   505,   521,     0,     0,
     520,   524,   225,     0,     0,     0,   191,     0,   178,     0,
     218,     0,   181,     0,   200,   202,   203,   204,     0,   172,
     175,   409,     0,   396,   399,   390,   393,   299,     0,   286,
     289,   280,   283,   345,   161,   308,   546,     0,     0,   545,
       0,     0,   550,     0,   556,     0,   562,   163,     0,     0,
       0,     0,   326,   238,     0,     0,   325,     0,   252,   567,
     567,     0,     0,     0,     0,   163,     0,     0,     0,     0,
     329,   240,     0,     0,   328,     0,   256,   567,   567,     0,
       0,     0,     0,   334,     0,     0,   569,     0,   339,   354,
       0,     0,   363,     0,   376,    54,   375,     0,   380,   379,
     156,   151,   154,   153,     0,    54,     0,    54,   166,   449,
       0,     0,     0,   442,   444,   446,   448,   452,   319,   314,
     317,   316,   431,     0,     0,     0,   424,   426,   428,   430,
     421,     0,     0,   503,     0,     0,   501,   496,     0,     0,
     510,     0,   516,     0,     0,     0,   518,     0,   526,   525,
     226,    54,   227,   195,   210,     0,   220,   219,   223,   221,
     216,   198,   206,   205,   413,   303,   531,     0,     0,     0,
     530,   553,   559,   565,   228,   233,   239,     0,   232,   163,
       0,   243,   244,     0,   261,     0,   262,   163,     0,     0,
     253,   266,     0,     0,   230,   236,   241,     0,   235,   163,
       0,   246,   247,     0,   264,     0,   265,   163,     0,     0,
     257,   268,     0,     0,   332,   333,   337,     0,     0,   353,
     352,     0,     0,     0,     0,   369,     0,     0,     0,     0,
     192,   192,     0,     0,     0,   410,   410,     0,     0,     0,
     458,   420,   502,     0,     0,   497,   508,   517,     0,     0,
       0,     0,   514,     0,   217,     0,     0,     0,   532,   234,
     229,   254,     0,   276,   271,   267,     0,   270,   163,   237,
     231,   258,     0,   278,   274,   269,     0,   273,   163,   340,
     336,   362,     0,   361,   370,   168,     0,    54,     0,     0,
       0,   447,   443,   445,     0,     0,   429,   425,   427,     0,
     499,     0,     0,     0,     0,   519,   224,   222,     0,     0,
     272,   277,   275,   279,     0,   169,     0,   170,   450,   451,
     432,   433,     0,   498,     0,   512,   513,   533,     0,   534,
     360,     0,   500,     0,     0,   167,   515,   535,     0,     0,
       0
};

static const short yydefgoto[] =
{
     968,    40,    41,    42,    43,    44,    45,   143,   144,    46,
     149,   145,   146,   575,   185,   401,   297,   207,   208,   209,
     501,   190,   374,   375,   376,   377,   298,   379,   588,   589,
     249,   250,   251,   252,   253,   254,   445,   446,   447,   457,
     458,   459,   460,   461,   641,   255,   544,   554,   810,   679,
     830,   697,   546,   556,   547,   557,   154,   331,    47,    48,
     549,   559,   550,   560,   693,   819,   711,   839,   694,   712,
     286,   287,   288,   289,   290,   291,   487,   488,   489,   302,
     505,   193,   386,   387,   388,   389,    49,    50,   715,    51,
      52,   848,    53,    54,   294,    55,    56,    57,    58,   174,
     853,    59,   576,   577,   578,   360,    60,    61,   368,    62,
     268,   269,   270,   271,   272,   273,   471,   472,   473,    63,
     396,    64,   597,   756,   757,   758,   759,    65,    66,   591,
     743,   744,   745,   746,    67,   196,   393,    68,   216,   217,
     218,    69,   227,   228,   229,    70,    71,   425,   426,    72,
     619,   620,   777,    73,   436,   631,   520,   521,   522,    74,
      75,   306,   307,    76,   309,   310,    77,   312,   313,   815,
     343,   344
};

static const short yypact[] =
{
    1279,   477,     0,    21,   911,   983,    73,    59,    57,    61,
      55,    67,   526,   103,    56,   258,   306,  1593,   212,   734,
     753,    52,    70,   430,    40,   542,   614,   229,   241,  1661,
     141,   230,   278,   244,    47,    79,  1729,  1861,  1861,  1861,
    1347,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,   162,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,     8,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   114,
  -32768,  1128,   418,   219,   187,-32768,   191,-32768,   274,  1056,
  -32768,-32768,   316,  1861,   359,   369,   359,   381,   359,   396,
     359,   406,   221,   429,    50,   485,-32768,   455,   436,   465,
     439,   469,-32768,-32768,   481,   487,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,   268,-32768,   494,   349,   500,   443,
     515,   504,   497,   515,   518,-32768,   418,   550,  1209,   567,
     299,    87,   569,-32768,-32768,   195,   154,    31,-32768,-32768,
     578,-32768,-32768,   520,   523,   531,   593,   532,-32768,   615,
  -32768,-32768,-32768,-32768,   554,   590,   598,   670,   606,-32768,
     673,   284,   284,   685,   441,   695,   376,   698,-32768,-32768,
  -32768,-32768,-32768,-32768,   586,   418,   564,   418,   383,   364,
     635,-32768,-32768,-32768,   639,-32768,   712,-32768,-32768,-32768,
  -32768,-32768,-32768,  1209,   583,  1209,   383,-32768,   427,   650,
  -32768,-32768,-32768,   659,   729,-32768,-32768,-32768,-32768,-32768,
  -32768,  1225,   638,  1225,   383,-32768,   467,   668,-32768,-32768,
  -32768,   672,   752,    50,   754,   785,-32768,   384,   787,   788,
  -32768,   498,   789,   791,   656,    32,   147,-32768,    62,   151,
  -32768,   105,   158,-32768,   793,-32768,-32768,-32768,   781,   792,
     721,   597,-32768,  1128,  1128,-32768,-32768,-32768,-32768,-32768,
    1861,    58,-32768,   801,-32768,    66,-32768,   803,-32768,   361,
  -32768,-32768,-32768,   733,-32768,   249,-32768,   804,-32768,   805,
     287,-32768,   807,   328,-32768,   808,  1457,-32768,-32768,-32768,
    1861,-32768,   809,-32768,-32768,-32768,-32768,-32768,  1525,-32768,
  -32768,-32768,-32768,   443,   747,   111,-32768,-32768,   -14,   810,
  -32768,-32768,-32768,-32768,-32768,   497,   748,   115,-32768,-32768,
     817,-32768,  1163,   112,-32768,  1189,   226,-32768,   813,-32768,
     814,   815,   756,-32768,   438,-32768,   195,   195,   195,-32768,
  -32768,-32768,-32768,-32768,   722,-32768,-32768,-32768,-32768,-32768,
     746,-32768,   741,    41,    48,   353,-32768,   389,-32768,   441,
     844,    39,   740,   821,-32768,   823,   474,-32768,  1128,  1128,
     806,  1128,-32768,-32768,-32768,   758,   757,-32768,   630,-32768,
  -32768,-32768,   383,-32768,   738,-32768,-32768,   525,   383,   595,
  -32768,   749,-32768,   418,   635,   418,-32768,-32768,-32768,-32768,
  -32768,   760,   768,-32768,   726,   551,-32768,  1209,   650,  1209,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,   767,   772,-32768,
     764,   600,-32768,  1225,   668,  1225,-32768,-32768,   209,-32768,
  -32768,   866,   747,-32768,-32768,   879,   748,-32768,-32768,   165,
     769,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
     845,   777,-32768,-32768,   564,-32768,  1861,-32768,   583,-32768,
    1861,-32768,   638,-32768,  1861,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,   221,   886,   228,    36,   702,   848,    50,   849,
     743,    74,   221,   888,   374,    72,   778,   851,    50,   852,
     784,   102,   853,   887,    50,   417,-32768,-32768,   854,   419,
  -32768,   856,   515,-32768,   857,   786,   858,   790,-32768,-32768,
     861,   862,   448,   443,   443,   443,   625,-32768,   -14,-32768,
     174,   863,   473,   497,   497,   497,   235,   864,  1861,-32768,
     883,-32768,-32768,-32768,-32768,-32768,    31,-32768,-32768,-32768,
  -32768,-32768,   830,    51,   831,   233,-32768,-32768,-32768,   838,
  -32768,   441,   935,   441,   906,  1795,-32768,-32768,   284,   284,
  -32768,-32768,-32768,   835,   839,   833,-32768,   564,-32768,   334,
  -32768,   256,-32768,   383,   595,-32768,-32768,-32768,   449,   635,
  -32768,-32768,   583,-32768,-32768,   650,-32768,-32768,   638,-32768,
  -32768,   668,-32768,  1200,-32768,-32768,-32768,   739,   632,-32768,
     739,   860,-32768,   865,-32768,   903,-32768,   515,   859,    37,
     418,   221,-32768,-32768,    58,    58,-32768,    71,-32768,   981,
     981,   249,  1017,    75,   579,   515,   902,    94,   418,   221,
  -32768,-32768,    66,    66,-32768,    76,-32768,   981,   981,   249,
    1018,    82,   604,-32768,   418,   982,-32768,   984,  1013,-32768,
    1005,  1008,-32768,   301,-32768,  1128,-32768,  1861,-32768,-32768,
  -32768,   111,-32768,-32768,  1128,  1128,   988,  1128,-32768,-32768,
     938,   939,   174,   985,   986,-32768,-32768,-32768,-32768,   115,
  -32768,-32768,-32768,   978,   980,   235,   989,   993,-32768,-32768,
  -32768,   418,  1209,-32768,   979,   441,-32768,-32768,   237,   441,
  -32768,   441,-32768,  1007,  1012,  1014,-32768,   208,-32768,-32768,
  -32768,  1128,-32768,-32768,-32768,   324,-32768,  1051,-32768,-32768,
  -32768,   383,-32768,-32768,-32768,-32768,-32768,  1053,  1054,  1049,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,  1048,  1163,   515,
    1052,-32768,-32768,  1082,-32768,    74,-32768,   515,  1077,    38,
  -32768,-32768,  1225,   249,-32768,-32768,-32768,  1078,  1163,   515,
    1079,-32768,-32768,  1126,-32768,   102,-32768,   515,  1080,    97,
  -32768,-32768,  1225,   249,  1163,-32768,-32768,  1225,  1158,-32768,
  -32768,  1159,   508,  1160,  1086,-32768,  1085,  1088,  1091,  1087,
     564,   564,   490,   174,   174,   583,   583,   495,   235,   235,
    1163,  1189,-32768,   290,   454,-32768,-32768,-32768,  1164,  1165,
    1166,  1861,-32768,  1094,-32768,  1157,  1861,  1167,-32768,-32768,
  -32768,-32768,  1097,-32768,-32768,-32768,  1098,  1200,   515,-32768,
  -32768,-32768,  1103,-32768,-32768,-32768,  1104,  1200,   515,  1200,
  -32768,-32768,  1114,-32768,-32768,-32768,  1107,  1128,  1109,  1115,
    1117,-32768,   986,-32768,  1118,  1119,-32768,   993,-32768,   555,
  -32768,  1120,  1122,  1127,  1130,-32768,-32768,-32768,  1123,   293,
  -32768,-32768,-32768,-32768,  1172,-32768,  1124,-32768,-32768,-32768,
  -32768,-32768,  1132,-32768,  1221,-32768,-32768,-32768,  1194,-32768,
  -32768,  1133,-32768,   441,  1138,-32768,-32768,-32768,  1264,  1265,
  -32768
};

static const short yypgoto[] =
{
  -32768,-32768,  1227,-32768,-32768,-32768,-32768,  -135,-32768,-32768,
  -32768,     4,   944,   -15,    -4,-32768,   -19,   867,   403,-32768,
  -32768,   776,  -243,   700,   231,-32768,  -149,  -185,-32768,   693,
    -122,  -237,  -217,-32768,-32768,-32768,  -488,-32768,   648,  -234,
     646,  -431,-32768,-32768,   652,   -28,-32768,-32768,  -314,-32768,
    -309,-32768,-32768,-32768,   128,   118,   620,   509,-32768,-32768,
     133,   110,-32768,-32768,  -653,-32768,  -672,-32768,-32768,-32768,
    -265,  -271,  -248,-32768,-32768,-32768,   761,-32768,   637,  -320,
  -32768,   794,  -250,   703,   234,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   445,
  -32768,-32768,   930,-32768,   574,-32768,-32768,-32768,-32768,-32768,
    -187,  -249,  -233,-32768,-32768,-32768,  -497,-32768,   651,-32768,
  -32768,-32768,-32768,   549,   437,   440,-32768,-32768,-32768,-32768,
     565,   451,   442,-32768,-32768,  1290,-32768,-32768,-32768,-32768,
     901,-32768,-32768,-32768,   897,-32768,-32768,  1099,  -390,-32768,
    -581,  -227,-32768,-32768,-32768,-32768,-32768,-32768,   -74,-32768,
  -32768,-32768,   797,-32768,-32768,   796,-32768,-32768,   795,    -2,
    -264,   770
};


#define	YYLAST		1929


static const short yytable[] =
{
     147,   147,   184,   267,   285,   206,   320,   433,   390,   150,
     165,   395,   464,   339,   236,   494,   301,   545,   490,   478,
     321,   304,   305,   308,   311,   565,   555,   644,   442,   498,
     468,   673,   475,   482,   768,   617,   671,   617,   838,   818,
     622,   202,   612,   523,   407,   542,   691,    80,   295,   614,
     491,   506,   764,   194,   502,   586,   159,   167,   155,   623,
     152,   341,   157,  -454,  -248,  -248,   542,  -248,   161,    81,
    -248,   197,   813,   527,   392,   552,   162,   833,   474,  -566,
     299,  -454,   691,   682,   464,  -566,  -454,  -454,   400,   195,
     153,  -248,   153,  -248,   153,  -248,   153,  -248,  -248,  -454,
    -248,   203,   204,   552,  -454,  -454,   709,   195,   203,   204,
     587,   709,   296,  -454,  -454,  -454,   531,  -454,  -454,   700,
     151,  -454,   820,   342,   584,   448,   318,   168,   594,   840,
     582,  -454,  -454,  -454,  -136,   592,  -454,   147,   329,  -454,
     203,   204,   237,   408,   300,   147,   478,   598,   203,   204,
     166,   524,   238,   327,   563,   464,   613,   806,   895,   599,
     683,   205,   892,   902,   624,  -454,   896,   906,   205,   615,
     267,  -454,   765,  -454,  -248,   239,  -248,   543,  -248,  -454,
    -248,   528,   402,  -454,   873,   553,   404,   564,   739,  -454,
    -566,  -454,   564,   692,   525,  -566,   701,  -454,   529,   821,
     205,   405,   240,   241,   242,   533,   841,   243,   205,   317,
     244,   464,   666,   186,   826,   740,   741,   905,   639,   494,
     275,   710,   661,   585,   532,   478,   649,   595,   655,   678,
     230,   256,   319,   663,   680,   267,   681,   267,   778,   779,
     267,   257,   233,   276,   696,   292,   656,   662,   650,   752,
     429,   430,   431,   285,   245,   285,   203,   204,   285,   169,
     246,   600,   247,   526,   258,   406,   322,   530,   248,   355,
     277,   278,   279,   601,   534,   280,   753,   754,   244,   274,
     187,   667,   203,   204,   687,   422,   296,   293,   568,   275,
     786,   259,   260,   261,   705,   742,   262,   231,   232,   244,
     398,   432,   851,   323,   633,   423,   635,   171,   324,   234,
     203,   204,   276,   170,   300,   541,   205,   787,   788,   147,
     147,   325,   281,   356,   881,   564,   301,   539,   282,   571,
     283,   551,   357,   358,   882,   561,   284,   569,   359,   277,
     278,   279,   205,   263,   280,   852,   399,   244,  -359,   264,
     362,   265,   424,   874,   422,   767,   755,   266,   786,   875,
     644,   172,   562,   328,   173,   807,   378,   267,   924,   925,
     205,   817,   919,   920,   423,   238,  -163,   435,   572,   789,
     718,   698,   790,   699,   721,   787,   788,   723,   827,   837,
     422,   281,   494,   677,   770,   330,   772,   282,   239,   283,
     616,  -523,  -523,   695,   363,   284,   929,   364,  -163,   958,
     423,   462,   930,   365,   366,   959,   332,   449,   717,   367,
     720,   424,   378,  -523,   378,   240,   241,   242,   334,   238,
     243,   199,  -163,   244,   147,   147,   618,   147,   257,   370,
     349,   799,   632,   336,   450,   451,   267,   789,   371,   267,
     884,   267,   239,   338,   372,   350,   784,   424,   429,   430,
     431,   258,   285,   643,  -163,   285,  -163,   285,   200,   201,
     285,   429,   430,   431,   476,   463,   340,   245,   275,   240,
     241,   242,   352,   246,   243,   247,   347,   244,   259,   260,
     261,   248,   804,   262,   345,   406,   244,   353,   370,   628,
     629,   276,   346,   898,   452,   373,   453,   371,   454,   432,
     824,   305,   348,   372,   492,   308,   351,   455,   456,   311,
     378,   630,   432,   908,    78,    79,   931,   163,   277,   278,
     279,   245,   809,   280,   354,  -567,   244,   246,   477,   247,
     263,   361,   876,   210,   877,   248,   264,   369,   265,   406,
     829,   380,   381,   381,   266,   382,   382,   897,   808,   583,
     605,   383,   383,   172,   373,   391,   173,   384,   384,   164,
     730,   464,   429,   430,   431,   871,   828,   907,   493,   792,
     281,   793,   909,   761,   593,   822,   282,   823,   283,  -461,
     854,   464,   844,   796,   284,   748,   800,   394,   211,   212,
     857,   863,   859,   213,   214,   215,   868,   464,   238,   406,
     776,   842,   921,   843,   397,   219,   403,   926,   385,   385,
     443,   444,   478,   432,   890,   409,   494,   952,   156,   158,
     160,   239,   893,   464,   410,   285,   494,   411,   494,   870,
     413,   238,   469,   470,   900,   412,   883,   876,   414,   429,
     430,   431,   903,   642,   643,   797,   798,   509,   240,   241,
     242,  -475,   415,   243,   239,   333,   244,   335,   416,   337,
     220,   221,   301,   222,   223,   224,   225,   226,   510,   654,
     643,   511,   512,   513,   514,   515,   516,   517,   518,   519,
     301,   240,   241,   242,   483,   484,   243,   485,   486,   244,
     432,   438,   876,  -527,   417,   835,   835,   439,   463,   440,
     245,   441,   418,   941,   684,   685,   246,   419,   247,   538,
     421,   147,   420,   943,   248,   645,   646,   647,   660,   643,
     147,   147,   428,   147,   267,   188,   966,   257,   856,  -149,
     734,   463,   434,   245,   189,   437,   735,   465,   736,   246,
     737,   247,   638,   466,   191,   689,   690,   248,  -312,   467,
     258,   510,   479,   192,   511,   512,   513,   514,   515,   516,
     517,   518,   519,   480,  -149,   275,   481,   147,   211,   212,
     495,  -149,   946,   213,   214,   215,   496,   259,   260,   261,
     702,   703,   262,  -312,   285,   244,   707,   708,   276,   497,
    -312,   499,   220,   221,   301,   222,   223,   224,   225,   226,
     607,   608,   811,   812,   285,   732,   733,   834,   836,   285,
     831,   832,   814,   816,   301,   277,   278,   279,   750,   751,
     280,   535,   500,   244,   503,   504,   507,   477,   508,   263,
      78,   537,   536,   267,   548,   264,   558,   265,   653,   564,
     590,   566,   567,   266,   570,   573,   579,   596,   583,   593,
     602,   603,   604,   611,   621,   625,   935,   406,   626,   285,
     627,   938,   640,   637,   634,   493,   189,   281,   636,   285,
     651,   285,   648,   282,   652,   283,   659,   657,   658,   192,
     668,   284,   669,   670,   542,   686,   688,   552,   704,   706,
     713,   719,   714,   722,   724,   726,   727,   725,   728,   729,
     747,   760,    82,   147,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   762,   763,   766,   769,   771,   773,   780,   -54,   782,
     781,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     801,   139,   140,   805,   148,   802,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   803,   164,   691,   825,   709,   847,   845,
     141,   846,   142,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   849,   139,   140,   850,   858,   860,   861,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   863,   865,   864,   866,
     868,   872,   141,   326,   142,   869,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   878,   139,   140,   879,   891,
     880,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   885,   886,
     887,   888,   889,   901,   238,   141,   683,   142,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   239,   139,   140,
     257,   894,   899,   701,   904,   910,   911,   913,   914,   915,
     916,   275,   917,   918,   932,   933,   936,   934,   937,   939,
     257,   821,   940,   258,   240,   241,   242,   841,   942,   243,
     944,   945,   244,   947,   276,   948,   275,   949,   950,   951,
     960,   963,   953,   258,   964,   957,   961,   141,   954,   142,
     259,   260,   261,   955,   962,   262,   956,   965,   244,   276,
     967,   277,   278,   279,   969,   970,   280,   316,   540,   244,
     259,   260,   261,   606,   463,   262,   245,   664,   244,    -7,
       1,   738,   246,   731,   247,   783,   277,   278,   279,   791,
     248,   280,   785,   675,   244,   795,   749,   912,   581,   665,
     477,   855,   263,   794,   867,   927,   923,   862,   264,   928,
     265,   493,   198,   281,   922,   609,   266,   610,     0,   282,
       0,   283,   263,   672,     2,     3,   674,   284,   264,   676,
     265,   427,     0,     0,   716,     0,   266,     0,   281,     0,
       0,     0,     0,     0,   282,     0,   283,    -4,   314,     0,
       0,     0,   284,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,     0,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
       0,     0,     2,     0,     0,   315,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,     0,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,   574,     0,
    -371,  -371,  -371,  -371,  -371,  -371,  -371,  -371,  -371,  -371,
    -371,  -371,  -371,  -371,  -371,  -371,  -371,  -371,  -371,  -371,
    -371,  -371,  -371,  -371,  -371,  -371,  -371,  -371,  -371,  -371,
    -371,  -371,  -371,  -371,  -371,  -371,  -371,     0,     0,     0,
       0,     0,     0,     0,  -371,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -371,  -371,
    -371,  -371,  -371,  -371,  -371,  -371,   580,     0,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,     0,     0,     0,     0,     0,
       0,     0,  -366,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,   177,   178,   179,
     180,   181,   182,   183,   175,     0,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,   177,   178,   179,   180,   181,
     182,   183,   235,     0,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,   177,   178,   179,   180,   181,   182,   183,
     303,     0,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     176,   177,   178,   179,   180,   181,   182,   183,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   774,   775,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,   177,   178,   179,
     180,   181,   182,   183,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,   177,   178,   179,   180,   181,   182,   183
};

static const short yycheck[] =
{
       4,     5,    17,    31,    32,    24,   141,   234,   193,     5,
      12,   198,   249,   162,    29,   286,    35,   331,   283,   268,
     142,    36,    37,    38,    39,   345,   335,   458,   245,   293,
     263,   528,   266,   281,   615,   425,   524,   427,   710,   692,
       1,     1,     1,    11,    13,     8,     8,    47,     1,     1,
     284,   301,     1,     1,   297,    69,     1,     1,     1,    20,
       1,    11,     1,    11,     9,     8,     8,     8,     1,    48,
       9,     1,     1,    11,   196,     9,     9,     1,   265,     8,
       1,    11,     8,    47,   321,     9,    34,    35,     1,    37,
      35,    36,    35,    36,    35,    36,    35,    36,    43,    47,
      43,    61,    62,     9,    34,    35,     9,    37,    61,    62,
     124,     9,    65,    61,    62,    63,    11,    47,    66,    47,
      47,    69,    47,    73,    13,   247,   118,    71,    13,    47,
     373,    61,    62,    63,    47,   385,    66,   141,   153,    69,
      61,    62,     1,   112,    65,   149,   395,    35,    61,    62,
      47,   119,    11,   149,   339,   392,   115,   120,   120,    47,
     124,   121,   815,   835,   125,   113,   819,   839,   121,   121,
     198,   119,   121,   121,   119,    34,   119,   119,   119,   127,
     119,   119,   201,   113,   765,   119,   205,   116,    14,   119,
     119,   121,   116,   119,    47,   119,   124,   127,    47,   124,
     121,    47,    61,    62,    63,    47,   124,    66,   121,    47,
      69,   448,    47,     1,   120,    41,    42,   120,   452,   490,
      11,   119,   493,   112,   119,   474,   463,   112,   477,   543,
       1,     1,   118,   498,     6,   263,     8,   265,   628,   629,
     268,    11,     1,    34,   553,     1,   479,   495,   465,    14,
      17,    18,    19,   281,   113,   283,    61,    62,   286,     1,
     119,    35,   121,   116,    34,   111,    47,   116,   127,     1,
      61,    62,    63,    47,   116,    66,    41,    42,    69,     1,
      68,   116,    61,    62,   548,     1,    65,    43,     1,    11,
      34,    61,    62,    63,   558,   121,    66,    68,    69,    69,
       1,    68,     1,   116,   439,    21,   441,     1,   117,    68,
      61,    62,    34,    55,    65,   330,   121,    61,    62,   323,
     324,    47,   113,    55,   116,   116,   345,   323,   119,     1,
     121,   333,    64,    65,   126,   337,   127,    50,    70,    61,
      62,    63,   121,   113,    66,    44,    47,    69,    47,   119,
       1,   121,    68,   116,     1,   122,   121,   127,    34,   122,
     791,    55,     1,    47,    58,   679,     5,   395,   865,   866,
     121,   691,   860,   861,    21,    11,    15,     1,    50,   123,
     565,     7,   126,     9,   569,    61,    62,   572,   697,   709,
       1,   113,   663,   542,   621,    36,   623,   119,    34,   121,
      47,    25,    26,   552,    55,   127,   116,    58,    47,   116,
      21,    47,   122,    64,    65,   122,    47,    34,     1,    70,
       1,    68,     5,    47,     5,    61,    62,    63,    47,    11,
      66,     1,    15,    69,   438,   439,    47,   441,    11,    55,
       1,   668,   438,    47,    61,    62,   474,   123,    64,   477,
     126,   479,    34,    47,    70,    16,   122,    68,    17,    18,
      19,    34,   490,   129,    47,   493,    47,   495,    38,    39,
     498,    17,    18,    19,    47,   111,    47,   113,    11,    61,
      62,    63,     1,   119,    66,   121,    50,    69,    61,    62,
      63,   127,   677,    66,     9,   111,    69,    16,    55,    25,
      26,    34,    47,   823,   121,   121,   123,    64,   125,    68,
     695,   526,    47,    70,    47,   530,    47,   134,   135,   534,
       5,    47,    68,   843,    47,    48,    72,     1,    61,    62,
      63,   113,   681,    66,    47,     9,    69,   119,   111,   121,
     113,    47,   769,     1,   771,   127,   119,    47,   121,   111,
     699,    47,    55,    55,   127,    58,    58,   822,   680,   111,
     122,    64,    64,    55,   121,    47,    58,    70,    70,    43,
     122,   808,    17,    18,    19,   762,   698,   842,   111,   130,
     113,   132,   847,   598,   111,     6,   119,     8,   121,    47,
     725,   828,   714,   667,   127,   122,   670,    47,    56,    57,
     735,   111,   737,    61,    62,    63,   111,   844,    11,   111,
     625,     7,   122,     9,    47,     1,    47,   122,   121,   121,
      56,    57,   871,    68,   809,    47,   897,    72,     8,     9,
      10,    34,   817,   870,   114,   663,   907,   114,   909,   761,
      47,    11,    59,    60,   829,   114,   781,   874,   116,    17,
      18,    19,   837,   128,   129,    23,    24,     1,    61,    62,
      63,    47,    47,    66,    34,   156,    69,   158,   114,   160,
      56,    57,   691,    59,    60,    61,    62,    63,    22,   128,
     129,    25,    26,    27,    28,    29,    30,    31,    32,    33,
     709,    61,    62,    63,    56,    57,    66,    59,    60,    69,
      68,   115,   929,    47,   114,   707,   708,   121,   111,   123,
     113,   125,   114,   898,    12,    13,   119,    47,   121,   122,
      47,   725,   116,   908,   127,   130,   131,   132,   128,   129,
     734,   735,    47,   737,   762,     1,   963,    11,   734,     5,
     115,   111,    47,   113,    10,    47,   121,   112,   123,   119,
     125,   121,   122,   114,     1,    12,    13,   127,     5,    47,
      34,    22,   112,    10,    25,    26,    27,    28,    29,    30,
      31,    32,    33,   114,    40,    11,    47,   781,    56,    57,
     112,    47,   917,    61,    62,    63,   114,    61,    62,    63,
      12,    13,    66,    40,   822,    69,    12,    13,    34,    47,
      47,    47,    56,    57,   823,    59,    60,    61,    62,    63,
     407,   408,   684,   685,   842,   584,   585,   707,   708,   847,
     702,   703,   689,   690,   843,    61,    62,    63,   594,   595,
      66,    50,    47,    69,    47,    47,    47,   111,    47,   113,
      47,   120,    50,   871,    43,   119,    43,   121,   122,   116,
      40,    47,    47,   127,    47,    47,    47,    40,   111,   111,
      47,    47,    47,   122,    20,   125,   881,   111,    47,   897,
      47,   886,   134,   116,    68,   111,    10,   113,   120,   907,
     120,   909,   133,   119,   116,   121,   122,   120,   116,    10,
     121,   127,    47,   116,     8,    47,    47,     9,    47,    47,
      47,    47,    15,    47,    47,    47,   116,   121,    47,    47,
      47,    47,     1,   917,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    68,   122,   122,   116,    20,    50,   122,    47,   126,
     121,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
     120,    70,    71,   124,     1,   120,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,   120,    43,     8,   124,     9,    15,    47,
     119,    47,   121,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    47,    70,    71,    47,    68,   119,   119,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,   111,   119,   112,   119,
     111,   122,   119,    47,   121,   112,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,   118,    70,    71,   116,    47,
     116,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,   117,   116,
     116,   122,   124,    47,    11,   119,   124,   121,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    34,    70,    71,
      11,   124,   124,   124,   124,    47,    47,    47,   122,   124,
     122,    11,   121,   126,    50,    50,   122,    51,    61,    52,
      11,   124,   124,    34,    61,    62,    63,   124,   124,    66,
     116,   124,    69,   124,    34,   120,    11,   120,   120,   120,
      68,    20,   122,    34,    50,   122,   122,   119,   126,   121,
      61,    62,    63,   126,   122,    66,   126,   124,    69,    34,
     122,    61,    62,    63,     0,     0,    66,    40,   324,    69,
      61,    62,    63,   406,   111,    66,   113,   501,    69,     0,
       1,   588,   119,   583,   121,   637,    61,    62,    63,   643,
     127,    66,   640,   532,    69,   658,   593,   852,   368,   505,
     111,   727,   113,   652,   755,   868,   864,   742,   119,   869,
     121,   111,    22,   113,   863,   414,   127,   420,    -1,   119,
      -1,   121,   113,   526,    45,    46,   530,   127,   119,   534,
     121,   232,    -1,    -1,   564,    -1,   127,    -1,   113,    -1,
      -1,    -1,    -1,    -1,   119,    -1,   121,     0,     1,    -1,
      -1,    -1,   127,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    -1,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
      -1,    -1,    45,    -1,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    -1,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,     1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,
      63,    64,    65,    66,    67,    68,     1,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    62,    63,    64,
      65,    66,    67,    68,     1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    61,    62,    63,    64,    65,    66,
      67,    68,     1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    62,    63,    64,    65,    66,    67,    68,
       1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    62,    63,    64,    65,    66,    67,    68,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    62,    63,    64,
      65,    66,    67,    68,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    62,    63,    64,    65,    66,    67,    68
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
#line 465 "rpsl.y"
{
   YYACCEPT;
}
    break;
case 2:
#line 468 "rpsl.y"
{
   YYACCEPT;
}
    break;
case 3:
#line 471 "rpsl.y"
{
   handle_object_error("Error: syntax error\n");
   YYABORT;
}
    break;
case 4:
#line 475 "rpsl.y"
{ // end of file
   YYACCEPT;
}
    break;
case 5:
#line 478 "rpsl.y"
{ // end of file
   YYACCEPT;
}
    break;
case 6:
#line 481 "rpsl.y"
{ // end of file
   handle_object_error("Error: syntax error\n");
   YYABORT;
}
    break;
case 7:
#line 485 "rpsl.y"
{ // end of file
   YYABORT;
}
    break;
case 8:
#line 490 "rpsl.y"
{
   (*current_object) += yyvsp[0].attr;
}
    break;
case 9:
#line 493 "rpsl.y"
{
   (*current_object) += yyvsp[0].attr;
}
    break;
case 46:
#line 534 "rpsl.y"
{ // the current line started w/ non-attribute
   yyval.attr = changeCurrentAttr(new Attr);
   handle_error("Error: syntax error\n");
}
    break;
case 47:
#line 538 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new Attr);
   handle_error("Error: syntax error\n");
   yyerrok;
}
    break;
case 48:
#line 547 "rpsl.y"
{
   free(yyvsp[-2].string);
   yyval.attr = yyvsp[-3].attr;
}
    break;
case 49:
#line 551 "rpsl.y"
{
   handle_error("Error: \"changed: <email> <YYYYMMDD>\" expected\n");
   yyerrok;
}
    break;
case 50:
#line 557 "rpsl.y"
{
   char *start = strchr(yyvsp[-1].attr->object->contents + yyvsp[-1].attr->offset, ':') + 1;
   int len = yyvsp[-1].attr->object->contents + yyvsp[-1].attr->offset + yyvsp[-1].attr->len - start;
   ItemFREETEXT *ft = new ItemFREETEXT(start, len);
   ItemList *il = new ItemList;
   il->append(ft);
   
   yyval.attr = changeCurrentAttr(new AttrGeneric(yyvsp[-1].attr->type, il));
}
    break;
case 51:
#line 568 "rpsl.y"
{
   char *start = strchr(yyvsp[-1].attr->object->contents + yyvsp[-1].attr->offset, ':') + 1;
   int len = yyvsp[-1].attr->object->contents + yyvsp[-1].attr->offset + yyvsp[-1].attr->len - start;

   while (isspace(*start)) {
      start++;
      len --;
   }

   char *comment = strrchr(start, '#');
   if (comment && comment < start+len)
      len = comment - start;

   while (isspace(*(start+len-1)))
      len --;

   Buffer b(len+1);
   b.append(start, len);
   b.append("", 1);

   regmatch_t *matches = (regmatch_t *) malloc(sizeof(regmatch_t) 
					       * (yyvsp[-1].attr->type->reIndex + 1));
   int code = regexec(yyvsp[-1].attr->type->reComp, b.contents, 
		      yyvsp[-1].attr->type->reIndex + 1, matches, 0);

   if (code) {
      handle_error("Error: argument to %s should match %s.\n",
		   yyvsp[-1].attr->type->name(), yyvsp[-1].attr->type->re);
      yyval.attr = changeCurrentAttr(new AttrGeneric(yyvsp[-1].attr->type, NULL));
   } else {
      Buffer *buf = new Buffer(matches[yyvsp[-1].attr->type->reIndex].rm_eo 
			       - matches[yyvsp[-1].attr->type->reIndex].rm_so);
      buf->append(b.contents + matches[yyvsp[-1].attr->type->reIndex].rm_so,buf->capacity);

      ItemBUFFER *item = new ItemBUFFER(buf);
      ItemList   *il = new ItemList;
      il->append(item);
   
      yyval.attr = changeCurrentAttr(new AttrGeneric(yyvsp[-1].attr->type, il));
   }
}
    break;
case 52:
#line 611 "rpsl.y"
{
   if (!yyvsp[-2].attr->type->subsyntax()->validate(yyvsp[-1].list)) {
      handle_error("Error: argument to %s should be %s.\n",
	      yyvsp[-2].attr->type->name(), yyvsp[-2].attr->type->subsyntax()->name());
      delete yyvsp[-1].list;
      yyval.attr = changeCurrentAttr(new AttrGeneric(yyvsp[-2].attr->type, NULL));
   } else 
      yyval.attr = changeCurrentAttr(new AttrGeneric(yyvsp[-2].attr->type, yyvsp[-1].list));
}
    break;
case 53:
#line 620 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: argument to %s should be %s.\n",
	   yyvsp[-2].attr->type->name(), yyvsp[-2].attr->type->subsyntax()->name());
   yyerrok;
}
    break;
case 54:
#line 628 "rpsl.y"
{
   yyval.list = new ItemList;
}
    break;
case 56:
#line 634 "rpsl.y"
{ 	
   yyval.list = new ItemList;
   yyval.list->append(yyvsp[0].item);
}
    break;
case 57:
#line 638 "rpsl.y"
{
   yyval.list = yyvsp[-2].list;
   yyval.list->append(yyvsp[0].item);
}
    break;
case 58:
#line 644 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrGeneric(yyvsp[-2].attr->type, yyvsp[-1].list));
}
    break;
case 59:
#line 647 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: argument to %s should be blob sequence.\n",
	   yyvsp[-2].attr->type->name());
   yyerrok;
}
    break;
case 60:
#line 655 "rpsl.y"
{ 	
   yyval.list = new ItemList;
   yyval.list->append(yyvsp[0].item);
}
    break;
case 61:
#line 659 "rpsl.y"
{
   yyval.list = yyvsp[-1].list;
   yyval.list->append(yyvsp[0].item);
}
    break;
case 62:
#line 665 "rpsl.y"
{
   yyval.item = yyvsp[0].item;
}
    break;
case 63:
#line 668 "rpsl.y"
{
   yyval.item = new ItemRange(yyvsp[-2].item, yyvsp[0].item);
}
    break;
case 64:
#line 673 "rpsl.y"
{
   yyval.item = new ItemASNO(yyvsp[0].i);
}
    break;
case 65:
#line 676 "rpsl.y"
{
   yyval.item = new ItemINT(yyvsp[0].i);
}
    break;
case 66:
#line 679 "rpsl.y"
{
   yyval.item = new ItemREAL(yyvsp[0].real);
}
    break;
case 67:
#line 682 "rpsl.y"
{
   yyval.item = new ItemSTRING(yyvsp[0].string);
}
    break;
case 68:
#line 685 "rpsl.y"
{
   yyval.item = new ItemTimeStamp(yyvsp[0].time);
}
    break;
case 69:
#line 688 "rpsl.y"
{
   yyval.item = new ItemIPV4(yyvsp[0].ip);
}
    break;
case 70:
#line 691 "rpsl.y"
{
   yyval.item = new ItemPRFXV4(yyvsp[0].prfx);
}
    break;
case 71:
#line 694 "rpsl.y"
{
   yyval.item = new ItemPRFXV4Range(yyvsp[0].prfxrng);
}
    break;
case 72:
#line 697 "rpsl.y"
{
  yyval.item = new ItemIPV6(yyvsp[0].ipv6);
}
    break;
case 73:
#line 700 "rpsl.y"
{
  yyval.item = new ItemPRFXV6(yyvsp[0].prfxv6);
}
    break;
case 74:
#line 703 "rpsl.y"
{
  yyval.item = new ItemPRFXV6Range(yyvsp[0].prfxv6rng);
}
    break;
case 75:
#line 706 "rpsl.y"
{
   yyval.item = new ItemConnection(yyvsp[-2].ip, yyvsp[0].i);
}
    break;
case 76:
#line 709 "rpsl.y"
{
   yyval.item = new ItemConnection(yyvsp[-2].sid, yyvsp[0].i);
}
    break;
case 77:
#line 712 "rpsl.y"
{
   yyval.item = new ItemASNAME(yyvsp[0].sid);
}
    break;
case 78:
#line 715 "rpsl.y"
{
   yyval.item = new ItemRSNAME(yyvsp[0].sid);
}
    break;
case 79:
#line 718 "rpsl.y"
{
   yyval.item = new ItemRTRSNAME(yyvsp[0].sid);
}
    break;
case 80:
#line 721 "rpsl.y"
{
   yyval.item = new ItemPRNGNAME(yyvsp[0].sid);
}
    break;
case 81:
#line 724 "rpsl.y"
{
   yyval.item = new ItemFLTRNAME(yyvsp[0].sid);
}
    break;
case 82:
#line 727 "rpsl.y"
{
   yyval.item = new ItemBOOLEAN(yyvsp[0].i);
}
    break;
case 83:
#line 730 "rpsl.y"
{
   yyval.item = new ItemWORD(yyvsp[0].string);
}
    break;
case 84:
#line 733 "rpsl.y"
{
   yyval.item = new ItemWORD(yyvsp[0].string);
}
    break;
case 85:
#line 736 "rpsl.y"
{
   yyval.item = new ItemDNS(yyvsp[0].sid);
}
    break;
case 86:
#line 739 "rpsl.y"
{
   yyval.item = new ItemEMAIL(yyvsp[0].string);
}
    break;
case 87:
#line 742 "rpsl.y"
{
   yyval.item = new ItemBLOB(yyvsp[0].string);
}
    break;
case 88:
#line 745 "rpsl.y"
{
   yyval.item = yyvsp[-1].list;
}
    break;
case 89:
#line 748 "rpsl.y"
{
   yyval.item = new ItemFilter(yyvsp[-1].filter);
}
    break;
case 90:
#line 753 "rpsl.y"
{
   yyval.string = yyvsp[0].string;
}
    break;
case 91:
#line 756 "rpsl.y"
{
   char buffer[64];
   sprintf(buffer, "AS%d", yyvsp[0].i);
   yyval.string = strdup(buffer);
}
    break;
case 92:
#line 761 "rpsl.y"
{
   yyval.string = strdup(yyvsp[0].sid);
}
    break;
case 93:
#line 764 "rpsl.y"
{
   yyval.string = strdup(yyvsp[0].sid);
}
    break;
case 94:
#line 767 "rpsl.y"
{
   yyval.string = strdup(yyvsp[0].sid);
}
    break;
case 95:
#line 770 "rpsl.y"
{
   yyval.string = strdup(yyvsp[0].sid);
}
    break;
case 96:
#line 773 "rpsl.y"
{
   yyval.string = strdup(yyvsp[0].sid);
}
    break;
case 97:
#line 776 "rpsl.y"
{
   if (yyvsp[0].i)
      yyval.string = strdup("true");
   else
      yyval.string = strdup("false");
}
    break;
case 99:
#line 785 "rpsl.y"
{
   yyval.string = strdup("true");
}
    break;
case 100:
#line 788 "rpsl.y"
{
   yyval.string = strdup("false");
}
    break;
case 101:
#line 791 "rpsl.y"
{
   yyval.string = strdup("action");
}
    break;
case 102:
#line 794 "rpsl.y"
{
   yyval.string = strdup("accept");
}
    break;
case 103:
#line 797 "rpsl.y"
{
   yyval.string = strdup("announce");
}
    break;
case 104:
#line 800 "rpsl.y"
{
   yyval.string = strdup("from");
}
    break;
case 105:
#line 803 "rpsl.y"
{
   yyval.string = strdup("to");
}
    break;
case 106:
#line 806 "rpsl.y"
{
   yyval.string = strdup("at");
}
    break;
case 107:
#line 809 "rpsl.y"
{
   yyval.string = strdup("any");
}
    break;
case 108:
#line 812 "rpsl.y"
{
   yyval.string = strdup("refine");
}
    break;
case 109:
#line 815 "rpsl.y"
{
   yyval.string = strdup("except");
}
    break;
case 110:
#line 818 "rpsl.y"
{
   yyval.string = strdup("static");
}
    break;
case 111:
#line 821 "rpsl.y"
{
   yyval.string = strdup("networks");
}
    break;
case 112:
#line 824 "rpsl.y"
{
   yyval.string = strdup("masklen");
}
    break;
case 113:
#line 827 "rpsl.y"
{
   yyval.string = strdup("union");
}
    break;
case 114:
#line 830 "rpsl.y"
{
   yyval.string = strdup("range");
}
    break;
case 115:
#line 833 "rpsl.y"
{
   yyval.string = strdup("list");
}
    break;
case 116:
#line 836 "rpsl.y"
{
   yyval.string = strdup("of");
}
    break;
case 117:
#line 839 "rpsl.y"
{
   yyval.string = strdup("operator");
}
    break;
case 118:
#line 842 "rpsl.y"
{
   yyval.string = strdup("syntax");
}
    break;
case 119:
#line 845 "rpsl.y"
{
   yyval.string = strdup("special");
}
    break;
case 120:
#line 848 "rpsl.y"
{
   yyval.string = strdup("regexp");
}
    break;
case 121:
#line 851 "rpsl.y"
{
   yyval.string = strdup("optional");
}
    break;
case 122:
#line 854 "rpsl.y"
{
   yyval.string = strdup("mandatory");
}
    break;
case 123:
#line 857 "rpsl.y"
{
   yyval.string = strdup("internal");
}
    break;
case 124:
#line 860 "rpsl.y"
{
   yyval.string = strdup("deleted");
}
    break;
case 125:
#line 863 "rpsl.y"
{
   yyval.string = strdup("singlevalued");
}
    break;
case 126:
#line 866 "rpsl.y"
{
   yyval.string = strdup("multivalued");
}
    break;
case 127:
#line 869 "rpsl.y"
{
   yyval.string = strdup("lookup");
}
    break;
case 128:
#line 872 "rpsl.y"
{
   yyval.string = strdup("key");
}
    break;
case 129:
#line 875 "rpsl.y"
{
   yyval.string = strdup("obsolete");
}
    break;
case 130:
#line 878 "rpsl.y"
{
   yyval.string = strdup("peeras");
}
    break;
case 131:
#line 881 "rpsl.y"
{
   yyval.string = strdup("protocol");
}
    break;
case 132:
#line 884 "rpsl.y"
{
   yyval.string = strdup("into");
}
    break;
case 133:
#line 887 "rpsl.y"
{
   yyval.string = strdup("atomic");
}
    break;
case 134:
#line 890 "rpsl.y"
{
   yyval.string = strdup("inbound");
}
    break;
case 135:
#line 893 "rpsl.y"
{
   yyval.string = strdup("outbound");
}
    break;
case 136:
#line 902 "rpsl.y"
{
   yyval.filter = new FilterASNAME(symbols.symID("AS-ANY"));
}
    break;
case 138:
#line 908 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 140:
#line 914 "rpsl.y"
{
   yyval.filter = new FilterAND(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 141:
#line 917 "rpsl.y"
{
   yyval.filter = new FilterEXCEPT(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 143:
#line 923 "rpsl.y"
{
   yyval.filter = yyvsp[-1].filter;
}
    break;
case 145:
#line 929 "rpsl.y"
{
   yyval.filter = new FilterASNO(yyvsp[0].i);
}
    break;
case 146:
#line 932 "rpsl.y"
{
   yyval.filter = new FilterASNAME(yyvsp[0].sid);
}
    break;
case 147:
#line 939 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 148:
#line 942 "rpsl.y"
{
   yyval.filter = yyvsp[0].filter;
}
    break;
case 149:
#line 947 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 150:
#line 950 "rpsl.y"
{
   yyval.filter = yyvsp[0].filter;
}
    break;
case 151:
#line 955 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 153:
#line 961 "rpsl.y"
{
   yyval.filter = new FilterAND(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 154:
#line 964 "rpsl.y"
{
   yyval.filter = new FilterEXCEPT(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 156:
#line 970 "rpsl.y"
{
   yyval.filter = yyvsp[-1].filter;
}
    break;
case 158:
#line 976 "rpsl.y"
{
   yyval.filter = new FilterRouter(yyvsp[0].ip);
}
    break;
case 159:
#line 979 "rpsl.y"
{
   yyval.filter = new FilterRouterName(yyvsp[0].sid);
}
    break;
case 160:
#line 982 "rpsl.y"
{
   yyval.filter = new FilterRTRSNAME(yyvsp[0].sid);
}
    break;
case 161:
#line 989 "rpsl.y"
{
   yyval.peering = new PolicyPeering(yyvsp[-2].filter, yyvsp[-1].filter, yyvsp[0].filter);
}
    break;
case 162:
#line 992 "rpsl.y"
{
   yyval.peering = new PolicyPeering(yyvsp[0].sid);
}
    break;
case 163:
#line 999 "rpsl.y"
{
   yyval.actionList = new PolicyActionList;
}
    break;
case 164:
#line 1002 "rpsl.y"
{
   yyval.actionList = yyvsp[0].actionList;
}
    break;
case 165:
#line 1007 "rpsl.y"
{
   yyval.actionList = new PolicyActionList;
   if (yyvsp[0].actionNode)
      yyval.actionList->append(yyvsp[0].actionNode);  
}
    break;
case 166:
#line 1012 "rpsl.y"
{
   yyval.actionList = yyvsp[-1].actionList;
   if (yyvsp[0].actionNode)
      yyval.actionList->append(yyvsp[0].actionNode);  
}
    break;
case 167:
#line 1019 "rpsl.y"
{
   const AttrMethod *mtd = searchMethod(yyvsp[-6].rp_attr, yyvsp[-4].string, yyvsp[-2].list);
   if (mtd)
      yyval.actionNode = new PolicyAction(yyvsp[-6].rp_attr, mtd, yyvsp[-2].list);
   else {
      delete yyvsp[-2].list;
      yyval.actionNode = NULL;
   }
   free(yyvsp[-4].string);
}
    break;
case 168:
#line 1029 "rpsl.y"
{
   ItemList *plist = new ItemList;
   plist->append(yyvsp[-1].item);

   const AttrMethod *mtd = searchMethod(yyvsp[-3].rp_attr, yyvsp[-2].string, plist);
   if (mtd)
      yyval.actionNode = new PolicyAction(yyvsp[-3].rp_attr, mtd, plist);
   else {
      delete plist;
      yyval.actionNode = NULL;
   }
   // Added by wlee
   free(yyvsp[-2].string);
}
    break;
case 169:
#line 1043 "rpsl.y"
{
   const AttrMethod *mtd = searchMethod(yyvsp[-4].rp_attr, "()", yyvsp[-2].list);
   if (mtd)
      yyval.actionNode = new PolicyAction(yyvsp[-4].rp_attr, mtd, yyvsp[-2].list);
   else {
      delete yyvsp[-2].list;
      yyval.actionNode = NULL;
   }
}
    break;
case 170:
#line 1052 "rpsl.y"
{
   const AttrMethod *mtd = searchMethod(yyvsp[-4].rp_attr, "[]", yyvsp[-2].list);
   if (mtd)
      yyval.actionNode = new PolicyAction(yyvsp[-4].rp_attr, mtd, yyvsp[-2].list);
   else {
      delete yyvsp[-2].list;
      yyval.actionNode = NULL;
   }
}
    break;
case 171:
#line 1061 "rpsl.y"
{
   yyval.actionNode = NULL;
}
    break;
case 172:
#line 1069 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 173:
#line 1072 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-1].filter, yyvsp[0].filter);
}
    break;
case 175:
#line 1078 "rpsl.y"
{
   yyval.filter = new FilterAND(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 177:
#line 1084 "rpsl.y"
{
   yyval.filter = new FilterNOT(yyvsp[0].filter);
}
    break;
case 178:
#line 1087 "rpsl.y"
{
   yyval.filter = yyvsp[-1].filter;
}
    break;
case 180:
#line 1093 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 181:
#line 1096 "rpsl.y"
{
   yyval.filter = new FilterASPath(yyvsp[-1].re);
}
    break;
case 182:
#line 1099 "rpsl.y"
{
   if (yyvsp[0].filter)
      yyval.filter = yyvsp[0].filter;
   else
      yyval.filter = new FilterNOT(new FilterANY);
}
    break;
case 183:
#line 1105 "rpsl.y"
{
   yyval.filter = new FilterFLTRNAME(yyvsp[0].sid);
}
    break;
case 185:
#line 1111 "rpsl.y"
{
   yyvsp[0].moreSpecOp->f1 = yyvsp[-1].filter;
   yyval.filter = yyvsp[0].moreSpecOp;
}
    break;
case 187:
#line 1118 "rpsl.y"
{
   yyval.filter = new FilterASNO(yyvsp[0].i);
}
    break;
case 188:
#line 1121 "rpsl.y"
{
   yyval.filter = new FilterPeerAS;
}
    break;
case 189:
#line 1124 "rpsl.y"
{
   yyval.filter = new FilterASNAME(yyvsp[0].sid);
}
    break;
case 190:
#line 1127 "rpsl.y"
{
   yyval.filter = new FilterRSNAME(yyvsp[0].sid);
}
    break;
case 191:
#line 1130 "rpsl.y"
{ 
   yyval.filter = yyvsp[-1].filter; 
}
    break;
case 192:
#line 1135 "rpsl.y"
{
   yyval.filter = new FilterPRFXList;
}
    break;
case 193:
#line 1138 "rpsl.y"
{
  yyval.filter = yyvsp[0].filter;
}
    break;
case 194:
#line 1143 "rpsl.y"
{
   ((FilterPRFXList *) (yyval.filter = new FilterPRFXList))->add_high(*yyvsp[0].prfxrng);
   delete yyvsp[0].prfxrng;
}
    break;
case 195:
#line 1147 "rpsl.y"
{
   yyval.filter = yyvsp[-2].filter;
   ((FilterPRFXList *) (yyval.filter))->add_high(*yyvsp[0].prfxrng);
   delete yyvsp[0].prfxrng;
}
    break;
case 196:
#line 1154 "rpsl.y"
{
   yyval.prfxrng = yyvsp[0].prfx;
}
    break;
case 197:
#line 1157 "rpsl.y"
{
   yyval.prfxrng = yyvsp[0].prfxrng;
}
    break;
case 198:
#line 1162 "rpsl.y"
{
   yyval.re = new regexp_or(yyvsp[-2].re, yyvsp[0].re);
}
    break;
case 200:
#line 1168 "rpsl.y"
{
   yyval.re = new regexp_cat(yyvsp[-1].re, yyvsp[0].re);
}
    break;
case 202:
#line 1174 "rpsl.y"
{
   yyval.re = new regexp_star(yyvsp[-1].re);
}
    break;
case 203:
#line 1177 "rpsl.y"
{
   yyval.re = new regexp_question(yyvsp[-1].re);
}
    break;
case 204:
#line 1180 "rpsl.y"
{
   yyval.re = new regexp_plus(yyvsp[-1].re);
}
    break;
case 205:
#line 1183 "rpsl.y"
{
   yyval.re = new regexp_tildaplus(yyvsp[-2].re);
}
    break;
case 206:
#line 1186 "rpsl.y"
{
   yyval.re = new regexp_tildastar(yyvsp[-2].re);
}
    break;
case 208:
#line 1192 "rpsl.y"
{
   yyval.re = new regexp_bol;
}
    break;
case 209:
#line 1195 "rpsl.y"
{
   yyval.re = new regexp_eol;
}
    break;
case 210:
#line 1198 "rpsl.y"
{
   yyval.re = yyvsp[-1].re;
}
    break;
case 212:
#line 1204 "rpsl.y"
{
   yyval.re = new regexp_symbol(yyvsp[0].i);
}
    break;
case 213:
#line 1207 "rpsl.y"
{
   yyval.re = new regexp_symbol(symbols.symID("PEERAS"));
}
    break;
case 214:
#line 1210 "rpsl.y"
{
   yyval.re = new regexp_symbol(yyvsp[0].sid);
}
    break;
case 215:
#line 1213 "rpsl.y"
{
   yyval.re = new regexp_symbol(regexp_symbol::MIN_AS, regexp_symbol::MAX_AS);
}
    break;
case 216:
#line 1216 "rpsl.y"
{
   yyval.re = yyvsp[-1].re;
}
    break;
case 217:
#line 1219 "rpsl.y"
{
   yyval.re = yyvsp[-1].re;
   ((regexp_symbol *) yyval.re)->complement();
}
    break;
case 218:
#line 1225 "rpsl.y"
{
   yyval.re = new regexp_symbol;
}
    break;
case 219:
#line 1228 "rpsl.y"
{
   ((regexp_symbol *) (yyval.re = yyvsp[-1].re))->add(yyvsp[0].i);
}
    break;
case 220:
#line 1231 "rpsl.y"
{
   ((regexp_symbol *) (yyval.re = yyvsp[-1].re))->add(symbols.symID("PEERAS"));
}
    break;
case 221:
#line 1234 "rpsl.y"
{
   ((regexp_symbol *) (yyval.re = yyvsp[-1].re))->add(regexp_symbol::MIN_AS, regexp_symbol::MAX_AS);
}
    break;
case 222:
#line 1237 "rpsl.y"
{
   ((regexp_symbol *) (yyval.re = yyvsp[-3].re))->add(yyvsp[-2].i, yyvsp[0].i);
}
    break;
case 223:
#line 1240 "rpsl.y"
{
   ((regexp_symbol *) (yyval.re = yyvsp[-1].re))->add(yyvsp[0].sid);
}
    break;
case 224:
#line 1245 "rpsl.y"
{
   const AttrMethod *mtd = searchMethod(yyvsp[-5].rp_attr, yyvsp[-3].string, yyvsp[-1].list);
   if (mtd)
      yyval.filter = new FilterRPAttribute(yyvsp[-5].rp_attr, mtd, yyvsp[-1].list);
   else {
      delete yyvsp[-1].list;
      yyval.filter = NULL;
   }
   free(yyvsp[-3].string);
}
    break;
case 225:
#line 1255 "rpsl.y"
{
   ItemList *plist = new ItemList;
   plist->append(yyvsp[0].item);

   const AttrMethod *mtd = searchMethod(yyvsp[-2].rp_attr, yyvsp[-1].string, plist);
   if (mtd)
      yyval.filter = new FilterRPAttribute(yyvsp[-2].rp_attr, mtd, plist);
   else {
      delete plist;
      yyval.filter = NULL;
   }
   // Added by wlee
   free(yyvsp[-1].string);
}
    break;
case 226:
#line 1269 "rpsl.y"
{
   const AttrMethod *mtd = searchMethod(yyvsp[-3].rp_attr, "()", yyvsp[-1].list);
   if (mtd)
      yyval.filter = new FilterRPAttribute(yyvsp[-3].rp_attr, mtd, yyvsp[-1].list);
   else {
      delete yyvsp[-1].list;
      yyval.filter = NULL;
   }
}
    break;
case 227:
#line 1278 "rpsl.y"
{
   const AttrMethod *mtd = searchMethod(yyvsp[-3].rp_attr, "[]", yyvsp[-1].list);
   if (mtd)
      yyval.filter = new FilterRPAttribute(yyvsp[-3].rp_attr, mtd, yyvsp[-1].list);
   else {
      delete yyvsp[-1].list;
      yyval.filter = NULL;
   }
}
    break;
case 228:
#line 1291 "rpsl.y"
{
   yyval.peeringActionList = new List<PolicyPeeringAction>;
   yyval.peeringActionList->append(new PolicyPeeringAction(yyvsp[-1].peering, yyvsp[0].actionList));
}
    break;
case 229:
#line 1295 "rpsl.y"
{
   yyval.peeringActionList = yyvsp[-3].peeringActionList;
   yyval.peeringActionList->append(new PolicyPeeringAction(yyvsp[-1].peering, yyvsp[0].actionList));
}
    break;
case 230:
#line 1301 "rpsl.y"
{
   yyval.peeringActionList = new List<PolicyPeeringAction>;
   yyval.peeringActionList->append(new PolicyPeeringAction(yyvsp[-1].peering, yyvsp[0].actionList));
}
    break;
case 231:
#line 1305 "rpsl.y"
{
   yyval.peeringActionList = yyvsp[-3].peeringActionList;
   yyval.peeringActionList->append(new PolicyPeeringAction(yyvsp[-1].peering, yyvsp[0].actionList));
}
    break;
case 232:
#line 1313 "rpsl.y"
{
   ItemList *afi_list = new ItemList;
   afi_list->append(new ItemAFI("ipv4.unicast"));
   yyval.policyFactor = new PolicyFactor(yyvsp[-2].peeringActionList, new FilterAFI (afi_list, yyvsp[0].filter));
}
    break;
case 233:
#line 1320 "rpsl.y"
{
   yyval.policyTerm = new PolicyTerm;
   yyval.policyTerm->append(yyvsp[-1].policyFactor);
}
    break;
case 234:
#line 1324 "rpsl.y"
{
   yyval.policyTerm = yyvsp[-2].policyTerm;
   yyval.policyTerm->append(yyvsp[-1].policyFactor);
}
    break;
case 235:
#line 1330 "rpsl.y"
{
   ItemList *afi_list = new ItemList;
   afi_list->append(new ItemAFI("ipv4.unicast"));
   yyval.policyFactor = new PolicyFactor(yyvsp[-2].peeringActionList, new FilterAFI (afi_list, yyvsp[0].filter));
}
    break;
case 236:
#line 1337 "rpsl.y"
{
   yyval.policyTerm = new PolicyTerm;
   yyval.policyTerm->append(yyvsp[-1].policyFactor);
}
    break;
case 237:
#line 1341 "rpsl.y"
{
   yyval.policyTerm = yyvsp[-2].policyTerm;
   yyval.policyTerm->append(yyvsp[-1].policyFactor);
}
    break;
case 238:
#line 1349 "rpsl.y"
{
   PolicyTerm *term = new PolicyTerm;
   term->append(yyvsp[-1].policyFactor);
   yyval.policyTerm = term;
}
    break;
case 239:
#line 1354 "rpsl.y"
{
   yyval.policyTerm = yyvsp[-1].policyTerm;
}
    break;
case 240:
#line 1359 "rpsl.y"
{
   PolicyTerm *term = new PolicyTerm;
   term->append(yyvsp[-1].policyFactor);
   yyval.policyTerm = term;
}
    break;
case 241:
#line 1364 "rpsl.y"
{
   yyval.policyTerm = yyvsp[-1].policyTerm;
}
    break;
case 242:
#line 1371 "rpsl.y"
{
   yyval.policyExpr = yyvsp[0].policyTerm;
}
    break;
case 243:
#line 1374 "rpsl.y"
{
   yyval.policyExpr = new PolicyRefine(yyvsp[-2].policyTerm, yyvsp[0].policyExpr);
}
    break;
case 244:
#line 1377 "rpsl.y"
{
   yyval.policyExpr = new PolicyExcept(yyvsp[-2].policyTerm, yyvsp[0].policyExpr);
}
    break;
case 245:
#line 1382 "rpsl.y"
{
   yyval.policyExpr = yyvsp[0].policyTerm;
}
    break;
case 246:
#line 1385 "rpsl.y"
{
   yyval.policyExpr = new PolicyRefine(yyvsp[-2].policyTerm, yyvsp[0].policyExpr);
}
    break;
case 247:
#line 1388 "rpsl.y"
{
   yyval.policyExpr = new PolicyExcept(yyvsp[-2].policyTerm, yyvsp[0].policyExpr);
}
    break;
case 248:
#line 1395 "rpsl.y"
{
   yyval.protocol = schema.searchProtocol("BGP4");
}
    break;
case 249:
#line 1398 "rpsl.y"
{
   yyval.protocol = schema.searchProtocol(yyvsp[0].string);
   if (!yyval.protocol) {
      handle_warning("Warning: unknown protocol %s, BGP4 assumed.\n", yyvsp[0].string);
      yyval.protocol = schema.searchProtocol("BGP4");
   }
   free(yyvsp[0].string);
}
    break;
case 250:
#line 1408 "rpsl.y"
{
   yyval.protocol = schema.searchProtocol("BGP4");
}
    break;
case 251:
#line 1411 "rpsl.y"
{
   yyval.protocol = schema.searchProtocol(yyvsp[0].string);
   if (!yyval.protocol) {
      handle_warning("Warning: unknown protocol %s, BGP4 assumed.\n", yyvsp[0].string);
      yyval.protocol = schema.searchProtocol("BGP4");
   }
   free(yyvsp[0].string);;
}
    break;
case 252:
#line 1424 "rpsl.y"
{
  yyval.attr = changeCurrentAttr(new AttrImport(yyvsp[-3].protocol, yyvsp[-2].protocol, yyvsp[-1].policyExpr));
}
    break;
case 253:
#line 1427 "rpsl.y"
{
   PolicyTerm *term = new PolicyTerm;
   yyvsp[-1].policyFactor->filter = new FilterAFI(yyvsp[-2].list, yyvsp[-1].policyFactor->filter);
   term->append(yyvsp[-1].policyFactor);
   yyval.attr = changeCurrentAttr(new AttrImport(yyvsp[-4].protocol, yyvsp[-3].protocol, term));
}
    break;
case 254:
#line 1433 "rpsl.y"
{
   yyval.attr = yyvsp[-6].attr;
   handle_error ("Error: in peering/filter specification\n");
   yyerrok;
}
    break;
case 255:
#line 1438 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: wrong mp-import.\n");
   yyerrok;
}
    break;
case 256:
#line 1447 "rpsl.y"
{
  yyval.attr = changeCurrentAttr(new AttrExport(yyvsp[-3].protocol, yyvsp[-2].protocol, yyvsp[-1].policyExpr));
}
    break;
case 257:
#line 1450 "rpsl.y"
{
   PolicyTerm *term = new PolicyTerm;
   yyvsp[-1].policyFactor->filter = new FilterAFI(yyvsp[-2].list, yyvsp[-1].policyFactor->filter);
   term->append(yyvsp[-1].policyFactor);
   yyval.attr = changeCurrentAttr(new AttrExport(yyvsp[-4].protocol, yyvsp[-3].protocol, term));
}
    break;
case 258:
#line 1456 "rpsl.y"
{
   yyval.attr = yyvsp[-6].attr;
   handle_error ("Error: in peering/filter specification\n");
   yyerrok;
}
    break;
case 259:
#line 1461 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: wrong mp-export.\n");
   yyerrok;
}
    break;
case 260:
#line 1469 "rpsl.y"
{
   yyval.policyExpr = yyvsp[0].policyTerm;
}
    break;
case 261:
#line 1472 "rpsl.y"
{
   yyval.policyExpr = new PolicyRefine(yyvsp[-2].policyTerm, yyvsp[0].policyExpr);
}
    break;
case 262:
#line 1475 "rpsl.y"
{
   yyval.policyExpr = new PolicyExcept(yyvsp[-2].policyTerm, yyvsp[0].policyExpr);
}
    break;
case 263:
#line 1480 "rpsl.y"
{
   yyval.policyExpr = yyvsp[0].policyTerm;
}
    break;
case 264:
#line 1483 "rpsl.y"
{
   yyval.policyExpr = new PolicyRefine(yyvsp[-2].policyTerm, yyvsp[0].policyExpr);
}
    break;
case 265:
#line 1486 "rpsl.y"
{
   yyval.policyExpr = new PolicyExcept(yyvsp[-2].policyTerm, yyvsp[0].policyExpr);
}
    break;
case 266:
#line 1493 "rpsl.y"
{
   PolicyTerm *term = new PolicyTerm;
   yyvsp[-1].policyFactor->filter = new FilterAFI(yyvsp[-2].list, yyvsp[-1].policyFactor->filter);
   term->append(yyvsp[-1].policyFactor);
   yyval.policyTerm = term;
}
    break;
case 267:
#line 1499 "rpsl.y"
{
   for (PolicyFactor *pf = yyvsp[-1].policyTerm->head(); pf; pf = yyvsp[-1].policyTerm->next(pf)) {
     pf->filter = new FilterAFI(yyvsp[-3].list, pf->filter);
   }
   yyval.policyTerm = yyvsp[-1].policyTerm;
}
    break;
case 268:
#line 1507 "rpsl.y"
{
   PolicyTerm *term = new PolicyTerm;
   yyvsp[-1].policyFactor->filter = new FilterAFI(yyvsp[-2].list, yyvsp[-1].policyFactor->filter);
   term->append(yyvsp[-1].policyFactor);
   yyval.policyTerm = term;
}
    break;
case 269:
#line 1513 "rpsl.y"
{
   for (PolicyFactor *pf = yyvsp[-1].policyTerm->head(); pf; pf = yyvsp[-1].policyTerm->next(pf)) {
     pf->filter = new FilterAFI(yyvsp[-3].list, pf->filter);
   }
   yyval.policyTerm = yyvsp[-1].policyTerm;
}
    break;
case 270:
#line 1523 "rpsl.y"
{
   yyval.policyFactor = new PolicyFactor(yyvsp[-2].peeringActionList, yyvsp[0].filter);
}
    break;
case 271:
#line 1528 "rpsl.y"
{
   yyval.policyTerm = new PolicyTerm;
   yyval.policyTerm->append(yyvsp[-1].policyFactor);
}
    break;
case 272:
#line 1532 "rpsl.y"
{
   yyval.policyTerm = yyvsp[-2].policyTerm;
   yyval.policyTerm->append(yyvsp[-1].policyFactor);
}
    break;
case 273:
#line 1538 "rpsl.y"
{
   yyval.policyFactor = new PolicyFactor(yyvsp[-2].peeringActionList, yyvsp[0].filter);
}
    break;
case 274:
#line 1543 "rpsl.y"
{
   yyval.policyTerm = new PolicyTerm;
   yyval.policyTerm->append(yyvsp[-1].policyFactor);
}
    break;
case 275:
#line 1547 "rpsl.y"
{
   yyval.policyTerm = yyvsp[-2].policyTerm;
   yyval.policyTerm->append(yyvsp[-1].policyFactor);
}
    break;
case 276:
#line 1555 "rpsl.y"
{
   yyval.peeringActionList = new List<PolicyPeeringAction>;
   yyval.peeringActionList->append(new PolicyPeeringAction(yyvsp[-1].peering, yyvsp[0].actionList));
}
    break;
case 277:
#line 1559 "rpsl.y"
{
   yyval.peeringActionList = yyvsp[-3].peeringActionList;
   yyval.peeringActionList->append(new PolicyPeeringAction(yyvsp[-1].peering, yyvsp[0].actionList));
}
    break;
case 278:
#line 1565 "rpsl.y"
{
   yyval.peeringActionList = new List<PolicyPeeringAction>;
   yyval.peeringActionList->append(new PolicyPeeringAction(yyvsp[-1].peering, yyvsp[0].actionList));
}
    break;
case 279:
#line 1569 "rpsl.y"
{
   yyval.peeringActionList = yyvsp[-3].peeringActionList;
   yyval.peeringActionList->append(new PolicyPeeringAction(yyvsp[-1].peering, yyvsp[0].actionList));
}
    break;
case 280:
#line 1577 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 281:
#line 1580 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-1].filter, yyvsp[0].filter);
}
    break;
case 283:
#line 1586 "rpsl.y"
{
   yyval.filter = new FilterAND(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 285:
#line 1592 "rpsl.y"
{
   yyval.filter = new FilterNOT(yyvsp[0].filter);
}
    break;
case 286:
#line 1595 "rpsl.y"
{
   yyval.filter = yyvsp[-1].filter;
}
    break;
case 288:
#line 1601 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 289:
#line 1604 "rpsl.y"
{
   yyval.filter = new FilterASPath(yyvsp[-1].re);
}
    break;
case 290:
#line 1607 "rpsl.y"
{
   if (yyvsp[0].filter)
      yyval.filter = yyvsp[0].filter;
   else
      yyval.filter = new FilterNOT(new FilterANY);
}
    break;
case 291:
#line 1613 "rpsl.y"
{
   yyval.filter = new FilterFLTRNAME(yyvsp[0].sid);
}
    break;
case 293:
#line 1619 "rpsl.y"
{
   yyvsp[0].moreSpecOp->f1 = yyvsp[-1].filter;
   yyval.filter = yyvsp[0].moreSpecOp;
}
    break;
case 295:
#line 1626 "rpsl.y"
{
   yyval.filter = new FilterASNO(yyvsp[0].i);
}
    break;
case 296:
#line 1629 "rpsl.y"
{
   yyval.filter = new FilterPeerAS;
}
    break;
case 297:
#line 1632 "rpsl.y"
{
   yyval.filter = new FilterASNAME(yyvsp[0].sid);
}
    break;
case 298:
#line 1635 "rpsl.y"
{
   yyval.filter = new FilterRSNAME(yyvsp[0].sid);
}
    break;
case 299:
#line 1638 "rpsl.y"
{ 
   yyval.filter = yyvsp[-1].filter; 
}
    break;
case 300:
#line 1643 "rpsl.y"
{
   yyval.filter = new FilterMPPRFXList;
}
    break;
case 302:
#line 1649 "rpsl.y"
{

   ((FilterMPPRFXList *) (yyval.filter = new FilterMPPRFXList))->push_back(*yyvsp[0].mpprefix);
}
    break;
case 303:
#line 1653 "rpsl.y"
{
   yyval.filter = yyvsp[-2].filter;
   ((FilterMPPRFXList *) (yyval.filter))->push_back(*yyvsp[0].mpprefix);
}
    break;
case 304:
#line 1659 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix(yyvsp[0].prfxv6);
}
    break;
case 305:
#line 1662 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix(yyvsp[0].prfxv6rng);
}
    break;
case 306:
#line 1665 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix(yyvsp[0].prfx);
}
    break;
case 307:
#line 1668 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix(yyvsp[0].prfxrng);
}
    break;
case 308:
#line 1675 "rpsl.y"
{
   yyval.peering = new PolicyPeering(yyvsp[-2].filter, yyvsp[-1].filter, yyvsp[0].filter);
// need to check here that afi in opt_mp_router_expr == afi in opt_mp_router_expr_with_at ?

}
    break;
case 309:
#line 1680 "rpsl.y"
{
   yyval.peering = new PolicyPeering(yyvsp[0].sid);
}
    break;
case 310:
#line 1687 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 311:
#line 1690 "rpsl.y"
{
   yyval.filter = yyvsp[0].filter;
}
    break;
case 312:
#line 1695 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 313:
#line 1698 "rpsl.y"
{
   yyval.filter = yyvsp[0].filter;
}
    break;
case 314:
#line 1703 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 316:
#line 1709 "rpsl.y"
{
   yyval.filter = new FilterAND(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 317:
#line 1712 "rpsl.y"
{
   yyval.filter = new FilterEXCEPT(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 319:
#line 1718 "rpsl.y"
{
   yyval.filter = yyvsp[-1].filter;
}
    break;
case 321:
#line 1724 "rpsl.y"
{
   yyval.filter = new FilterRouter(yyvsp[0].ip);
}
    break;
case 322:
#line 1727 "rpsl.y"
{
   yyval.filter = new FilterRouter(yyvsp[0].ipv6);
}
    break;
case 323:
#line 1730 "rpsl.y"
{
   yyval.filter = new FilterRouterName(yyvsp[0].sid);
}
    break;
case 324:
#line 1733 "rpsl.y"
{
   yyval.filter = new FilterRTRSNAME(yyvsp[0].sid);
}
    break;
case 325:
#line 1742 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrImport(yyvsp[-3].protocol, yyvsp[-2].protocol, yyvsp[-1].policyExpr));
}
    break;
case 326:
#line 1745 "rpsl.y"
{
   PolicyTerm *term = new PolicyTerm;
   term->append(yyvsp[-1].policyFactor);

   yyval.attr = changeCurrentAttr(new AttrImport(yyvsp[-3].protocol, yyvsp[-2].protocol, term));
}
    break;
case 327:
#line 1751 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: from <peering> expected.\n");
   yyerrok;
}
    break;
case 328:
#line 1760 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrExport(yyvsp[-3].protocol, yyvsp[-2].protocol, yyvsp[-1].policyExpr));
}
    break;
case 329:
#line 1763 "rpsl.y"
{
   PolicyTerm *term = new PolicyTerm;
   term->append(yyvsp[-1].policyFactor);

   yyval.attr = changeCurrentAttr(new AttrExport(yyvsp[-3].protocol, yyvsp[-2].protocol, term));
}
    break;
case 330:
#line 1769 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: to <peering> expected.\n");
   yyerrok;
}
    break;
case 331:
#line 1776 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 332:
#line 1779 "rpsl.y"
{
   yyval.filter = yyvsp[0].filter;
}
    break;
case 333:
#line 1788 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrDefault(yyvsp[-3].peering, yyvsp[-2].actionList, yyvsp[-1].filter));
}
    break;
case 334:
#line 1791 "rpsl.y"
{
   if (yyvsp[-2].peering)
      delete yyvsp[-2].peering;
   handle_error("Error: badly formed filter/action or keyword NETWORKS/ACTION missing.\n");
   yyerrok;
}
    break;
case 335:
#line 1797 "rpsl.y"
{
   handle_error("Error: TO <peer> missing.\n");
   yyerrok;
}
    break;
case 336:
#line 1808 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrDefault(yyvsp[-5].list, yyvsp[-3].peering, yyvsp[-2].actionList, yyvsp[-1].filter));
}
    break;
case 337:
#line 1811 "rpsl.y"
{
   if (yyvsp[-2].peering)
      delete yyvsp[-2].peering;
   handle_error("Error: badly formed filter/action or keyword NETWORKS/ACTION missing.\n");
   yyerrok;
}
    break;
case 338:
#line 1817 "rpsl.y"
{
   handle_error("Error: TO <peer> missing. in mp-default\n");
   yyerrok;
}
    break;
case 339:
#line 1823 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 340:
#line 1826 "rpsl.y"
{
   yyval.filter = yyvsp[0].filter;
}
    break;
case 341:
#line 1833 "rpsl.y"
{
  // check that "mp-filter:" is not present
   if (current_object->hasAttr("mp-filter")) {
     handle_error("Error: mp-filter and filter attributes can't be used together\n");
     yyerrok;
    } else {
      //$$ = changeCurrentAttr(new AttrFilter($2));
      yyval.attr = changeCurrentAttr(new AttrFilter(new FilterAFI(new ItemAFI(new AddressFamily("ipv4.unicast")), yyvsp[-1].filter)));
    }
}
    break;
case 342:
#line 1843 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: badly formed filter.\n");
   yyerrok;
}
    break;
case 343:
#line 1852 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrMPPeval(yyvsp[-1].filter));
}
    break;
case 344:
#line 1855 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: badly formed filter in mp-peval.\n");
   yyerrok;
}
    break;
case 345:
#line 1862 "rpsl.y"
{
   yyval.filter = new FilterAFI((ItemList *) yyvsp[-1].list, yyvsp[0].filter);
}
    break;
case 346:
#line 1869 "rpsl.y"
{
  // check that "filter:" is not present
   if (current_object->hasAttr("filter")) {
    handle_error("Error: mp-filter and filter attributes can't be used together\n");
    yyerrok;
  } else {
    yyval.attr = changeCurrentAttr(new AttrFilter(yyvsp[-1].filter));
  }
}
    break;
case 347:
#line 1878 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: badly formed filter in mp-filter.\n");
   yyerrok;
}
    break;
case 348:
#line 1887 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrPeering(yyvsp[-1].peering));
}
    break;
case 349:
#line 1890 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: badly formed filter.\n");
   yyerrok;
}
    break;
case 350:
#line 1899 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrPeering(yyvsp[-1].peering));
}
    break;
case 351:
#line 1902 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: badly formed peering.\n");
   yyerrok;
}
    break;
case 352:
#line 1913 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrIfAddr(new MPPrefix((PrefixRange *) yyvsp[-4].ip), yyvsp[-2].i, yyvsp[-1].actionList, NULL));
}
    break;
case 353:
#line 1916 "rpsl.y"
{
   delete yyvsp[-4].ip;
   yyval.attr = yyvsp[-5].attr;
   handle_error("Error: in action specification.\n");
   yyerrok;
}
    break;
case 354:
#line 1922 "rpsl.y"
{
   delete yyvsp[-3].ip;
   yyval.attr = yyvsp[-4].attr;
   handle_error("Error: integer mask length expected.\n");
   yyerrok;
}
    break;
case 355:
#line 1928 "rpsl.y"
{
   delete yyvsp[-2].ip;
   yyval.attr = yyvsp[-3].attr;
   handle_error("Error: MASKLEN <length> expected.\n");
   yyerrok;
}
    break;
case 356:
#line 1934 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: <ip_address> MASKLEN <length> [<action>] expected.\n");
   yyerrok;
}
    break;
case 357:
#line 1943 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix(yyvsp[0].ip);
}
    break;
case 358:
#line 1946 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix(yyvsp[0].ipv6);
}
    break;
case 359:
#line 1951 "rpsl.y"
{
  yyval.tunnel = NULL;
}
    break;
case 360:
#line 1954 "rpsl.y"
{
  yyval.tunnel = new Tunnel(yyvsp[-2].mpprefix, new ItemWORD(yyvsp[0].string));
  if (! (schema.searchTypedef("encapsulation"))->validate(new ItemWORD(yyvsp[0].string))) {
     delete yyval.tunnel;
     handle_error("Error: wrong encapsulation specified.\n");
     yyerrok;
  }
}
    break;
case 361:
#line 1966 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrIfAddr(yyvsp[-5].mpprefix, yyvsp[-3].i, yyvsp[-2].actionList, yyvsp[-1].tunnel));
   if (yyvsp[-1].tunnel) {
      if ((yyvsp[-5].mpprefix->ipv4 && yyvsp[-1].tunnel->remote_ip->ipv6) || (yyvsp[-5].mpprefix->ipv6 && yyvsp[-1].tunnel->remote_ip->ipv4)) {
        handle_error("Error: address family mismatch in local/remote tunnel endpoint\n");
        yyerrok;
      }
   }
}
    break;
case 362:
#line 1975 "rpsl.y"
{
   yyval.attr = yyvsp[-6].attr;
   handle_error("Error: error in tunnel specification.\n");
   yyerrok;
}
    break;
case 363:
#line 1980 "rpsl.y"
{
   yyval.attr = yyvsp[-4].attr;
   handle_error("Error: integer mask length expected.\n");
   yyerrok;
}
    break;
case 364:
#line 1985 "rpsl.y"
{
   yyval.attr = yyvsp[-3].attr;
   handle_error("Error: MASKLEN <length> expected.\n");
   yyerrok;
}
    break;
case 365:
#line 1990 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: <ip_address> MASKLEN <length> [<action>] [<tunnel>] expected.\n");
   yyerrok;
}
    break;
case 366:
#line 1999 "rpsl.y"
{
   yyval.peer_option_list = new List<AttrPeerOption>;
}
    break;
case 367:
#line 2002 "rpsl.y"
{
   yyval.peer_option_list = yyvsp[0].peer_option_list;
}
    break;
case 368:
#line 2007 "rpsl.y"
{
   yyval.peer_option_list = new List<AttrPeerOption>;
   yyval.peer_option_list->append(yyvsp[0].peer_option);
}
    break;
case 369:
#line 2011 "rpsl.y"
{
   yyval.peer_option_list = yyvsp[-2].peer_option_list;
   yyval.peer_option_list->append(yyvsp[0].peer_option);
}
    break;
case 370:
#line 2017 "rpsl.y"
{
   yyval.peer_option = new AttrPeerOption(yyvsp[-3].string, yyvsp[-1].list);
}
    break;
case 372:
#line 2023 "rpsl.y"
{
   yyval.ip = new IPAddr;
}
    break;
case 373:
#line 2026 "rpsl.y"
{
   yyval.ip = new IPAddr;
}
    break;
case 374:
#line 2029 "rpsl.y"
{
   yyval.ip = new IPAddr;
}
    break;
case 375:
#line 2034 "rpsl.y"
{
   const AttrProtocol *protocol = schema.searchProtocol(yyvsp[-3].string);
   int position;
   const RPType *correctType;
   bool error = false;

   if (!protocol) {
      handle_error("Error: unknown protocol %s.\n", yyvsp[-3].string);
      error = true;
   } else {
      ((AttrProtocol *) protocol)->startMandatoryCheck();
      for (AttrPeerOption *opt = yyvsp[-1].peer_option_list->head(); opt; opt = yyvsp[-1].peer_option_list->next(opt)) {
	 const AttrProtocolOption *decl = protocol->searchOption(opt->option);
	 if (!decl)  {
	    handle_error("Error: protocol %s does not have option %s.\n", 
			 yyvsp[-3].string, opt->option);
	    error = true;
	 } else {
	    for (; decl; decl = protocol->searchNextOption(decl))
	       if (decl->option->validateArgs(opt->args, position, correctType))
		  break;
	    if (! decl) {
	       if (! (!strcasecmp(protocol->name, "BGP4")
		      && !strcasecmp(opt->option, "asno")
		      && opt->args->isSingleton()
		      && typeid(*opt->args->head()) == typeid(ItemWORD)
		      && !strcasecmp(((ItemWORD *) opt->args->head())->word,
				     "peeras"))) {
		  handleArgumentTypeError(yyvsp[-3].string, opt->option, position, 
					  correctType);
		  error = true;
	       }
	    }
	 }
      }
   }

   if (! error) {
      const AttrProtocolOption *missing = 
	 ((AttrProtocol *) protocol)->missingMandatoryOption();
      if (missing) {
	 handle_error("Error: mandatory option %s of protocol %s is missing.\n", 
		      missing->option->name, yyvsp[-3].string);
	 error = true;
      }
   }
      
   if (!error)
      //$$ = changeCurrentAttr(new AttrPeer(protocol, $3, $4));
      yyval.attr = changeCurrentAttr(new AttrPeer(protocol, new MPPrefix((PrefixRange *) yyvsp[-2].ip), yyvsp[-1].peer_option_list));
   else {
      free(yyvsp[-3].string);
      delete yyvsp[-2].ip;
      delete yyvsp[-1].peer_option_list;
   }
}
    break;
case 376:
#line 2090 "rpsl.y"
{
   yyval.attr = yyvsp[-4].attr;
   free(yyvsp[-3].string);
   delete yyvsp[-2].ip;
   handle_error("Error: in peer option.\n");
   yyerrok;
}
    break;
case 377:
#line 2097 "rpsl.y"
{
   yyval.attr = yyvsp[-3].attr;
   free(yyvsp[-2].string);
   handle_error("Error: missing peer ip_address.\n");
   yyerrok;
}
    break;
case 378:
#line 2103 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: missing protocol name.\n");
   yyerrok;
}
    break;
case 379:
#line 2112 "rpsl.y"
{ 
   const AttrProtocol *protocol = schema.searchProtocol(yyvsp[-3].string);
   int position;
   const RPType *correctType;
   bool error = false;

   if (!protocol) {
      handle_error("Error: unknown protocol %s.\n", yyvsp[-3].string);
      error = true;
   } else {
      ((AttrProtocol *) protocol)->startMandatoryCheck();
      for (AttrPeerOption *opt = yyvsp[-1].peer_option_list->head(); opt; opt = yyvsp[-1].peer_option_list->next(opt)) {
         const AttrProtocolOption *decl = protocol->searchOption(opt->option);
         if (!decl)  {
            handle_error("Error: protocol %s does not have option %s.\n", 
                         yyvsp[-3].string, opt->option);
            error = true;
         } else {
            for (; decl; decl = protocol->searchNextOption(decl))
               if (decl->option->validateArgs(opt->args, position, correctType))
                  break;
            if (! decl) {
               if (! (!strcasecmp(protocol->name, "BGP4")
                      && !strcasecmp(opt->option, "asno")
                      && opt->args->isSingleton()
                      && typeid(*opt->args->head()) == typeid(ItemWORD)
                      && !strcasecmp(((ItemWORD *) opt->args->head())->word,
                                     "peeras"))) {
                  handleArgumentTypeError(yyvsp[-3].string, opt->option, position, 
                                          correctType);
                  error = true;
               }
            }
         }
      }
   }

   if (! error) {
      const AttrProtocolOption *missing = 
         ((AttrProtocol *) protocol)->missingMandatoryOption();
      if (missing) {
         handle_error("Error: mandatory option %s of protocol %s is missing.\n", 
                      missing->option->name, yyvsp[-3].string);
         error = true;
      }
   }
      
   if (!error) {
     yyval.attr = changeCurrentAttr(new AttrPeer(protocol, yyvsp[-2].mpprefix, yyvsp[-1].peer_option_list));
   }
   else {
      free(yyvsp[-3].string);
      delete yyvsp[-2].mpprefix;
      delete yyvsp[-1].peer_option_list;
   }
  
}
    break;
case 380:
#line 2169 "rpsl.y"
{
   yyval.attr = yyvsp[-4].attr;
   free(yyvsp[-3].string);
   delete yyvsp[-2].mpprefix;
   handle_error("Error: in peer option.\n");
   yyerrok;
}
    break;
case 381:
#line 2176 "rpsl.y"
{
   yyval.attr = yyvsp[-3].attr;
   free(yyvsp[-2].string);
   handle_error("Error: missing peer ip_address.\n");
   yyerrok;
}
    break;
case 382:
#line 2182 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: missing protocol name.\n");
   yyerrok;
}
    break;
case 383:
#line 2189 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix(yyvsp[0].ip);
}
    break;
case 384:
#line 2192 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix(yyvsp[0].ipv6);
}
    break;
case 385:
#line 2195 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix;
}
    break;
case 386:
#line 2198 "rpsl.y"
{
//   $$ = new IPAddr;
  yyval.mpprefix = new MPPrefix;
}
    break;
case 387:
#line 2202 "rpsl.y"
{
//   $$ = new IPAddr;
  yyval.mpprefix = new MPPrefix;
}
    break;
case 388:
#line 2214 "rpsl.y"
{
  yyval.attr = changeCurrentAttr(new AttrFilter(yyvsp[-1].filter));
}
    break;
case 389:
#line 2217 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: badly formed filter.\n");
   yyerrok;
}
    break;
case 390:
#line 2224 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 391:
#line 2227 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-1].filter, yyvsp[0].filter);
}
    break;
case 393:
#line 2233 "rpsl.y"
{
   yyval.filter = new FilterAND(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 395:
#line 2239 "rpsl.y"
{
   yyval.filter = new FilterNOT(yyvsp[0].filter);
}
    break;
case 396:
#line 2242 "rpsl.y"
{
   yyval.filter = yyvsp[-1].filter;
}
    break;
case 398:
#line 2248 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 399:
#line 2251 "rpsl.y"
{
   yyval.filter = new FilterASPath(yyvsp[-1].re);
}
    break;
case 400:
#line 2254 "rpsl.y"
{
   if (yyvsp[0].filter)
      yyval.filter = yyvsp[0].filter;
   else
      yyval.filter = new FilterNOT(new FilterANY);
}
    break;
case 401:
#line 2260 "rpsl.y"
{
   yyval.filter = new FilterFLTRNAME(yyvsp[0].sid);
}
    break;
case 403:
#line 2266 "rpsl.y"
{
   yyvsp[0].moreSpecOp->f1 = yyvsp[-1].filter;
   yyval.filter = yyvsp[0].moreSpecOp;
}
    break;
case 405:
#line 2273 "rpsl.y"
{
   yyval.filter = new FilterASNO(yyvsp[0].i);
}
    break;
case 406:
#line 2276 "rpsl.y"
{
   yyval.filter = new FilterPeerAS;
}
    break;
case 407:
#line 2279 "rpsl.y"
{
   yyval.filter = new FilterASNAME(yyvsp[0].sid);
}
    break;
case 408:
#line 2282 "rpsl.y"
{
   yyval.filter = new FilterRSNAME(yyvsp[0].sid);
}
    break;
case 409:
#line 2285 "rpsl.y"
{ 
   yyval.filter = yyvsp[-1].filter; 
}
    break;
case 410:
#line 2290 "rpsl.y"
{
   yyval.filter = new FilterMPPRFXList;
}
    break;
case 412:
#line 2296 "rpsl.y"
{

   ((FilterMPPRFXList *) (yyval.filter = new FilterMPPRFXList))->push_back(*yyvsp[0].mpprefix);
}
    break;
case 413:
#line 2300 "rpsl.y"
{
   yyval.filter = yyvsp[-2].filter;
   ((FilterMPPRFXList *) (yyval.filter))->push_back(*yyvsp[0].mpprefix);
}
    break;
case 414:
#line 2307 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix(yyvsp[0].prfxv6);
}
    break;
case 415:
#line 2310 "rpsl.y"
{
  yyval.mpprefix = new MPPrefix(yyvsp[0].prfxv6rng);
}
    break;
case 416:
#line 2317 "rpsl.y"
{
  yyval.attr = yyvsp[-3].attr;
}
    break;
case 417:
#line 2320 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: [ATOMIC] [[<filter>] [PROTOCOL <protocol> <filter>] ...] expected.\n");
   yyerrok;
}
    break;
case 418:
#line 2327 "rpsl.y"
{
}
    break;
case 419:
#line 2329 "rpsl.y"
{
   delete yyvsp[0].filter;
}
    break;
case 420:
#line 2332 "rpsl.y"
{
   free(yyvsp[-1].string);
   delete yyvsp[0].filter;
}
    break;
case 421:
#line 2343 "rpsl.y"
{
  yyval.attr = yyvsp[-4].attr;
  delete yyvsp[-3].filter;
  delete yyvsp[-2].actionList;
  delete yyvsp[-1].filter;
  
}
    break;
case 422:
#line 2350 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: [at <router-exp>] [action <action>] [upon <condition>] expected.\n");
   yyerrok;
}
    break;
case 423:
#line 2357 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 424:
#line 2360 "rpsl.y"
{
   yyval.filter = yyvsp[0].filter;
}
    break;
case 425:
#line 2365 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 426:
#line 2368 "rpsl.y"
{
}
    break;
case 427:
#line 2372 "rpsl.y"
{
   yyval.filter = new FilterAND(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 429:
#line 2378 "rpsl.y"
{
   yyval.filter = yyvsp[-1].filter;
}
    break;
case 431:
#line 2384 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 432:
#line 2387 "rpsl.y"
{ 
   yyval.filter = new FilterV6HAVE_COMPONENTS((FilterMPPRFXList *) yyvsp[-1].filter); 
}
    break;
case 433:
#line 2390 "rpsl.y"
{ 
   yyval.filter = new FilterV6EXCLUDE((FilterMPPRFXList *) yyvsp[-1].filter); 
}
    break;
case 434:
#line 2397 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   delete yyvsp[-1].filter;
}
    break;
case 435:
#line 2401 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: <as-expression> expected.\n");
   yyerrok;
}
    break;
case 436:
#line 2408 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
}
    break;
case 437:
#line 2411 "rpsl.y"
{
   delete yyvsp[-1].filter;
}
    break;
case 438:
#line 2414 "rpsl.y"
{
   yyval.attr = yyvsp[-3].attr;
   handle_error("Error: OUTBOUND <as-expression> expected.\n");
   yyerrok;
}
    break;
case 439:
#line 2419 "rpsl.y"
{
   yyval.attr = yyvsp[-3].attr;
   handle_error("Error: INBOUND can not be followed by anything.\n");
   yyerrok;
}
    break;
case 440:
#line 2424 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: keyword INBOUND or OUTBOUND expected.\n");
   yyerrok;
}
    break;
case 441:
#line 2433 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 442:
#line 2436 "rpsl.y"
{
   yyval.filter = yyvsp[0].filter;
}
    break;
case 443:
#line 2441 "rpsl.y"
{
   yyval.filter = new FilterOR(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 445:
#line 2447 "rpsl.y"
{
   yyval.filter = new FilterAND(yyvsp[-2].filter, yyvsp[0].filter);
}
    break;
case 447:
#line 2453 "rpsl.y"
{
   yyval.filter = yyvsp[-1].filter;
}
    break;
case 449:
#line 2459 "rpsl.y"
{
   yyval.filter = new FilterANY;
}
    break;
case 450:
#line 2462 "rpsl.y"
{ 
   yyval.filter = new FilterHAVE_COMPONENTS((FilterPRFXList *) yyvsp[-1].filter); 
}
    break;
case 451:
#line 2465 "rpsl.y"
{ 
   yyval.filter = new FilterEXCLUDE((FilterPRFXList *) yyvsp[-1].filter); 
}
    break;
case 452:
#line 2470 "rpsl.y"
{
   yyval.attr = yyvsp[-4].attr;
   delete yyvsp[-3].filter;
   delete yyvsp[-2].actionList;
   delete yyvsp[-1].filter;
}
    break;
case 453:
#line 2476 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: [at <router-exp>] [action <action>] [upon <condition>] expected.\n");
   yyerrok;
}
    break;
case 456:
#line 2489 "rpsl.y"
{
}
    break;
case 457:
#line 2491 "rpsl.y"
{
   delete yyvsp[0].filter;
}
    break;
case 458:
#line 2494 "rpsl.y"
{
   free(yyvsp[-1].string);
   delete yyvsp[0].filter;
}
    break;
case 459:
#line 2500 "rpsl.y"
{
   yyval.attr = yyvsp[-3].attr;
}
    break;
case 460:
#line 2503 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: [ATOMIC] [[<filter>] [PROTOCOL <protocol> <filter>] ...] expected.\n");
   yyerrok;
}
    break;
case 461:
#line 2512 "rpsl.y"
{
   yyval.list = new ItemList;
}
    break;
case 462:
#line 2515 "rpsl.y"
{
   yyval.list = yyvsp[0].list;
}
    break;
case 463:
#line 2520 "rpsl.y"
{ 	
   yyval.list = new ItemList;
   yyval.list->append(yyvsp[0].item);
}
    break;
case 464:
#line 2524 "rpsl.y"
{
   yyval.list = yyvsp[-2].list;
   yyval.list->append(yyvsp[0].item);
}
    break;
case 465:
#line 2530 "rpsl.y"
{
   yyval.item = new ItemASNO(yyvsp[0].i);
}
    break;
case 466:
#line 2533 "rpsl.y"
{
   yyval.item = new ItemMSItem(new ItemASNO(yyvsp[-1].i), yyvsp[0].moreSpecOp->code, yyvsp[0].moreSpecOp->n, yyvsp[0].moreSpecOp->m);
   delete yyvsp[0].moreSpecOp;
}
    break;
case 467:
#line 2537 "rpsl.y"
{
   yyval.item = new ItemASNAME(yyvsp[0].sid);
}
    break;
case 468:
#line 2540 "rpsl.y"
{
   yyval.item = new ItemMSItem(new ItemASNAME(yyvsp[-1].sid), yyvsp[0].moreSpecOp->code, yyvsp[0].moreSpecOp->n, yyvsp[0].moreSpecOp->m);
   delete yyvsp[0].moreSpecOp;
}
    break;
case 469:
#line 2544 "rpsl.y"
{
   yyval.item = new ItemRSNAME(yyvsp[0].sid);
}
    break;
case 470:
#line 2547 "rpsl.y"
{
   yyval.item = new ItemMSItem(new ItemRSNAME(yyvsp[-1].sid), yyvsp[0].moreSpecOp->code, yyvsp[0].moreSpecOp->n, yyvsp[0].moreSpecOp->m);
   delete yyvsp[0].moreSpecOp;
}
    break;
case 471:
#line 2551 "rpsl.y"
{
   yyval.item = new ItemPRFXV4(yyvsp[0].prfx);
}
    break;
case 472:
#line 2554 "rpsl.y"
{
   yyval.item = new ItemPRFXV4Range(yyvsp[0].prfxrng);
}
    break;
case 473:
#line 2559 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrGeneric(yyvsp[-2].attr->type, yyvsp[-1].list));
}
    break;
case 474:
#line 2562 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: invalid member\n");
   yyerrok;
}
    break;
case 475:
#line 2571 "rpsl.y"
{
   yyval.list = new ItemList;
}
    break;
case 476:
#line 2574 "rpsl.y"
{
   yyval.list = yyvsp[0].list;
}
    break;
case 477:
#line 2579 "rpsl.y"
{
   yyval.list = new ItemList;
   yyval.list->append(yyvsp[0].item);
}
    break;
case 478:
#line 2583 "rpsl.y"
{
   yyval.list = yyvsp[-2].list;
   yyval.list->append(yyvsp[0].item);
}
    break;
case 479:
#line 2589 "rpsl.y"
{
   yyval.item = new ItemASNO(yyvsp[0].i);
}
    break;
case 480:
#line 2592 "rpsl.y"
{
   yyval.item = new ItemMSItem(new ItemASNO(yyvsp[-1].i), yyvsp[0].moreSpecOp->code, yyvsp[0].moreSpecOp->n, yyvsp[0].moreSpecOp->m);
   delete yyvsp[0].moreSpecOp;
}
    break;
case 481:
#line 2596 "rpsl.y"
{
   yyval.item = new ItemASNAME(yyvsp[0].sid);
}
    break;
case 482:
#line 2599 "rpsl.y"
{
   yyval.item = new ItemMSItem(new ItemASNAME(yyvsp[-1].sid), yyvsp[0].moreSpecOp->code, yyvsp[0].moreSpecOp->n, yyvsp[0].moreSpecOp->m);
   delete yyvsp[0].moreSpecOp;
}
    break;
case 483:
#line 2603 "rpsl.y"
{
   yyval.item = new ItemRSNAME(yyvsp[0].sid);
}
    break;
case 484:
#line 2606 "rpsl.y"
{
   yyval.item = new ItemMSItem(new ItemRSNAME(yyvsp[-1].sid), yyvsp[0].moreSpecOp->code, yyvsp[0].moreSpecOp->n, yyvsp[0].moreSpecOp->m);
   delete yyvsp[0].moreSpecOp;
}
    break;
case 485:
#line 2610 "rpsl.y"
{
   yyval.item = new ItemPRFXV4(yyvsp[0].prfx);
}
    break;
case 486:
#line 2613 "rpsl.y"
{
   yyval.item = new ItemPRFXV4Range(yyvsp[0].prfxrng);
}
    break;
case 487:
#line 2616 "rpsl.y"
{
   yyval.item = new ItemPRFXV6(yyvsp[0].prfxv6);
}
    break;
case 488:
#line 2619 "rpsl.y"
{
   yyval.item = new ItemPRFXV6Range(yyvsp[0].prfxv6rng);
}
    break;
case 489:
#line 2624 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrGeneric(yyvsp[-2].attr->type, yyvsp[-1].list));
}
    break;
case 490:
#line 2627 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: invalid member\n");
   yyerrok;
}
    break;
case 491:
#line 2636 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrRPAttr(yyvsp[-2].string, yyvsp[-1].methodlist));
}
    break;
case 492:
#line 2639 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrRPAttr(yyvsp[-2].rp_attr->name, yyvsp[-1].methodlist));
}
    break;
case 493:
#line 2642 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: invalid rp-attribute specification\n");
   yyerrok;
}
    break;
case 494:
#line 2649 "rpsl.y"
{
   yyval.methodlist = new List<AttrMethod>;
   if (yyvsp[0].method)
      yyval.methodlist->append(yyvsp[0].method);
}
    break;
case 495:
#line 2654 "rpsl.y"
{
   yyval.methodlist = yyvsp[-1].methodlist;
   if (yyvsp[0].method)
      yyval.methodlist->append(yyvsp[0].method);
}
    break;
case 496:
#line 2661 "rpsl.y"
{
   yyval.method = new AttrMethod(yyvsp[-2].string, new List<RPTypeNode>, false);
}
    break;
case 497:
#line 2664 "rpsl.y"
{
   yyval.method = new AttrMethod(yyvsp[-3].string, yyvsp[-1].typelist, false);
}
    break;
case 498:
#line 2667 "rpsl.y"
{
   yyval.method = new AttrMethod(yyvsp[-5].string, yyvsp[-3].typelist, true);
}
    break;
case 499:
#line 2670 "rpsl.y"
{
   char buffer[16];
   strcpy(buffer, "operator");
   strcat(buffer, yyvsp[-3].string);
   yyval.method = new AttrMethod(strdup(buffer), yyvsp[-1].typelist, false, true);
   free(yyvsp[-3].string);
}
    break;
case 500:
#line 2677 "rpsl.y"
{
   char buffer[16];
   strcpy(buffer, "operator");
   strcat(buffer, yyvsp[-5].string);
   yyval.method = new AttrMethod(strdup(buffer), yyvsp[-3].typelist, true, true);
   free(yyvsp[-5].string);
}
    break;
case 501:
#line 2684 "rpsl.y"
{
   free(yyvsp[-2].string);
   yyval.method = NULL;
   handle_error("Error: invalid method specification for %s\n", yyvsp[-2].string);
}
    break;
case 502:
#line 2689 "rpsl.y"
{
   yyval.method = NULL;
   handle_error("Error: invalid operator specification for %s\n", yyvsp[-2].string);
   free(yyvsp[-2].string);
}
    break;
case 503:
#line 2694 "rpsl.y"
{
   yyval.method = NULL;
   handle_error("Error: invalid operator\n");
}
    break;
case 504:
#line 2698 "rpsl.y"
{
   yyval.method = NULL;
   handle_error("Error: method specification expected\n");
}
    break;
case 505:
#line 2706 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrTypedef(yyvsp[-2].string, yyvsp[-1].typenode));
}
    break;
case 506:
#line 2709 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: invalid typedef specification\n");
   yyerrok;
}
    break;
case 507:
#line 2716 "rpsl.y"
{
   yyval.typelist = new List<RPTypeNode>;
   if (yyvsp[0].typenode)
      yyval.typelist->append(new RPTypeNode(yyvsp[0].typenode));
}
    break;
case 508:
#line 2721 "rpsl.y"
{
   yyval.typelist = yyvsp[-2].typelist;
   if (yyvsp[0].typenode)
      yyval.typelist->append(new RPTypeNode(yyvsp[0].typenode));
}
    break;
case 509:
#line 2728 "rpsl.y"
{
   yyval.typenode = RPType::newRPType("union", yyvsp[0].typelist);
   if (!yyval.typenode) {
      handle_error("Error: empty union specification\n");
      delete yyvsp[0].typelist;
   }
}
    break;
case 510:
#line 2735 "rpsl.y"
{
   if (yyvsp[0].typenode)
      yyval.typenode = new RPTypeRange(yyvsp[0].typenode);
   else {
      yyval.typenode = NULL;
   }
}
    break;
case 511:
#line 2742 "rpsl.y"
{
   yyval.typenode = RPType::newRPType(yyvsp[0].string);
   if (!yyval.typenode) {
      handle_error("Error: invalid type %s\n", yyvsp[0].string);
   }
   free(yyvsp[0].string);
}
    break;
case 512:
#line 2749 "rpsl.y"
{
   yyval.typenode = RPType::newRPType(yyvsp[-5].string, yyvsp[-3].i, yyvsp[-1].i);
   if (!yyval.typenode) {
      handle_error("Error: invalid type %s[%d,%d]\n", yyvsp[-5].string, yyvsp[-3].i, yyvsp[-1].i);
   }
   free(yyvsp[-5].string);
}
    break;
case 513:
#line 2756 "rpsl.y"
{
   yyval.typenode = RPType::newRPType(yyvsp[-5].string, yyvsp[-3].real, yyvsp[-1].real);
   if (!yyval.typenode) {
      handle_error("Error: invalid type %s[%f,%f]\n", yyvsp[-5].string, yyvsp[-3].real, yyvsp[-1].real);
   }
   free(yyvsp[-5].string);
}
    break;
case 514:
#line 2763 "rpsl.y"
{
   yyval.typenode = RPType::newRPType(yyvsp[-3].string, yyvsp[-1].wordlist);
   if (!yyval.typenode) {
      handle_error("Error: invalid type %s, enum expected\n", yyvsp[-3].string);
      delete yyvsp[-1].wordlist;
   }
   free(yyvsp[-3].string);
}
    break;
case 515:
#line 2771 "rpsl.y"
{
   if (yyvsp[0].typenode)
      if (yyvsp[-5].i < yyvsp[-3].i)
	 yyval.typenode = new RPTypeList(yyvsp[0].typenode, yyvsp[-5].i, yyvsp[-3].i);
      else
	 yyval.typenode = new RPTypeList(yyvsp[0].typenode, yyvsp[-3].i, yyvsp[-5].i);
   else {
      yyval.typenode = NULL;
      delete yyvsp[0].typenode;
   }
}
    break;
case 516:
#line 2782 "rpsl.y"
{
   if (yyvsp[0].typenode)
      yyval.typenode = new RPTypeList(yyvsp[0].typenode);
   else {
      yyval.typenode = NULL;
   }
}
    break;
case 517:
#line 2789 "rpsl.y"
{
   yyval.typenode = NULL;
   delete yyvsp[0].typenode;
   handle_error("Error: invalid list size\n");
}
    break;
case 518:
#line 2796 "rpsl.y"
{
   yyval.wordlist = new List<WordNode>;
   yyval.wordlist->append(new WordNode(yyvsp[0].string));
}
    break;
case 519:
#line 2800 "rpsl.y"
{
   yyval.wordlist = yyvsp[-2].wordlist;
   yyval.wordlist->append(new WordNode(yyvsp[0].string));
}
    break;
case 520:
#line 2808 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrProtocol(yyvsp[-2].string, yyvsp[-1].protocol_option_list));
}
    break;
case 521:
#line 2811 "rpsl.y"
{
   yyval.attr = yyvsp[-3].attr;
   handle_error("Error: invalid protocol option\n");
   yyerrok;
}
    break;
case 522:
#line 2816 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: invalid protocol name\n");
   yyerrok;
}
    break;
case 523:
#line 2823 "rpsl.y"
{
   yyval.protocol_option_list = new List<AttrProtocolOption>;
}
    break;
case 524:
#line 2826 "rpsl.y"
{
   yyval.protocol_option_list = yyvsp[-1].protocol_option_list;
   yyval.protocol_option_list->append(yyvsp[0].protocol_option);
}
    break;
case 525:
#line 2832 "rpsl.y"
{
   yyval.protocol_option = new AttrProtocolOption(false, yyvsp[0].method);
}
    break;
case 526:
#line 2835 "rpsl.y"
{
   yyval.protocol_option = new AttrProtocolOption(true, yyvsp[0].method);
}
    break;
case 527:
#line 2842 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr(ATTR_GENERIC, RPType::newRPType("free_text"));
}
    break;
case 528:
#line 2845 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr(ATTR_GENERIC, RPType::newRPType("free_text"));
   *yyval.rpslattr |= *yyvsp[0].rpslattr;
   delete yyvsp[0].rpslattr;
}
    break;
case 529:
#line 2852 "rpsl.y"
{
   yyval.rpslattr = yyvsp[0].rpslattr;
}
    break;
case 530:
#line 2855 "rpsl.y"
{
   yyval.rpslattr = yyvsp[-2].rpslattr;
   *yyval.rpslattr |= *yyvsp[0].rpslattr;
   delete yyvsp[0].rpslattr;
}
    break;
case 531:
#line 2860 "rpsl.y"
{
   yyval.rpslattr = yyvsp[0].rpslattr;
   handle_error("Error: in attr option specification.\n");
}
    break;
case 532:
#line 2866 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr(ATTR_GENERIC, yyvsp[-1].typenode);
}
    break;
case 533:
#line 2869 "rpsl.y"
{
   int syntax = schema.searchAttrSyntax(yyvsp[-1].string);
   if (syntax < 0) {
      handle_error("Error: no known syntax rule for %s.\n", yyvsp[-1].string);
      yyval.rpslattr = new AttrAttr;
   } else
      yyval.rpslattr = new AttrAttr(syntax, NULL);
   free(yyvsp[-1].string);
}
    break;
case 534:
#line 2878 "rpsl.y"
{
   regex_t *re = (regex_t *) malloc(sizeof(regex_t));
   int code = regcomp(re, yyvsp[-1].string, REG_EXTENDED | REG_ICASE);

   if (code) {
      handle_error("Error: could not compile regexp %s.\n", yyvsp[-1].string);
      free(re);
      free(yyvsp[-1].string);
      yyval.rpslattr = new AttrAttr;
   } else
      yyval.rpslattr = new AttrAttr(ATTR_REGEXP, yyvsp[-1].string, re);
}
    break;
case 535:
#line 2890 "rpsl.y"
{
   regex_t *re = (regex_t *) malloc(sizeof(regex_t));
   int code = regcomp(re, yyvsp[-3].string, REG_EXTENDED | REG_ICASE);

   if (code) {
      handle_error("Error: could not compile regexp %s.\n", yyvsp[-3].string);
      free(re);
      free(yyvsp[-3].string);
      yyval.rpslattr = new AttrAttr;
   } else
      yyval.rpslattr = new AttrAttr(ATTR_REGEXP, yyvsp[-3].string, re, yyvsp[-1].i);
}
    break;
case 536:
#line 2902 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr(AttrAttr::OPTIONAL);
}
    break;
case 537:
#line 2905 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr;
}
    break;
case 538:
#line 2908 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr(AttrAttr::DELETED);
}
    break;
case 539:
#line 2911 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr;
}
    break;
case 540:
#line 2914 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr(AttrAttr::MULTIVALUED);
}
    break;
case 541:
#line 2917 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr(AttrAttr::LOOKUP);
}
    break;
case 542:
#line 2920 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr(AttrAttr::KEY);
}
    break;
case 543:
#line 2923 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr(AttrAttr::OBSOLETE);
}
    break;
case 544:
#line 2926 "rpsl.y"
{
   yyval.rpslattr = new AttrAttr(AttrAttr::INTERNAL);
}
    break;
case 545:
#line 2931 "rpsl.y"
{
   yyvsp[-1].rpslattr->setName(yyvsp[-2].string);
   yyval.attr = changeCurrentAttr(yyvsp[-1].rpslattr);
}
    break;
case 546:
#line 2935 "rpsl.y"
{
   yyval.attr = yyvsp[-3].attr;
   free(yyvsp[-2].string);
   handle_error("Error: in attr option specification.\n");
   yyerrok;
}
    break;
case 547:
#line 2941 "rpsl.y"
{
   yyval.attr = yyvsp[-2].attr;
   handle_error("Error: attr name expected.\n");
   yyerrok;
}
    break;
case 548:
#line 2951 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrMntRoutes(yyvsp[-1].listMntPrfxPair));
}
    break;
case 549:
#line 2956 "rpsl.y"
{
   yyval.listMntPrfxPair = new List<AttrMntRoutes::MntPrfxPair>;
   yyval.listMntPrfxPair->append(yyvsp[0].mntPrfxPair);
}
    break;
case 550:
#line 2960 "rpsl.y"
{
   yyval.listMntPrfxPair = yyvsp[-2].listMntPrfxPair;
   yyval.listMntPrfxPair->append(yyvsp[0].mntPrfxPair);
}
    break;
case 551:
#line 2966 "rpsl.y"
{
   yyval.mntPrfxPair = new AttrMntRoutes::MntPrfxPair(yyvsp[0].string, NULL);
}
    break;
case 552:
#line 2969 "rpsl.y"
{
   yyval.mntPrfxPair = new AttrMntRoutes::MntPrfxPair(yyvsp[-1].string, NULL);
}
    break;
case 553:
#line 2972 "rpsl.y"
{
   yyval.mntPrfxPair = new AttrMntRoutes::MntPrfxPair(yyvsp[-3].string, (FilterMPPRFXList *) yyvsp[-1].filter);
}
    break;
case 554:
#line 2979 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrMntRoutes(yyvsp[-1].listMntPrfxPair));
}
    break;
case 555:
#line 2983 "rpsl.y"
{
   yyval.listMntPrfxPair = new List<AttrMntRoutes::MntPrfxPair>;
   yyval.listMntPrfxPair->append(yyvsp[0].mntPrfxPair);
}
    break;
case 556:
#line 2987 "rpsl.y"
{
   yyval.listMntPrfxPair = yyvsp[-2].listMntPrfxPair;
   yyval.listMntPrfxPair->append(yyvsp[0].mntPrfxPair);
}
    break;
case 557:
#line 2993 "rpsl.y"
{
   yyval.mntPrfxPair = new AttrMntRoutes::MntPrfxPair(yyvsp[0].string, NULL);
}
    break;
case 558:
#line 2996 "rpsl.y"
{
   yyval.mntPrfxPair = new AttrMntRoutes::MntPrfxPair(yyvsp[-1].string, NULL);
}
    break;
case 559:
#line 2999 "rpsl.y"
{
   yyval.mntPrfxPair = new AttrMntRoutes::MntPrfxPair(yyvsp[-3].string, (FilterMPPRFXList *) yyvsp[-1].filter);
}
    break;
case 560:
#line 3006 "rpsl.y"
{
   yyval.attr = changeCurrentAttr(new AttrMntRoutes(yyvsp[-1].listMntPrfxPair));
}
    break;
case 561:
#line 3010 "rpsl.y"
{
   yyval.listMntPrfxPair = new List<AttrMntRoutes::MntPrfxPair>;
   yyval.listMntPrfxPair->append(yyvsp[0].mntPrfxPair);
}
    break;
case 562:
#line 3014 "rpsl.y"
{
   yyval.listMntPrfxPair = yyvsp[-2].listMntPrfxPair;
   yyval.listMntPrfxPair->append(yyvsp[0].mntPrfxPair);
}
    break;
case 563:
#line 3020 "rpsl.y"
{
   yyval.mntPrfxPair = new AttrMntRoutes::MntPrfxPair(yyvsp[0].string, NULL);
}
    break;
case 564:
#line 3023 "rpsl.y"
{
   yyval.mntPrfxPair = new AttrMntRoutes::MntPrfxPair(yyvsp[-1].string, NULL);
}
    break;
case 565:
#line 3026 "rpsl.y"
{
   yyval.mntPrfxPair = new AttrMntRoutes::MntPrfxPair(yyvsp[-3].string, (FilterMPPRFXList *) yyvsp[-1].filter);
}
    break;
case 566:
#line 3039 "rpsl.y"
{
  yyval.list = yyvsp[0].list;
}
    break;
case 567:
#line 3042 "rpsl.y"
{
  yyval.list = new ItemList;
  yyval.list->append(new ItemAFI("any"));
  yyval.list = yyval.list->expand();
}
    break;
case 568:
#line 3049 "rpsl.y"
{
  yyval.list = new ItemList;
  yyval.list->append(yyvsp[0].item);
  yyval.list = yyval.list->expand();
}
    break;
case 569:
#line 3054 "rpsl.y"
{
  yyval.list = yyvsp[-2].list;
	yyval.list->append(yyvsp[0].item);
  yyval.list = yyval.list->expand();
}
    break;
case 570:
#line 3061 "rpsl.y"
{
    yyval.item = new ItemAFI(yyvsp[0].afi);
}
    break;
case 571:
#line 3064 "rpsl.y"
{
    // workaround for clashing 'any'
    yyval.item = new ItemAFI("any");
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
#line 3070 "rpsl.y"


void enable_yy_parser_debugging() {
#if YYDEBUG != 0
   yydebug = 1;
#endif
}

void handleArgumentTypeError(char *attr, char *method, int position,
			     const RPType *correctType, 
			     bool isOperator = false) {
   if (isOperator)
      if (position)
	 handle_error("Error: argument %d to %s.operator%s should be %s.\n",
		   position, attr, method, ((RPType *) correctType)->name());
      else
	 handle_error("Error: wrong number of arguments to %s.operator%s.\n",
		      attr, method);
   else
      if (position)
	 handle_error("Error: argument %d to %s.%s should be %s.\n",
		   position, attr, method, ((RPType *) correctType)->name());
      else
	 handle_error("Error: wrong number of arguments to %s.%s.\n",
		      attr, method);
}

const AttrMethod *searchMethod(const AttrRPAttr *rp_attr, char *method, ItemList *args) {
   const AttrMethod *mtd = rp_attr->searchMethod(method);
   int position;
   const RPType *correctType;
   
   if (!mtd) {
      handle_error("Error: rp-attribute %s does not have %s defined.\n",
		   rp_attr->name, method);
      return NULL;
   }
   
   for (; mtd; mtd = rp_attr->searchNextMethod(mtd))
      if (mtd->validateArgs(args, position, correctType))
	 return mtd;

   handleArgumentTypeError(rp_attr->name, method, position, correctType);
   
   return NULL;
}
