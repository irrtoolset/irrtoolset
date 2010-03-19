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

#ifndef SYMBOLS_HH
#define SYMBOLS_HH

#include "config.h"
#include "Allocator.hh"
#include "dataset/SetOfSymID.hh"
#include "irrutil/strcase.h"
#include "rpsl/rpsl_asnum.hh"
#include <cstdio>

typedef unsigned int ASt;

const int ExpNoOfSymbols = 2000; // covers all AS#'s, and set names in '97
const int ExpSymbolLength = 20;  // well much bigger

typedef char* SymID;

class Symbols {
private:
   Allocator allocator;
   SetOfSymID symset;
   SymID as_any;
   SymID peerAS;
public:

   Symbols() : allocator(ExpNoOfSymbols*ExpSymbolLength, ExpSymbolLength*5),
      symset(ExpNoOfSymbols) {
      as_any = symID("AS-ANY");
      peerAS = symID("PEERAS");
   }
   Symbols(int expNoOfSymbols, int expSymbolLength) : 
      allocator(expNoOfSymbols*expSymbolLength, expSymbolLength*5),
      symset(ExpNoOfSymbols) {
      as_any = symID("AS-ANY");
      peerAS = symID("PEERAS");
   }

   SymID symID(const char *name, int len = -1) {
      Pix r;
      char *nname=strdup(name);	

      strupr(nname);
      if (r = symset.seek((char *) nname))
	 return symset(r);

      if (len == -1)
	 len = strlen(nname);
      char *cptr = (char *) allocator.allocate(len+1);
      strcpy(cptr, nname);
      symset.add(cptr);

      free (nname);

      return cptr;
   }

   char *operator[](const SymID p) const {
      return p;
   }

   bool isASAny(SymID sid) {
      return sid == as_any;
   }
   bool isPeerAS(SymID sid) {
      return sid == peerAS;
   }

   SymID resolvePeerAS(SymID sid, ASt peerAS) {
      char *buffer = (char *) malloc(strlen(sid) * 3);
      char *head, *tail, *ptr;
      int  written;
      ptr = buffer;
      for (head = strstr(sid, "PEERAS"), tail = sid; 
	   head;
	   tail = head + 6, head++, head = strstr(head, "PEERAS")) {
	 strncpy(ptr, tail, head - tail);
	 ptr += (head - tail);
	 asnum_string(ptr, peerAS);
	 written = strlen(ptr);
	 ptr += written;
      }
      if (tail == sid) {
	 free(buffer);
	 return sid;
      }

      strcpy(ptr, tail);

      SymID nid = symID(buffer);
      free(buffer);

      return nid;
   }
};

extern Symbols symbols;

#endif   // SYMBOLS_HH
