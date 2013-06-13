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

#ifndef REGEXP_NF_H
#define REGEXP_NF_H

#include "config.h"
#include <iostream>
#include "rpsl/List.hh"
#include "rpsl/regexp.hh"
#include "rpsl/rpsl_item.hh"

typedef unsigned int ASt;
const int RE_INVALID_AS = RANGE_TOP + 1;

extern "C" {
#include "re2dfa.h"
}

#define REGEXPNF_FRIENDS \
   friend std::ostream& operator<<(std::ostream& os, const regexp& r); \
   friend class regexp; \
   friend class CiscoConfig;\
   friend class JunosConfig;\
   friend class IOSConfig;\
   friend class IOSXRConfig;\
   friend class JUNOSConfig;\
   friend class RPSLConfig;

class regexp_nf : public regexp {
REGEXPNF_FRIENDS
friend std::ostream& operator<<(std::ostream& os, const regexp_nf& r);
private:
   rd_fm* m;
   static bool expand_as_macros;

private:
   class RegexpConjunct : public ListNode {
   public:
      class ReInt : public ListNode {
      public:
	 regexp *re;
	 int    negated;
	 ReInt() {
	    re = (regexp *) NULL;
	    negated = 0;
	 }
	 ReInt(const ReInt& b) {
	    re = b.re->dup();
	    negated = b.negated;
	 }
	~ReInt(void) { 
	  if (re) delete re;
	}
      };
      int    mark;
      List<ReInt> regexs;
      RegexpConjunct() {
	 mark    = 0;
      }
      RegexpConjunct(const RegexpConjunct &s);
   };
   List<RegexpConjunct> rclist;

public:
   regexp_nf() {
      m = (rd_fm *) NULL;
      become_empty();
   }

   regexp_nf(const regexp_nf& s);

   regexp_nf(regexp* re, ASt peerAS = RE_INVALID_AS) {
      RegexpConjunct *rc = new RegexpConjunct;
      RegexpConjunct::ReInt *ri = new RegexpConjunct::ReInt;
      ri->re = re;
      ri->negated = 0;
      rc->regexs.append(ri);
      rclist.append(rc);
      m = (rd_fm *) NULL;
      m = dfa(peerAS);
  }

  ~regexp_nf(void) {
     if (m) rd_free_dfa(m);
  }

   virtual regexp* dup() const;

   regexp_nf* dup_nf() const { 
      return new regexp_nf(*this);
   }

   void become_universal();
   void become_empty();

   bool operator==(regexp_nf& b);
   bool equals(const regexp &b) {
      return typeid(b) == typeid(regexp_nf)
	 && (*this) == (regexp_nf &) b;
   }

   bool is_universal();
   bool isEmpty();
   bool isEmptyStr();

   void do_not();
   void do_or(regexp_nf &b);	// destroys b
   void do_and(regexp_nf &b);	// destroys b

   static void expandASSets() {
      expand_as_macros = true;
   }

   regexp* construct() const;
   
   bool match(List<ItemASNO> & path);  //Added by mehringe@isi.edu

private:
   regexp* buildCat(regexp *l, regexp *r) const;
   regexp* buildOr(regexp *l, regexp *r) const;
   regexp* buildStar(regexp *l) const;
   regexp* buildQuestion(regexp *l) const;


   rd_fm* dfa(ASt peerAS = RE_INVALID_AS);
   rd_fm* re2nfa(regexp *re, ASt peerAS) const;
   void do_and_terms(regexp_nf &b);	// destroys b's terms
   rd_fm *do_tildaplus_as(rd_fm *result, int as, bool star) const;
   rd_fm *do_tildaplus(regexp_symbol *sym, ASt peerAS, bool star) const;
};

#endif   // REGEXP_NF_H
