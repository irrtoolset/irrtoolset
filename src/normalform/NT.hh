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

#ifndef NT_H
#define NT_H

#include "config.h"
#include <iostream.h>
#include <cassert>
extern "C" {
#include <sys/types.h>
}

#include "FilterOfPrefix.hh"
#include "FilterOfSymbol.hh"
#include "FilterOfASPath.hh"
#include "FilterOfCommunity.hh"
#include "util/debug.hh"

class NormalTerm {
public:
   static const int PRFX         = 0;
   static const int AS_PATH      = 1;
   static const int COMMUNITY    = 2;
   static const int SYMBOLS      = 3;
   static const int FILTER_COUNT = 4;

   // A NormalTerm is an intersection of following filters
   FilterOfPrefix     prfx_set;	    
   FilterOfASPath     as_path;      
   FilterOfCommunity  community;      
   FilterOfSymbol     symbols;       

   //NormalTerm(NormalTerm& a);
   //NormalTerm() {}
   //~NormalTerm();

   NEFilter& operator [] (int i) {
      switch (i) {
      case PRFX:
	 return prfx_set;
      case AS_PATH:
	 return as_path;
      case COMMUNITY:
	 return community;
      case SYMBOLS:
	 return symbols;
      default:
	 assert(0);
      }
   }

   friend ostream& operator<<(ostream& stream, NormalTerm& nt);

   void make_universal(int exception = -1) {
      for (int i = 0; i < FILTER_COUNT; i++)
	 if (exception != i) 
	    (*this)[i].make_universal();
   }

   void make_empty() {
      for (int j = 0; j < FILTER_COUNT; ++j)
	 (*this)[j].make_empty();
   }

   int isEmpty() {
      for (int j = 0; j < FILTER_COUNT; ++j)
	 if ((*this)[j].isEmpty())
	    return true;
      return false;
   }

   int is_universal() {
      for (int j = 0; j < FILTER_COUNT; ++j)
	 if (! (*this)[j].is_universal())
	    return false;
      return true;
   }

   int find_diff(NormalTerm &other);

   CLASS_DEBUG_MEMORY_HH(NormalTerm);
};

typedef NormalTerm* NormalTermPtr;

#endif   // NT_H
