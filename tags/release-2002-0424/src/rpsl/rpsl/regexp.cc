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

#include "config.h"

#include "regexp.hh"
#include <cstring>
#include "symbols.hh"

// Static members
int regexp_symbol::MIN_AS = 0;
int regexp_symbol::MAX_AS = RANGE_TOP;

////////////////////////////// output ////////////////////

ostream& operator<<(ostream& os, regexp_symbol& rs) {
   static RangeList::Range zero_to_zero(0, 0);

   int put_brackets = 0;

   if (rs.asSets.empty() && ! rs.asnumbers.universal()
       && rs.asnumbers.contains(zero_to_zero)) {
      rs.complemented = ! rs.complemented;
      rs.asnumbers.complement();
   }

   if (rs.complemented 
       || ! ((rs.asnumbers.is_singleton() && rs.asSets.length() == 0)
	     || (rs.asnumbers.empty() && rs.asSets.length() == 1))) {
      put_brackets = 1;
      if (rs.asnumbers.useOrList)
	 os << "(";
      else
	 os << "[";
      if (rs.complemented) {
	 if (rs.asnumbers.useOrList)
	    rs.asnumbers.complement();
	 else
	    os << "^";
      }
   }

   if (!rs.asnumbers.empty() || rs.asSets.empty())
      os << rs.asnumbers;
   
   if (!rs.asnumbers.empty() && !rs.asSets.empty())
      if (rs.asnumbers.useOrList)
	 os << "|";
      else
	 os << " ";

   for (Pix p = rs.asSets.first(); p; ) {
      os << rs.asSets(p);
      rs.asSets.next(p);
      if (p && rs.asnumbers.useOrList)
	 os << "|";
      else
	 os << " ";
   }
   
   if (put_brackets)
      if (rs.asnumbers.useOrList)
	 os << ")";
      else
	 os << "]";

   if (rs.complemented && rs.asnumbers.useOrList)
      rs.asnumbers.complement();

   return os;
}

ostream& operator<<(ostream& os, const regexp& r) {
   if (typeid(r) == typeid(regexp_bol))
      os << "^";
   else if (typeid(r) == typeid(regexp_eol))
      os << "$";
   else if (typeid(r) == typeid(regexp_symbol))
      os << (regexp_symbol &) r;
   else if (typeid(r) == typeid(regexp_cat))
      os << *((regexp_cat &) r).left << " " << *((regexp_cat &) r).right;
   else if (typeid(r) == typeid(regexp_or))
      os << "(" << *((regexp_or &) r).left 
	 << " | " << *((regexp_or &) r).right << ")";
   else if (typeid(r) == typeid(regexp_star))
      if (typeid(*((regexp_star &) r).left) == typeid(regexp_cat))
	 os << "(" << *((regexp_star &) r).left << ")*";
      else 
	 os << *((regexp_star &) r).left << "*";
   else if (typeid(r) == typeid(regexp_tildastar))
      if (typeid(*((regexp_tildastar &) r).left) == typeid(regexp_cat))
	 os << "(" << *((regexp_tildastar &) r).left << ")~*";
      else 
	 os << *((regexp_tildastar &) r).left << "~*";
   else if (typeid(r) == typeid(regexp_tildaplus))
      if (typeid(*((regexp_tildaplus &) r).left) == typeid(regexp_cat))
	 os << "(" << *((regexp_tildaplus &) r).left << ")~+";
      else 
	 os << *((regexp_tildaplus &) r).left << "~+";
   else if (typeid(r) == typeid(regexp_question))
      if (typeid(*((regexp_question &) r).left) == typeid(regexp_cat))
	 os << "(" << *((regexp_question &) r).left << ")?";
      else 
	 os << *((regexp_question &) r).left << "?";
   else if (typeid(r) == typeid(regexp_plus))
      if (typeid(*((regexp_plus &) r).left) == typeid(regexp_cat))
	 os << "(" << *((regexp_plus &) r).left << ")+";
      else 
	 os << *((regexp_plus &) r).left << "+";
   else if (typeid(r) == typeid(regexp_empty_str))
      ;
   else if (typeid(r) == typeid(regexp_empty_set))
      ;
   else
      os << "REGEXP_UNKNOWN";

   return os;
}

////////////////////////////// virtual dup ////////////////////

regexp* regexp_empty_set::dup() const { 
   return new regexp_empty_set(*this);
}

regexp* regexp_empty_str::dup() const { 
   return new regexp_empty_str(*this);
}

regexp* regexp_bol::dup() const { 
   return new regexp_bol;
}

regexp* regexp_eol::dup() const { 
   return new regexp_eol;
}

regexp* regexp_symbol::dup() const { 
   return new regexp_symbol((regexp_symbol&) *this);
}

regexp* regexp_cat::dup() const { 
   return new regexp_cat(*this);
}

regexp* regexp_or::dup() const { 
   return new regexp_or(*this);
}

regexp* regexp_star::dup() const { 
   return new regexp_star(*this);
}

regexp* regexp_tildastar::dup() const { 
   return new regexp_tildastar(*this);
}

regexp* regexp_tildaplus::dup() const { 
   return new regexp_tildaplus(*this);
}

regexp* regexp_question::dup() const { 
   return new regexp_question(*this);
}

regexp* regexp_plus::dup() const { 
   return new regexp_plus(*this);
}
