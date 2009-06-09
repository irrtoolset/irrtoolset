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
// Buffer.hh
// Author(s): Ramesh Govindan
//            WeeSan Lee
//            Cengiz Alaettinoglu

#ifndef _Buffer_hh
#define _Buffer_hh

#include <cstdlib>
#include <cstring>
#include <ostream>
extern "C" {
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif // HAVE_MALLOC_H
}

#include "rpsl/List.hh"

const int BufferExtendIncrement	= 4 * 1024;

// Buffer containing data and length
class Buffer : public ListNode {
  friend std::ostream &operator<<(std::ostream &os, const Buffer &b);

  public:
    // Initialize a caller allocated buffer
    Buffer() : ListNode() {
        contents = (char *)NULL;
        capacity = 0;
        size = 0;
        offset = 0;
        callerAllocated = false;
    }
    Buffer(const Buffer &b) : ListNode() {
       contents = (char *)NULL;
       capacity = 0;
       size = 0;
       offset = 0;
       callerAllocated = false;
       append(b.contents, b.size);
    }
    Buffer(char *cont, unsigned long sz) : ListNode() {
        contents = cont;
        capacity = sz;
        size = sz;
        offset = 0;
        callerAllocated = true;
    }
    // Added by wlee
    Buffer(const char *cont) : ListNode() {
        contents = NULL;
	capacity = size = offset = 0;
        callerAllocated = false;
        append(cont);
    }

    // Initiated a callee allocated buffer
    Buffer(unsigned long sz) : ListNode() {
        contents = (char *) malloc(sz);
	contents[0] = '\0';
        capacity = sz;
        size = 0;
        offset = 0;
        callerAllocated = false;
    }

    // Delete buffer
    virtual ~Buffer() {
        if (!callerAllocated) {
            free(contents);
        }
    }
    
    // increase capacity of Buffer, by at least minExtend
    void extend(unsigned long minExtend = BufferExtendIncrement);
    void append(const char *buf, unsigned long sz);
    void append(const char *str) {
       append(str, strlen(str));
    }
    void append(Buffer &buf);
    void appendf(const char *format, ...);
   // remove sz bytes at atOffset, does not do a free/realloc/malloc
   void flush(unsigned long sz, unsigned long atOffset = 0);
   void insert(Buffer &buf, unsigned long atOffset = 0);
   // destroy the contents
   void destroy() {
      if (contents && ! callerAllocated)
	 free(contents);
      contents = (char *)NULL;
      capacity = 0;
      size = 0;
      offset = 0;
      callerAllocated = false;
   }
   // copy b into this, destroy b
   void destructiveCopy(Buffer &b) {
      destroy();
      contents = b.contents;
      capacity = b.capacity;
      size     = b.size;
      offset   = b.offset;
      callerAllocated = b.callerAllocated;
      b.callerAllocated = true;
      b.destroy();
   }
   bool isFull() {
      return capacity && capacity == size;
   }
   bool hasSpace(unsigned long sz) {
      return size + sz <= capacity;
   }
   bool operator==(const Buffer &b) const {
      return size == b.size && !strncasecmp(contents, b.contents, size);
   }
   bool operator!=(const Buffer &b) const {
      return ! (*this == b);
   }
   bool operator==(const char *cont) const {
      unsigned long len = strlen(cont);
      return size == len && !strncasecmp(contents, cont, len);
   }
   bool operator!=(const char *cont) const {
      return ! (*this == cont);
   }
   bool contains(const char *needle); // search for needle in buffer
   char *getContents(void) const {
     return contents;
   }
   unsigned long getSize(void) const {
     return size;
   }
   void toLowerCase();

public:
    char*		contents;
    unsigned long	capacity;
    unsigned long	size;
    unsigned long	offset;
    bool		callerAllocated;
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

#endif // _Buffer_hh
