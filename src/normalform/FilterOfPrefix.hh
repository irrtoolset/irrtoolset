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
//             Katie Petrusha <katie@ripe.net>   

#ifndef FilterOfPrefix_H
#define FilterOfPrefix_H

#include "config.h"
#include <iostream>
#include "Filter.hh"
#include "SetOfPrefix.hh"
#include "SetOfIPv6Prefix.hh"

class FilterOfPrefix : public NEFilter, public SetOfPrefix {
public:
   // FilterOfPrefix();
   // ~FilterOfPrefix();
   // FilterOfPrefix(const FilterOfPrefix& other);

   virtual int isEmpty() {
      return SetOfPrefix::isEmpty();
   }

   virtual int is_universal() {
      return SetOfPrefix::universal();
   }

   virtual void make_universal() {
      SetOfPrefix::make_universal();
   }

   virtual void make_empty() {
      SetOfPrefix::clear();
   }

   virtual void operator ~  () { // complement
      SetOfPrefix::operator~();
   }
   void operator |= (FilterOfPrefix& b) { // union
      SetOfPrefix::operator|=(b);
   }
   void operator &= (FilterOfPrefix& b) { // intersection
      SetOfPrefix::operator&=(b);
   }
   int  operator == (FilterOfPrefix& b) { // equivalance
      return SetOfPrefix::operator==(b);
   }
   void operator =  (FilterOfPrefix& b) { // assignment
      SetOfPrefix::operator=(b);
   }

   void operator = (const PrefixRanges& b) { // assignment
      SetOfPrefix::operator=(b);
   }
   void operator |= (const PrefixRanges & b) { // union
      SetOfPrefix::operator|=(b);
   }

   // below is an ugly trick
   virtual void operator |= (NEFilter& b) {
      *this |= (FilterOfPrefix&) b;
   }
   virtual void operator &= (NEFilter& b) {
      *this &= (FilterOfPrefix&) b;
   }
   virtual int  operator == (NEFilter& b) {
      return (*this == (FilterOfPrefix&) b);
   }
   virtual void operator =  (NEFilter& b) {
      *this = (FilterOfPrefix&) b;
   }

   virtual void do_print (std::ostream& stream) {
      stream << (SetOfPrefix &) *this;
   }

   CLASS_DEBUG_MEMORY_HH(FilterOfPrefix);

};

class FilterOfIPv6Prefix : public NEFilter, public SetOfIPv6Prefix {
public:

   virtual int isEmpty() {
      return SetOfIPv6Prefix::isEmpty();
   }

   virtual int is_universal() {
      return SetOfIPv6Prefix::universal();
   }

   virtual void make_universal() {
      SetOfIPv6Prefix::make_universal();
   }

   virtual void make_empty() {
      SetOfIPv6Prefix::clear();
   }

   virtual void operator ~  () { // complement
      SetOfIPv6Prefix::operator~();
   }
   void operator |= (FilterOfIPv6Prefix& b) { // union
      SetOfIPv6Prefix::operator|=(b);
   }
   void operator &= (FilterOfIPv6Prefix& b) { // intersection
      SetOfIPv6Prefix::operator&=(b);
   }
   int  operator == (FilterOfIPv6Prefix& b) { // equivalance
      return SetOfIPv6Prefix::operator==(b);
   }
   void operator =  (FilterOfIPv6Prefix& b) { // assignment
      SetOfIPv6Prefix::operator=(b);
   }

   void operator = (const MPPrefixRanges& b) { // assignment
      SetOfIPv6Prefix::operator=(b);
   }
   void operator |= (const MPPrefixRanges & b) { // union
      SetOfIPv6Prefix::operator|=(b);
   }

   // below is an ugly trick
   virtual void operator |= (NEFilter& b) {
      *this |= (FilterOfIPv6Prefix&) b;
   }
   virtual void operator &= (NEFilter& b) {
      *this &= (FilterOfIPv6Prefix&) b;
   }
   virtual int  operator == (NEFilter& b) {
      return (*this == (FilterOfIPv6Prefix&) b);
   }
   virtual void operator =  (NEFilter& b) {
      *this = (FilterOfIPv6Prefix&) b;
   }

   virtual void do_print (std::ostream& stream) {
      stream << (SetOfIPv6Prefix &) *this;
   }

   CLASS_DEBUG_MEMORY_HH(FilterOfIPv6Prefix);

};





#endif // FilterOfPrefix_H
