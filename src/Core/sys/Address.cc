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
