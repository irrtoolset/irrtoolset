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

extern "C" {
#include <sys/types.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
}

#include "util/Types.hh"
#include "util/Trail.hh"
#include "util/Handler.hh"
#include "util/Buffer.hh"
#include "sys/File.hh"
#include "sys/Pipe.hh"
#include "sys/Time.hh"
#include "sched/Timer.hh"
#include "sched/Dispatcher.hh"

#include "network/TProbe.hh"
#include "network/Headers.hh"
#include "network/Network.hh"

// Constants
static const unsigned char 	UDPProtocol = 17;

// Locals
static TraceCode	traceTProbe("net");
static TimeShort	maxWait(10, 0);
static unsigned int	probeNumber = 0x80000000;

static void
tprobeTimerHandler(void* ptr,
                   void*)
{
    ((TProbe*) ptr)->handleTimer();
}

TProbe::TProbe(Address* a,
               Handler& cb,
               int hc,
               Address* v)
        : ListNode()
{
    done = cb;
    to = *a;
    hopcount = hc;
    if (v) {
        via = *v;
    }
    timer = NULL;
    result = TProbeResultNone;
    residualHops = 0;
    network->pendingTProbes.append(this);
    probeId = 0;
    lastRtt = InfiniteInterval;
    sent = 0;
}

TProbe::~TProbe()
{
    network->pendingTProbes.remove(this);
    if (timer) {
        delete timer;
    }
}

void
TProbe::send()
{
    Handler	th(tprobeTimerHandler, this);
    Handler	nh(NULL, NULL);
    Buffer*	buf;
    IP*		ip;
    IPOptLSRR*	ipopt;
    UDP*	udp;
    RawSocket*	raw;
    U32*	t;
    TimeLong	at;
    Address	srcaddr;

    responder.set(0);
    result = TProbeResultNone;
    sent++;
    network->tprobesSent++;

    // Send packet
    raw = new RawSocket(nh, nh, RawSocketICMP);
    if (via.get()) {
        buf = new Buffer(sizeof(IP) + sizeof(IPOptLSRR) + sizeof(UDP));
    } else {
        buf = new Buffer(sizeof(IP) + sizeof(UDP));
    }
    buf->zeroFill();

    ip = (IP*) buf->contents;
    ip->headerLength = via.get() ? 
        (sizeof(IP) + sizeof(IPOptLSRR)) >> 2 :
         sizeof(IP) >> 2;
    ip->version = 4;
    ip->typeOfService = 0;
    ip->totalLength = (ip->headerLength << 2) + sizeof(UDP);
    ip->identifier = 0;
    ip->fragmentOffset = 0;
    ip->timeToLive = hopcount;
    ip->protocol = UDPProtocol;
    ip->source = (network->interfaces.head())->address.get();
    ip->destination = (via.get()) ? via.get() : to.get();
    ip->hton();
    ip->checksum = 0;
    ip->checksum = network->cksum((U16*) ip, 
                                  ip->headerLength << 2);

    if (via.get()) {
        IPOptLSRR*	lsrr;

        lsrr = (IPOptLSRR*) (ip + 1);
        lsrr->nullopt = IPOPT_NOP;
        lsrr->lsrropt = IPOPT_LSRR;
        lsrr->length = sizeof(U32) + 3;
        lsrr->pointer = IPOPT_MINOFF;
        lsrr->gateway = to.get();
        lsrr->hton();
        udp = (UDP*) (lsrr + 1);
    } else {
        udp = (UDP*) (ip + 1);
    }

    ASSERT(probeNumber != 0xffffffff);	// Wrap around
    probeNumber++;
    probeId = probeNumber;

    udp->sourcePort = ((unsigned int) probeId) & 0xffff;
    udp->destinationPort = (((unsigned int) probeId >> 16) & 0xffff);
    ASSERT(udp->destinationPort & 0x8000);
    udp->length = sizeof(UDP);
    udp->hton();
    udp->checksum = 0;

    dispatcher.systemClock.sync();
    sentAt = dispatcher.systemClock;

    raw->sendTo(buf->contents, buf->size, to, 0);

    TRACE(traceTProbe, "tprobe to %s:%d\n", to.name(), hopcount);

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
TProbe::handleTimer()
{
    TRACE(traceTProbe, "tprobe failed to %s\n", to.name());

    timer = NULL;
    network->tprobesTimedOut++;
    done.callBack((void*) this);
    lastRtt = InfiniteInterval;
    return;
}

Boolean
TProbe::receive(IP* ip)
{
    ICMP*		icmp;
    UDP*		udp;
    unsigned short	sum;
    IP*			iip;
    int			datalen;

    // Verify signature to see if it is for us
    icmp = (struct ICMP*) ((char*) ip + (ip->headerLength << 2));
    if (icmp->type != ICMPTypeTimeExceeded &&
        icmp->type != ICMPTypeDestinationUnreachable) {
        return false;
    }

    datalen = ip->totalLength - (ip->headerLength << 2) - sizeof(ICMP);
    if (datalen < sizeof(IP)) {	
        return false;		// Malformed response, discard
    }

    iip = (struct IP*) (icmp + 1);
    iip->ntoh();

    if (datalen < ((iip->headerLength << 2) + 2 * sizeof(unsigned long))) {
        return false;		// Timestamp not there, discard
    }

    if (iip->destination != to.get()) {
        return false;
    }

    udp = (UDP*) (((char*) iip) + (iip->headerLength<<2));
    udp->ntoh();

    if (udp->sourcePort != (((unsigned int) probeId) & 0xffff)) {
        return false;
    }

    if (udp->destinationPort != (((unsigned int) probeId >> 16) & 0xffff)) {
        return false;
    }

    dispatcher.systemClock.sync();
    lastRtt = dispatcher.systemClock - sentAt;

    switch (icmp->type) {
        case ICMPTypeTimeExceeded:
	    if (iip->timeToLive <= 1) {
                responder.set(ip->source);
                result = TProbeResultTimeExpired;
                TRACE(traceTProbe, "received tprobe for %s:%d from %s\n",
                      to.name(), hopcount, responder.name());
	    } else {
                responder.set(ip->source);
                ERROR("unexpected ttl %d in ICMP TimeExceeded from %s\n",
                      iip->timeToLive, responder.name());
	        result = TProbeResultDontCare;
	    } 
            done.callBack((void*) this);
            break;

        case ICMPTypeDestinationUnreachable:
            residualHops = iip->timeToLive;
            responder.set(ip->source);
            switch (icmp->code) {
                case ICMPUnreachNet:
                case ICMPUnreachHost:
                    result = TProbeResultUnreachable;
                    break;

                case ICMPUnreachProto:
                case ICMPUnreachPort:
                    result = TProbeResultReachedNode;
                    break;

                case ICMPUnreachSrtFail:
                    result = TProbeResultSourceRouteFailed;
                    break;

                case ICMPUnreachNeedFrag:
                default:
                    result = TProbeResultDontCare;
                    break;
            }
            TRACE(traceTProbe, "received tprobe for %s:%d from %s\n",
                  to.name(), hopcount, responder.name());
            done.callBack((void*) this);
            // FallThrough

        default:
            break;
    }
    return true;
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


