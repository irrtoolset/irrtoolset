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

#ifndef _Mtrace_hh
#define _Mtrace_hh

#include "sys/File.hh"
#include "network/Headers.hh"
#include "sys/Time.hh"

class Timer;
static const U32	AllRoutersMulticast = 0xe0000002;
static const U32	MtraceMulticast = 0xe0000120;

// Classes defined elsewhere
class Handler;
class IP;

class Mtrace : public ListNode {
    private:
    Mtrace(const Mtrace&);

  public:
    // To send a receive a specific mtrace
    Mtrace(const Address*,		// In: source to mtrace
           const Address*,		// In: group to mtrace
           const Address*,		// In: response address
           const Address*,		// In: receiver of mtrace
           const Address*,		// In: target of mtrace
           int,			        // In: number of hops
           const Handler&);		// In: handler to call back on reply

    // To receive any mtrace
    Mtrace();

    virtual ~Mtrace();

    // Send one
    void send();

    // Handle timers
    virtual void timeout();

    // Process a multicast mtrace response
    void
    processMresp();

    // Process received packet
    void
    receive(const IGMP *igmp,
	    const IGMPTrace *trace, 
	    int length); //     packet and its length

    virtual bool accept_trace(const IGMP *, const IGMPTrace* trace) const {
	return (trace->queryId == identifier &&
		trace->source == source.get());
    }

    Address	source;
    Address	group;
    Address	replyTo;
    Address	receiver;
    Address	target;
    int		nhops;
    IGMPTrace	recvTrace_;
    IGMP        recvIGMP_;
    TimeShort   maxWait_;
    Address     localA_;

    class Response : public ListNode {
        friend Mtrace;

      private:
        Response(const Response&);

      public:
        Response(IGMPTraceResponse*);	// In: one response block
        ~Response();
	void ntoh() { response.ntoh(); }
	void hton() { response.hton(); }
        IGMPTraceResponse	response;
    };
    List<Response>	responses;

  protected:
    U32			identifier;	// Of last mtrace sent
    Handler		done;
    Timer*		timer;
};

extern DList<Mtrace>	pendingMtraces;

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

#endif _Ping_hh
