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
// Trail.hh
// Author(s): Ramesh Govindan
//
// Tracing code points.

#ifndef _Trail_hh
#define _Trail_hh

#include "util/Types.hh"
#include "util/List.hh"
#include "sys/File.hh"
#include "sys/Time.hh"

enum TraceStatus {
    TraceStatusUnknown,
    TraceStatusDisabled,
    TraceStatusEnabled
};

class EnabledTrace : public ListNode {
private:
    EnabledTrace(const EnabledTrace&);
public:
    EnabledTrace(char*);		// Name of trace to enable
    ~EnabledTrace();

    char*		name;
};

struct TrailConfig {
    Boolean		printFuncs;	// For stderr tracing only
    Boolean		printTimeStamp;	// For stderr tracing only
    Boolean		background;	// Don't trace to foreground
    int			logLevel;	// Which syslog local level
    List<EnabledTrace>	enabled;	// List of enabled traces
};


// Classes defined elsewhere
//class DiskFile;

// A Trail object encapsulates the interface to the system
// tracing and logging facilities. There is exactly one such
// object in the system.
//
// A TraceCode instance represents a sequence of related code points
// that need to be traced together. Each trace instance is identified
// by a unique name. Traces can be selectively enabled or disabled
// from the command line

class Trail {
  public:
    // Create the system wide trail object
    Trail(char *programName);

    // Destroy the trail object
    ~Trail();
    
    class Trace {
      public:
        // Create a new trace instance
        Trace(const char*);		// In: unique name of trace

        // Delete a trace instance
        ~Trace();

        // Enable this trace
        void
        enable();

        // Disable this trace
        void
        disable();

        // cengiz added the following
        Boolean
        isEnabled();
        // Is this trace enabled?
        Boolean
        isDisabled();
        operator int() {
	   return isEnabled();
	}

        // Is the status of this unknown?
        Boolean
        isUnknown();

        // Return name of trace code
        char*			// Out: pointer to name
        name();

      private:
        TraceStatus		status;
        char*			nameString;

        // cengiz added the following
        inline void makeStatusKnown();
    };

    // Trace code point
    void
    trace(const char*,		// In: Name of func
          Trace&,		// In: Code corresponding to trace pt
          const char*,		// In: printf style format string
          ...);			// In: variable args

    // Log a message in syslog
    void
    log(Boolean,		// In: are we logging fatal errors
        const char*,		// In: Name of func
        const char*,		// In: printf style format string
        ...);			// In: variable args

  private:
    char*		buffer;
    TimeLong		start;
};

// Syntactic sugar
typedef Trail::Trace TraceCode;

extern Trail *systemTrail;
extern TrailConfig trailConfig;
extern char* programName;

// Trace a code point
#ifdef HAVE_TRACING
#define TRACE(code, format, args...)					\
    systemTrail->trace(__PRETTY_FUNCTION__, (code), (format) ,##args)
#else
#define TRACE(code, format, args...)
#endif // HAVE_TRACING

// A fatal error has occurred
#define FATAL(format, args...)						\
do {									\
    systemTrail->log(true, __PRETTY_FUNCTION__, (format) , ## args);	\
    closePidFile();							\
    abort();								\
} while(0)

// A recoverable error has occurred
#define ERROR(format, args...)						\
do {									\
    systemTrail->log(false, __PRETTY_FUNCTION__, (format) , ## args);	\
} while(0)

// Assert program invariant
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(bool)							\
do {									\
    if (!(bool)) {							\
        systemTrail->log(true, __PRETTY_FUNCTION__,			\
                           "assertion failure in file %s at line %d",	\
                           __FILE__,					\
                           __LINE__);					\
	closePidFile();							\
        abort();							\
    }									\
} while (0)
    
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

#endif // _Trail_hh
