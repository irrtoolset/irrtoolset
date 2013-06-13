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
//  Author(s): Hagen Boehm <hboehm@brutus.nic.dtag.de>

#ifndef IRREVAL_H
#define IRREVAL_H

#include "f_base.hh"
#include "config.h"
#include "rpsl/rpsl.hh"

class AttrRPAttr;
class AttrMethod;
class SetOfPrefix;
class SetOfIPv6Prefix;
class FilterOfASPath;
class MPPrefix;

class IRReval {

  struct FNameList {
    char *name;
    FNameList *next;
  };

public:
   IRReval();

   void initialize ();
   void configure (int argc, char **argv, char **envp);
   void reconfigure (int argc, char **argv);
   void setCompression();
   void accessList(char *filter);
   void aspathAccessList(char *filter);
   void rpslASSet(char *filter);
   void evaluate(char *command);
   void evaluate_rpsl(char *command);
   void evaluate_vendor(char *command);
   void extractNameAndExpr(char *command, char *ctype, char *&name, char *&expr);
   bool configFormatIs(char *format);

   static int select_config_format(char *dst, char *key, char *nextArg);
   static int set_config_cache_dir(char *dst, char *key, char *nextArg);
   static int set_compression_format(char *dst, char *key, char *nextArg);
   static int use_16bit_ASN(char *dst, char *key, char *nextArg);
   static int use_32bit_ASN(char *dst, char *key, char *nextArg);
   static int use_opt_ASDOT(char *dst, char *key, char *nextArg);
#ifdef RPKI
   static int add_rpki_filename(char *dst, char *key, char *nextArg);
#endif /* RPKI */
   static int add_rpsl_filename(char *dst, char *key, char *nextArg);


   static bool printEmptyLists;
   static void loadDictionary();

   static const AttrRPAttr  *dctn_rp_pref;
   static const AttrMethod  *dctn_rp_pref_set;
   static const AttrRPAttr  *dctn_rp_nhop;
   static const AttrMethod  *dctn_rp_nhop_set;
   static const AttrRPAttr  *dctn_rp_dpa;
   static const AttrMethod  *dctn_rp_dpa_set;
   static const AttrRPAttr  *dctn_rp_med;
   static const AttrMethod  *dctn_rp_med_set;
   static const AttrRPAttr  *dctn_rp_community;
   static const AttrMethod  *dctn_rp_community_setop;
   static const AttrMethod  *dctn_rp_community_appendop;
   static const AttrMethod  *dctn_rp_community_append;
   static const AttrMethod  *dctn_rp_community_delete;
   static const AttrRPAttr  *dctn_rp_aspath;
   static const AttrMethod  *dctn_rp_aspath_prepend;

private:
   static BaseConfig  *configuration;
   static char        *configFormat;
   static char        *configCacheDir;
   static int          configCompression;
   static bool         configASN16;
   static bool         configASDot;
   static bool         initialized;
   static char        *executableName;
   static FNameList   *rpkiFilenameList;
   static FNameList   *rpslFilenameList;

};

extern char *opt_prompt;

//////// Command line options ///////////////////////////////////////////

#ifdef RPKI
#define IRREVAL_CACHE_COMMAND_LINE_OPTIONS \
	{"-d", ARGV_FUNC, (char *) &set_config_cache_dir, (char *) NULL, \
	 "Directory for all RPKI and RPSL files with no path"}, \
	{"-f", ARGV_FUNC, (char *) &IRReval::add_rpsl_filename, (char *) NULL, \
	 "A file name containing only RPSL as-set and route-set objects"}, \
	{"-rf", ARGV_FUNC, (char *) &IRReval::add_rpki_filename, (char *) NULL, \
	 "A file name containing only RPKI expansion AS objects"}, \
	{"-rs", ARGV_FUNC, (char *) &IRR::ArgvRPKISymbolFormat, (char *) NULL, \
	 "Symbolname format for RPKI searching route-set objects"}, \
	{"-re", ARGV_FUNC, (char *) &IRR::ArgvRPKIExpansionMode, (char *) NULL, \
	 "Expansion mode for route-sets: first, rpki, rpsl or all (default)"}
#else
#define IRREVAL_CACHE_COMMAND_LINE_OPTIONS \
	{"-d", ARGV_FUNC, (char *) &IRReval::set_config_cache_dir, (char *) NULL, \
	 "Directory for all RPKI and RPSL files with no path"}, \
	{"-f", ARGV_FUNC, (char *) &IRReval::add_rpsl_filename, (char *) NULL, \
	 "A file name containing only RPSL as-set and route-set objects"}
#endif /* RPKI */

#ifdef DTAG
#define IRREVAL_DTAG_COMMAND_LINE_OPTIONS \
	{"-asn16", ARGV_FUNC, (char *) &IRReval::use_16bit_ASN, (char *) NULL, \
	 "translate 32bit AS numbers into AS23456."}, \
	{"-asdot", ARGV_FUNC, (char *) &IRReval::use_opt_ASDOT, (char *) NULL, \
	 "print AS numbers in asdot format."}, \
	{"-asn32", ARGV_FUNC, (char *) &IRReval::use_32bit_ASN, (char *) NULL, \
	 "print AS numbers in plain format (default).\n"}
#else
#define IRREVAL_DTAG_COMMAND_LINE_OPTIONS \
	{"-asdot", ARGV_FUNC, (char *) &IRReval::use_opt_ASDOT, (char *) NULL, \
	 "print AS numbers in asdot format."}, \
	{"-asn32", ARGV_FUNC, (char *) &IRReval::use_32bit_ASN, (char *) NULL, \
	 "print AS numbers in plain format (default).\n"}
#endif /* DTAG */


#define IRREVAL_COMMAND_LINE_OPTIONS \
	{"-h", ARGV_FUNC, (char *) &IRR::ArgvHost, (char *) NULL, \
	 "Host name of the RAWhoisd server"}, \
	{"-p", ARGV_FUNC, (char *) &IRR::ArgvPort, (char *) NULL, \
	 "Port number of the RAWhoisd server"}, \
	{"-proxy_host", ARGV_FUNC, (char *) &IRR::ArgvProxyHost, (char *) NULL, \
	 "Proxy to use when tunneling RAWhoisd server communication"}, \
	{"-proxy_port", ARGV_FUNC, (char *) &IRR::ArgvProxyPort, (char *) NULL, \
	 "Proxy port number to use when tunneling RAWhoisd server communication"}, \
	{"-s", ARGV_FUNC, (char *) &IRR::ArgvSources, (char *) NULL, \
	 "Order of databases"}, \
	IRREVAL_CACHE_COMMAND_LINE_OPTIONS, \
	IRR_WHOIS_COMMAND_LINE_OPTIONS, \
	IRR_ERROR_COMMAND_LINE_OPTIONS, \
	{"-config", ARGV_FUNC, (char *) &IRReval::select_config_format, (char *) NULL, \
	 "Configuration format (junos, ios, iosxr, rpsl)"}, \
	{"-disable_access_list_cache", ARGV_BOOL, (char *) NULL, (char *) &BaseConfig::useAclCaches, \
	 "Do not reuse the old access list numbers (Cisco only)."}, \
	{"-compression", ARGV_FUNC, (char *) &IRReval::set_compression_format, (char *) NULL, \
	 "Level of compression when prefix-lists are generated."}, \
	{"-cisco_force_tilda", ARGV_BOOL, (char *) NULL, (char *) &BaseConfig::forceTilda, \
	 "Interpret * and + as ~* and ~+ operators in AS paths to shorten as path acl's lines (Cisco only)."}, \
	IRREVAL_DTAG_COMMAND_LINE_OPTIONS

// option singleRouteFilter
//	{"-junos_single_routefilter", ARGV_BOOL, (char *) NULL, (char *) &BaseConfig::singleRouteFilter, \
//	"Configure only one route-filter in multiple prefix blocks (Junos only)."}, \

#endif   // IRREVAL_H
