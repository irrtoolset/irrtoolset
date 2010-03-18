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

#ifndef REGEXP_H
#define REGEXP_H

#include "config.h"
#include <iostream>
#include <cassert>
#include "gnu/SetOfSymID.hh"
#include "util/List.hh"
#include "rangelist.hh"
#include "symbols.hh"

#define REGEXP_FRIENDS \
   friend std::ostream& operator<<(std::ostream& os, const regexp& r); \
   friend class regexp_nf; \
   friend class CiscoConfig;\
   friend class JunosConfig;\
   friend class RSdConfig;

class regexp { 
REGEXP_FRIENDS
public:
   regexp() { 
   }
   regexp(const regexp&) { 
   }
   bool operator==(const regexp &b) {
      return equals(b);
   }
   bool operator!=(const regexp &b) {
      return ! equals(b);
   }
   virtual regexp* dup() const = 0;
   virtual bool equals(const regexp &b) = 0;
   virtual ~regexp() {}
};

class regexp_empty_set : public regexp {
REGEXP_FRIENDS
public:
   regexp_empty_set() { 
   }
   regexp_empty_set(const regexp_empty_set& r) {
   }
   virtual ~regexp_empty_set() {}
   virtual regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_empty_set);
   }
};

class regexp_empty_str : public regexp {
REGEXP_FRIENDS
public:
   regexp_empty_str () { 
   }
   regexp_empty_str (const regexp_empty_str& r)  {
   }
   virtual ~regexp_empty_str () {}

   virtual regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_empty_str);
   }
};

class regexp_bol : public regexp { // bol is beginning-of-line ^
REGEXP_FRIENDS
public:
   regexp_bol() { 
   }
   regexp_bol(const regexp_bol&) {
   }
   virtual ~regexp_bol() {}
   regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_bol);
   }
};

class regexp_eol : public regexp { // eol is end-of-line $
REGEXP_FRIENDS
public:
   regexp_eol() { 
   }
   regexp_eol(const regexp_eol&) {
   }
   virtual ~regexp_eol() {}
   regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_eol);
   }
};

class regexp_symbol : public regexp {
REGEXP_FRIENDS
friend std::ostream& operator<<(std::ostream& os, regexp_symbol& r);
public:
   static int MIN_AS;
   static int MAX_AS;

private:
   RangeList asnumbers;
   SetOfSymID asSets;
   int complemented;

public:
   regexp_symbol () { 
      complemented = 0;
   }
   // ~regexp_symbol() {   } // compiler is OK
   // regexp_symbol(const regexp_symbol& s) {assert(0);} // compiler is OK

   regexp_symbol(int as1) {
      complemented = 0;
      asnumbers.add(as1, as1);
   }
   regexp_symbol(int as1, int as2) {
      complemented = 0;
      asnumbers.add(as1, as2);
   }
   regexp_symbol(SymID asset) {
      complemented = 0;
      asSets.add(asset);
   }

   void add(int as1) {
      asnumbers.add(as1, as1);
   }
   void add(int as1, int as2) {
      asnumbers.add(as1, as2);
   }
   void add(SymID asset) {
      asSets.add(asset);
   }

   void complement() {
      complemented = ! complemented;
   }
   regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_symbol) 
	 && complemented == ((regexp_symbol &) b).complemented
	 && asnumbers == ((regexp_symbol &) b).asnumbers
	 && asSets == ((regexp_symbol &) b).asSets;
   }
};

class regexp_cat : public regexp {
REGEXP_FRIENDS
private:
   regexp*	left;
   regexp*	right;

public:
   regexp_cat(regexp *l, regexp *r) {
      left = l;
      right = r;
   }

   regexp_cat(const regexp_cat &b) {
      left = b.left->dup();
      right = b.right->dup();
   }

   virtual ~regexp_cat() { 
      if (left)
	 delete left;
      if (right)
	 delete right;
   }
   regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_cat) 
	 && left == ((regexp_cat &) b).left
	 && right == ((regexp_cat &) b).right;
   }
};

class regexp_or : public regexp {
REGEXP_FRIENDS
private:
   regexp*	left;
   regexp*	right;

public:
   regexp_or(regexp *l, regexp *r) {
      left = l;
      right = r;
   }

   regexp_or(const regexp_or &b) {
      left = b.left->dup();
      right = b.right->dup();
   }

   virtual ~regexp_or() {
      if (left)
	 delete left;
      if (right)
	 delete right;
   }
   regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_or) 
	 && left == ((regexp_or &) b).left
	 && right == ((regexp_or &) b).right;
   }
};

class regexp_star : public regexp {
REGEXP_FRIENDS
private:
   regexp*	left;

public:
   regexp_star(regexp *l) {
      left = l;
   }
   regexp_star(const regexp_star &b) {
      left = b.left->dup();
   }

   virtual ~regexp_star() {
      if (left)
	 delete left;
   }
   regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_star) 
	 && left == ((regexp_star &) b).left;
   }
};

class regexp_question : public regexp {
REGEXP_FRIENDS
private:
   regexp*	left;

public:
   regexp_question(regexp *l) {
      left = l;
   }
   regexp_question(const regexp_question &b) {
      left = b.left->dup();
   }
   virtual ~regexp_question() {
      if (left)
	 delete left;
   }
   regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_question) 
	 && left == ((regexp_question &) b).left;
   }
};

class regexp_plus : public regexp {
REGEXP_FRIENDS
private:
   regexp*	left;

public:
   regexp_plus(regexp *l) {
      left = l;
   }
   regexp_plus(const regexp_plus &b) {
      left = b.left->dup();
   }

   virtual ~regexp_plus() {
      if (left)
	 delete left;
   }
   regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_plus) 
	 && left == ((regexp_plus &) b).left;
   }
};

class regexp_tildastar : public regexp {
REGEXP_FRIENDS
private:
   regexp*	left;

public:
   regexp_tildastar(regexp *l) {
      left = l;
   }
   regexp_tildastar(const regexp_tildastar &b) {
      left = b.left->dup();
   }

   virtual ~regexp_tildastar() {
      if (left)
	 delete left;
   }
   regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_tildastar) 
	 && left == ((regexp_tildastar &) b).left;
   }
};

class regexp_tildaplus : public regexp {
REGEXP_FRIENDS
private:
   regexp*	left;

public:
   regexp_tildaplus(regexp *l) {
      left = l;
   }
   regexp_tildaplus(const regexp_tildaplus &b) {
      left = b.left->dup();
   }

   virtual ~regexp_tildaplus() {
      if (left)
	 delete left;
   }
   regexp* dup() const;
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_tildaplus) 
	 && left == ((regexp_tildaplus &) b).left;
   }
};

#endif   // REGEXP_H
