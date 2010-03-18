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

// Local Variables:
// outline-regexp: "^[a-zA-Z0-9\*]+$\n[a-zA-Z0-9]+[:][:]\\|^inline [a-zA-Z0-9\*]+\n[a-zA-Z0-9]+[:][:]\\|[a-zA-Z0-9]+[:][:]\\|^struct\\|^class"
// outline-heading-end-regexp: "\{\n"
// End:

#endif // _Rim_Msg_hh
