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
// system.cc
// Author: Ramesh Govindan <govindan@isi.edu>
//
// Abstracted OS facilities for file system access and
// for communication primitives. This file contains implementations of:
//	- network addresses (Address class)
//	- OS file descriptors and descriptor sets
//	- a common time representation
//

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
#include <sys/time.h>
#include <sys/resource.h>
}
#include "sys/FileSet.hh"
#include "util/Types.hh"
#include "util/List.hh"
#include "util/Trail.hh"


//#include "sched/Dispatcher.hh"

extern "C" {
#ifndef STDC_HEADERS
extern int getrlimit(...);    
extern int select(...);
#endif
}

// File local variables
static TraceCode traceFileSet("file_set");

// The FileSet construct hides the internal details of
// UNIX descriptor sets.

FileSet::FileSet()
	: List<File>()
{
    TRACE(traceFileSet,
          "creating new file set");
    
    FD_ZERO(&readDescriptors);
    FD_ZERO(&writeDescriptors);

#ifdef RLIMIT_NOFILE
    struct rlimit rl;
    
    (void) getrlimit(RLIMIT_NOFILE,
                     &rl);
    fdWidth = rl.rlim_cur;    // Current limit, assume we don't set it
#else // RLIMIT_NOFILE
#ifndef	NOFILE
#ifdef	_NFILE
#define	NOFILE	_NFILE
#else	// _NFILE 
#ifdef	OPEN_MAX
#define	NOFILE	OPEN_MAX
#else	// OPEN_MAX 
#define	NOFILE	20
#endif	// OPEN_MAX 
#endif	// _NFILE 
#endif	// NOFILE 
    fdWidth = NOFILE;
#endif // RLIMIT_NOFILE
}

FileSet::~FileSet()
{
    // Empty
}

void
FileSet::inset(File* file)
{
    TRACE(traceFileSet,
          "adding new descriptor %d to file set",
          file->descriptor());
    
    if (file->mode() != FileModeReadOnly) {
        if (file->has_wh()) {
            FD_SET(file->descriptor(), &writeDescriptors);
        }
    }
    if (file->has_rh()) {
        FD_SET(file->descriptor(), &readDescriptors);
    }
    append(file);
}

void
FileSet::outset(File* file)
{
    TRACE(traceFileSet,
          "removing descriptor %d from file set",
          file->descriptor());
    
    remove(file);
    if (file->mode() != FileModeReadOnly) {
        FD_CLR(file->descriptor(),
               &writeDescriptors);
    }
    FD_CLR(file->descriptor(),
           &readDescriptors);
    return;
}

Boolean
FileSet::issetRead(File* file)
{
    TRACE(traceFileSet,
          "testing if descriptor %d is readable",
          file->descriptor());
    
    return FD_ISSET(file->descriptor(),
                    &lastRead);
}

Boolean
FileSet::issetWrite(File* file)
{
    TRACE(traceFileSet,
          "testing if descriptor %d is writable",
          file->descriptor());
    
    return FD_ISSET(file->descriptor(),
                     &lastWrite);
}

int
FileSet::select(TimeShort& interval)
{
    struct timeval tv;
    TimeLong intv;
    int retval;

    TRACE(traceFileSet,
          "selecting on file set");
    
    intv = interval.lengthen();
    intv.systemTime(&tv);
    lastRead = readDescriptors;
    lastWrite = writeDescriptors;
    retval = ::select(fdWidth,
                      &lastRead,
                      &lastWrite,
                      NULL,
                      &tv);
    if (retval < 0) {
        switch (errno) {
            case EINTR:
                return FileOpSoftError;
            default:
                return FileOpHardError;
        }
    }
    return retval;
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
