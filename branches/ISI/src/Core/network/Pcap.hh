//
// $Id$
//
// Author(s): Cengiz Alaettinoglu

#ifndef _Pcap_hh
#define _Pcap_hh

#include "util/List.hh"

typedef struct pcap pcap_t;

// Classes defined elsewhere
class Address;
class File;

class Pcap : public ListNode {
private:
   Pcap(const Pcap&);

public:
   Pcap(char *filter = NULL,    // In: tcpdump style filter expression
				      // leave it blank for all packets
	char *device = NULL);   // In: set to device name, e.g. eth0, ppp0
				      // leave it blank for us to guess it
	
   virtual ~Pcap();

   // Process captured packet
   virtual void receive(const Address &source,
			const Address &destination, 
			int protocol) = 0;
   virtual void receive_ip(const U8 *p, U32 caplen, U32 length); 

   void issueRead();		// Internal function
   int datalink_;
	bool error_;
	char *ebuf_;
private:
	static void pcapHandleEther(u_char *, const struct pcap_pkthdr *, const u_char *);
	static void   pcapHandleATM(u_char *, const struct pcap_pkthdr *, const u_char *);
	static void   pcapHandlePPP(u_char *, const struct pcap_pkthdr *, const u_char *);

   void (*handler_)(u_char *, const struct pcap_pkthdr *, const u_char *);
   pcap_t *pcap_descriptor;
   File   *file;
};

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

#endif _Pcap_hh
