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

#ifndef FilterOfSymbol_H
#define FilterOfSymbol_H

#include "config.h"
#include <iostream>
#include "Filter.hh"
#include "SetOfSymbol.hh"

class FilterOfSymbol : public NEFilter, public SetOfSymbol {
   friend class NormalTerm;
public:
   FilterOfSymbol() {}

   virtual int isEmpty() {
      return SetOfSymbol::empty();
   }

   virtual int is_universal() {
      return SetOfSymbol::universal();
   }

   virtual void make_universal() {
      SetOfSymbol::make_universal();
   }

   virtual void make_empty() {
      SetOfSymbol::clear();
   }

   virtual void operator ~  () { // complement
      SetOfSymbol::operator~();
   }

   void operator |= (FilterOfSymbol& b) { // union
      SetOfSymbol::operator|=(b);
   }
   void operator &= (FilterOfSymbol& b) { // intersection
      SetOfSymbol::operator&=(b);
   }
   int  operator == (FilterOfSymbol& b) { // equivalance
      return SetOfSymbol::operator==(b);
   }
   void operator =  (FilterOfSymbol& b) { // assignment
      SetOfSymbol::operator=(b);
   }

   void operator &= (SetOfSymbol& b) { // intersection
      SetOfSymbol::operator&=(b);
   }
   void operator = (SetOfSymbol& b) { // assignment
      SetOfSymbol::operator=(b);
   }
   void operator = (const SetOfSymID& b) { // assignment
      SetOfSymbol::operator=(b);
   }
   void operator = (const SetOfUInt& b) { // assignment
      SetOfSymbol::operator=(b);
   }
   void add(ASt as) {
      SetOfSymbol::add(as);
   }
   void add(SymID sid) {
      SetOfSymbol::add(sid);
   }
   // below is an ugly trick
   virtual void operator |= (NEFilter& b) {
      *this |= (FilterOfSymbol&) b;
   }
   virtual void operator &= (NEFilter& b) {
      *this &= (FilterOfSymbol&) b;
   }
   virtual int  operator == (NEFilter& b) {
      return (*this == (FilterOfSymbol&) b);
   }
   virtual void operator =  (NEFilter& b) {
      *this = (FilterOfSymbol&) b;
   }

   virtual void do_print (std::ostream& stream) {
      stream << (SetOfSymbol &) *this;
   }

   CLASS_DEBUG_MEMORY_HH(FilterOfSymbol);

};

#endif   // FilterOfSymbol_H
