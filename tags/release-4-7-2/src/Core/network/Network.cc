//
// $Id$
//
// Author(s): Ramesh Govindan

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cerrno>

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#if HOST_OS_IS_SOLARIS
#include <sys/sockio.h>
#endif
};

#include "util/Types.hh"
#include "util/Trail.hh"
#include "util/Handler.hh"
#include "util/Buffer.hh"
#include "sys/File.hh"
#include "sys/Pipe.hh"
#include "sched/Job.hh"
#include "sched/Dispatcher.hh"

#include "network/Ping.hh"
#include "network/Prefix.hh"
#include "network/Mtrace.hh"
#include "network/Network.hh"
#include "network/Headers.hh"

// Constants
static const int	MaxBufferSize = 1024;
static const int	MaxInterfaces = 32;

// File local
static TraceCode	traceNetwork("network");

// Globals
Network*	network = NULL;

static void
netReadHandler(void* ptr,
                void* arg)
{
    ((Network*) ptr)->receive((RawSocket*) arg);
}

Network::Interface::Interface(char* ifname,
                              U32 ifaddr)
	: ListNode()
{
    name = strdup(ifname);
    address.set(ifaddr);
    TRACE(traceNetwork, "adding interface %s:%s\n", name, address.name());
}

Network::Interface::~Interface()
{
    TRACE(traceNetwork, "deleting interface %s:%s\n", name, address.name());
    free(name);
}

Network::Network()
{
    Handler		rh(netReadHandler, this);
    Handler		nh(NULL, NULL);
    struct sockaddr_in	sin;
    int			socklen = sizeof(sin);
    int 		sock;
    Network::Interface*	interface;
    Address		group;

    icmpSocket = new RawSocket(rh, nh, RawSocketICMP);
    igmpSocket = new RawSocket(rh, nh, RawSocketIGMP);

    group.set(MtraceMulticast);

    U8 loop= 0;
    if(igmpSocket->setopt(IPPROTO_IP, IP_MULTICAST_LOOP,
			  &loop, sizeof(loop)) < 0) {
  	    TRACE(traceNetwork, "setsockopt IP_MULTICAST_LOOP\n");
    }

    igmpSocket->join(group);
    recvBuffer = new Buffer(MaxBufferSize);

    // First find all interfaces
    if ((sock = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        FATAL("couldn't open socket for getting interfaces: %s\n",
              strerror(errno));
        // NotReached
    }

    struct ifconf	ifc;
    int                 buf_len= sizeof(struct ifreq)*MaxInterfaces;
    char                buf[buf_len];
    struct ifreq	*ifr;
    ifc.ifc_len = buf_len;
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, (char*) &ifc) < 0) {
        FATAL("failed ioctl for getting interface list: %s\n",
              strerror(errno));
        // NotReached
    }
#ifndef SA_LEN
#define SA_LEN(_x) sizeof(*(_x))
#endif
    for (char *cp= buf, *cplim = buf + ifc.ifc_len;
	 cp < cplim; 
	 cp+= sizeof(ifr->ifr_name)+SA_LEN(&ifr->ifr_addr)) {
	    ifr= (struct ifreq *)cp;
	    U32 if_addr= ((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr.s_addr;
	    if (ifr->ifr_addr.sa_family != AF_INET || if_addr == 0)
		    continue;
	    if_addr = ntohl(if_addr);
	    if (if_addr != INADDR_LOOPBACK) {
		    interface = new Network::Interface(ifr->ifr_name, if_addr);
		    interfaces.append(interface);
	    }
    }
    close(sock);

    if (interfaces.isEmpty()) {
        FATAL("couldn't find any usable interfaces\n");
        // NotReached
    }

    tprobesSent = 0;
    tprobesTimedOut = 0;
    icmprobesSent = 0;
    icmprobesTimedOut = 0;
    
    return;
}

Network::~Network()
{
    delete icmpSocket;
    delete igmpSocket;
    delete recvBuffer;
    interfaces.clear();
    pendingPings.clear();
    pendingTProbes.clear();
    pendingICMProbes.clear();
}

void
Network::dump(FILE* fp)
{
    fprintf(fp, "tprobes sent %Lu timed-out %Lu\n", 
            tprobesSent, tprobesTimedOut);
    fprintf(fp, "icmprobes sent %Lu timed-out %Lu\n", 
            icmprobesSent, icmprobesTimedOut);
    return;
}

void
Network::receive(RawSocket* socket)
{
    int		length;
    IP*		ip;
    ICMP*	icmp;
    IGMP*	igmp;
    U16		sum;

    length = socket->read(recvBuffer->contents, recvBuffer->capacity);
    if (length < 0) {
        return;
    }
    ip = (IP*) recvBuffer->contents;
    ip->ntoh();
#if HOST_OS_IS_FREEBSD || HOST_OS_IS_SOLARIS
    ip->totalLength+= (ip->headerLength << 2);
#endif
    if (ip->totalLength != length) {
        return;
    }

    if (length < (ip->headerLength << 2)) {
        return;
    }
    length -= ip->headerLength << 2;

    switch (ip->protocol) {
        case IPPROTO_ICMP: 
        {
            // Sanity check ICMP packet
            if (length < sizeof(ICMP)) {
                return;
            }
    
            icmp = (ICMP*) ((char*) ip + (ip->headerLength << 2));
            sum = icmp->checksum;
            icmp->checksum = 0;
            if (sum && (sum != network->cksum((U16*) icmp, length))) {
                return;
            }
            icmp->ntoh();

            // Save a Backup since receive functions like to
            // reverse the address bytes for big vs little endian machines
            IP * ipBackup = new IP;
            memcpy(ipBackup,ip,sizeof(struct IP));

            // Farm out packet to whoever's waiting
            for (Ping* ping = pendingPings.head(); ping != NULL;
                 ping = pendingPings.next(ping)) {
                memcpy(ip,ipBackup,sizeof(struct IP));
                if (ping->receive(ip)) {
                    break;
                }
            }
            for (TProbe* tp = pendingTProbes.head(); tp != NULL;
                 tp = pendingTProbes.next(tp)) {
                memcpy(ip,ipBackup,sizeof(struct IP));
                if (tp->receive(ip)) {
                    break;
                }
            }
            for (ICMProbe* tp = pendingICMProbes.head(); tp != NULL;
                 tp = pendingICMProbes.next(tp)) {
                memcpy(ip,ipBackup,sizeof(struct IP));
                if (tp->receive(ip)) {
                    break;
                }
            }
            break;
        }

        case IPPROTO_IGMP:
        {
            // Sanity check IGMP packet
            if (length < sizeof(IGMP)) {
                return;
            }

            igmp = (IGMP*) ((char*) ip + (ip->headerLength << 2));
            sum = igmp->checksum;
            igmp->checksum = 0;
            if (sum && (sum != network->cksum((U16*) igmp, length))) {
                return;
            }
            igmp->ntoh();
	    length-= sizeof(IGMP);
	    if (igmp->type != IGMPMtraceResp)
		    return;
	    if (length < sizeof(IGMPTrace)) {
		    TRACE(traceNetwork, "receive: IGMPMtraceResp from is too short\n");
		    return;
	    }
	    IGMPTrace *trace = (IGMPTrace*) (igmp + 1);
	    trace->ntoh();
	    length -= sizeof(IGMPTrace);

            Mtrace* mt = pendingMtraces.head();
            while (mt != NULL) {
                Mtrace* nextMt = pendingMtraces.next(mt);
		if (mt->accept_trace(igmp, trace))
		    mt->receive(igmp, trace, length);
                mt = nextMt;
            }
            break;
        }
            
        default:
            return;
    }
    return;
}

U16
Network::cksum(const U16* addr,
               int len)
{
    int nleft = len;
    const u_short *w = addr;
    u_short answer;
    u_short odd_byte = 0;
    register int sum = 0;

    /*
     *  Our algorithm is simple, using a 32 bit accumulator (sum),
     *  we add sequential 16 bit words to it, and at the end, fold
     *  back all the carry bits from the top 16 bits into the lower
     *  16 bits.
     */
    while( nleft > 1 )  {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if( nleft == 1 ) {
        *(u_char *)(&odd_byte) = *(u_char *)w;
        sum += odd_byte;
    }

    /*
     * add back carry outs from top 16 bits to low 16 bits
     */
    sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
    sum += (sum >> 16);                     /* add carry */
    answer = ~sum;                          /* truncate to 16 bits */
    return (answer);
}


//  Copyright (c) 1994 by the University of Southern California.
//  All rights reserved.
//
//  Permission to use, copy, modify, and distribute this software and
//  its documentation in source and binary forms for lawful
//  non-commercial purposes and without fee is hereby granted, provided
//  that the above copyright notice appear in all copies and that both
//  the copyright notice and this permission notice appear in supporting
//  documentation, and that any documentation, advertising materials,
//  and other materials related to such distribution and use acknowledge
//  that the software was developed by the University of Southern
//  California and/or Information Sciences Institute.
//  The name of the University of Southern California may not
//  be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THE UNIVERSITY OF SOUTHERN CALIFORNIA DOES NOT MAKE ANY REPRESENTATIONS
//  ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  THIS SOFTWARE IS
//  PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
//  INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, TITLE, AND 
//  NON-INFRINGEMENT.
//
//  IN NO EVENT SHALL USC, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT,
//  TORT, OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH,
//  THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//  Questions concerning this software should be directed to 
//  scan@isi.edu.
//


