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
// Types.hh
// Author(s): Ramesh Govindan

#ifndef _Types_hh
#define _Types_hh

#include <cstdio>
#include <cstdlib>
#include <math.h>

extern "C" {
#include <sys/types.h>
#include <netinet/in.h>

#define __XOPEN_SOURCE
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif    

#include <string.h>
#if NEED_MEMORY_H
#include <memory.h>
#endif // NEED_MEMORY_H

#ifndef STDC_HEADERS
// extern int bzero(...);
// extern int bcopy(...);
#endif // STDC_HEADERS
}

// Syntactic sugar
typedef unsigned long long u_longlong;

// Typedefs that explicitly encode word length
typedef u_char U8;
typedef u_short U16;
typedef u_long U32;
typedef u_longlong U64;

// Other common typedefs; syntactic sugar
typedef bool Boolean;
typedef u_short Port;

// A ternary type for certain tests
enum Ternary {None, One, Two};

// A 4-valued type for other tests
enum Quad {none, one, two, three};

// Generic constants
const int Nbby = 8;		// Number of bits per byte

// Format conversion macros
#ifdef WORDS_BIGENDIAN

static inline U64
ntohll(U64& lw)
{
	return lw;
}

static inline U64
htonll(U64& lw)
{
	return lw;
}

#else // not BIGENDIAN 

static inline U64
ntohll(U64& lw)
{
    char*	p;
    char	t;
    
    p = (char *) &lw;
    t = p[0]; p[0] = p[7]; p[7] = t;
    t = p[1]; p[1] = p[6]; p[6] = t;
    t = p[2]; p[2] = p[5]; p[5] = t;
    t = p[3]; p[3] = p[4]; p[4] = t;

    return lw;
}

static inline U64
htonll(U64& lw)
{
    char*	p;
    char	t;
    
    p = (char *) &lw;
    t = p[0]; p[0] = p[7]; p[7] = t;
    t = p[1]; p[1] = p[6]; p[6] = t;
    t = p[2]; p[2] = p[5]; p[5] = t;
    t = p[3]; p[3] = p[4]; p[4] = t;

    return lw;
}

#endif // not BIGENDIAN

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

#endif // _Types_hh
