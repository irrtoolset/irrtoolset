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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "util/Types.hh"
#include "util/Trail.hh"
#include "util/Handler.hh"
#include "util/Buffer.hh"
#include "sys/File.hh"
#include "sys/Pipe.hh"
#include "sys/Time.hh"
#include "sched/Timer.hh"
#include "sched/Dispatcher.hh"

#include "network/Ping.hh"
#include "network/Headers.hh"
#include "network/Network.hh"

// Locals
static TraceCode	tracePing("network");
static TimeShort	maxWait(10, 0);
static unsigned int	probeNumber = 1;


static void
pingTimerHandler(void* ptr,
                 void*)
{
    ((Ping*) ptr)->handleTimer();
}

Ping::Ping(Address* a,
           Handler& cb)
        : ListNode()
{
    done = cb;
    addr = *a;
    timer = NULL;
    network->pendingPings.append(this);
}

Ping::~Ping()
{
    if (timer) {
        delete timer;
    }
    network->pendingPings.remove(this);
}

void
Ping::send()
{
    Handler	th(pingTimerHandler, this);
    Handler	nh(NULL, NULL);
    Buffer*	buf;
    IP*		ip;
    ICMP*	icmp;
    RawSocket*	raw;
    unsigned long*	t;
    TimeLong	at;

    raw = new RawSocket(nh, nh, RawSocketICMP);
    buf = new Buffer(sizeof(IP) + sizeof(ICMP) + sizeof(unsigned long long));
    buf->zeroFill();

    ip = (IP*) buf->contents;
    ip->headerLength = sizeof(IP) >> 2;
    ip->version = 4;
    ip->typeOfService = 0;
    ip->totalLength = sizeof(IP) + sizeof(ICMP) + sizeof(unsigned long long);
    ip->identifier = 0;
    ip->fragmentOffset = 0;
    ip->timeToLive = 0xff;
    ip->protocol = IPPROTO_ICMP;
    ip->source = (network->interfaces.head())->address.get();
    ip->destination = addr.get();
    ip->hton();
    ip->checksum = 0;
    ip->checksum = htons(network->cksum((U16*) ip, sizeof(IP)));

    // We encode the sequence number and ident in the probe
    sequence = probeNumber++;
    icmp = (ICMP*) (ip + 1);
    icmp->type = ICMPTypeEcho;
    icmp->code = 0;
    icmp->gateway = sequence;
    icmp->hton();

    dispatcher.systemClock.sync();
    sent = dispatcher.systemClock;

    icmp->checksum = 0;
    icmp->checksum = network->cksum((U16*) icmp,
                                    sizeof(ICMP) + sizeof(unsigned long long));

    raw->sendTo(buf->contents, buf->size, addr, 0);

    TRACE(tracePing, "ping sent to %s\n", addr.name());

    delete raw;
    delete buf;
    
    if (timer) {
        delete timer;
    }
    at = dispatcher.systemClock;
    at = at + maxWait;
    timer = new Timer(th, at);

    return;
}

void
Ping::handleTimer()
{
    TRACE(tracePing, "ping failed to %s\n", addr.name());

    timer = NULL;
    lastRtt = InfiniteInterval;
    done.callBack((void*) this);
    return;
}

Boolean
Ping::receive(IP* ip)
{
    ICMP*		icmp;
    unsigned long*	t;

    icmp = (struct ICMP*) ((char*) ip + (ip->headerLength << 2));
    // Verify signature to see if it is for us
    if (icmp->type == ICMPTypeEchoReply) {
        if (ip->source != addr.get()) {
            return false;
        }
        if (icmp->gateway != sequence) {
            return false;
        }
        
        TRACE(tracePing, "ping received from %s\n", addr.name());

        t = (unsigned long *) (icmp+1);

        dispatcher.systemClock.sync();
        lastRtt = dispatcher.systemClock - sent;
        done.callBack((void*) this);
        return true;
    }
    return false;
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


