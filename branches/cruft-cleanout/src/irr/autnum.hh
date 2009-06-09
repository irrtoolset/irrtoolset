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
#include "dataset/SetOfUInt.hh"
#include "rpsl/List.hh"
#include "rpsl/schema.hh"
#include "irr/irr.hh"
#include "rpsl/prefix.hh"
#include "rpsl/rpsl_policy.hh"
#include <vector>
#include <string>

using namespace std;

#define INVALID_AS    ~0

class Peering : public ListNode {
public:
   ASt      peerAS;
   MPPrefix peerIP;
   MPPrefix localIP;

public:
   Peering(ASt _peerAS, const MPPrefix &_peerIP, const MPPrefix &_localIP) 
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
   void gatherRouters(Filter *f, MPPrefixRanges *result);

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

// REIMPLEMENTED
static bool isPeeringMatching(Filter *f, const MPPrefix *ip) {

   if (typeid(*f) == typeid(FilterANY))
      return true;
   if (typeid(*f) == typeid(FilterRouter)) {
      return ((*((FilterRouter *) f)->ip) == *ip);
   }
   if (typeid(*f) == typeid(FilterRouterName)) {
      char *dns = ((FilterRouterName *) f)->name;
      if (!irr)
	 return false;
      const InetRtr *rtr = irr->getInetRtr(dns);
      if (!rtr)
	 return false;
      AttrIterator<AttrIfAddr> itr(rtr, "ifaddr");
      while (itr) {
        if (ip->ipv4 && itr()->ifaddr->ipv4) {
	        if (itr()->ifaddr->ipv4->get_ipaddr() == ip->ipv4->get_ipaddr())
	          return true;
        }
	    itr++;
      }
      AttrIterator<AttrIfAddr> itr1(rtr, "interface");
      while (itr1) {
         if (itr1()->ifaddr->ipv4 && ip->ipv4) {
            if (itr1()->ifaddr->ipv4->get_ipaddr() == ip->ipv4->get_ipaddr())
               return true;
         }
         if (itr1()->ifaddr->ipv6 && ip->ipv6) {
            if (itr1()->ifaddr->get_ipaddr() == ip->get_ipaddr())
               return true;
         }
         itr1++;
      }

      return false;
   }
   if (typeid(*f) == typeid(FilterRTRSNAME)) {
      SymID set = ((FilterRTRSNAME *) f)->rtrsname;
      if (!irr)
	 return false;

      const MPPrefixRanges *rtrSet = irr->expandRtrSet(set);
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
   //if (typeid(*f) == typeid(FilterANY))
   //   return true;
   assert(false);
   return false;
}

static bool isPeeringMatching(PolicyPeering *prng, SymID pset,
		const ASt peerAS, const MPPrefix *peerIP, const MPPrefix *ip) {

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
   for (AttrIterator<AttrPeering> itr(peers, "mp-peering"); itr; itr++)
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

// partly reimplemented
class AutNumDefaultIterator : public AttrIterator<AttrDefault> {
private:
   const ASt peerAS;
   const MPPrefix *peerIP;
   const MPPrefix *ip;

// Made it protected by wlee
protected:      
   virtual bool isMatching(Attr *attr) {
      return isPeeringMatching(((AttrDefault *) attr)->peering, 
			       NULL, peerAS, peerIP, ip);
   }

public:
   AutNumDefaultIterator(const AutNum *an,
			 const ASt _peerAS = INVALID_AS, 
			 const char *attrib = "default",
			 const MPPrefix *_peerIP = NULL, 
			 const MPPrefix *_ip = NULL):
     AttrIterator<AttrDefault>(an, attrib), 
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
	  std::cerr << "Internal Error!" << std::endl;
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
	this->first();	
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
   ItemList *afi_list;
   List<FilterAction> filterActionList;
   FilterAction      *current;
public:
   AutNumSelector(const AutNum *an, const char *attrib, SymID pset,
                  const ASt peerAS, const MPPrefix *peerIP, const MPPrefix *ip,
                  const char *fProtName = "BGP4", const char *iProtName = "BGP4"):
      current(NULL), afi_list(new ItemList) {
      AttrIterator<AttrType> itr(an, attrib);
      const AttrType *import;
      List<FilterAction> *list;
      const AttrProtocol *fProt=schema.searchProtocol(fProtName);
      const AttrProtocol *iProt=schema.searchProtocol(iProtName);
      ItemList *tmp_afi_list = new ItemList;

      for (import = itr.first(); import; import = itr.next()) {
         if (import->fromProt == fProt && import->intoProt == iProt) {
            list = select(import->policy, pset, peerAS, peerIP, ip, &tmp_afi_list);
            if (list) {
               filterActionList.splice(*list);
               afi_list->merge(*tmp_afi_list);
               delete list;
               delete tmp_afi_list;
               tmp_afi_list = new ItemList;
            }
         }
      }
      delete tmp_afi_list;
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

   List<FilterAction> *get_fa_list() {
      return &filterActionList;
   }

   ItemList *get_afi_list() {
     return afi_list;
   }

private:
   List<FilterAction> *select(PolicyExpr *policy, SymID pset,
                                  const ASt peerAS, const MPPrefix *peerIP, 
                                  const MPPrefix *ip, ItemList **tmp_afi_list,
                                  Filter **combinedFilter = NULL) {
      if (typeid(*policy) == typeid(PolicyTerm)) {
         List<FilterAction> *list = new List<FilterAction>;
         PolicyTerm   *pt = (PolicyTerm *) policy;
         PolicyFactor *pf;
         PolicyPeeringAction *pa;
         bool          noMatch;
         ItemList *afi;

         for (pf = pt->head(); pf; pf = pt->next(pf)) {
            noMatch = true;
            for (pa = pf->peeringActionList->head(); 
                 pa; 
                 pa = pf->peeringActionList->next(pa)) {
               if (noMatch && isPeeringMatching(pa->peering, pset,peerAS,peerIP,ip)) {
                  list->append((new FilterAction((Filter *) 
                                                 pf->filter->dup(), 
                                                 (PolicyActionList *) 
                                                 pa->action->dup())));
                  // save the afi
                  afi = ((FilterAFI *) pf->filter)->afi_list;
                  (*tmp_afi_list)->merge(*(new ItemList (*afi)));
                  // rpsl specification order rule dictates only one match here
                  // so break out of this if statement
                  noMatch = false;
                  if (!combinedFilter)
                     break;
               }
               if (combinedFilter)
                  if (*combinedFilter)
                     *combinedFilter = 
                        new FilterOR(*combinedFilter, (Filter *) pf->filter->dup());
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
         ItemList *afi_list_left = new ItemList;
         ItemList *afi_list_right = new ItemList;

         if (combinedFilter) {
            Filter  *leftFilter = NULL;
            Filter  *rghtFilter = NULL;
            left = select(pr->left, pset, peerAS, peerIP, ip, &afi_list_left, &leftFilter);
            rght = select(pr->right, pset, peerAS, peerIP, ip, &afi_list_right, &rghtFilter);
            *combinedFilter = new FilterAND(leftFilter, rghtFilter);
         } else {
            left = select(pr->left, pset, peerAS, peerIP, ip, &afi_list_left, NULL);
            rght = select(pr->right, pset, peerAS, peerIP, ip, &afi_list_right, NULL);
         }

         afi_list_left->_and(*afi_list_right);
         (*tmp_afi_list)->merge(*afi_list_left);

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
         ItemList *afi_list_left = new ItemList;
         ItemList *afi_list_right = new ItemList;

         List<FilterAction> *left = select(pe->left, pset, peerAS, peerIP, ip, &afi_list_left,
                                               &leftFilter);
         List<FilterAction> *rght = select(pe->right, pset, peerAS, peerIP, ip, &afi_list_right,
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
            ItemList *t = new ItemList (*afi_list_right);
            t->_not();
            afi_list_left->_and(*t);
            delete t;
         }
         delete rghtFilter;

         if (rght) {
            FilterAction *filterAction;
            for (filterAction = rght->head();
                 filterAction;
                 filterAction = rght->next(filterAction))
               filterAction->filter = new FilterAND(filterAction->filter,
                                   (Filter  *) leftFilter->dup());
            afi_list_right->_and(*afi_list_left);
         }
         delete leftFilter;

         if (!left) {
            (*tmp_afi_list)->merge(*afi_list_right);
            return rght;
         }

         if (!rght) {
            (*tmp_afi_list)->merge(*afi_list_left);
            return left;
         }

         rght->splice(*left);
         (*tmp_afi_list)->merge(*afi_list_left);
         (*tmp_afi_list)->merge(*afi_list_right);
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

class Interface : public ListNode {

public:
  AttrIfAddr interface;

public:
   Interface(AttrIfAddr _interface)
      : ListNode(), interface(_interface) {
   }
   Interface(const Interface &b) 
      : ListNode(), interface(b.interface) {
   }
   ~Interface() {
   }

   bool operator < (const Interface &b) const {
      return (*(interface.ifaddr) < *(b.interface.ifaddr)); 
   }

   bool operator == (const Interface &b) const {
      return (*(interface.ifaddr) == *(b.interface.ifaddr)); 
   }
};

class InterfaceIterator {
private: 
      SortedList<Interface> *ifaces;
      Interface *current;

public:
      InterfaceIterator (const InetRtr *inetrtr) {
        ifaces = new SortedList<Interface>;
        for (AttrIterator<AttrIfAddr> itr(inetrtr, "ifaddr"); itr; itr++) {
          ifaces->insertSortedNoDups(new Interface(*itr));
        }
        for (AttrIterator<AttrIfAddr> itr1(inetrtr, "interface"); itr1; itr1++) {
          ifaces->insertSortedNoDups(new Interface(*itr1));
        }
      }
      Interface *first() {
        current = ifaces->head();
        return current;
      }

      Interface *next() {
      if (current)
        current = ifaces->next(current);
        return current;
      }
};

class Peer : public ListNode {
public:
   AttrPeer peer;

public:
   Peer(AttrPeer _peer)
      : ListNode(), peer(_peer) {
   }
   Peer(const Peer &b)
      : ListNode(), peer(b.peer) {
   }
   ~Peer() {
   }

   bool operator < (const Peer &b) const {
      return (*(peer.peer) < *(b.peer.peer));
   }

   bool operator == (const Peer &b) const {
      return (*(peer.peer) == *(b.peer.peer));
   }
};

class PeerIterator {
private:
      SortedList<Peer> *peers;
      Peer *current;

public:
      PeerIterator (const InetRtr *inetrtr) {
        peers = new SortedList<Peer>;
        for (AttrIterator<AttrPeer> itr(inetrtr, "peer"); itr; itr++) {
          peers->insertSortedNoDups(new Peer(*itr));
        }
        for (AttrIterator<AttrPeer> itr1(inetrtr, "mp-peer"); itr1; itr1++) {
          peers->insertSortedNoDups(new Peer(*itr1));
        }
      }
      Peer *first() {
        current =peers->head();
        return current;
      }

      Peer *next() {
      if (current)
        current = peers->next(current);
        return current;
      }
};


class PeeringSetIterator {
private:
      SortedList<Peering> *peerings;
      Peering *current;

public:
      PeeringSetIterator(const PeeringSet *prngSet) {
        peerings = new SortedList<Peering>;
        for (AttrIterator<AttrPeering> itr(prngSet, "peering"); itr; itr++) {
          if (itr()->peering->prngSet)  {
            const PeeringSet *set = irr->getPeeringSet(itr()->peering->prngSet);
            PeeringSetIterator *itr1 = new PeeringSetIterator((PeeringSet *) set);
            peerings->spliceNoDups(*(itr1->peerings));
          }
          else if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)
             && typeid(*itr()->peering->localRtrs) == typeid(FilterRouter)
             && typeid(*itr()->peering->peerASes) == typeid(FilterASNO))
               peerings->insertSortedNoDups(new Peering(
                                            ((FilterASNO *) itr()->peering->peerASes)->asno,
                                            *(((FilterRouter *) itr()->peering->peerRtrs)->ip),
                                            *(((FilterRouter *) itr()->peering->localRtrs)->ip)
                                           ));   
          else 
             assert(0);
        }
        for (AttrIterator<AttrPeering> itr(prngSet, "mp-peering"); itr; itr++) {
          if (itr()->peering->prngSet)  {
            const PeeringSet *set = irr->getPeeringSet(itr()->peering->prngSet);
            PeeringSetIterator *itr1 = new PeeringSetIterator((PeeringSet *) set);
            peerings->spliceNoDups(*(itr1->peerings));
          }
          else if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)
             && typeid(*itr()->peering->localRtrs) == typeid(FilterRouter)
             && typeid(*itr()->peering->peerASes) == typeid(FilterASNO))
               peerings->insertSortedNoDups(new Peering(
                                            ((FilterASNO *) itr()->peering->peerASes)->asno,
                                            *(((FilterRouter *) itr()->peering->peerRtrs)->ip),
                                            *(((FilterRouter *) itr()->peering->localRtrs)->ip)
                                           ));
          else
             assert(0);
        }
      }
  Peering *first() {
      current = peerings->head();
      return current;
   }

  Peering *next() {
      if (current)
         current = peerings->next(current);
      return current;
   }
};


#endif   // AUTNUM_H
