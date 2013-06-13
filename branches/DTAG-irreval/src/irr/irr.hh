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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#ifndef _IRR_HH
#define _IRR_HH

#include "config.h"
#include "rpsl/object.hh"
#include "dataset/SetOfUInt.hh"
#include "dataset/prefixranges.hh"
#include "rpsl/prefix.hh"
#include "classes.hh"

class AutNum;
class Route;

extern void collectASNO(void *result,   const Object *o);
extern void collectIfAddr(void *result, const Object *o);
extern void collectPrefix(void *result, const Object *o);

class IRR {
   friend class RPSLConfig;

public:
   enum PROTOCOL { rawhoisd, ripe, bird, unknown = -1 };

#ifdef DIAG
   char *last_query;

   // indicates that an error occurred when queriing the IRR
   // 0 == no error occurred
   // 1 == query error occurred
   // 2 == response error occurred
   int err_occ;

   // indicates that an error occurred when queriing the IRR
   // but the error shall be treated as warning
   bool warn_occ;

   // if "err_occ" or "warn_occ" is true this array carries the related error message.
   char *err_msg;
#endif /* DIAG */

protected:
   static char        buffer[128];   // temporary buffer for format conversion
   static char const *dflt_host;    
   static char const *dflt_sources;
   static char const *proxy_host;
   static int         proxy_port;
   static int         dflt_port;
   static PROTOCOL    dflt_protocol;
   static bool        ignore_errors;
   static bool        use_proxy_connection;
#ifdef DIAG
   static bool        irr_warnings;
   static Buffer       rpslInfo;
#endif /* DIAG */
#ifdef RPKI
   static char const *rpkiSymbolFormat;
   static int         rpkiExpansionMode;
#endif /* RPKI */
#ifdef RIPE
   static int         responseRetries;
   static int         responseTimeout;
#endif /* RIPE */

   // Moved from RAWhoisClient
   static char     protocol[64];
   char     host[64];
   char     sources[256];
   int      port;

public:
   IRR(void) {
     strcpy(host, dflt_host);
     strcpy(sources, dflt_sources);
     port = dflt_port;
#ifdef DIAG
     err_occ = 0;
     warn_occ = false;
     err_msg = new char[0];
#endif /* DIAG */
   }
   // connection management
   virtual void Open(const char *_host = dflt_host, 
		     const int _port = dflt_port, 
		     const char *_sources = dflt_sources) = 0;
   virtual void Close() = 0;
   virtual void SetSources(const char *_sources = dflt_sources) {
     strcpy(sources, _sources);
   }

   // get objects
   const AutNum     *getAutNum(ASt as);
   const Set        *getSet(SymID asset, const char *clss);
   const ASSet      *getASSet(SymID asset);
   const RSSet      *getRSSet(SymID rsset);
   const RtrSet     *getRtrSet(SymID rtrset);
   const FilterSet  *getFilterSet(SymID fset);
   const PeeringSet *getPeeringSet(SymID set);
   const InetRtr    *getInetRtr(SymID inetRtr);
   void getRoute(Route *&route, Prefix *rt, ASt as);
   void getRoute(Route *&route, char *rt, ASt as);

   // expand sets
   // REIMP const PrefixRanges *expandAS(ASt as);
   const MPPrefixRanges *expandAS(ASt as);
   const SetOfUInt    *expandASSet(SymID asset);
   // REIMP const PrefixRanges *expandRSSet(SymID rsset)
   const MPPrefixRanges *expandRSSet(SymID rsset);
   // REIMP const PrefixRanges *expandRtrSet(SymID rtrset);
   const MPPrefixRanges *expandRtrSet(SymID rtrset);

   // options
   static void SetDefaultHost(const char *_host);
   static void SetProxyHost(const char *_proxyHost);
   static void SetDefaultSources(const char *_sources);
   static void SetDefaultPort(const int _port);
   static void SetProxyPort(const int _proxyPort);
   static void SetDefaultProtocol(const char *_protocol);
   static PROTOCOL GetProtocol(void) {
     return dflt_protocol;
   }
   static void initCache(const char *filename); // read objects from filename

#ifdef RPKI
   // RPKI functionality
   static void  initRPKICache(const char *filename);
   static void  setRPKISymbolFormat(const char *format);
   static void  setRPKIExpansionMode(const char *mode);
#endif /* RPKI */

#ifdef RIPE
   static void setResponseRetries(const int num);
   static void setResponseTimeout(const int sec);
#endif /* RIPE */

   // the following interface is used by ParseArgv
   static int ArgvHost     (char *dst, char *key, const char *_nextArg);
   static int ArgvProxyHost(char *dst, char *key, const char *_nextArg);
   static int ArgvSources  (char *dst, char *key, const char *_nextArg);
   static int ArgvPort     (char *dst, char *key, const char *_nextArg);
   static int ArgvProxyPort(char *dst, char *key, const char *_nextArg);
   static int ArgvInitCache(char *dst, char *key, const char *_nextArg);
   static int ArgvProtocol(char *dst, char *key, const char *_nextArg);
   static int ReportErrors(char *dst, char *key, const char *_nextArg);
   static int IgnoreErrors(char *dst, char *key, const char *_nextArg);
#ifdef DIAG
   static int DBWarnings(char *dst, char *key, const char *_nextArg);
#endif /* DIAG */

#ifdef RPKI
   static int ArgvInitRPKICache(char *dst, char *key, const char *_nextArg);
   static int ArgvRPKISymbolFormat(char *dst, char *key, const char *_nextArg);
   static int ArgvRPKIExpansionMode(char *dst, char *key, const char *_nextArg);
#endif /* RPKI */

#ifdef RIPE
   static int ArgvResponseRetries(char *dst, char *key, const char *_nextArg);
   static int ArgvResponseTimeout(char *dst, char *key, const char *_nextArg);
#endif /* RIPE */

   static void handleEnvironmentVariables(char **envp);
   static IRR *newClient();

public:
   virtual void setFastResponseFlag(bool onoff) {}
   virtual void querySourceOrigin(const char *rt) = 0;
   virtual bool readReady(void) = 0;

   // Added by wlee@isi.edu to determine the AS# by given an IP addr
   // If the route is not found, an one-level less specific route will be used
public:
   virtual ASt getOrigin(char *format, ...) = 0;

#ifdef DIAG
   void die_error(const char *txt, int type, bool die = true);
   void warn_error(void);
   void clear_warning(void);
#endif /* DIAG */

protected:
   // the following to be implemented by RAWhoisC and BirdWhoisC
   // their implementation do not know of RAToolSet internals
   // they work on char *'s
   // they put the database response into text by first allocating memory for it
   // using new char[size]
   // and set the len to strlen(text)
   virtual bool getAutNum(char *as,          char *&text, int &len) = 0;
   virtual bool getSet(SymID sname, const char *clss, char *&text, int &len) = 0;
   virtual bool getRoute(char *rt, char *as, char *&text, int &len) = 0;
   virtual bool getInetRtr(SymID inetrtr,    char *&text, int &len) = 0;

   // REIMP virtual bool expandAS(char *as,           PrefixRanges *result)  = 0;
   virtual bool expandAS(char *as,           MPPrefixRanges *result)  = 0;
   virtual bool expandASSet(SymID asset,     SetOfUInt    *result)  = 0;
   // REIMP virtual bool expandRSSet(SymID rsset,     PrefixRanges *result) = 0;
   virtual bool expandRSSet(SymID rsset,     MPPrefixRanges *result) = 0;
   // REIMP virtual bool expandRtrSet(SymID rtrset,   PrefixRanges *result)  = 0;
   virtual bool expandRtrSet(SymID rtrset,   MPPrefixRanges *result)  = 0;
   // REIMP void         expandItem(Item *pt,         PrefixRanges *result);
   void         expandItem(Item *pt,         MPPrefixRanges *result);

   bool         isIndirectMember(Object *o, 
			   AttrGenericIterator<ItemWORD> &mbrs_by_ref);

#ifdef DIAG
  // the following method is used to extract some info when the query/response process revealed
  // problems. In this case the RPSL object name is stored in a buffer that is used by RPSLConfig
  static void extractRPSLInfo(char *query);
#endif /* DIAG */
  // next method is used by RAWhoisClient to query the SetCache
  bool queryCache(SymID setID, Set *&set);


private:
   static void initCache(char *objectText, int objectLength, const char *clss, const char *fname);
};

struct ProtocolName {
  IRR::PROTOCOL protocol;
  const char *name;
};

extern IRR *irr;
extern ProtocolName protocolName[];

//////// Command line options ///////////////////////////////////////////

#ifdef RPKI
#define IRR_CACHE_COMMAND_LINE_OPTIONS \
	{"-f", ARGV_FUNC, (char *) &IRR::ArgvInitCache, (char *) NULL,\
	 "A file name containing only RPSL aut-num objects"},\
	{"-rf", ARGV_FUNC, (char *) &IRR::ArgvInitRPKICache, (char *) NULL,\
	 "A file name containing only RPKI expansion AS objects"},\
	{"-rs", ARGV_FUNC, (char *) &IRR::ArgvRPKISymbolFormat, (char *) NULL,\
	 "Symbolname format for RPKI searching route-set objects"},\
	{"-re", ARGV_FUNC, (char *) &IRR::ArgvRPKIExpansionMode, (char *) NULL,\
	 "Expansion mode for route-sets: first, rpki, rpsl or all (default)"}
#else
#define IRR_CACHE_COMMAND_LINE_OPTIONS \
	{"-f", ARGV_FUNC, (char *) &IRR::ArgvInitCache, (char *) NULL,\
	"A file name containing only RPSL aut-num objects"}
#endif /* RPKI */

#ifdef RIPE
#define IRR_WHOIS_COMMAND_LINE_OPTIONS \
	{"-protocol", ARGV_FUNC, (char *)&IRR::ArgvProtocol, (char *) NULL,\
	 "Protocol to be used to connect to the server"},\
	{"-retries", ARGV_FUNC, (char *) &IRR::ArgvResponseRetries, (char *) NULL,\
	 "maximum number of retries on response timeouts (RIPE: default 7)"},\
	{"-timeout", ARGV_FUNC, (char *) &IRR::ArgvResponseTimeout, (char *) NULL,\
	 "number of seconds for response timeout (RIPE: default 3)"}
#else
#define IRR_WHOIS_COMMAND_LINE_OPTIONS \
	{"-protocol", ARGV_FUNC, (char *)&IRR::ArgvProtocol, (char *) NULL,\
	 "Protocol to be used to connect to the server"}
#endif /* RIPE */

#ifdef DIAG
#define IRR_ERROR_COMMAND_LINE_OPTIONS \
	{"-db_warnings", ARGV_FUNC, (char *)&IRR::DBWarnings, (char *) NULL,\
	 "Print warning messages if errors occurred during IRR inquiry"},\
	{"-ignore_errors", ARGV_FUNC, (char *)&IRR::IgnoreErrors, (char *) NULL,\
	 "Ignore IRR error and warning messages"},\
	{"-report_errors", ARGV_FUNC, (char *)&IRR::ReportErrors, (char *) NULL,\
	 "Print IRR error and warning messages"}
#else
#define IRR_ERROR_COMMAND_LINE_OPTIONS \
	{"-ignore_errors", ARGV_FUNC, (char *)&IRR::IgnoreErrors, (char *) NULL,\
	 "Ignore IRR error and warning messages"},\
	{"-report_errors", ARGV_FUNC, (char *)&IRR::ReportErrors, (char *) NULL,\
	 "Print IRR error and warning messages"}
#endif /* DIAG */

#define IRR_COMMAND_LINE_OPTIONS \
	{"-h", ARGV_FUNC, (char *) &IRR::ArgvHost, (char *) NULL,\
	 "Host name of the RAWhoisd server"},\
	{"-p", ARGV_FUNC, (char *) &IRR::ArgvPort, (char *) NULL,\
	 "Port number of the RAWhoisd server"},\
	{"-proxy_host", ARGV_FUNC, (char *) &IRR::ArgvProxyHost, (char *) NULL,\
	 "Proxy to use when tunneling RAWhoisd server communication"},\
	{"-proxy_port", ARGV_FUNC, (char *) &IRR::ArgvProxyPort, (char *) NULL,\
	 "Proxy port number to use when tunneling RAWhoisd server communication"},\
	{"-s", ARGV_FUNC, (char *) &IRR::ArgvSources,   (char *) NULL,\
	 "Order of databases"},\
	IRR_CACHE_COMMAND_LINE_OPTIONS,\
	IRR_WHOIS_COMMAND_LINE_OPTIONS,\
	IRR_ERROR_COMMAND_LINE_OPTIONS

#endif // _IRR_HH
