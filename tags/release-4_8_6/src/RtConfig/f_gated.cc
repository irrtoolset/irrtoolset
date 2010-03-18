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
#include "rpsl/schema.hh"

#define DBG_GATED 7

void GatedConfig::printASPath(FilterOfASPath &re) {
   regexp *reg = re.re->construct();
   RangeList::prefix_str = "";
   cout << "<" << *reg << ">";
   delete reg;

   // ideally, we could just do:
   // cout << *re.re; 
   // but re.re may have ^ or $, the gated does not have
   // more work than it deserves ...
}

void GatedConfig::printActions(PolicyActionList *actions) {
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
      cout << " metric " << med << " ";
   if (dpa != -1)
      cout << " DPA " << dpa << " ";
}

void GatedConfig::printRoutes(SetOfPrefix &nets, PolicyActionList *action,
			      int import_flag) {    
   static char buffer[128];

   if (nets.isEmpty())
      return;

   RadixSet::SortedPrefixIterator itr(&nets.members);
   u_int addr;
   u_int leng;

   for (bool ok = itr.first(addr, leng); ok; ok = itr.next(addr, leng)) {
      if (!import_flag)
	 cout << "   ";
      cout << "   " << int2quad(buffer, addr) 
	   << " masklen " << leng << " exact";
      if (action) {
	 printActions(action); 
	 cout << ";\n";
      } else
	 cout << "\trestrict;\n";
   }
}

void GatedConfig::printRoutes(int include, List<Result> &rlist, 
			      int import_flag) {
   SetOfPrefix nets_covered;	 
   PolicyActionList *allAction = (PolicyActionList *) NULL;
   SetOfPrefix prfx;
   Result *r;

   for (r = rlist.head(); r; r = rlist.next(r), include >>= 1)
      if (include & 1) {
	 prfx = *r->prfx_set();
	 ~nets_covered;
	 prfx &= nets_covered;
	 ~nets_covered;
	 if (!prfx.isEmpty()) {
	    nets_covered |= prfx;
	    if (! allAction 
		&& (nets_covered.negated() || nets_covered.universal()))
	       allAction = r->action;
	    else 
	       printRoutes(prfx, r->action, import_flag);
	 }
      }

   if (! import_flag)
      cout << "   ";
   if (! allAction)
      cout << "   all restrict;\n";
   else {
      if (nets_covered.negated()) {
	 ~nets_covered;
	 printRoutes(nets_covered, NULL, import_flag);
      }
      cout << "   all ";
      printActions(allAction);
      cout << ";\n";
   }
}

// Gated first looks for an aspath match. If it finds one, it checks prefixes 
// associated with that aspath. If no prefix matches, gated stops the search.
// There may be a later aspath that also matches with a matching prefix 
// as well. There is no way to continue the search after an aspath match.
// the only work around is the following:
// with n (as-path expression, netlist, action) triplets
// we have to generate 2^n-1 gated import statements
// one for each possible aspath intersection combinations.

void GatedConfig::normalize(List<f_gated_node> &result, List<Result> &rlist) {
   f_gated_node *node, *nptr, *nptr2;
   unsigned int fill;
   unsigned int i;
   Result *r;
   int last = 0;

   for (i = 1, r = rlist.head(); r; r = rlist.next(r), i <<= 1) {
      for (nptr = result.head(); nptr; nptr = result.next(nptr))
	 if (r->as_path() && nptr->as_path == *r->as_path())
	    break;
      if (nptr)
	 nptr->contains |= i;
      else {
	 node = new f_gated_node;
	 node->as_path = *r->as_path();
	 node->contains = i;
	 result.append(node);
      }
      Debug(Channel(DBG_GATED) << hex << i << "\n");
   }

   for (nptr = result.head(); nptr; nptr = result.next(nptr)) {
      for (fill = 0, i = 1, r = rlist.head(); 
	   r; 
	   r = rlist.next(r), fill |= i, i <<= 1) {
	 Debug(Channel(DBG_GATED) << nptr->contains << " " << i << " ");
	 if (!(nptr->contains & i) && !(nptr->contains & fill)) {
	    node = new f_gated_node;
	    node->as_path = nptr->as_path;
	    node->as_path &= *r->as_path();
	    if (!node->as_path.isEmpty()) {
	       node->contains = nptr->contains | i;
	       result.append(node);
	       Debug(Channel(DBG_GATED) << hex << node->contains);
	    } else
	       delete node;
	 }
	 Debug(Channel(DBG_GATED) << "\n");
      }
   }
   
   FilterOfASPath as_path_covered, tmp_as_path;
   for (nptr = result.tail(), last = 0; 
	nptr && !last; 
	nptr = nptr2) {
      nptr2 = result.prev(nptr);
      if (!nptr->as_path.is_universal()) {
	 tmp_as_path = as_path_covered;
	 ~tmp_as_path;
	 nptr->as_path &= tmp_as_path;
      }
      if (nptr->as_path.isEmpty()) {
	 result.remove(nptr);
      } else {
	 tmp_as_path = nptr->as_path;
	 as_path_covered |= tmp_as_path;
	 last = as_path_covered.is_universal();
      }
   }
   for (nptr = nptr2; nptr && last; nptr = result.prev(nptr))
      result.remove(nptr);

}

void GatedConfig::importP(ASt asno, IPAddr *addr, 
			 ASt peerAS, IPAddr *peer_addr) {
   static unsigned int lastImportingASNo = 0;
   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);

   if (!autnum) {
      cerr << "Error: no object for AS" << asno << endl;
      return;
    }

   if (lastImportingASNo != asno) {
      fillPrefMap(autnum);
      lastImportingASNo = asno;
   }

   AutNumSelector<AttrImport> itr(autnum, "import", 
				  NULL, peerAS, peer_addr, addr);
   const FilterAction *fa = itr.first();
   if (! fa) {
      printPolicyWarning(asno, addr, peerAS, peer_addr, "import");
      return;
   }

   // for each import policy
   // make rlist contain a list of (ne, action) pairs
   // that will be imported
   List<Result> rlist;
   NormalExpression *ne;
   int last = 0;
   for (fa = itr.first(); fa && !last; fa = itr.next()) {
      ne = NormalExpression::evaluate(fa->filter, peerAS);
      if (ne->isEmpty()) {
	 delete ne;
	 continue;
      }

      for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) {
	 Result *r = new Result;
	 r->ne = new NormalExpression;
	 *(r->ne) += new NormalTerm(*nt);
	 r->action = (PolicyActionList *) fa->action->dup();
	 rlist.append(r);
      }

      if (ne->is_universal()) {
	 delete ne;
	 break;
      }
      delete ne;
   }

   if (rlist.size() > 32) {
      cerr << "Error: expression is too complicated for gated, contains more than 32 (as-path expression, netlist, pref) triplets, which may cause billions of gated import/export statements..." << endl;
      rlist.clear();
      return;
   }

   List<f_gated_node> result;
   normalize(result, rlist);

   f_gated_node *nptr;
   for (nptr = result.tail(); nptr; nptr = result.prev(nptr)) {
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
   }

   result.clear();
   rlist.clear();
}


void GatedConfig::exportP(ASt asno, IPAddr *addr, 
			 ASt peerAS, IPAddr *peer_addr) {
   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);

   if (!autnum) {
      cerr << "Error: no object for AS" << asno << endl;
      return;
    }

   AutNumSelector<AttrImport> itr(autnum, "export", 
				  NULL, peerAS, peer_addr, addr);
   const FilterAction *fa = itr.first();
   if (! fa) {
      printPolicyWarning(asno, addr, peerAS, peer_addr, "export");
      return;
   }

   // for each import policy
   // make rlist contain a list of (ne, action) pairs
   // that will be imported
   List<Result> rlist;
   NormalExpression *ne;
   int last = 0;
   for (fa = itr.first(); fa && !last; fa = itr.next()) {
      ne = NormalExpression::evaluate(fa->filter, peerAS);
      if (ne->isEmpty()) {
	 delete ne;
	 continue;
      }

      for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) {
	 Result *r = new Result;
	 r->ne = new NormalExpression;
	 *(r->ne) += new NormalTerm(*nt);
	 r->action = (PolicyActionList *) fa->action->dup();
	 rlist.append(r);
      }

      if (ne->is_universal()) {
	 delete ne;
	 break;
      }
      delete ne;
   }

   if (rlist.size() > 32) {
      cerr << "Error: expression is too complicated for gated, contains more than 32 (as-path expression, netlist, pref) triplets, which may cause billions of gated import/export statements..." << endl;
      rlist.clear();
      return;
   }

   List<f_gated_node> result;
   normalize(result, rlist);

   f_gated_node *nptr;
   for (nptr = result.tail(); nptr; nptr = result.prev(nptr)) {
      cout << "   proto bgp ";
	    
      if (result.isSingleton() && nptr->as_path.is_universal())
	 cout << "aspath .* origin any {\n";
      else {
	 cout << "aspath ";
	 printASPath(nptr->as_path);
	 cout << " origin any {\n";
      }

      printRoutes(nptr->contains, rlist, 0);

      cout << "   };\n\n";
   }

   result.clear();
   rlist.clear();
}

int GatedConfig::normalizePref(int pref) {
   for (int i = 0; prefMap[i] != -1 && i < 256; ++i)
      if (prefMap[i] == pref)
	 return i;

   return 255;
}

void GatedConfig::fillPrefMap(const AutNum *an) {
   prefMap[0] = -1;

   // walk over all imports of an
   AttrIterator<AttrImport> itr(an, "import");
   const AttrImport *import;

   const AttrProtocol *prot=schema.searchProtocol("BGP4");

   for (import = itr.first(); import; import = itr.next())
      if (import->fromProt == prot && import->intoProt == prot)
	 _fillPrefMap(import->policy);
}

void GatedConfig::_fillPrefMap(PolicyExpr *policy) {
   if (typeid(*policy) == typeid(PolicyTerm)) {

      for (ListIterator<PolicyFactor> pf(*(PolicyTerm *) policy); pf; ++pf)
	 for (ListIterator<PolicyPeeringAction> pa(*pf->peeringActionList); 
	      pa; 
	      ++pa)
	    for (ListIterator<PolicyAction> actn(*pa->action); actn; ++actn)
	       if (actn->rp_attr == dctn_rp_pref 
		   && actn->rp_method == dctn_rp_pref_set)
		  _fillPrefMap(((ItemINT *) actn->args->head())->i);
      
   } else if (typeid(*policy) == typeid(PolicyRefine)) {
      _fillPrefMap(((PolicyRefine *) policy)->left);
      _fillPrefMap(((PolicyRefine *) policy)->right);
   } else if (typeid(*policy) == typeid(PolicyExcept)) {
      _fillPrefMap(((PolicyExcept *) policy)->left);
      _fillPrefMap(((PolicyExcept *) policy)->right);
   }
}

void GatedConfig::_fillPrefMap(int pref) {
   int i;
   for (i = 0; prefMap[i] <= pref && prefMap[i] != -1 && i < 256; ++i)
      if (prefMap[i] == pref)
	 return;

   if (prefMap[i] != -1 && prefMap[i] > pref) {
      for (; prefMap[i] != -1 && i < 256; ++i) {
	 int save = prefMap[i];
	 prefMap[i] = pref;
	 pref = save;
      }
   }

   if (i < 255) {
      prefMap[i] = pref;
      prefMap[i+1] = -1;
   } else {
      prefMap[255] = -1;
      cerr << "Warning: more than 255 distinct preferences with gated/RSd." 
	   << endl;
   }
}
