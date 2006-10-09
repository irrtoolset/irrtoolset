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
// Dispatcher.cc
// Author(s): Ramesh Govindan <govindan@isi.edu>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "util/Types.hh"
#include "util/Trail.hh"
#include "sys/File.hh"
#include "sys/Signal.hh"

#include "sched/Timer.hh"
#include "sched/Job.hh"
#include "sched/Dispatcher.hh"

// Constants
static const int MaxFiles = 4096;

// File local variables
static File *readableFiles[MaxFiles];
static File *writableFiles[MaxFiles];
static TraceCode traceDispatcher("dispatcher");

// The dispatcher class. There is only one instance of
// this class in the system.
Dispatcher dispatcher;
TimeLong& sysClock = dispatcher.systemClock;

Dispatcher::Dispatcher()
{
    systemClock.sync();
}

void
Dispatcher::JobList::process()
{
    Job*	job;

    if (isEmpty()) {
        return;
    }

    TRACE(traceDispatcher,
          "processing a job\n");
    job = dequeue();
    dispatcher.systemClock.sync();
    job->handle();
    if (job->persistent) {
	append(job); // move it to the end of the list
    } else {
	delete job; 
    }
    return;
}
    
void
Dispatcher::TimerList::process()
{
    Timer*	timer;
    
    if (isEmpty()) {
        return;
    }

    while ((timer= head()) && (dispatcher.systemClock > timer->time())) {
	TRACE(traceDispatcher,
              "processing a timer\n");
        dispatcher.systemClock.sync();
        timer->handle();
    }
    return;
}

void
Dispatcher::TimerList::nextTimer(TimeLong& next)
{
    if (isEmpty()) {
        next = InfiniteTime;
        return;
    }

    next = head()->time();
}

void
Dispatcher::FileList::process(TimeShort& waitFor)
{
    File* 	file;
    int		nr, nw;
    
    if (select(waitFor) <= 0) {
        return;
    }

    nr = 0; nw = 0;
    for (file = head(); file != NULL; file = next(file)) {
        if (issetRead(file)) {
            ASSERT(nr < MaxFiles);
            readableFiles[nr++] = file;
        }
    }

    TRACE(traceDispatcher,
          "%d file descriptor(s) readable\n",
          nr);

    while (--nr >= 0) {
        TRACE(traceDispatcher,
              "calling read handler\n");
        dispatcher.systemClock.sync();
        readableFiles[nr]->handle_r();
	readableFiles[nr] = (File *)NULL;
    }

    for (file = head(); file != NULL; file = next(file)) {
        if (issetWrite(file)) {
            ASSERT(nw < MaxFiles);
            writableFiles[nw++] = file;
        }
    }


    TRACE(traceDispatcher,
          "%d file descriptor(s) writable\n",
          nw);

    while (--nw >= 0) {
        TRACE(traceDispatcher,
              "performing callback\n");
        dispatcher.systemClock.sync();
        writableFiles[nw]->handle_w();
	writableFiles[nw] = (File *)NULL;
    }

    return;
}

void
Dispatcher::SignalList::mark(const char* name)
{
    Signal*	signal;
    
    if (isEmpty()) {
        return;
    }

    for (signal = head(); signal != NULL; signal = next(signal)) {
        if (!strcmp(name, signal->name())) {
            TRACE(traceDispatcher,
                  "marking the %s signal handler\n",
                  signal->name());
            signal->pending(true);
        }
    }
}

void
Dispatcher::SignalList::process()
{
    Signal*	signal;
    
    if (isEmpty()) {
        return;
    }

    for (signal = head(); signal != NULL; signal = next(signal)) {
        if (signal->pending()) {
            TRACE(traceDispatcher,
                  "calling the %s signal handler\n",
                  signal->name());
            signal->pending(false);
            signal->handle(NULL);
        }
    }
}
    
void
Dispatcher::loop()
{
    TimeLong	next;
    TimeShort	wait;
    TimeShort	maxWait(8,0);		// 8 second wait
    
    while (1) {
	timers.process();
        signals.process();
        jobs.process();
        signals.process();

        if (!jobs.isEmpty()) {
            wait.set(0);	// Poll
        } else {
            timers.nextTimer(next);
            systemClock.sync();
            wait = next - systemClock;
            if (wait > maxWait) {
                wait = maxWait;
            }
        }
        files.process(wait);
        signals.process();
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
