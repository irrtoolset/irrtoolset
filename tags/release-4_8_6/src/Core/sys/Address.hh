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
// Address.hh
// Author(s): Ramesh Govindan

#ifndef _Address_hh
#define _Address_hh

extern "C" {
#include <stdio.h>
   //#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
}
#include "util/Types.hh"

// Constants, for debugging
const int MaxAddressNameLength = 20;

// IP addresses: For now, this object describes IPv4
// addresses. 

class Address {
  public:
    // Create a new address instance
    Address();
    Address(const char * const str);
    Address(U32);
    Address(const Address&); // copy constructor	

    // Comparison: for IPv4, does a numeric comparison
    Boolean
    operator<(const Address&) const;	// In: address

    // Equality test
    Boolean
    operator==(const Address&) const;	// In: address
    Boolean
    operator!=(const Address&b) const {	// In: address
       return ! ((*this) == b);
    }

    // Assignment
    void
    operator=(const Address&);	// In: address

    // Set the value of
    void
    set(U32);			// In: Set to a 32-bit value

    // Get the value of
    U32				// Out: 32-bit IPv4 address
    get() const;

    // Get the textual representation of
    char*			// Out: dotted decimal representation
    name() const;

    Boolean classAAddress() const {
	return ((address & 0x80000000) == 0);
    }
    Boolean classBAddress() const {
	return ((address & 0xc0000000) == 0x80000000);
    }
    Boolean classCAddress() const {
	return ((address & 0xe0000000) == 0xc0000000);
    }
    Boolean classDAddress() const {
	return ((address & 0xf0000000) == 0xe0000000);
    }
    bool inside(const Address &a, int len) const;
    operator U32() const {
       return address;
    }
  private:
    U32 address;
    mutable char nameString[MaxAddressNameLength];
};

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

#endif // _Address_hh
