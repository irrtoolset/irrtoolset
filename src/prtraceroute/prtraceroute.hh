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
