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
// Address.cc
// Author: Ramesh Govindan <govindan@isi.edu>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cerrno>

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

#include "util/Types.hh"
#include "sys/Address.hh"

static u_int masks[] ={ 0x00000000,
                        0x80000000, 0xC0000000, 0xE0000000, 0xF0000000,
                        0xF8000000, 0xFC000000, 0xFE000000, 0xFF000000,
                        0xFF800000, 0xFFC00000, 0xFFE00000, 0xFFF00000,
                        0xFFF80000, 0xFFFC0000, 0xFFFE0000, 0xFFFF0000,
                        0xFFFF8000, 0xFFFFC000, 0xFFFFE000, 0xFFFFF000, 
                        0xFFFFF800, 0xFFFFFC00, 0xFFFFFE00, 0xFFFFFF00,
                        0xFFFFFF80, 0xFFFFFFC0, 0xFFFFFFE0, 0xFFFFFFF0,
                        0xFFFFFFF8, 0xFFFFFFFC, 0xFFFFFFFE, 0xFFFFFFFF 
};

// Operations on IP addresses. IP address are stored in the
// address struct in host byte order, and only the name() operation
// converts it into network byte order before printing it.
//

Address::Address()
{
    address= nameString[0]= 0;
}

Address::Address(const char * const str) {
	address= ntohl(inet_addr(str));
	strcpy(nameString, str);
}

Address::Address(U32 a) : address(a) { 
	nameString[0]= 0;
}

Address::Address(const Address& other) {
	address= other.address;
	memcpy(nameString, other.nameString, MaxAddressNameLength);
}

Boolean
Address::operator<(const Address& other) const
{
    return (address < other.address);
}

Boolean
Address::operator==(const Address& other) const
{
    return (address == other.address);
}

void
Address::operator=(const Address& other)
{
    address = other.address;
    memcpy(nameString, other.nameString, MaxAddressNameLength);
}

void
Address::set(U32 addr)
{
    address = addr;
    nameString[0]= 0;
}

U32
Address::get() const
{
    return address;
}

char*
Address::name() const
{
    if (!nameString[0]) {
        struct in_addr in;
        
        in.s_addr = htonl(address);
        sprintf(nameString,
                "%s", 
                ::inet_ntoa(in));
    }
    return nameString;
}

bool Address::inside(const Address &a, int len) const {
   return (address & masks[len]) == (a.address & masks[len]);
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
