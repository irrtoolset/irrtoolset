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

#define BROKEN_RSD_BLANK_ASPATH

#include "config.h"
#include <cstring>
#include <iostream.h>
#include <iomanip.h>
#include <cctype>
#include "normalform/NE.hh"
#include "irr/irr.hh"
#include "irr/autnum.hh"
#include "RtConfig.hh"
#include "f_gated.hh"
#include "f_rsd.hh"
#include "rpsl/schema.hh"


#define DBG_RSD 7

void RSdConfig::printActions(PolicyActionList *actions) {
#define UNIMPLEMENTED_METHOD \
   cerr << "Warning: unimplemented method " \
	<< actn->rp_attr->name << "." << actn->rp_method->name << endl

   int pref = -1;
   int dpa  = -1;
   int med  = -1;

   PolicyAction *actn;
   for (actn = actions->head(); actn; actn = actions->next(actn)) {
      if (actn->rp_attr == dctn_rp_pref) {
	 if (actn->rp_method == dctn_rp_pref_set) {
	    pref = ((ItemINT *) actn->args->head())->i;
	 } else 
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_dpa) {
	 if (actn->rp_method == dctn_rp_dpa_set) {
	    dpa = ((ItemINT *) actn->args->head())->i;
	 } else 
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_med) {
	 if (actn->rp_method == dctn_rp_med_set) {
	    Item *item = actn->args->head();
	    if (typeid(*item) == typeid(ItemINT))
	       med = ((ItemINT *) item)->i;
	    else
	       UNIMPLEMENTED_METHOD;
	 } else 
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_community) {
	 if (actn->rp_method == dctn_rp_community_setop) {
	    UNIMPLEMENTED_METHOD;
	 } else if (actn->rp_method == dctn_rp_community_appendop) {
	    UNIMPLEMENTED_METHOD;
	 } else if (actn->rp_method == dctn_rp_community_append) {
	    UNIMPLEMENTED_METHOD;
	 } else
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_aspath) {
	 if (actn->rp_method == dctn_rp_aspath_prepend) {
	    UNIMPLEMENTED_METHOD;
	 } else
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      cerr << "Warning: unimplemented attribute " 
	   << *actn->rp_attr << endl;
   }

   if (pref != -1)
      cout << " preference " << normalizePref(pref) << " ";
   if (med != -1)
      cout << " MED " << med << " ";
   if (dpa != -1)
      cout << " DPA " << dpa << " ";
}

regexp *RSdConfig::fixBOLinRE(regexp *r, int asno) {
   if (typeid(*r) == typeid(regexp_bol)) {
      r = new regexp_cat(r, new regexp_symbol(asno));
   } else if (typeid(*r) == typeid(regexp_star)) {
      ((regexp_star *) r)->left = 
       fixBOLinRE(((regexp_star *) r)->left, asno);
   } else if (typeid(*r) == typeid(regexp_question)) {
      ((regexp_question *) r)->left = 
       fixBOLinRE(((regexp_question *) r)->left, asno);
   } else if (typeid(*r) == typeid(regexp_plus)) {
      ((regexp_plus *) r)->left = 
       fixBOLinRE(((regexp_plus *) r)->left, asno);
   } else if (typeid(*r) == typeid(regexp_cat)) {
      ((regexp_cat *) r)->left = 
       fixBOLinRE(((regexp_cat *) r)->left, asno);
      ((regexp_cat *) r)->right = 
       fixBOLinRE(((regexp_cat *) r)->right, asno);
   } else if (typeid(*r) == typeid(regexp_or)) {
      ((regexp_or *) r)->left = 
       fixBOLinRE(((regexp_or *) r)->left, asno);
      ((regexp_or *) r)->right = 
       fixBOLinRE(((regexp_or *) r)->right, asno);
   }

   return r;
}

Filter *RSdConfig::fixBOLinRE(Filter *n, int asno) {
   if (typeid(*n) == typeid(FilterOR)) {
      fixBOLinRE(((FilterOR *) n)->f1, asno);
      fixBOLinRE(((FilterOR *) n)->f2, asno);
   } else if (typeid(*n) == typeid(FilterAND)) {
      fixBOLinRE(((FilterAND *) n)->f1, asno);
      fixBOLinRE(((FilterAND *) n)->f2, asno);
   } else if (typeid(*n) == typeid(FilterNOT)) {
      fixBOLinRE(((FilterNOT *) n)->f1, asno);
   } else if (typeid(*n) == typeid(FilterASPath)) {
      ((FilterASPath *) n)->re 
	 = fixBOLinRE(((FilterASPath *) n)->re, asno);
   } 

   return n;
}

void RSdConfig::exportP(ASt asno, IPAddr *addr, 
		       ASt peerAS, IPAddr *peer_addr) {
   cerr << "Error: Unknown RtConfig/RSd command." << endl;
}

void RSdConfig::importP(ASt asno, IPAddr *addr, 
		       ASt peerAS, IPAddr *peer_addr) {
   // get the aut-num objects
   const AutNum *autnum = irr->getAutNum(asno);
   const AutNum *peerAutnum = irr->getAutNum(peerAS);
   static unsigned int lastImportingASNo = 0;

   if (!autnum) {
      cerr << "Error: no object for AS" << asno << endl;
      return;
   }

   if (lastImportingASNo != asno) {
      fillPrefMap(autnum);
      lastImportingASNo = asno;
   }

   if (!peerAutnum) {
      cerr << "Error: no object for AS" << peerAS << endl;
      return;
   }

   AutNumSelector<AttrImport> itr(autnum, "import", 
				  NULL, peerAS, peer_addr, addr);
   const FilterAction *fa = itr.first();
   if (! fa) {
      printPolicyWarning(asno, addr, peerAS, peer_addr, "import");
      return;
   }

   AutNumSelector<AttrExport> peerItr(peerAutnum, "export", 
				      NULL, asno, addr, peer_addr);
   const FilterAction *peerFa = peerItr.first();
   if (! peerFa) {
      printPolicyWarning(peerAS, peer_addr, asno, addr, "export");
      return;
   }

   // convert the policies of exporting as so that ^ is replaced by ^peerASno
   for (peerFa = peerItr.first(); peerFa; peerFa = peerItr.next())
      fixBOLinRE(peerFa->filter, peerAS);

   int last = 0;
   List<Result> rlist;
   NormalExpression *ne, *peerNe;
   // make rlist contain a list of (ne, pref) pairs
   // that will be imported
   for (peerFa = peerItr.first(); peerFa; peerFa = peerItr.next())
      for (fa = itr.first(); fa; fa = itr.next())  {
	 peerNe = NormalExpression::evaluate(peerFa->filter, asno);
	 ne = NormalExpression::evaluate(fa->filter, peerAS);
	 if (ne->isEmpty() || peerNe->isEmpty()) {
	    delete ne;
	    delete peerNe;
	    continue;
	 }
	 ne->do_and(*peerNe);

	 PolicyActionList *action = (PolicyActionList *) peerFa->action->dup();
	 action->splice(*(PolicyActionList *) fa->action->dup());

	 for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) {
	    Result *r = new Result;
	    r->ne = new NormalExpression;
	    *(r->ne) += new NormalTerm(*nt);
	    r->action = (PolicyActionList *) action->dup();
	    rlist.append(r);
	 }

	 if (ne->is_universal()) {
	    delete action;
	    delete ne;
	    delete peerNe;
	    break;
	 }

	 delete action;
	 delete ne;
	 delete peerNe;
      }

   // with n (as-path expression, netlist, pref) triplets
   // we have to generate 2^n-1 rsd import statements
   // because rsd stops after first as path match
   // Sep 5, 1995
   // rsd now has continue statement. So above statement is no longer true.
   // However, we are not going to take advantage of it for the time being.

   List<f_gated_node> result;
   f_gated_node *nptr;
   normalize(result, rlist);


   for (nptr = result.tail(); nptr; nptr = result.prev(nptr)) {
#ifdef BROKEN_RSD_BLANK_ASPATH
      if ((result.isSingleton() && nptr->as_path.is_universal())
	  || !(nptr->as_path.is_empty() || nptr->as_path.is_empty_str())) {
#endif // BROKEN_RSD_BLANK_ASPATH
	 cout << "import proto bgp ";
	    
	 if (result.isSingleton() && nptr->as_path.is_universal())
	    cout << "as " << peerAS << " {\n";
	 else {
	    cout << "aspath ";
	    printASPath(nptr->as_path);
	    cout << " origin any {\n";
	 }

	 printRoutes(nptr->contains, rlist, 1);

	 cout << "};\n\n";
#ifdef BROKEN_RSD_BLANK_ASPATH
      }
#endif // BROKEN_RSD_BLANK_ASPATH
   }

   result.clear();
   rlist.clear();
}
