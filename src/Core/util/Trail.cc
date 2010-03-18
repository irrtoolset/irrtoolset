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
// Trail.cc
// Author: Ramesh Govindan <govindan@isi.edu>
//
// Tracing implementation

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "util/Types.hh"
#include "util/Trail.hh"
#include "sched/Dispatcher.hh"

extern "C" {
#include <sys/types.h>
#include <syslog.h>
#include <string.h>    

#ifndef STDC_HEADERS
extern void openlog(const char*, int, int);
extern void syslog(int, const char*, ...);
extern void closelog(void);
extern int setlinebuf(...);
#endif
}

#ifndef LOG_LOCAL0
#define LOG_LOCAL0	(16 << 3)
#endif // LOG_LOCAL0

// Globals
Trail*		systemTrail = NULL;
TrailConfig	trailConfig;

// Constants
const int	MaxTraceBufferSize  = 1024;

EnabledTrace::EnabledTrace(char* n)
        : ListNode()
{
    name = new char[strlen(n)+1];
    strcpy(name, n);
    return;
}

EnabledTrace::~EnabledTrace()
{
    delete [] name;
}

Trail::Trail(char *programName)
{
    buffer = new char[MaxTraceBufferSize];
    dispatcher.systemClock.sync();
    start = dispatcher.systemClock;

    // Open the log if necessary
    if (trailConfig.background) {
        int	l;
        
        l = LOG_LOCAL0;
        if (trailConfig.logLevel <= 7) {	// Is this kosher?
            l += trailConfig.logLevel;
        }
	trailConfig.logLevel = l;
        openlog(programName, LOG_PID | LOG_NDELAY, 
		trailConfig.logLevel);
    }
    return;
}

Trail::~Trail()
{
   delete [] buffer;
   trailConfig.enabled.clear();
   closelog();
}

void
Trail::trace(const char *func,
             TraceCode& code,
             const char *fmt,
             ...)
{
    va_list ap;
    
    // cengiz 
    // return immediately if this trace is disabled
    if (code.isDisabled()) {
        return;
    }

    // it is a bug to return w/o va_end after a va_start
    va_start(ap, fmt);
#ifdef HAVE_VSNPRINTF
    (void) vsnprintf(buffer, MaxTraceBufferSize, fmt, ap);
#else
    (void) vsprintf(buffer, fmt, ap);
#endif
    va_end(ap);
    if (trailConfig.background) {
        syslog(trailConfig.logLevel | LOG_INFO, 
               "(%s) %s", code.name(), buffer);
    } else {
        if (trailConfig.printTimeStamp) {
	   /*	   
	   time_t t = time(NULL);
	   struct tm *tm = localtime(&t);
  
	   fprintf(stderr,
		   "%02d/%02d/%04d %02d:%02d:%02d ", 
		   tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900,
		   tm->tm_hour, tm->tm_min, tm->tm_sec);
	   */
	   /*
            TimeShort	diff;
        
            dispatcher.systemClock.sync();
            diff = dispatcher.systemClock - start;
            (void) fprintf(stderr,
                           "(%u,%u) ",
                           (u_int) diff.seconds(),
                           (u_int) diff.fraction());
	   */
        }
        (void) fprintf(stderr, "(%s) ", code.name());
        (void) fprintf(stderr, "%s", buffer);
        if (trailConfig.printFuncs) {
            (void) fprintf(stderr, "\t\t%.40s\n", func);
        }
    }
    (void) fflush(stderr);
    return;
}

void
Trail::log(Boolean fatal,
           const char* func,
           const char* fmt,
           ...)
{
    va_list ap;
    
    va_start(ap, fmt);
#ifdef HAVE_VSNPRINTF
    (void) vsnprintf(buffer, MaxTraceBufferSize, fmt, ap);
#else
    (void) vsprintf(buffer, fmt, ap);
#endif
    va_end(ap);
    if (trailConfig.background) {
        (void) syslog(trailConfig.logLevel | 
                      ((fatal) ? LOG_CRIT : LOG_WARNING), 
                      "%s\t\t%.40s%s\n", buffer, func);
    } else {
        (void) fprintf(stderr, "ERROR in %s: %s", func, buffer);
        (void) fflush(stderr);
    }
    return;
}

TraceCode::Trace(const char* str)
{
    nameString = new char[strlen(str)+1];
    strcpy(nameString, str);
    status = TraceStatusUnknown;
}

TraceCode::~Trace()
{
  // Modified by wlee
  // delete nameString;
  delete []nameString;
}

void
TraceCode::enable()
{
    status = TraceStatusEnabled;
}

void
TraceCode::disable()
{
    status = TraceStatusDisabled;
}

inline void TraceCode::makeStatusKnown() {
   if (!isUnknown())
      return;

   for (EnabledTrace *et = trailConfig.enabled.head(); 
        et != NULL; 
        et = trailConfig.enabled.next(et))
      if (!strcmp(name(), et->name)) {
	 enable();
	 return;
      }
   disable();
   return;
}

Boolean
TraceCode::isDisabled()
{
    // cengiz modified the following
    if (status == TraceStatusDisabled)
       return true;
    makeStatusKnown();
    
    return (status == TraceStatusDisabled);
}

Boolean
TraceCode::isEnabled()
{
    // cengiz modified the following
    if (status == TraceStatusEnabled)
       return true;
    makeStatusKnown();
    
    return (status == TraceStatusEnabled);
}

Boolean
TraceCode::isUnknown()
{
    return (status == TraceStatusUnknown);
}

char*
TraceCode::name()
{
    return nameString;
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
//  Author(s): Ramesh Govindan <govindan@isi.edu>

