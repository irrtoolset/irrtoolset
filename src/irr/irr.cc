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

#include "config.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include "dataset/prefixranges.hh"
#include "dataset/SetOfUInt.hh"
#include "irr.hh"
#include "autnum.hh"
#include "route.hh"
#include "cache.hh"
#include "classes.hh"

using namespace std;

IRR *irr;
ProtocolName protocolName[] = {
  { IRR::rawhoisd, "rawhoisd" },
#ifdef RIPE
  { IRR::ripe,     "ripe" },
#else
  { IRR::bird,     "ripe" },
#endif /* RIPE */
  { IRR::bird,     "bird" },
  { IRR::unknown,  "unknown" }
};

char          IRR::buffer[128];
char          IRR::protocol[64] = "irrd";
char const   *IRR::dflt_host      = "whois.radb.net";
char const   *IRR::proxy_host     = "";
char const   *IRR::dflt_sources   = "";
int           IRR::dflt_port      = 43;
int           IRR::proxy_port     = 8086;
IRR::PROTOCOL IRR::dflt_protocol  = rawhoisd;
bool          IRR::ignore_errors  = false;
bool          IRR::use_proxy_connection = false;
#ifdef DIAG
bool          IRR::irr_warnings   = false;
Buffer        IRR::rpslInfo(1024);
#endif /* DIAG */
#ifdef RPKI
char const   *IRR::rpkiSymbolFormat = "";
int           IRR::rpkiExpansionMode = -1;
#endif /* RPKI */
#ifdef RIPE
int           IRR::responseRetries = 7;
int           IRR::responseTimeout = 3;
#endif /* RIPE */

////// Options //////////////////////////////////////////////////////

void IRR::SetDefaultHost(const char *_host) {
   dflt_host = _host;
}

void IRR::SetProxyHost(const char *_proxyHost) {
   proxy_host = _proxyHost;
   use_proxy_connection = true;
}

void IRR::SetDefaultSources(const char *_sources) {
   dflt_sources = _sources;
}

void IRR::SetDefaultPort(const int _port) {
   dflt_port = _port;
}

void IRR::SetProxyPort(const int _proxyPort) {
   proxy_port = _proxyPort;
}

void IRR::SetDefaultProtocol(const char *_protocol) {
  strcpy(protocol, _protocol);
  if (strcasecmp(_protocol, "rawhoisd") == 0)
    dflt_protocol = rawhoisd;
  else
    if (strcasecmp(_protocol, "irrd") == 0)
      dflt_protocol = rawhoisd;
    else
	 if (strcasecmp(_protocol, "ripe") == 0)
#ifdef RIPE
	    dflt_protocol = ripe;
#else
	    dflt_protocol = bird;
#endif /* RIPE */
	 else
	    if (strcasecmp(_protocol, "bird") == 0)
	       dflt_protocol = bird;
	    else {
	       cerr << "Error: unknown irr protocol " << _protocol << ", using irrd" << endl;
#ifdef RIPE
	       cerr << "Error: known protocols: irrd(rawhoisd), ripe, bird" << endl;
#else
	       cerr << "Error: known protocols: irrd(rawhoisd), ripe(bird)" << endl;
#endif /* RIPE */
	       dflt_protocol =  rawhoisd;
	    }
}

int IRR::ArgvHost(char *dst, char *key, const char *_nextArg) {
   if (_nextArg)
      {
      SetDefaultHost(_nextArg);
      return 1; // return 1 to signify nextArg is used by us
      }
   return 0;
}

int IRR::ArgvProxyHost(char *dst, char *key, const char *_nextArg) {
   if (_nextArg) {
      SetProxyHost(_nextArg);
      return 1; // return 1 to signify nextArg is used by us
   }
   return 0;
}

int IRR::ArgvSources(char *dst, char *key, const char *_nextArg) {
   if (_nextArg)
      {
      SetDefaultSources(_nextArg);
      return 1; // return 1 to signify nextArg is used by us
      }
   return 0;
}

int IRR::ArgvPort(char *dst, char *key, const char *_nextArg) {
   if (_nextArg)
      {
      SetDefaultPort(atoi(_nextArg));
      return 1; // return 1 to signify nextArg is used by us
      }
   return 0;
}

int IRR::ArgvProxyPort(char *dst, char *key, const char *_nextArg) {
   if (_nextArg) {
      SetProxyPort(atoi(_nextArg));
      return 1; // return 1 to signify nextArg is used by us
   }
   return 0;
}
 
int IRR::ArgvInitCache(char *dst, char *key, const char *_nextArg) {
   if (_nextArg)
      {
      initCache(_nextArg);
      return 1; // return 1 to signify nextArg is used by us
      }
   return 0;
}

int IRR::ReportErrors(char *dst, char *key, const char *_nextArg) {
   ignore_errors = false;
   return 0; // return 0 to signify nextArg is not used by us
}

int IRR::IgnoreErrors(char *dst, char *key, const char *_nextArg) {
   ignore_errors = true;
   return 0; // return 0 to signify nextArg is not used by us
}

#ifdef DIAG
int IRR::DBWarnings(char *dst, char *key, const char *_nextArg) {
   irr_warnings = true;
   return 0; // return 0 to signify nextArg is not used by us
}
#endif /* DIAG */

int IRR::ArgvProtocol(char *dst, char *key, const char *_nextArg)
{
   if (_nextArg) {
      SetDefaultProtocol(_nextArg);
      return 1;
   } else {
      dflt_protocol = rawhoisd;
      strcpy(protocol, "irrd");
      return 0;
   }
}

#ifdef RPKI
int IRR::ArgvInitRPKICache(char *dst, char *key, const char *_nextArg) {
	if (_nextArg) {
		initRPKICache(_nextArg);
		return 1;
	}
	return 0;
}

int IRR::ArgvRPKISymbolFormat(char *dst, char *key, const char *_nextArg) {
	if (_nextArg) {
		setRPKISymbolFormat(_nextArg);
		return 1;
	}
	return 0;
}

int IRR::ArgvRPKIExpansionMode(char *dst, char *key, const char *_nextArg) {
	if (_nextArg) {
		setRPKIExpansionMode(_nextArg);
		return 1;
	}
	return 0;
}
#endif /* RPKI */

#ifdef RIPE
int IRR::ArgvResponseRetries(char *dst, char *key, const char *_nextArg) {
	if (_nextArg) {
		setResponseRetries(atoi(_nextArg));
		return 1; // return 1 to signify nextArg is used by us
	}
	return 0;
}

int IRR::ArgvResponseTimeout(char *dst, char *key, const char *_nextArg) {
	if (_nextArg) {
		setResponseTimeout(atoi(_nextArg));
		return 1; // return 1 to signify nextArg is used by us
	}
	return 0;
}
#endif /* RIPE */

void IRR::handleEnvironmentVariables(char **envp) {

   for (char **p = envp; *p != NULL; p++) {
      if (strncmp(*p, "IRR_HOST=", 9) == 0)  {
	 SetDefaultHost(*p + 9);
         continue;
      }
      if (strncmp(*p, "IRR_PORT=", 9) == 0)  {
	 SetDefaultPort(atoi(*p + 9));
         continue;
      }
      if (strncmp(*p, "IRR_SOURCES=", 12) == 0)  {
	 SetDefaultSources(*p + 12);
         continue;
      }
      if (strncmp(*p, "IRR_PROTOCOL=", 13) == 0)  {
	 SetDefaultProtocol(*p + 13);
         continue;
      } 
      if (strncmp(*p, "IRR_PROXY_HOST=", 15) == 0)  {
	 SetProxyHost(*p + 15);
         continue;
      }
      if (strncmp(*p, "IRR_PROXY_PORT=", 15) == 0)  {
	 SetProxyPort(atoi(*p + 15));
         continue;
      }
   }
}

////// Get Objects //////////////////////////////////////////////////////

Cache<ASt,   AutNum *>       AutNumCache;
Cache<SymID, Set *>          SetCache;
Cache<SymID, InetRtr *>      InetRtrCache;
Cache<SymID, SetOfUInt *>    expandASSetCache;
Cache<ASt,   MPPrefixRanges *> expandASCache;
#ifdef RPKI
Cache<ASt,   MPPrefixRanges *> expandRPKICache;
#endif /* RPKI */
Cache<SymID, MPPrefixRanges *> expandRSSetCache;
Cache<SymID, MPPrefixRanges *> expandRtrSetCache;

bool IRR::queryCache(SymID setID, Set *&set) {
  return (SetCache.query(setID, set));
}

void IRR::initCache(char *objectText, int objectLength, const char *clss, const char *fname) {
   Buffer b(objectText, objectLength);
   Set *o = new Set(b);
   Set *result = NULL;
   if (! o->has_error) {
      AttrGenericIterator<ItemSID> itr(o, clss);
      
      const ItemSID *sname = itr.first();
      
      if (!sname) {
	 	 delete o;
#ifdef DIAG
		 cerr << "Error: could not identify name of " << clss
			  << " object found in cache file \"" << fname << "\":\n" << objectText << endl;
         exit(-1);
#endif /* DIAG */
      } else {
	 	 if (! SetCache.query(sname->name, result)) {
	        SetCache.add(sname->name, o);
	     } else {
#ifdef DIAG
	  	    cerr << "Warning: cache file " << clss << " object \"" << sname->name << "\" defined at least twice!\n"
	             << "Warning: second definition found in cache file \"" << fname << "\" ignored!!\n";
#endif /* DIAG */
	        delete o;
	     }
      }
   } else {
      delete o;
#ifdef DIAG
      cerr << "Error: an error occurred when parsing the following " << clss
    	   << " object found in cache file \"" << fname << "\":\n" << objectText << endl;
      exit(-1);
#endif /* DIAG */
   }
}

void IRR::initCache(const char *fname) {
   if (!fname || !*fname) {
#ifdef DIAG
	  cerr << "Error: name of IRR cache file missing" << endl;
      exit(-1);
#else
      return;
#endif /* DIAG */
   }

   ifstream in(fname);
   if (!in) {
#ifdef DIAG
	  cerr << "Error: IRR cache file \"" << fname << "\" not found" << endl;
      exit(-1);
#else
      cerr << "Warning: Could not open '" << fname << "' for reading" << endl;
      return;
#endif /* DIAG */
   }

   bool code = true;
   char *objectText;
   int objectLength;

   while (in) {
      objectText = (char *) NULL;
      Buffer b;
      code = Object::read(b, in);
      objectText = b.contents;
      objectLength = b.size;
      if (! code) {
	      if (! objectText)
	         free(objectText);
        break;
      }

#ifdef DTAG
      if (strncmp(objectText, "aut-num:", 8) == 0) {
	 AutNum *o = new AutNum(b);
	 AutNum *result = NULL;
	 if (! o->has_error) {
	    AttrGenericIterator<ItemASNO> itr(o, "aut-num");

	    const ItemASNO *asn = itr.first();

	    if (!asn)
	       delete o;
	    else {
	       if (! AutNumCache.query(asn->asno, result)) {
		  AutNumCache.add(asn->asno, o);
	       }
	    }
	 } else
	    delete o;
      } else if (strncmp(objectText, "as-set:", 7) == 0)
	initCache(objectText, objectLength, "as-set", fname);
      else if (strncmp(objectText, "route-set:", 10) == 0)
	initCache(objectText, objectLength, "route-set", fname);
      else if (strncmp(objectText, "rtr-set:", 8) == 0)
	initCache(objectText, objectLength, "rtr-set", fname);
      else if (strncmp(objectText, "filter-set:", 11) == 0)
	initCache(objectText, objectLength, "filter-set", fname);
      else if (strncmp(objectText, "peering-set:", 12) == 0)
	initCache(objectText, objectLength, "peering-set", fname);
      else if (strncmp(objectText, "inet-rtr:", 9) == 0) {
	 InetRtr *o = new InetRtr(b);
	 InetRtr *result = NULL;
	 if (! o->has_error) {
	    AttrGenericIterator<ItemDNS> itr(o, "inet-rtr");

	    const ItemDNS *rtrname = itr.first();

	    if (!rtrname)
	       delete o;
	    else {
	       SymID sid = symbols.symID(rtrname->name);
	       if (! InetRtrCache.query(sid, result)) {
	          InetRtrCache.add(sid, o);
	       }
	    }
	 } else
	    delete o;
#else
      if (strstr(objectText, "aut-num")) {
	 AutNum *o = new AutNum(b);
	 AutNum *result = NULL;
	 if (! o->has_error) {
	    AttrGenericIterator<ItemASNO> itr(o, "aut-num");

	    const ItemASNO *asn = itr.first();

	    if (!asn)
	       delete o;
	    else {
	       if (! AutNumCache.query(asn->asno, result)) {
		  AutNumCache.add(asn->asno, o);
	       }
	    }      
	 } else
	    delete o;
      } else if (strstr(objectText, "as-set")) 
	 initCache(objectText, objectLength, "as-set", fname);
      else if (strstr(objectText, "route-set"))
	 initCache(objectText, objectLength, "route-set", fname);
      else if (strstr(objectText, "rtr-set"))
	 initCache(objectText, objectLength, "rtr-set", fname);
      else if (strstr(objectText, "filter-set"))
	 initCache(objectText, objectLength, "filter-set", fname);
      else if (strstr(objectText, "peering-set"))
	 initCache(objectText, objectLength, "peering-set", fname);
      else if (strstr(objectText, "inet-rtr")) {
	 InetRtr *o = new InetRtr(b);
	 InetRtr *result = NULL;
	 if (! o->has_error) {
	    AttrGenericIterator<ItemDNS> itr(o, "inet-rtr");

	    const ItemDNS *rtrname = itr.first();

	    if (!rtrname)
	       delete o;
	    else {
	       SymID sid = symbols.symID(rtrname->name);
	       if (! InetRtrCache.query(sid, result)) {
	          InetRtrCache.add(sid, o);
	       }
	    }      
	 } else
	    delete o;
#endif /* DTAG */
      } else {
#ifdef DIAG
        cerr << "Error: could not classify cache object found in cache file \""
             << fname << "\"\n" << objectText << endl;
        exit(-1);
#else
        cerr << "Warning: could not identify cache object found in cache-file \""
             << fname << "\"\n" << objectText << endl;
#endif /* DIAG */
      }
   }
}

#ifdef RPKI
void IRR::initRPKICache(const char *fname) {
    if (!fname || !*fname) {
#ifdef DIAG
      cerr << "Error: name of RPKI expansion file missing" << endl;
      exit(-1);
#else
      return;
#endif /* DIAG */
    }

    ifstream in(fname);
    if (!in) {
#ifdef DIAG
  	  cerr << "Error: RPKI expansion file \"" << fname << "\" not found" << endl;
      exit(-1);
#else
      cerr << "Warning: Could not open '" << fname << "' for reading" << endl;
      return;
#endif /* DIAG */
    }

    char line[2049];
    while (in) {
       line[0] = '\0';
       in.getline(line, 2048);

	   if (strncmp(line, "AS", 2) == 0) {
	      char *as = strtok(line, " \t");
		  ASt asnr = atoi(as+2);
	      if (asnr > 0) {
	    	 MPPrefixRanges *result = NULL;
	    	 MPPrefixRanges *pref_list = new MPPrefixRanges;

	    	 if (! expandRPKICache.query(asnr, result)) {
	            result = new MPPrefixRanges;
		        expandRPKICache.add(asnr, result);
	    	 }
	         for (char *prefix = strtok(NULL, " \t"); prefix; prefix = strtok(NULL, " \t")) {
		    	pref_list->push_back(MPPrefix(prefix));
	         }

	         result->append_list(pref_list);
	         delete pref_list;
	      } else {
#ifdef DIAG
	        cerr << "Error: wrong AS number found in RPKI expansion file \""
	             << fname << "\"\n" << line << endl;
	        exit(-1);
#else
            cerr << "Warning: wrong AS number found in RPKI expansion file \""
	             << fname << "\"\n" << line << endl;
#endif /* DIAG */
	      }
	   }
    }
}

void IRR::setRPKISymbolFormat(const char *format) {
	if (strstr(format, "%u") != NULL) {
		rpkiSymbolFormat = format;
	}
	else {
		cerr << "Error: missing '%u' in RPKI symbol format \"" << format << "\", searching disabled" << endl;
#ifdef DIAG
	    exit(-1);
#else
		rpkiSymbolFormat = "";
#endif /* DIAG */
	}
}

void IRR::setRPKIExpansionMode(const char *mode) {
	if (strcmp(mode, "first") == 0) {
		rpkiExpansionMode = 0;
	}
	else if (strcmp(mode, "rpki") == 0) {
		rpkiExpansionMode = 1;
	}
	else if (strcmp(mode, "rpsl") == 0) {
		rpkiExpansionMode = 2;
	}
	else if (strcmp(mode, "all") == 0) {
		rpkiExpansionMode = -1;
	}
	else {
#ifdef DIAG
		cerr << "Error: RPKI expansion mode \"" << mode << "\" is not supported!" << endl;
	    exit(-1);
#else
		cerr << "Warning: RPKI expansion mode \"" << mode << "\" is not supported, using \"all\"" << endl;
		rpkiExpansionMode = -1;
#endif /* DIAG */
	}
}
#endif /* RPKI */

#ifdef RIPE
void IRR::setResponseRetries(const int num) {
	if (num < 0) {
#ifdef DIAG
		cerr << "Error: number of response retries < 0!" << endl;
	    exit(-1);
#else
		cerr << "Warning: number of response retries < 0, using 0!" << endl;
		responseRetries = 0;
#endif /* DIAG */
	}
	else {
		responseRetries = num;
	}
}

void IRR::setResponseTimeout(const int sec) {
	if (sec < 0) {
#ifdef DIAG
		cerr << "Error: response timeout seconds < 0!" << endl;
	    exit(-1);
#else
		cerr << "Warning: response timeout seconds < 0, using 0!" << endl;
		responseTimeout = 0;
#endif /* DIAG */
	}
	else {
		responseTimeout = sec;
	}
}
#endif /* RIPE */

const AutNum *IRR::getAutNum(ASt as) {
   char *text;
   int  len;
   AutNum *result = NULL;

   if (! AutNumCache.query(as, result)) {
      asnum_string_plain(buffer, as);	// IRRDBs don't like asdot
      if (getAutNum(buffer, text, len)) {
         Buffer b(text, len);
         result = new AutNum(b);
         AutNumCache.add(as, result);
      } else
         AutNumCache.add(as, NULL); // a negative object
   }

   return result;
}

const Set *IRR::getSet(SymID sname, const char *clss) {
   char *text;
   int  len;
   Set *result = NULL;

   if (! SetCache.query(sname, result)) {
      if (getSet(sname, clss, text, len)) {
	 Buffer b(text, len);
	 result = new Set(b);
	 SetCache.add(sname, result);
      } else
	 SetCache.add(sname, NULL); // a negative cache result
   }

   return result;  
}

const ASSet *IRR::getASSet(SymID sname) {
   return getSet(sname, "as-set");
}

const RSSet *IRR::getRSSet(SymID sname) {
   return getSet(sname, "route-set");
}

const RtrSet *IRR::getRtrSet(SymID sname) {
   return getSet(sname, "rtr-set");
}

const FilterSet *IRR::getFilterSet(SymID sname) {
   return getSet(sname, "filter-set");
}

const PeeringSet *IRR::getPeeringSet(SymID sname) {
   return getSet(sname, "peering-set");
}

void IRR::getRoute(Route *&route, Prefix *rt, ASt as) {
   char *text;
   int  len;

   asnum_string_plain(buffer, as);
   if (getRoute(rt->get_text(), buffer, text, len)) {
      Buffer b(text, len);
      route = new Route(b);
   } else
      route = NULL;
}

void IRR::getRoute(Route *&route, char *rt, ASt as)
{
  char *text;
  int  len;

  asnum_string_plain(buffer, as);
  if (getRoute(rt, buffer, text, len)) {
     Buffer b(text, len);
     route = new Route(b);
  } else
     route = NULL;
}

const InetRtr *IRR::getInetRtr(SymID inetRtr)
{
   char *text;
   int  len;
   InetRtr *result = NULL;

   if (! InetRtrCache.query(inetRtr, result)) {
      if (getInetRtr(inetRtr, text, len)) {
	      Buffer b(text, len);
	      result = new InetRtr(b);
	      InetRtrCache.add(inetRtr, result);
      } else {
	     InetRtrCache.add(inetRtr, NULL); // a negative object
      }
   }

   return result;  
}

////// Expand Sets //////////////////////////////////////////////////////

#ifdef RPKI
// rpkiExpansionMode
//     0  first
//     1  rpki
//     2  rpsl
//    -1  all
//
const MPPrefixRanges *IRR::expandAS(ASt as) {
   MPPrefixRanges *rpki_result = NULL;
   MPPrefixRanges *rpsl_result = NULL;

   // RPKI expansion (mode first / rpki / all)
   if (rpkiExpansionMode != 2) {
	  if (! expandRPKICache.query(as, rpki_result)) {
		 if (strlen(rpkiSymbolFormat) > 0) {
			sprintf(buffer, rpkiSymbolFormat, as);
			SymID rpki_symbol = symbols.symID(buffer);

			rpki_result = (MPPrefixRanges *) IRR::expandRSSet(rpki_symbol);
			if (rpki_result && rpki_result->empty()) {
				rpki_result = NULL;
			}
		 }
	  }
	  // expansion mode rpki / first
	  if ((rpkiExpansionMode == 1) ||
		  (rpki_result && (rpkiExpansionMode == 0))) {
		 return rpki_result;
	  }
   }

   // RPSL expansion (mode first / rpsl / all)
   if (rpkiExpansionMode != 1) {
	  if (! expandASCache.query(as, rpsl_result)) {
		 rpsl_result = new MPPrefixRanges;
	     // we insert the set to the cache before expanding
	     // this is needed to avoid recursion if sets are recursively defined
	     expandASCache.add(as, rpsl_result);
	     asnum_string_plain(buffer, as);
	     if (!expandAS(buffer, rpsl_result)) {
	        expandASCache.nullify(as);
	        delete rpsl_result;
	        rpsl_result = NULL; // A negative cache
	     }
	  }
	  // expansion mode rpsl / first
	  if ((rpkiExpansionMode == 2) ||
		  (rpsl_result && (rpkiExpansionMode == 0))) {
		 return rpsl_result;
	  }
   }

   // expansion mode all
   if (rpki_result) {
	  if (rpsl_result) {
		 rpki_result->append_list(rpsl_result);
	  }
	  return rpki_result;
   }
   return rpsl_result;
}
#else
const MPPrefixRanges *IRR::expandAS(ASt as) {
   MPPrefixRanges *result;
   MPPrefix prfx;

   if (! expandASCache.query(as, result)) {
      result = new MPPrefixRanges;
      // we insert the set to the cache before expanding
      // this is needed to avoid recursion if sets are recursively defined
      expandASCache.add(as, result);
      asnum_string_plain(buffer, as);
      if (!expandAS(buffer, result)) {
         expandASCache.nullify(as);
         delete result;
         result = NULL; // A negative cache
      }
   }

   return result;
}
#endif /* RPKI */

const SetOfUInt *IRR::expandASSet(SymID asset) {
   SetOfUInt *result;

   if (! expandASSetCache.query(asset, result)) {
      result = new SetOfUInt;
      // we insert the set to the cache before expanding
      // this is needed to avoid recursion if sets are recursively defined
      expandASSetCache.add(asset, result); 
      if (!expandASSet(asset, result)) {
	 expandASSetCache.nullify(asset);
	 delete result;
	 result = NULL; // A negative cache
      }
   }

   return result;
}

const MPPrefixRanges *IRR::expandRSSet(SymID rsset) {
   MPPrefixRanges *result;

   if (! expandRSSetCache.query(rsset, result)) {
      result = new MPPrefixRanges;
      // we insert the set to the cache before expanding
      // this is needed to avoid recursion if sets are recursively defined
      expandRSSetCache.add(rsset, result);
      if (!expandRSSet(rsset, result)) {
        expandRSSetCache.nullify(rsset);
        delete result;
        result = NULL; // A negative cache
      }
   }
   return result;
}

const MPPrefixRanges *IRR::expandRtrSet(SymID rtrset) {
   MPPrefixRanges *result;

   if (! expandRtrSetCache.query(rtrset, result)) {
      result = new MPPrefixRanges;
      // we insert the set to the cache before expanding
      // this is needed to avoid recursion if sets are recursively defined
      expandRtrSetCache.add(rtrset, result);
      if (!expandRtrSet(rtrset, result)) {
   expandRtrSetCache.nullify(rtrset);
   delete result;
   result = NULL; // A negative cache
      }
   }      

   return result;
} 

void IRR::expandItem(Item *pt, MPPrefixRanges *result) {
   const MPPrefixRanges *sr;

   if (typeid(*pt) == typeid(ItemRSNAME)) { // aka route-set
      sr = expandRSSet(((ItemRSNAME *)pt)->name);
      if (sr) {
         result->append_list(sr);
      }
      return;
   }
   if (typeid(*pt) == typeid(ItemASNAME)) { // aka as-set
      const SetOfUInt *s = expandASSet(((ItemASNAME *)pt)->name);
      if (s && ! s->isEmpty()) {
      	 for (Pix p = s->first(); p; s->next(p)) {
      	    sr = expandAS((*s)(p));
      	    if (sr)
      	       result->append_list(sr);
      	 }
      }
      return;
   } 
   if (typeid(*pt) == typeid(ItemASNO)) {
      sr = expandAS(((ItemASNO *)pt)->asno);
      if (sr)
      	 result->append_list(sr);
      return;
   }
   if (typeid(*pt) == typeid(ItemPRFXV4)) {
      result->push_back(* ( new MPPrefix((PrefixRange *) ((ItemPRFXV4 *) pt)->prfxv4) ));
      return;
   } 
   if (typeid(*pt) == typeid(ItemPRFXV4Range)) {
      result->push_back(* ( new MPPrefix((PrefixRange *) ((ItemPRFXV4Range *) pt)->prfxv4) ));
      return;
   } 
   if (typeid(*pt) == typeid(ItemPRFXV6)) {
      result->push_back(* ( new MPPrefix((IPv6PrefixRange *) ((ItemPRFXV6 *) pt)->prfxv6) ));
      return;
   }

   if (typeid(*pt) == typeid(ItemPRFXV6Range)) {
      result->push_back(* ( new MPPrefix((IPv6PrefixRange *) ((ItemPRFXV6Range *) pt)->prfxv6) ));
      return;
   }
   if (typeid(*pt) == typeid(ItemMSItem)) {
      MPPrefixRanges *tmp = new MPPrefixRanges; 
      expandItem(((ItemMSItem *)pt)->item, (MPPrefixRanges *) tmp); // recursion!!!
      MPPrefixRanges::iterator p;
      for (p = tmp->begin(); p != tmp->end(); ++p) {
        if (p->makeMoreSpecific(((ItemMSItem *)pt)->code, 
             ((ItemMSItem *)pt)->n, 
             ((ItemMSItem *)pt)->m))
          result->push_back(*p);
      }
      return;
   }
   if (typeid(*pt) == typeid(ItemIPV4)) { // aka router address
      result->push_back(* ( new MPPrefix((PrefixRange *) ((ItemIPV4 *) pt)->ipv4) ));
      return;
   }
   if (typeid(*pt) == typeid(ItemIPV6)) { // aka router address
      result->push_back(* ( new MPPrefix((IPv6PrefixRange *) ((ItemIPV6 *) pt)->ipv6) ));
      return;
   }
   if (typeid(*pt) == typeid(ItemRTRSNAME)) { // aka rtr-set
      sr = expandRtrSet(((ItemRTRSNAME *)pt)->name);
      if (sr)
      	 result->append_list(sr);
      return;
   }
   if (typeid(*pt) == typeid(ItemDNS)) { // aka inet-rtr name
      const InetRtr *ir = getInetRtr(((ItemDNS *)pt)->name);
      collectIfAddr(result, ir);
      if (ir) {
	      AttrIterator<AttrIfAddr> itr(ir, "ifaddr");
      	 while (itr) {
      	    result->push_back(MPPrefix(*(itr()->ifaddr)));
      	    itr++;
      	 }
        AttrIterator<AttrIfAddr> itr1(ir, "interface");
         while (itr1) {
            result->push_back(MPPrefix(*(itr1()->ifaddr)));
            itr1++; 
         }
      }
      return;
      assert(0);
   }
}

bool IRR::isIndirectMember(Object *o, 
			   AttrGenericIterator<ItemWORD> &mbrs_by_ref) {
   for (AttrGenericIterator<ItemWORD> mntnrs(o, "mnt-by"); mntnrs; mntnrs++)
      for (mbrs_by_ref.first(); mbrs_by_ref; mbrs_by_ref++)
	 if (!strcasecmp(mntnrs()->word, mbrs_by_ref()->word))
	    return true;
   return false;
}

void collectASNO(void *result, const Object *o) {
   ((SetOfUInt *) result)->add(((AutNum *) o)->asno());
}

void collectIfAddr(void *result, const Object *o) {
   AttrIterator<AttrIfAddr> itr(o, "ifaddr");
   while (itr) {
      ((MPPrefixRanges *) result)->push_back(MPPrefix(*(itr()->ifaddr)));
      itr++;
   }
   AttrIterator<AttrIfAddr> itr1(o, "interface");
   while (itr1) {
      ((MPPrefixRanges *) result)->push_back(MPPrefix(*(itr1()->ifaddr)));
      itr1++;
   }
}

void collectPrefix(void *result, const Object *o) {
   ((MPPrefixRanges *) result)->push_back(*((Route *) o)->getPrefix());
}

//////////////////////////////////////////////////////////////////////

#include "rawhoisc.hh"
#include "birdwhoisc.hh"
#ifdef RIPE
#include "ripewhoisc.hh"
#endif /* RIPE */

IRR *IRR::newClient() {
   switch (dflt_protocol) {
   case rawhoisd:
      return new RAWhoisClient;
#ifdef RIPE
   case ripe:
      return new RipeWhoisClient;
#endif /* RIPE */
   case bird:
      return new BirdWhoisClient;
   default:
      return NULL;
   }
}

#ifdef DIAG
void IRR::die_error(const char *txt, int type, bool die) {
   fprintf(stderr, "Error: IRR database communication error!\n");
   switch (type) {
     case 0:      // Connection not established
       fprintf(stderr, "Error: Connection setup failure! host = %s, port = %d, protocol = %s\n",
               host, port, protocolName[dflt_protocol].name);
       break;
     case 1:      // Request failed
       fprintf(stderr, "Error: Request failure! host = %s, port = %d, protocol = %s\n",
               host, port, protocolName[dflt_protocol].name);
       break;
     case 2:      // Response failed
       fprintf(stderr, "Error: Response failure! host = %s, port = %d, protocol = %s\n",
               host, port, protocolName[dflt_protocol].name);
       break;
     default:
       fprintf(stderr, "Error: unknown error type! host = %s port = %d protocol = %s\n",
               host, port, protocolName[dflt_protocol].name);
   }
   fprintf(stderr, "Error: %s\n", txt);
   if (die) exit(1);
}

void IRR::warn_error() {
   warn_occ = true;
   fprintf(stderr, "Warning: %s\n", err_msg);
}

void IRR::clear_warning() {
   delete err_msg;
   err_msg = NULL;
   warn_occ = false;
}

void IRR::extractRPSLInfo(char *query) {
  char *rpslPtr = strstr(query, " AS");
  rpslInfo.append("\t");
  if (rpslPtr != NULL) rpslInfo.append(rpslPtr);
  else rpslInfo.append(query);
  rpslInfo.append("\n");
}
#endif /* DIAG */
