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
// Signal.cc
// Author: Ramesh Govindan <govindan@isi.edu>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cerrno>

extern "C" {
#if HAVE_UNISTD_H
#include <unistd.h>
#endif // HAVE_UNISTD_H

#include <sys/types.h>
#include <signal.h>
}

#include "util/Types.hh"
#include "util/List.hh"
#include "util/Trail.hh"

#include "sys/Signal.hh"
#include "sched/Dispatcher.hh"

extern "C" {
#ifndef STDC_HEADERS
#ifdef HAVE_SIGVEC
extern void sigvec(...);
#endif
#ifdef HAVE_SIGACTION
extern int sigaction(...);
extern int sigemptyset(...);
#endif
#ifdef HAVE_SIGSET
extern int sigset(...);
#endif
#endif
}

// File local stuff
static struct _sigTypeToName {
    int sigtype;
    const char* signame;
} sigTypeToName[] = {
    {SIGTERM, "terminate"},
    {SIGHUP,  "hup"},
    {SIGINT,  "int"},
    {SIGQUIT, "quit"},
    {SIGPIPE, "pipe"},
#ifdef SIGTTOU
    {SIGTTOU, "ttou"},
#endif // SIGTTOU
#ifdef SIGTTIN
    {SIGTTIN, "ttin"},
#endif // SIGTTIN
#ifdef SIGTSTP
    {SIGTSTP, "tstp"},
#endif // SIGTSTP
    {SIGHUP, "hup"},
    {0},
};
static TraceCode traceSignal("signal");

// The Signal class represents a specific signal.
// Usually, the main program:
//	creates one or more Signal objects (of which three types
//		are recognized - terminate, restart, dump)
//	this automatically registers the object with the dispatcher

static void
sysSignalFromKernel(int sigtype)
{
    struct _sigTypeToName	*st;

    for (st = &sigTypeToName[0]; st->sigtype != 0; st++) {
        if (st->sigtype == sigtype) {
            TRACE(traceSignal,
                  "received %s signal, number %d\n",
                  st->signame,
                  st->sigtype);
#ifdef HAVE_SIGSET
            // Re-arm signal
            sigset(sigtype, sysSignalFromKernel);
#endif // HAVE_SIGSET            
            dispatcher.signals.mark(st->signame);
            return;
        }
    }

    ERROR("received unrecognized signal %d\n",
          sigtype);
    return;
}

Signal::Signal(const char* p,
               const Handler* h)
	: ListNode()
{
    struct _sigTypeToName	*st;

    pending_ = false;
    for (st = &sigTypeToName[0]; st->sigtype != 0; st++) {
        if (!strcmp(st->signame, p)) { 
            name_ = st->signame;
            if (h == NULL) {
		goto Ignore;
            }
            handler_ = *h;
            dispatcher.signals.append(this);
            goto Arm;
        }
    }
    ERROR("signal %s not defined on this system, ignoring\n", name_);
    return;

  Ignore:
    {
#ifdef HAVE_SIGACTION
        struct sigaction act;

        TRACE(traceSignal,
              "asking kernel to ignore signal number %d\n",
              st->sigtype);
    
        sigemptyset(&act.sa_mask);
        act.sa_handler = SIG_IGN;
        act.sa_flags = 0;

        if (sigaction(st->sigtype, &act, NULL) < 0) {
            FATAL("sigaction ignore failed on %d: %s\n",
                  st->sigtype,
                  strerror(errno));
        }
#else // HAVE_SIGACTION
        signal(st->sigtype, SIG_IGN);
#endif // HAVE_SIGACTION

        return;
    }

  Arm:
    {
#ifdef HAVE_SIGACTION
        struct sigaction act;

        sigemptyset(&act.sa_mask);
        act.sa_handler = (void (*)(int)) sysSignalFromKernel;
        act.sa_flags = 0;

        TRACE(traceSignal,
              "arming %s signal\n",
              name_);
        
        if (sigaction(st->sigtype, &act, NULL) < 0) {
            FATAL("sigaction failed on %d: %s\n",
                  st->sigtype,
                  strerror(errno));
        }
#endif // HAVE_SIGACTION

#ifdef HAVE_SIGVEC
        struct sigvec	vec, ovec;

        memset((char*) &vec, 0, sizeof(vec));
        vec.sv_handler = sysSignalFromKernel;

        TRACE(traceSignal,
              "arming %s signal\n",
              name_);
        if (sigvec(st->sigtype, &vec, &ovec) < 0) {
            FATAL("sigvec failed on %d: %s\n",
                  st->sigtype,
                  strerror(errno));
        }
#endif // HAVE_SIGVEC

#ifdef HAVE_SIGSET
        TRACE(traceSignal,
              "arming %s signal\n",
              name_);
        sigset(st->sigtype, sysSignalFromKernel);
#endif // HAVE_SIGSET

        return;
    }
}

Signal::~Signal() {
    dispatcher.signals.remove(this);
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
