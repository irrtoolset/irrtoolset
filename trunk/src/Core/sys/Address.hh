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

#endif // _Address_hh
