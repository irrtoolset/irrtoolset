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
//             John Mehringer	<mehringe@isi.edu>  Updated for RAToolSet 4.x

#include "config.h"
#include <cstdlib>

extern "C" {
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif
}
#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>

#include "util/Argv.hh"
#include "util/net.hh"
#include "util/rusage.hh"
#include "util/debug.hh"
#include "util/trace.hh"
#include "util/version.hh"

#include "irr/irr.hh"

//#include "Node.h"
//#include "aut-num.hh"
//#include "Route.hh"
#include "CIDRAdvisor.hh"
#include "bgp/bgproute.hh"

static IRR * whois;

Rusage ru;

int  opt_radius           = -2;
int  opt_rusage           = 0;
//char *opt_prompt          = "CIDRAdvisor> ";
char *opt_agg_as          = NULL;
char *opt_proxy_as        = NULL;
int  opt_echo_as          = 0;
int  opt_incl_multihomed  = 0;

static ASt source_as;
static ASt proxy_as = 0;
static Origin proxies;
addr* chk_multi_homed(addr **);

//--------------------------------------------------------------
// int start_tracing(char *dst, char *key, char *nextArg)
//
//
//--------------------------------------------------------------
int start_tracing(char *dst, char *key, char *nextArg) {
   if (nextArg) {
      trace.enable(nextArg);
      return 1; // return 1 to signify nextArg is used by us
   }
   return 0; 
}


//--------------------------------------------------------------
// int start_debugging(char *dst, char *key, char *nextArg)
//
//
//--------------------------------------------------------------
int start_debugging(char *dst, char *key, char *nextArg) {
   if (nextArg) {
      Debug(dbg.enable(atoi(nextArg)));
      return 1; // return 1 to signify nextArg is used by us
   }
   return 0;
}


//--------------------------------------------------------------
// void init_and_set_options (int argc, char **argv, char **envp)
//
//
//--------------------------------------------------------------
void init_and_set_options (int argc, char **argv, char **envp) {
   ArgvInfo argTable[] = {
      // RAToolSet common arguments
      // key, type, src, dst, help
      {"-T", ARGV_FUNC, (char *) &start_tracing,      (char *) NULL, 
       "Start tracing the next argument"},
      {"-D", ARGV_FUNC, (char *) &start_debugging,    (char *) NULL, 
       "Start debugging the next argument"},
      {"-version", ARGV_FUNC, (char *) &version,      (char *) NULL,
       "Show version"},
      {"-rusage", ARGV_CONSTANT, (char *) 1,          (char *) &opt_rusage,
       "On termination print resource usage"},
//      {"-prompt", ARGV_STRING,  (char *) NULL,        (char *) &opt_prompt,
//       "Prompt"},
      
      IRR_COMMAND_LINE_OPTIONS,

      // CIDRadvisor specific arguments
      {"-as",     ARGV_STRING,  (char *) NULL,        (char *) &opt_agg_as,
       "AS performing the aggregation"},
      {"-proxy",  ARGV_STRING,  (char *) NULL,        (char *) &opt_proxy_as,
       "Comma separated list of ASes to be proxy aggregated"},
      {"-radius", ARGV_INT,     (char *) NULL,        (char *) &opt_radius,
       "Used to configure the amount of policy and topology considered"},
      {"-incl_multihomed", ARGV_CONSTANT, (char *) 1, (char *) &opt_incl_multihomed, 
       "Include all multihomed prefixes in the aggregates"},
             
      {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL, (char *) NULL}
   };

   IRR::handleEnvironmentVariables(envp);
   
   IRR::IgnoreErrors(NULL, NULL, NULL);

   if (ParseArgv(&argc, argv, argTable, 0) != ARGV_OK) {
      cerr << endl;
      exit(1);
   }
   
   
   // Setup Connection to IRR server
   if (! (whois = IRR::newClient())) {
      cerr << "Unknown protocol!" << endl;
      exit(1);
   }


   // if there is a remaining argument, it is my as number
   switch (argc) {
   case 2 :
      opt_agg_as = argv[1];
   case 1:
      break;
   default:
      cerr << "Wrong number of arguments..." << endl;
      exit(-1);
   }

   // have a prompt only if the input is coming from a tty
//   if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
//      *opt_prompt = 0;
}





//--------------------------------------------------------------
// void parse_proxy_ases()
//
//  Takes the listed proxy ases passed in from the command line 
//  converts them to ASt and addes them to the list of proxy ASes
//  to check for proxy aggregation
//
//  Requires:  static Origin proxies; // defined at the top of 
//                                       this file
//
//--------------------------------------------------------------
void parse_proxy_ases() {
  char as_num[8];
  //* Pix as;
  ASt as;
  int i = 0, j = 0;
  int index = 0;

  while (opt_proxy_as[i] != '\0') {
    // Extract each AS to a buffer for processing
    if(opt_proxy_as[i] != ',') {
      as_num[j++] = opt_proxy_as[i];
    } else {
    // Once we have an AS then convert to ASt and
    // add it to the proxy AS list
      as_num[j] = '\0';
      j = 0;
      //Convert String to ASt
      as = atoi(as_num + 2);
      if(!(proxies.contains(as))) {
        //*ListNodePix *p = new ListNodePix(as);
        proxies.add(as);
      }
    }
    ++i;
  }
  
  // Take care of last AS from the opt_proxy_as string
  as_num[j] = '\0';
  as = atoi(as_num + 2);
  if(as && !(proxies.contains(as))) {
    proxies.add(as);   
  }
}    


//--------------------------------------------------------------
// main (int argc, char **argv, char **envp) 
//   CIDRadvisor is a tool which will give possible aggregations
//   of routes to a given AS.  If no AS is given the routes are
//   determined from the host AS
//
//--------------------------------------------------------------

main (int argc, char **argv, char **envp) {
   radix_node * root;
   addr * aggregates;
   int chk_proxies = 0;
   int num_prefixes;
   //*_SetOfPix routes;
   const PrefixRanges * routes;

   schema.initialize();

   init_and_set_options(argc, argv, envp);

   if (opt_agg_as && ((opt_agg_as[0] == 'A' || opt_agg_as[0] == 'a') &&
                      (opt_agg_as[1] == 'S' || opt_agg_as[1] == 's'))) {
     //*source_as = AS_map.add_entry(opt_agg_as);
     source_as = atoi(opt_agg_as + 2);
   } else {
     ipAddr myip;
     char prefix[16];
     myip.setToMyHost();
     source_as = whois->getOrigin(myip.getIpaddr());
   }
   
         
   //- Create root of radiz tree, to store routes
   root = radix_create_node(NULL,0,0,INVALID_AS,EMPTY);

   aggregates = NULL;
   proxies.clear();

   if(opt_proxy_as) 
       parse_proxy_ases();
       
   //- Add proxy AS routes
   for (ItemASNO * asln = proxies.list.head(); asln;
                   asln = proxies.list.next(asln)) {
     routes = whois->expandAS(asln->asno);
     if(routes->empty()) {
       cout << "No routes present for AS" << asln->asno << "\n";
       continue;
     }
       
     chk_proxies = 1;
    
     //- Add Each Proxy Route to Radix for later aggregation
     for (int i = routes->low(); 
	           i < routes->fence(); i++)
       radix_insert((*routes)[i].get_ipaddr(), (*routes)[i].get_length(), source_as, root);
     
   }

   //- Get Local Routes
   routes = whois->expandAS(source_as);
   
   if(!routes || routes->isEmpty()) {
     cout << "No routes present for " << opt_agg_as << "\n";
     exit(1);
   }
      
   //- Add Each Source AS Route to Radix for later aggregation
   for (int i = routes->low();
	         i < routes->fence(); i++) {
     radix_insert((*routes)[i].get_ipaddr(), (*routes)[i].get_length(), source_as, root);
    
    //**char prefix_quad[16];

    //**int2dd(prefix_quad, (*routes)[i].get_ipaddr());
    //**printf("%s/%d \n",prefix_quad,(*routes)[i].get_length());
   }

   //- Aggregate Routes in Radix Tree
   radix_aggregate(root);
   aggregates = radix_get_aggs(root);
   
   //- Multihomed aggregation more difficult 
   //- because we might send all traffic to one interface
   //- by poor aggregation choices
   
   if (!opt_incl_multihomed) {
     aggregates = chk_multi_homed(&aggregates);
   }
   
   cout <<  "Best possible aggregation:" << endl;
   print_aggregates(aggregates,COMPONENTS);
   
   //- if opt_radius is -2 then ignore policy checks
   //- otherwise you have to check AS policies for
   //- aggregation
   if (opt_radius > -2 || opt_radius == -3) { 
   
     const AutNum * source_autnum = whois->getAutNum(source_as);
     if ( ! source_autnum)
       cerr << "No autnum for AS" << source_as << endl;
     
     examine_policy(source_as, chk_proxies, aggregates, opt_radius, whois);
   }
   //- Clean up after ourselves
   radix_delete_tree(root);
   
   if (opt_rusage)
     clog << ru;
      
   delete_list(&aggregates);
}





//--------------------------------------------------------------
// addr* chk_multi_homed(addr** aggregates) 
//  
//  Check for multihomed aggregates and if found ask if we 
//  should try aggregating them.  Could be dangerous and screw 
//  up route preferences.
//--------------------------------------------------------------
addr* chk_multi_homed(addr** aggregates) {
  //*Route route;
  //Origin * origin;
  NLRI * nlri;
  
  radix_node * root;  // root of the final included routes radix 
  addr * aggs = NULL, * incl_addrs = NULL, * excl_addrs = NULL;
  addr * incl_aggs = NULL, * incl_nonaggs = NULL;
  char prefix_quad[16];
  char prefix_buffer[32];

  //- Loop through all routes to check if they are multihomed or not
  for (addr * index = *aggregates; index ; index = index->next) {
    for (addr *comps = index->components; comps ; comps = comps->next) {
      //-comps->
      //*Pix rt = (comps->rt).nlri.pix;
      
      //-route.clear();
      //-route.nlri.pix = rt;
      //-route.define();
      //~origin = new Origin(comps->getStringPrefix(prefix_buffer), whois);
      comps->findOrigin(whois);
      //- if only one origin then not multihomed
      //~if(origin->getSize() == 1) 
      if(comps->bgproute->origin->getSize() == 1) { 
        append(&incl_addrs, comps);
      }
        
      //- else if > 1 then is multihomed so we
      //~else if(origin->getSize() > 1) {
      else if(comps->bgproute->origin->getSize() > 1) {
		
        //- Since multihomed must check to see if ok to aggregate
        char prefix_quad[16];
        int2dd(prefix_quad,comps->prefix);
        printf("\n%s/%d\n",prefix_quad,comps->length);
        printf("Origins:\n");
        //*for(ListNodePix *q = route.origin.head() ; q ; q = route.origin.next(q->l))
        //*  cout << AS_map.key(q->pix) << " " ;
        for (ItemASNO * asln = comps->bgproute->origin->list.head(); asln; 
                        asln = comps->bgproute->origin->list.next(asln))
          cout << "AS" << asln->asno << " ";
        cout << endl << "Can the address be aggregated?";
        char ch;
        ch = 'n'; // default is multihomed addrs not incl in aggs
        cin >> ch;
        if(ch == 'y') 
          append(&incl_addrs,comps);
        else 
          append(&excl_addrs,comps);
      }
    }



    if(excl_addrs) {
      append_aggregates(&aggs,&excl_addrs);
      root = radix_create_node(NULL,0,0,INVALID_AS,EMPTY);

      for(addr* tmp = incl_addrs; tmp ; tmp = tmp->next) {
        //*ListNodePix *p = (incl_addrs->rt).origin.head();
       //* radix_insert(tmp->prefix,tmp->length,p->pix,root);
        ItemASNO * asln = incl_addrs->bgproute->origin->list.head();
        radix_insert(tmp->prefix,tmp->length, asln->asno, root);
       
        incl_aggs = radix_get_aggs(root);
        //incl_nonaggs = radix_get_nonaggs(root);
        append_aggregates(&aggs,&incl_aggs);
      }
    } else {
      append(&aggs,index);
    }
    delete_list(&incl_addrs);
    delete_list(&excl_addrs);
    delete_list(&incl_aggs);
    //delete_list(&incl_nonaggs);
  }

  delete_list(aggregates);
  cout << "\n";
  return(aggs);
}
