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
#include <cstdio>
#include <fstream>
#include <iostream>
#include "gnu/prefixranges.hh"
#include "gnug++/SetOfUInt.hh"
#include "irr.hh"
#include "autnum.hh"
#include "route.hh"
#include "cache.hh"
#include "classes.hh"

using namespace std;

IRR *irr;
ProtocolName protocolName[] = {
  { IRR::rawhoisd, "rawhoisd" },
  { IRR::ripe,     "ripe_perl" },
  { IRR::bird,     "ripe" },
  { IRR::bird,     "bird" },
  { IRR::unknown,  "unknown" }
};

char          IRR::buffer[128];
char          IRR::protocol[64] = "irrd";
char const   *IRR::dflt_host      = "whois.radb.net";
char const   *IRR::dflt_sources   = "";
int           IRR::dflt_port      = 43;
IRR::PROTOCOL IRR::dflt_protocol  = rawhoisd;
bool          IRR::ignore_errors  = false;

////// Options //////////////////////////////////////////////////////

void IRR::SetDefaultHost(const char *_host) {
   dflt_host = _host;
}

void IRR::SetDefaultSources(const char *_sources) {
   dflt_sources = _sources;
}

void IRR::SetDefaultPort(const int _port) {
   dflt_port = _port;
}

void IRR::SetDefaultProtocol(const char *_protocol) {
  strcpy(protocol, _protocol);
  if (strcasecmp(_protocol, "rawhoisd") == 0)
    dflt_protocol = rawhoisd;
  else
    if (strcasecmp(_protocol, "irrd") == 0)
      dflt_protocol = rawhoisd;
    else
      if (strcasecmp(_protocol, "ripe_perl") == 0)
	dflt_protocol = ripe;
      else
	 if (strcasecmp(_protocol, "ripe") == 0)
	    dflt_protocol = bird;
	 else
	    if (strcasecmp(_protocol, "bird") == 0)
	       dflt_protocol = bird;
	    else {
	       cerr << "Error: unknown irr protocol " << _protocol 
		    << ", using irrd" << endl;
	       cerr << "Error: known protocols: irrd(rawhoisd), ripe(bird), ripe_perl" << endl;
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
   }
}

////// Get Objects //////////////////////////////////////////////////////

Cache<ASt,   AutNum *>       AutNumCache;
Cache<SymID, Set *>          SetCache;
Cache<SymID, InetRtr *>      InetRtrCache;
Cache<SymID, SetOfUInt *>    expandASSetCache;
Cache<ASt,   MPPrefixRanges *> expandASCache;
Cache<SymID, MPPrefixRanges *> expandRSSetCache;
Cache<SymID, MPPrefixRanges *> expandRtrSetCache;

void IRR::initCache(char *objectText, int objectLength, char *clss) {
   Buffer b(objectText, objectLength);
   Set *o = new Set(b);
   Set *result = NULL;
   if (! o->has_error) {
      AttrGenericIterator<ItemSID> itr(o, clss);
      
      const ItemSID *sname = itr.first();
      
      if (!sname)
	 delete o;
      else {
	 if (! SetCache.query(sname->name, result)) {
	    SetCache.add(sname->name, o);
	 } else
	    delete o;
      }      
   } else
      delete o;
}

void IRR::initCache(const char *fname) {
   if (!fname || !*fname)
      return;

   ifstream in(fname);
   if (!in)
      return;

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
	 initCache(objectText, objectLength, "as-set");
      else if (strstr(objectText, "route-set"))
	 initCache(objectText, objectLength, "route-set");
      else if (strstr(objectText, "rtr-set"))
	 initCache(objectText, objectLength, "rtr-set");
      else if (strstr(objectText, "filter-set"))
	 initCache(objectText, objectLength, "filter-set");
      else if (strstr(objectText, "peering-set"))
	 initCache(objectText, objectLength, "peering-set");
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
  }
   }
}

const AutNum *IRR::getAutNum(ASt as) {
   char *text;
   int  len;
   AutNum *result = NULL;

   if (! AutNumCache.query(as, result)) {
      sprintf(buffer, "AS%d", as);
      if (getAutNum(buffer, text, len)) {
	 Buffer b(text, len);
	 result = new AutNum(b);
	 AutNumCache.add(as, result);
      } else
	 AutNumCache.add(as, NULL); // a negative object
   }

   return result;
}

const Set *IRR::getSet(SymID sname, char *clss) {
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

   sprintf(buffer, "AS%d", as);
   if (getRoute(rt->get_text(), buffer, text, len)) {
      Buffer b(text, len);
      route = new Route(b);
   } else
      route = NULL;
}

// Added by wlee
void IRR::getRoute(Route *&route, char *rt, ASt as)
{
  char *text;
  int  len;

  sprintf(buffer, "AS%d", as);
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

const MPPrefixRanges *IRR::expandAS(ASt as) {
   MPPrefixRanges *result;
   MPPrefix prfx;
   char *text;
   int  len;

   if (! expandASCache.query(as, result)) {
      result = new MPPrefixRanges;
      // we insert the set to the cache before expanding
      // this is needed to avoid recursion if sets are recursively defined
      expandASCache.add(as, result);
      sprintf(buffer, "AS%d", as);
      if (!expandAS(buffer, result)) {
   expandASCache.nullify(as);
  delete result;
  result = NULL; // A negative cache
      }
   }

   return result;
}

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
#include "ripewhoisc.hh"
#include "birdwhoisc.hh"

IRR *IRR::newClient() {
   switch (dflt_protocol) {
   case rawhoisd:
      return new RAWhoisClient;
   case ripe:
      return NULL; //new RipeWhoisClient;
   case bird:
      return new BirdWhoisClient;
   default:
      return NULL;
   }
}

