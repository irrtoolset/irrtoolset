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

#include "network/Mtrace.hh"
#include "network/Headers.hh"
#include "network/Network.hh"

// Constants
static const U8		MaxUnicastTtl = 255;
static const U8		MaxMulticastTtl = 192;

// Locals
static TraceCode	traceMtrace("network");
static TimeShort	maxWait(10, 0);
static int		mtraceSequence = 0;

static void
mtraceTimerHandler(void* ptr,
                   void*)
{
    ((Mtrace*) ptr)->timeout();
}

Mtrace::Mtrace(const Address* s,
               const Address* g,
               const Address* r,
               const Address* R,
               const Address* t,
               int h,
               const Handler& cb)
        : ListNode(), localA_(0UL)
{
    done = cb;
    source = *s;
    group = *g;
    maxWait_= maxWait;
    DatagramSocket ds(Handler(0,0), Handler(0,0));
    if (ds.defaultInterface(localA_, group) < 0 ) {
	    FATAL("couldn't find local address\n");
    }

    if (r) {
	    replyTo = *r;
	    //ASSERT(!IN_MULTICAST(replyTo.get()));
    } else {
	    replyTo= localA_; //.set(MtraceMulticast);
    }

    if (R) {
        receiver = *R;
    } else {
	    receiver= localA_;
    }
    if (t) {
        target = *t;
    } else {
        target.set(AllRoutersMulticast);
    }

    nhops = h;
    timer = NULL;
    network->pendingMtraces.append(this);
}

Mtrace::~Mtrace()
{
    if (timer) {
        delete timer;
    }

    responses.clear();
    network->pendingMtraces.remove(this);
}

void
Mtrace::send()
{
    Handler	th(mtraceTimerHandler, this);
    Handler	nh(NULL, NULL);
    Buffer*	buf;
    IP*		ip;
    IGMP*	igmp;
    IGMPTrace*	trace;
    TimeLong	at;
    Address	addr;
    RawSocket raw(nh, nh, RawSocketIGMP);
    U8 ttl= 1;
    if(raw.setopt(IPPROTO_IP, IP_MULTICAST_TTL,
		  (char*)&ttl, sizeof(ttl)) < 0) {
	    TRACE(traceMtrace, "setsockopt IP_MULTICAST_TTL %d\n", ttl);
    }
    struct in_addr adr;
    adr.s_addr= htonl(localA_.get());
    if(raw.setopt(IPPROTO_IP, IP_MULTICAST_IF,
		  (char*)&adr, sizeof(adr)) < 0) {
	    TRACE(traceMtrace, "setsockopt IP_MULTICAST_IF %s\n", localA_.name());
    }
//      if(network->igmpSocket->setopt(IPPROTO_IP, IP_MULTICAST_IF,
//  	   (char*)&adr, sizeof(adr)) < 0) {
//  	    TRACE(traceMtrace, "setsockopt for igmpSocket IP_MULTICAST_IF %s\n", localA_.name());
//      }
    U8 loop= 1;
    if(raw.setopt(IPPROTO_IP, IP_MULTICAST_LOOP,
  		  (char*)&loop, sizeof(loop)) < 0) {
  	    TRACE(traceMtrace, "setsockopt IP_MULTICAST_LOOP\n");
    }

    buf = new Buffer(sizeof(IP) + sizeof(IGMP) + sizeof(IGMPTrace));

    buf->size= buf->capacity;
    ip = (IP*) buf->contents;
    ip->headerLength = sizeof(IP) >> 2;
    ip->version = 4;
    ip->typeOfService = 0;
    ip->totalLength = sizeof(IP) + sizeof(IGMP) + sizeof(IGMPTrace);
    ip->identifier = 0;
    ip->fragmentOffset = 0;
    ip->timeToLive = 0xff;
    ip->protocol = IPPROTO_IGMP;
    ip->source = localA_.get(); //(network->interfaces.head())->address.get();
    ip->destination = target.get();
    ip->hton();
    ip->checksum = 0;
    ip->checksum = htons(network->cksum((U16*) ip, sizeof(IP)));

    igmp = (IGMP*) (ip + 1);
    igmp->type = IGMPMtrace;
    igmp->code = nhops;
    igmp->group = group.get();
    igmp->hton();

    trace = (IGMPTrace*) (igmp + 1);
    trace->source = source.get();
    trace->destination = receiver.get();
    trace->responseAddress = replyTo.get();
    trace->timeToLive = 
        IN_MULTICAST(trace->destination) ? 
        	MaxMulticastTtl : MaxUnicastTtl;
    //    identifier = ++mtraceSequence;
    identifier = random() >> 8;
    trace->queryId = identifier;
    trace->hton();

    igmp->checksum = 0;
    igmp->checksum = network->cksum((U16*) igmp,
                                    sizeof(IGMP) + sizeof(IGMPTrace));

    addr.set(AllRoutersMulticast);
    raw.sendTo(buf->contents, buf->size, addr, 0);

    TRACE(traceMtrace, "mtrace sent to %s\n", group.name());

    delete buf;
    
    if (timer) {
        delete timer;
    }
    at = dispatcher.systemClock;
    at = at + maxWait_;
    timer = new Timer(th, at);

    return;
}

void
Mtrace::timeout()
{
    TRACE(traceMtrace, "mtrace failed to %s\n", group.name());

    timer = NULL;
    done.callBack((void*) this);
    return;
}

void
Mtrace::receive(const IGMP *igmp, const IGMPTrace *trace, int length)
{
    recvTrace_= *trace;
    recvIGMP_= *igmp;
    IGMPTraceResponse*	resp;
    // Save the response
    resp = (IGMPTraceResponse*) (trace + 1);
    for (int i = 0; i < (length / sizeof(IGMPTraceResponse)); i++) {
            Mtrace::Response*	response;
	    
            response = new Mtrace::Response(resp);
	    response->ntoh();
            responses.append(response);
            resp++;
    }
    
    done.callBack((void*) this);
    return;
}

Mtrace::Response::Response(IGMPTraceResponse* r)
	: ListNode()
{
    response = *r;
}

Mtrace::Response::~Response()
{
    // Empty
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


