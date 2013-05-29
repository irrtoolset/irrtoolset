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

#ifndef SymbolConjunct_H
#define SymbolConjunct_H

#include "config.h"
#include <iostream>
extern "C" {
#include <sys/types.h>
}

#include "dataset/SetOfSymID.hh"
#include "irrutil/debug.hh"
#include "rpsl/symbols.hh"


class SetOfSymbol;
class regexp;

class SymbolConjunct {
public:
   friend class SetOfSymbol;
   friend class NormalTerm;
   friend std::ostream& operator<<(std::ostream& stream, SetOfSymbol& set);
   friend std::ostream& operator<<(std::ostream& os, const regexp& r);

   SymbolConjunct() : symbols(), notSymbols() {
   }
   SymbolConjunct(SymID sid, bool negated = false) : symbols(), notSymbols() {
      if (negated)
	 notSymbols.add(sid);
      else
	 symbols.add(sid);
   }
   SymbolConjunct(const SymbolConjunct& b) : 
      symbols(b.symbols), notSymbols(b.notSymbols) {
   }
   SymbolConjunct(const SetOfSymID& b) : symbols(b), notSymbols() {
   }
   ~SymbolConjunct() {
      symbols.clear();
      notSymbols.clear();
   }

   friend std::ostream& operator<<(std::ostream& stream,
                                   SymbolConjunct &conjunct);

   int length() {
      return symbols.length() + notSymbols.length();
   }
   int operator<=(const SymbolConjunct& b) {
      return symbols.length() <= b.symbols.length() 
	 && notSymbols.length() <= b.notSymbols.length() 
	 && symbols <= b.symbols 
	 && notSymbols <= b.notSymbols;
   }
   int operator==(const SymbolConjunct& b) {
      return symbols.length() == b.symbols.length() 
	 && notSymbols.length() == b.notSymbols.length() 
	 && b.symbols == symbols 
	 && b.notSymbols == notSymbols;
   }
   void merge(SymbolConjunct& b) {
      symbols |= b.symbols;
      notSymbols |= b.notSymbols;
   }
   void negatedMerge(SymbolConjunct& b) {
      symbols |= b.notSymbols;
      notSymbols |= b.symbols;
   }
   void remove(SymbolConjunct& b) {
      symbols -= b.symbols;
      notSymbols -= b.notSymbols;
   }
   bool isUniversal() {
      SetOfSymID tmp(symbols);
      tmp &= notSymbols;
      return !tmp.empty();
   }
   bool isEmpty() {
      return symbols.empty() && notSymbols.empty();
   }

   void first(SymID &sid, bool &negated) {
      turn = true;
      itr = symbols.first();
      if (itr) {
	 sid = symbols(itr);
      } else {
	 turn = false;
	 itr = notSymbols.first();
	 if (!itr)
	    sid = (SymID) NULL;
      }
      negated = ! turn;
   }
   void next(SymID &sid, bool &negated) {
      if (turn) {
	 symbols.next(itr);
	 if (itr) {
	    sid = symbols(itr);
	 } else {
	    turn = false;
	    itr = notSymbols.first();
	    if (!itr)
	       sid = (SymID) NULL;
	    else
	       sid = notSymbols(itr);
	 }
      } else {
	 notSymbols.next(itr);
	 if (!itr)
	    sid = (SymID) NULL;
	 else
	    sid = notSymbols(itr);
      }
      negated = ! turn;
   }

   void add(SymID sid) {
      symbols.add(sid);
   }

   CLASS_DEBUG_MEMORY_HH(SymbolConjunct);
private:
   Pix itr;
   bool turn;
   SetOfSymID symbols;
   SetOfSymID notSymbols;
};

typedef SymbolConjunct *SymbolConjunctPtr;

#endif   // SymbolConjunct_H
