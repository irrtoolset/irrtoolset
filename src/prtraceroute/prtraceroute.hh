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
//  Author(s): Rusty Eddy <eddy@isi.edu>
//

#ifndef _PRTRACEROUTE_H_
#define _PRTRACEROUTE_H_

#include "config.h"
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>


#include <iostream.h>
#include <iomanip.h>
//#include "dbase.hh"
//#include "whois.hh"
//#include "Node.h"
//#include "debug.hh"
//#include "trace.hh"
//#include "rusage.hh"
//#include "aut-num.hh"
//#include "Route.hh"
#include "irr/irr.hh"
#include "bgp/bgproute.hh"


extern "C" {
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif

#include <sys/param.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>

#ifndef HAVE_DECL_GETTIMEOFDAY
extern int gettimeofday(...);    
#endif // HAVE_DECL_GETTIMEOFDAY
}

#ifndef BYTE_ORDER
#ifdef  WORDS_BIGENDIAN
#define BYTE_ORDER 1234
#else
#define BYTE_ORDER 4321
#endif  // WORDS_BIGENDIAN
#endif  // BYTE_ORDER

#include "netraw.hh"

struct probe_pkt {
    u_char seq;
    u_char ttl;
    struct timeval tv;
};

class ttlProbe : public rawUDP {
    int ident;
    int size;
    probe_pkt *probepkt; 

  public:
    ttlProbe(char *dst, int sport, int dport,
	     int lsrr = 0, ipAddr **source_routes = NULL);

    int send (u_char, u_char);
    int get_ident() { return ident; }
};

class ICMPProbeReply : public ICMP {
  public:
    int ok(int, int, int);
    void print () { icmp_print(); }
};

// for flags

class Hop {
private:
    //Route  *route;
  const AutNum * autnum;
  ASt as;

    //Pix    aspix;
    //Pix    rtpix;
  u_int  flags;

  void _hop ();
    
  void _add(ipAddr *ipaddr);

public:
  ipAddr *ipaddr;

  Hop () { _hop (); }
  Hop (ipAddr *ipaddr) { _hop(); _add (ipaddr); }
  Hop (ipAddr &ipaddr) { _hop(); _add (&ipaddr); }
  void add(ipAddr * ip){ _add(ip); }
  ASt getAS() { return as; }
};

#define POLICYLEN 30


class ASPref : public ListNode {
public:
  ASt as;
  int pref;
  
  ASPref(ASt new_as, int new_pref) {
    as = new_as;
    pref = new_pref;
  }
};
  

class Path {
public:
  //Route route;
  Hop * hops[MAXTTL];
  Hop destination;
  int last_ttl;
  
  List<ASPref> import_pref_list;

  char exports[MAXTTL][POLICYLEN];
  char imports [MAXTTL][POLICYLEN];
  //char asout[MAXTTL][POLICYLEN];
  //char asin [MAXTTL][POLICYLEN];

  // holds the relation between the current and previous hops
  // i.e. relation[1] holds the peering according to hop0 and hop1
  Path() {
    for (int i = 0; i < MAXTTL; i++) {
      last_ttl = -1;
      hops[i] = (Hop *) NULL;
      // bzero ((char *) asin[i],  POLICYLEN);
      //  bzero ((char *) asout[i], POLICYLEN);
    }
  }
  
  void addHop(ipAddr *ip, int ttl); // rely on the user to insert correct ttls
  void addDestination (ipAddr *ip) { destination.add (ip); }
  ASt getHopAS(int ttl);
  void process_policies();
  void findImportPreference(BGPRoute &bgproute, ASt as, const AutNum * as_autnum,
                           ASt peer_as, const IPAddr * peer_ip, const IPAddr * local_ip);
};




#endif // _PRTRACEROUTE_H_
