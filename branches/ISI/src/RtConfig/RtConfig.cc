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

#include "config.h"
#include <cstdlib>

extern "C" {
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif
}
#include <iostream.h>
#include <iomanip.h>
#include "irr/irr.hh"
#include "irr/rawhoisc.hh"
#include "irr/ripewhoisc.hh"
#include "util/debug.hh"
#include "util/trace.hh"
#include "util/rusage.hh"
#include "RtConfig.hh"
#include "util/Argv.hh"
#include "util/version.hh"
#include "re2dfa/regexp_nf.hh"
#include "f_cisco.hh"
#include "f_junos.hh"
#include "f_bcc.hh"
#include "f_gated.hh"
#include "f_rsd.hh"
#include "rpsl/schema.hh"
#include "irr/classes.hh"

bool opt_rusage                  = false;
Rusage ru(clog, &opt_rusage);

char *opt_prompt                 = "RtConfig> ";

bool         RtConfig::supressMartians          = false;
int          RtConfig::preferenceCeiling        = 1000;

const AttrRPAttr *RtConfig::dctn_rp_pref = (AttrRPAttr *) NULL;
const AttrMethod     *RtConfig::dctn_rp_pref_set = (AttrMethod *) NULL;
const AttrRPAttr *RtConfig::dctn_rp_nhop = (AttrRPAttr *) NULL;
const AttrMethod     *RtConfig::dctn_rp_nhop_set = (AttrMethod *) NULL;
const AttrRPAttr *RtConfig::dctn_rp_dpa = (AttrRPAttr *) NULL;
const AttrMethod     *RtConfig::dctn_rp_dpa_set = (AttrMethod *) NULL;
const AttrRPAttr *RtConfig::dctn_rp_med = (AttrRPAttr *) NULL;
const AttrMethod     *RtConfig::dctn_rp_med_set = (AttrMethod *) NULL;
const AttrRPAttr *RtConfig::dctn_rp_community = (AttrRPAttr *) NULL;
const AttrMethod     *RtConfig::dctn_rp_community_setop= (AttrMethod *) NULL;
const AttrMethod     *RtConfig::dctn_rp_community_appendop=(AttrMethod*)NULL;
const AttrMethod     *RtConfig::dctn_rp_community_append= (AttrMethod*) NULL;
const AttrMethod     *RtConfig::dctn_rp_community_delete= (AttrMethod*) NULL;
const AttrRPAttr *RtConfig::dctn_rp_aspath = (AttrRPAttr *) NULL;
const AttrMethod     *RtConfig::dctn_rp_aspath_prepend = (AttrMethod *) NULL;

typedef struct {
   char     *name;
   RtConfig *config;
} config_format_type;

CiscoConfig ciscoConfig;
JunosConfig junosConfig;
GatedConfig gatedConfig;
RSdConfig   rsdConfig;
BccConfig bccConfig;

config_format_type config_formats[] = {
   { "rsd",         &rsdConfig },
   { "gated",       &gatedConfig },
   { "cisco",       &ciscoConfig },
   { "junos",       &junosConfig },
   { "bcc",       &bccConfig },
   //   { "rsd",         rsd_process_line },
   { "", 0 }
};

RtConfig *rtConfig = &ciscoConfig;

int start_tracing(char *dst, char *key, char *nextArg) {
   if (nextArg) {
      trace.enable(nextArg);
      return 1; // return 1 to signify nextArg is used by us
   }
   return 0; 
}

int start_debugging(char *dst, char *key, char *nextArg) {
   if (nextArg) {
      Debug(dbg.enable(atoi(nextArg)));
      return 1; // return 1 to signify nextArg is used by us
   }
   return 0;
}

int select_config_format(char *dst, char *key, char *nextArg) {
   if (!nextArg)
      return 0;

   int opt_config;
   for (opt_config = 0; *config_formats[opt_config].name; opt_config++)
      if (strcmp(nextArg, config_formats[opt_config].name) == 0)
	 break;
   if (! *config_formats[opt_config].name) {
      cerr << "Error: config file format '" << nextArg << "' is not supported..." << endl;
      exit(1);
   }
   rtConfig = config_formats[opt_config].config;

   return 1; // return 1 to signify nextArg is used by us
}

void init_and_set_options (int argc, char **argv, char **envp) {
   bool omit = true;

   ArgvInfo argTable[] = {
     // RAToolSet common arguments
     // key, type, src, dst, help
     {"-T", ARGV_FUNC, (char *) &start_tracing,         (char *) NULL, 
      "Start tracing the next argument"},
     {"-D", ARGV_FUNC, (char *) &start_debugging,       (char *) NULL, 
      "Start debugging the next argument"},
     {"-version", ARGV_FUNC, (char *) &version,         (char *) NULL,
      "Show version"},
     {"-rusage", ARGV_BOOL, (char *) NULL,              (char *) &opt_rusage,
      "On termination print resource usage"},
     {"-prompt", ARGV_STRING,  (char *) NULL,           (char *) &opt_prompt,
      "Prompt"},
     
     IRR_COMMAND_LINE_OPTIONS,

     {"-config", ARGV_FUNC, (char *) &select_config_format, (char *) NULL, 
      "Configuration format (junos, cisco, bcc, gated or rsd)"},
     {"-no_match_ip_inbound", ARGV_BOOL, 
      (char *) NULL, (char *) &CiscoConfig::forcedInboundMatchIP,
      "Produce a match ip clause for inbound maps.\n\t\t\t\tCisco only."},
     {"-disable_access_list_cache", ARGV_BOOL, 
      (char *) NULL, (char *) &CiscoConfig::useAclCaches,
      "Do not reuse the old access list numbers.\n\t\t\t\tCisco only."},
     {"-cisco_skip_route_maps", ARGV_BOOL, 
      (char *) NULL, (char *) &CiscoConfig::printRouteMap,
      "Do not print route maps, only print access lists.\n\t\t\t\tCisco only."},
     {"-supress_martian", ARGV_BOOL, 
      (char *) NULL, (char *) &RtConfig::supressMartians,
      "Generate access lists supressing invalid routes.\n\t\t\t\tCisco only."},
     {"-cisco_compress_acls", ARGV_BOOL, 
      (char *) NULL, (char *) &omit,
      "Combine multiple cisco access list lines into a single line using wildcards whenever possible.\n\t\t\t\tCisco only."},
     {"-cisco_no_compress_acls", ARGV_BOOL, 
      (char *) NULL, (char *) &CiscoConfig::compressAcls,
      "Do not combine multiple cisco access list lines into a single line using wildcards whenever possible.\n\t\t\t\tCisco only."},
     {"-cisco_use_prefix_lists", ARGV_BOOL, 
      (char *) NULL, (char *) &CiscoConfig::usePrefixLists,
      "Output new high performance prefix lists.\n\t\t\t\tCisco only."},
     {"-cisco_eliminate_dup_map_parts", ARGV_BOOL, 
      (char *) NULL, (char *) &CiscoConfig::eliminateDupMapParts,
      "Eliminate if a later map part is contained in an earlier map part.\n\t\t\t\tCisco only."},
     {"-cisco_force_tilda", ARGV_BOOL, 
      (char *) NULL, (char *) &CiscoConfig::forceTilda,
      "Interpret * and + as ~* and ~+ operators in AS paths to shorten as path acl's lines.\n\t\t\t\tCisco only."},
     
     {"-junos_no_compress_acls", ARGV_BOOL, 
      (char *) NULL, (char *) &JunosConfig::compressAcls,
      "Do not combine multiple route-filter lines into a single line whenever possible.\n\t\t\t\tJunos only."},

     {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL,
      (char *) NULL}
   };
   
   IRR::handleEnvironmentVariables(envp);

   if (ParseArgv(&argc, argv, argTable, ARGV_NO_LEFTOVERS) != ARGV_OK) {
      cerr << endl;
      exit(1);
   }

   if (! (irr = IRR::newClient())) {
      cerr << "Unknown protocol!" << endl;
      exit(1);
   }

   // have a prompt only if the input is coming from a tty
   if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
      opt_prompt = (char *) NULL;
}

main(int argc, char **argv, char **envp) {
   extern int commandparse();

   schema.initialize();
   init_and_set_options(argc, argv, envp);
   regexp_nf::expandASSets();

   RtConfig::loadDictionary();
     
   commandparse();
   
   if (opt_prompt)
      cout << endl;

   cout.flush();
   cerr.flush();
}

void RtConfig::loadDictionary() {
   dctn_rp_pref      = schema.searchRPAttr("pref");
   dctn_rp_nhop      = schema.searchRPAttr("next-hop");
   dctn_rp_dpa       = schema.searchRPAttr("dpa");
   dctn_rp_med       = schema.searchRPAttr("med");
   dctn_rp_community = schema.searchRPAttr("community");
   dctn_rp_aspath    = schema.searchRPAttr("aspath");
   
   if (dctn_rp_pref)
      dctn_rp_pref_set         = dctn_rp_pref->searchMethod("=");
   if (dctn_rp_nhop)
      dctn_rp_nhop_set         = dctn_rp_nhop->searchMethod("=");
   if (dctn_rp_dpa)
      dctn_rp_dpa_set          = dctn_rp_dpa->searchMethod("=");
   if (dctn_rp_med)
      dctn_rp_med_set          = dctn_rp_med->searchMethod("=");
   if (dctn_rp_community) {
      dctn_rp_community_setop    = dctn_rp_community->searchMethod("=");
      dctn_rp_community_appendop = dctn_rp_community->searchMethod(".=");
      dctn_rp_community_append   = dctn_rp_community->searchMethod("append");
      dctn_rp_community_delete   = dctn_rp_community->searchMethod("delete");
   }
   if (dctn_rp_aspath)
      dctn_rp_aspath_prepend   = dctn_rp_aspath->searchMethod("prepend");
}

// @RtConfig configureRouter rtr.isp.net
void RtConfig::configureRouter(char *name) {
   const InetRtr *rtr = irr->getInetRtr(name);
   const AttrProtocol *bgp = schema.searchProtocol("BGP4");

   AttrIterator<AttrIfAddr> itr1(rtr, "ifaddr");
   IPAddr myIP(itr1()->ifaddr.get_ipaddr());

   AttrGenericIterator<ItemASNO> itr2(rtr, "local-as");
   ASt myAS = itr2()->asno;

   AttrIterator<AttrPeer> itr(rtr, "peer");

   for (const AttrPeer *peer = itr.first(); peer; peer = itr.next()) {
      if (peer->protocol == bgp) {
	 ASt peerAS = ((ItemASNO *) peer->searchOption("asno")->args->head())->asno;
	 importP(myAS, &myIP, peerAS, peer->peer);
	 exportP(myAS, &myIP, peerAS, peer->peer);
      }
   }
}

void RtConfig::printPrefixes(char *filter, char *fmt) {
   Buffer peval(strlen(filter)+16);
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {
      AttrIterator<AttrFilter> itr(o, "peval");
      
      NormalExpression *ne = 
	 NormalExpression::evaluate(itr()->filter, ~0);
      if (ne && ne->singleton_flag == NormalTerm::PRFX)
	 printPrefixes_(ne->first()->prfx_set, fmt);
      else
	 cerr << "Error: Filter can only contain prefix filters." << endl;
      
      delete ne;
   }
   
   delete o;
}

void RtConfig::printPrefixRanges(char *filter, char *fmt) {
   Buffer peval(strlen(filter)+16);
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {
      AttrIterator<AttrFilter> itr(o, "peval");
      
      NormalExpression *ne = 
	 NormalExpression::evaluate(itr()->filter, ~0);
      if (ne && ne->singleton_flag == NormalTerm::PRFX)
	 printPrefixRanges_(ne->first()->prfx_set, fmt);
      else
	 cerr << "Error: Filter can only contain prefix filters." << endl;
      
      delete ne;
   }
   
   delete o;
}

void RtConfig::printSuperPrefixRanges(char *filter, char *fmt) {
   Buffer peval(strlen(filter)+16);
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {
      AttrIterator<AttrFilter> itr(o, "peval");
      
      NormalExpression *ne = 
	 NormalExpression::evaluate(itr()->filter, ~0);
      if (ne && ne->singleton_flag == NormalTerm::PRFX)
	 printSuperPrefixRanges_(ne->first()->prfx_set, fmt);
      else
	 cerr << "Error: Filter can only contain prefix filters." << endl;
      
      delete ne;
   }
   
   delete o;
}

void RtConfig::printPrefixes_(SetOfPrefix& nets, char *fmt) {
// eg: printRouteList(nets, "network  %p/%l^%n-%m\tmask %k\n");
//       where %p represents prefix
//             %l represents length
//             %L represents 32-length
//             %n represents n
//             %m represents m
//             %k represents mask
//             %K represents inverse of mask (i.e. ~mask)
//             %% escape %
//             \n carriage return
//             \t tab
   char buf[256];
   char *percent;
   char *format, *p;
   p = format = strdup(fmt);
   RadixSet::SortedPrefixIterator itr(&nets.members);
   u_int addr;
   u_int leng;
   u_int start;
   u_int end;
 
   for (bool ok = itr.first(addr, leng);
	ok;
	ok = itr.next(addr, leng)) {
      start = end = leng;
      format = p;
      while (percent = strchr(format, '%')) {
	 *percent = 0;
	 cout << format;
	 *percent = '%';
	 percent++;
	 switch (*percent)
	    {
	    case '%':
	    case '\t':
	    case '\n':
	       cout << *percent;
	       break;
	    case 'p':
	       cout << int2quad(buf, addr);
	       break;
	    case 'l':
	       cout << leng;
	       break;
	    case 'L':
	       cout << (32-leng);
	       break;
	    case 'n':
	       cout << start;
	       break;
	    case 'm':
	       cout << end;
	       break;
	    case 'k':
	       cout << int2quad(buf, masks[leng]);
	       break;
	    case 'K':
	       cout << int2quad(buf, ~masks[leng]);
	       break;
	    default:
	       cerr << "Unknown operator: " << *percent << endl;
	    }
	 format = ++percent;
      }
      if (*format) cout << format;
   }
   free(p);
}

void RtConfig::printPrefixRanges_(SetOfPrefix& nets, char *fmt) {
// eg: printRouteList(nets, "network  %p/%l^%n-%m\tmask %k\n");
//       where %p represents prefix
//             %l represents length
//             %L represents 32-length
//             %n represents n
//             %m represents m
//             %k represents mask
//             %K represents inverse of mask (i.e. ~mask)
//             %% escape %
//             \n carriage return
//             \t tab
   char buf[256];
   char *percent;
   char *format, *p;
   p = format = strdup(fmt);
   RadixSet::SortedPrefixRangeIterator itr(&nets.members);
   u_int addr;
   u_int leng;
   u_int start;
   u_int end;
 
   for (bool ok = itr.first(addr, leng, start, end);
	ok;
	ok = itr.next(addr, leng, start, end)) {
      format = p;
      while (percent = strchr(format, '%')) {
	 *percent = 0;
	 cout << format;
	 *percent = '%';
	 percent++;
	 switch (*percent)
	    {
	    case '%':
	    case '\t':
	    case '\n':
	       cout << *percent;
	       break;
	    case 'p':
	       cout << int2quad(buf, addr);
	       break;
	    case 'l':
	       cout << leng;
	       break;
	    case 'L':
	       cout << (32-leng);
	       break;
	    case 'n':
	       cout << start;
	       break;
	    case 'm':
	       cout << end;
	       break;
	    case 'k':
	       cout << int2quad(buf, masks[leng]);
	       break;
	    case 'K':
	       cout << int2quad(buf, ~masks[leng]);
	       break;
	    default:
	       cerr << "Unknown operator: " << *percent << endl;
	    }
	 format = ++percent;
      }
      if (*format) cout << format;
   }
   free(p);
}

void RtConfig::printSuperPrefixRanges_(SetOfPrefix& nets, char *fmt) {
// eg: printRouteList(nets, "network  %p/%l^%n-%m\tmask %k\n");
//       where %p represents prefix
//             %l represents length
//             %L represents 32-length
//             %n represents n
//             %m represents m
//             %k represents mask
//             %K represents inverse of mask (i.e. ~mask)
//             %% escape %
//             \n carriage return
//             \t tab
   char buf[256];
   char *percent;
   char *format, *p;
   p = format = strdup(fmt);
   RadixSet::SortedIterator itr(&nets.members);
   u_int addr;
   u_int leng;
   u_int64_t rngs;
 
   for (bool ok = itr.first(addr, leng, rngs);
	ok;
	ok = itr.next(addr, leng, rngs)) {
      format = p;
      while (percent = strchr(format, '%')) {
	 *percent = 0;
	 cout << format;
	 *percent = '%';
	 percent++;
	 switch (*percent)
	    {
	    case '%':
	    case '\t':
	    case '\n':
	       cout << *percent;
	       break;
	    case 'p':
	       cout << int2quad(buf, addr);
	       break;
	    case 'l':
	       cout << leng;
	       break;
	    case 'L':
	       cout << (32-leng);
	       break;
	    case 'k':
	       cout << int2quad(buf, masks[leng]);
	       break;
	    case 'K':
	       cout << int2quad(buf, ~masks[leng]);
	       break;
	    case 'D':
	       cout << int2quad(buf, (int) rngs);
	       break;
	    default:
	       cerr << "Unknown operator: " << *percent << endl;
	    }
	 format = ++percent;
      }
      if (*format) cout << format;
   }
   free(p);
}

void RtConfig::accessList(char *filter) {
   Buffer peval(strlen(filter)+16);
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {
      AttrIterator<AttrFilter> itr(o, "peval");
      
      NormalExpression *ne = 
	 NormalExpression::evaluate(itr()->filter, ~0);
      if (ne && ne->singleton_flag == NormalTerm::PRFX)
	 printAccessList(ne->first()->prfx_set);
      else
	 cerr << "Error: Filter can only contain prefix filters." << endl;
      
      delete ne;
   }
   
   delete o;
}

void RtConfig::aspathAccessList(char *filter) {
   Buffer peval(strlen(filter)+16);
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {
      AttrIterator<AttrFilter> itr(o, "peval");
      
      NormalExpression *ne = 
	 NormalExpression::evaluate(itr()->filter, ~0);
      if (ne && ne->singleton_flag == NormalTerm::AS_PATH)
	 printAspathAccessList(ne->first()->as_path);
      else
	 cerr << "Error: Filter can only contain as path filters." << endl;
      
      delete ne;
   }
   
   delete o;
}



