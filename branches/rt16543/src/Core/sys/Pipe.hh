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

#endif // _Pipe_hh
