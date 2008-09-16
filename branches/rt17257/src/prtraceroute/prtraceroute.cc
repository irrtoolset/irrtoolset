//
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
// NOTE: the 'traceroute' part of this code was taken from 
// traceroute v1.2, and munged into my somewhat confused idea
// of c++ classes. the 'pr' part of the program was inspired by
// the original PRIDE prtraceroute.  The policy evaluation uses
// the ratoolset libs.
//

/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

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
//  Author(s): Rusty Eddy      <eddy@isi.edu>
//             John Mehringer  <mehringe@isi.edu>  Updated for RAToolSet 4.x
//
// Missing or unworking options: [-s -d -r]
// 

#include "config.h"
#include "prtraceroute.hh"

#include "irrutil/Argv.hh"
#include "irrutil/debug.hh"
#include "irrutil/trace.hh"
#include "irrutil/version.hh"

#include "rpsl/schema.hh"

#include "irr/irr.hh"

//#include "version.hh"
//#include "Argv.hh"

//Rusage ru;

////////////////////////////////////////////////////////////////////////

extern IRR * irr;  // defined in irr.cc

int  opt_rusage         = 0;

int policy_flag = 1;
int nflag = 0;
int nprobes = 3;
int max_ttl = 30;
int socket_options = 0;
int verbose = 0;
int waittime = 5;
int datalen = sizeof (probe_pkt);
char *dst;
int  port = 32768+666;
int tos = 0;
char *sourceif;
char *progname;

int debug = 0;

// Used for bgpropute.executeAction()
static ActionDictionary * actiondictionary;


// source routing
#define MAXSOURCES 9
int lsrr = 0;
ipAddr *source_routes[MAXSOURCES] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

void init_and_set_options (int, char **, char **);
void process_policies (Hop path[], int ttl);
Path path;
//extern WhoisParser whois;

////////////////////////////////////////////////////////////////////////
main (int argc, char **argv, char **envp) {
  progname = argv[0];
  
  schema.initialize();

  init_and_set_options (argc, argv, envp);
  
  /*  if (!verbose && policy_flag)
	whois.error.ignore (1);
    else
	whois.error.immediate(0);*/

  // dst and port are globals
  ipAddr *dstip   = new ipAddr (dst); // ipaddrs for src and dst
  dstip->error.die(progname);

  ttlProbe probe(dst, INADDR_ANY, port, lsrr, source_routes);
  probe.error.die (progname);

  u_long lastaddr = probe.srcInaddr();
  u_long dstaddr  = dstip->getInaddr();
  u_long newaddr; 
  ipAddr *localip = new ipAddr (lastaddr);
  localip->error.die (progname);
  
  if (policy_flag) {
    path.addHop (localip, 0);		    // set the first hop
    path.addDestination (dstip);	    // set the destination
  }
  

  ICMPProbeReply reply;		    // wait for incoming icmp's
  Timer t1, t2;
  int ttl, got_there = 0, unreachable = 0;

  printf ("prtraceroute to %s (%s), %d hops max, %d byte packets\n",
          dst, dstip->getIpaddr(), max_ttl, datalen);
          	    
  for (ttl = 1; ttl <= max_ttl; ++ttl ) {
    printf ("%2d ", ttl);
    for (int p = 0, seq = 0; p < nprobes; ++p) {
      int cc, i;
      t1.gettimeofday();
      probe.send (++seq, ttl);
      if (probe.error.warn(progname))
        continue;

      while ((cc = reply.wait_for_reply (waittime)) != 0) {
        t2.gettimeofday();
        if ((i = reply.ok(cc, probe.get_ident(), seq)) != 0) {
          newaddr = reply.srcInaddr();
          if (newaddr != lastaddr) {
            ipAddr *ipaddr = new ipAddr (newaddr);
            if (policy_flag) {
              path.addHop(ipaddr, ttl);
              printf (" [AS%d]", path.getHopAS(ttl));
            }
            if (!nflag) {
              printf (" %s (%s)", ipaddr->getName(),
              ipaddr->getIpaddr());
            } else {
              printf (" %s", ipaddr->getIpaddr());
            }
            lastaddr = newaddr;
          }
          printf ("  %g ms", t2.delta (t1));
		    
          switch (i-1) {
            case ICMP_UNREACH_PORT:
              if (reply.getTTL() <= 1)
                printf (" !");
              ++got_there;
              break;
            case ICMP_UNREACH_NET:
              ++unreachable;
              printf (" !N");
              break;
            case ICMP_UNREACH_HOST:
              ++unreachable;
              printf (" !H");
              break;
            case ICMP_UNREACH_PROTOCOL:
              ++got_there;
              printf (" !P");
              break;
            case ICMP_UNREACH_NEEDFRAG:
              ++unreachable;
              printf (" !F");
              break;
            case ICMP_UNREACH_SRCFAIL:
              ++unreachable;
              printf (" !S");
              break;
              // The following are added by rusty.
            case 6:			
              ++unreachable; // UNKNOWN NET
              printf (" ?N");
              break;
            case 7:
              ++unreachable; // DST UNKNOWN HOST
              printf (" ?H");
              break;
            case 8:
              ++unreachable;
              printf (" IH");		// host isolated OBSOLETE!
              break;
            case 9:
              ++unreachable;
              printf (" !AN");	// dst host admin, prohibited
              break;
            case 10:
              ++unreachable;
              printf (" !AH");	// dst net admin, prohibited
              break;
            case 11:
              ++unreachable;
              printf (" !TN");	// net unreach for TOS
              break;
            case 12:
              ++unreachable;
              printf (" !TH");	// dst unreach for TOS
              break;
		      case 13:
              ++unreachable;
              printf (" !AF");	// comm. admin prohibited
              break;
            case 14:
              ++unreachable;
              printf (" !PV");	// host preced. violation
              break;
            case 15:
              ++unreachable;
              printf (" !PC");	// precedence cut-off
              break;
          } // switch
          break;
        } // if reply.ok
      } // while wait_for_reply
     
      if (cc == 0) {  // If something times out print *
        printf (" *");
        if (verbose)
          reply.error.warn(" *");
        else
          reply.error.reset();
      }
      fflush (stdout);
    }
    putchar ('\n');
    /*if (policy_flag && verbose && whois.error())
	   whois.error.warn ("prtraceroute:");*/

    if (got_there || unreachable >= nprobes - 1)
      break;
  }
  
  if (policy_flag)
    path.process_policies();
  exit (0);
}



////////////////////////////////////////////////////////////////////////
void
usage(char *argv[]) {
    cerr << "\nUsage: " << argv[0] << " [options] host [data size]" << endl;
    cerr << "Options: [-dnrv] [-np] [-w wait] [-m max_ttl] [-p port#] [-t tos]" << endl;
    cerr << "         [-s src_addr] [-q nqueries] [-g gateway]" << endl;
    cerr << "         [-irr_server host] [-irr_port port#] [-irr_databases dbs]\n" << endl;
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
// int gFlag(char *dst, char *key, char *nextArg)
//
//
//--------------------------------------------------------------
int gFlag(char *dst, char *key, char *nextArg) {
   int limit = ((MAX_IPOPTLEN - IPOPT_MINOFF) / sizeof (u_long));
   if ((lsrr+1) >= limit) {
     fprintf (stderr, "No more than %d gateways\n",
	      (u_int) limit - 1);
     exit (1);
   }
   source_routes[lsrr++] = new ipAddr(nextArg);  
   return 1;
}

//--------------------------------------------------------------
// int mFlag(char *dst, char *key, char *nextArg)
//
//
//--------------------------------------------------------------
int mFlag(char *dst, char *key, char *nextArg) {
   if (nextArg) max_ttl = atoi(nextArg);
   if (max_ttl < 1) {
      fprintf (stderr, "TTL must be greater than one.");
      exit (1);
   }
   return 1;
}

//------------------------------------------------------------
// int sFlag(char *dst, char *key, char *nextArg)
//
//
//------------------------------------------------------------
int sFlag(char *dst, char *key, char *nextArg) {
   if (nextArg) sourceif = nextArg;
   fprintf (stderr, "Sorry '-s source' not supported yet'\n");
   return 1;
}

//------------------------------------------------------------
// void
// init_and_set_options (int argc, char **argv, char **envp)
//
//------------------------------------------------------------
void
init_and_set_options (int argc, char **argv, char **envp) {
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
//      {"-prompt", ARGV_STRING,  (char *) NULL,        (char *) &opt_prompt,
 //      "Prompt"},

      IRR_COMMAND_LINE_OPTIONS,

      // prtraceroute specific arguments
      {"-g", ARGV_FUNC, (char *)gFlag, (char *)NULL,
       "Gateway"},
      {"-m", ARGV_FUNC, (char *)mFlag, (char *)NULL,
       "Max TTL"},
      {"-n", ARGV_CONSTANT, (char *)1, (char *)&nflag,
       "N flag"},
      {"-np", ARGV_CONSTANT, (char *)0, (char *)&policy_flag,
       "No policy"},
      {"-p", ARGV_INT, (char *)NULL, (char *)&port,
       "Probe port"},
      {"-q", ARGV_INT, (char *)NULL, (char *)&nprobes,
       "Query"},
      {"-s", ARGV_FUNC, (char *)sFlag, (char *)NULL,
       "'-s source' not supported yet"},
      {"-t", ARGV_INT, (char *)NULL, (char *)&tos,
       "Type of service"},
      {"-v", ARGV_CONSTANT, (char *)1, (char *)&verbose,
       "Verbose"},
      {"-w", ARGV_INT, (char *)NULL, (char *)&waittime,
       "Wait time"},

      {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL, (char *) NULL}
   };


   IRR::handleEnvironmentVariables(envp);
   
   IRR::IgnoreErrors(NULL, NULL, NULL);


   if (ParseArgv(&argc, argv, argTable, 0) != ARGV_OK) {
      cerr << endl;
      exit(1);
   }
   
     // Setup Connection to IRR server
   if (! (irr = IRR::newClient())) {
      cerr << "Unknown protocol!" << endl;
      exit(1);
   }

   switch (argc) {
     case 3:
       if (*argv[--argc] == '-') {
	   fprintf (stderr, "unknown option: %s\n", argv[argc]);
	   usage (argv);
       }
       for (char *c = argv[argc]; *c; c++)
	   if (!isdigit(*c)) {
	       fprintf (stderr, "bad packet len: %s\n", argv[argc]);
	       break;
	   }
       datalen += atoi (argv[argc]);
     case 2:
       if (*argv[--argc] == '-') {
	   fprintf (stderr, "unknown option: %s\n", argv[argc-1]);
	   usage (argv);
       }
       dst = new char[strlen (argv[argc])+1];
       strcpy (dst, argv[argc]);
       break;			// just ignore all extra
     default:
       usage (argv);
       break;
   } 
}

////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------
// 
//ttlProbe::ttlProbe(char *dst, int sport, int dport,
//		               int lsrr, ipAddr **source_routes) 
//
//------------------------------------------------------------
ttlProbe::ttlProbe(char *dst, int sport, int dport,
		   int lsrr, ipAddr **source_routes) :
    rawUDP (dst)
{
    int on = 1;
    if (error()) {
	error.syserr (Yes);
	error.die ("ttlProbe: ");
    }

    if (lsrr && source_routes) {
	addLSRR (lsrr, source_routes);
	error.die ("ttlProbe: ");
    }

#ifdef IP_HDRINCL    
    setsockopt (IPPROTO_IP, IP_HDRINCL, (char *) &on, sizeof (on));
#endif // IP_HDRINCL
    init_udp (sport, dport);
    if (error())
	error.die ("ttlProbe: ");
    ident = (getpid() & 0xffff) | 0x8000;
    ip_hdr->ip_id = ntohs(ident);
    probepkt = (probe_pkt *) udp_data;
    size = sizeof(probe_pkt);

    // set sizes
    udp_hdr->uh_ulen += htons(size);	// sets the udp len
    ip_hdr->ip_len += size;	// maps to the ip len
}

//------------------------------------------------------------
// int
// ttlProbe::send (u_char seq, u_char ttl)
//
//------------------------------------------------------------
int
ttlProbe::send (u_char seq, u_char ttl)
{
    if (error())
	return error.warning("ttlProbe: ");
    struct timezone tz;
    probepkt->ttl = ip_hdr->ip_ttl = ttl;
    probepkt->seq = seq;
    gettimeofday (&(probepkt->tv), &tz);
    udp_hdr->uh_dport = htons(port + seq);
    udp_hdr->uh_sport = ip_hdr->ip_id;
    ip_hdr->ip_p = IPPROTO_UDP;
    return sendto ();
}

//////////////////////////////////////////////////////////////

//------------------------------------------------------------
// int
// ICMPProbeReply::ok(int size, int ident, int seq)
//
//------------------------------------------------------------
int
ICMPProbeReply::ok(int size, int ident, int seq)
{
#ifdef STRUCT_IP_USES_VHL
    int hlen = (ip_hdr->ip_vhl & 0xF) << 2;
#else // STRUCT_IP_USES_VHL
    int hlen = ip_hdr->ip_hl << 2;
#endif // STRUCT_IP_USES_VHL
    if (size < hlen + ICMP_MINLEN) {
	if (verbose)
	    printf ("Packet too short (%d bytes) from %s\n",
		    size, inet_ntoa (ip_hdr->ip_src));
	return 0;
    }

    size -= hlen;

    int type = icmp_hdr->icmp_type;
    int code = icmp_hdr->icmp_code;

    struct ip *hip = (struct ip *) NULL;
    if ((type == ICMP_TIMXCEED && code == ICMP_TIMXCEED_INTRANS) ||
	type == ICMP_UNREACH) {
	
	struct ip *hip = &icmp_hdr->icmp_ip;
#ifdef STRUCT_IP_USES_VHL
	hlen = (hip->ip_vhl & 0xF) << 2;
#else // STRUCT_IP_USES_VHL
	hlen = hip->ip_hl << 2;
#endif // STRUCT_IP_USES_VHL
	
	struct udphdr *up = (struct udphdr *) ((u_char *) hip + hlen);
	if (hlen + 12 <= size &&
	    hip->ip_p == IPPROTO_UDP &&
	    up->uh_sport == htons(ident) &&
	    up->uh_dport == htons (port+seq)) {
	    return (type == ICMP_TIMXCEED ? -1 : code + 1);
	}
    }
	
    if (verbose) {
	u_long *lp = (u_long *) &icmp_hdr->icmp_ip;
	struct in_addr in;
	in.s_addr = get_srcInaddr();

	printf ("\n%d bytes from %s to %s", size,
		in, inet_ntoa (ip_hdr->ip_dst));
	
	printf (": icmp type %d (%s) code %d (%s)\n",
		type, icmp_type_string(),
		code, icmp_code_string());
    }
    return (0);
}

////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------
// void 
// Hop::_hop ()  common intitializer for Hop class to zero 
//   everything out first
//
//------------------------------------------------------------
void 
Hop::_hop () {
  ipaddr = (ipAddr *) NULL;
  as = 0;
	   //route = new Route();
  autnum = (AutNum *) NULL;
}
//------------------------------------------------------------
// void
// Hop::_add (ipAddr *ip)
//  Fills in all the Hop info from a given ip
//------------------------------------------------------------
void Hop::_add (ipAddr *ip) {
  if (ip) {
    ipaddr = ip;
    as = irr->getOrigin("%s/32",ip->getIpaddr());
  }  else {
    ipaddr = NULL;
    as = 0;
  }
    
/*
    if (ip == (ipAddr *) NULL)
	return; 
    ipaddr = ip;
    rtpix  = Prefask_map.add_entry(ipaddr->getIpaddrMask());
    route->nlri.pix = rtpix;
    route->define_less_or_equal_specific();
    ListNodePix *p;
    char *asptr = (char *) NULL;
    for (p = route->origin.head(); p; p = route->origin.next(p->l)) {
	char *ptr = (char *) AS_map (p->pix);
	// the following checks can be removed when we put
	// in Jerry's corrected radbserver.
	if (ptr != (char *) NULL) { 
	    if (!strcmp (ptr, "AS0") || !strcmp (ptr, "AS1800")) 
		continue;
	    else {
		asname = asptr = ptr;
		ipaddr->setAsname (ptr);
	    }
	}
    }
    if (!asptr)
	return;
    aspix  = AS_map.add_entry(asptr);
    autnum = AS_map.define(aspix);
*/
}


////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------
// void
// Path::addHop(ipAddr *ip, int i)
//
//------------------------------------------------------------
void
Path::addHop(ipAddr *ip, int i) {
  if (i > last_ttl)
    last_ttl = i;
    
  hops[i] = new Hop(ip);
}

//------------------------------------------------------------
// ASt 
// Path::getHopAS(int ttl)
//
//------------------------------------------------------------
ASt 
Path::getHopAS(int ttl) {
  if (ttl <= last_ttl && hops[ttl])
    return hops[ttl]->getAS();
  else
    return 0;
}


//------------------------------------------------------------
// void
// Path::process_policies()
//
//------------------------------------------------------------
void
Path::process_policies()  {
  int i;
  ASt export_as, import_as;
  BGPRoute bgproute(getHopAS(0),destination.getAS());
  const AutNum * autnum;
  
  char test[80];
  
  actiondictionary = new ActionDictionary;

  // just print out the path taken, as gathered while the trace 
  // was running
  cout << endl << "Path taken: ";
  
  if (!hops[0]->getAS()) {  // Empty path
    cout << endl << "(???)";
  } else {
    cout << endl << "AS" << hops[0]->getAS() << " "; 
    for (i = 1; i <= last_ttl; i++) {
      if (hops[i]->getAS() && hops[i-1]->getAS()) {
        if (getHopAS(i) != getHopAS(i-1))
          cout << "AS" << hops[i]->getAS() << " ";
      } else if (hops[i]->getAS()) {
          cout << "AS" << hops[i]->getAS() << " ";
      } else {
        cout << "(???) ";
      }
    } // end for
  }
  cout << endl << endl;
  
  //--- Policy Analysis of the Path Taken

  //--- analyze from destination to source
  if (getHopAS(last_ttl) == destination.getAS()) 
    strcpy (imports[last_ttl], "destination");
  else
	strcpy (imports[last_ttl], "!reached");
   
  for (i = last_ttl; i > 0; i--) {
    export_as  = getHopAS(i);     // hops[i]->getHopAS();
    import_as  = getHopAS(i-1);   // hops[i-1]->getHopAS();

    // Check to see if we have changed to different AS
    if (export_as == import_as) {
      if (import_as) {
        strcpy(exports[i], "internal");
        strcpy(imports[i-1], "internal");
      } else {
        strcpy (exports[i],  "!registered");
        strcpy (imports[i-1], "!registered");
      }
	    // continue;
    } else {  // Check for export policies to new AS
      if (export_as) {
        bgproute.add(export_as);
        autnum = irr->getAutNum(export_as);
        if (autnum) {
          if (import_as) {
            AutNumSelector<AttrExport> export_itr(autnum, "export", NULL,
                                                  import_as, NULL, NULL);
            
            if (export_itr.first())
              strcpy(exports[i], "export");
            else // No Exports for Peer AS 
              strcpy(exports[i], "!export");
          } else // No Peer AS given
            strcpy(exports[i], "peer unregistered");
        } else // No Autnum found
          strcpy(exports[i], "!registered");
      } else // No export_as given 
        strcpy(exports[i], "!registered");
        
      // Check for import policies of new as to previous as
      
      if (import_as) {
        /* irr is global variable */
        const AutNum * import_autnum = irr->getAutNum(import_as);
        /* Get All Imports */
        if (import_autnum) {
          import_pref_list.clear();
	  AutNumPeeringIterator itr(import_autnum);
          for (const Peering *peering = itr.first(); 
	       peering; 
	       peering = itr.next()) {
             // Check for Match & Build preference list
	     findImportPreference(bgproute, import_as, import_autnum, 
				  peering->peerAS, &(peering->peerIP), 
				  &(peering->localIP));
          } // End for
          
          /* Based upon preference list write out the preference */
          int import_pref = 1;
          for (ASPref * listpref = import_pref_list.head(); listpref;
                        listpref = import_pref_list.next(listpref)) {
            if (listpref->as != export_as)
              import_pref++;
          }
          sprintf(imports[i-1], "import: %d",import_pref);
        } else { 
          strcpy(imports [i-1], "!registered");
        }
      } else
        strcpy(imports [i-1], "!registered");
    }
  }
  
  strcpy (exports[0], "source");

  // now print the proceessed policies
  for (i = last_ttl; i >= 0; i--) {
    if (hops[i] != NULL) {
      //cout << i << "   AS" << getHopAS(i) << " ";
      printf ("%3d  AS%d", i, getHopAS(i));
      if (nflag)		// XXX, this is a global, and is bad!!!
        //cout << hops[i]->ipaddr->getIpaddr();
        printf (" %-35s", hops[i]->ipaddr->getIpaddr());
      else
       // cout << "Ip Address Name  ";
        //cout << hops[i]->ipaddr->getName();
        printf (" %-35s", hops[i]->ipaddr->getName());
      
      //cout << imports[i] << " -> " << exports[i] << endl;  
        printf (" %10s -> %s \n", imports[i], exports[i]);
    } else {
      //cout << i << "   (NULL) (???)" << endl;
      printf ("%3d  %s", i, "(NULL)");
      printf (" (???)\n");
    }
  }
}


void
Path::findImportPreference(BGPRoute &bgproute, ASt as, const AutNum * as_autnum,
                           ASt peer_as, const IPAddr * peer_ip, const IPAddr * local_ip) {
                           
  AutNumSelector<AttrImport> import_itr(as_autnum, "import", NULL,
					peer_as, peer_ip, local_ip);
  if (const FilterAction * fa = import_itr.first()) {
    for (; fa; fa = import_itr.next()) {
      if (bgproute.isMatching(as, fa->filter, irr)) {
        bgproute.executeAction(fa->action, actiondictionary);
        // Insert AS & its preference into the sorted preference list
        ASPref * aspref = new ASPref(as, bgproute.pref.pref);
        if (!import_pref_list.isEmpty()) {
          for (ASPref * listpref = import_pref_list.head(); listpref;
                        listpref = import_pref_list.next(listpref)) {
            if (listpref->pref > aspref->pref) {
              import_pref_list.insertBefore(listpref, aspref);
              break;
            }           
          }
        } else {
          import_pref_list.append(aspref);
        }
      }
    }
  }
}

