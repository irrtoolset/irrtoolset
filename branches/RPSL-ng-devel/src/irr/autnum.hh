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

#ifndef AUTNUM_H
#define AUTNUM_H

#include "rpsl/object.hh"
#include "gnug++/SetOfUInt.hh"
#include "util/List.hh"
#include "rpsl/schema.hh"
#include "irr/irr.hh"
#include "rpsl/prefix.hh"
#include "rpsl/rpsl_policy.hh"
#include <vector>
#include <string>

#define INVALID_AS    ~0

class Peering : public ListNode {
public:
   ASt    peerAS;
   IPAddr peerIP;
   IPAddr localIP;

public:
   Peering(ASt _peerAS, const IPAddr &_peerIP, const IPAddr &_localIP) 
      : ListNode(), peerAS(_peerAS), peerIP(_peerIP), localIP(_localIP) {
   }
   Peering(const Peering &b) 
      : ListNode(), peerAS(b.peerAS), peerIP(b.peerIP), localIP(b.localIP) {
   }
   ~Peering() {
   }

   bool operator < (const Peering &b) const {
      return (peerAS < b.peerAS) 
	 || (peerAS == b.peerAS && peerIP < b.peerIP) 
	 || (peerAS == b.peerAS && peerIP == b.peerIP && localIP < b.localIP);
   }
   bool operator == (const Peering &b) const {
      return peerAS == b.peerAS && peerIP == b.peerIP && localIP == b.localIP;
   }
};

class AutNum : public Object {
   friend class AutNumPeeringIterator;
private:
   SortedList<Peering> *peerings;
   void gatherPeerings(PolicyExpr *policy, SortedList<Peering> *peerings);
   void gatherPeerings(PolicyPeering *peering, SortedList<Peering> *peerings);
   void gatherASes(Filter *f, SetOfUInt *result);
   void gatherRouters(Filter *f, SetOfUInt *result);

public:
   AutNum() : Object(), peerings(NULL) {}
   AutNum(Buffer &buf) : Object(buf), peerings(NULL) {}
   AutNum(const AutNum &au) : Object(au), peerings(NULL) {
   }
   ~AutNum() {
      if (peerings)
	 delete peerings;
   }

   void gatherPeerings();
   // Added by wlee@isi.edu
   void removePeer(ASt peerAS);
   ASt asno(void) const;
};


class FilterAction : public ListNode {
public:
   Filter       *filter;
   PolicyActionList *action;
public:
   FilterAction(Filter *f, PolicyActionList *a) : filter(f), action(a) {}

   FilterAction(const FilterAction &b) {
      filter = (Filter *) b.filter->dup();
      action = (PolicyActionList *) b.action->dup();
   }

   ~FilterAction() {
      delete filter;
      delete action;
   }
};

static bool isPeeringMatching(Filter *f, const ASt as) {
   if (typeid(*f) == typeid(FilterASNO))
      return ((FilterASNO *) f)->asno == as;
   if (typeid(*f) == typeid(FilterASNAME)) {
      SymID set = ((FilterASNAME *) f)->asname;
      if (symbols.isASAny(set))
	 return true;
      if (!irr)
	 return false;
      const SetOfUInt *asSet = irr->expandASSet(set);
      return (asSet && asSet->contains(as));
   }
   if (typeid(*f) == typeid(FilterAND))
      return isPeeringMatching(((FilterAND *) f)->f1, as)
	 && isPeeringMatching(((FilterAND *) f)->f2, as);
   if (typeid(*f) == typeid(FilterEXCEPT))
      return isPeeringMatching(((FilterEXCEPT *) f)->f1, as)
	 && ! isPeeringMatching(((FilterEXCEPT *) f)->f2, as);
   if (typeid(*f) == typeid(FilterOR))
      return isPeeringMatching(((FilterOR *) f)->f1, as)
	 || isPeeringMatching(((FilterOR *) f)->f2, as);
   assert(false);
   return false;
}

static bool isPeeringMatching(Filter *f, const IPAddr *ip) {
   if (typeid(*f) == typeid(FilterANY))
      return true;
   if (typeid(*f) == typeid(FilterRouter))
      return ((FilterRouter *) f)->ip->get_ipaddr() == ip->get_ipaddr();
   if (typeid(*f) == typeid(FilterRouterName)) {
      char *dns = ((FilterRouterName *) f)->name;
      if (!irr)
	 return false;
      const InetRtr *rtr = irr->getInetRtr(dns);
      if (!rtr)
	 return false;
      AttrIterator<AttrIfAddr> itr(rtr, "ifaddr");
      while (itr) {
	 if (itr()->ifaddr.get_ipaddr() == ip->get_ipaddr())
	    return true;
	 itr++;
      }

      return false;
   }
   if (typeid(*f) == typeid(FilterRTRSNAME)) {
      SymID set = ((FilterRTRSNAME *) f)->rtrsname;
      if (!irr)
	 return false;

      const PrefixRanges *rtrSet = irr->expandRtrSet(set);
      return (rtrSet && rtrSet->contains(*ip));
   }
   if (typeid(*f) == typeid(FilterAND))
      return isPeeringMatching(((FilterAND *) f)->f1, ip)
	 && isPeeringMatching(((FilterAND *) f)->f2, ip);
   if (typeid(*f) == typeid(FilterEXCEPT))
      return isPeeringMatching(((FilterEXCEPT *) f)->f1, ip)
	 && ! isPeeringMatching(((FilterEXCEPT *) f)->f2, ip);
   if (typeid(*f) == typeid(FilterOR))
      return isPeeringMatching(((FilterOR *) f)->f1, ip)
	 || isPeeringMatching(((FilterOR *) f)->f2, ip);
   if (typeid(*f) == typeid(FilterANY))
      return true;
   assert(false);
   return false;
}

static bool isPeeringMatching(PolicyPeering *prng, SymID pset,
		const ASt peerAS, const IPAddr *peerIP, const IPAddr *ip) {
   if (!prng)
      return false;

   if (prng->prngSet) {
      if (pset)
	 return prng->prngSet == pset;
      else {
	 const PeeringSet *peers = irr->getPeeringSet(prng->prngSet);
	 if (!peers)
	    return false;
	 for (AttrIterator<AttrPeering> itr(peers, "peering"); itr; itr++)
	    if (isPeeringMatching(itr()->peering, pset, peerAS, peerIP, ip))
	       return true;
	 
	 return false;
      }
   }

   if (pset)
      return false;

   if (prng->peerRtrs && peerIP && !isPeeringMatching(prng->peerRtrs, peerIP))
      return false;
      
   if (prng->localRtrs && ip && !isPeeringMatching(prng->localRtrs, ip))
      return false;

   if (prng->peerASes && peerAS != INVALID_AS 
       && !isPeeringMatching(prng->peerASes, peerAS))
      return false;

   return true;
}

class AutNumDefaultIterator : public AttrIterator<AttrDefault> {
private:
   const ASt peerAS;
   const IPAddr *peerIP;
   const IPAddr *ip;

// Made it protected by wlee
protected:      
   virtual bool isMatching(Attr *attr) {
      return isPeeringMatching(((AttrDefault *) attr)->peering, 
			       NULL, peerAS, peerIP, ip);
   }

public:
   AutNumDefaultIterator(const AutNum *an,
			 const ASt _peerAS = INVALID_AS, 
			 const IPAddr *_peerIP = NULL, 
			 const IPAddr *_ip = NULL):
     AttrIterator<AttrDefault>(an, "default"), 
     peerAS(_peerAS), peerIP(_peerIP), ip(_ip) {}
};


template <class AttrType>
class AutNumImportExportIterator : public AttrIterator<AttrType> {
private:
  const ASt peerAS;

protected:
  virtual bool isMatching(Attr *attr) {
    if (!attr) return false;
    PolicyExpr *policy = ((AttrType *)attr)->policy;
    return isMatching(policy);
  }
  virtual bool isMatching(PolicyExpr *policy) {
    if (typeid(*policy) == typeid(PolicyTerm))
      {
      PolicyTerm *pt = (PolicyTerm *)policy;
      for (ListIterator<PolicyFactor> i(*pt); i; ++i)
        // modified and commented out by katie@ripe.net
	for (ListIterator<PolicyPeeringAction> j(*(i->peeringActionList)); j; ++j)	
          // isPeeringMatching for extracting AS from peering-sets and as-sets as peerings
	  if (j->peering && isPeeringMatching(j->peering, NULL, peerAS, NULL, NULL)) return true;

       // never returned true!
/*#if 0
	  if (j()->peering && j()->peering->peerAS == peerAS) return true;


#else
      ;
#endif */
    }
    else
      if (typeid(*policy) == typeid(PolicyRefine))
	{
	PolicyRefine *pr = (PolicyRefine *)policy;
	if (pr->left && isMatching(pr->left)) return true;
	if (pr->right && isMatching(pr->right)) return true;
	}
      else
	if (typeid(*policy) == typeid(PolicyExcept))
	  {
	  PolicyExcept *pe = (PolicyExcept *)policy;
	  if (pe->left && isMatching(pe->left)) return true;
	  if (pe->right && isMatching(pe->right)) return true;
	  }
	else
	  {
	  cerr << "Internal Error!" << endl;
	  abort();
	  }
    return false;
  }

public:
   AutNumImportExportIterator(const AutNum *an, 
			      const char *attrib,
			      const ASt _peerAS) :
      AttrIterator<AttrType>(an, attrib),
      peerAS(_peerAS) {
	// Has to be called here again in order to get things right!
	// It's not perfect but it works
	first();	
   }
};


class AutNumImportIterator : public AutNumImportExportIterator<AttrImport> {
public:
  AutNumImportIterator(const AutNum *an, const ASt _peerAS) :
    AutNumImportExportIterator<AttrImport>(an, "import", _peerAS) {}
};


class AutNumExportIterator : public AutNumImportExportIterator<AttrExport> {
public:
  AutNumExportIterator(const AutNum *an, const ASt _peerAS) :
    AutNumImportExportIterator<AttrExport>(an, "export", _peerAS) {}
};


template<class AttrType> 
class AutNumSelector {
private:
   List<FilterAction> filterActionList;
   FilterAction      *current;
public:
   AutNumSelector(const AutNum *an, const char *attrib, SymID pset,
                  const ASt peerAS, const IPAddr *peerIP, const IPAddr *ip,
                  char *fProtName = "BGP4", char *iProtName = "BGP4"):
      current(NULL) {
      AttrIterator<AttrType> itr(an, attrib);
      const AttrType *import;
      List<FilterAction> *list;
      const AttrProtocol *fProt=schema.searchProtocol(fProtName);
      const AttrProtocol *iProt=schema.searchProtocol(iProtName);

      for (import = itr.first(); import; import = itr.next()) {
         if (import->fromProt == fProt && import->intoProt == iProt) {
            list = select(import->policy, pset, peerAS, peerIP, ip);
            if (list) {
               filterActionList.splice(*list);
               delete list;
            }
         }
      }
   }

   FilterAction *first() {
      current = filterActionList.head();
      return current;
   }

   FilterAction *next() {
      if (current)
         current = filterActionList.next(current);
      return current;
   }

private:
   List<FilterAction> *select(PolicyExpr *policy, SymID pset,
                                  const ASt peerAS, const IPAddr *peerIP, 
                                  const IPAddr *ip,
                                  Filter **combinedFilter = NULL) {
      if (typeid(*policy) == typeid(PolicyTerm)) {
         List<FilterAction> *list = new List<FilterAction>;
         FilterAction *filterAction;
         PolicyTerm   *pt = (PolicyTerm *) policy;
         PolicyFactor *pf;
         PolicyPeeringAction *pa;
         bool          noMatch;

         for (pf = pt->head(); pf; pf = pt->next(pf)) {
            noMatch = true;
            for (pa = pf->peeringActionList->head(); 
                 pa; 
                 pa = pf->peeringActionList->next(pa)) {
               if (noMatch 
		   && isPeeringMatching(pa->peering, pset,peerAS,peerIP,ip)) {
                  list->append((new FilterAction((Filter *) 
                                                 pf->filter->dup(), 
                                                 (PolicyActionList *) 
                                                 pa->action->dup())));
                  // rpsl specification order rule dictates only one match here
                  // so break out of this if statement
                  noMatch = false;
                  if (!combinedFilter)
                     break;
               }
               if (combinedFilter)
                  if (*combinedFilter)
                     *combinedFilter = 
                        new FilterOR(*combinedFilter, 
                                          (Filter *) pf->filter->dup());
                  else
                     *combinedFilter = (Filter *) pf->filter->dup();
            }
         }
         
         if (! list->isEmpty())
            return list;
         else {
            delete list;
            return NULL;
         }
      }

      if (typeid(*policy) == typeid(PolicyRefine)) {
         PolicyRefine *pr = (PolicyRefine *) policy;
         List<FilterAction> *left;
         List<FilterAction> *rght;
         if (combinedFilter) {
            Filter  *leftFilter = NULL;
            Filter  *rghtFilter = NULL;
            left = select(pr->left, pset, peerAS, peerIP, ip, &leftFilter);
            rght = select(pr->right, pset, peerAS, peerIP, ip, &rghtFilter);
            *combinedFilter = new FilterAND(leftFilter, rghtFilter);
         } else {
            left = select(pr->left, pset, peerAS, peerIP, ip, NULL);
            rght = select(pr->right, pset, peerAS, peerIP, ip, NULL);
         }

         if (!left || !rght) {
            if (left)
               delete left;
            if (rght)
               delete rght;
            return NULL;
         }

         List<FilterAction> *list = new List<FilterAction>;
         FilterAction *l, *r;
         Filter  *fltr;
         PolicyActionList  *actn, *actn2;

         for (l = left->head(); l; l = left->next(l)) 
            for (r = rght->head(); r; r = rght->next(r)) {
               fltr = new FilterAND((Filter  *) l->filter->dup(), 
                                         (Filter  *) r->filter->dup());
               actn = (PolicyActionList *) l->action->dup();
               actn2 = (PolicyActionList *) r->action->dup();
               actn->splice(*actn2);
               delete actn2;

               list->append((new FilterAction(fltr, actn)));
            }

         delete left;
         delete rght;
         return list;
      }

      if (typeid(*policy) == typeid(PolicyExcept)) {
         PolicyExcept *pe = (PolicyExcept *) policy;
         Filter  *leftFilter = NULL;
         Filter  *rghtFilter = NULL;
         List<FilterAction> *left = select(pe->left, pset, peerAS, peerIP, ip, 
                                               &leftFilter);
         List<FilterAction> *rght = select(pe->right, pset, peerAS, peerIP, ip,
                                               &rghtFilter);

         if (combinedFilter)
            *combinedFilter = (Filter  *) leftFilter->dup();

         if (!left && !rght) {
            delete leftFilter;
            delete rghtFilter;
            return NULL;
         }

         if (left) {
            FilterAction *filterAction;
            for (filterAction = left->head();
                 filterAction;
                 filterAction = left->next(filterAction))
               filterAction->filter = new FilterAND(filterAction->filter,
                  new FilterNOT((Filter  *) rghtFilter->dup()));
         }
         delete rghtFilter;

         if (rght) {
            FilterAction *filterAction;
            for (filterAction = rght->head();
                 filterAction;
                 filterAction = rght->next(filterAction))
               filterAction->filter = new FilterAND(filterAction->filter,
                                   (Filter  *) leftFilter->dup());
         }
         delete leftFilter;

         if (!left)
            return rght;

         if (!rght)
            return left;

         rght->splice(*left);
         delete left;

         return rght;
      }

      assert(0);
      return NULL;
   }
};   

class AutNumPeeringIterator {
private:
   AutNum  *o;
   Peering *currentPeering;
public:
   AutNumPeeringIterator(const AutNum *an) : o((AutNum *) an) {
      if (o)
	 o->gatherPeerings();
      first();
   }
   const Peering *first() {
      if (!o)
	 return (Peering *) NULL;
      currentPeering = o->peerings->head();
      return currentPeering;
   }
   const Peering *next() {
      if (! (o && currentPeering))
	 return (Peering *) NULL;
      currentPeering = o->peerings->next(currentPeering);
      return currentPeering;
   }
   const AutNumPeeringIterator &operator++(int) {
      next();
      return *this;
   }
   operator Peering*() const { return currentPeering; }
   Peering* operator()(void) const { return currentPeering; }
};



#endif   // AUTNUM_H
