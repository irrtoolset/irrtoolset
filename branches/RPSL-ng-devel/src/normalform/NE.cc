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
//             Katie Petrusha <katie@ripe.net>

#include "config.h"
#include <iostream.h>
#include "NE.hh"
#include "util/debug.hh"
#include "irr/irr.hh"
#include "irr/classes.hh"
#include "rpsl/schema.hh"
#include "gnug++/SetOfUInt.hh"
#include "gnu/prefixranges.hh"

#define DBG_REDUCE 3
#define DBG_REDUCE_DETAIL 4
#define DBG_OR 5
#define DBG_NOT 6
#define DBG_AND 7

CLASS_DEBUG_MEMORY_CC(NormalExpression);

NormalExpression::NormalExpression(NormalExpression& a) {
   singleton_flag = a.singleton_flag;
   for (Pix i = a.terms.first(); i; a.terms.next(i))
      terms.append(new NormalTerm(*a.terms(i)));
}

NormalExpression *NormalExpression::evaluate(const FilterRPAttribute *ptree, 
					     ASt peerAS,
					     unsigned int expand) {
   static const AttrRPAttr *comm_rp_attr = (AttrRPAttr *) NULL;
   static const AttrMethod *comm_contains = (AttrMethod *) NULL;
   static const AttrMethod *comm_fcall = (AttrMethod *) NULL;
   static const AttrMethod *comm_equals = (AttrMethod *) NULL;

   if (!comm_rp_attr)
	 comm_rp_attr = schema.searchRPAttr("community");
   if (!comm_contains)
	 comm_contains = comm_rp_attr->searchMethod("contains");
   if (!comm_fcall)
	 comm_fcall = comm_rp_attr->searchMethod("()");
   if (!comm_equals)
	 comm_equals = comm_rp_attr->searchMethod("==");
   
   NormalExpression *ne = new NormalExpression;
   
   if (ptree->rp_attr != comm_rp_attr) {
      cerr << "Warning: unimplemented rp-attribute " 
	   << ptree->rp_attr->name << " in filter, assuming NOT ANY.\n";
      return ne;
   }
   if (ptree->rp_method != comm_contains
       && ptree->rp_method != comm_fcall
       && ptree->rp_method != comm_equals) {
      cerr << "Warning: unimplemented method " << ptree->rp_attr->name << "."
	   << ptree->rp_method->name << " in filter, assuming NOT ANY.\n";
      return ne;
   }

   NormalTerm *nt = new NormalTerm;

   if (ptree->rp_method == comm_equals) {
      CommunitySet *clist = new CommunitySet;
      ItemList *arg1 = (ItemList *) ptree->args->head();
      for (Item *nd = arg1->head(); nd; nd = arg1->next(nd))
	 clist->addCommunity(nd);
      nt->community.set(clist, true);
   } else
      for (Item *nd = ptree->args->head(); 
	   nd; 
	   nd = ptree->args->next(nd)) {
	 FilterOfCommunity *fc = new FilterOfCommunity;
	 fc->set(new CommunitySet(nd));
	 nt->community |= *fc;
	 delete fc;
      }

   nt->make_universal(NormalTerm::COMMUNITY);     // this makes it universal
   ne->singleton_flag = NormalTerm::COMMUNITY;

   *ne += nt;
   return ne;
}

NormalExpression *NormalExpression::evaluate(const Filter *ptree, 
					     ASt peerAS,
					     unsigned int expand) {
   NormalExpression *ne, *ne2;
   NormalTerm *nt;

   if (typeid(*ptree) == typeid(FilterOR)) {
      ne  = evaluate(((FilterOR *) ptree)->f1, peerAS, expand);
      if (ne->is_universal())
	 return ne;

      ne2 = evaluate(((FilterOR *) ptree)->f2, peerAS, expand);
      Debug(Channel(DBG_OR) << "op1: " << *ne << "\n");
      Debug(Channel(DBG_OR) << "op2: " << *ne2 << "\n");
      ne->do_or(*ne2);
      Debug(Channel(DBG_OR) << "or:  " << *ne << "\n");
      delete ne2;

      return ne;
   }

   if (typeid(*ptree) == typeid(FilterAND)) {
      ne  = evaluate(((FilterAND *) ptree)->f1, peerAS, expand);
      if (ne->isEmpty())
	 return ne;

      ne2 = evaluate(((FilterAND *) ptree)->f2, peerAS, expand);
      Debug(Channel(DBG_AND) << "op1: " << *ne << "\n");
      Debug(Channel(DBG_AND) << "op2: " << *ne2 << "\n");
      ne->do_and(*ne2);
      Debug(Channel(DBG_AND) << "and: " << *ne << "\n");
      delete ne2;

      return ne;
   }

   if (typeid(*ptree) == typeid(FilterNOT)) {
      ne = evaluate(((FilterNOT *) ptree)->f1, peerAS, expand);
      Debug(Channel(DBG_NOT) << "op1: " << *ne << "\n");
      ne->do_not();
      Debug(Channel(DBG_NOT) << "not: " << *ne << "\n");
      return ne;
   }

   if (typeid(*ptree) == typeid(FilterMS)) {
      ne = evaluate(((FilterMS *) ptree)->f1, peerAS, expand);
      Debug(Channel(DBG_NOT) << "op1: " << *ne << "\n");
      ne->makeMoreSpecific(((FilterMS *) ptree)->code,
			   ((FilterMS *) ptree)->n,
			   ((FilterMS *) ptree)->m);
      Debug(Channel(DBG_NOT) << "ms: " << *ne << "\n");
      return ne;
   }

   if (typeid(*ptree) == typeid(FilterASNO)) {
      ne = new NormalExpression;

      if (expand & EXPAND_AS) {
	 const PrefixRanges *s 
	    = irr->expandAS(((FilterASNO *) ptree)->asno);
	 if (!s || s->isEmpty())
	    return ne;

	 nt = new NormalTerm;
	 nt->prfx_set = * (PrefixRanges *) s;        // radix tree copied here
	 nt->make_universal(NormalTerm::PRFX);       // this makes it universal
	 ne->singleton_flag = NormalTerm::PRFX;
      } else {
	 nt = new NormalTerm;
	 nt->symbols.add(((FilterASNO *) ptree)->asno);   
	 nt->make_universal(NormalTerm::SYMBOLS);    // this makes it universal
	 ne->singleton_flag = NormalTerm::SYMBOLS;
      }

      *ne += nt;
      return ne;
   }

   if (typeid(*ptree) == typeid(FilterPeerAS)) {
      ne = new NormalExpression;

      if (expand & EXPAND_AS) {
	 const PrefixRanges *s 
	    = irr->expandAS(peerAS);
	 if (!s || s->isEmpty())
	    return ne;

	 nt = new NormalTerm;
	 nt->prfx_set = * (PrefixRanges *) s;        // radix tree copied here
	 nt->make_universal(NormalTerm::PRFX);       // this makes it universal
	 ne->singleton_flag = NormalTerm::PRFX;
      } else {
	 nt = new NormalTerm;
	 nt->symbols.add(peerAS);   
	 nt->make_universal(NormalTerm::SYMBOLS);    // this makes it universal
	 ne->singleton_flag = NormalTerm::SYMBOLS;
      }

      *ne += nt;
      return ne;
   }

   if (typeid(*ptree) == typeid(FilterASNAME)) {
      ne = new NormalExpression;

      SymID name = symbols.resolvePeerAS(((FilterASNAME *) ptree)->
					 asname, peerAS);

      if (expand & EXPAND_AS_MACROS) {
	 const SetOfUInt *s = irr->expandASSet(name);
	 if (!s || s->isEmpty())
	    return ne;

	 if (expand & EXPAND_AS) {
	    nt = new NormalTerm;

	    const PrefixRanges *sr;
	    for (Pix p = s->first(); p; s->next(p)) {
	       sr = irr->expandAS((*s)(p));
	       if (sr)
		  nt->prfx_set |= *sr;
	    }

	    if (nt->prfx_set.isEmpty()) {
	       delete nt;
	       return ne;
	    }

	    nt->make_universal(NormalTerm::PRFX);    // this makes it universal
	    ne->singleton_flag = NormalTerm::PRFX;
	 } else {
	    // set assignment makes a copy
	    nt = new NormalTerm;
	    nt->symbols = * s;
	    nt->make_universal(NormalTerm::SYMBOLS); // this makes it universal
	    ne->singleton_flag = NormalTerm::SYMBOLS;
	 }
      } else {
	 nt = new NormalTerm;
	 nt->symbols.add(name);
	 nt->make_universal(NormalTerm::SYMBOLS);  // this makes it universal
	 ne->singleton_flag = NormalTerm::SYMBOLS;
      }
      
      *ne += nt;
      return ne;
   }

   if (typeid(*ptree) == typeid(FilterRSNAME)) {
      ne = new NormalExpression;

      SymID name = symbols.resolvePeerAS(((FilterRSNAME *) ptree)->rsname, 
					 peerAS);

      if (expand & EXPAND_COMMUNITIES) {
	 const PrefixRanges *s = irr->expandRSSet(name);
	 if (!s || s->isEmpty())
	    return ne;
	 // set assignment makes a copy
	 nt = new NormalTerm;
	 nt->prfx_set = * (PrefixRanges *) s;
	 nt->make_universal(NormalTerm::PRFX);
	 ne->singleton_flag = NormalTerm::PRFX;
      } else {
	 nt = new NormalTerm;
	 nt->symbols.add(name);
	 nt->make_universal(NormalTerm::SYMBOLS); // this makes it universal
	 ne->singleton_flag = NormalTerm::SYMBOLS;
      }

      *ne += nt;
      return ne;
   }

   if (typeid(*ptree) == typeid(FilterANY)) {
      ne = new NormalExpression;
      ne->become_universal();
      return ne;
   }

   if (typeid(*ptree) == typeid(FilterFLTRNAME)) {
      cout << "eval. FilterFLTRNAME" << endl;
      SymID name = symbols.resolvePeerAS(((FilterFLTRNAME *) ptree)->fltrname, peerAS);
      const FilterSet *fset = irr->getFilterSet(name);
      if (fset) {
      	 AttrIterator<AttrFilter> itr(fset, "filter");
	       if (itr) {
	         return evaluate(itr()->filter, peerAS, expand);
         }

         AttrIterator<AttrFilter> itr1(fset, "mp-filter"); 
         if (itr1)
           return evaluate(itr1()->filter, peerAS, expand);
      }

      ne = new NormalExpression;
      return ne;
   }

   if (typeid(*ptree) == typeid(FilterASPath)) {
      ne = new NormalExpression;
      nt = new NormalTerm;

      nt->as_path.compile(((FilterASPath *) ptree)->re, peerAS);
      nt->make_universal(NormalTerm::AS_PATH);       // this makes it universal
      ne->singleton_flag = NormalTerm::AS_PATH;

      *ne += nt;
      return ne;
   }
   
   if (typeid(*ptree) == typeid(FilterPRFXList)) {
      ne = new NormalExpression;
      nt = new NormalTerm;

      cout << "eval. FilterPRFXList" << endl;
      nt->prfx_set |= * (FilterPRFXList *) ptree; //RadixSet created here
      nt->make_universal(NormalTerm::PRFX);	     // this makes it universal
      ne->singleton_flag = NormalTerm::PRFX;

      *ne += nt;
      return ne;
   }

   if (typeid(*ptree) == typeid(FilterRPAttribute))
      return evaluate((FilterRPAttribute *) ptree, peerAS, expand);


   if (typeid(*ptree) == typeid(FilterAFI)) {
   
      cout << "eval. FilterAFI" << endl;
      ne = evaluate(((FilterAFI *) ptree)->f, peerAS, expand);
      cout << "NE before restrict" << endl;
      cout << *ne;
      cout << " END" << endl;
      ne->restrict((FilterAFI *) ptree);
      Debug(Channel(DBG_NOT) << "op1: " << *ne << "\n");
      Debug(Channel(DBG_NOT) << "afi: " << *ne << "\n");
      return ne;

   }

   if (typeid(*ptree) == typeid(FilterMPPRFXList)) {
      cout << "eval. FilterMPPRFXList" << endl;

      // mixed v4/v6 prefix lists
      FilterPRFXList *list_v4 = ((FilterMPPRFXList *) ptree)->get_v4();
      FilterMPPRFXList *list_v6 = ((FilterMPPRFXList *) ptree)->get_v6();

      ne = new NormalExpression;
      if (list_v4) {
        nt = new NormalTerm;
        nt->prfx_set |= * (FilterPRFXList *) list_v4; // radix set created here
        nt->make_universal(NormalTerm::PRFX);      // this makes it universal
        *ne += nt;
      }
      if (list_v6) {
        nt = new NormalTerm;
        nt->ipv6_prfx_set |= * (FilterMPPRFXList *) list_v6; // radix set created here
        nt->make_universal(NormalTerm::IPV6_PRFX);      // this makes it universal
        *ne += nt;
      }
      return ne;
   }

   assert(0);
}

ostream& operator<<(ostream& stream, NormalExpression& ne) {
   NormalTerm *term;
   Pix i;

   switch (ne.is_any()) {
   case ANY: 
      stream << "ANY";
      break;
   case NOT_ANY:
      stream << "NOT ANY";
      break;
   default:
      for (i = ne.terms.first(); i; ) {
	 term = ne.terms(i);
	 stream << "(" << *term << ")";
	 ne.terms.next(i);
	 if (i)
	    stream << "\n OR ";
      }
   }
   return stream;
}

void NormalExpression::reduce() {
   NormalTerm *term, *otherterm;
   int change = 1;
   Pix pixi, pixj, nextpixj;
   int j;

   // worst case O(N^3) Sigh.
   // can be improved by keeping sorted lists on set sizes
   while (change) {
      change = 0;
      for (pixi = terms.first(); pixi; terms.next(pixi)) {
	 term = terms(pixi);
	 for (pixj = pixi, terms.next(pixj); pixj; pixj = nextpixj) {
	    otherterm = terms(pixj);
	    nextpixj = pixj;
	    terms.next(nextpixj);

	    Debug(Channel(DBG_REDUCE_DETAIL) << *this << "\n");
	    
	    j = term->find_diff(*otherterm); // returns -1 if more than 1 diff
	    if (j >= 0) { 
	       // term and otherterm are identical 
	       // except in the set indexed by j
	       if (j != NormalTerm::FILTER_COUNT) // o/w *term == *otherterm
		  (*term)[j] |= (*otherterm)[j];
		  
	       delete otherterm;
	       terms.del(pixj);

	       change = 1;

	       int jj;
	       for (jj = 0; jj < NormalTerm::FILTER_COUNT; jj++)
		  if (!(*term)[jj].is_universal())
		     break;
	       if (jj == NormalTerm::FILTER_COUNT) { 
		  // we became ANY
		  become_universal();
		  return;
	       }
	    }
	 }
	 Debug(Channel(DBG_REDUCE) << *this << "\n");
      }
   }
}

void NormalExpression::do_or(NormalExpression &other) {
   NormalTerm *term, *otherterm;

   if (is_any() == ANY)
      return;
   if (other.is_any() == NOT_ANY)
      return;

   if (other.is_any() == ANY) { // become ANY
      become_universal();
      return;
   }

   if (is_any() == NOT_ANY) { // become other
      terms.join(other.terms);
      singleton_flag = other.singleton_flag;
      return;
   }

   assert(is_any() == NEITHER && other.is_any() == NEITHER);

   if (singleton_flag >= 0 && other.singleton_flag == singleton_flag) { 
      // special case for speedup
      term = terms(terms.first());
      otherterm = other.terms(other.terms.first());
      (*term)[singleton_flag] |= (*otherterm)[singleton_flag];
      if ((*term)[singleton_flag].is_universal())
	 become_universal();
      return;
   } 

   // GENERAL CASE

   singleton_flag = -1;

   // add his terms to my terms
   terms.join(other.terms);

   Debug(Channel(DBG_OR) << *this << "\n");
 
   // get rid of duplicate terms
   reduce();
}

void NormalExpression::do_and(NormalExpression &other) {
   NormalTerm *term, *newt, *otherterm;
   NormalExpression result;

   if (is_any() == NOT_ANY)
      return;
   if (other.is_any() == ANY)
      return;

   if (other.is_any() == NOT_ANY) { // become NOT ANY
      become_empty();
      return;
   }

   if (is_any() == ANY) { // become other
      terms.clear();
      terms.join(other.terms);
      singleton_flag = other.singleton_flag;
      return;
   }

   assert(is_any() == NEITHER && other.is_any() == NEITHER);

   if (singleton_flag >= 0 && other.singleton_flag == singleton_flag) { 
      // special case for speedup
      term = terms(terms.first());
      otherterm = other.terms(other.terms.first());
      (*term)[singleton_flag] &= (*otherterm)[singleton_flag];
      if ((*term)[singleton_flag].isEmpty())
	 become_empty();
      return;
   } 

   singleton_flag = -1;

   if (terms.length() == 1 && other.terms.length() == 1) { 
      // special case for speedup
      term = terms(terms.first());
      otherterm = other.terms(other.terms.first());
      for (int i = 0; i < NormalTerm::FILTER_COUNT; i++) {
	 (*term)[i] &= (*otherterm)[i];
	 if ((*term)[i].isEmpty()) {
	    become_empty();
	    break;
	 }
      }
      return;
   }

   // GENERAL CASE

   // do a cartesian product
   NormalTerm *tempt = new NormalTerm; //use tempt to take the hit
   for (Pix pixi = terms.first(); pixi;  terms.next(pixi)) {
      term = terms(pixi);
      for (Pix pixj = other.terms.first(); pixj; other.terms.next(pixj)) {
	 otherterm = other.terms(pixj);
	 newt = new NormalTerm;

	 for (int i=0; i < NormalTerm::FILTER_COUNT; i++) {
	    (*newt)[i]  = (*term)[i];
            (*tempt)[i] = (*otherterm)[i]; //copy into tempt
            (*newt)[i] &= (*tempt)[i];     //blast tempt, not otherterm!
	    if ((*newt)[i].isEmpty()) {
	       delete newt;
	       goto skip_otherterm;
	    }
	 }

	 result.terms.append(newt);
	       
	skip_otherterm: ;
      }
   }
   delete tempt;
   
   terms.clear();
   terms.join(result.terms);
   if (!terms.empty())
      reduce();   // get rid of duplicate terms
}

void NormalExpression::restrict(FilterAFI *af) {

   NormalTerm *term = new NormalTerm;
   NormalExpression *ne = new NormalExpression (*this);
   become_empty();
   if (af->afi_item->is_ipv6()) { //v6
      this->singleton_flag = NormalTerm::IPV6_PRFX;
      for (term = ne->first(); term ; term = ne->next())
        if (term->prfx_set.universal())
          *this += term;
   } 
   else { //v4
      this->singleton_flag = NormalTerm::PRFX;
      for (term = ne->first(); term ; term = ne->next())
        if (term->ipv6_prfx_set.universal())
           *this += term;
   }
}

void NormalExpression::do_not() {
// the value of singleton_flag is not affected by do_not
   NormalTerm *term, *newt;
   NormalExpression result;
   NormalExpression tmpexp;

   if (is_any() == NOT_ANY) { // become ANY
      become_universal();
      return;
   }
   if (is_any() == ANY) { // become NOT_ANY
      become_empty();
      return;
   }

   if (singleton_flag >= 0) { // special case for speedup
      ~(*terms(terms.first()))[singleton_flag];
      return;
   } 


   // GENERAL CASE
   result.become_universal();
 
   for (Pix pixi = terms.first(); pixi;  terms.next(pixi)) {
      term = terms(pixi);
      for (int i = 0; i < NormalTerm::FILTER_COUNT; i++) {
	 if (!(*term)[i].is_universal()) {
	    newt = new NormalTerm;
	    (*newt)[i] = (*term)[i];
	    ~(*newt)[i];

	    newt->make_universal(i);
	    
	    tmpexp.terms.append(newt);
	 }
      }

      Debug(Channel(DBG_NOT) << tmpexp << "\n"); 
      result.do_and(tmpexp);
      tmpexp.terms.clear(); // this also free's elements' memory
   }

   terms.clear();
   terms.join(result.terms);
}

