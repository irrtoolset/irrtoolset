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

#ifndef SYMBOLS_HH
#define SYMBOLS_HH

#include "config.h"
#include "util/Allocator.hh"
#include "gnu/SetOfSymID.hh"
#include "util/xstring.h"
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

   SymID symID(char *name, int len = -1) {
      Pix r;
      strupr(name);
      if (r = symset.seek((char *) name))
	 return symset(r);

      if (len == -1)
	 len = strlen(name);
      char *cptr = (char *) allocator.allocate(len+1);
      strcpy(cptr, name);
      symset.add(cptr);

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
      char *buffer = (char *) malloc(strlen(sid) * 2);
      char *head, *tail, *ptr;
      int  written;
      ptr = buffer;
      for (head = strstr(sid, "PEERAS"), tail = sid; 
	   head;
	   tail = head + 6, head++, head = strstr(head, "PEERAS")) {
	 strncpy(ptr, tail, head - tail);
	 ptr += (head - tail);
	 sprintf(ptr, "AS%d", peerAS);
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
