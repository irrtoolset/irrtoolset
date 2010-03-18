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
