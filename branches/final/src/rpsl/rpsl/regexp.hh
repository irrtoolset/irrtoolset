//  $Id$
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

#ifndef REGEXP_H
#define REGEXP_H

#include "config.h"
#include <iostream.h>
#include <cassert>
#include "gnu/SetOfSymID.hh"
#include "util/List.hh"
#include "rangelist.hh"
#include "symbols.hh"

#define REGEXP_FRIENDS \
   friend ostream& operator<<(ostream& os, const regexp& r); \
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
friend ostream& operator<<(ostream& os, regexp_symbol& r);
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
