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

#ifndef _Pipe_hh
#define _Pipe_hh

#include "util/Types.hh"
#include "util/List.hh"
#include "util/Buffer.hh"
#include "sys/Address.hh"
#include "sys/Time.hh"

// Classes defined elsewhere
class StreamSocket;
class Timer;
class ListenSocket;

class Pipe : public ListNode {
  private:
    Pipe(const Pipe&);

  public:
    // Passive end of a TCP connection
    Pipe(ListenSocket*,	// In: accept a connection on this
         const TimeShort& = InfiniteInterval,
    			// In: teardown after inactivity
         Boolean unb = false);	// Unbuffered write

    // Active end of a TCP connection
    Pipe(const Address&,// In: connect to this address
         Port,		// In: ... and this port
         Boolean unb = false);	// In: unbuffered writes

    virtual ~Pipe();

    // Read some data from the connection
    void
    read();

    // Write some data to the connection
    void
    write();

    // Send a response to the remote end
    void
    send(Buffer*);

    // Prepare to send one or more responses to remote end
    void
    prepareToSend();

    // Process inactivity timeout
    void
    timeout();

    // Flush the incoming buffer
    void
    flush(int);				// In: how many bytes to flush

    // Process received data
    virtual void
    receive() {
        // Empty
    };

    // Notify abnormal termination of pipe
    virtual void
    terminate() {
        // Empty
    };

    // Notify of ready to write
    virtual Buffer*
    nextBuffer() {
        return NULL;
    };

    // Flush any remaining contents and close
    void
    flushAndClose();

  protected:
    Buffer*		incoming;
    Boolean		passive;

    Address		address;
    Port		port;

    StreamSocket*	socket;
    List<Buffer>	outgoing;

    Timer*		timer;
    Boolean		gotsome;
    Boolean		connected;
    Boolean		logicallyDeleted;
    Boolean		unbuffered;
    TimeShort		interval;
};

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

#endif // _Pipe_hh
