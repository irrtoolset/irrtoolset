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
