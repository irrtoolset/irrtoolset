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

#ifndef FilterOfASPath_H
#define FilterOfASPath_H

#include "config.h"
#include "Filter.hh"
#include "re2dfa/regexp_nf.hh"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif // TRUE

class FilterOfASPath : public NEFilter {
public:
   friend class JunosConfig;

   FilterOfASPath() {
      re = new regexp_nf(new regexp_empty_set);
   };
   ~FilterOfASPath() {
      if (re)
        delete re;
   }
   FilterOfASPath(const FilterOfASPath& other) { 
      re = other.re->dup_nf(); 
   }

   virtual int isEmpty() {
      return re->isEmpty();
   }

   virtual int is_universal() {
      return re->is_universal();
   }

   virtual int is_empty() {
      return re->isEmpty();
   }

   virtual int is_empty_str() {
      return re->isEmptyStr();
   }

   virtual void make_universal() {
      re->become_universal();
   }

   virtual void make_empty() {
      re->become_empty();
   }

   void compile(regexp *r, ASt peerAS) { 
      delete re;
      re = new regexp_nf(r->dup(), peerAS);
   }

   virtual void operator ~ () { // complement
      re->do_not();
   }

   void operator |= (FilterOfASPath& b) { // union
      re->do_or(*b.re); // makes b empty
   }

   void operator &= (FilterOfASPath& b) { // intersection
      re->do_and(*b.re); // makes b empty
   }

   int  operator == (FilterOfASPath& b) { // equivalance
      return *re == *b.re;
   }

   void operator =  (FilterOfASPath& b) { // assignment
      delete re;
      re = b.re->dup_nf();
   }

   // below is an ugly trick
   virtual void operator |= (NEFilter& b) {
      *this |= (FilterOfASPath&) b;
   }
   virtual void operator &= (NEFilter& b) {
      *this &= (FilterOfASPath&) b;
   }
   virtual int  operator == (NEFilter& b) {
      return (*this == (FilterOfASPath&) b);
   }
   virtual void operator =  (NEFilter& b) {
      *this = (FilterOfASPath&) b;
   }

   virtual void do_print (std::ostream& stream);

   CLASS_DEBUG_MEMORY_HH(FilterOfASPath);

    operator regexp_nf&() {
      return *re;
   }

private:
   regexp_nf *re;
};


#endif   // FilterOfASPath_H
