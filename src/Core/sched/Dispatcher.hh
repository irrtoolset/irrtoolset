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
// Dispatcher.hh
// Author: Ramesh Govindan <govindan@isi.edu>
//
// Machine-independent co-routine scheduler.

#ifndef _Dispatcher_hh
#define _Dispatcher_hh

#include "sys/Time.hh"
#include "sys/FileSet.hh"


#include "sched/Job.hh"
#include "sched/Timer.hh"
#include "sys/Signal.hh"
// Classes defined elsewhere
//  class Timer;
//  class Job;
//  class Signal;

// This central scheduler arbitrates between timer expiration
// requests, requests to perform a "job", and I/O handling.
// There is exactly one of these in the system.

class Dispatcher {
  public:
    // Create a dispatcher
    Dispatcher();
   ~Dispatcher() {
      timers.clear();
      jobs.clear();
      signals.clear();
      files.clear();
   }

    // The central scheduler function, never returns
    void loop();
    
    TimeLong		systemClock;

    // Time sorted list of system timers
    class TimerList : public SortedList<Timer> {
      friend class Dispatcher;

      public:
        // Handle all expired timers
        void process();

        // The smallest expiration time among all timers
        void nextTimer(TimeLong&);		// Out: expiration time
    } timers;

    // FIFO queue of one-shot jobs
    class JobList : public List<Job> {
      friend class Dispatcher;

      public:
        // Call back the function associated with the first job 
        void process();
    } jobs;

    // List of signals that we're interested in
    class SignalList : public List<Signal> {
      friend class Dispatcher;

      public:
        // A signal was received, mark it
        void mark(const char*);

        // Process any marked signals
        void process();
    } signals;

    // Collection of open file descriptors in the system
    class FileList : public FileSet {
      friend class Dispatcher;

      public:
        // Process any file descriptors that are readable or writable
        void process(TimeShort&);		// In: time to wait in select()
    } files;

};

extern Dispatcher dispatcher;
extern TimeLong& sysClock;

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

// Local Variables:
// outline-regexp: "^[a-zA-Z0-9\*]+$\n[a-zA-Z0-9]+[:][:]\\|^inline [a-zA-Z0-9\*]+\n[a-zA-Z0-9]+[:][:]\\|[a-zA-Z0-9]+[:][:]\\|^struct\\|^class"
// outline-heading-end-regexp: "\{\n"
// End:

#endif // _Rim_Msg_hh
