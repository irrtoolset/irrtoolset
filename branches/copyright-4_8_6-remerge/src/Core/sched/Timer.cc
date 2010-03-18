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
// Timer.cc
// Author(s): Ramesh Govindan <govindan@isi.edu>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "util/Types.hh"
#include "util/Handler.hh"
#include "util/List.hh"
#include "util/Trail.hh"

#include "sys/Time.hh"
#include "sched/Timer.hh"
#include "sched/Dispatcher.hh"

// File local variables
static TraceCode traceTimer("timer");

// The one-shot timer callback is initiated by the dispatcher.
// Typically, we emulate periodic timers by re-creating a new
// timer from the handler.

Timer::Timer(const Handler& h,
             const TimeLong& at)
	: ListNode(), expireAt(at), handler(h), pending_(true)
{
    dispatcher.timers.insertSorted(this);
    TRACE(traceTimer,
          "creating new timer\n");
}

Timer::~Timer()
{
	if (pending_) {
		dispatcher.timers.remove(this);
		TRACE(traceTimer,
		      "dequeuing and freeing timer\n");
	}
}

bool
Timer::operator<(const Timer& t) const 
{
    return (expireAt < t.expireAt);
}

bool
Timer::operator==(const Timer& t) const
{
    return (expireAt == t.expireAt);
}

ITimer::ITimer(const Handler& h,
	       const TimeShort& ival) 
	: Timer(h, dispatcher.systemClock+ival), 
	  ival_(ival) {
}

void
ITimer::handle() {
	restart();
	if (!handler.null())
		handler.callBack((void*) this);
}
void
ITimer::restart() {
	if (pending_) 
		dispatcher.timers.remove(this);
	expireAt= dispatcher.systemClock + ival_;
	dispatcher.timers.insertSorted(this);
	pending_= true;
	TRACE(traceTimer,
	      "rescheduled interval timer\n");
}
void 
ITimer::stop() {
	if (pending_) {
		pending_= false;
		dispatcher.timers.remove(this);
	}
}
// 
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
