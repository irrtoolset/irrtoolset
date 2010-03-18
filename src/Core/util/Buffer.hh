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

#include "util/List.hh"

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
    
    // Compress a buffer 
    Buffer*				// Out: new, compressed buffer
    compress();

    // Decompress a compressed buffer
    Buffer*				// Out: uncompressed buffer
    uncompress();

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
   void zeroFill(); 
   void toLowerCase();

   // creates a string (by appending null terminated) from the contents
   // this allocates memory for the string, i.e. like strdup
   char *makeString() const;
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

#endif // _Buffer_hh
