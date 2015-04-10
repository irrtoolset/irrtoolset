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

#include <fstream>
#include <cstring>

#include "schema.hh"
#include "object.hh"
#include "rpsl.y.hh"
#include "rptype.hh"

using namespace std;

#define NO          0
#define PRS         1
#define MAN         1
#define SNG         1
#define HDN         1
#define KEY         1
#define IDX         1
#define OBS         1


char Schema::dictionary_text[] = "";

// org: should be defined as:
//"attr:  org              syntax(rpsl_word),                                     optional,  multiple\n"
// however, this requires a substantial amount of plumbing
#define CMN_ATTRS \
"attr:  notify           syntax(email),                                         optional,  multiple\n" \
"attr:  remarks                                                                 optional,  multiple\n" \
"attr:  org                                                                     optional,  multiple, lookup\n" \
"attr:  source           syntax(rpsl_word),                                     mandatory, single,   internal\n" \
"attr:  integrity        syntax(rpsl_word),                                     optional, single,   internal\n" \
"attr:  changed          optional,  multiple\n" \
"attr:  created          optional,  single\n" \
"attr:  last-modified    optional,  single\n" \
"attr:  deleted                                                                 optional,  single, deleted\n"

static char base_text[] = 
"class: dictionary\n"
"attr:  dictionary       syntax(rpsl_word),                                     mandatory, single,   key\n"
"attr:  rp-attribute     syntax(special,rp-attribute),                          optional,  multiple\n"
"attr:  typedef          syntax(special,typedef),                               optional,  multiple\n"
"attr:  protocol         syntax(special,protocol),                              optional,  multiple\n"
CMN_ATTRS 
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 optional,  multiple, lookup\n"
"\n"
"dictionary: rpsl\n"
"descr:   rpsl dictionary\n"
"admin-c: Cengiz Alaettinoglu\n"
"tech-c: Cengiz Alaettinoglu\n"
"mnt-by: MNT-CENGIZ\n"
"changed: cengiz@isi.edu 19980324\n"
"source: RPS-WG\n"
"typedef: ListOfIPv4Prefix list of Address_Prefix\n"
"typedef: as_number-as_set_name union as_number, as_set_name\n"
"typedef: ListOfas_number list of as_number\n"
"typedef: ListOfas_number-as_set_name list of as_number-as_set_name\n"
"typedef: ListOfas_set_name  list of as_set_name\n"
"typedef: ListOfroute_set_name  list of route_set_name\n"
"typedef: as_number-as_set_name-route_set_name-IPv4Prefix\n"
"         union as_number, as_set_name, route_set_name, Address_Prefix\n"
"typedef: ListOfas_number-as_set_name-route_set_name-IPv4Prefix\n"
"         list of as_number-as_set_name-route_set_name-IPv4Prefix\n"
"typedef: ListOfIPv6Prefix list of ipv6_Address_Prefix\n"
"typedef: as_number-as_set_name-route_set_name-IPv4Prefix-IPv6Prefix\n"
"         union as_number, as_set_name, route_set_name, Address_Prefix, ipv6_Address_Prefix\n"
"typedef: ListOfas_number-as_set_name-route_set_name-IPv4Prefix-IPv6Prefix\n"
"         list of as_number-as_set_name-route_set_name-IPv4Prefix-IPv6Prefix\n"
"typedef: ListOfrpsl_word List of rpsl_word\n"
"typedef: encapsulation enum[GRE, IPv6inIPv4, IPinIP, DVMRP]\n"
"typedef: address_family enum[ipv4, ipv6, any]\n"
"typedef: address_sub_family enum[unicast, multicast]\n"
"rp-attribute: # preference, smaller values represent higher preferences\n"
"              pref\n"
"              operator=(integer[0, 65535])  \n"
"rp-attribute: # BGP multi_exit_discriminator attribute\n"
"              med\n"
"              operator=(union integer[0, 65535], enum[igp_cost])\n"
"              # to set med to the IGP metric: med = igp_cost;\n"
"              #operator=(enum[igp_cost])\n"
"rp-attribute: # BGP destination preference attribute (dpa)\n"
"              dpa\n"
"              operator=(integer[0, 65535])  \n"
"rp-attribute: # BGP aspath attribute\n"
"              aspath\n"
"              # prepends AS numbers from last to first order\n"
"              prepend(as_number, ...)\n"
"typedef:      # a community value in RPSL is either\n"
"              #  - a 4 byte integer\n"
"              #  - internet, no_export, no_advertise (see RFC-1997)\n"
"              community_elm union\n"
"              integer[1, 4294967295],\n"
"              enum[internet, no_export, no_advertise]\n"
"typedef:      # list of community values { 40, no_export, 3561:70}\n"
"              community_list\n"
"              list of community_elm\n"
"rp-attribute: # BGP community attribute\n"
"              community\n"
"              # set to a list of communities\n"
"              operator=(community_list)\n"
"              # order independent equality comparison\n"
"              operator==(community_list)\n"
"              # append community values\n"
"              operator.=(community_list)\n"
"              append(community_elm, ...)\n"
"              # delete community values\n"
"              delete(community_elm, ...)\n"
"              # a filter: true if one of community values is contained\n"
"              contains(community_elm, ...)\n"
"              # shortcut to contains: community(no_export, {3561,70})\n"
"              operator()(community_elm, ...)\n"
"rp-attribute: # next hop router in a static route\n"
"              next-hop\n"
"              operator=(union ipv4_address, ipv6_address, enum[self])\n"
"rp-attribute: # cost of a static route\n"
"              cost\n"
"              operator=(integer[0, 65535])\n"
"protocol: BGP4\n"
"          # as number of the peer router\n"
"          MANDATORY asno(as_number)\n"
"          # enable flap damping\n"
"          OPTIONAL flap_damp()\n"
"          OPTIONAL flap_damp(integer[0,65535],# penalty per flap\n"
"                             integer[0,65535],# penalty value for supression\n"
"                             integer[0,65535],# penalty value for reuse\n"
"                             integer[0,65535],# halflife in secs when up\n"
"                             integer[0,65535],# halflife in secs when down\n"
"                             integer[0,65535])# maximum penalty\n"
"protocol: OSPF\n"
"protocol: RIP\n"
"protocol: IGRP\n"
"protocol: IS-IS\n"
"protocol: STATIC\n"
"protocol: RIPng\n"
"protocol: DVMRP\n"
"protocol: PIM-DM\n"
"protocol: PIM-SM\n"
"protocol: CBT\n"
"protocol: MOSPF\n"
"protocol: MPBGP\n";

static char classes_text[] =
"class: mntner\n"
"attr:  mntner           syntax(rpsl_word),                                     mandatory, single,   key\n"
"attr:  auth             syntax(special, blobs),                                mandatory, multiple\n"
"attr:  upd-to           syntax(email),                                         mandatory, multiple\n"
"attr:  mnt-nfy          syntax(email),                                         optional,  multiple\n"
CMN_ATTRS
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  optional,  multiple, lookup\n"
"\n"
"class: person\n"
"attr:  person                                                                  mandatory, single,   lookup\n"
"attr:  nic-hdl          syntax(rpsl_word),                                     mandatory, single,   key\n"
"attr:  address                                                                 mandatory, multiple\n"
"attr:  phone                                                                   mandatory, multiple\n"
"attr:  fax-no                                                                  optional,  multiple\n"
"attr:  e-mail           syntax(email),                                         optional,  multiple, lookup\n"
CMN_ATTRS
"attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup\n"
"\n"
"class: role\n"
"attr:  role                                                                    mandatory, single,   lookup\n"
"attr:  nic-hdl                                                                 mandatory, single,   key\n"
"attr:  address                                                                 mandatory, multiple\n"
"attr:  phone                                                                   mandatory, multiple\n"
"attr:  fax-no                                                                  optional,  multiple\n"
"attr:  e-mail           syntax(email),                                         mandatory, multiple, lookup\n"
"attr:  trouble                                                                 optional,  multiple\n"
CMN_ATTRS
"attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"\n"
"class: route\n"
"attr:  route            syntax(address_prefix),                                mandatory, single,   key, lookup\n"
"attr:  origin           syntax(as_number),                                     mandatory, single,   key, lookup\n"
"attr:  withdrawn        syntax(date),                                          optional,  single,   deleted\n"
"attr:  member-of        syntax(ListOfroute_set_name),                          optional,  multiple, lookup\n"
"attr:  inject           syntax(special, inject),                               optional,  multiple\n"
"attr:  components       syntax(special, components),                           optional,  single\n"
"attr:  aggr-bndry       syntax(special, aggr-bndry),                           optional,  single\n"
"attr:  aggr-mtd         syntax(special, aggr-mtd),                             optional,  single\n"
"attr:  export-comps     syntax(special, filter),                         optional,  single\n"
"attr:  holes            syntax(ListOfIPv4Prefix),                              optional,  multiple\n"
"attr:  pingable                                                                optional,  multiple\n"
"attr:  ping-hdl         syntax(list of rpsl_word),                             optional,  multiple\n"
CMN_ATTRS
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  mnt-lower        syntax(list of rpsl_word),                             optional, multiple, lookup\n"
"attr:  mnt-routes       syntax(special, mnt-routes),                           optional, multiple, lookup\n"
"attr:  admin-c                                                                 optional,  multiple, lookup\n"
"attr:  tech-c                                                                  optional,  multiple, lookup\n"
"attr:  cross-nfy        syntax(list of rpsl_word),                             optional,  multiple\n"
"attr:  cross-mnt        syntax(list of rpsl_word),                             optional,  multiple\n"
"\n"
"class: route6\n"
"attr:  route6            syntax(ipv6_address_prefix),                                mandatory, single,   key, lookup\n"
"attr:  origin           syntax(as_number),                                     mandatory, single,   key, lookup\n"
"attr:  withdrawn        syntax(date),                                          optional,  single,   deleted\n"
"attr:  member-of        syntax(ListOfroute_set_name),                          optional,  multiple, lookup\n"
"attr:  inject           syntax(special, v6_inject),                               optional,  multiple\n"
"attr:  components       syntax(special, v6_components),                           optional,  single\n"
"attr:  aggr-bndry       syntax(special, aggr-bndry),                           optional,  single\n"
"attr:  aggr-mtd         syntax(special, aggr-mtd),                             optional,  single\n"
"attr:  export-comps     syntax(special, v6_filter),                         optional,  single\n"
"attr:  holes            syntax(ListOfIPv6Prefix),                              optional,  multiple\n"
"attr:  pingable                                                                optional,  multiple\n"
"attr:  ping-hdl         syntax(list of rpsl_word),                             optional,  multiple\n"
CMN_ATTRS
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  mnt-lower        syntax(list of rpsl_word),                             optional, multiple, lookup\n"
"attr:  mnt-routes      syntax(special, mnt-routes6),                             optional, multiple, lookup\n"
"attr:  admin-c                                                                 optional,  multiple, lookup\n"
"attr:  tech-c                                                                  optional,  multiple, lookup\n"
"attr:  cross-nfy        syntax(list of rpsl_word),                             optional,  multiple\n"
"attr:  cross-mnt        syntax(list of rpsl_word),                             optional,  multiple\n"
"\n"
"class: route-set\n"
"attr:  route-set        syntax(route_set_name),                                mandatory, single,   key\n"
"attr:  members          syntax(special, rs-members),                           optional,  multiple, lookup \n"
"attr:  mp-members       syntax(special, rs-mp-members),                        optional,  multiple, lookup \n"
"attr:  mbrs-by-ref      syntax(list of rpsl_Word),                             optional,  multiple, lookup\n"
CMN_ATTRS
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"\n"
"class: as-set\n"
"attr:  as-set           syntax(as_set_name),                                   mandatory, single,   key\n"
"attr:  members          syntax(list of union as_number, as_set_name),          optional,  multiple, lookup     \n"
"attr:  mbrs-by-ref      syntax(list of rpsl_word),                             optional,  multiple, lookup\n"
CMN_ATTRS
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"\n"
"class: rtr-set\n"
"attr:  rtr-set        syntax(rtr_set_name),                                       mandatory, single,   key\n"
"attr:  members        syntax(list of union rtr_set_name, dns_name, ipv4_address), optional,  multiple, lookup \n"
"attr:  mp-members     syntax(list of union rtr_set_name, dns_name, ipv4_address, ipv6_address), optional,  multiple, lookup \n"
"attr:  mbrs-by-ref    syntax(list of rpsl_Word),                               	  optional,  multiple, lookup\n"
CMN_ATTRS
"attr:  descr                                                                   	  mandatory, multiple\n"
"attr:  mnt-by         syntax(list of rpsl_word),                               	  mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 	  mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  	  mandatory, multiple, lookup\n"
"\n"
"class: peering-set\n"
"attr:  peering-set    syntax(peering_set_name),                                   mandatory, single,   key\n"
"attr:  peering        syntax(special, peering),                                   optional, multiple\n"
"attr:  mp-peering     syntax(special, mp-peering),                                optional, multiple\n"
CMN_ATTRS
"attr:  descr                                                                   	  mandatory, multiple\n"
"attr:  mnt-by         syntax(list of rpsl_word),                               	  mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 	  mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  	  mandatory, multiple, lookup\n"
"\n"
"class: filter-set\n"
"attr:  filter-set       syntax(filter_set_name),                               mandatory, single,   key\n"
"attr:  filter           syntax(special, filter),                               optional, single\n"
"attr:  mp-filter        syntax(special, mp-filter),                            optional, single\n"
CMN_ATTRS
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"\n"
"class: aut-num\n"
"attr:  aut-num          syntax(as_number),                                     mandatory, single,   key\n"
"attr:  as-name          syntax(rpsl_word),                                     mandatory, single,   lookup\n"
"attr:  member-of        syntax(List Of AS_set_name),                           optional,  multiple, lookup\n"
"attr:  import           syntax(special,import),                                optional,  multiple\n"
"attr:  mp-import        syntax(special,mp-import),                             optional,  multiple\n"
"attr:  import-via                                                              optional,  multiple\n"
"attr:  export           syntax(special,export),                                optional,  multiple\n"
"attr:  mp-export        syntax(special,mp-export),                             optional,  multiple\n"
"attr:  export-via                                                              optional,  multiple\n"
"attr:  default          syntax(special,default),                               optional,  multiple\n"
"attr:  mp-default       syntax(special,mp-default),                            optional,  multiple\n"
CMN_ATTRS
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  mnt-routes       syntax(special, mnt-routes-mp),                           optional,  multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"attr:  cross-nfy        syntax(list of rpsl_word),                             optional,  multiple\n"
"attr:  cross-mnt        syntax(list of rpsl_word),                             optional,  multiple\n"
"\n"
"class: inet-rtr\n"
"attr:  inet-rtr         syntax(dns_name),                                      mandatory, single,   key\n"
"attr:  alias            syntax(dns_name),                                      optional,  multiple, lookup\n"
"attr:  local-as         syntax(as_number),                                     mandatory, single,   lookup\n"
"attr:  ifaddr           syntax(special,ifaddr),                                mandatory, multiple\n"
"attr:  interface        syntax(special,interface),                             optional, multiple\n"
"attr:  peer             syntax(special,peer),                                  optional,  multiple\n"
"attr:  mp-peer          syntax(special,mp-peer),                               optional,  multiple\n"
CMN_ATTRS
"attr:  member-of        syntax(List Of rtr_set_name),                           optional,  multiple, lookup\n"
"attr:  descr                                                                   optional,  multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"\n"
"class: inetnum\n"
"attr:  inetnum          syntax(range of ipv4_address),                         mandatory, single,   key\n"
"attr:  netname                                                                 mandatory, single,   lookup\n"
"attr:  country                                                                 mandatory, multiple\n"
"attr:  rev-srv                                                                 optional,  multiple, lookup\n"
"attr:  status                                                                  mandatory, single\n"
"attr:  mnt-lower        syntax(ListOfrpsl_word),                               optional,  multiple\n"
CMN_ATTRS
"attr:  geoloc                                                                  optional,  single\n"
"attr:  language                                                                optional,  multiple\n"
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup\n"
"attr:  mnt-routes       syntax(special, mnt-routes),                           optional,  multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"\n"
"class: inet6num\n"
"attr:  inet6num          syntax(ipv6_address_prefix),                         mandatory, single,   key\n"
"attr:  netname                                                                 mandatory, single,   lookup\n"
"attr:  country                                                                 mandatory, multiple\n"
"attr:  rev-srv                                                                 optional,  multiple, lookup\n"
"attr:  status                                                                  mandatory, single\n"
"attr:  assignment-size                                                         optional, single\n"
"attr:  mnt-lower        syntax(ListOfrpsl_word),                               optional,  multiple\n"
CMN_ATTRS
"attr:  geoloc                                                                  optional,  single\n"
"attr:  language                                                                optional,  multiple\n"
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup\n"
"attr:  mnt-routes       syntax(special, mnt-routes6),                          optional,  multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"\n"
"class: domain\n"
"attr:  domain           syntax(dns_name),                                      mandatory, single,   key\n"
"attr:  sub-dom                                                                 optional,  multiple, lookup\n"
"attr:  dom-net                                                                 optional,  multiple\n"
"attr:  zone-c                                                                  mandatory, multiple, lookup\n"
"attr:  nserver                                                                 optional,  multiple, lookup\n"
"attr:  mnt-lower        syntax(ListOfrpsl_word),                               optional,  multiple\n"
CMN_ATTRS
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"\n"
"class: limerick\n"
"attr:  limerick                                                                mandatory, single,   key\n"
"attr:  text                                                                    mandatory, multiple\n"
"attr:  author                                                                  mandatory, multiple, lookup\n"
CMN_ATTRS
"attr:  descr                                                                   optional,  multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"\n"
"class: as-block\n"
"attr:  as-block         syntax(union as_number, range of as_number),           mandatory, single,   key\n"
"attr:  mnt-lower        syntax(ListOfrpsl_word),                               optional,  multiple\n"
CMN_ATTRS
"attr:  descr                                                                   mandatory, multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  tech-c                                                                  mandatory, multiple, lookup\n"
"\n"
"class: key-cert\n"
"attr:  key-cert       mandatory, single,    key,        syntax(rpsl_word)\n"
"attr:  method         mandatory, single,                syntax(rpsl_word)\n"
"attr:  owner          mandatory, multiple\n"
"attr:  fingerpr       mandatory, single\n"
"attr:  certif         mandatory, single\n"
"attr:  remarks        optional,  multiple\n"
"attr:  source         mandatory, single,   lookup,      syntax(rpsl_word)\n"
"attr:  changed        optional,  multiple\n"
"attr:  deleted        optional,  single, deleted\n"
"attr:  mnt-by         mandatory, multiple, lookup,      syntax(list of rpsl_word)\n"
"attr:  notify         optional,  multiple,              syntax(email)\n"
"\n"
"class: peval   \n"
"attr:  peval            syntax(special,filter),                                 optional,  single\n"
"\n"
"class: mp-peval\n"
"attr:  mp-peval         syntax(special,mp-peval),                              optional,  single\n"
"\n"
"class: repository\n"
"attr:  repository           mandatory,  single,  key,   syntax(rpsl_word)\n"
"attr:  query-address        mandatory,  multiple\n"
"attr:  response-auth-type   mandatory,  multiple\n"
"attr:  submit-address       mandatory,  multiple\n"
"attr:  submit-auth-type     mandatory,  multiple\n"
"attr:  repository-cert      mandatory,  multiple\n"
"attr:  expire               mandatory,  single\n"
"attr:  heartbeat-interval   mandatory,  single\n"
"attr:  descr                optional,   multiple\n"
"attr:  admin-c              mandatory,  multiple, lookup\n"
"attr:  tech-c               mandatory,  multiple, lookup\n"
"attr:  mnt-by               mandatory,  multiple, lookup\n"
CMN_ATTRS
"\n"
"class: poem\n"
"attr:  poem                                                                    mandatory, single, key\n"
"attr:  form                                                                    mandatory, single, lookup\n"
"attr:  text                                                                    mandatory, multiple\n"
"attr:  author                                                                  mandatory, multiple, lookup\n"
"attr:  descr                                                                   optional,  multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  notify           syntax(email),                                         optional,  multiple\n" \
"attr:  remarks                                                                 optional,  multiple\n" \
"attr:  source           syntax(rpsl_word),                                     mandatory, single,   internal\n" \
"attr:  integrity        syntax(rpsl_word),                                     optional, single,   internal\n" \
"attr:  changed          optional,  multiple\n" \
"attr:  deleted                                                                 optional,  single, deleted\n"
"\n"
"class: poetic-form\n"
// this should really be:
// "attr:  poetic-form   syntax(poetic_form_name)                                  mandatory, single, key\n"
"attr:  poetic-form                                                             mandatory, single, key\n"
"attr:  descr                                                                   optional,  multiple\n"
"attr:  mnt-by           syntax(list of rpsl_word),                             mandatory, multiple, lookup\n"
"attr:  admin-c                                                                 mandatory, multiple, lookup\n"
"attr:  notify           syntax(email),                                         optional,  multiple\n" \
"attr:  remarks                                                                 optional,  multiple\n" \
"attr:  source           syntax(rpsl_word),                                     mandatory, single,   internal\n" \
"attr:  integrity        syntax(rpsl_word),                                     optional, single,   internal\n" \
"attr:  changed          optional,  multiple\n" \
"attr:  deleted                                                                 optional,  single, deleted\n"
"\n"
"class: organisation\n"
"attr:  org-name                                                                mandatory, single, lookup\n"
"attr:  org-type                                                                mandatory, single\n"
"attr:  descr                                                                   optional,  multiple\n"
"attr:  address                                                                 mandatory, multiple\n"
"attr:  phone                                                                   optional,  multiple\n"
"attr:  fax-no                                                                  optional,  multiple\n"
"attr:  e-mail           syntax(email),                                         mandatory, multiple, lookup\n"
"attr:  trouble                                                                 optional,  multiple\n"
CMN_ATTRS
"attr:  mnt-by           syntax(list of rpsl_word),                             optional,  multiple, lookup\n"
"attr:  admin-c                                                                 optional, multiple, lookup\n"
"attr:  tech-c                                                                  optional, multiple, lookup\n"
"attr:  ref-nfy          syntax(email),                                         optional, multiple, lookup\n"
"attr:  mnt-ref          syntax(list of rpsl_word),                             optional, multiple, lookup\n"
"attr:  abuse-mailbox    syntax(email),                                         optional, multiple, lookup\n"
;

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

Object *Schema::initializeFromString(char *class_text, const char *lookFor) {
   Object *o;
   const AttrAttr *attr;
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
   RPSLKeyword("mp-peval",          ATTR_MP_PEVAL,          1),
   RPSLKeyword("peering",           ATTR_PEERING,           1),
   RPSLKeyword("mp-peering",        ATTR_MP_PEERING,        1),
   RPSLKeyword("blobs",             ATTR_BLOBS,             1),
   RPSLKeyword("mnt-routes",        ATTR_MNT_ROUTES,        1),
   RPSLKeyword("mnt-routes6",       ATTR_MNT_ROUTES6,       1),
   RPSLKeyword("mnt-routes-mp",     ATTR_MNT_ROUTES_MP,     1),
   RPSLKeyword("components",        ATTR_COMPONENTS,        1),
   RPSLKeyword("v6_components",     ATTR_V6_COMPONENTS,     1),
   RPSLKeyword("inject",            ATTR_INJECT,            1),
   RPSLKeyword("v6_inject",         ATTR_V6_INJECT,         1),
   RPSLKeyword("aggr-mtd",          ATTR_AGGR_MTD,          1),
   RPSLKeyword("aggr-bndry",        ATTR_AGGR_BNDRY,        1),
   RPSLKeyword("rs-members",        ATTR_RS_MEMBERS,        1),
   RPSLKeyword("rs-mp-members",     ATTR_RS_MP_MEMBERS,     1),
   RPSLKeyword(NULL, 0,                                     1)
};

Schema schema(rpsl_keywords, rpsl_rules);
