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
#include "sys/Time.hh"
#include "sys/File.hh"
#include "sched/Timer.hh"
#include "sched/Dispatcher.hh"
#include "sys/Pipe.hh"

#include <iostream>

using namespace std;

// Constants
static const int	ReadBufferSize = 512;

// File locals
static TraceCode	tracePipe("pipe");

static void
pipeRead(void* ptr,
         void*)
{
    ((Pipe*) ptr)->read();
}

static void
pipeWrite(void* ptr,
          void*)
{
    ((Pipe*) ptr)->write();
}

static void
pipeTimeout(void* ptr,
            void*)
{
    ((Pipe*) ptr)->timeout();
}

Pipe::Pipe(ListenSocket* s,
           const TimeShort& t,
           Boolean u)
	: ListNode()
{
    Handler	readH(pipeRead, this);
    Handler	nullH((CallBackFunc)NULL, (void *)NULL);
    Handler	tH(pipeTimeout, this);
    TimeLong	at;

    incoming = new Buffer(ReadBufferSize);
    if (s->accept(&socket, address, &port, readH, nullH) < 0) {
        return;		// XXX: cleanup?
    }

    timer = (Timer *)NULL;
    gotsome = false;
    connected = true;
    interval = t;
    passive = true;
    unbuffered = u;
    logicallyDeleted = false;

    if (interval < InfiniteInterval) {
        dispatcher.systemClock.sync();
        at = dispatcher.systemClock;
        at = at + interval;
        timer = new Timer(tH, at);
    }

    TRACE(tracePipe,
          "new passive end of pipe for %s:%u\n",
          address.name(), port);
    return;
}

Pipe::Pipe(const Address& addr,
           Port p,
           Boolean u)
	: ListNode()
{
    Handler	rh((CallBackFunc)NULL, (void *)NULL);
    Handler	wh(pipeWrite, this);
    
    incoming = new Buffer(ReadBufferSize);
    address = addr; 
    port = p;
    connected = false;

    passive = false;
    gotsome = false;
    interval = InfiniteInterval;
    timer = (Timer *)NULL;
    unbuffered = u;
    logicallyDeleted = false;

    socket = new StreamSocket(address, port, rh, wh);
    TRACE(tracePipe,
          "new active end for pipe to %s:%u\n",
          address.name(),
          port);
    return;
}

Pipe::~Pipe()
{
    TRACE(tracePipe,
          "deleting server %s:%u\n",
          address.name(),
          port);
    
    if (timer) {
        delete timer;
    }
    delete socket;
    delete incoming;
    outgoing.clear();
}

void
Pipe::read()
{
    int		length;

    ASSERT(!logicallyDeleted);

    gotsome = true;
    length = socket->read((char *) (incoming->contents + incoming->size),
                              incoming->capacity - incoming->size);

    if (length == 0 || length == FileOpHardError) {
        terminate();
        return;
    } else if (length == FileOpSoftError) {
        return;
    }

    TRACE(tracePipe,
          "read %u bytes from %s:%d, size %u\n",
          length, address.name(), port, incoming->size);

    // Check if we can flush what we've read so far
    incoming->size += length;
    receive();

    // If we have exceeded buffer capacity...
    if (incoming->size == incoming->capacity) {
        Buffer*		old = incoming;

        incoming = new Buffer((old->capacity << 1));
        incoming->append(old->contents, old->size);
        delete old;
    }

    return;
}

void
Pipe::write()
{
    int		length;
    Handler	rh(pipeRead, this);
    Handler	nh((CallBackFunc)NULL, (void *)NULL);
    Buffer*	send;

    if (!connected) {
        TRACE(tracePipe, "client connected to %s:%u\n", 
              address.name(), port);

        connected = true;
        if (outgoing.isEmpty()) {
            socket->setHandlers((logicallyDeleted) ? nh : rh, nh);
        }
        return;
    }

    gotsome = true;
    send = outgoing.head();
    if (!send && unbuffered) {
        send = nextBuffer();
        if (send) {
            outgoing.append(send);
        } else {
            socket->setHandlers(rh, nh);
            return;
        }
    }

    length = socket->write((char *) (send->contents + send->offset),
                           send->size - send->offset);

    if (length < 0) {
        switch (length) {
            case FileOpHardError:
                terminate();
                return;
            case FileOpSoftError:
                return;
            default:
                ASSERT(false);
        }
    }

    if (length == 0) {
        TRACE(tracePipe,
              "remote end %s:%u closed connection\n",
              address.name(), port);
        terminate();
        return;
    }

    TRACE(tracePipe,
          "wrote %u bytes to %s:%u\n",
          length,
          address.name(),
          port);
    
    send->offset += length;
    if (send->offset == send->size) {
        TRACE(tracePipe,
              "completed send of length %u on %s:%u\n",
              send->size, address.name(), port);
        
	outgoing.remove(send);
        delete send;
        if (outgoing.isEmpty()) {
            if (logicallyDeleted) {
                TRACE(tracePipe, "closing logically deleted pipe %s:%u\n",
                      address.name(), port);
                terminate();
                return;
            } 
            if (unbuffered) {
                send = nextBuffer();
                if (send) {
                    outgoing.append(send);
                    return;
                }
            }
            socket->setHandlers(rh, nh);
        }
    }
    return;
}

void
Pipe::send(Buffer *buf)
{
    Handler	rh(pipeRead, this);
    Handler	wh(pipeWrite, this);
    
    // Can't call send after deleting pipe
    if (logicallyDeleted) {
      delete buf;
      return;
    }

    buf->offset = 0;
    gotsome = true;
    if (outgoing.isEmpty()) {
        socket->setHandlers(rh, wh);
    }
    outgoing.append(buf);
    if (outgoing.size() > 5)
       cout << "Queue of " << this << " has " << outgoing.size() 
	    << " members.\n";
        
    TRACE(tracePipe,
          "queued response of length %u on %s:%u\n",
          buf->size, address.name(), port);
    return;
}

void
Pipe::prepareToSend()
{
    Handler	rh(pipeRead, this);
    Handler	wh(pipeWrite, this);
    
    // Can't send to unbuffered pipe
    ASSERT(unbuffered);

    // Can't call send after deleting pipe
    if (logicallyDeleted) {
      return;
    }

    socket->setHandlers(rh, wh);
    return;
}

void
Pipe::timeout()
{
    Handler	th(pipeTimeout, this);
    TimeLong	at;
    
    timer = (Timer *)NULL;
    if (!gotsome) {
        TRACE(tracePipe,
              "no activity on connection to %s:%u, closing\n",
              address.name(), port);
        terminate();
        return;
    }

    gotsome = false;
    dispatcher.systemClock.sync();
    at = dispatcher.systemClock;
    at = at + interval;
    timer = new Timer(th, at);
    return;
}

void
Pipe::flush(int length)
{
    incoming->flush(length, 0);
    TRACE(tracePipe,
          "flushing %d bytes from incoming buffer for %s:%u\n",
          length, address.name(), port);
    return;
}

void
Pipe::flushAndClose()
{
    Handler	nh((CallBackFunc) NULL, (void*) NULL);
    Handler	wh(pipeWrite, this);

    logicallyDeleted = true;
    if (outgoing.isEmpty()) {
        terminate();
        return;
    } 
    socket->setHandlers(nh, wh);	// Prevent reads
    return;
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
//  info-ra@isi.edu.
//

