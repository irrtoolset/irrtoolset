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

#ifndef NE_H
#define NE_H

#include "config.h"
#include <iostream>
extern "C" {
#include <sys/types.h>
}
#include "NT.hh"
#include "dataset/NormalTermPtr.DLList.h"
#include "irrutil/debug.hh"
#include "rpsl/rpsl_filter.hh"

const unsigned int EXPAND_AS_MACROS         = 0x000001;
const unsigned int EXPAND_COMMUNITIES       = 0x000002;
const unsigned int EXPAND_AS                = 0x000004;
const unsigned int EXPAND_NOTHING           = 0;
const unsigned int EXPAND_ALL               = (~0);


#define NOT_ANY 0
#define ANY     1
#define NEITHER 2

class NormalExpression {
public:
   NormalExpression() : terms() { 
      singleton_flag = -1;
   }
   NormalExpression(NormalExpression& a);
   ~NormalExpression() {
      terms.clear();
   }

   friend std::ostream& operator<<(std::ostream& stream, NormalExpression& ne);

   void do_and(NormalExpression &other);
   void do_or(NormalExpression &other);
   void restrict(FilterAFI *af);
   void do_not();

   int is_universal() {
      return length() == 1 && terms(terms.first())->is_universal();
   }
   int isEmpty () {
      return (length() == 0);
   }
   int is_any() {
      if (isEmpty()) 
	 return NOT_ANY;
      if (is_universal())
	 return ANY;
      return NEITHER;
   }

   void become_universal () {
      NormalTerm* nt = new NormalTerm;
      nt->make_universal();
      terms.clear();
      terms.append(nt);

      singleton_flag = -1;
   }

   void become_empty () {
      terms.clear();
      singleton_flag = -1;
   }

   void makeMoreSpecific(int code, int n, int m) {
      NormalTerm *nt = first();
      if (nt) {
	 nt->prfx_set.makeMoreSpecific(code, n, m);
	 if (nt->isEmpty())
	    become_empty();
      }
   }

   void operator +=(NormalTerm* nt) {
      terms.append(nt);
   }

   CLASS_DEBUG_MEMORY_HH(NormalExpression);

   NormalTerm *first () {
      i = terms.first();
      return i ? terms(i) : (NormalTerm *)0;
   }

   NormalTerm *next () {
      terms.next(i);
      return i ? terms(i) : (NormalTerm *)0;
   }

   int length () {
      return terms.length();
   }

   static NormalExpression *evaluate(const FilterRPAttribute *ptree, 
				     ASt peerAS,
				     unsigned int expand = EXPAND_ALL);
   static NormalExpression *evaluate(const Filter *ptree,
				     ASt peerAS,
				     unsigned int expand = EXPAND_ALL);

private:
   // Normal Expression is the union of Normal Terms in the following list
   NormalTermPtrDLList terms;	
   
   Pix i;
   void reduce();

public:
   int singleton_flag;          // If the NE contains one NT
                                // and that one NT contains only one filter
                                // which is not universal,
                                // then this flag contains that filter's index
                                // the value of this field is -1 otherwise
                                // the value of the flag is set by
                                // Node::Evalutate and do_and, do_or, do_not
				// used to speed up NE manipulation
};

#endif   // NE_H
