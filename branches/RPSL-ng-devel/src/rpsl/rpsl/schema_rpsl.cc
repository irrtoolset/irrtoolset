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

#include <fstream.h>
#include <cstring>
#include "gnu/std.h"

#include "schema.hh"
#include "object.hh"
#include "rpsl.y.hh"
#include "rptype.hh"

#define NO          0
#define PRS         1
#define MAN         1
#define SNG         1
#define HDN         1
#define KEY         1
#define IDX         1
#define OBS         1


char Schema::dictionary_text[] = "";

#define CMN_ATTRS "
attr:  notify           syntax(email),                                         optional,  multiple
attr:  remarks                                                                 optional,  multiple
attr:  source           syntax(rpsl_word),                                     mandatory, single,   internal
attr:  integrity        syntax(rpsl_word),                                     optional, single,   internal
attr:  changed          optional,  multiple
attr:  deleted                                                                 optional,  single, deleted"

static char base_text[] = "class: dictionary        
attr:  dictionary       syntax(rpsl_word),                                     mandatory, single,   key
attr:  rp-attribute     syntax(special,rp-attribute),                          optional,  multiple
attr:  typedef          syntax(special,typedef),                               optional,  multiple
attr:  protocol         syntax(special,protocol),                              optional,  multiple                   " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup
attr:  tech-c                                                                  mandatory, multiple, lookup
attr:  admin-c                                                                 optional,  multiple, lookup

dictionary: rpsl
descr:   rpsl dictionary
admin-c: Cengiz Alaettinoglu
tech-c: Cengiz Alaettinoglu
mnt-by: MNT-CENGIZ
changed: cengiz@isi.edu 19980324
source: RPS-WG
typedef: ListOfIPv4Prefix list of Address_Prefix
typedef: as_number-as_set_name union as_number, as_set_name
typedef: ListOfas_number list of as_number
typedef: ListOfas_number-as_set_name list of as_number-as_set_name
typedef: ListOfas_set_name  list of as_set_name
typedef: ListOfroute_set_name  list of route_set_name
typedef: as_number-as_set_name-route_set_name-IPv4Prefix
         union as_number, as_set_name, route_set_name, Address_Prefix
typedef: ListOfas_number-as_set_name-route_set_name-IPv4Prefix
         list of as_number-as_set_name-route_set_name-IPv4Prefix
typedef: ListOfIPv6Prefix list of ipv6_Address_Prefix
typedef: as_number-as_set_name-route_set_name-IPv4Prefix-IPv6Prefix
         union as_number, as_set_name, route_set_name, Address_Prefix, ipv6_Address_Prefix
typedef: ListOfas_number-as_set_name-route_set_name-IPv4Prefix-IPv6Prefix
         list of as_number-as_set_name-route_set_name-IPv4Prefix-IPv6Prefix
typedef: ListOfrpsl_word List of rpsl_word
typedef: encapsulation enum[GRE, IPv6inIPv4, IPinIP, DVMRP]
typedef: address_family enum[ipv4, ipv6]
typedef: address_sub_family enum[unicast, multicast]
rp-attribute: # preference, smaller values represent higher preferences
              pref
              operator=(integer[0, 65535])  
rp-attribute: # BGP multi_exit_discriminator attribute
              med    
              operator=(union integer[0, 65535], enum[igp_cost])
              # to set med to the IGP metric: med = igp_cost;
              #operator=(enum[igp_cost])
rp-attribute: # BGP destination preference attribute (dpa)
              dpa    
              operator=(integer[0, 65535])  
rp-attribute: # BGP aspath attribute
              aspath
              # prepends AS numbers from last to first order
              prepend(as_number, ...)
typedef:      # a community value in RPSL is either
              #  - a 4 byte integer
              #  - internet, no_export, no_advertise (see RFC-1997)
              community_elm union
              integer[1, 4294967295],
              enum[internet, no_export, no_advertise]
typedef:      # list of community values { 40, no_export, 3561:70}
              community_list
              list of community_elm
rp-attribute: # BGP community attribute
              community 
              # set to a list of communities
              operator=(community_list)
              # order independent equality comparison
              operator==(community_list)    
              # append community values                              
              operator.=(community_list)
              append(community_elm, ...)
              # delete community values
              delete(community_elm, ...)
              # a filter: true if one of community values is contained
              contains(community_elm, ...)
              # shortcut to contains: community(no_export, {3561,70})
              operator()(community_elm, ...)
rp-attribute: # next hop router in a static route
              next-hop 
              operator=(union ipv4_address, ipv6_address, enum[self])
rp-attribute: # cost of a static route
              cost 
              operator=(integer[0, 65535])
protocol: BGP4
          # as number of the peer router
          MANDATORY asno(as_number)
          # enable flap damping
          OPTIONAL flap_damp()        
          OPTIONAL flap_damp(integer[0,65535],# penalty per flap
                             integer[0,65535],# penalty value for supression
                             integer[0,65535],# penalty value for reuse
                             integer[0,65535],# halflife in secs when up
                             integer[0,65535],# halflife in secs when down
                             integer[0,65535])# maximum penalty
protocol: OSPF
protocol: RIP
protocol: IGRP
protocol: IS-IS
protocol: STATIC
protocol: RIPng
protocol: DVMRP
protocol: PIM-DM
protocol: PIM-SM
protocol: CBT
protocol: MOSPF
protocol: MPBGP
";

static char classes_text[] = "class: mntner
attr:  mntner           syntax(rpsl_word),                                     mandatory, single,   key
attr:  auth             syntax(special, blobs),                                mandatory, multiple
attr:  upd-to           syntax(email),                                         mandatory, multiple
attr:  mnt-nfy          syntax(email),                                         optional,  multiple                   " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup
attr:  tech-c                                                                  optional,  multiple, lookup

class: person
attr:  person                                                                  mandatory, single,   lookup
attr:  nic-hdl          syntax(rpsl_word),                                     mandatory, single,   key
attr:  address                                                                 mandatory, multiple
attr:  phone                                                                   mandatory, multiple
attr:  fax-no                                                                  optional,  multiple
attr:  e-mail           syntax(email),                                         optional,  multiple, lookup           " CMN_ATTRS "
attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup

class: role
attr:  role                                                                    mandatory, single,   lookup
attr:  nic-hdl                                                                 mandatory, single,   key
attr:  address                                                                 mandatory, multiple
attr:  phone                                                                   mandatory, multiple
attr:  fax-no                                                                  optional,  multiple
attr:  e-mail           syntax(email),                                         mandatory, multiple, lookup
attr:  trouble                                                                 optional,  multiple                   " CMN_ATTRS "
attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup
attr:  tech-c                                                                  mandatory, multiple, lookup

class: route
attr:  route            syntax(address_prefix),                                mandatory, single,   key, lookup
attr:  origin           syntax(as_number),                                     mandatory, single,   key, lookup
attr:  withdrawn        syntax(date),                                          optional,  single,   deleted
attr:  member-of        syntax(ListOfroute_set_name),                          optional,  multiple, lookup
attr:  inject           syntax(special, inject),                               optional,  multiple
attr:  components       syntax(special, components),                           optional,  single
attr:  aggr-bndry       syntax(special, aggr-bndry),                           optional,  single
attr:  aggr-mtd         syntax(special, aggr-mtd),                             optional,  single
attr:  export-comps     syntax(special, filter),                         optional,  single
attr:  holes            syntax(ListOfIPv4Prefix),                              optional,  multiple                   " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup
attr:  mnt-lower        syntax(list of rpsl_word),                             optional, multiple, lookup
attr:  mnt-routes       syntax(special, mnt-routes),                           optional, multiple, lookup
attr:  admin-c                                                                 optional,  multiple, lookup
attr:  tech-c                                                                  optional,  multiple, lookup
attr:  cross-nfy        syntax(list of rpsl_word),                             optional,  multiple
attr:  cross-mnt        syntax(list of rpsl_word),                             optional,  multiple

class: route6
attr:  route6            syntax(ipv6_address_prefix),                                mandatory, single,   key, lookup
attr:  origin           syntax(as_number),                                     mandatory, single,   key, lookup
attr:  withdrawn        syntax(date),                                          optional,  single,   deleted
attr:  member-of        syntax(ListOfroute_set_name),                          optional,  multiple, lookup
attr:  inject           syntax(special, v6_inject),                               optional,  multiple
attr:  components       syntax(special, v6_components),                           optional,  single
attr:  aggr-bndry       syntax(special, aggr-bndry),                           optional,  single
attr:  aggr-mtd         syntax(special, aggr-mtd),                             optional,  single
attr:  export-comps     syntax(special, v6_filter),                         optional,  single
attr:  holes            syntax(ListOfIPv6Prefix),                              optional,  multiple                   " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup
attr:  mnt-lower        syntax(list of rpsl_word),                             optional, multiple, lookup
attr:  mnt-routes      syntax(special, mnt-routes),                             optional, multiple, lookup
attr:  admin-c                                                                 optional,  multiple, lookup
attr:  tech-c                                                                  optional,  multiple, lookup
attr:  cross-nfy        syntax(list of rpsl_word),                             optional,  multiple
attr:  cross-mnt        syntax(list of rpsl_word),                             optional,  multiple

class: route-set
attr:  route-set        syntax(route_set_name),                                mandatory, single,   key
attr:  members          syntax(special, rs-members),                           optional,  multiple, lookup 
attr:  mp-members       syntax(special, rs-mp-members),                        optional,  multiple, lookup 
attr:  mbrs-by-ref      syntax(list of rpsl_Word),                             optional,  multiple, lookup           " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup
attr:  tech-c                                                                  mandatory, multiple, lookup

class: as-set
attr:  as-set           syntax(as_set_name),                                   mandatory, single,   key
attr:  members          syntax(list of union as_number, as_set_name),          optional,  multiple, lookup     
attr:  mbrs-by-ref      syntax(list of rpsl_word),                             optional,  multiple, lookup           " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup
attr:  tech-c                                                                  mandatory, multiple, lookup

class: rtr-set
attr:  rtr-set        syntax(rtr_set_name),                                       mandatory, single,   key
attr:  members        syntax(list of union rtr_set_name, dns_name, ipv4_address), optional,  multiple, lookup 
attr:  mp-members     syntax(special, rtr-mp-members), optional,  multiple, lookup 
attr:  mbrs-by-ref    syntax(list of rpsl_Word),                               	  optional,  multiple, lookup           " CMN_ATTRS "
attr:  descr                                                                   	  mandatory, multiple
attr:  mnt-by         syntax(list of rpsl_word),                               	  mandatory, multiple, lookup
attr:  admin-c                                                                 	  mandatory, multiple, lookup
attr:  tech-c                                                                  	  mandatory, multiple, lookup

class: peering-set
attr:  peering-set    syntax(peering_set_name),                                   mandatory, single,   key
attr:  peering        syntax(special, peering),                                   optional, multiple
attr:  mp-peering     syntax(special, mp-peering),                                optional, multiple                   " CMN_ATTRS "
attr:  descr                                                                   	  mandatory, multiple
attr:  mnt-by         syntax(list of rpsl_word),                               	  mandatory, multiple, lookup
attr:  admin-c                                                                 	  mandatory, multiple, lookup
attr:  tech-c                                                                  	  mandatory, multiple, lookup

class: filter-set
attr:  filter-set       syntax(filter_set_name),                               mandatory, single,   key
attr:  filter           syntax(special, filter),                               optional, single
attr:  mp-filter        syntax(special, mp-filter),                            optional, single                       " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup
attr:  tech-c                                                                  mandatory, multiple, lookup

class: aut-num
attr:  aut-num          syntax(as_number),                                     mandatory, single,   key
attr:  as-name          syntax(rpsl_word),                                     mandatory, single,   lookup
attr:  member-of        syntax(List Of AS_set_name),                           optional,  multiple, lookup
attr:  import           syntax(special,import),                                optional,  multiple
attr:  mp-import        syntax(special,mp-import),                             optional,  multiple
attr:  export           syntax(special,export),                                optional,  multiple
attr:  mp-export        syntax(special,mp-export),                             optional,  multiple
attr:  default          syntax(special,default),                               optional,  multiple
attr:  mp-default       syntax(special,mp-default),                            optional,  multiple                   " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup
attr:  mnt-routes       syntax(special, mnt-routes),                           optional,  multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup
attr:  tech-c                                                                  mandatory, multiple, lookup
attr:  cross-nfy        syntax(list of rpsl_word),                             optional,  multiple
attr:  cross-mnt        syntax(list of rpsl_word),                             optional,  multiple

class: inet-rtr
attr:  inet-rtr         syntax(dns_name),                                      mandatory, single,   key
attr:  alias            syntax(dns_name),                                      optional,  multiple, lookup
attr:  local-as         syntax(as_number),                                     mandatory, single,   lookup
attr:  ifaddr           syntax(special,ifaddr),                                mandatory, multiple
attr:  interface        syntax(special,interface),                             optional, multiple
attr:  peer             syntax(special,peer),                                  optional,  multiple
attr:  mp-peer          syntax(special,mp-peer),                               optional,  multiple                   " CMN_ATTRS "
attr:  member-of        syntax(List Of rtr_set_name),                           optional,  multiple, lookup
attr:  descr                                                                   optional,  multiple
attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup
attr:  tech-c                                                                  mandatory, multiple, lookup

class: inetnum
attr:  inetnum          syntax(range of ipv4_address),                         mandatory, single,   key
attr:  netname                                                                 mandatory, single,   lookup
attr:  country                                                                 mandatory, multiple
attr:  rev-srv                                                                 optional,  multiple, lookup
attr:  status                                                                  mandatory, single
attr:  mnt-lower        syntax(ListOfrpsl_word),                               optional,  multiple                   " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup
attr:  tech-c                                                                  mandatory, multiple, lookup

class: domain
attr:  domain           syntax(dns_name),                                      mandatory, single,   key
attr:  sub-dom                                                                 optional,  multiple, lookup
attr:  dom-net                                                                 optional,  multiple
attr:  zone-c                                                                  mandatory, multiple, lookup
attr:  nserver                                                                 optional,  multiple, lookup
attr:  mnt-lower        syntax(ListOfrpsl_word),                               optional,  multiple                   " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup
attr:  tech-c                                                                  mandatory, multiple, lookup

class: limerick
attr:  limerick                                                                mandatory, single,   key
attr:  text                                                                    mandatory, multiple
attr:  author                                                                  mandatory, multiple, lookup           " CMN_ATTRS "
attr:  descr                                                                   optional,  multiple
attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup

class: as-block
attr:  as-block         syntax(union as_number, range of as_number),           mandatory, single,   key
attr:  mnt-lower        syntax(ListOfrpsl_word),                               optional,  multiple                   " CMN_ATTRS "
attr:  descr                                                                   mandatory, multiple
attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup
attr:  admin-c                                                                 mandatory, multiple, lookup
attr:  tech-c                                                                  mandatory, multiple, lookup

class: key-cert
attr:  key-cert       mandatory, single,    key,        syntax(rpsl_word)
attr:  method         mandatory, single,                syntax(rpsl_word)
attr:  owner          mandatory, multiple
attr:  fingerpr       mandatory, single
attr:  certif         mandatory, single
attr:  remarks        optional,  multiple
attr:  source         mandatory, single,   lookup,      syntax(rpsl_word)
attr:  changed        optional,  multiple
attr:  deleted        optional,  single, deleted
attr:  mnt-by         mandatory, multiple, lookup,      syntax(list of rpsl_word)
attr:  notify         optional,  multiple,              syntax(email)

class: peval   
attr:  peval            syntax(special,filter),                                 optional,  single

class: repository
attr:  repository           mandatory,  single,  key,   syntax(rpsl_word)
attr:  query-address        mandatory,  multiple
attr:  response-auth-type   mandatory,  multiple
attr:  submit-address       mandatory,  multiple
attr:  submit-auth-type     mandatory,  multiple
attr:  repository-cert      mandatory,  multiple
attr:  expire               mandatory,  single
attr:  heartbeat-interval   mandatory,  single
attr:  descr                optional,   multiple
attr:  admin-c              mandatory,  multiple, lookup
attr:  tech-c               mandatory,  multiple, lookup
attr:  mnt-by               mandatory,  multiple, lookup " CMN_ATTRS "

";

void Schema::addClass(AttrClass *clss) {
   classes[lastClass++] = clss;
}

void Schema::initializeBase() {
   static bool initializeBaseDone = false;
   if (initializeBaseDone)
      return;
   initializeBaseDone = true;

   AttrClass *classClass = new AttrClass(strdup("class"));
   addClass(classClass);

   classClass->addAttr(new AttrAttr(strdup("class"), ATTR_GENERIC, 
				    RPType::newRPType("rpsl_word"),
				    AttrAttr::LOOKUP));
   classClass->addAttr(new AttrAttr(strdup("attr"), ATTR_ATTR,    
				    (RPType *) NULL, 
				    AttrAttr::MULTIVALUED));

   dictionary = initializeFromString(base_text, "dictionary");
}

List<Object> *Schema::initializeFromFile(char *filename) {
   Object *o;
   const AttrAttr *attr;
   char *class_name;
   bool code = true;
   List<Object> *result = new List<Object>;
   AttrClass *clss = NULL;

   initializeBase();

   ifstream in(filename);
   if (!in)
      return result;

   while (in) {
      o = new Object;
      code = o->read(in);
      if (! code) {
	 delete o;
	 continue;
      }

      code = o->scan();
      if (o->has_error)
	 delete o;
      else {
	 if (! strcasecmp(o->type->name, "class")) {
	    Object *o2 = new Object(*o);
	    AttrGenericIterator<ItemWORD> cname(o2, "class");
	    AttrIterator<AttrAttr>  attrItr(o2, "attr");
	    char *name = cname.first()->word;

	    addClass(clss = new AttrClass(strdup(name)));
	    clss->object = o2;
	    //cerr << "Reading "<< name << " class definition..." << endl;
	    
	    for (attr = attrItr.first(); attr; attr = attrItr.next())
	       clss->addAttr((AttrAttr *) attr);

	 } 

         result->append(o);
      }
   }

   return result;
}

Object *Schema::initializeFromString(char *class_text, char *lookFor) {
   Object *o;
   const AttrAttr *attr;
   char *class_name;
   Object *result = (Object *) NULL;
   AttrClass *clss = NULL;
   int count = 0;
   bool keepO = false;

   initializeBase();

   do {
      count++;
      keepO = false;
      o = new Object;
      if (count == 2 && ! dictionary) {
	 keepO = true;
	 dictionary = o;
      }

      o->scan(class_text, strlen(class_text));

      if (! o->has_error) {
	 if (!strcasecmp(o->type->name, "class")) {
	    keepO = true;
	    AttrGenericIterator<ItemWORD> cname(o, "class");
	    AttrIterator<AttrAttr>  attrItr(o, "attr");
	    char *name = cname.first()->word;

	    addClass(clss = new AttrClass(strdup(name)));
	    clss->object = o;
	    // cerr << "Reading "<< name << " class definition..." << endl;

	    for (attr = attrItr.first(); attr; attr = attrItr.next())
	       clss->addAttr((AttrAttr *) attr);
	 }

	 if (lookFor && ! strcasecmp(o->type->name, lookFor)) {
	    keepO = true;
	    result = o;
	 }
      }

      if (!keepO)
	 delete o;

      class_text = strstr(class_text, "\n\n");
      if (class_text)
	 class_text += 2;
   } while (class_text && *class_text);

   return result;
}

void Schema::initialize() {
   initializeBase();
   initializeFromString(classes_text);
}

static RPSLKeyword rpsl_keywords[] = {
   RPSLKeyword("true",     KEYW_TRUE,     1),
   RPSLKeyword("false",    KEYW_FALSE,    1),
   RPSLKeyword("action",   KEYW_ACTION,   1),
   RPSLKeyword("accept",   KEYW_ACCEPT,   1),
   RPSLKeyword("announce", KEYW_ANNOUNCE, 1),
   RPSLKeyword("from",     KEYW_FROM,     1),
   RPSLKeyword("to",       KEYW_TO,       1),
   RPSLKeyword("at",       KEYW_AT,       1),
   RPSLKeyword("any",      KEYW_ANY,      1),
   RPSLKeyword("protocol", KEYW_PROTOCOL, 1),
   RPSLKeyword("into",     KEYW_INTO,     1),
   RPSLKeyword("refine",   KEYW_REFINE,   1),
   RPSLKeyword("except",   KEYW_EXCEPT,   1),
   RPSLKeyword("static",   KEYW_STATIC,   1),
   RPSLKeyword("networks", KEYW_NETWORKS, 1),
   RPSLKeyword("masklen",  KEYW_MASKLEN,  1),
   RPSLKeyword("peeras",   KEYW_PEERAS,   1),

   RPSLKeyword("atomic",   KEYW_ATOMIC,   1),
   RPSLKeyword("inbound",  KEYW_INBOUND,  1),
   RPSLKeyword("outbound", KEYW_OUTBOUND, 1),

   RPSLKeyword("or",       OP_OR ,        1),
   RPSLKeyword("and",      OP_AND,        1),
   RPSLKeyword("not",      OP_NOT,        1),

   RPSLKeyword("union",        KEYW_UNION,        1),
   RPSLKeyword("range",        KEYW_RANGE,        1),
   RPSLKeyword("list",         KEYW_LIST,         1),
   RPSLKeyword("of",           KEYW_OF,           1),
   RPSLKeyword("operator",     KEYW_OPERATOR,     1),
   RPSLKeyword("upon",         KEYW_UPON,         1),
   RPSLKeyword("have-components", KEYW_HAVE_COMPONENTS, 1),
   RPSLKeyword("exclude",      KEYW_EXCLUDE,      1),

   RPSLKeyword("syntax",       KEYW_SYNTAX,       1),
   RPSLKeyword("special",      KEYW_SPECIAL,      1),
   RPSLKeyword("regexp",       KEYW_REGEXP,       1),
   RPSLKeyword("optional",     KEYW_OPTIONAL,     1),
   RPSLKeyword("mandatory",    KEYW_MANDATORY,    1),
   RPSLKeyword("internal",     KEYW_INTERNAL,     1),
   RPSLKeyword("single",       KEYW_SINGLEVALUED, 1),
   RPSLKeyword("multiple",     KEYW_MULTIVALUED,  1),
   RPSLKeyword("lookup",       KEYW_LOOKUP,       1),
   RPSLKeyword("key",          KEYW_KEY,          1),
   RPSLKeyword("deleted",      KEYW_DELETED,       1),
   
   RPSLKeyword("afi",          KEYW_AFI,           1),
   RPSLKeyword("tunnel",       KEYW_TUNNEL,        1),

   RPSLKeyword(NULL,       0,             0)
};

static RPSLKeyword rpsl_rules[] = {
   RPSLKeyword("changed",           ATTR_CHANGED,           1),
   RPSLKeyword("import",            ATTR_IMPORT,            1),
   RPSLKeyword("mp-import",         ATTR_MP_IMPORT,         1),
   RPSLKeyword("export",            ATTR_EXPORT,            1),
   RPSLKeyword("mp-export",         ATTR_MP_EXPORT,         1),
   RPSLKeyword("default",           ATTR_DEFAULT,           1),
   RPSLKeyword("mp-default",        ATTR_MP_DEFAULT,        1),
   RPSLKeyword("ifaddr",            ATTR_IFADDR,            1),
   RPSLKeyword("interface",         ATTR_INTERFACE,         1),
   RPSLKeyword("peer",              ATTR_PEER,              1),
   RPSLKeyword("mp-peer",           ATTR_MP_PEER,           1),
   RPSLKeyword("rp-attribute",      ATTR_RP_ATTR,           1),
   RPSLKeyword("typedef",           ATTR_TYPEDEF,           1),
   RPSLKeyword("protocol",          ATTR_PROTOCOL,          1),
   RPSLKeyword("filter",            ATTR_FILTER,            1),
   RPSLKeyword("v6_filter",         ATTR_V6_FILTER,         1),
   RPSLKeyword("mp-filter",         ATTR_MP_FILTER,         1),
   RPSLKeyword("peering",           ATTR_PEERING,           1),
   RPSLKeyword("mp-peering",        ATTR_MP_PEERING,        1),
   RPSLKeyword("blobs",             ATTR_BLOBS,             1),
   RPSLKeyword("mnt-routes",        ATTR_MNT_ROUTES,        1),
   RPSLKeyword("components",        ATTR_COMPONENTS,        1),
   RPSLKeyword("v6_components",     ATTR_V6_COMPONENTS,     1),
   RPSLKeyword("inject",            ATTR_INJECT,            1),
   RPSLKeyword("v6_inject",         ATTR_V6_INJECT,         1),
   RPSLKeyword("aggr-mtd",          ATTR_AGGR_MTD,          1),
   RPSLKeyword("aggr-bndry",        ATTR_AGGR_BNDRY,        1),
   RPSLKeyword("rs-members",        ATTR_RS_MEMBERS,        1),
   RPSLKeyword("rs-mp-members",     ATTR_RS_MP_MEMBERS,     1),
   RPSLKeyword("rtr-mp-members",    ATTR_RTR_MP_MEMBERS,    1),
   RPSLKeyword(NULL, 0,                                     1)
};

Schema schema(rpsl_keywords, rpsl_rules);
