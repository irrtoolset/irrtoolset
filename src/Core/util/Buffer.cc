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
// Author: Ramesh Govindan <govindan@isi.edu>
//         Cengiz Alaettinoglu
//         WeeSan Lee <wlee@isi.edu>
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <iomanip>
#include <cctype>
#if (__GNUC__ > 2)
#include <ostream>
#else
#include <ostream.h>
#endif

using namespace std;

#ifdef NEED_COMPRESSION
#include <zlib.h>
#include "util/Trail.hh"
#endif // NEED_COMPRESSION
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif // HAVE_STRINGS_H

#include "util/Buffer.hh"

#ifdef NEVER
int myallocCnt = 0;

static void *mymalloc(int opaque, int n, int sz) {
   myallocCnt++;
   return malloc(n * sz);
}
static void myfree(int opaque, void *ptr) {
printf("freeing\n");
   myallocCnt--;
   free(ptr);
}
#else
#define mymalloc 0
#define myfree 0
#endif // never

ostream &operator<<(ostream &os, const Buffer &b)
{
  os << setw(b.getSize()) << b.getContents() << " (" << b.getSize() << ")";
  return os;
}

Buffer*
Buffer::compress()
{
#ifdef NEED_COMPRESSION
    z_stream                    cp;
    int                         err;
    Buffer*			nbuf;

    // Need at most 1.1 * length for the decompressed buffer
    nbuf = new Buffer((unsigned long) ((1.1 * size) + 12));

    // Compress the incoming data
    cp.zalloc = (alloc_func) mymalloc;
    cp.zfree = (free_func) myfree;
    cp.opaque = (voidpf) 0;

    err = deflateInit(&cp, Z_BEST_COMPRESSION);
    if (err != Z_OK) {
        ERROR("zlib error %d while initializing stream\n", err);
        delete nbuf;
        return NULL;
    }

    cp.next_in = (unsigned char*) contents;
    cp.avail_in = size;
    cp.next_out = (unsigned char*) nbuf->contents;
    cp.avail_out = nbuf->capacity;
    
    err = deflate(&cp, Z_FINISH);
    if (err != Z_STREAM_END) {
        ERROR("zlib error %d when compressing data\n", err);
        delete nbuf;
        return NULL;
    }

    err = deflateEnd(&cp);
    if (err != Z_OK) {
        ERROR("zlib error %d when closing compression stream\n", err);
        delete nbuf;
        return NULL;
    }

    nbuf->size = cp.total_out;
    return nbuf;
#else
    return new Buffer(*this);
#endif
}

Buffer*
Buffer::uncompress()
{
#ifdef NEED_COMPRESSION
    z_stream		decompress;
    int			error;
    Buffer*		nbuf;
    unsigned int	guess;

    guess = size;
    while (1) {
        guess <<= 2;
        nbuf = new Buffer(guess);

        decompress.zalloc = (alloc_func) mymalloc;
        decompress.zfree = (free_func) myfree;
        decompress.opaque = (voidpf) 0;
        decompress.next_in = (unsigned char*) contents;
        decompress.avail_in = size;
        decompress.next_out = (unsigned char*) nbuf->contents;
        decompress.avail_out = nbuf->capacity;
        
	error = inflateInit(&decompress);
        if (error != Z_OK) {
            ERROR("error number %d while initializing decompression stream\n",
                  error);
            delete nbuf;
            return NULL;
        }

       error = inflate(&decompress, Z_FINISH);
        if (error == Z_STREAM_END) {
            error = inflateEnd(&decompress);
            if (error != Z_OK) {
                ERROR("error number %d while closing decompression stream\n",
                      error);
                delete nbuf;
                return NULL;
            }
            nbuf->size = decompress.total_out;
            return nbuf;
        }
        delete nbuf;
    }
    // Not Reached
#else
    return new Buffer(*this);
#endif
}

void Buffer::extend(unsigned long minExtend) {
   assert(!callerAllocated);	// !!!
   if (capacity + BufferExtendIncrement > size + minExtend) {
      capacity = capacity + BufferExtendIncrement;
   } else {
      capacity = size + minExtend;
   }
   contents = (char *)realloc(contents, capacity);
}

void 
Buffer::append(const char *buf, unsigned long sz)
{
   if (size + sz + 1 > capacity)
      extend(sz);
   
   memmove(contents+size, buf, sz+1);
   size += sz;
}

void Buffer::appendf(const char *format, ...) {
   if (size + BufferExtendIncrement / 2 > capacity)
      extend();

   va_list ap;
    
   va_start(ap, format);
#ifdef HAVE_VSNPRINTF
   (void) vsnprintf(contents + size, capacity - size, format, ap);
#else
   (void) vsprintf(contents + size, format, ap);
#endif
   va_end(ap);
   size += strlen(contents + size);
}

void Buffer::append(Buffer &buf) {
   if (size + buf.size > capacity)
      extend(buf.size);
   memcpy(contents + size, buf.contents, buf.size);
   size += buf.size;
}

void Buffer::insert(Buffer &buf, unsigned long atOffset) {
   if (size + buf.size > capacity)
      extend(buf.size);
   memmove(contents + atOffset + buf.size, 
	   contents + atOffset, 
	   size - atOffset);
   memcpy(contents + atOffset, buf.contents, buf.size);
   size += buf.size;
}

void Buffer::flush(unsigned long sz, unsigned long atOffset) {
   if (sz == 0 || size == 0)
      return;

   assert(atOffset + sz <= size);

   memmove(contents + atOffset, contents + atOffset + sz, size - atOffset - sz);

   size -= sz;
   if (offset >= atOffset)
      offset = (offset >= atOffset + sz) ? offset-sz : atOffset;
}

char *Buffer::makeString() const {
   char *str = (char *) malloc(size + 1);
   strncpy(str, contents, size);
   *(str + size) = 0;
   return str;
}

bool Buffer::contains(const char *needle) { // search for needle in buffer
   if (capacity > size) {
      *(contents + size) = 0;
      return strstr(contents, needle);
   }

   char last = *(contents + size - 1);
   *(contents + size - 1) = 0;

   if (strstr(contents, needle)) {
      *(contents + size - 1) = last;
      return true;
   }

   char last2 = *(needle + strlen(needle) - 1);
   if (last != last2) {
      *(contents + size - 1) = last;
      return false;
   }

   char *needle2 = strdup(needle);
   *(needle2 + strlen(needle2) - 1) = 0;

   if (strstr(contents + size - strlen(needle), needle2)) {
      *(contents + size - 1) = last;
      free(needle2);
      return true;
   }

   *(contents + size - 1) = last;
   free(needle2);
   return false;
}

void Buffer::toLowerCase() {
   for (char *p = contents; p < contents + size; ++p)
      if (isascii(*p) && isupper(*p))
         *p = tolower(*p);
}

void Buffer::zeroFill() {
   bzero(contents, capacity);
   size = capacity;
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
//  Author(s): Ramesh Govindan <govindan@isi.edu>

