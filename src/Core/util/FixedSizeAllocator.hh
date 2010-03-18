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

#ifndef FixedSizeAllocator_H
#define FixedSizeAllocator_H

#include "config.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
extern "C" {
//#ifdef HAVE_MALLOC_H
//#include <malloc.h>
//#endif // HAVE_MALLOC_H
#include <sys/types.h>
}

class FixedSizeAllocator {
private:
   int size;
   int count;
   void *root;
   void *free_ptr;

   void get_more_memory() {
      void *p = malloc(size * count + sizeof(void *));

      assert(p);

      * (void **) ((char *) p + size * count) = root;
      root = p;

      char *q = (char *) p;
      for (int i = 0; i < count - 1; i++) {
	 * (char **) q = q + size;
	 q += size;
      }
      * (char **) q = (char *) NULL;

      free_ptr = p;
   }

public:
   FixedSizeAllocator(int _size, int _count) {
      size = _size;
      count = _count;
      root = NULL;
      free_ptr = NULL;      
   }
   ~FixedSizeAllocator() {
      // Deallocate memory here will cause problems since
      // some classes stay longer than this 
      // deallocate_all();
   }

   void *allocate() {
      if (!free_ptr)
	 get_more_memory();
      assert(free_ptr);

      void *p = free_ptr;
      free_ptr = * (void **) free_ptr;
      char buf[1024];
      sprintf (buf,"%p", free_ptr); // to workaround library/code problem on solaris
      return p;
   }

   void deallocate(void *p) {
      * (void **) p = free_ptr;
      free_ptr = p;
   }

   void deallocate_all() {
      for (void *p = root; p; p = root) {
	 root = * (void **) ((char *) root + size * count);
	 free(p);
      }
      free_ptr = NULL;
      root = NULL;
  }
};

#endif   // FixedSizeAllocator_H
