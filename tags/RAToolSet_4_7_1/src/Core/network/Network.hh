//
// $Id$
//
// Author(s): Ramesh Govindan

#ifndef _Network_hh
#define _Network_hh

#include "util/Types.hh"
#include "util/List.hh"
#include "util/Buffer.hh"
#include "network/Ping.hh"
#include "network/TProbe.hh"
#include "network/ICMProbe.hh"
#include "network/Mtrace.hh"

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK         (u_long)0x7F000001
#endif

// Classes defined elsewhere
class RawSocket;

class Network {
  private:
    Network(const Network&);

  public:
    Network();

    ~Network();

    void
    receive(RawSocket*);	// In: socket on which data came in

    void
    dump(FILE*);		// In: statistics

    static U16
    cksum(const U16*,		// In: pointer to data
          int);			// In: num bytes to checksum

    class Interface : public ListNode {
    private:
        Interface(const Interface&);

    public:
        Interface(char*, 				// Interface name
                  U32);				// Interface address

        ~Interface();

        char*		name;
        Address		address;
    };

    Interface*		defaultInterface;	// defined by default route
    List<Interface>	interfaces;		// all interfaces
    
    List<Ping>		pendingPings;		// list of pending pings
    List<TProbe>	pendingTProbes;		// list of pending traceroutes
    List<ICMProbe>	pendingICMProbes;	// pending traceroutes (ICMP)
    List<Mtrace>	pendingMtraces;		// list of pending mtraces

    RawSocket*		icmpSocket;
    RawSocket*		igmpSocket;

    // Statistics
    unsigned long long	tprobesSent;
    unsigned long long	tprobesTimedOut;
    unsigned long long	icmprobesSent;
    unsigned long long	icmprobesTimedOut;
        
  private:
    Buffer*	recvBuffer;
};

// Externs
extern Network	*network;

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

#endif _Network_hh
