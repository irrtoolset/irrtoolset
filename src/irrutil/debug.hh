//  $Id: debug.hh 215 2008-09-17 00:36:52Z shane $
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
//
//  Copyright (c) 1994 by the University of Southern California
//  All rights reserved.
//
//  Permission to use, copy, modify, and distribute this software and its
//  documentation in source and binary forms for lawful non-commercial
//  purposes and without fee is hereby granted, provided that the above
//  copyright notice appear in all copies and that both the copyright
//  notice and this permission notice appear in supporting documentation,
//  and that any documentation, advertising materials, and other materials
//  related to such distribution and use acknowledge that the software was
//  developed by the University of Southern California, Information
//  Sciences Institute. The name of the USC may not be used to endorse or
//  promote products derived from this software without specific prior
//  written permission.
//
//  THE UNIVERSITY OF SOUTHERN CALIFORNIA DOES NOT MAKE ANY
//  REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY
//  PURPOSE.  THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
//  TITLE, AND NON-INFRINGEMENT.
//
//  IN NO EVENT SHALL USC, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT, TORT,
//  OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH, THE USE
//  OR PERFORMANCE OF THIS SOFTWARE.
//
//  Questions concerning this software should be directed to
//  ratoolset@isi.edu.

// Copyright (C) 1993 Cengiz Alaettinoglu <ca@cs.umd.edu>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You didn't receive a copy of the GNU General Public License along
// with this program; so, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
//
// Author(s): Cengiz Alaettinoglu <ca@cs.umd.edu>
//            Computer Science Department
//            University of Maryland
//            College Park, MD 20742

#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"
#ifdef ENABLE_DEBUG
#include <iostream>

#define DBG_ERR             1
#define DBG_INFO            2 

void Abort();
void copy_constructor(const char *p);

class dbgstream {
public:
   dbgstream();
   dbgstream(dbgstream& a) { copy_constructor("dbgstream"); }
   int operator[](int _level) { 
      level = _level;
      return enabledp();
   }
   void enable();
   void disable();
   void enable(int level);
   void disable(int level);
   int  enabledp();
   int  enabledp(int level);
private:
   int level;
   int enabled;
};

extern dbgstream dbg;

//----------------------------------------------------------------------
// ASSERT
//      If condition is false,  print a message and dump core.
//	Useful for documenting assumptions in the code.
//
//	NOTE: needs to be a #define, to be able to print the location 
//	where the error occurred.
//----------------------------------------------------------------------
/*
#ifndef ASSERT
#define ASSERT(condition)                                                     \
    if (!(condition)) {                                                       \
        cerr << __FILE__ << ":" << __LINE__                                   \
	     << ": Assertion failed `" << #condition << "'\n";                \
        Abort();                                                              \
    }
#endif // ASSERT
*/

#define Debug(whatever) whatever
#define Channel(no) if (dbg.enabledp(no)) cerr
#define PRINT(var) cout << #var << " = " << var << endl;




#else /* ENABLE_DEBUG */
#define copy_constructor(p) 
#ifndef ASSERT
#define ASSERT(condition) /* do nothing */
#endif // ASSERT
#define Debug(whatever)   /* do nothing */
#define PRINT(var)        /* do nothing */

#endif /* ENABLE_DEBUG */

#endif /* DEBUG_H */

#ifdef DEBUG_MEMORY
#define CLASS_DEBUG_MEMORY_HH(classX)        \
   void* operator new(size_t size) {         \
      classX::count++;                             \
      return ::operator new(size);           \
   }                                         \
   void operator delete(void* p) {           \
      classX::count--;                             \
      ::delete(p);                           \
   }                                         \
   static int count                         
#define CLASS_DEBUG_MEMORY_CC(classX) \
   int classX::count = 0
#define CLASS_DEBUG_MEMORY_PRINT(classX) \
   dbg[DBG_INFO] << #classX << " remaining: " << classX::count << "\n"
#else // DEBUG_MEMORY
#define CLASS_DEBUG_MEMORY_HH(classX)
#define CLASS_DEBUG_MEMORY_CC(classX)
#define CLASS_DEBUG_MEMORY_PRINT(classX)
#endif // DEBUG_MEMORY
