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

#ifndef _IRR_HH
#define _IRR_HH

#include "config.h"
#include "rpsl/object.hh"
#include "gnug++/SetOfUInt.hh"
#include "gnu/prefixranges.hh"
#include "classes.hh"

class AutNum;
class Route;

extern void collectASNO(void *result,   const Object *o);
extern void collectIfAddr(void *result, const Object *o);
extern void collectPrefix(void *result, const Object *o);

class IRR {
public:
   enum PROTOCOL { rawhoisd, ripe, bird, unknown = -1 };

protected:
   static char        buffer[128];   // temporary buffer for format conversion
   static char const *dflt_host;    
   static char const *dflt_sources;
   static int         dflt_port;
   static PROTOCOL    dflt_protocol;
   static bool        ignore_errors;
   // Moved from RAWhoisClient
   static char     protocol[64];
   char     host[64];
   char     sources[256];
   int      port;

public:
   // Added by wlee@isi.edu
   IRR(void) {
     strcpy(host, dflt_host);
     strcpy(sources, dflt_sources);
     port = dflt_port;
   }
   // connection management
   virtual void Open(const char *_host = dflt_host, 
		     const int _port = dflt_port, 
		     const char *_sources = dflt_sources) = 0;
   virtual void Close() = 0;
   // Modified by wlee@isi.edu
   //  virtual void SetSources(char *s) {}
   virtual void SetSources(const char *_sources = dflt_sources) {
     strcpy(sources, _sources);
   }

   // Added by wlee@isi.edu
   virtual const char *GetSources(void) { 
     return sources;
   }

   // get objects
   const AutNum     *getAutNum(ASt as);
   const Set        *getSet(SymID asset, char *clss);
   const ASSet      *getASSet(SymID asset);
   const RSSet      *getRSSet(SymID rsset);
   const RtrSet     *getRtrSet(SymID rtrset);
   const FilterSet  *getFilterSet(SymID fset);
   const PeeringSet *getPeeringSet(SymID set);
   const InetRtr    *getInetRtr(SymID inetRtr);
   void getRoute(Route *&route, Prefix *rt, ASt as);
   // Added by wlee
   void getRoute(Route *&route, char *rt, ASt as);

   // expand sets
   const PrefixRanges *expandAS(ASt as);
   const SetOfUInt    *expandASSet(SymID asset);
   const PrefixRanges *expandRSSet(SymID rsset);
   const PrefixRanges *expandRtrSet(SymID rtrset);

   // options
   static void SetDefaultHost(const char *_host);
   static void SetDefaultSources(const char *_sources);
   static void SetDefaultPort(const int _port);
   static void SetDefaultProtocol(const char *_protocol);
   static PROTOCOL GetProtocol(void) {
     return dflt_protocol;
   }
   static void initCache(const char *filename); // read objects from filename

   // the following interface is used by ParseArgv
   static int ArgvHost     (char *dst, char *key, const char *_nextArg);
   static int ArgvSources  (char *dst, char *key, const char *_nextArg);
   static int ArgvPort     (char *dst, char *key, const char *_nextArg);
   static int ArgvInitCache(char *dst, char *key, const char *_nextArg);
   static int ArgvProtocol(char *dst, char *key, const char *_nextArg);
   static int ReportErrors(char *dst, char *key, const char *_nextArg);
   static int IgnoreErrors(char *dst, char *key, const char *_nextArg);

   static void handleEnvironmentVariables(char **envp);
   static IRR *newClient();

   // Added by wlee@isi.edu 
   // For compatibility reasons, mostly for roe
public:
   virtual void setFullObjectFlag(bool onoff) {}
   virtual void setFastResponseFlag(bool onoff) {}
   virtual int getSourceOrigin(char *&buffer, const char *rt) = 0;
   virtual int getSourceOrigin(char *&buffer) = 0;
   virtual void querySourceOrigin(const char *rt) = 0;
   virtual bool readReady(void) = 0;

   // Added by wlee@isi.edu to determine the AS# by given an IP addr
   // If the route is not found, an one-level less specific route will be used
public:
   virtual ASt getOrigin(char *format, ...) = 0;

protected:
   // the following to be implemented by RAWhoisC and BirdWhoisC
   // their implementation do not know of RAToolSet internals
   // they work on char *'s
   // they put the database response into text by first allocating memory for it
   // using new char[size]
   // and set the len to strlen(text)
   virtual bool getAutNum(char *as,          char *&text, int &len) = 0;
   virtual bool getSet(SymID sname, char *clss, char *&text, int &len) = 0;
   virtual bool getRoute(char *rt, char *as, char *&text, int &len) = 0;
   virtual bool getInetRtr(SymID inetrtr,    char *&text, int &len) = 0;

   virtual bool expandAS(char *as,           PrefixRanges *result)  = 0;
   virtual bool expandASSet(SymID asset,     SetOfUInt    *result)  = 0;
   virtual bool expandRSSet(SymID rsset,     PrefixRanges *result)  = 0;
   virtual bool expandRtrSet(SymID rtrset,   PrefixRanges *result)  = 0;
   void         expandItem(Item *pt,         PrefixRanges *result);
   bool         isIndirectMember(Object *o, 
			   AttrGenericIterator<ItemWORD> &mbrs_by_ref);

private:
   static void initCache(char *objectText, int objectLength, char *clss);
};

struct ProtocolName {
  IRR::PROTOCOL protocol;
  const char *name;
};

extern IRR *irr;
extern ProtocolName protocolName[];

//////// Command line options ///////////////////////////////////////////

#define IRR_COMMAND_LINE_OPTIONS \
     {"-h", ARGV_FUNC, (char *) &IRR::ArgvHost,      (char *) NULL,\
      "Host name of the RAWhoisd server"},\
     {"-p", ARGV_FUNC, (char *) &IRR::ArgvPort,      (char *) NULL,\
      "Port number of the RAWhoisd server"},\
     {"-s", ARGV_FUNC, (char *) &IRR::ArgvSources,   (char *) NULL,\
      "Order of databases"},\
     {"-f", ARGV_FUNC, (char *) &IRR::ArgvInitCache, (char *) NULL,\
      "A file name containing only RPSL aut-num objects"},\
     {"-protocol", ARGV_FUNC, (char *)&IRR::ArgvProtocol, (char *)NULL,\
      "Protocol to be used to connect to the server"},\
     {"-ignore_errors", ARGV_FUNC, (char *)&IRR::IgnoreErrors, (char *)NULL,\
      "Ignore IRR error and warning messages"},\
     {"-report_errors", ARGV_FUNC, (char *)&IRR::ReportErrors, (char *)NULL,\
      "Print IRR error and warning messages"}

#endif // _IRR_HH
