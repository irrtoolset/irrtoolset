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

   //extern void* memalign(...);

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

#endif // _Types_hh
