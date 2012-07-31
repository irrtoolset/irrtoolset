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

#ifndef SetOfSymbol_H
#define SetOfSymbol_H

#include "config.h"
#include <cstdio>
#include "SymbolConjunct.hh"
#include "dataset/SymbolConjunctPtr.SplayBag.h"
#include "rpsl/rpsl.hh"

typedef class SymbolConjunctPtrSplayBag BagOfSymbolConjunct;
class SetOfUInt;

class SetOfSymbol {
public:
   friend class NormalTerm;

   SetOfSymbol() : conjuncts() {
      _universal = 0;
   }
   SetOfSymbol(SetOfSymbol &);
   ~SetOfSymbol() {
      clear();
   }

   virtual int empty() {
      return (! _universal && conjuncts.empty());
   }

   virtual int universal() {
      return (_universal);
   }

   virtual void clear();

   virtual void make_universal() {
      clear();
      _universal = 1;
   }

   virtual void operator ~  (); // complement
   void operator |= (SetOfSymbol& b); // union
   void operator &= (SetOfSymbol& b); // intersection
   int  operator == (SetOfSymbol& b); // equivalance
   void operator =  (SetOfSymbol& b); // assignment
   void operator =  (const SetOfSymID& b); // assignment
   void operator =  (const SetOfUInt& b); // assignment
   void add(ASt as);
   void add(SymID sid);

   friend std::ostream& operator<<(std::ostream& stream, SetOfSymbol& set);

private:
   BagOfSymbolConjunct conjuncts;
   int _universal;

   void reduce();
   void splice(SetOfSymbol& b);
};

#endif   // SetOfSymbol_H
