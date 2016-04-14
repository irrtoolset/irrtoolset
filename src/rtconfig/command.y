%{
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

#include <ostream>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdarg>
#include "rtconfig.hh"
#include "f_cisco.hh"
#include "f_ciscoxr.hh"
#include "f_junos.hh" 
#include "irr/irr.hh"
#include "irrutil/trace.hh"
#include "rpsl/rpsl.hh"

extern "C" {
// FIXME
#if HAVE_MEMORY_H && 0
#   include <memory.h>
#endif
#ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
#endif
}

#define yylineno commandlineno

extern int yyerror(const char *);
extern int yylex();
extern int yylineno;

int xx_eof = 0;

%}

%union {
   int i;
   char *val;
   /*IPAddr *ip;*/
   MPPrefix *ip;
   ASt as;
}

%name-prefix "command"
%output "command.y.cc"

%token <ip>  TKN_IP
%token <as>  TKN_ASNUM
%token <val> TKN_ERROR
%token <val> TKN_STR
%token <i>   TKN_INT
%token <val> TKN_DNSNAME
%token <val> TKN_FILTER
%token <val> TKN_WORD

%token <val> KW_IMPORT
%token <val> KW_IMPORT_PEERGROUP
%token <val> KW_EXPORT
%token <val> KW_EXPORT_GROUP
%token <val> KW_IMPORT_GROUP
%token <val> KW_PRINT_PREFIXES
%token <val> KW_PRINT_PREFIX_RANGES
%token <val> KW_PRINT_SUPER_PREFIX_RANGES
%token <val> KW_ASPATH_ACCESS_LIST
%token <val> KW_ACCESS_LIST
%token <val> KW_CONFIGURE_ROUTER
%token <val> KW_STATIC2BGP
%token <val> KW_SET
%token <val> KW_CISCO_MAP_NAME
%token <val> KW_JUNOS_POLICY_NAME
%token <val> KW_CISCO_PREFIX_ACL_NO
%token <val> KW_CISCO_ASPATH_ACL_NO
%token <val> KW_CISCO_PKTFILTER_ACL_NO
%token <val> KW_CISCO_COMMUNITY_ACL_NO
%token <val> KW_COMMUNITY_SET_NO
%token <val> KW_CISCO_ACCESS_LIST_NO
%token <val> KW_SOURCE
%token <val> KW_PREFERENCECEILING
%token <val> KW_CISCO_MAX_PREFERENCE
%token <val> KW_NETWORKS
%token <val> KW_V6NETWORKS
%token <val> KW_DEFAULT
%token <val> KW_CISCO_MAP_INC
%token <val> KW_CISCO_MAP_START
%token <val> KW_INBOUND_PKT_FILTER
%token <val> KW_OUTBOUND_PKT_FILTER

%%
input_stream: {
#if !(HAVE_LIBREADLINE && HAVE_LIBHISTORY)
   if (opt_prompt)
      std::cout << opt_prompt;
#endif // HAVE_LIBREADLINE && HAVE_LIBHISTORY
}
| input_stream input {
#if !(HAVE_LIBREADLINE && HAVE_LIBHISTORY)
   if (opt_prompt)
      std::cout << opt_prompt;
#endif // !(HAVE_LIBREADLINE && HAVE_LIBHISTORY)
}
;

input: input_line '\n'
| error '\n' {
  yyerrok;
}
| '\n'
;

input_line: import_line
| import_peergroup_line
| export_line
| export_group_line
| import_group_line
| print_prefixes_line
| print_prefix_ranges_line
| print_super_prefix_ranges_line
| access_list_line
| aspath_access_list_line
| default_line
| configure_router_line
| static2bgp_line
| networks_line
| v6networks_line
| inbound_pkt_filter_line
| outbound_pkt_filter_line
| cisco_map_name_line
| junos_policy_name_line
| cisco_map_inc_line
| cisco_map_start_line
| cisco_access_list_no_line
| preferenceCeiling_line
| source_line
;

import_line: KW_IMPORT TKN_ASNUM TKN_IP TKN_ASNUM TKN_IP {
   /*
   cout << "!" << endl
        << "!LINE " << yylineno << " -- import" << endl
        << "!" << endl;
   */
   rtConfig->importP($2, $3, $4, $5);
   delete $3;
   delete $5;
}
;

import_peergroup_line: KW_IMPORT_PEERGROUP TKN_ASNUM TKN_IP TKN_ASNUM TKN_IP TKN_WORD {
   /*
   cout << "!" << endl
        << "!LINE " << yylineno << " -- import" << endl
        << "!" << endl;
   */
   rtConfig->importPeerGroup($2, $3, $4, $5, $6);
   delete $3;
   delete $5;
}
;

export_line: KW_EXPORT TKN_ASNUM TKN_IP TKN_ASNUM TKN_IP {
   /*
   cout << "!" << endl
        << "!LINE " << yylineno << " -- export" << endl
        << "!" << endl;
   */
   rtConfig->exportP($2, $3, $4, $5);
   delete $3;
   delete $5;
}
;

export_group_line: KW_EXPORT_GROUP TKN_ASNUM TKN_WORD {
   /* 
   cout << "!" << endl
        << "!LINE " << yylineno << " -- export" << endl
        << "!" << endl;
   */
   rtConfig->exportGroup($2, $3);
}
;

import_group_line: KW_IMPORT_GROUP TKN_ASNUM TKN_WORD {
   /* 
   cout << "!" << endl
        << "!LINE " << yylineno << " -- export" << endl
        << "!" << endl;
   */
   rtConfig->importGroup($2, $3);
}
;

print_prefixes_line: KW_PRINT_PREFIXES TKN_STR TKN_FILTER {
   char *p = $2;
   while ((p = strchr(p, '\\'))) {
      if (*(p+1) == 'n') {
	 *p = '%';
	 *(p+1) = '\n';
      }
      if (*(p+1) == 't') {
	 *p = '%';
	 *(p+1) = '\t';
      }
   }
   rtConfig->printPrefixes($3, $2);
};

print_prefix_ranges_line: KW_PRINT_PREFIX_RANGES TKN_STR TKN_FILTER {
   char *p = $2;
   while ((p = strchr(p, '\\'))) {
      if (*(p+1) == 'n') {
	 *p = '%';
	 *(p+1) = '\n';
      }
      if (*(p+1) == 't') {
	 *p = '%';
	 *(p+1) = '\t';
      }
   }
   rtConfig->printPrefixRanges($3, $2);
};

print_super_prefix_ranges_line: KW_PRINT_SUPER_PREFIX_RANGES TKN_STR TKN_FILTER {
   char *p = $2;
   while ((p = strchr(p, '\\'))) {
      if (*(p+1) == 'n') {
	 *p = '%';
	 *(p+1) = '\n';
      }
      if (*(p+1) == 't') {
	 *p = '%';
	 *(p+1) = '\t';
      }
   }
   rtConfig->printSuperPrefixRanges($3, $2);
};

access_list_line: KW_ACCESS_LIST TKN_FILTER {
   rtConfig->accessList($2);
};

aspath_access_list_line: KW_ASPATH_ACCESS_LIST TKN_FILTER {
   rtConfig->aspathAccessList($2);
};

default_line: KW_DEFAULT TKN_ASNUM TKN_ASNUM {
   rtConfig->deflt($2, $3);
}
;

configure_router_line: KW_CONFIGURE_ROUTER TKN_DNSNAME {
   rtConfig->configureRouter($2);
   free($2);
}
;

static2bgp_line: KW_STATIC2BGP TKN_ASNUM TKN_IP {
   rtConfig->static2bgp($2, $3);
   delete $3;
}
;

networks_line: KW_NETWORKS TKN_ASNUM {
   rtConfig->networks($2);
}
;

v6networks_line: KW_V6NETWORKS TKN_ASNUM {
   rtConfig->IPv6networks($2);
}
;

inbound_pkt_filter_line: KW_INBOUND_PKT_FILTER TKN_STR TKN_ASNUM TKN_IP TKN_ASNUM TKN_IP {
   rtConfig->inboundPacketFilter($2, $3, $4, $5, $6);
   delete $4;
   delete $6;
}
;

outbound_pkt_filter_line: KW_OUTBOUND_PKT_FILTER TKN_STR TKN_ASNUM TKN_IP TKN_ASNUM TKN_IP {
   rtConfig->outboundPacketFilter($2, $3, $4, $5, $6);
   delete $4;
   delete $6;
}
;

cisco_map_name_line: KW_SET KW_CISCO_MAP_NAME '=' TKN_STR {
   strcpy(CiscoConfig::mapNameFormat, $4);
   strcpy(CiscoXRConfig::mapNameFormat, $4);
   Trace(TR_INPUT) << "RtConfig: cisco_map_name '"
		   << CiscoConfig::mapNameFormat << "'" << std::endl;
   Trace(TR_INPUT) << "RtConfig: cisco_map_name '"
		   << CiscoXRConfig::mapNameFormat << "'" << std::endl;
}
;
junos_policy_name_line: KW_SET KW_JUNOS_POLICY_NAME '=' TKN_STR {
   strcpy(JunosConfig::mapNameFormat, $4);
   Trace(TR_INPUT) << "RtConfig: junos_policy_name '"
		   << JunosConfig::mapNameFormat << "'" << std::endl;
}
;
cisco_map_inc_line: KW_SET KW_CISCO_MAP_INC '=' TKN_INT {
   CiscoConfig::mapIncrements = $4;
   Trace(TR_INPUT) << "RtConfig: cisco_map_increment_by '" 
		   << CiscoConfig::mapIncrements << "'" << std::endl;
}
;

cisco_map_start_line: KW_SET KW_CISCO_MAP_START '=' TKN_INT {
   CiscoConfig::mapNumbersStartAt = $4;
   Trace(TR_INPUT) << "RtConfig: cisco_map_first_no '" 
		   << CiscoConfig::mapNumbersStartAt << "'" << std::endl;
}
;

cisco_access_list_no_line: KW_SET KW_CISCO_PREFIX_ACL_NO '=' TKN_INT {
   if ($4 > 0)
      prefixMgr.setNextID($4);
   Trace(TR_INPUT) << "RtConfig: cisco_prefix_access_list_no '"
		   << $4 << "'" << std::endl;
}
| KW_SET KW_CISCO_ASPATH_ACL_NO '=' TKN_INT {
   if ($4 > 0)
      aspathMgr.setNextID($4);
   Trace(TR_INPUT) << "RtConfig: cisco_aspath_access_list_no '"
		   << $4 << "'" << std::endl;
}
| KW_SET KW_CISCO_PKTFILTER_ACL_NO '=' TKN_INT {
   if ($4 > 0)
      pktFilterMgr.setNextID($4);
   Trace(TR_INPUT) << "RtConfig: cisco_pktfilter_access_list_no '"
		   << $4 << "'" << std::endl;
}
| KW_SET KW_CISCO_COMMUNITY_ACL_NO '=' TKN_INT {
   if ($4 > 0)
      communityMgr.setNextID($4);
   Trace(TR_INPUT) << "RtConfig: cisco_community_access_list_no '"
		   << $4 << "'" << std::endl;
}
| KW_SET KW_CISCO_ACCESS_LIST_NO '=' TKN_INT {
   if ($4 > 0) {
      communityMgr.setNextID($4);
      pktFilterMgr.setNextID($4);
      aspathMgr.setNextID($4);
      prefixMgr.setNextID($4);
      ipv6prefixMgr.setNextID($4);
      ipv6pktFilterMgr.setNextID($4);
   }
   Trace(TR_INPUT) << "RtConfig: cisco_access_list_no '"
		   << $4 << "'" << std::endl;
}
| KW_SET KW_COMMUNITY_SET_NO '=' TKN_INT {
   if ($4 > 0)
      communitySetMgr.setNextID($4);
   Trace(TR_INPUT) << "RtConfig: community_set_no '"
		   << $4 << "'" << std::endl;
}
;

preferenceCeiling_line: KW_SET KW_PREFERENCECEILING '=' TKN_INT {
   if ($4 >= 0)
      RtConfig::preferenceCeiling = $4;

   Trace(TR_INPUT) << "RtConfig: preferenceCeiling '"
		   << RtConfig::preferenceCeiling << "'" << std::endl;
}
;

source_line: KW_SET KW_SOURCE '=' TKN_STR {
   irr->SetSources($4);
   Trace(TR_INPUT) << "RtConfig: database order is changed to'"
		   << $4 << "'" << std::endl;
}
;


%%

int yyerror(const char *s) {
     std::cerr << "Error in template file at line " << yylineno
               << ": " << s
               << std::endl;
     return(0);
}

