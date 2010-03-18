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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

extern "C" {
#include <string.h>
#if NEED_MEMORY_H
#include <memory.h>
#endif // NEED_MEMORY_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>    
}

#include "gnu/ACG.h"
#include "gnu/RndInt.h"
#include "util/Types.hh"
#include "util/Trail.hh"
#include "util/Handler.hh"
#include "util/Buffer.hh"
#include "sys/Address.hh"
#include "network/Prefix.hh"
#include "sched/Dispatcher.hh"

// Constants
static const	int	maxSignedInt = 0x7fffffff;

// File locals
ACG		*acg = NULL;
RandomInteger	*rndInt = NULL;

Prefix::Prefix()
{
    address = 0;
    maskLength = 0;
    (void) memset(nameString, 0, MaxPrefixNameLength);
}

Prefix::Prefix(Address& a)
{
    address = a.get();
    maskLength = 32;
    (void) memset(nameString, 0, MaxPrefixNameLength);
}

Prefix::Prefix(char *text) {
   unsigned int i1, i2, i3, i4, i5;
   char slash;

   sscanf(text, "%u.%u.%u.%u%c%u", &i1, &i2, &i3, &i4, &slash, &i5);
   address = i4 + (i3 << 8) + (i2 << 16) + (i1 << 24);
   if (slash == '/')
      maskLength  = i5;
   else
      if (i4 == 0)
	 if (i3 == 0)
	    if (i2 == 0)
	       if (i1 == 0)
		  maskLength = 0;
	       else
		  maskLength = 8;
	    else
	       maskLength = 16;
	 else
	    maskLength = 24;
      else
	 maskLength = 32;

   in_addr_t _mask;
   if (maskLength == 0)
      _mask = 0;
   else 
      _mask = ~ (0u) << (32 - maskLength);
  
   address &= _mask;
}

Prefix::Prefix(Address& a, 
               int l)
{
    U32 	baddr;

    ASSERT((l >= 0) && (l <= 30));	// Note upper bound on length

    address = a.get();
    maskLength = l;
    (void) memset(nameString, 0, MaxPrefixNameLength);

    address &= mask();
    return;
}

Prefix::Prefix(Prefix& p)
{
    address = p.address;
    maskLength = p.maskLength;
    return;
}

Prefix::~Prefix()
{
    // Empty
}

void
Prefix::operator--(int j)
{
    ASSERT(j < sizeof(address) * 8);	// NBBY?
    maskLength -= 1;
    address &= mask();
    return;
}

Boolean
Prefix::operator==(Prefix& pfx)
{
    return ((maskLength == pfx.maskLength) &&
            (address == pfx.address));
}

U32
Prefix::mask()
{
    return ~((0x1 << (32 - maskLength)) - 1);
}

unsigned int
Prefix::numHosts()
{
    return ((0x1 << (32 - maskLength)) - 1);
}

char*
Prefix::name()
{
    struct in_addr	in;

    if (!nameString[0]) {
        in.s_addr = htonl(address);
        sprintf(nameString, "%s/%d", ::inet_ntoa(in), maskLength);
    }
    return nameString;
}

void
Prefix::randomHost(Address& addr)
{
    unsigned int	hostNum;

    if (acg == NULL) {
        dispatcher.systemClock.sync();
        acg = new ACG(dispatcher.systemClock.seconds());
        rndInt = new RandomInteger(0, maxSignedInt, acg);
    }

    while (!(hostNum = rndInt->asLong(numHosts()))) {
        // Empty
    }
    addr.set(address + hostNum);
    return;
}

void
Prefix::buddy()
{
    U32		base;

    base = (address >> (32 - maskLength));
    if (base & 0x1) {
        base -= 1;
    } else {
        base += 1;
    }
    address = (base << (32 - maskLength));
    return;
}

void
Prefix::neighbor()
{
    U32		base;

    base = (address >> (32 - maskLength));
    if (base & 0x1) {
        base += 1;
    } else {
        base -= 1;
    }
    address = (base << (32 - maskLength));	// XXX: wrap around?
    return;
}

Boolean
Prefix::match(Address& a)
{
    return ((a.get() & mask()) == address);
}

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


