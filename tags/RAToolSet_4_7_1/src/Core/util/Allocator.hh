//  $Id$
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
