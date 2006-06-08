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

#include "config.h"

#ifndef _RAWHOISC_HH
#define _RAWHOISC_HH

#include <cstdio>
#include <cstdarg>

#include "irr.hh"
#include "birdwhoisc.hh"
#include "util/Error.hh"

class RAWhoisClient : public IRR { 
// Whois Client that talks to RAWhoisd server
private:
   char  *current_sources;
   int version;

protected:
   int _is_open;
   FILE *in;
   FILE *out;

   char last_query[256];

// Made them all private to make sure no applications would use them directly,
// instead, some common APIs should be used to enforce various whois clients
// are portable -- wlee@isi.edu
private:
   void WriteQuery(const char *format, va_list ap);
   // Query using printf style arguments
   void Query(const char *format, ...);

   // Response allocates enough space to hold the result
   int  Response(char *&response);

   // do a Query and Response, returning the AXX and CXX lines
   // in addition to the the response.
   int TotalResponse (char *&response);

   // do a Query and Response
   int  QueryResponse(char *&response, const char *format, ...);

   // do a Query and Response but kill the reply
   int  QueryKillResponse(const char *format, ...);

   bool is_rpslng() {
     if (version >= 22) // RPSLng versions of irrd start from 2.2
        return true;
     return false;
   }

public:
   Error error;

   RAWhoisClient() {
      error.immediate(1);
      error.ignore(ignore_errors);
      _is_open = 0;
      current_sources = NULL;
   };
   ~RAWhoisClient() {
      if (_is_open)
	 Close();
      if (current_sources) delete []current_sources;
   }

   int is_open() {
      return _is_open;
   }

   int in_fileno () { return fileno(in); } 
   int out_fileno() { return fileno(out); }

   void Open(const char *_host = dflt_host, 
	     const int _port = dflt_port, 
	     const char *_sources = dflt_sources);

   void Close();

   void CheckConnection();

   void SetSources(const char *_sources = dflt_sources);

   // Modified by wlee@isi.edu
   //   char *GetSources(void);
   const char *GetSources(void);

   void GetVersion();

   // PendingData() returns true if there is data available for reading
   int  PendingData(); 

   virtual bool getAutNum(char *as,          char *&text, int &len);
   virtual bool getSet(SymID sname, char *clss, char *&text, int &len);
   virtual bool getRoute(char *rt, char *as, char *&text, int &len);
   virtual bool getInetRtr(SymID inetrtr,    char *&text, int &len);
   virtual bool expandASSet(SymID asset,     SetOfUInt    *result);
   // REIMP
   virtual bool expandAS(char *as,           MPPrefixRanges *result);
   virtual bool expandRSSet(SymID rsset,     MPPrefixRanges *result);
   virtual bool expandRtrSet(SymID rsset,    MPPrefixRanges *result);

   // Added by wlee@isi.edu for roe basically
   void setFullObjectFlag(bool onoff) {
     if (onoff)
       QueryKillResponse("!ufo=1");
     else
       QueryKillResponse("!ufo=0");
   }
   void setFastResponseFlag(bool onoff) {
     if (onoff)
       QueryKillResponse("!uF=1");
     else
       QueryKillResponse("!uF=0");
   }
   int getSourceOrigin(char *&buffer, const char *rt) {
     return QueryResponse(buffer, "!r%s,o", rt);
   }
   int getSourceOrigin(char *&buffer) {
     return Response(buffer);
   }
   void querySourceOrigin(const char *rt) {
     Query("!r%s,o", rt);
   }
   bool readReady(void) {
     return PendingData();
   }
   ASt getOrigin(char *format, ...);
};

#endif // _RAWHOISC_HH
