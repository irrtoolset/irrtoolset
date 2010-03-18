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

#endif _Headers_hh
