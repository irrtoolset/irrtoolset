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

#ifndef FilterOfCommunity_H
#define FilterOfCommunity_H

#include "config.h"
#include "Filter.hh"
#include <iostream.h>
#include "util/List.hh"
#include "community.hh"

class CommunityConjunct : public ListNode {
   friend class FilterOfCommunity;
   friend class BccConfig;
   friend class CiscoConfig;
   friend class JunosConfig;
private:
   // CommunityConjunct is an AND of the following 4 fields
   CommunitySet *p;		// any community in p must be contained in
   CommunitySet *n;		// must not be contained
   CommunitySet *pe;		// pe must equal the community attribute 
   EQList<CommunitySet> ne;	// ne must not equal the community attribute

   bool marked;

public:
   CommunityConjunct() : ne() {
      p  = new CommunitySet;
      n  = new CommunitySet;
      pe = new CommunitySet;
      marked = false;
   }
   CommunityConjunct(const CommunityConjunct& b) : ne(b.ne)  {
      p  = new CommunitySet(*b.p);
      n  = new CommunitySet(*b.n);
      pe = new CommunitySet(*b.pe);
      marked = false;
   }
   ~CommunityConjunct() {
      if (p)
	 delete p;
      if (n)
	 delete n;
      if (pe)
	 delete pe;
      ne.clear();
   };

   friend ostream& operator<<(ostream& os, CommunityConjunct& f);

   int isEmpty() {
      return pe->isEmpty() && p->isEmpty() && n->isEmpty() && ne.isEmpty();
   }
   void make_empty() {
      if (pe)
	 pe->clear();
      if (p)
	 p->clear();
      if (n)
	 n->clear();
      ne.clear();
   }

   void do_and(CommunityConjunct &b);
   void reduce();
   bool implies(const CommunityConjunct &b);

   int operator ==(const CommunityConjunct &b);
};

class FilterOfCommunity : public NEFilter {
   friend class NormalTerm;
   friend class BccConfig;
   friend class CiscoConfig;
   friend class JunosConfig;
private:
   EQList<CommunityConjunct> conjuncts;
   bool _universal;

public:
   FilterOfCommunity() : conjuncts(), _universal(false) {
   }
   // FilterOfCommunity(FilterOfCommunity &); let the compiler generate it

   ~FilterOfCommunity() {}

   void set(CommunitySet *clist, bool exact = false) {
      CommunityConjunct *ptr = new CommunityConjunct;
      if (exact)
	 ptr->pe = clist;
      else
	 ptr->p = clist;
      conjuncts.append(ptr);
   }

   virtual int isEmpty() {
      return (! _universal && conjuncts.isEmpty());
   }

   virtual int is_universal() {
      return (_universal);
   }

   virtual void make_empty() {
      conjuncts.clear();
      _universal = false;
   }

   virtual void make_universal() {
      conjuncts.clear();
      _universal = true;
   }

   virtual void operator ~  (); // complement
   void operator |= (FilterOfCommunity& b); // union
   void operator &= (FilterOfCommunity& b); // intersection
   int  operator == (FilterOfCommunity& b); // equivalance
   void operator =  (FilterOfCommunity& b); // assignment

   // below is an ugly trick
   virtual void operator |= (NEFilter& b) {
      *this |= (FilterOfCommunity&) b;
   }
   virtual void operator &= (NEFilter& b) {
      *this &= (FilterOfCommunity&) b;
   }
   virtual int  operator == (NEFilter& b) {
      return (*this == (FilterOfCommunity&) b);
   }
   virtual void operator =  (NEFilter& b) {
      *this = (FilterOfCommunity&) b;
   }

   friend ostream& operator<<(ostream& stream, FilterOfCommunity& Filter);
   void NegateCommunityConjunct(CommunityConjunct *cp);

   virtual void do_print (ostream& stream);

private:
   void reduce();
};

#endif   // FilterOfCommunity_H
