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
// $Id$
//
// Author(s): Cengiz Alaettinoglu

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(HAVE_PCAP_H)

#include "util/Types.hh"
#include "util/Trail.hh"
#include "util/Handler.hh"
#include "util/Buffer.hh"
#include "sys/File.hh"
#include "sys/Pipe.hh"
#include "sys/Time.hh"
#include "sched/Timer.hh"
#include "sched/Dispatcher.hh"

#include "network/Pcap.hh"
#include "network/Headers.hh"
#include "network/Network.hh"

extern "C" {
#include <sys/socket.h>
#include <net/if.h>
    //#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/in_systm.h>
    //#include <netinet/ip.h>

#include <pcap.h>

int     pcap_read(pcap_t *, int cnt, pcap_handler, U8 *);
int tuba=0, idrp=0; //some pcaps want these flags declared
}


// Locals
static TraceCode	tracePcap("network");
const int DefaultSnapLen = 68;

static void pcapReadHandler(void* pcap, void*) {
   ((Pcap *) pcap)->issueRead();
}

void Pcap::pcapHandleEther(u_char *pcap, const struct pcap_pkthdr *h, const u_char *p) {
    U32 caplen = h->caplen;
    U32 length = h->len;
    U16 pay_type;
    
    if (caplen < sizeof(struct ether_header)) {
	    TRACE(tracePcap, "ethernet header not found\n");
	    return;
    }
    length-= sizeof(struct ether_header);
    caplen-= sizeof(struct ether_header);
    pay_type = ntohs(((struct ether_header *)p)->ether_type);
    p += sizeof(struct ether_header);

    if (pay_type != ETHERTYPE_IP) return;

    ((Pcap*) pcap)->receive_ip(p, caplen, length);
}

void Pcap::pcapHandleATM(u_char *pcap, const struct pcap_pkthdr *h, const u_char *p) {
    U32 caplen = h->caplen;
    U32 length = h->len;
    U16 pay_type;
    
    if (caplen < 8) {
	    TRACE(tracePcap, "ATM header not found\n");
	    return;
    }
    if (p[4] == 0xaa || p[5] == 0xaa || p[6] == 0x03) {
	    /* if first 4 bytes are cookie/vpci */
	    p+= 4;
	    length-= 4;
	    caplen-= 4;
    } else if (p[0] != 0xaa || p[1] != 0xaa || p[2] != 0x03) {
	    /*XXX assume 802.6 MAC header from fore driver */
	    p+= 20;
	    length-= 20;
	    caplen-= 20;
    }
    pay_type = p[6] << 8 | p[7];
    if (pay_type != ETHERTYPE_IP) return;
    length-= 8;
    caplen-= 8;
    p+= 8;
    ((Pcap*) pcap)->receive_ip(p, caplen, length);
}

void Pcap::pcapHandlePPP(u_char *pcap, const struct pcap_pkthdr *h, const u_char *p) {
    U32 caplen = h->caplen;
    U32 length = h->len;
    U16 pay_type;
#define PPP_HDRLEN 4
    if (caplen<PPP_HDRLEN) {
	    TRACE(tracePcap, "PPP header's too short\n");
	    return;
    }
#define PPP_PROTOCOL(p) ((((U8 *)(p))[2] << 8) + ((U8 *)(p))[3])
#define PPP_IP          0x0021  /* Raw IP */
    switch (pay_type= PPP_PROTOCOL(p)) {
    case PPP_IP:
	    pay_type= ETHERTYPE_IP;
	    /* fallthrough */
    case ETHERTYPE_IP:
	    p+= PPP_HDRLEN;
	    length-= PPP_HDRLEN;
	    caplen-= PPP_HDRLEN;
	    break;
    }
    if (pay_type != ETHERTYPE_IP) return;
    ((Pcap*) pcap)->receive_ip(p, caplen, length);
}

Pcap::Pcap(char *filter, char *device) : ListNode(), error_(true),  pcap_descriptor(0), file(0) {
   int snaplen = DefaultSnapLen;
   int promiscuous = 1;
   bpf_u_int32 localnet, netmask;
   struct bpf_program fcode;
   int Oflag = 1;			/* run filter code optimizer */
   int i;
   ebuf_= new char[PCAP_ERRBUF_SIZE];
   if (device == NULL) {
	   device = pcap_lookupdev(ebuf_);
	   if (device == NULL) {
		   sprintf(ebuf_, "no device found");
		   return;
      }
   }
   pcap_descriptor= 0;
   pcap_descriptor = pcap_open_live(device, snaplen, promiscuous, 1000, ebuf_);
   if (pcap_descriptor == NULL)
	   return;

   datalink_= pcap_datalink(pcap_descriptor);
   switch(datalink_) {
   case DLT_EN10MB :
   case DLT_IEEE802 :
	   handler_= pcapHandleEther;
	   break;
   case DLT_ATM_RFC1483 :
	   handler_= pcapHandleATM;
	   break;
   case DLT_PPP :
	   handler_= pcapHandlePPP;
	   break;
   default:
	   sprintf(ebuf_, "unsupported link layer: %d", datalink_);
	   return;
   }
   i = pcap_snapshot(pcap_descriptor);
   if (snaplen < i) {
      ERROR("snaplen raised from %d to %d", snaplen, i);
      snaplen = i;
   }

   if (pcap_lookupnet(device, &localnet, &netmask, ebuf_) < 0) {
      localnet = 0;
      netmask = 0;
      ERROR("%s\n", ebuf_);
   }

   if (pcap_compile(pcap_descriptor, &fcode, filter, Oflag, netmask) < 0) {
	   sprintf(ebuf_, "%s", pcap_geterr(pcap_descriptor));
	   return;
   }
   if (pcap_setfilter(pcap_descriptor, &fcode) < 0) {
	   sprintf(ebuf_, "%s\n", pcap_geterr(pcap_descriptor));
	   return;
   }

   Handler rH(pcapReadHandler, this);
   Handler nH((CallBackFunc)NULL, (void *)NULL);
   // the pcap_t points to memory where the first word is the file descriptor 
   file = new File(* (int *) pcap_descriptor, FileModeReadOnly, rH, nH);

   error_= false;
}

Pcap::~Pcap() {
	if (pcap_descriptor)
		pcap_close(pcap_descriptor);
	if (file)
		delete file;
	delete [] ebuf_;
}

void Pcap::issueRead() {
   if (pcap_read(pcap_descriptor, 
		 1, 
		 handler_, 
		 (u_char *) this) < 0)
      TRACE(tracePcap, "pcap: %s\n", pcap_geterr(pcap_descriptor));
}

void Pcap::receive_ip(const U8 *p, U32 caplen, U32 length) {
	if (caplen < sizeof(IP)) {
		TRACE(tracePcap, "ip header not found\n");
		return;
	}
	IP *ip= (IP*)p;
	ip->ntoh();
	receive(Address(ip->source), Address(ip->destination), ip->protocol);
}
//  Copyright (c) 1994 by the University of Southern California.
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


#endif HAVE_PCAP

