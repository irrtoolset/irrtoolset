/*
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
//  Author(s): eddy@isi.edu
*/


#ifndef __NETRAW_HH__
#define __NETRAW_HH__

#include "config.h"
#include "util/net.hh"

#ifndef MAX_IPOPTLEN
#define MAX_IPOPTLEN    40
#endif /* MAX_IPOPTLEN */

extern "C" {
#ifdef __linux__
#include "linux-compat.h"
#else
#include <netinet/ip_icmp.h>
#endif // __linux__
#include "ifaddrlist.h"  
}



////////////////////////////////////////////////////////////////////////
// in the raw packet classes all reads and write will use implicit
// buffering and i/o provided by the base packet.
// XXX - the concept of size is broken, needs fixing, in other words
// how should the following headers increase ip_len, and how should
// rawPacket know about ip_len?
////////////////////////////////////////////////////////////////////////

class rawPacket : public Socket {
    char buffer[MAXPACKETLEN];

  protected:
    char *packet_data;		// data pointer for this packet
    int  packet_size;		// size of this packet

  public:
    rawPacket (int protocol = IPPROTO_RAW) :
	Socket (PF_INET, SOCK_RAW, protocol) {
	if (error())
	    error.fatal("rawPacket: ");
	bzero ((char *) &buffer, sizeof (buffer));
	packet_data = buffer;
	packet_size = 0;
    }

    int write (char *p, int l)    { return Socket::write  (p, l); }
    int send (char *p, int l)     { return Socket::send  (p, l); }
    int sendto (char *p, int l)   { return Socket::sendto (p, l); }
    int recvfrom (char *p, int l) { return Socket::recvfrom (p, l); }
    
    int wait_for_reply (int timeout = 0) {
	return (packet_size =
		Socket::wait_for_reply ((char *) &buffer, MAXPACKETLEN, timeout));
    }
    void print_hex () {
	// XXX: this is broke
	for (int i = packet_size; i > 0; i - 16) { 
	    if (i % 16)
		printf ("\n");
	    else if (i % 8)
		printf (" ");
	    printf ("%4x", *packet_data);
	} 
    }
    virtual void print () { print_hex(); }
};

////////////////////////////////////////////////////////////////////////
class rawIP : public rawPacket {
    ipAddr *src_ipaddr;	
    ipAddr *dst_ipaddr;

  protected:
    struct ip *ip_hdr;		// pointer to the ip header
    void *ip_data;		// pointer to the data immediately following ip
    char *ip_opts;		// pointer to options if they exist
    int optlen;
    int optflags;
    int  ip_size;

  public:
    rawIP (int protocol = IPPROTO_RAW);
    rawIP (char *hostname, int protocol = IPPROTO_RAW);

    void setIpaddresses ();		// can be used for incoming packets.

    int write (){return rawPacket::write ((char *) ip_hdr, ip_hdr->ip_len);}
    int send (){return rawPacket::send ((char *) ip_hdr, ip_hdr->ip_len);}
    int sendto (){return rawPacket::sendto ((char *) ip_hdr, ip_hdr->ip_len);}

    int wait_for_reply (int timeout);

    ipAddr *srcIpaddr () { return src_ipaddr; }
    ipAddr *dstIpaddr () { return dst_ipaddr; }

    u_long srcInaddr () { return ip_hdr->ip_src.s_addr; }
    u_long dstInaddr () { return ip_hdr->ip_dst.s_addr; }

    int getTTL () { return ip_hdr->ip_ttl; }

    int addLSRR (int nroutes, ipAddr **source_routes);

    void print();
    void print_hex () { rawPacket::print(); }
};

////////////////////////////////////////////////////////////////////////
class rawUDP : public rawIP {
  protected:
    void init_udp (int sport, int dport);
    struct udphdr *udp_hdr;
    void *udp_data;
    int udp_size;

  public:
    rawUDP (char *dst) : rawIP(dst) {}
    rawUDP (int sport, int dport)  { init_udp (sport, dport); }
    rawUDP (char *dst, int sport, int dport) : rawIP (dst) {
	if (error()) return;
	init_udp(sport, dport);
	bind();
    }


    udphdr *get_udp ()     { return udp_hdr; }
    void   *get_udpdata () { return udp_data; }
    void   print ();
};

////////////////////////////////////////////////////////////////////////
class ICMP : public rawIP {
  protected:
    struct icmp *icmp_hdr;

  protected:
    void *icmp_dat;		// name collision with icmp_data
    u_int icmp_size;
  
  public:
    ICMP ();
    int type () { return icmp_hdr->icmp_type; }
    int code () { return icmp_hdr->icmp_code; }
    void icmp_print ();
    char *icmp_type_string ();
    char *icmp_code_string ();
};

#endif  // __NETRAW_HH__
