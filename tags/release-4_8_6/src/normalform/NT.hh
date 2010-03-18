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

#ifndef NT_H
#define NT_H

#include "config.h"
#include <iostream>
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
   static const int IPV6_PRFX    = 4;
   static const int FILTER_COUNT = 5;

   // A NormalTerm is an intersection of following filters
   FilterOfPrefix     prfx_set;	    
   FilterOfIPv6Prefix ipv6_prfx_set;	    
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
      case IPV6_PRFX:
   return ipv6_prfx_set;
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

   friend std::ostream& operator<<(std::ostream& stream, NormalTerm& nt);

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
      for (int j = 0; j < FILTER_COUNT; ++j) {
     	 if (! (*this)[j].is_universal())
	       return false;
     }
      return true;
   }

   int find_diff(NormalTerm &other);

   CLASS_DEBUG_MEMORY_HH(NormalTerm);
};

typedef NormalTerm* NormalTermPtr;

#endif   // NT_H
