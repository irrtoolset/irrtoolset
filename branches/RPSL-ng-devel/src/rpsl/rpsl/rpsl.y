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
//             Katie Petrusha <katie@ripe.net>

#include "config.h"
#include "time.h"
#include "schema.hh"
#include "object.hh"
#include "regexp.hh"
#include "rptype.hh"

#ifdef DEBUG
#define YYDEBUG 1
#endif // DEBUG
#if YYDEBUG != 0
// stdio is needed for yydebug
#include <cstdio>
#endif

extern "C" {
#ifdef HAVE_REGEX_H
#include <regex.h>
#else
#include <gnu/regex.h>
#endif
}

extern void handle_error(char *, ...);
extern void handle_object_error(char *, ...);
extern void handle_warning(char *, ...);
extern int yylex();
char *token_name(int token_id);
void rpslerror(char *s, ...);
Attr *changeCurrentAttr(Attr *b);
void handleArgumentTypeError(char *attr, char *method, int position,
			     const RPType *correctType, 
			     bool isOperator = false);
const AttrMethod *searchMethod(const AttrRPAttr *rp_attr, char *method, ItemList *args);

/* argument to yyparse result of parsing should be stored here */
#define YYPARSE_PARAM object	
#define yyschema schema
#define enable_yy_parser_debugging enable_rpsl_parser_debugging

extern Object *current_object;

%}

%expect 1

%union {
   long long int      i;
   double             real;
   char              *string;
   void              *ptr;
   time_t             time;
   Item              *item;
   ItemList          *list;
   regexp            *re;
   SymID              sid;
   IPAddr            *ip;
   Prefix            *prfx;
   PrefixRange       *prfxrng;
   IPv6Addr          *ipv6;
   IPv6Prefix        *prfxv6;
   IPv6PrefixRange   *prfxv6rng;
   AddressFamily     *afi;
   RPType            *typenode;
   MPPrefix          *mpprefix;
   Tunnel            *tunnel;

   Filter                    *filter;
   FilterMS                  *moreSpecOp;
   PolicyPeering             *peering;
   PolicyPeering             *mp_peering;
   PolicyActionList          *actionList;
   PolicyAction              *actionNode;
   PolicyFactor              *policyFactor;
   PolicyTerm                *policyTerm;
   PolicyExpr                *policyExpr;
   List<PolicyPeeringAction> *peeringActionList;

   Attr               *attr;
   AttrAttr           *rpslattr;
   AttrMethod         *method;
   const AttrRPAttr   *rp_attr;
   const AttrProtocol *protocol;
   AttrProtocolOption *protocol_option;
   List<AttrProtocolOption> *protocol_option_list;

   AttrPeerOption       *peer_option;
   List<AttrPeerOption> *peer_option_list;

   List<RPTypeNode> *typelist;
   List<AttrMethod> *methodlist;
   List<WordNode>   *wordlist;

   List<AttrMntRoutes::MntPrfxPair> *listMntPrfxPair;
   AttrMntRoutes::MntPrfxPair       *mntPrfxPair;
}

%token 		KEYW_TRUE
%token 		KEYW_FALSE
%token 		KEYW_ACTION
%token 		KEYW_ACCEPT
%token 		KEYW_ANNOUNCE
%token 		KEYW_FROM
%token 		KEYW_TO
%token 		KEYW_AT
%token 		KEYW_ANY
%token    KEYW_REFINE
%token    KEYW_EXCEPT
%token 		KEYW_STATIC
%token 		KEYW_NETWORKS
%token 		KEYW_MASKLEN
%token 		KEYW_UNION
%token 		KEYW_RANGE
%token 		KEYW_LIST
%token 		KEYW_OF
%token 		KEYW_OPERATOR
%token 		KEYW_SYNTAX
%token 		KEYW_SPECIAL
%token 		KEYW_REGEXP
%token 		KEYW_OPTIONAL
%token 		KEYW_MANDATORY
%token 		KEYW_INTERNAL
%token 		KEYW_SINGLEVALUED
%token 		KEYW_MULTIVALUED
%token 		KEYW_LOOKUP
%token 		KEYW_KEY
%token 		KEYW_DELETED
%token 		KEYW_OBSOLETE
%token 		KEYW_PEERAS
%token 		KEYW_PROTOCOL
%token 		KEYW_INTO
%token 		KEYW_ATOMIC
%token 		KEYW_INBOUND
%token 		KEYW_OUTBOUND
%token 		KEYW_UPON
%token 		KEYW_HAVE_COMPONENTS
%token 		KEYW_EXCLUDE
%token 		KEYW_AFI
%token 		KEYW_TUNNEL

%token          TKN_ERROR
%token          TKN_UNKNOWN_CLASS
%token          TKN_EOA		/* end of attribute */
%token          TKN_EOO		/* end of object */
%token          TKN_FREETEXT
%token <i>      TKN_INT
%token <real>   TKN_REAL
%token <string> TKN_STRING
%token <time>   TKN_TIMESTAMP
%token <string> TKN_BLOB
%token <ip>     TKN_IPV4 
%token <prfx>   TKN_PRFXV4
%token <prfxrng>   TKN_PRFXV4RNG
%token <ipv6>   TKN_IPV6
%token <prfxv6> TKN_PRFXV6
%token <prfxv6rng> TKN_PRFXV6RNG
%token <i>      TKN_ASNO
%token <sid>    TKN_ASNAME
%token <sid>    TKN_RSNAME
%token <sid>    TKN_RTRSNAME
%token <sid>    TKN_PRNGNAME
%token <sid>    TKN_FLTRNAME
%token <i>      TKN_BOOLEAN
%token <string> TKN_WORD
%token <rp_attr> TKN_RP_ATTR
%token <sid>    TKN_DNS
%token <string> TKN_EMAIL
%token          TKN_3DOTS
%token <afi>    TKN_AFI

%token <attr> ATTR_GENERIC
%token <attr> ATTR_BLOBS
%token <attr> ATTR_REGEXP
%token <attr> ATTR_IMPORT
%token <attr> ATTR_MP_IMPORT
%token <attr> ATTR_EXPORT
%token <attr> ATTR_MP_EXPORT
%token <attr> ATTR_DEFAULT
%token <attr> ATTR_MP_DEFAULT
%token <attr> ATTR_FREETEXT
%token <attr> ATTR_CHANGED
%token <attr> ATTR_IFADDR
%token <attr> ATTR_INTERFACE
%token <attr> ATTR_PEER
%token <attr> ATTR_MP_PEER
%token <attr> ATTR_INJECT
%token <attr> ATTR_V6_INJECT
%token <attr> ATTR_COMPONENTS
%token <attr> ATTR_V6_COMPONENTS
%token <attr> ATTR_AGGR_MTD
%token <attr> ATTR_AGGR_BNDRY
%token <attr> ATTR_RS_MEMBERS
%token <attr> ATTR_RS_MP_MEMBERS
%token <attr> ATTR_RTR_MP_MEMBERS
%token <attr> ATTR_RP_ATTR
%token <attr> ATTR_TYPEDEF
%token <attr> ATTR_PROTOCOL
%token <attr> ATTR_FILTER
%token <attr> ATTR_V6_FILTER
%token <attr> ATTR_MP_FILTER
%token <attr> ATTR_MP_PEVAL
%token <attr> ATTR_PEERING
%token <attr> ATTR_MP_PEERING
%token <attr> ATTR_ATTR
%token <attr> ATTR_MNT_ROUTES
%token <attr> ATTR_MNT_ROUTES6
%token <attr> ATTR_MNT_ROUTES_MP

%left  		  OP_OR 
%left  		  OP_AND
%right 		  OP_NOT
%nonassoc<moreSpecOp> OP_MS
%nonassoc<string> TKN_OPERATOR

%type<list>      generic_list
%type<list>      rs_members_list
%type<list>      opt_rs_members_list

%type<list>      rs_mp_members_list
%type<list>      opt_rs_mp_members_list

%type<list>      afi_list

%type<list>      blobs_list
%type<list>      generic_non_empty_list
%type<item>      list_item
%type<item>      list_item_0
%type<item>      rs_member
%type<item>      rs_mp_member
%type<item>      afi

%type<string>    tkn_word
%type<string>    tkn_word_from_keyw

%type<attr>      attribute
%type<attr>      generic_attribute
%type<attr>      blobs_attribute
%type<attr>      regexp_attribute
%type<attr>      changed_attribute
%type<attr>      ifaddr_attribute
%type<attr>      interface_attribute
%type<attr>      peer_attribute
%type<attr>      mp_peer_attribute
%type<attr>      components_attribute
%type<attr>      v6_components_attribute
%type<attr>      inject_attribute
%type<attr>      v6_inject_attribute
%type<attr>      aggr_mtd_attribute
%type<attr>      aggr_bndry_attribute

%type<attr>      import_attribute
%type<attr>      mp_import_attribute
%type<attr>      export_attribute
%type<attr>      mp_export_attribute
%type<attr>      default_attribute
%type<attr>      mp_default_attribute
%type<attr>      typedef_attribute
%type<attr>      rpattr_attribute
%type<attr>      rs_members_attribute
%type<attr>      rs_mp_members_attribute
%type<attr>      protocol_attribute
%type<attr>      filter_attribute
%type<attr>      v6_filter_attribute
%type<attr>      mp_filter_attribute
%type<attr>      mp_peval_attribute
%type<attr>      peering_attribute
%type<attr>      mp_peering_attribute
%type<attr>      attr_attribute
%type<attr>      freetext_attribute
%type<attr>      mnt_routes_attribute
%type<attr>      mnt_routes6_attribute
%type<attr>      mnt_routes_mp_attribute

%type<filter>    filter
%type<filter>    opt_default_filter
%type<filter>    filter_term
%type<filter>    filter_factor
%type<filter>    filter_operand
%type<filter>    filter_prefix
%type<filter>    filter_prefix_operand
%type<filter>    opt_filter_prefix_list
%type<filter>    filter_prefix_list
%type<prfxrng>   filter_prefix_list_prefix
%type<filter>    filter_rp_attribute

%type<filter>    mp_filter
%type<filter>    opt_default_mp_filter
%type<filter>    mp_filter_term
%type<filter>    mp_filter_factor
%type<filter>    mp_filter_operand
%type<filter>    mp_filter_prefix
%type<filter>    mp_filter_prefix_operand
%type<filter>    opt_mp_filter_prefix_list
%type<filter>    mp_filter_prefix_list
%type<mpprefix>   mp_filter_prefix_list_prefix
%type<filter>    mp_peval

%type<filter>    v6_filter
%type<filter>    v6_filter_term
%type<filter>    v6_filter_factor
%type<filter>    v6_filter_operand
%type<filter>    v6_filter_prefix
%type<filter>    v6_filter_prefix_operand
%type<filter>    opt_v6_filter_prefix_list
%type<filter>    v6_filter_prefix_list
%type<mpprefix>   v6_filter_prefix_list_prefix

%type<filter>    opt_v6_inject_expr
%type<filter>    v6_inject_expr
%type<filter>    v6_inject_expr_term
%type<filter>    v6_inject_expr_factor
%type<filter>    v6_inject_expr_operand

%type<filter>    opt_as_expr
%type<filter>    as_expr
%type<filter>    as_expr_term
%type<filter>    as_expr_factor
%type<filter>    as_expr_operand

%type<filter>    opt_router_expr
%type<filter>    opt_router_expr_with_at
%type<filter>    router_expr
%type<filter>    router_expr_term
%type<filter>    router_expr_factor
%type<filter>    router_expr_operand

%type<filter>    opt_mp_router_expr
%type<filter>    opt_mp_router_expr_with_at
%type<filter>    mp_router_expr
%type<filter>    mp_router_expr_term
%type<filter>    mp_router_expr_factor
%type<filter>    mp_router_expr_operand

%type<filter>    opt_inject_expr
%type<filter>    inject_expr
%type<filter>    inject_expr_term
%type<filter>    inject_expr_factor
%type<filter>    inject_expr_operand

%type<re>    filter_aspath
%type<re>    filter_aspath_term
%type<re>    filter_aspath_closure
%type<re>    filter_aspath_factor
%type<re>    filter_aspath_no
%type<re>    filter_aspath_range

%type<actionList>      action
%type<actionList>      opt_action
%type<actionNode>      single_action

%type<peering>     peering
%type<peering>     mp_peering

%type<peeringActionList> import_peering_action_list
%type<peeringActionList> export_peering_action_list
%type<policyFactor>      import_factor
%type<policyTerm>        import_factor_list
%type<policyTerm>        import_term
%type<policyExpr>        import_expr
%type<policyFactor>      export_factor
%type<policyTerm>        export_factor_list
%type<policyTerm>        export_term
%type<policyExpr>        export_expr

%type<peeringActionList> mp_import_peering_action_list
%type<peeringActionList> mp_export_peering_action_list
%type<policyFactor>      mp_import_factor
%type<policyTerm>        mp_import_factor_list
%type<policyTerm>        mp_import_term
%type<policyExpr>        mp_import_expr
%type<policyFactor>      mp_export_factor
%type<policyTerm>        mp_export_factor_list
%type<policyTerm>        mp_export_term
%type<policyExpr>        mp_export_expr

%type<protocol>          opt_protocol_from
%type<protocol>          opt_protocol_into

%type<wordlist>    enum_list
%type<typenode>    typedef_type
%type<typelist>    typedef_type_list

%type<method>      method
%type<methodlist>      methods

%type<protocol_option> protocol_option
%type<protocol_option_list> protocol_options

%type<peer_option> peer_option
%type<peer_option_list> peer_options
%type<peer_option_list> opt_peer_options
%type<ip> peer_id
%type<mpprefix> mp_peer_id
%type<mpprefix> interface_address
%type<tunnel>   opt_tunnel_spec

%type<rpslattr>    opt_attr_options
%type<rpslattr>    attr_options
%type<rpslattr>    attr_option

%type<listMntPrfxPair>  mnt_routes_list
%type<mntPrfxPair>      mnt_routes_list_item
%type<listMntPrfxPair>  mnt_routes6_list
%type<mntPrfxPair>      mnt_routes6_list_item
%type<listMntPrfxPair>  mnt_routes_mp_list
%type<mntPrfxPair>      mnt_routes_mp_list_item

%%
object: attribute_list TKN_EOO {
   YYACCEPT;
}
| TKN_UNKNOWN_CLASS TKN_EOO {
   YYACCEPT;
}
| error TKN_EOO {
   handle_object_error("Error: syntax error\n");
   YYABORT;
}
| attribute_list { // end of file
   YYACCEPT;
}
| TKN_UNKNOWN_CLASS { // end of file
   YYACCEPT;
}
| error { // end of file
   handle_object_error("Error: syntax error\n");
   YYABORT;
}
| { // end of file
   YYABORT;
}
;

attribute_list: attribute {
   (*current_object) += $1;
}
| attribute_list attribute {
   (*current_object) += $2;
}
;

attribute: generic_attribute
| blobs_attribute
| regexp_attribute
| changed_attribute
| import_attribute
| mp_import_attribute
| export_attribute
| mp_export_attribute
| default_attribute
| mp_default_attribute
| peer_attribute
| mp_peer_attribute
| ifaddr_attribute
| interface_attribute
| components_attribute
| v6_components_attribute
| inject_attribute
| v6_inject_attribute
| aggr_mtd_attribute
| aggr_bndry_attribute
| typedef_attribute
| protocol_attribute
| rpattr_attribute
| rs_members_attribute
| rs_mp_members_attribute
| filter_attribute
| v6_filter_attribute
| mp_filter_attribute
| mp_peval_attribute
| peering_attribute
| mp_peering_attribute
| attr_attribute
| freetext_attribute
| mnt_routes_attribute
| mnt_routes6_attribute
| mnt_routes_mp_attribute
| TKN_ERROR TKN_EOA { // the current line started w/ non-attribute
   $$ = changeCurrentAttr(new Attr);
   handle_error("Error: syntax error\n");
}
| error TKN_EOA {
   $$ = changeCurrentAttr(new Attr);
   handle_error("Error: syntax error\n");
   yyerrok;
}
;

//**** Generic Attributes ************************************************

changed_attribute: ATTR_CHANGED TKN_EMAIL TKN_INT TKN_EOA {
   free($2);
   $$ = $1;
}
| ATTR_CHANGED error TKN_EOA {
   handle_error("Error: \"changed: <email> <YYYYMMDD>\" expected\n");
   yyerrok;
}
;

freetext_attribute: ATTR_FREETEXT TKN_EOA {
   char *start = strchr($1->object->contents + $1->offset, ':') + 1;
   int len = $1->object->contents + $1->offset + $1->len - start;
   ItemFREETEXT *ft = new ItemFREETEXT(start, len);
   ItemList *il = new ItemList;
   il->append(ft);
   
   $$ = changeCurrentAttr(new AttrGeneric($1->type, il));
}
;

regexp_attribute: ATTR_REGEXP TKN_EOA {
   char *start = strchr($1->object->contents + $1->offset, ':') + 1;
   int len = $1->object->contents + $1->offset + $1->len - start;

   while (isspace(*start)) {
      start++;
      len --;
   }

   char *comment = strrchr(start, '#');
   if (comment && comment < start+len)
      len = comment - start;

   while (isspace(*(start+len-1)))
      len --;

   Buffer b(len+1);
   b.append(start, len);
   b.append("", 1);

   regmatch_t *matches = (regmatch_t *) malloc(sizeof(regmatch_t) 
					       * ($1->type->reIndex + 1));
   int code = regexec($1->type->reComp, b.contents, 
		      $1->type->reIndex + 1, matches, 0);

   if (code) {
      handle_error("Error: argument to %s should match %s.\n",
		   $1->type->name(), $1->type->re);
      $$ = changeCurrentAttr(new AttrGeneric($1->type, NULL));
   } else {
      Buffer *buf = new Buffer(matches[$1->type->reIndex].rm_eo 
			       - matches[$1->type->reIndex].rm_so);
      buf->append(b.contents + matches[$1->type->reIndex].rm_so,buf->capacity);

      ItemBUFFER *item = new ItemBUFFER(buf);
      ItemList   *il = new ItemList;
      il->append(item);
   
      $$ = changeCurrentAttr(new AttrGeneric($1->type, il));
   }
}
;

generic_attribute: ATTR_GENERIC generic_list TKN_EOA {
   if (!$1->type->subsyntax()->validate($2)) {
      handle_error("Error: argument to %s should be %s.\n",
	      $1->type->name(), $1->type->subsyntax()->name());
      delete $2;
      $$ = changeCurrentAttr(new AttrGeneric($1->type, NULL));
   } else 
      $$ = changeCurrentAttr(new AttrGeneric($1->type, $2));
}
| ATTR_GENERIC error TKN_EOA {
   $$ = $1;
   handle_error("Error: argument to %s should be %s.\n",
	   $1->type->name(), $1->type->subsyntax()->name());
   yyerrok;
}
;

generic_list: /* empty list */ {
   $$ = new ItemList;
}
| generic_non_empty_list
;

generic_non_empty_list: list_item { 	
   $$ = new ItemList;
   $$->append($1);
}
| generic_non_empty_list ',' list_item {
   $$ = $1;
   $$->append($3);
}
;

blobs_attribute: ATTR_BLOBS blobs_list TKN_EOA {
   $$ = changeCurrentAttr(new AttrGeneric($1->type, $2));
}
| ATTR_BLOBS error TKN_EOA {
   $$ = $1;
   handle_error("Error: argument to %s should be blob sequence.\n",
	   $1->type->name());
   yyerrok;
}
;

blobs_list: list_item { 	
   $$ = new ItemList;
   $$->append($1);
}
| blobs_list list_item {
   $$ = $1;
   $$->append($2);
}
;

list_item: list_item_0 {
   $$ = $1;
}
| list_item_0 '-' list_item_0 {
   $$ = new ItemRange($1, $3);
}
;

list_item_0: TKN_ASNO {
   $$ = new ItemASNO($1);
}
| TKN_INT {
   $$ = new ItemINT($1);
}
| TKN_REAL {
   $$ = new ItemREAL($1);
}
| TKN_STRING {
   $$ = new ItemSTRING($1);
}
| TKN_TIMESTAMP {
   $$ = new ItemTimeStamp($1);
}
| TKN_IPV4 {
   $$ = new ItemIPV4($1);
}
| TKN_PRFXV4 {
   $$ = new ItemPRFXV4($1);
}
| TKN_PRFXV4RNG {
   $$ = new ItemPRFXV4Range($1);
}
| TKN_IPV6 {
  $$ = new ItemIPV6($1);
}
| TKN_PRFXV6 {
  $$ = new ItemPRFXV6($1);
}
| TKN_PRFXV6RNG {
  $$ = new ItemPRFXV6Range($1);
}
| TKN_IPV4 ':' TKN_INT {
   $$ = new ItemConnection($1, $3);
}
| TKN_DNS ':' TKN_INT {
   $$ = new ItemConnection($1, $3);
}
| TKN_ASNAME {
   $$ = new ItemASNAME($1);
}
| TKN_RSNAME {
   $$ = new ItemRSNAME($1);
}
| TKN_RTRSNAME {
   $$ = new ItemRTRSNAME($1);
}
| TKN_PRNGNAME {
   $$ = new ItemPRNGNAME($1);
}
| TKN_FLTRNAME {
   $$ = new ItemFLTRNAME($1);
}
| TKN_BOOLEAN {
   $$ = new ItemBOOLEAN($1);
}
| TKN_WORD {
   $$ = new ItemWORD($1);
}
| tkn_word_from_keyw {
   $$ = new ItemWORD($1);
}
| TKN_DNS {
   $$ = new ItemDNS($1);
}
| TKN_EMAIL {
   $$ = new ItemEMAIL($1);
}
| TKN_BLOB {
   $$ = new ItemBLOB($1);
}
| '{' generic_list '}'  {
   $$ = $2;
}
| '(' filter ')' {
   $$ = new ItemFilter($2);
}
;

tkn_word: TKN_WORD {
   $$ = $1;
}
| TKN_ASNO {
   char buffer[64];
   sprintf(buffer, "AS%d", $1);
   $$ = strdup(buffer);
}
| TKN_ASNAME {
   $$ = strdup($1);
}
| TKN_RSNAME {
   $$ = strdup($1);
}
| TKN_RTRSNAME {
   $$ = strdup($1);
}
| TKN_PRNGNAME {
   $$ = strdup($1);
}
| TKN_FLTRNAME {
   $$ = strdup($1);
}
| TKN_BOOLEAN {
   if ($1)
      $$ = strdup("true");
   else
      $$ = strdup("false");
}
| tkn_word_from_keyw
;

tkn_word_from_keyw: KEYW_TRUE {
   $$ = strdup("true");
}
| KEYW_FALSE {
   $$ = strdup("false");
}
| KEYW_ACTION {
   $$ = strdup("action");
}
| KEYW_ACCEPT {
   $$ = strdup("accept");
}
| KEYW_ANNOUNCE {
   $$ = strdup("announce");
}
| KEYW_FROM {
   $$ = strdup("from");
}
| KEYW_TO {
   $$ = strdup("to");
}
| KEYW_AT {
   $$ = strdup("at");
}
| KEYW_ANY {
   $$ = strdup("any");
}
| KEYW_REFINE {
   $$ = strdup("refine");
}
| KEYW_EXCEPT {
   $$ = strdup("except");
}
| KEYW_STATIC {
   $$ = strdup("static");
}
| KEYW_NETWORKS {
   $$ = strdup("networks");
}
| KEYW_MASKLEN {
   $$ = strdup("masklen");
}
| KEYW_UNION {
   $$ = strdup("union");
}
| KEYW_RANGE {
   $$ = strdup("range");
}
| KEYW_LIST {
   $$ = strdup("list");
}
| KEYW_OF {
   $$ = strdup("of");
}
| KEYW_OPERATOR {
   $$ = strdup("operator");
}
| KEYW_SYNTAX {
   $$ = strdup("syntax");
}
| KEYW_SPECIAL {
   $$ = strdup("special");
}
| KEYW_REGEXP {
   $$ = strdup("regexp");
}
| KEYW_OPTIONAL {
   $$ = strdup("optional");
}
| KEYW_MANDATORY {
   $$ = strdup("mandatory");
}
| KEYW_INTERNAL {
   $$ = strdup("internal");
}
| KEYW_DELETED {
   $$ = strdup("deleted");
}
| KEYW_SINGLEVALUED {
   $$ = strdup("singlevalued");
}
| KEYW_MULTIVALUED {
   $$ = strdup("multivalued");
}
| KEYW_LOOKUP {
   $$ = strdup("lookup");
}
| KEYW_KEY {
   $$ = strdup("key");
}
| KEYW_OBSOLETE {
   $$ = strdup("obsolete");
}
| KEYW_PEERAS {
   $$ = strdup("peeras");
}
| KEYW_PROTOCOL {
   $$ = strdup("protocol");
}
| KEYW_INTO {
   $$ = strdup("into");
}
| KEYW_ATOMIC {
   $$ = strdup("atomic");
}
| KEYW_INBOUND {
   $$ = strdup("inbound");
}
| KEYW_OUTBOUND {
   $$ = strdup("outbound");
}
;

//**** aut-num class ******************************************************

//// as_expr ////////////////////////////////////////////////////////////

opt_as_expr: {
   $$ = new FilterASNAME(symbols.symID("AS-ANY"));
}
| as_expr 
;

as_expr: as_expr OP_OR as_expr_term {
   $$ = new FilterOR($1, $3);
}
| as_expr_term
;

as_expr_term: as_expr_term OP_AND as_expr_factor {
   $$ = new FilterAND($1, $3);
}
| as_expr_term KEYW_EXCEPT as_expr_factor {
   $$ = new FilterEXCEPT($1, $3);
}
| as_expr_factor
;

as_expr_factor: '(' as_expr ')' {
   $$ = $2;
}
| as_expr_operand 
;

as_expr_operand: TKN_ASNO {
   $$ = new FilterASNO($1);
}
| TKN_ASNAME {
   $$ = new FilterASNAME($1);
}
;

//// router_expr ///////////////////////////////////////////////////////////

opt_router_expr: {
   $$ = new FilterANY;
}
| router_expr {
   $$ = $1;
}
;

opt_router_expr_with_at: {
   $$ = new FilterANY;
}
| KEYW_AT router_expr {
   $$ = $2;
}
;

router_expr: router_expr OP_OR router_expr_term {
   $$ = new FilterOR($1, $3);
}
| router_expr_term
;

router_expr_term: router_expr_term OP_AND router_expr_factor {
   $$ = new FilterAND($1, $3);
}
| router_expr_term KEYW_EXCEPT router_expr_factor {
   $$ = new FilterEXCEPT($1, $3);
}
| router_expr_factor
;

router_expr_factor: '(' router_expr ')' {
   $$ = $2;
}
| router_expr_operand 
;

router_expr_operand: TKN_IPV4 {
   $$ = new FilterRouter($1);
}
| TKN_DNS {
   $$ = new FilterRouterName($1);
}
| TKN_RTRSNAME {
   $$ = new FilterRTRSNAME($1);
}
;

//// peering ////////////////////////////////////////////////////////////

peering: as_expr opt_router_expr opt_router_expr_with_at {
   $$ = new PolicyPeering($1, $2, $3);
}
| TKN_PRNGNAME {
   $$ = new PolicyPeering($1);
} 
;

//// action /////////////////////////////////////////////////////////////

opt_action: {
   $$ = new PolicyActionList;
}
| KEYW_ACTION action {
   $$ = $2;
}
;

action: single_action {
   $$ = new PolicyActionList;
   if ($1)
      $$->append($1);  
}
| action single_action {
   $$ = $1;
   if ($2)
      $$->append($2);  
}
;

single_action: TKN_RP_ATTR '.' TKN_WORD '(' generic_list ')' ';' {
   const AttrMethod *mtd = searchMethod($1, $3, $5);
   if (mtd)
      $$ = new PolicyAction($1, mtd, $5);
   else {
      delete $5;
      $$ = NULL;
   }
   free($3);
}
| TKN_RP_ATTR TKN_OPERATOR list_item ';'  {
   ItemList *plist = new ItemList;
   plist->append($3);

   const AttrMethod *mtd = searchMethod($1, $2, plist);
   if (mtd)
      $$ = new PolicyAction($1, mtd, plist);
   else {
      delete plist;
      $$ = NULL;
   }
   // Added by wlee
   free($2);
}
| TKN_RP_ATTR '(' generic_list ')' ';' {
   const AttrMethod *mtd = searchMethod($1, "()", $3);
   if (mtd)
      $$ = new PolicyAction($1, mtd, $3);
   else {
      delete $3;
      $$ = NULL;
   }
}
| TKN_RP_ATTR '[' generic_list ']' ';' {
   const AttrMethod *mtd = searchMethod($1, "[]", $3);
   if (mtd)
      $$ = new PolicyAction($1, mtd, $3);
   else {
      delete $3;
      $$ = NULL;
   }
}
| ';' {
   $$ = NULL;
}
;

//// filter /////////////////////////////////////////////////////////////
//// default afi = ipv4

filter: filter OP_OR filter_term {
   $$ = new FilterOR($1, $3);
}
| filter filter_term %prec OP_OR {
   $$ = new FilterOR($1, $2);
}
| filter_term
;

filter_term : filter_term OP_AND filter_factor {
   $$ = new FilterAND($1, $3);
}
| filter_factor
;

filter_factor :  OP_NOT filter_factor {
   $$ = new FilterNOT($2);
}
| '(' filter ')' {
   $$ = $2;
}
| filter_operand 
;

filter_operand: KEYW_ANY {
   $$ = new FilterANY;
}
| '<' filter_aspath '>' {
   $$ = new FilterASPath($2);
}
| filter_rp_attribute {
   if ($1)
      $$ = $1;
   else
      $$ = new FilterNOT(new FilterANY);
}
| TKN_FLTRNAME {
   $$ = new FilterFLTRNAME($1);
}
| filter_prefix
;

filter_prefix: filter_prefix_operand OP_MS {
   $2->f1 = $1;
   $$ = $2;
}
|  filter_prefix_operand
;

filter_prefix_operand: TKN_ASNO {
   $$ = new FilterASNO($1);
}
| KEYW_PEERAS {
   $$ = new FilterPeerAS;
}
| TKN_ASNAME {
   $$ = new FilterASNAME($1);
}
| TKN_RSNAME {
   $$ = new FilterRSNAME($1);
}
| '{' opt_filter_prefix_list '}' { 
   $$ = $2; 
}
;

opt_filter_prefix_list: {
   $$ = new FilterPRFXList;
}
| filter_prefix_list {
  $$ = $1;
}
;

filter_prefix_list: filter_prefix_list_prefix {
   ((FilterPRFXList *) ($$ = new FilterPRFXList))->add_high(*$1);
   delete $1;
}
| filter_prefix_list ',' filter_prefix_list_prefix {
   $$ = $1;
   ((FilterPRFXList *) ($$))->add_high(*$3);
   delete $3;
}
;

filter_prefix_list_prefix: TKN_PRFXV4 {
   $$ = $1;
}
| TKN_PRFXV4RNG {
   $$ = $1;
}
;

filter_aspath: filter_aspath '|' filter_aspath_term {
   $$ = new regexp_or($1, $3);
}
| filter_aspath_term
;

filter_aspath_term: filter_aspath_term filter_aspath_closure {
   $$ = new regexp_cat($1, $2);
}
| filter_aspath_closure
;

filter_aspath_closure: filter_aspath_closure '*' {
   $$ = new regexp_star($1);
}
| filter_aspath_closure '?' {
   $$ = new regexp_question($1);
}
| filter_aspath_closure '+' {
   $$ = new regexp_plus($1);
}
| filter_aspath_no '~' '+' {
   $$ = new regexp_tildaplus($1);
}
| filter_aspath_no '~' '*' {
   $$ = new regexp_tildastar($1);
}
| filter_aspath_factor
;

filter_aspath_factor: '^' {
   $$ = new regexp_bol;
}
| '$' {
   $$ = new regexp_eol;
}
| '(' filter_aspath ')' {
   $$ = $2;
}
| filter_aspath_no
;

filter_aspath_no: TKN_ASNO {
   $$ = new regexp_symbol($1);
}
| KEYW_PEERAS {
   $$ = new regexp_symbol(symbols.symID("PEERAS"));
}
| TKN_ASNAME {
   $$ = new regexp_symbol($1);
}
| '.' {
   $$ = new regexp_symbol(regexp_symbol::MIN_AS, regexp_symbol::MAX_AS);
}
| '[' filter_aspath_range ']' {
   $$ = $2;
}
| '[' '^' filter_aspath_range ']' {
   $$ = $3;
   ((regexp_symbol *) $$)->complement();
}
;

filter_aspath_range: {
   $$ = new regexp_symbol;
}
| filter_aspath_range TKN_ASNO {
   ((regexp_symbol *) ($$ = $1))->add($2);
}
| filter_aspath_range KEYW_PEERAS {
   ((regexp_symbol *) ($$ = $1))->add(symbols.symID("PEERAS"));
}
| filter_aspath_range '.' {
   ((regexp_symbol *) ($$ = $1))->add(regexp_symbol::MIN_AS, regexp_symbol::MAX_AS);
}
| filter_aspath_range TKN_ASNO '-' TKN_ASNO {
   ((regexp_symbol *) ($$ = $1))->add($2, $4);
}
| filter_aspath_range TKN_ASNAME {
   ((regexp_symbol *) ($$ = $1))->add($2);
}
;

filter_rp_attribute: TKN_RP_ATTR '.' TKN_WORD '(' generic_list ')' {
   const AttrMethod *mtd = searchMethod($1, $3, $5);
   if (mtd)
      $$ = new FilterRPAttribute($1, mtd, $5);
   else {
      delete $5;
      $$ = NULL;
   }
   free($3);
}
| TKN_RP_ATTR TKN_OPERATOR list_item {
   ItemList *plist = new ItemList;
   plist->append($3);

   const AttrMethod *mtd = searchMethod($1, $2, plist);
   if (mtd)
      $$ = new FilterRPAttribute($1, mtd, plist);
   else {
      delete plist;
      $$ = NULL;
   }
   // Added by wlee
   free($2);
}
| TKN_RP_ATTR '(' generic_list ')' {
   const AttrMethod *mtd = searchMethod($1, "()", $3);
   if (mtd)
      $$ = new FilterRPAttribute($1, mtd, $3);
   else {
      delete $3;
      $$ = NULL;
   }
}
| TKN_RP_ATTR '[' generic_list ']' {
   const AttrMethod *mtd = searchMethod($1, "[]", $3);
   if (mtd)
      $$ = new FilterRPAttribute($1, mtd, $3);
   else {
      delete $3;
      $$ = NULL;
   }
}
;

//// peering action pair ////////////////////////////////////////////////

import_peering_action_list: KEYW_FROM peering opt_action {
   $$ = new List<PolicyPeeringAction>;
   $$->append(new PolicyPeeringAction($2, $3));
}
| import_peering_action_list KEYW_FROM peering opt_action {
   $$ = $1;
   $$->append(new PolicyPeeringAction($3, $4));
}
;

export_peering_action_list: KEYW_TO peering opt_action {
   $$ = new List<PolicyPeeringAction>;
   $$->append(new PolicyPeeringAction($2, $3));
}
| export_peering_action_list KEYW_TO peering opt_action {
   $$ = $1;
   $$->append(new PolicyPeeringAction($3, $4));
}
;

//// import/export factor ///////////////////////////////////////////////

import_factor: import_peering_action_list KEYW_ACCEPT filter  {
   $$ = new PolicyFactor($1, $3);
}
;

import_factor_list: import_factor ';' {
   $$ = new PolicyTerm;
   $$->append($1);
}
| import_factor_list import_factor ';' {
   $$ = $1;
   $$->append($2);
}
;

export_factor: export_peering_action_list KEYW_ANNOUNCE filter  {
   $$ = new PolicyFactor($1, $3);
}
;

export_factor_list: export_factor ';' {
   $$ = new PolicyTerm;
   $$->append($1);
}
| export_factor_list export_factor ';' {
   $$ = $1;
   $$->append($2);
}
;

//// import/export term /////////////////////////////////////////////////

import_term: import_factor ';' {
   PolicyTerm *term = new PolicyTerm;
   term->append($1);
   $$ = term;
}
| '{' import_factor_list '}' {
   $$ = $2;
}
;

export_term: export_factor ';' {
   PolicyTerm *term = new PolicyTerm;
   term->append($1);
   $$ = term;
}
| '{' export_factor_list '}' {
   $$ = $2;
}
;

//// import/export expr /////////////////////////////////////////////////

import_expr: import_term {
   $$ = $1;
}
| import_term KEYW_REFINE import_expr {
   $$ = new PolicyRefine($1, $3);
}
| import_term KEYW_EXCEPT import_expr {
   $$ = new PolicyExcept($1, $3);
}
;

export_expr: export_term {
   $$ = $1;
}
| export_term KEYW_REFINE export_expr {
   $$ = new PolicyRefine($1, $3);
}
| export_term KEYW_EXCEPT export_expr {
   $$ = new PolicyExcept($1, $3);
}
;

//// protocol ///////////////////////////////////////////////////////////

opt_protocol_from: {
   $$ = schema.searchProtocol("BGP4");
}
| KEYW_PROTOCOL tkn_word {
   $$ = schema.searchProtocol($2);
   if (!$$) {
      handle_warning("Warning: unknown protocol %s, BGP4 assumed.\n", $2);
      $$ = schema.searchProtocol("BGP4");
   }
   free($2);
}
;

opt_protocol_into: {
   $$ = schema.searchProtocol("BGP4");
}
| KEYW_INTO tkn_word {
   $$ = schema.searchProtocol($2);
   if (!$$) {
      handle_warning("Warning: unknown protocol %s, BGP4 assumed.\n", $2);
      $$ = schema.searchProtocol("BGP4");
   }
   free($2);;
}
;
//**** mp-import/mp-export attributes TBD *************************************

mp_import_attribute: ATTR_MP_IMPORT
                     opt_protocol_from opt_protocol_into
                     KEYW_AFI afi_list
                     mp_import_expr TKN_EOA {
  $$ = changeCurrentAttr(new AttrImport($2, $3, $5, $6));
}
| ATTR_MP_IMPORT opt_protocol_from opt_protocol_into KEYW_AFI afi_list error TKN_EOA {
   handle_error ("Error: in peering/filter specification\n");
   yyerrok;
}
| ATTR_MP_IMPORT opt_protocol_from opt_protocol_into KEYW_AFI error TKN_EOA {
  handle_error ("Error: in afi specification\n");
  yyerrok;
}
| ATTR_MP_IMPORT error TKN_EOA {
   $$ = $1;
   handle_error("Error: wrong mp-import.\n");
   yyerrok;
}
;

mp_export_attribute: ATTR_MP_EXPORT
                     opt_protocol_from opt_protocol_into
                     KEYW_AFI afi_list
                     mp_export_expr TKN_EOA {
  $$ = changeCurrentAttr(new AttrExport($2, $3, $5, $6));
}
| ATTR_MP_EXPORT opt_protocol_from opt_protocol_into KEYW_AFI afi_list error TKN_EOA {
   handle_error ("Error: in peering specification\n");
   yyerrok;
}
| ATTR_MP_EXPORT opt_protocol_from opt_protocol_into KEYW_AFI error TKN_EOA {
  handle_error ("Error: in afi specification\n");
  yyerrok;
}
| ATTR_MP_EXPORT error TKN_EOA {
   $$ = $1;
   handle_error("Error: wrong mp-export.\n");
   yyerrok;
}
;
///////// mp-import/mp-export expr TBD //////////////

mp_import_expr: mp_import_term {
   $$ = $1;
}
| mp_import_term KEYW_REFINE mp_import_expr {
   $$ = new PolicyRefine($1, $3);
}  
| mp_import_term KEYW_EXCEPT mp_import_expr {
   $$ = new PolicyExcept($1, $3);
}
;

mp_export_expr: mp_export_term {
   $$ = $1;
}
| mp_export_term KEYW_REFINE mp_export_expr {
   $$ = new PolicyRefine($1, $3);
}
| mp_export_term KEYW_EXCEPT mp_export_expr {
   $$ = new PolicyExcept($1, $3);
}
;

///////  mp-import/mp-export term TBD //////////////////

mp_import_term: mp_import_factor ';' {
   PolicyTerm *term = new PolicyTerm;
   term->append($1);
   $$ = term;
}
| '{' mp_import_factor_list '}' {
   $$ = $2;
}
;

mp_export_term: mp_export_factor ';' {
   PolicyTerm *term = new PolicyTerm;
   term->append($1);
   $$ = term;
}
| '{' mp_export_factor_list '}' {
   $$ = $2;
}
;

//// mp-import/mp-export factor TBD ///////////////////////////////////////////////

mp_import_factor: mp_import_peering_action_list KEYW_ACCEPT mp_filter  {
   $$ = new PolicyFactor($1, $3);
}
;

mp_import_factor_list: mp_import_factor ';' {
   $$ = new PolicyTerm;
   $$->append($1);
}
| mp_import_factor_list mp_import_factor ';' {
   $$ = $1;
   $$->append($2);
}
;

mp_export_factor: mp_export_peering_action_list KEYW_ANNOUNCE mp_filter  {
   $$ = new PolicyFactor($1, $3);
}
;

mp_export_factor_list: mp_export_factor ';' {
   $$ = new PolicyTerm;
   $$->append($1);
}
| mp_export_factor_list mp_export_factor ';' {
   $$ = $1;
   $$->append($2);
}
;

//// mp-import-mp-export peering action pair TBD ///////////////////////////

mp_import_peering_action_list: KEYW_FROM mp_peering opt_action {
   $$ = new List<PolicyPeeringAction>;
   $$->append(new PolicyPeeringAction($2, $3));
}
| mp_import_peering_action_list KEYW_FROM mp_peering opt_action {
   $$ = $1;
   $$->append(new PolicyPeeringAction($3, $4));
}
;

mp_export_peering_action_list: KEYW_TO mp_peering opt_action {
   $$ = new List<PolicyPeeringAction>;
   $$->append(new PolicyPeeringAction($2, $3));
}
| mp_export_peering_action_list KEYW_TO mp_peering opt_action {
   $$ = $1;
   $$->append(new PolicyPeeringAction($3, $4));
}
;

//// mp-filter //////////////////////

mp_filter: mp_filter OP_OR mp_filter_term {
   $$ = new FilterOR($1, $3);
}
| mp_filter mp_filter_term %prec OP_OR {
   $$ = new FilterOR($1, $2);
}
| mp_filter_term
;

mp_filter_term : mp_filter_term OP_AND mp_filter_factor {
   $$ = new FilterAND($1, $3);
}
| mp_filter_factor
;

mp_filter_factor :  OP_NOT mp_filter_factor {
   $$ = new FilterNOT($2);
}
| '(' mp_filter ')' {
   $$ = $2;
}
| mp_filter_operand 
;

mp_filter_operand: KEYW_ANY {
   $$ = new FilterANY;
}
| '<' filter_aspath '>' {
   $$ = new FilterASPath($2);
}
| filter_rp_attribute {
   if ($1)
      $$ = $1;
   else
      $$ = new FilterNOT(new FilterANY);
}
| TKN_FLTRNAME {
   $$ = new FilterFLTRNAME($1);
}
| mp_filter_prefix
;

mp_filter_prefix: mp_filter_prefix_operand OP_MS {
   $2->f1 = $1;
   $$ = $2;
}
|  mp_filter_prefix_operand
;

mp_filter_prefix_operand: TKN_ASNO {
   $$ = new FilterASNO($1);
}
| KEYW_PEERAS {
   $$ = new FilterPeerAS;
}
| TKN_ASNAME {
   $$ = new FilterASNAME($1);
}
| TKN_RSNAME {
   $$ = new FilterRSNAME($1);
}
| '{' opt_mp_filter_prefix_list '}' { 
   $$ = $2; 
}
;

opt_mp_filter_prefix_list: {
   $$ = new FilterMPPRFXList;
}
| mp_filter_prefix_list
;

mp_filter_prefix_list: mp_filter_prefix_list_prefix {

   ((FilterMPPRFXList *) ($$ = new FilterMPPRFXList))->push_back(*$1);
}
| mp_filter_prefix_list ',' mp_filter_prefix_list_prefix {
   $$ = $1;
   ((FilterMPPRFXList *) ($$))->push_back(*$3);
}
;

mp_filter_prefix_list_prefix: TKN_PRFXV6 {
  $$ = new MPPrefix($1);
}
| TKN_PRFXV6RNG {
  $$ = new MPPrefix($1);
}
| TKN_PRFXV4 {
  $$ = new MPPrefix($1);
}
| TKN_PRFXV4RNG {
  $$ = new MPPrefix($1);
}
;

//// mp-peering TBD /////////////////////

mp_peering: as_expr opt_mp_router_expr opt_mp_router_expr_with_at {
   $$ = new PolicyPeering($1, $2, $3);
// need to check here that afi in opt_mp_router_expr == afi in opt_mp_router_expr_with_at
   if (typeid(*$2) != typeid(*$3)) {
     handle_error ("Error: routers address families mismatch\n");
     yyerrok;
   }
}
| TKN_PRNGNAME {
   $$ = new PolicyPeering($1);
} 
;

//// opt_mp_router_expr/with_at ///////////

opt_mp_router_expr: {
   $$ = new FilterANY;
}
| mp_router_expr {
   $$ = $1;
}
;

opt_mp_router_expr_with_at: {
   $$ = new FilterANY;
}
| KEYW_AT mp_router_expr {
   $$ = $2;
}
;

mp_router_expr: mp_router_expr OP_OR mp_router_expr_term {
   $$ = new FilterOR($1, $3);
}
| mp_router_expr_term
;

mp_router_expr_term: mp_router_expr_term OP_AND mp_router_expr_factor {
   $$ = new FilterAND($1, $3);
}
| mp_router_expr_term KEYW_EXCEPT mp_router_expr_factor {
   $$ = new FilterEXCEPT($1, $3);
}
| mp_router_expr_factor
;

mp_router_expr_factor: '(' mp_router_expr ')' {
   $$ = $2;
}
| mp_router_expr_operand
;

mp_router_expr_operand: TKN_IPV4 {
   $$ = new FilterRouter($1);
}
| TKN_IPV6 {
   $$ = new FilterIPv6Router($1);
}
| TKN_DNS {
   $$ = new FilterRouterName($1);
}
| TKN_RTRSNAME {
   $$ = new FilterRTRSNAME($1);
}
;

//**** import/export attributes *******************************************

import_attribute: ATTR_IMPORT 
                  opt_protocol_from opt_protocol_into 
                  import_expr TKN_EOA {
   $$ = changeCurrentAttr(new AttrImport($2, $3, $4));
}
| ATTR_IMPORT opt_protocol_from opt_protocol_into import_factor TKN_EOA {
   PolicyTerm *term = new PolicyTerm;
   term->append($4);

   $$ = changeCurrentAttr(new AttrImport($2, $3, term));
}
| ATTR_IMPORT error TKN_EOA {
   $$ = $1;
   handle_error("Error: from <peering> expected.\n");
   yyerrok;
}
;

export_attribute: ATTR_EXPORT 
                  opt_protocol_from opt_protocol_into 
                  export_expr TKN_EOA {
   $$ = changeCurrentAttr(new AttrExport($2, $3, $4));
}
| ATTR_EXPORT opt_protocol_from opt_protocol_into export_factor TKN_EOA {
   PolicyTerm *term = new PolicyTerm;
   term->append($4);

   $$ = changeCurrentAttr(new AttrExport($2, $3, term));
}
| ATTR_EXPORT error TKN_EOA {
   $$ = $1;
   handle_error("Error: to <peering> expected.\n");
   yyerrok;
}
;

opt_default_filter: {
   $$ = new FilterANY;
}
| KEYW_NETWORKS filter {
   $$ = $2;
}
;

// **** default attribute

default_attribute: ATTR_DEFAULT KEYW_TO peering 
                                opt_action 
                                opt_default_filter TKN_EOA {
   $$ = changeCurrentAttr(new AttrDefault($3, $4, $5));
}
| ATTR_DEFAULT KEYW_TO peering error TKN_EOA {
   if ($3)
      delete $3;
   handle_error("Error: badly formed filter/action or keyword NETWORKS/ACTION missing.\n");
   yyerrok;
}
| ATTR_DEFAULT error TKN_EOA {
   handle_error("Error: TO <peer> missing.\n");
   yyerrok;
}
;

// **** mp-default attribute TBD *******************///

mp_default_attribute: ATTR_MP_DEFAULT KEYW_AFI afi_list 
                                KEYW_TO mp_peering
                                opt_action
                                opt_default_mp_filter TKN_EOA {
   $$ = changeCurrentAttr(new AttrDefault($3, $5, $6, $7));
}
| ATTR_MP_DEFAULT KEYW_AFI afi_list KEYW_TO mp_peering error TKN_EOA {
   if ($5)
      delete $5;
   handle_error("Error: badly formed filter/action or keyword NETWORKS/ACTION missing.\n");
   yyerrok;
}
| ATTR_MP_DEFAULT error TKN_EOA {
   handle_error("Error: TO <peer> missing. in mp-default\n");
   yyerrok;
}
;

opt_default_mp_filter: {
   $$ = new FilterANY;
}
| KEYW_NETWORKS mp_filter {
   $$ = $2;
}
;

/// filter specification //////////////////////////

filter_attribute: ATTR_FILTER filter TKN_EOA {
  // check that "mp-filter:" is not present
   if (current_object->hasAttr("mp-filter")) {
     handle_error("Error: mp-filter and filter attributes can't be used together\n");
     yyerrok;
    } else {
      //$$ = changeCurrentAttr(new AttrFilter($2));
      $$ = changeCurrentAttr(new AttrFilter(new FilterAFI(new ItemAFI(new AddressFamily("ipv4")), $2)));
    }
}
| ATTR_FILTER error TKN_EOA {
   $$ = $1;
   handle_error("Error: badly formed filter.\n");
   yyerrok;
}
;

// for peval

mp_peval_attribute: ATTR_MP_PEVAL mp_peval TKN_EOA {
   $$ = changeCurrentAttr(new AttrMPPeval($2));
}
| ATTR_MP_PEVAL error TKN_EOA {
   $$ = $1;
   handle_error("Error: badly formed filter in mp-peval.\n");
   yyerrok;
}
;

mp_peval: KEYW_AFI afi_list mp_filter  {
   $$ = new FilterAFI((ItemList *) $2, $3);
}
;

/// mp-filter attribute ///

mp_filter_attribute: ATTR_MP_FILTER mp_filter TKN_EOA {
  // check that "filter:" is not present
   if (current_object->hasAttr("filter")) {
    handle_error("Error: mp-filter and filter attributes can't be used together\n");
    yyerrok;
  } else {
    $$ = changeCurrentAttr(new AttrFilter($2));
  }
}
| ATTR_MP_FILTER error TKN_EOA {
   $$ = $1;
   handle_error("Error: badly formed filter in mp-filter.\n");
   yyerrok;
}  
;  

//** peering attribute *****//

peering_attribute: ATTR_PEERING peering TKN_EOA {
   $$ = changeCurrentAttr(new AttrPeering($2));
}
| ATTR_PEERING error TKN_EOA {
   $$ = $1;
   handle_error("Error: badly formed filter.\n");
   yyerrok;
}
;

//** mp-peering attribute TBD *****//

mp_peering_attribute: ATTR_MP_PEERING mp_peering TKN_EOA {
   $$ = changeCurrentAttr(new AttrMPPeering($2));
}
| ATTR_MP_PEERING error TKN_EOA {
   $$ = $1;
   handle_error("Error: badly formed peering.\n");
   yyerrok;
}
;

//**** inet-rtr class *****************************************************

////// ifaddr attribute ///////////////////////////////////////////////////

ifaddr_attribute: ATTR_IFADDR TKN_IPV4 KEYW_MASKLEN TKN_INT opt_action TKN_EOA {
   $$ = changeCurrentAttr(new AttrIfAddr($2->get_ipaddr(), $4, $5));
   delete $2;
}
| ATTR_IFADDR TKN_IPV4 KEYW_MASKLEN TKN_INT error TKN_EOA {
   delete $2;
   $$ = $1;
   handle_error("Error: in action specification.\n");
   yyerrok;
}
| ATTR_IFADDR TKN_IPV4 KEYW_MASKLEN error TKN_EOA {
   delete $2;
   $$ = $1;
   handle_error("Error: integer mask length expected.\n");
   yyerrok;
}
| ATTR_IFADDR TKN_IPV4 error TKN_EOA {
   delete $2;
   $$ = $1;
   handle_error("Error: MASKLEN <length> expected.\n");
   yyerrok;
}
| ATTR_IFADDR error TKN_EOA {
   $$ = $1;
   handle_error("Error: <ip_address> MASKLEN <length> [<action>] expected.\n");
   yyerrok;
}
;

////// interface attribute TBD ///////////////////////////////////////////

interface_address: TKN_IPV4 {
  $$ = new MPPrefix($1);
}
| TKN_IPV6 {
  $$ = new MPPrefix($1);
}
;

opt_tunnel_spec: {
}
| KEYW_TUNNEL interface_address ',' TKN_WORD {
  $$ = new Tunnel($2, new ItemWORD($4));
  if (! (schema.searchTypedef("encapsulation"))->validate(new ItemWORD($4))) {
     delete $$;
     handle_error("Error: wrong encapsulation specified.\n");
     yyerrok;
  }
}
;

interface_attribute: ATTR_INTERFACE interface_address KEYW_MASKLEN TKN_INT 
                     opt_action
                     opt_tunnel_spec TKN_EOA {
   $$ = changeCurrentAttr(new AttrInterface($2, $4, $5, $6));
   //delete $2;
   if ($6) {
      if (($2->ipv4 && $6->remote_ip->ipv6) || ($2->ipv6 && $6->remote_ip->ipv4)) {
        handle_error("Error: address family mismatch in local/remote tunnel endpoint\n");
        yyerrok;
      }
   }
}
| ATTR_INTERFACE interface_address KEYW_MASKLEN error TKN_EOA {
   $$ = $1;
   handle_error("Error: integer mask length expected.\n");
   yyerrok;
}  
| ATTR_INTERFACE interface_address error TKN_EOA {
   $$ = $1;
   handle_error("Error: MASKLEN <length> expected.\n");
   yyerrok;
}
| ATTR_INTERFACE error TKN_EOA {
   $$ = $1;
   handle_error("Error: <ip_address> MASKLEN <length> [<action>] [<tunnel>] expected.\n");
   yyerrok;
}
;

//// peer attribute /////////////////////////////////////////////////////

opt_peer_options: {
   $$ = new List<AttrPeerOption>;
}
| peer_options {
   $$ = $1;
}
;

peer_options: peer_option {
   $$ = new List<AttrPeerOption>;
   $$->append($1);
}
| peer_options ',' peer_option {
   $$ = $1;
   $$->append($3);
}
;

peer_option: tkn_word '(' generic_list ')' {
   $$ = new AttrPeerOption($1, $3);
}
;

peer_id: TKN_IPV4
| TKN_DNS  {
   $$ = new IPAddr;
}
| TKN_RTRSNAME  {
   $$ = new IPAddr;
}
| TKN_PRNGNAME {
   $$ = new IPAddr;
}
;

peer_attribute: ATTR_PEER tkn_word peer_id opt_peer_options TKN_EOA {
   const AttrProtocol *protocol = schema.searchProtocol($2);
   int position;
   const RPType *correctType;
   bool error = false;

   if (!protocol) {
      handle_error("Error: unknown protocol %s.\n", $2);
      error = true;
   } else {
      ((AttrProtocol *) protocol)->startMandatoryCheck();
      for (AttrPeerOption *opt = $4->head(); opt; opt = $4->next(opt)) {
	 const AttrProtocolOption *decl = protocol->searchOption(opt->option);
	 if (!decl)  {
	    handle_error("Error: protocol %s does not have option %s.\n", 
			 $2, opt->option);
	    error = true;
	 } else {
	    for (; decl; decl = protocol->searchNextOption(decl))
	       if (decl->option->validateArgs(opt->args, position, correctType))
		  break;
	    if (! decl) {
	       if (! (!strcasecmp(protocol->name, "BGP4")
		      && !strcasecmp(opt->option, "asno")
		      && opt->args->isSingleton()
		      && typeid(*opt->args->head()) == typeid(ItemWORD)
		      && !strcasecmp(((ItemWORD *) opt->args->head())->word,
				     "peeras"))) {
		  handleArgumentTypeError($2, opt->option, position, 
					  correctType);
		  error = true;
	       }
	    }
	 }
      }
   }

   if (! error) {
      const AttrProtocolOption *missing = 
	 ((AttrProtocol *) protocol)->missingMandatoryOption();
      if (missing) {
	 handle_error("Error: mandatory option %s of protocol %s is missing.\n", 
		      missing->option->name, $2);
	 error = true;
      }
   }
      
   if (!error)
      $$ = changeCurrentAttr(new AttrPeer(protocol, $3, $4));
   else {
      free($2);
      delete $3;
      delete $4;
   }
}
| ATTR_PEER tkn_word TKN_IPV4 error TKN_EOA {
   $$ = $1;
   free($2);
   delete $3;
   handle_error("Error: in peer option.\n");
   yyerrok;
}
| ATTR_PEER tkn_word error TKN_EOA {
   $$ = $1;
   free($2);
   handle_error("Error: missing peer ip_address.\n");
   yyerrok;
}
| ATTR_PEER error TKN_EOA {
   $$ = $1;
   handle_error("Error: missing protocol name.\n");
   yyerrok;
}
;

// mp-peer attribute TBD *****************//

mp_peer_attribute: ATTR_MP_PEER TKN_WORD mp_peer_id opt_peer_options TKN_EOA { 
   const AttrProtocol *protocol = schema.searchProtocol($2);
   int position;
   const RPType *correctType;
   bool error = false;

   if (!protocol) {
      handle_error("Error: unknown protocol %s.\n", $2);
      error = true;
   } else {
      ((AttrProtocol *) protocol)->startMandatoryCheck();
      for (AttrPeerOption *opt = $4->head(); opt; opt = $4->next(opt)) {
         const AttrProtocolOption *decl = protocol->searchOption(opt->option);
         if (!decl)  {
            handle_error("Error: protocol %s does not have option %s.\n", 
                         $2, opt->option);
            error = true;
         } else {
            for (; decl; decl = protocol->searchNextOption(decl))
               if (decl->option->validateArgs(opt->args, position, correctType))
                  break;
            if (! decl) {
               if (! (!strcasecmp(protocol->name, "BGP4")
                      && !strcasecmp(opt->option, "asno")
                      && opt->args->isSingleton()
                      && typeid(*opt->args->head()) == typeid(ItemWORD)
                      && !strcasecmp(((ItemWORD *) opt->args->head())->word,
                                     "peeras"))) {
                  handleArgumentTypeError($2, opt->option, position, 
                                          correctType);
                  error = true;
               }
            }
         }
      }
   }

   if (! error) {
      const AttrProtocolOption *missing = 
         ((AttrProtocol *) protocol)->missingMandatoryOption();
      if (missing) {
         handle_error("Error: mandatory option %s of protocol %s is missing.\n", 
                      missing->option->name, $2);
         error = true;
      }
   }
      
   if (!error) {
     $$ = changeCurrentAttr(new AttrMPPeer(protocol, $3, $4));
   }
   else {
      free($2);
      delete $3;
      delete $4;
   }
  
}
| ATTR_MP_PEER TKN_WORD mp_peer_id error TKN_EOA {
   $$ = $1;
   free($2);
   delete $3;
   handle_error("Error: in peer option.\n");
   yyerrok;
}
| ATTR_MP_PEER TKN_WORD error TKN_EOA {
   $$ = $1;
   free($2);
   handle_error("Error: missing peer ip_address.\n");
   yyerrok;
}
| ATTR_MP_PEER error TKN_EOA {
   $$ = $1;
   handle_error("Error: missing protocol name.\n");
   yyerrok;
}
;

mp_peer_id: TKN_IPV4 {
  $$ = new MPPrefix($1);
}
| TKN_IPV6 {
  $$ = new MPPrefix($1);
}
| TKN_DNS  {
  $$ = new MPPrefix;
}
| TKN_RTRSNAME  {
//   $$ = new IPAddr;
  $$ = new MPPrefix;
}
| TKN_PRNGNAME {
//   $$ = new IPAddr;
  $$ = new MPPrefix;
}
;


//**** route6 class *******************************************************


//// v6_filter v6 only filter in route6 object //////////////////////////////////////////////////////////

v6_filter_attribute: ATTR_V6_FILTER v6_filter TKN_EOA {
  $$ = changeCurrentAttr(new AttrFilter($2));
}
| ATTR_V6_FILTER error TKN_EOA {
   $$ = $1;
   handle_error("Error: badly formed filter.\n");
   yyerrok;
}  
;

v6_filter: v6_filter OP_OR v6_filter_term {
   $$ = new FilterOR($1, $3);
}
| v6_filter v6_filter_term %prec OP_OR {
   $$ = new FilterOR($1, $2);
}
| v6_filter_term
;

v6_filter_term : v6_filter_term OP_AND v6_filter_factor {
   $$ = new FilterAND($1, $3);
}
| v6_filter_factor
;

v6_filter_factor :  OP_NOT v6_filter_factor {
   $$ = new FilterNOT($2);
}
| '(' v6_filter ')' {
   $$ = $2;
}
| v6_filter_operand 
;

v6_filter_operand: KEYW_ANY {
   $$ = new FilterANY;
}
| '<' filter_aspath '>' {
   $$ = new FilterASPath($2);
}
| filter_rp_attribute {
   if ($1)
      $$ = $1;
   else
      $$ = new FilterNOT(new FilterANY);
}
| TKN_FLTRNAME {
   $$ = new FilterFLTRNAME($1);
}
| v6_filter_prefix
;

v6_filter_prefix: v6_filter_prefix_operand OP_MS {
   $2->f1 = $1;
   $$ = $2;
}
|  v6_filter_prefix_operand
;

v6_filter_prefix_operand: TKN_ASNO {
   $$ = new FilterASNO($1);
}
| KEYW_PEERAS {
   $$ = new FilterPeerAS;
}
| TKN_ASNAME {
   $$ = new FilterASNAME($1);
}
| TKN_RSNAME {
   $$ = new FilterRSNAME($1);
}
| '{' opt_v6_filter_prefix_list '}' { 
   $$ = $2; 
}
;

opt_v6_filter_prefix_list: {
   $$ = new FilterMPPRFXList;
}
| v6_filter_prefix_list
;

v6_filter_prefix_list: v6_filter_prefix_list_prefix {

   ((FilterMPPRFXList *) ($$ = new FilterMPPRFXList))->push_back(*$1);
}
| v6_filter_prefix_list ',' v6_filter_prefix_list_prefix {
   $$ = $1;
   ((FilterMPPRFXList *) ($$))->push_back(*$3);
}
;


v6_filter_prefix_list_prefix: TKN_PRFXV6 {
  $$ = new MPPrefix($1);
}
| TKN_PRFXV6RNG {
  $$ = new MPPrefix($1);
}  
;  

//// v6-components attribute ///////////////////////////////////////////////

v6_components_attribute: ATTR_V6_COMPONENTS opt_atomic v6_components_list TKN_EOA {
  $$ = $1;
}
| ATTR_V6_COMPONENTS error TKN_EOA {
   $$ = $1;
   handle_error("Error: [ATOMIC] [[<filter>] [PROTOCOL <protocol> <filter>] ...] expected.\n");
   yyerrok;
}
;

v6_components_list: {
}
| v6_filter {
   delete $1;
}
| v6_components_list KEYW_PROTOCOL TKN_WORD v6_filter {
   free($3);
   delete $4;
}
;




//// v6-inject attribute ///////////////////////////////////////////////////

v6_inject_attribute: ATTR_V6_INJECT opt_mp_router_expr_with_at opt_action opt_v6_inject_expr TKN_EOA {
  $$ = $1;
  delete $2;
  delete $3;
  delete $4;
  
}
| ATTR_V6_INJECT error TKN_EOA {
   $$ = $1;
   handle_error("Error: [at <router-exp>] [action <action>] [upon <condition>] expected.\n");
   yyerrok;
}  
;

opt_v6_inject_expr: {
   $$ = new FilterANY;
}
| KEYW_UPON v6_inject_expr {
   $$ = $2;
}
;

v6_inject_expr: v6_inject_expr OP_OR v6_inject_expr_term {
   $$ = new FilterOR($1, $3);
}
| v6_inject_expr_term {
}
;

v6_inject_expr_term: v6_inject_expr_term OP_AND v6_inject_expr_factor {
   $$ = new FilterAND($1, $3);
}
| v6_inject_expr_factor
;

v6_inject_expr_factor: '(' v6_inject_expr ')' {
   $$ = $2;
}
| v6_inject_expr_operand 
;

v6_inject_expr_operand: KEYW_STATIC {
   $$ = new FilterANY;
}
| KEYW_HAVE_COMPONENTS '{' opt_v6_filter_prefix_list '}' { 
   $$ = new FilterV6HAVE_COMPONENTS((FilterMPPRFXList *) $3); 
} 
| KEYW_EXCLUDE '{' opt_v6_filter_prefix_list '}' { 
   $$ = new FilterV6EXCLUDE((FilterMPPRFXList *) $3); 
} 
;

//**** route class ********************************************************

aggr_bndry_attribute: ATTR_AGGR_BNDRY as_expr TKN_EOA {
   $$ = $1;
   delete $2;
}
| ATTR_AGGR_BNDRY error TKN_EOA {
   $$ = $1;
   handle_error("Error: <as-expression> expected.\n");
   yyerrok;
}
;

aggr_mtd_attribute: ATTR_AGGR_MTD KEYW_INBOUND TKN_EOA {
   $$ = $1;
}
| ATTR_AGGR_MTD KEYW_OUTBOUND opt_as_expr TKN_EOA {
   delete $3;
}
| ATTR_AGGR_MTD KEYW_OUTBOUND error TKN_EOA {
   $$ = $1;
   handle_error("Error: OUTBOUND <as-expression> expected.\n");
   yyerrok;
}
| ATTR_AGGR_MTD KEYW_INBOUND error TKN_EOA {
   $$ = $1;
   handle_error("Error: INBOUND can not be followed by anything.\n");
   yyerrok;
}
| ATTR_AGGR_MTD error TKN_EOA {
   $$ = $1;
   handle_error("Error: keyword INBOUND or OUTBOUND expected.\n");
   yyerrok;
}
;

//// inject attribute ///////////////////////////////////////////////////

opt_inject_expr: {
   $$ = new FilterANY;
}
| KEYW_UPON inject_expr {
   $$ = $2;
}
;

inject_expr: inject_expr OP_OR inject_expr_term {
   $$ = new FilterOR($1, $3);
}
| inject_expr_term
;

inject_expr_term: inject_expr_term OP_AND inject_expr_factor {
   $$ = new FilterAND($1, $3);
}
| inject_expr_factor
;

inject_expr_factor: '(' inject_expr ')' {
   $$ = $2;
}
| inject_expr_operand 
;

inject_expr_operand: KEYW_STATIC {
   $$ = new FilterANY;
}
| KEYW_HAVE_COMPONENTS '{' opt_filter_prefix_list '}' { 
   $$ = new FilterHAVE_COMPONENTS((FilterPRFXList *) $3); 
} 
| KEYW_EXCLUDE '{' opt_filter_prefix_list '}' { 
   $$ = new FilterEXCLUDE((FilterPRFXList *) $3); 
} 
;

inject_attribute: ATTR_INJECT opt_router_expr_with_at opt_action opt_inject_expr TKN_EOA {
   $$ = $1;
   delete $2;
   delete $3;
   delete $4;
}
| ATTR_INJECT error TKN_EOA {
   $$ = $1;
   handle_error("Error: [at <router-exp>] [action <action>] [upon <condition>] expected.\n");
   yyerrok;
}
;

//// components attribute ///////////////////////////////////////////////

opt_atomic:
| KEYW_ATOMIC
;

components_list: {
}
| filter {
   delete $1;
}
| components_list KEYW_PROTOCOL tkn_word filter {
   free($3);
   delete $4;
}
;

components_attribute: ATTR_COMPONENTS opt_atomic components_list TKN_EOA {
   $$ = $1;
}
| ATTR_COMPONENTS error TKN_EOA {
   $$ = $1;
   handle_error("Error: [ATOMIC] [[<filter>] [PROTOCOL <protocol> <filter>] ...] expected.\n");
   yyerrok;
}
;

//**** route-set **********************************************************

opt_rs_members_list: /* empty list */ {
   $$ = new ItemList;
}
| rs_members_list {
   $$ = $1;
}  
;

rs_members_list: rs_member { 	
   $$ = new ItemList;
   $$->append($1);
}
| rs_members_list ',' rs_member {
   $$ = $1;
   $$->append($3);
}
;

rs_member: TKN_ASNO {
   $$ = new ItemASNO($1);
}
| TKN_ASNO OP_MS {
   $$ = new ItemMSItem(new ItemASNO($1), $2->code, $2->n, $2->m);
   delete $2;
}
| TKN_ASNAME {
   $$ = new ItemASNAME($1);
}
| TKN_ASNAME OP_MS {
   $$ = new ItemMSItem(new ItemASNAME($1), $2->code, $2->n, $2->m);
   delete $2;
}
| TKN_RSNAME {
   $$ = new ItemRSNAME($1);
}
| TKN_RSNAME OP_MS {
   $$ = new ItemMSItem(new ItemRSNAME($1), $2->code, $2->n, $2->m);
   delete $2;
}
| TKN_PRFXV4 {
   $$ = new ItemPRFXV4($1);
}
| TKN_PRFXV4RNG {
   $$ = new ItemPRFXV4Range($1);
}
;

rs_members_attribute: ATTR_RS_MEMBERS opt_rs_members_list TKN_EOA {
   $$ = changeCurrentAttr(new AttrGeneric($1->type, $2));
}
| ATTR_RS_MEMBERS error TKN_EOA {
   $$ = $1;
   handle_error("Error: invalid member\n");
   yyerrok;
}
;

//*  mp-members attribute of route-set object TBD */

opt_rs_mp_members_list: /* empty list */ {
   $$ = new ItemList;
}
| rs_mp_members_list {
   $$ = $1;
}
;

rs_mp_members_list: rs_mp_member {
   $$ = new ItemList;
   $$->append($1);
}
| rs_mp_members_list ',' rs_mp_member {
   $$ = $1;
   $$->append($3);
}
;

rs_mp_member: TKN_ASNO {
   $$ = new ItemASNO($1);
}
| TKN_ASNO OP_MS {
   $$ = new ItemMSItem(new ItemASNO($1), $2->code, $2->n, $2->m);
   delete $2;
}
| TKN_ASNAME {
   $$ = new ItemASNAME($1);
}
| TKN_ASNAME OP_MS {
   $$ = new ItemMSItem(new ItemASNAME($1), $2->code, $2->n, $2->m);
   delete $2;
}
| TKN_RSNAME {
   $$ = new ItemRSNAME($1);
}
| TKN_RSNAME OP_MS {
   $$ = new ItemMSItem(new ItemRSNAME($1), $2->code, $2->n, $2->m);
   delete $2;
}
| TKN_PRFXV4 {
   $$ = new ItemPRFXV4($1);
}
| TKN_PRFXV4RNG {
   $$ = new ItemPRFXV4Range($1);
}
| TKN_PRFXV6 {
   $$ = new ItemPRFXV6($1);
}
| TKN_PRFXV6RNG {
   $$ = new ItemPRFXV6Range($1);
}
;

rs_mp_members_attribute: ATTR_RS_MP_MEMBERS opt_rs_mp_members_list TKN_EOA {
   $$ = changeCurrentAttr(new AttrGeneric($1->type, $2));
}
| ATTR_RS_MP_MEMBERS error TKN_EOA {
   $$ = $1;
   handle_error("Error: invalid member\n");
   yyerrok;
}
;

//**** dictionary *********************************************************

rpattr_attribute: ATTR_RP_ATTR TKN_WORD methods TKN_EOA {
   $$ = changeCurrentAttr(new AttrRPAttr($2, $3));
}
| ATTR_RP_ATTR TKN_RP_ATTR methods TKN_EOA {
   $$ = changeCurrentAttr(new AttrRPAttr($2->name, $3));
}
| ATTR_RP_ATTR error TKN_EOA {
   $$ = $1;
   handle_error("Error: invalid rp-attribute specification\n");
   yyerrok;
}
;

methods: method {
   $$ = new List<AttrMethod>;
   if ($1)
      $$->append($1);
}
| methods method {
   $$ = $1;
   if ($2)
      $$->append($2);
}
;

method: TKN_WORD '(' ')' {
   $$ = new AttrMethod($1, new List<RPTypeNode>, false);
}
| TKN_WORD '(' typedef_type_list ')' {
   $$ = new AttrMethod($1, $3, false);
}
| TKN_WORD '(' typedef_type_list ',' TKN_3DOTS ')' {
   $$ = new AttrMethod($1, $3, true);
}
| KEYW_OPERATOR TKN_OPERATOR '(' typedef_type_list ')' {
   char buffer[16];
   strcpy(buffer, "operator");
   strcat(buffer, $2);
   $$ = new AttrMethod(strdup(buffer), $4, false, true);
   free($2);
}
| KEYW_OPERATOR TKN_OPERATOR '(' typedef_type_list ',' TKN_3DOTS ')' {
   char buffer[16];
   strcpy(buffer, "operator");
   strcat(buffer, $2);
   $$ = new AttrMethod(strdup(buffer), $4, true, true);
   free($2);
}
| TKN_WORD error ')' {
   free($1);
   $$ = NULL;
   handle_error("Error: invalid method specification for %s\n", $1);
}
| KEYW_OPERATOR TKN_OPERATOR error ')' {
   $$ = NULL;
   handle_error("Error: invalid operator specification for %s\n", $2);
   free($2);
}
| KEYW_OPERATOR error ')' {
   $$ = NULL;
   handle_error("Error: invalid operator\n");
}
| error ')' {
   $$ = NULL;
   handle_error("Error: method specification expected\n");
}
;

//// typedef attribute  /////////////////////////////////////////////////

typedef_attribute: ATTR_TYPEDEF TKN_WORD typedef_type TKN_EOA {
   $$ = changeCurrentAttr(new AttrTypedef($2, $3));
}
| ATTR_TYPEDEF error TKN_EOA {
   $$ = $1;
   handle_error("Error: invalid typedef specification\n");
   yyerrok;
}
;

typedef_type_list: typedef_type {
   $$ = new List<RPTypeNode>;
   if ($1)
      $$->append(new RPTypeNode($1));
}
| typedef_type_list ',' typedef_type {
   $$ = $1;
   if ($3)
      $$->append(new RPTypeNode($3));
}
;

typedef_type: KEYW_UNION typedef_type_list {
   $$ = RPType::newRPType("union", $2);
   if (!$$) {
      handle_error("Error: empty union specification\n");
      delete $2;
   }
}
| KEYW_RANGE KEYW_OF typedef_type {
   if ($3)
      $$ = new RPTypeRange($3);
   else {
      $$ = NULL;
   }
}
| TKN_WORD {
   $$ = RPType::newRPType($1);
   if (!$$) {
      handle_error("Error: invalid type %s\n", $1);
   }
   free($1);
}
| TKN_WORD '[' TKN_INT ',' TKN_INT ']' {
   $$ = RPType::newRPType($1, $3, $5);
   if (!$$) {
      handle_error("Error: invalid type %s[%d,%d]\n", $1, $3, $5);
   }
   free($1);
}
| TKN_WORD '[' TKN_REAL ',' TKN_REAL ']' {
   $$ = RPType::newRPType($1, $3, $5);
   if (!$$) {
      handle_error("Error: invalid type %s[%f,%f]\n", $1, $3, $5);
   }
   free($1);
}
| TKN_WORD '[' enum_list ']' {
   $$ = RPType::newRPType($1, $3);
   if (!$$) {
      handle_error("Error: invalid type %s, enum expected\n", $1);
      delete $3;
   }
   free($1);
}
| KEYW_LIST '[' TKN_INT ':' TKN_INT ']' KEYW_OF typedef_type {
   if ($8)
      if ($3 < $5)
	 $$ = new RPTypeList($8, $3, $5);
      else
	 $$ = new RPTypeList($8, $5, $3);
   else {
      $$ = NULL;
      delete $8;
   }
}
| KEYW_LIST KEYW_OF typedef_type {
   if ($3)
      $$ = new RPTypeList($3);
   else {
      $$ = NULL;
   }
}
| KEYW_LIST error KEYW_OF typedef_type {
   $$ = NULL;
   delete $4;
   handle_error("Error: invalid list size\n");
}
;

enum_list: tkn_word {
   $$ = new List<WordNode>;
   $$->append(new WordNode($1));
}
| enum_list ',' tkn_word {
   $$ = $1;
   $$->append(new WordNode($3));
}
;

//// protocol attribute /////////////////////////////////////////////////

protocol_attribute: ATTR_PROTOCOL tkn_word protocol_options TKN_EOA {
   $$ = changeCurrentAttr(new AttrProtocol($2, $3));
}
| ATTR_PROTOCOL tkn_word error TKN_EOA {
   $$ = $1;
   handle_error("Error: invalid protocol option\n");
   yyerrok;
}
| ATTR_PROTOCOL error TKN_EOA {
   $$ = $1;
   handle_error("Error: invalid protocol name\n");
   yyerrok;
}
;

protocol_options: {
   $$ = new List<AttrProtocolOption>;
}
| protocol_options protocol_option {
   $$ = $1;
   $$->append($2);
}
;

protocol_option: KEYW_MANDATORY method {
   $$ = new AttrProtocolOption(false, $2);
}
| KEYW_OPTIONAL method  {
   $$ = new AttrProtocolOption(true, $2);
}
;

//**** schema class *******************************************************

opt_attr_options: {
   $$ = new AttrAttr(ATTR_GENERIC, RPType::newRPType("free_text"));
}
| attr_options {
   $$ = new AttrAttr(ATTR_GENERIC, RPType::newRPType("free_text"));
   *$$ |= *$1;
   delete $1;
}
;

attr_options: attr_option {
   $$ = $1;
}
| attr_options ',' attr_option {
   $$ = $1;
   *$$ |= *$3;
   delete $3;
}
| error ',' attr_option {
   $$ = $3;
   handle_error("Error: in attr option specification.\n");
}
;

attr_option: KEYW_SYNTAX '(' typedef_type ')' {
   $$ = new AttrAttr(ATTR_GENERIC, $3);
}
| KEYW_SYNTAX '(' KEYW_SPECIAL ',' tkn_word ')' {
   int syntax = schema.searchAttrSyntax($5);
   if (syntax < 0) {
      handle_error("Error: no known syntax rule for %s.\n", $5);
      $$ = new AttrAttr;
   } else
      $$ = new AttrAttr(syntax, NULL);
   free($5);
}
| KEYW_SYNTAX '(' KEYW_REGEXP ',' TKN_STRING ')' {
   regex_t *re = (regex_t *) malloc(sizeof(regex_t));
   int code = regcomp(re, $5, REG_EXTENDED | REG_ICASE);

   if (code) {
      handle_error("Error: could not compile regexp %s.\n", $5);
      free(re);
      free($5);
      $$ = new AttrAttr;
   } else
      $$ = new AttrAttr(ATTR_REGEXP, $5, re);
}
| KEYW_SYNTAX '(' KEYW_REGEXP ',' TKN_STRING ',' TKN_INT ')' {
   regex_t *re = (regex_t *) malloc(sizeof(regex_t));
   int code = regcomp(re, $5, REG_EXTENDED | REG_ICASE);

   if (code) {
      handle_error("Error: could not compile regexp %s.\n", $5);
      free(re);
      free($5);
      $$ = new AttrAttr;
   } else
      $$ = new AttrAttr(ATTR_REGEXP, $5, re, $7);
}
| KEYW_OPTIONAL {
   $$ = new AttrAttr(AttrAttr::OPTIONAL);
}
| KEYW_MANDATORY {
   $$ = new AttrAttr;
}
| KEYW_DELETED {
   $$ = new AttrAttr(AttrAttr::DELETED);
}
| KEYW_SINGLEVALUED {
   $$ = new AttrAttr;
}
| KEYW_MULTIVALUED {
   $$ = new AttrAttr(AttrAttr::MULTIVALUED);
}
| KEYW_LOOKUP {
   $$ = new AttrAttr(AttrAttr::LOOKUP);
}
| KEYW_KEY {
   $$ = new AttrAttr(AttrAttr::KEY);
}
| KEYW_OBSOLETE {
   $$ = new AttrAttr(AttrAttr::OBSOLETE);
}
| KEYW_INTERNAL {
   $$ = new AttrAttr(AttrAttr::INTERNAL);
}
;

attr_attribute: ATTR_ATTR tkn_word opt_attr_options TKN_EOA {
   $3->setName($2);
   $$ = changeCurrentAttr($3);
}
| ATTR_ATTR tkn_word error TKN_EOA {
   $$ = $1;
   free($2);
   handle_error("Error: in attr option specification.\n");
   yyerrok;
}
| ATTR_ATTR error TKN_EOA {
   $$ = $1;
   handle_error("Error: attr name expected.\n");
   yyerrok;
}
;

//**** rps-auth stuff *****************************************************
// mnt-routes in route, inetnum

mnt_routes_attribute: ATTR_MNT_ROUTES mnt_routes_list TKN_EOA {
   $$ = changeCurrentAttr(new AttrMntRoutes($2));
}
;

mnt_routes_list: mnt_routes_list_item {
   $$ = new List<AttrMntRoutes::MntPrfxPair>;
   $$->append($1);
}
| mnt_routes_list ',' mnt_routes_list_item {
   $$ = $1;
   $$->append($3);
}
;

mnt_routes_list_item: tkn_word {
   $$ = new AttrMntRoutes::MntPrfxPair($1, NULL);
}
| tkn_word KEYW_ANY  {
   $$ = new AttrMntRoutes::MntPrfxPair($1, NULL);
}
| tkn_word '{' opt_filter_prefix_list '}'  {
   $$ = new AttrMntRoutes::MntPrfxPair($1, (FilterMPPRFXList *) $3);
}
;

//** mnt-routes in route6, inet6num

mnt_routes6_attribute: ATTR_MNT_ROUTES6 mnt_routes6_list TKN_EOA {
   $$ = changeCurrentAttr(new AttrMntRoutes($2));
}
;
mnt_routes6_list: mnt_routes6_list_item {
   $$ = new List<AttrMntRoutes::MntPrfxPair>;
   $$->append($1);
}
| mnt_routes6_list ',' mnt_routes6_list_item {
   $$ = $1;
   $$->append($3);
}
;

mnt_routes6_list_item: tkn_word {
   $$ = new AttrMntRoutes::MntPrfxPair($1, NULL);
}
| tkn_word KEYW_ANY  {
   $$ = new AttrMntRoutes::MntPrfxPair($1, NULL);
}
| tkn_word '{' opt_v6_filter_prefix_list '}'  {
   $$ = new AttrMntRoutes::MntPrfxPair($1, (FilterMPPRFXList *) $3);
}
;

// mnt-routes attribute in aut-num (multiprotocol)

mnt_routes_mp_attribute: ATTR_MNT_ROUTES_MP mnt_routes_mp_list TKN_EOA {
   $$ = changeCurrentAttr(new AttrMntRoutes($2));
} 
; 
mnt_routes_mp_list: mnt_routes_mp_list_item {
   $$ = new List<AttrMntRoutes::MntPrfxPair>;
   $$->append($1);
} 
| mnt_routes_mp_list ',' mnt_routes_mp_list_item {
   $$ = $1;
   $$->append($3);
}
; 
   
mnt_routes_mp_list_item: tkn_word {
   $$ = new AttrMntRoutes::MntPrfxPair($1, NULL);
}
| tkn_word KEYW_ANY  {
   $$ = new AttrMntRoutes::MntPrfxPair($1, NULL);
} 
| tkn_word '{' opt_mp_filter_prefix_list '}'  {
   $$ = new AttrMntRoutes::MntPrfxPair($1, (FilterMPPRFXList *) $3);
}  
;

// **** afi stuff ************************************************************

afi_list: afi {
  $$ = new ItemList;
  $$->append($1);
}
| afi_list ',' afi {
  $$ = $1;
  $$->append($3);
}
;

afi: TKN_AFI {
    $$ = new ItemAFI($1);
}
;

%%

void enable_yy_parser_debugging() {
#if YYDEBUG != 0
   yydebug = 1;
#endif
}

void handleArgumentTypeError(char *attr, char *method, int position,
			     const RPType *correctType, 
			     bool isOperator = false) {
   if (isOperator)
      if (position)
	 handle_error("Error: argument %d to %s.operator%s should be %s.\n",
		   position, attr, method, ((RPType *) correctType)->name());
      else
	 handle_error("Error: wrong number of arguments to %s.operator%s.\n",
		      attr, method);
   else
      if (position)
	 handle_error("Error: argument %d to %s.%s should be %s.\n",
		   position, attr, method, ((RPType *) correctType)->name());
      else
	 handle_error("Error: wrong number of arguments to %s.%s.\n",
		      attr, method);
}

const AttrMethod *searchMethod(const AttrRPAttr *rp_attr, char *method, ItemList *args) {
   const AttrMethod *mtd = rp_attr->searchMethod(method);
   int position;
   const RPType *correctType;
   
   if (!mtd) {
      handle_error("Error: rp-attribute %s does not have %s defined.\n",
		   rp_attr->name, method);
      return NULL;
   }
   
   for (; mtd; mtd = rp_attr->searchNextMethod(mtd))
      if (mtd->validateArgs(args, position, correctType))
	 return mtd;

   handleArgumentTypeError(rp_attr->name, method, position, correctType);
   
   return NULL;
}
