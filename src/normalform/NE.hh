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

#ifndef NE_H
#define NE_H

#include "config.h"
#include <iostream>
extern "C" {
#include <sys/types.h>
}
#include "NT.hh"
#include "gnug++/NormalTermPtr.DLList.h"
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
