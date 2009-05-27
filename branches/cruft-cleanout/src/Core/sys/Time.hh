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
// Time.hh
// Author(s): Ramesh Govindan

#ifndef _Time_hh
#define _Time_hh

#include "util/Types.hh"

// Forward definitions
class TimeLong;
class TimeShort;

// We use the NTP format for representing time.
//
// Time differences are usually represented 
// in a "short" format, with a 16 bit integral part and a
// 16 bit fractional part.
//
// Absolute time is usually represented in a "long" format
// using 32 bits for the integer part and 32 for the fractional part.
//

template <class T>
class Time {
  public:
    void
    operator=(const Time<T> y) {
        time = y.time;
    };

    Boolean
    operator==(const Time<T> y) const {
        return (time == y.time);
    };

    Boolean
    operator!=(const Time<T> y) const {
        return (time != y.time);
    };

    Boolean
    operator>(const Time<T> y) const {
        return (time > y.time);
    };

    Boolean
    operator<(const Time<T> y) const {
        return (time < y.time);
    };

    void
    operator>>=(int i) {
        time >>= i;
    }

    void
    operator<<=(int i) {
        time <<= i;
    }

    void
    operator+=(Time<T> y) {
        time += y.time;
    }

    void
    operator-=(Time<T> y) {
        time = (time > y.time) ? (time - y.time) : 0;
    }

    void
    operator*=(float x) {
        time = (T) (time * x);
    }

    T	time;
};

class TimeShort : public Time<U32> {
    friend class TimeLong;
    
  public:
    // Create empty time instance
    TimeShort();

    // Create time instance given a 32-bit value
    TimeShort(U32);

    // Create time instance given seconds and fraction
    TimeShort(U16 i,
              U16 f);

    // Convert to long format
    TimeLong
    lengthen();
    
    // Extract the seconds part
    U16
    seconds();

    // Extract the fractional part
    U16
    fraction();

    // Return the number of microseconds
    U32
    usecs();

    // Return the NTP representation
    U32
    get() const;
    
    // Set the integer and fractional part
    void
    set(U16, U16);
    
    // Set our value to the number of microseconds in the argument
    void
    set(U32 usecs);
};

class TimeLong : public Time<U64> {
    friend class TimeShort;
    
  public:
    // Create an empty time instance
    TimeLong();

    // Create an instance given the fixed point representation
    TimeLong(U64 t);

    // Create an instance given the components
    TimeLong(U32 i, U32 f);

    // Subtract a smaller time
    TimeShort
    operator-(TimeLong y);

    // Add a smaller time
    TimeLong
    operator+(TimeShort y);
    
    // Synchronize with operating system clock
    void
    sync();
    
    // Extract the seconds part
    U32
    seconds();

    // Extract the fractional part
    U32
    fraction();

    // Return the fixed point representation
    U64
    get() const;
    
    // Set the components
    void
    set(U32 secs, U32 frac);

    // Convert to operating system time representation
    void
    systemTime(struct timeval* tv);
};

// Constants
static const TimeShort	ZeroInterval(0, 0);
static const TimeShort 	InfiniteInterval(0xffff, 0xffff);
static const TimeLong	ZeroTime(0, 0);
static const TimeLong	InfiniteTime(0xffffffff, 0xffffffff);

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
//  Author(s): Ramesh Govindan <govindan@isi.edu>

#endif // _system_hh
