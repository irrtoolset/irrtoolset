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

#ifndef SymbolConjunct_H
#define SymbolConjunct_H

#include "config.h"
#include <iostream.h>
extern "C" {
#include <sys/types.h>
}

#include "gnu/SetOfSymID.hh"
#include "util/debug.hh"
#include "rpsl/symbols.hh"


class SetOfSymbol;
class regexp;

class SymbolConjunct {
public:
   friend class SetOfSymbol;
   friend class NormalTerm;
   friend ostream& operator<<(ostream& stream, SetOfSymbol& set);
   friend ostream& operator<<(ostream& os, const regexp& r);

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

   friend ostream& operator<<(ostream& stream, SymbolConjunct &conjunct);

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
