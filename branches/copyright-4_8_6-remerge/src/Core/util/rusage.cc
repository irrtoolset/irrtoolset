//  $Id$
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
//
//  Copyright (c) 1994 by the University of Southern California
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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include "config.h"
#include <ostream>
#include "rusage.hh"
#include <iomanip>

using namespace std;

#if RUSAGE_USES_TIMEVAL && HAVE_TIMEVAL && HAVE_GETTIMEOFDAY

extern "C" {
#include <sys/types.h>

#if TIME_WITH_SYS_TIME
#   include <sys/time.h>
#   include <time.h>
#else
#   if HAVE_SYS_TIME_H
#      include <sys/time.h>
#   else
#      include <time.h>
#   endif
#endif

#include <sys/resource.h>
//extern int getrusage(...);
//extern int gettimeofday(...);
#ifdef HAVE_UNISTD_H
#include <unistd.h>
//extern int getpagesize(...);
#endif
}

double tv2f(timeval &tv)
/* Converts a timeval into a double giving the time in seconds. */
{
    return tv.tv_sec + tv.tv_usec / 1e6;
}

void Rusage::start() {
    struct timeval start_time;
    struct rusage self;

    gettimeofday(&start_time, NULL);
    last_rtime = tv2f(start_time);

    getrusage(RUSAGE_SELF, &self);
    last_utime = tv2f(self.ru_utime);
    last_stime = tv2f(self.ru_stime);
}

ostream& operator<<(ostream& stream, Rusage& ru) {
    struct timeval end_time;
    struct rusage self;
    double rtime, utime, stime;

    getrusage(RUSAGE_SELF, &self);
    gettimeofday(&end_time, NULL);

    utime = tv2f(self.ru_utime) - ru.last_utime;
    stime = tv2f(self.ru_stime) - ru.last_stime;
    rtime = tv2f(end_time)      - ru.last_rtime;

    stream << setiosflags(ostream::fixed) << setprecision(2);
    stream <<  "     times:    "
           << utime << " "
           << stime << " "
           << rtime << endl;
    stream << "     i/o:      "
           << self.ru_inblock << " "
           << self.ru_oublock << endl;
    stream << "     faults:   "
           << self.ru_minflt << " "
           << self.ru_majflt << endl;
    stream << "     swaps:    "
           << self.ru_nswap << endl;
    stream << "     max size: "
           << self.ru_maxrss << " "
           << getpagesize() << endl;
    stream << "     ws size:  "
           << self.ru_idrss << endl;
    stream << "     signals:  "
           << self.ru_nsignals << endl;
    stream << "     vo/nv cs: "
           << self.ru_nvcsw << " "
           << self.ru_nivcsw << endl;
    stream << setiosflags(ostream::scientific) << setprecision(0);
    return stream;
}

#else // RUSAGE_USES_TIMEVAL && HAVE_TIMEVAL

void Rusage::start() {
    return;
}

ostream& operator<<(ostream& stream, Rusage& ru) {
   stream << "No resource usage is available in this system.\n";
   return stream;
}

#endif // RUSAGE_USES_TIMEVAL && HAVE_TIMEVAL
