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
//  Author(s): Ramesh Govindan <govindan@isi.edu>

