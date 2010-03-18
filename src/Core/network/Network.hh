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

#endif _Network_hh
