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

#ifndef FixedSizeAllocator_H
#define FixedSizeAllocator_H

#include "config.h"
#include <cstdio>
#include <cstdlib>
extern "C" {
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif // HAVE_MALLOC_H
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

      void *p = free_ptr;
      free_ptr = * (void **) free_ptr;
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
