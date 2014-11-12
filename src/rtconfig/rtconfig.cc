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
#include <cstdlib>

extern "C" {
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif
}
#include <iostream>
#include <iomanip>
#include "irr/irr.hh"
#include "irr/rawhoisc.hh"
#include "irrutil/debug.hh"
#include "irrutil/trace.hh"
#include "irrutil/rusage.hh"
#include "rtconfig.hh"
#include "irrutil/Argv.hh"
#include "irrutil/version.hh"
#include "re2dfa/regexp_nf.hh"
#include "f_cisco.hh"
#include "f_junos.hh"
#include "rpsl/schema.hh"
#include "irr/classes.hh"

#if HAVE_LIBREADLINE && HAVE_LIBHISTORY
#include <readline/readline.h>
#include <readline/history.h>
#endif // HAVE_LIBREADLINE && HAVE_LIBHISTORY

using namespace std;

bool opt_rusage                  = false;
Rusage ru(clog, &opt_rusage);

char *opt_prompt                 = (char *)"rtconfig> ";
bool opt_asdot                   = false;

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
   const char     *name;
   RtConfig *config;
} config_format_type;

CiscoConfig ciscoConfig;
JunosConfig junosConfig;

config_format_type config_formats[] = {
   { "cisco",       &ciscoConfig },
   { "junos",       &junosConfig },
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

     {"-asdot", ARGV_BOOL, (char *) NULL, (char *) &opt_asdot,
      "print AS numbers in asdot format."},

     {"-config", ARGV_FUNC, (char *) &select_config_format, (char *) NULL, 
      "Configuration format (junos, cisco)"},
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
      "deprecated\n"},
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
     {"-cisco_empty_lists", ARGV_BOOL, 
      (char *) NULL, (char *) &CiscoConfig::emptyLists,
      "Generate access lists for ANY and NOT ANY prefix filters.\n\t\t\t\tCisco only."},
     {"-cisco_no_default_afi", ARGV_BOOL, (char *) NULL, (char *) &AddressFamily::noDefaultAfi,
      "Do not assume the router defaults to ipv4 unicast address family."},
     {"-junos_no_compress_acls", ARGV_BOOL, 
      (char *) NULL, (char *) &JunosConfig::compressAcls,
      "Do not combine multiple route-filter lines into a single line whenever possible.\n\t\t\t\tJunos only."},
     {"-junos_replace", ARGV_BOOL, 
      (char *) NULL, (char *) &JunosConfig::load_replace,
      "Output replace: statements to be used with \"load replace\" on Junos.\n\t\t\t\tJunos only."},
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

#if HAVE_LIBREADLINE && HAVE_LIBHISTORY
int rtconfig_input (char *buf, int maxcmdlen) {
   static char *cmd = NULL;
   static int cmdlen = 0;
   static int upto = 0; 
   int buflen;

   if (opt_prompt) {
      if (upto >= cmdlen)
      {
         if (cmd != NULL)
            free(cmd);   

         cmd = readline(opt_prompt);

         if (cmd == NULL)   
            return 0;

         if (cmd[0] != '\0')
            add_history(cmd);

         cmdlen = strlen(cmd);
         cmd[cmdlen++] = '\n';   
         upto = 0;   
      }

      buflen = min(cmdlen - upto, maxcmdlen);
      memcpy(buf, cmd + upto, buflen);
      upto += buflen;

      return buflen;    
   } else {
      /* regular input for yyparse() */
      return fread(buf, 1, maxcmdlen, stdin); 
   }
}
#endif // HAVE_LIBREADLINE && HAVE_LIBHISTORY

int main(int argc, char **argv, char **envp) {
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

   const InetRtr *rtr = irr->getInetRtr(symbols.symID(name));
   if (!rtr)	{
	   cerr << "Error: no object for router: " << name << endl;
   }

   const AttrProtocol *bgp = schema.searchProtocol("BGP4");

   AttrGenericIterator<ItemASNO> itr1(rtr, "local-as");
   ASt myAS = itr1()->asno;
	
   InterfaceIterator itr = InterfaceIterator(rtr);
   // may need TBD on action/tunnel details
   PeerIterator itr2 = PeerIterator(rtr);

   for (Peer *peer = itr2.first(); peer; peer = itr2.next()) {
	   for (Interface *ifaddr = itr.first(); ifaddr; ifaddr = itr.next()) {
       MPPrefix *myIP = new MPPrefix(*(ifaddr->interface.ifaddr));
         if (peer->peer.protocol == bgp) {
	 		     ASt peerAS = ((ItemASNO *) peer->peer.searchOption("asno")->args->head())->asno;
	 		     importP(myAS, myIP, peerAS, peer->peer.peer);
	 		     exportP(myAS, myIP, peerAS, peer->peer.peer);
      	}
 	   }
   }
}

// reimplemented to handle mp-filters
void RtConfig::printPrefixes(char *filter, char *fmt) {

   Buffer peval(strlen(filter)+16);

   if (strstr(filter, "afi ")) {
     peval.append("mp-");
   }
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {

     NormalExpression *ne;
     if (strcmp(o->type->name,"mp-peval") == 0) {
       AttrIterator<AttrMPPeval> itr(o, "mp-peval");
       ne = NormalExpression::evaluate(itr()->filter, ~0);
     }
     else {
       AttrIterator<AttrFilter> itr(o, "peval");
       ne = NormalExpression::evaluate(itr()->filter, ~0);
     }
       
     if (ne) {
       if (ne->is_any() != NEITHER) {
         cerr << "Warning: filter \"" << filter << "\" matches ANY/NOT ANY" << endl;
         if (ciscoConfig.emptyLists) {
           // print "any/not any" prefix list
           char *ptr = strstr(filter, "afi ");
           if (ptr != NULL) {
             ptr += 4;
             while (isspace(*ptr)) {
               ptr++;
             }
             if (strncmp(ptr, "ipv4", 4) == 0) {
               // ipv4 prefix list
               SetOfPrefix *nets = new SetOfPrefix;
               if (ne->is_any() == ANY)
                  nets->not_ = true;
               printPrefixes_(*nets, fmt);
             } else if (strncmp(ptr, "ipv6", 4) == 0) {
               // ipv6 prefix list
               SetOfIPv6Prefix *nets = new SetOfIPv6Prefix;
               if (ne->is_any() == ANY)
                  nets->not_ = true;
               printPrefixes_(*nets, fmt);
             } else {
               cerr << "WARNING: unknown afi value! Can't print prefixes for filter \"" << filter << "\"" << endl;
             }
           } else {
             // ipv4 prefix list
             SetOfPrefix *nets = new SetOfPrefix;
             if (ne->is_any() == ANY)
                nets->not_ = true;
             printPrefixes_(*nets, fmt);
           }
         }
       } else {
         for (NormalTerm *nt = ne->first(); nt; nt = ne->next())
           if (nt->prfx_set.is_universal() && nt->ipv6_prfx_set.is_universal())
             cerr << "Error: badly formed prefix filter \"" << filter << "\"\n";
         for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) {
           printPrefixes_(nt->prfx_set, fmt);
           printPrefixes_(nt->ipv6_prfx_set, fmt);
         }
       }
       delete ne;
     }
   } else {
     cerr << "Warning: creation of filter object for \"" << filter << "\" revealed error!\n";
   }
   delete o;
}

void RtConfig::printPrefixRanges(char *filter, char *fmt) {
   Buffer peval(strlen(filter)+16);

   if (strstr(filter, "afi ")) {
     peval.append("mp-");
   }
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {

     NormalExpression *ne;
     if (strcmp(o->type->name,"mp-peval") == 0) {
       AttrIterator<AttrMPPeval> itr(o, "mp-peval");
       ne = NormalExpression::evaluate(itr()->filter, ~0);
     }
     else {
       AttrIterator<AttrFilter> itr(o, "peval");
       ne = NormalExpression::evaluate(itr()->filter, ~0);
     }

     if (ne) {
       if (ne->is_any() != NEITHER) {
         cerr << "Warning: filter \"" << filter << "\" matches ANY/NOT ANY" << endl;
         if (ciscoConfig.emptyLists) {
           // print "any/not any" prefix list
           char *ptr = strstr(filter, "afi ");
           if (ptr != NULL) {
             ptr += 4;
             while (isspace(*ptr)) {
               ptr++;
             }
             if (strncmp(ptr, "ipv4", 4) == 0) {
               // ipv4 prefix list
               SetOfPrefix *nets = new SetOfPrefix;
               if (ne->is_any() == ANY)
                  nets->not_ = true;
               printPrefixRanges_(*nets, fmt);
             } else if (strncmp(ptr, "ipv6", 4) == 0) {
               // ipv6 prefix list
               SetOfIPv6Prefix *nets = new SetOfIPv6Prefix;
               if (ne->is_any() == ANY)
                  nets->not_ = true;
               printPrefixRanges_(*nets, fmt);
             } else {
               cerr << "WARNING: unknown afi value! Can't print prefix ranges for filter \"" << filter << "\"" << endl;
             }
           } else {
             // ipv4 prefix list
             SetOfPrefix *nets = new SetOfPrefix;
             if (ne->is_any() == ANY)
                nets->not_ = true;
             printPrefixRanges_(*nets, fmt);
           }
         }
       } else {
         for (NormalTerm *nt = ne->first(); nt; nt = ne->next())
           if (nt->prfx_set.is_universal() && nt->ipv6_prfx_set.is_universal())
             cerr << "Error: badly formed prefix filter \"" << filter << "\"\n";
         for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) {
           printPrefixRanges_(nt->prfx_set, fmt);
           printPrefixRanges_(nt->ipv6_prfx_set, fmt);
         }
       }
       delete ne;
     }
   } else {
     cerr << "Warning: creation of filter object for \"" << filter << "\" revealed error!\n";
   }
   delete o;
}
void RtConfig::printSuperPrefixRanges(char *filter, char *fmt) {
   Buffer peval(strlen(filter)+16);

   if (strstr(filter, "afi ")) {
     peval.append("mp-");
   }
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {

     NormalExpression *ne;
     if (strcmp(o->type->name,"mp-peval") == 0) {
       AttrIterator<AttrMPPeval> itr(o, "mp-peval");
       ne = NormalExpression::evaluate(itr()->filter, ~0);
     }
     else {
       AttrIterator<AttrFilter> itr(o, "peval");
       ne = NormalExpression::evaluate(itr()->filter, ~0);
     }

     if (ne) {
       if (ne->is_any() != NEITHER) {
         cerr << "Warning: filter \"" << filter << "\" matches ANY/NOT ANY" << endl;
         if (ciscoConfig.emptyLists) {
           // print "any/not any" prefix list
           char *ptr = strstr(filter, "afi ");
           if (ptr != NULL) {
             ptr += 4;
             while (isspace(*ptr)) {
               ptr++;
             }
             if (strncmp(ptr, "ipv4", 4) == 0) {
               // ipv4 prefix list
               SetOfPrefix *nets = new SetOfPrefix;
               if (ne->is_any() == ANY)
                  nets->not_ = true;
               printSuperPrefixRanges_(*nets, fmt);
             } else if (strncmp(ptr, "ipv6", 4) == 0) {
               // ipv6 prefix list
               SetOfIPv6Prefix *nets = new SetOfIPv6Prefix;
               if (ne->is_any() == ANY)
                  nets->not_ = true;
               printSuperPrefixRanges_(*nets, fmt);
             } else {
               cerr << "WARNING: unknown afi value! Can't print super prefix ranges for filter \"" << filter << "\"" << endl;
             }
           } else {
             // ipv4 prefix list
             SetOfPrefix *nets = new SetOfPrefix;
             if (ne->is_any() == ANY)
                nets->not_ = true;
             printSuperPrefixRanges_(*nets, fmt);
           }
         }
       } else {
         for (NormalTerm *nt = ne->first(); nt; nt = ne->next())
           if (nt->prfx_set.is_universal() && nt->ipv6_prfx_set.is_universal())
             cerr << "Error: badly formed prefix filter \"" << filter << "\"\n";
         for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) {
           printSuperPrefixRanges_(nt->prfx_set, fmt);
           printSuperPrefixRanges_(nt->ipv6_prfx_set, fmt);
         }
       }
       delete ne;
     }
   } else {
     cerr << "Warning: creation of filter object for \"" << filter << "\" revealed error!\n";
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
      while ((percent = strchr(format, '%'))) {
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

void RtConfig::printPrefixes_(SetOfIPv6Prefix& nets, char *fmt) {
   char buf[256];
   char *percent;
   char *format, *p;
   p = format = strdup(fmt);
   IPv6RadixSet::SortedPrefixIterator itr(&nets.members);
   ipv6_addr_t addr;
   u_int leng;
   u_int start;
   u_int end;
 
   for (bool ok = itr.first(addr, leng);
        ok;
        ok = itr.next(addr, leng)) {
      start = end = leng;
      format = p;
      while ((percent = strchr(format, '%'))) {
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
               cout << ipv62hex(&addr, buf);
               break;
            case 'l':
               cout << leng;
               break;
            case 'L':
               cout << (128-leng);
               break;
            case 'n':
               cout << start;
               break;
            case 'm':
               cout << end;
               break;
            case 'k':
               cout << ipv62hex(&(addr.getmask(leng)),buf);
               break;
            case 'K':
               cout << ipv62hex(&(~(addr.getmask(leng))),buf);
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
      while ((percent = strchr(format, '%'))) {
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

void RtConfig::printPrefixRanges_(SetOfIPv6Prefix& nets, char *fmt) {
   char buf[256];
   char *percent;
   char *format, *p;
   p = format = strdup(fmt);
   IPv6RadixSet::SortedPrefixRangeIterator itr(&nets.members);
   ipv6_addr_t addr;
   u_int leng;
   u_int start;
   u_int end;
 
   for (bool ok = itr.first(addr, leng, start, end);
        ok;
        ok = itr.next(addr, leng, start, end)) {
      format = p;
      while ((percent = strchr(format, '%'))) {
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
               cout << ipv62hex(&addr, buf);
               break;
            case 'l':
               cout << leng;
               break;
            case 'L':
               cout << (128-leng);
               break;
            case 'n':
               cout << start;
               break;
            case 'm':
               cout << end;
               break;
            case 'k':
               cout << ipv62hex(&(addr.getmask(leng)), buf);
               break;
            case 'K':
               cout << ipv62hex(&(~(addr.getmask(leng))), buf);
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
      while ((percent = strchr(format, '%'))) {
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

void RtConfig::printSuperPrefixRanges_(SetOfIPv6Prefix& nets, char *fmt) {
   char buf[256];
   char *percent;
   char *format, *p;
   p = format = strdup(fmt);
   IPv6RadixSet::SortedIterator itr(&nets.members);
   ipv6_addr_t addr;
   u_int leng;
   ipv6_addr_t rngs;
 
   for (bool ok = itr.first(addr, leng, rngs);
        ok;
        ok = itr.next(addr, leng, rngs)) {
      format = p;
      while ((percent = strchr(format, '%'))) {
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
               cout << ipv62hex(&addr,buf);
               break;
            case 'l':
               cout << leng;
               break;
            case 'L':
               cout << (128-leng);
               break;
            case 'k':
               cout << ipv62hex(&(addr.getmask(leng)), buf);
               break;
            case 'K':
               cout << ipv62hex(&(~(addr.getmask(leng))), buf);
               break;
            case 'D': 
               cout << ipv62hex(&(addr.getbits(leng)), buf);
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

// reimplemented to handle mp-filters
void RtConfig::accessList(char *filter) {
   Buffer peval(strlen(filter)+16);

   if (strstr(filter, "afi ")) {
     peval.append("mp-");
   }
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {
     NormalExpression *ne;
     if (strcmp(o->type->name,"mp-peval") == 0) {
       AttrIterator<AttrMPPeval> itr(o, "mp-peval");
       ne = NormalExpression::evaluate(itr()->filter, ~0);
     }
     else {
       AttrIterator<AttrFilter> itr(o, "peval");
       ne = NormalExpression::evaluate(itr()->filter, ~0);
     }

     if (ne) {
       if (ne->is_any() != NEITHER) {
         cerr << "Warning: filter \"" << filter << "\" matches ANY/NOT ANY" << endl;
         if (ciscoConfig.emptyLists) {
           // print "any/not any" prefix list
           char *ptr = strstr(filter, "afi ");
           if (ptr != NULL) {
             ptr += 4;
             while (isspace(*ptr)) {
               ptr++;
             }
             if (strncmp(ptr, "ipv4", 4) == 0) {
               // ipv4 prefix list
               SetOfPrefix *nets = new SetOfPrefix;
               if (ne->is_any() == ANY)
                 nets->not_ = true;
               printAccessList(*nets);
             } else if (strncmp(ptr, "ipv6", 4) == 0) {
               // ipv6 prefix list
               SetOfIPv6Prefix *nets = new SetOfIPv6Prefix;
               if (ne->is_any() == ANY)
                 nets->not_ = true;
               printAccessList(*nets);
             } else {
               cerr << "WARNING: unknown afi value! Can't print configuration for filter \"" << filter << "\"" << endl;
             }
           } else {
             // ipv4 prefix list
             SetOfPrefix *nets = new SetOfPrefix;
             if (ne->is_any() == ANY)
                nets->not_ = true;
             printAccessList(*nets);
           }
         }
       } else {
         for (NormalTerm *nt = ne->first(); nt; nt = ne->next())
           if (nt->prfx_set.is_universal() && nt->ipv6_prfx_set.is_universal())
             cerr << "Error: badly formed prefix filter \"" << filter << "\"\n";
         for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) {
           printAccessList(nt->prfx_set);
           printAccessList(nt->ipv6_prfx_set);
         }
       }
       delete ne;
     }
   } else {
     cerr << "Warning: creation of filter object for \"" << filter << "\" revealed error!\n";
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

      if (ne && ne->is_any() != NEITHER)
        cerr << "Warning: filter matches ANY/NOT ANY" << endl;

      if (ne && ne->singleton_flag == NormalTerm::AS_PATH)
	 printAspathAccessList(ne->first()->as_path);
      else
	cerr << "Error: badly formed AS_path filter." << endl;

      delete ne;
   }
   
   delete o;
}

// REIMPLEMENTED
void RtConfig::printPolicyWarning(ASt as, MPPrefix* addr, ASt peerAS, MPPrefix* peerAddr, const char* policy)	
{
	cerr << "Warning: AS" << as;
        cerr << " has no " << policy << " policy for AS" << peerAS;
        cerr << " " << peerAddr->get_ip_text();
        cerr << " at " << addr->get_ip_text() << endl;
}


