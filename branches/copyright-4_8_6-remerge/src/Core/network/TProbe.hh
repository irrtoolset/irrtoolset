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

#ifndef _TProbe_hh
#define _TProbe_hh

#include "util/Handler.hh"
#include "sys/Time.hh"

// Classes defined elsewhere
class Handler;
class IP;
class Timer;

enum TProbeResult {
    TProbeResultNone,
    TProbeResultTimeExpired,
    TProbeResultUnreachable,
    TProbeResultReachedNode,
    TProbeResultSourceRouteFailed,
    TProbeResultDontCare
};

class TProbe : public DListNode {
  private:
    TProbe(const TProbe&);

  public:
    // Constructor
    TProbe(Address*,		// In: address to traceroute
           Handler&,		// In: handler to call back on reply
           int,			// In: number of hops
           Address* = NULL);	// In: via a specified gateway

    ~TProbe();

    // Handle timers
    void
    handleTimer();

    // Send one traceroute packet
    void
    send();

    // Receive one traceroute packet
    Boolean			// Out: true if matched
    receive(IP*);

    Address		to;
    Address		via;
    int			hopcount;
    Address		responder;
    TProbeResult	result;
    int			residualHops;
    int			sent;
    TimeShort		lastRtt;

  private:
    Timer*		timer;
    Handler		done;
    unsigned int	probeId;
    TimeLong		sentAt;
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

#endif _Traceroute_hh
