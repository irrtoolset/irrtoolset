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
// Author(s): Ramesh Govindan

#ifndef _Prefix_hh
#define _Prefix_hh

#include <sys/types.h>
//#include "util/Types.hh"
#include "sys/Address.hh"

// Constants, for debugging
const int MaxPrefixNameLength = 25;

class Prefix {
  private:
    Prefix(const Prefix&);

  public:
    // Create empty prefix
    Prefix();

    // Create 32 bit prefix
    Prefix(Address&);			// In: address corr to prefix

    // Create prefix of specified length
    Prefix(Address&, 			// In: address to base prefix on
           int);			// In: length

    // Duplicate the prefix
    Prefix(Prefix&);			// In: duplicate prefix

    // scan a prefix 
    Prefix(char *);

    ~Prefix();

    // Shorten prefix by some number of bits
    void
    operator--(int = 1);		// In: number of bits

    // Are two prefixes equal
    Boolean
    operator==(Prefix&);

   bool operator!=(Prefix &b) {
      return ! ((*this) == b);
   }

    // Printed representation of prefix
    char*
    name();

    // Mask from mask length
    U32
    mask();

    // Size of host part of prefix
    unsigned int
    numHosts();

    // Random host address in prefix
    void
    randomHost(Address&);		// Out: random in range

    // Make the buddy of this prefix
    void
    buddy();

    // Make the non-buddy neighbor of this prefix
    void
    neighbor();

    // Does the prefix match the address
    Boolean
    match(Address&);			// In: address to match

  private:
    U32		address;
    U32		maskLength;
    char	nameString[MaxPrefixNameLength];
};
    

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
//  scan@isi.edu.
//

#endif _Headers_hh
