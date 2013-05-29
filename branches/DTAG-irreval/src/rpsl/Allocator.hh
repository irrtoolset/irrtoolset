//  $Id$
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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#ifndef Allocator_H
#define Allocator_H

#include "config.h"
#include <cstdio>
#include <cstdlib>
#ifdef HAVE_MALLOC_H
extern "C" {
#include <malloc.h>
}
#endif // HAVE_MALLOC_H

// we allocate memory of chunks AllocatorChunkSize
// AllocatorLargeRequestSize is the largest possible fragmentation we can have
#define AllocatorChunkSize (1024 * 1024 - sizeof(void *))
#define AllocatorLargeRequestSize (16 * 1024)

class Allocator {
private:
   void *root;
   void *free_ptr;
   int  size;
   int  large_request;
   int  remaining;

   void *get_more_memory(int request) {
      void *p = malloc(request + sizeof(void *));

      * (void **) p = root;
      root = p;
      p = (char *) p + sizeof(void *);
      return p;
   }

public:
   Allocator(int _size = AllocatorChunkSize, 
	     int _large_request = AllocatorLargeRequestSize) {
      size = _size;
      large_request = _large_request;
      remaining = 0;
      free_ptr = NULL;
      root = NULL;
   }

   ~Allocator() {
      deallocate_all();
   }

   void *allocate(size_t request) {
      void *result;

      if (request > large_request)
	 return get_more_memory(request);

      if (request > remaining) {
	 free_ptr = get_more_memory(size);
	 remaining = size;
      }

      result = free_ptr;
      free_ptr = (char *) free_ptr + request;
      remaining -= request;
      return result;
   }

   void deallocate(char *) { // intentionally blank
   }

   void deallocate_all() {
      for (void *p = root; p; p = root) {
	 root = * (void **) root;
	 free(p);
      }
      remaining = 0;
      free_ptr = NULL;
      root = NULL;
  }
};

#endif   // Allocator_H
