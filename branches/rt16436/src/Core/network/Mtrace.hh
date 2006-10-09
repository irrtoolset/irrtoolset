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

#endif _Ping_hh
