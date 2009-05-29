/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TKN_IP = 258,
     TKN_ASNUM = 259,
     TKN_ERROR = 260,
     TKN_STR = 261,
     TKN_INT = 262,
     TKN_DNSNAME = 263,
     TKN_FILTER = 264,
     TKN_WORD = 265,
     KW_IMPORT = 266,
     KW_EXPORT = 267,
     KW_EXPORT_GROUP = 268,
     KW_IMPORT_GROUP = 269,
     KW_PRINT_PREFIXES = 270,
     KW_PRINT_PREFIX_RANGES = 271,
     KW_PRINT_SUPER_PREFIX_RANGES = 272,
     KW_ASPATH_ACCESS_LIST = 273,
     KW_ACCESS_LIST = 274,
     KW_CONFIGURE_ROUTER = 275,
     KW_STATIC2BGP = 276,
     KW_SET = 277,
     KW_CISCO_MAP_NAME = 278,
     KW_JUNOS_POLICY_NAME = 279,
     KW_CISCO_PREFIX_ACL_NO = 280,
     KW_CISCO_ASPATH_ACL_NO = 281,
     KW_CISCO_PKTFILTER_ACL_NO = 282,
     KW_CISCO_COMMUNITY_ACL_NO = 283,
     KW_CISCO_ACCESS_LIST_NO = 284,
     KW_SOURCE = 285,
     KW_PREFERENCECEILING = 286,
     KW_CISCO_MAX_PREFERENCE = 287,
     KW_NETWORKS = 288,
     KW_V6NETWORKS = 289,
     KW_DEFAULT = 290,
     KW_CISCO_MAP_INC = 291,
     KW_CISCO_MAP_START = 292,
     KW_PKT_FILTER = 293,
     KW_OUTBOUND_PKT_FILTER = 294,
     KW_BCC_VERSION = 295,
     KW_BCC_MAX_PREFERENCE = 296,
     KW_BCC_ADVERTISE_NETS = 297,
     KW_BCC_ADVERTISE_ALL = 298,
     KW_BCC_FORCE_BACK = 299,
     KW_BCC_MAX_PREFIXES = 300
   };
#endif
/* Tokens.  */
#define TKN_IP 258
#define TKN_ASNUM 259
#define TKN_ERROR 260
#define TKN_STR 261
#define TKN_INT 262
#define TKN_DNSNAME 263
#define TKN_FILTER 264
#define TKN_WORD 265
#define KW_IMPORT 266
#define KW_EXPORT 267
#define KW_EXPORT_GROUP 268
#define KW_IMPORT_GROUP 269
#define KW_PRINT_PREFIXES 270
#define KW_PRINT_PREFIX_RANGES 271
#define KW_PRINT_SUPER_PREFIX_RANGES 272
#define KW_ASPATH_ACCESS_LIST 273
#define KW_ACCESS_LIST 274
#define KW_CONFIGURE_ROUTER 275
#define KW_STATIC2BGP 276
#define KW_SET 277
#define KW_CISCO_MAP_NAME 278
#define KW_JUNOS_POLICY_NAME 279
#define KW_CISCO_PREFIX_ACL_NO 280
#define KW_CISCO_ASPATH_ACL_NO 281
#define KW_CISCO_PKTFILTER_ACL_NO 282
#define KW_CISCO_COMMUNITY_ACL_NO 283
#define KW_CISCO_ACCESS_LIST_NO 284
#define KW_SOURCE 285
#define KW_PREFERENCECEILING 286
#define KW_CISCO_MAX_PREFERENCE 287
#define KW_NETWORKS 288
#define KW_V6NETWORKS 289
#define KW_DEFAULT 290
#define KW_CISCO_MAP_INC 291
#define KW_CISCO_MAP_START 292
#define KW_PKT_FILTER 293
#define KW_OUTBOUND_PKT_FILTER 294
#define KW_BCC_VERSION 295
#define KW_BCC_MAX_PREFERENCE 296
#define KW_BCC_ADVERTISE_NETS 297
#define KW_BCC_ADVERTISE_ALL 298
#define KW_BCC_FORCE_BACK 299
#define KW_BCC_MAX_PREFIXES 300




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 89 "command.y"
{
   int i;
   char *val;
   /*IPAddr *ip;*/
   MPPrefix *ip;
   ASt as;
}
/* Line 1529 of yacc.c.  */
#line 147 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE commandlval;

