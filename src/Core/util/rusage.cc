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
//  Permission to use, copy, modify, and distribute this software and its
//  documentation in source and binary forms for lawful non-commercial
//  purposes and without fee is hereby granted, provided that the above
//  copyright notice appear in all copies and that both the copyright
//  notice and this permission notice appear in supporting documentation,
//  and that any documentation, advertising materials, and other materials
//  related to such distribution and use acknowledge that the software was
//  developed by the University of Southern California, Information
//  Sciences Institute. The name of the USC may not be used to endorse or
//  promote products derived from this software without specific prior
//  written permission.
//
//  THE UNIVERSITY OF SOUTHERN CALIFORNIA DOES NOT MAKE ANY
//  REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY
//  PURPOSE.  THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
//  TITLE, AND NON-INFRINGEMENT.
//
//  IN NO EVENT SHALL USC, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT, TORT,
//  OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH, THE USE
//  OR PERFORMANCE OF THIS SOFTWARE.
//
//  Questions concerning this software should be directed to
//  ratoolset@isi.edu.
//
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include "config.h"
#include "rusage.hh"

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
extern int getrusage(...);
extern int getpagesize(...);
extern int gettimeofday(...);
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

    stream.form("  Resource Usage: \n");
    stream.form("     times:    %1.2fu %1.2fs %1.2fr\n", utime, stime, rtime);
    stream.form("     i/o:      %d %d\n", self.ru_inblock, self.ru_oublock);
    stream.form("     faults:   %d %d\n", self.ru_minflt, self.ru_majflt);
    stream.form("     swaps:    %d\n", self.ru_nswap);
    stream.form("     max size: %d * %d\n", self.ru_maxrss, getpagesize());
    stream.form("     ws size:  %d\n", self.ru_idrss);
    stream.form("     signals:  %d\n", self.ru_nsignals);
    stream.form("     vo/nv cs: %d %d\n", self.ru_nvcsw, self.ru_nivcsw);

    return stream;
}

#else // RUSAGE_USES_TIMEVAL && HAVE_TIMEVAL

void Rusage::start() {
}

ostream& operator<<(ostream& stream, Rusage& ru) {
   stream << "No resource usage is available in this system.\n";
   return stream;
}

#endif // RUSAGE_USES_TIMEVAL && HAVE_TIMEVAL
