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

#include "config.h"
#include "netraw.hh"

////////////////////////////////////////////////////////////////////////
rawIP::rawIP (char *remote, int protocol) : rawPacket ()
{
    extern char *progname;
    struct ifaddrlist *al;
    char errbuf[132];		// cause traceroute.c uses 132...
    int n;

    if (error())
	return;

    ip_hdr = (struct ip *) packet_data;

    /* Get the interface address list */
    n = ifaddrlist(&al, errbuf);
    if (n < 0) {
	::fprintf(stderr, "%s: ifaddrlist: %s\n", progname, errbuf);
	exit(1);
    }
    if (n == 0) {
	::fprintf(stderr,
		  "%s: Can't find any network interfaces\n", progname);
	exit(1);
    }

    // this error handling bites.
    src_ipaddr = new ipAddr (al->addr);
    if (src_ipaddr->error()) {
	error = src_ipaddr->error; // XXX: test this
	return;
    }
    
    dst_ipaddr = new ipAddr (remote);
    if (dst_ipaddr->error()) {
	error = dst_ipaddr->error; // XXX: test this
	return;
    }

    struct in_addr in;
#ifdef STRUCT_IP_USES_VHL
    ip_hdr->ip_vhl    = (4 << 4) + 5;
#else // STRUCT_IP_USES_VHL
    ip_hdr->ip_v    = 4;
    ip_hdr->ip_hl   = 5;	
#endif // STRUCT_IP_USES_VHL
    ip_hdr->ip_len  = 20;
    ip_hdr->ip_tos  = 0;
    ip_hdr->ip_id   = 0;
    ip_hdr->ip_off  = 0;
    ip_hdr->ip_ttl  = MAXTTL;
    ip_hdr->ip_p    = protocol;
    ip_hdr->ip_sum  = 0;
    ip_hdr->ip_src.s_addr = src_ipaddr->getInaddr();
    ip_hdr->ip_dst.s_addr = dst_ipaddr->getInaddr();
    set_srcInaddr (src_ipaddr->getInaddr());
    set_dstInaddr (dst_ipaddr->getInaddr());
    ip_size = sizeof (struct ip);
    packet_size += ip_size;
    ip_data = ip_hdr + 1;
    ip_opts = NULL;
    optflags = 0;
    optlen = 0;
}

rawIP::rawIP (int protocol) : rawPacket (protocol)
{
    ip_hdr = (struct ip *) packet_data;
    ip_data = ip_hdr + 1;
    ip_opts = NULL;		// remain NULL unless they are used
    ip_size = sizeof (struct ip);
    packet_size += ip_size;
}

// when adding LSRR, the first node of the source_routes goes
// into the IP dst field and the dst goes into the last position
// of the source route list.  also the dst of the Socket class
// must be changed to the first hop of the source list.
// NOTE: i stole this from VJ's traceroute.
int 
rawIP::addLSRR (int nroutes, ipAddr **source_routes)
{
    char ipopts[MAX_IPOPTLEN], *ptr = (char *) &ipopts;
    int size = sizeof (u_long);
    optlen = IPOPT_MINOFF-1 + ((nroutes + 1) * size);

    bzero ((char *) &ipopts, sizeof (ipopts));
    optflags |= IPOPT_LSRR;
    *ptr++ = IPOPT_LSRR;
    *ptr++ = optlen;
    *ptr++ = IPOPT_MINOFF;	// pointer to LSRR addresses

    u_long hex = (u_long) source_routes[0]->getInaddr();
    bcopy ((char *) &hex, ptr, size);
    for (int i = 1; i < nroutes; i++, ptr += size) {
	hex = source_routes[i]->getInaddr();
	bcopy ((char *) &hex, ptr, size);
    }

    ptr += size;
    hex = dst_ipaddr->getInaddr();
    bcopy ((char *) &hex, ptr, size);

    optlen += 4 - (optlen % 4);

    while ((ptr - ipopts) & 3) ptr++; // pad to even boundry
    packet_size = ip_hdr->ip_len;
    setsockopt (IPPROTO_IP, IP_OPTIONS, (char *) &ipopts, optlen);
    return error();
}

// XXX add error
int
rawIP::wait_for_reply (int timeout = 0)
{
    int cc = rawPacket::wait_for_reply (timeout);
    if (!error()) {
	src_ipaddr = new ipAddr (ip_hdr->ip_src.s_addr);
	dst_ipaddr = new ipAddr (ip_hdr->ip_dst.s_addr);
    }
    return cc;
}

#define  phdr(ppp, fld, val) printf ("%10s - %s: 0x%x\n", ppp, fld, val)
#define phdrd(ppp, fld, val) printf ("%10s - %s: 0x%x == %d\n", ppp, fld, val, val)

void
rawIP::print ()
{
#define pip(fld, val)  phdr("IP", fld, val)
#define pipd(fld,val) phdrd("IP", fld, val)
#ifdef STRUCT_IP_USES_VHL
    pip ("Version", ip_hdr->ip_vhl & 0xF0);
    pip ("Hdr Len", ip_hdr->ip_vhl & 0xF);
#else // STRUCT_IP_USES_VHL
    pip ("Version", ip_hdr->ip_v);
    pip ("Hdr Len", ip_hdr->ip_hl);
#endif // STRUCT_IP_USES_VHL
    pipd("Length",  ip_hdr->ip_len);
    pipd("TOS",     ip_hdr->ip_tos);
    pip ("ID",      ip_hdr->ip_id);
    pip ("Offset",  ip_hdr->ip_off);
    pipd("TTL",     ip_hdr->ip_ttl);
    pipd("Proto",   ip_hdr->ip_p);
    pip ("Cksum",   ip_hdr->ip_sum);
    pip ("Source",  ip_hdr->ip_src.s_addr);
    pip ("Dest",    ip_hdr->ip_dst.s_addr);
}

////////////////////////////////////////////////////////////////////////
void
rawUDP::init_udp (int sport = -1, int dport = -1)
{
    if (error())
	return;
    udp_hdr = (struct udphdr *) ip_data;
    udp_size = sizeof (struct udphdr);
    udp_data = udp_hdr + 1;
    packet_size += udp_size;
    
    udp_hdr->uh_sport = htons(sport);
    udp_hdr->uh_dport = htons(dport);
    udp_hdr->uh_ulen =  htons(sizeof (struct udphdr));
    udp_hdr->uh_sum  = 0;

    // now retrofit some ip stuff, not great oo practice, but hey,
    // were nethacks!!! B-)
    ip_hdr->ip_p = IPPROTO_UDP;
    ip_hdr->ip_len += udp_size;
    packet_size = ip_hdr->ip_len;
}
	
void
rawUDP::print ()
{
#define pudp(fld, val)  phdr ("UDP", fld, val)
#define pudpd(fld, val) phdrd("UDP", fld, val)
    
    rawIP::print();
    pudpd("Source port", udp_hdr->uh_sport);
    pudpd("Dest   port", udp_hdr->uh_dport);
    pudpd("Len", udp_hdr->uh_ulen);
    pudp ("Cksum", udp_hdr->uh_sum);
}

    
////////////////////////////////////////////////////////////////////////
ICMP::ICMP () : rawIP (IPPROTO_ICMP)
{
    if (error())
	return;
    icmp_hdr = (struct icmp *) ip_data;
    icmp_dat = ip_hdr + 1;
    icmp_size = 0;
}

void
ICMP::icmp_print ()
{
#define  picmp(fld, val)  phdr("ICMP", fld, val);
#define picmpd(fld, val) phdrd("ICMP", fld, val);
    //rawIP::print();
    picmpd ("Type", icmp_hdr->icmp_type);
    picmpd ("Code", icmp_hdr->icmp_code);
    picmp  ("Cksum", icmp_hdr->icmp_cksum);

    // now do packet specific stuff
}

char *
ICMP::icmp_type_string ()
{
    char *str = "???";
    switch (icmp_hdr->icmp_type) {
      case ICMP_ECHOREPLY:
	return (str = "Echo Reply");
      case ICMP_UNREACH:
	return (str = "Destination unreachable");
      case ICMP_SOURCEQUENCH:
	return (str = "Source quench");
      case ICMP_REDIRECT:
	return (str = "Redirect");
      case ICMP_ECHO:
	return (str = "Echo");
      case ICMP_TIMXCEED:
	return (str = "Time exceeded");
      case ICMP_PARAMPROB:
	return (str = "Bad IP header");
      case ICMP_TSTAMP:
	return (str = "Time stamp request");
      case ICMP_TSTAMPREPLY:
	return (str = "Time stamp reply");
      case ICMP_IREQ:
	return (str = "Information Request");
      case ICMP_IREQREPLY:
	return (str = "Information Reply");
      case ICMP_MASKREQ:
	return (str = "Mask request");
      case ICMP_MASKREPLY:
	return (str = "Mask reply");
      default:
	return (str = "Unknown type");
    }
    return str;
}

char *
ICMP::icmp_code_string ()
{
    return ("not yet");
}

