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

#ifndef REGEXP_NF_H
#define REGEXP_NF_H

#include "config.h"
#include <iostream.h>
#include "util/List.hh"
#include "rpsl/regexp.hh"
#include "rpsl/rpsl_item.hh"

typedef unsigned int ASt;
const int RE_INVALID_AS = RANGE_TOP + 1;

extern "C" {
#include "re2dfa.h"
}

#define REGEXPNF_FRIENDS \
   friend ostream& operator<<(ostream& os, const regexp& r); \
   friend class regexp; \
   friend class BccConfig;\
   friend class CiscoConfig;\
   friend class GatedConfig;\
   friend class JunosConfig;\
   friend class RSdConfig;

class regexp_nf : public regexp {
REGEXPNF_FRIENDS
friend ostream& operator<<(ostream& os, const regexp_nf& r);
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
	// Added by wlee@isi.edu
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
