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
//             John Mehringer <mehringe@isi.edu>

#include "config.h"

#include <typeinfo>
#include <iostream.h>
#include <iomanip.h>
#include <cstring>
#include <cctype>


#include <cstdlib>
extern "C" {
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif
}

#include "util/Argv.hh"
#include "util/net.hh"
#include "util/rusage.hh"
#include "util/debug.hh"
#include "util/trace.hh"
#include "util/version.hh"

#include "rpsl/schema.hh"

#include "irr/irr.hh"
#include "irr/autnum.hh"
#include "irr/rawhoisc.hh"
#include "irr/ripewhoisc.hh"

#include "bgp/bgproute.hh"

//IRR * irr; // Global variable defined in irr/irr.cc
             // Used for bug 
            
static ASt source_as;  // AS number of source for prpath trace
static IRR * whois;    // Connection to IRR server for query
static ActionDictionary * actiondictionary;
Rusage ru;

static char * destination_arg;

static IPAddr * destination_ip = NULL;
static Prefix * destination_prefix = NULL;
static ASt destination_as;

int  opt_rusage        = 0;
char *opt_prompt       = "prpath> ";
char *opt_my_as        = NULL;
int  opt_echo_as       = 0;
int  opt_max_path_length = 8;
int  path_length_limit = 12;
int  opt_interactive = 1;

extern int BGPROUTE_MAX_ASPATH_LENGTH;

void Announce(BGPRoute &bgproute, ASt as);


//--------------------------------------------------------------
// void usage(char *argv[])
//
//
//--------------------------------------------------------------
void usage(char *argv[]) {
   cerr << "Usage: " << argv[0] << " to be written later" << endl;
   exit(1);
}

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
      {"-rusage", ARGV_BOOL, (char *) NULL,           (char *) &opt_rusage,
       "On termination print resource usage"},
      {"-prompt", ARGV_STRING,  (char *) NULL,        (char *) &opt_prompt,
       "Prompt"},

      IRR_COMMAND_LINE_OPTIONS,

      // prpath specific arguments
      {"-as",  ARGV_STRING,       (char *) NULL, (char *) &opt_my_as,
       "AS number of the aut-num object to use."},
      {"-mpl", ARGV_INT,          (char *) NULL, (char *) &opt_max_path_length,
       "Max path length"},

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

   if (opt_max_path_length) 
     BGPROUTE_MAX_ASPATH_LENGTH = opt_max_path_length;

   // if there are remaining arguments
   // the first one is my as number
   // the second one is a destination prefix
   switch (argc) {
     case 3 :
       opt_my_as = argv[1];
     case 2:
       destination_arg = argv[argc - 1];
       if ((destination_arg[0] == 'A' || destination_arg[0] == 'a') && 
           (destination_arg[1] == 'S' || destination_arg[1] == 's')) {
         destination_as = atoi(destination_arg + 2);
       } else if (strchr(destination_arg,'/')) {
         destination_prefix = new Prefix(destination_arg);
       } else {
         destination_ip = new IPAddr(destination_arg);
         destination_prefix = new Prefix(destination_ip->Prefix::get_text());
       }
       opt_interactive = 0;
     case 1:
       break;
     default:
       cerr << "Wrong number of arguments..." << endl;
       exit(-1);
   }
   
   if (opt_my_as)
     source_as = atoi(opt_my_as + 2);
   else {
     ipAddr myip;
     myip.setToMyHost();
     if ((source_as = whois->getOrigin("%s/32", myip.getIpaddr())) == 0) {
	    cerr << "prpath could not guess your AS number, "
	            "please specify an AS number on the command line." 
	         << endl;
	    delete whois;
	    exit(1);
	  }
   }      

   // have a prompt only if the input is coming from a tty
   if (!opt_interactive || !isatty(fileno(stdin)) || !isatty(fileno(stdout)))
      opt_prompt = NULL;
}

//-------------------------------------------------------------
// void Import(BGPRoute &bgproute, ASt export_as, ASt import_as,
//             IPAddr *import_ip, IPAddr *export_ip)
//
//  export_as was the current as import_as is the peer_as 
//  from IterateExports and 
//
//  Now we have to check to see if the peer as will import the
//  current as since BGP works from destination to source
//
//  import_ip & export_ip are the ip addresses of the routers
//      
//     also look at CIDRAdvisor/proxyagg.cc for similar code
//-------------------------------------------------------------
void Import(BGPRoute &bgproute, ASt export_as, ASt import_as,
            const IPAddr *import_ip, const IPAddr *export_ip) {
  const AutNum * import_autnum = NULL;
  // Check to see if loop is created by adding this AS 
  if (! bgproute.loopCheck(import_as)) {  
    // Get Imports from IRR 
    import_autnum = whois->getAutNum(import_as);
    if (import_autnum) {
      AutNumSelector<AttrImport> import_itr(import_autnum, "import", NULL,
					    export_as, export_ip, import_ip);
      if (const FilterAction * fa = import_itr.first()) {
              for (; fa; fa = import_itr.next()) {
        // if not at max nodes in route 
        // then continue depth first search
          if (!bgproute.atMaxRouteLength()) {
            if (bgproute.isMatching(import_as, fa->filter, whois)) {
              if (import_as == bgproute.source) { // Have we found the source
                 // If we have found the source as then print out our route      
                bgproute.print();
              } else {
                bgproute.executeAction(fa->action, actiondictionary);
                Announce(bgproute, import_as);
              }
            } // end if (bgproute.isMatching())
          } else {
            //cerr << "  Max Path length reached." << endl;
          }
        } // end for
        
      } else {  //No Imports then Check defaults
      // cerr << "   No Imports! Checking for default..." << endl;
        AttrIterator<AttrDefault> itr(import_autnum, "default");
        for (const AttrDefault *  ad = itr.first(); ad; ad = itr.next()) {
          if (! bgproute.atMaxRouteLength()) {
            if (bgproute.isMatching(import_as,ad->filter, whois)) {
              if (import_as == bgproute.source) { // Check to see if we found the source
                bgproute.print();
              } else {
                bgproute.executeAction(ad->action, actiondictionary);
                Announce(bgproute, import_as);
              }
            } // end if (bgproute.isMatching
          } else {
            //cerr << "Max Path length reached." << endl;
          }
        }  // end for
      }
    }  // end if (import_autnum)
  }  // end if (! bgproute.loopCheck())
}

//--------------------------------------------------------------
//  void IterateExports(BGPRoute &bgproute, ASt as, AutNum * as_autnum,
//                      ASt peer_as, IPAddr * peer_ip, IPAddr * local_ip)
//
//    Find export filter which contain peer_as,
//    Loop over list of exports,
//    Execute the actions for that export filter
//
//    peer_ip & local_ip are ip addresses for the routers
//-------------------------------------------------------------
void IterateExports(BGPRoute &bgproute, ASt as, const AutNum * as_autnum,
                    ASt peer_as, const IPAddr * peer_ip, 
		    const IPAddr * local_ip) {
  // Get matching Export Filter-Actions
  AutNumSelector<AttrExport> export_itr(as_autnum, "export", NULL, peer_as, peer_ip, local_ip);
  const FilterAction * fa;
  BGPRoute * import_route;
  // Check each Export filter for possible correct route
  for (fa = export_itr.first(); fa; fa = export_itr.next()) {
    import_route = new BGPRoute(bgproute); // Duplicate the currrent BGPRoute 
                                           // to check this possible path

    // For each export check if they allow the current AS's  
    // traffic on their network by looking at the peer AS's imports
    // Import(bgproute, current as, ip of current as, peer AS, peer IP address)
    import_route->executeAction(fa->action, actiondictionary);
    Import(*import_route, as, peer_as, peer_ip, local_ip);
    delete import_route;  // We are done with it
  }  
}  

//--------------------------------------------------------------
//  void Announce(BGPRoute &r, ASt as) 
//    Analogous to a bgp AS announcement
//      Check exports of current AS to see where to go next
//      doing a recursive depth first search on each export  
//      it terminates when either the max route length is reached
//      or the BGP's source AS is found
//
//   Announce()-->IterateExports()-->Imports()-->Announce()...until path is found 
//-------------------------------------------------------------
void Announce(BGPRoute &bgproute, ASt as) {
   const AutNum * as_autnum = whois->getAutNum(as);
   ASt peer_as;
   bgproute.add(as);  // Add the current AS as to the BGPRoute path list

   if (as_autnum) {  // Do we have an autnum record for this as?
      // Find all peers of current AS
      AutNumPeeringIterator itr(as_autnum);
      for (const Peering *peering = itr.first(); itr; peering = itr.next()){
	 // Get exports containing peerAS and check the imports of the peer
	 IterateExports(bgproute, as, as_autnum,
			peering->peerAS, &peering->peerIP, &peering->localIP);
      } // End for
   } else 
      cerr << "Warning: No autnum record found for AS" << as << endl;
}
  

//-------------------------------------------------------------
// main (int argc, char **argv, char **envp)
//
//
//-------------------------------------------------------------
main (int argc, char **argv, char **envp) {
  const AutNum * myAutNum;  
  BGPRoute * bgproute;

  schema.initialize();
    
  init_and_set_options(argc, argv, envp);
  
  irr = whois;  // For irr/autnum.hh  AutNumIterator::isMatching(Attribute *attr)

  actiondictionary = new ActionDictionary;
 
  if (destination_prefix != NULL) {
    bgproute = new BGPRoute(source_as, destination_prefix, whois);
  } else {
    bgproute = new BGPRoute(source_as, destination_as);
  }

 /* myAutNum = irr->getAutNum(2764);
  AutNumPeeringIterator itr(myAutNum);
  for (const Peering *peering = itr.first(); itr; peering = itr.next()) {
     cout << "AS" << peering->peerAS;
     if (!peering->peerIP.isNull())
	cout << " " << peering->peerIP;
     if (!peering->localIP.isNull()) 
	cout << " at " << peering->localIP;
     cout << "\n";
  }
   
  exit(1);*/

  // Start the search from destination to the source AS
  for (ItemASNO * asln = bgproute->origin->list.head(); asln;
                  asln = bgproute->origin->list.next(asln)) {
    if (asln->asno) {
      cout << "Route:" << endl;
      cout << "  NLRI: ";
      
      if (destination_prefix)
        cout << destination_arg << endl;
      else
        cout << "don't care" << endl;
        
      cout << "  Origins: AS" << asln->asno << endl; 

      Announce(*bgproute, asln->asno); 
    } else
      cout << "Unable to lookup destination AS" << endl;
  }
  
  delete irr;
  delete actiondictionary;
}
