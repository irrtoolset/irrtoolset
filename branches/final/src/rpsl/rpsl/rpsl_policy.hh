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

// Comments:
// any dynamic memory allocated by lexer or parser 
// is claimed by the constructors here
// They are either used internally or deleted using delete!
// if a pointer points to something in the dictionary, that is not claimed

#ifndef POLICY_HH
#define POLICY_HH

#include "config.h"
#include <cassert>
extern "C" {
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif // HAVE_MALLOC_H
}
#include "util/List.hh"
#include "symbols.hh"
#include "prefix.hh"
#include "rpsl_filter.hh"

// Added by wlee@isi.edu
#ifdef DEBUG
#define INDENT(indent) for (int iii = 0; iii < indent; iii++) os << " "
#endif // DEBUG

typedef unsigned int ASt;

class ostream;
class AttrRPAttr;
class AttrMethod;

class Policy {
public:
   virtual ~Policy() {}
   virtual ostream& print(ostream &out) const;
   virtual Policy* dup() const = 0;

#ifdef DEBUG
   virtual const char *className(void) const = 0;
   virtual void printClass(ostream &os, int indent) const {
      INDENT(indent); os << "(*** Need more work here ***)" << endl;
   }
#endif // DEBUG
};

inline ostream &operator<<(ostream &out, const Policy &p) {
   return p.print(out);
}

class PolicyPeering: public Policy {
public:
   Filter      *peerASes;
   Filter      *peerRtrs;
   Filter      *localRtrs;
   SymID        prngSet;
public:
   PolicyPeering(Filter *asExp, Filter *rtrExp, Filter *lRtrExp) 
      : peerASes(asExp), peerRtrs(rtrExp), localRtrs(lRtrExp), prngSet(NULL) {
   }
   PolicyPeering(SymID pSet) 
      : peerASes(NULL), peerRtrs(NULL), localRtrs(NULL), prngSet(pSet) {
   }
   PolicyPeering(const PolicyPeering &pt) : prngSet(pt.prngSet) {
      peerASes  = pt.peerASes ? pt.peerASes->dup() : NULL;
      peerRtrs  = pt.peerRtrs ? pt.peerRtrs->dup() : NULL;
      localRtrs = pt.localRtrs ? pt.localRtrs->dup() : NULL;
   }
   virtual ~PolicyPeering() {
      if (peerASes) 
	 delete peerASes;
      if (peerRtrs) 
	 delete peerRtrs;
      if (localRtrs) 
	 delete localRtrs;
   }
   
   virtual ostream& print(ostream &out) const;
   virtual Policy *dup() const {
      return new PolicyPeering(*this);
   }
#ifdef DEBUG
   virtual const char *className(void) const {
      return "PolicyPeering";
   }
   virtual void printClass(ostream &os, int indent) const {
      // For peerASSet
      INDENT(indent); 
   }
#endif // DEBUG
};

class PolicyAction: public Policy, public ListNode {
public:
   const AttrRPAttr     *rp_attr;   // into rp-attribute in the dictionary
   const AttrMethod     *rp_method; // into the method
   ItemList             *args;      // arguments to the method

public:
   PolicyAction(const AttrRPAttr *_rp_attr, const AttrMethod *_rp_mtd, 
		ItemList *_args): 
      rp_attr(_rp_attr), rp_method(_rp_mtd), args(_args) {}
   PolicyAction(const PolicyAction &pt) : 
      rp_attr(pt.rp_attr), rp_method(pt.rp_method) {
      args = new ItemList(*pt.args);
   }
   virtual ~PolicyAction() {
      delete args;
   }
   virtual ostream& print(ostream &out) const;
   virtual Policy *dup() const {
      return new PolicyAction(*this);
   }
#ifdef DEBUG
   virtual const char *className(void) const {
      return "PolicyAction";
   }
   virtual void printClass(ostream &os, int indent) const {
      // For rp_attr
      INDENT(indent); os << "rp_attr" << endl;
      INDENT(indent); os << "  _name = \"" << "\"" << endl;
      INDENT(indent); os << "  methods (RPMethod *)" << endl;
      INDENT(indent); os << "    _name = \"" << "\"" << endl;
      // For rp_method
      INDENT(indent); os << "rp_method" << endl;
      INDENT(indent); os << "  _name = \"" << "\"" << endl;
      // For rp_args
      INDENT(indent); os << "args (ItemList *)" << endl;
      args->printClass(os, indent + 2);
   }
#endif // DEBUG
};

class PolicyActionList: public Policy, public List<PolicyAction> {
public:
   PolicyActionList() {
   }
   virtual ~PolicyActionList() {
      List<PolicyAction>::clear();
   }
   virtual ostream& print(ostream &out) const;
   virtual Policy *dup() const {
      return new PolicyActionList(*this);
   }
#ifdef DEBUG
   virtual const char *className(void) const {
      return "PolicyActionList";
   }
   virtual void printClass(ostream &os, int indent) const {
      for (PolicyAction *node = head(); node; node = next(node)) 
	 node->printClass(os, indent);
   }
#endif // DEBUG
};

class PolicyPeeringAction: public Policy, public ListNode {
public:
   PolicyPeering     *peering;
   PolicyActionList  *action;
public:
   PolicyPeeringAction(PolicyPeering *p, PolicyActionList *a) : 
      peering(p), action(a) {}
   PolicyPeeringAction(const PolicyPeeringAction &b) {
      peering = new PolicyPeering(*b.peering);
      action  = new PolicyActionList(*b.action);
   }
   virtual ~PolicyPeeringAction() {
      delete peering;
      delete action;
   }
   virtual ostream& print(ostream &out) const;
   virtual Policy *dup() const {
      return new PolicyPeeringAction(*this);
   }
#ifdef DEBUG
   virtual const char *className(void) const {
      return "PolicyPeeringAction";
   }
   virtual void printClass(ostream &os, int indent) const {
      INDENT(indent); os << "peering (PolicyPeering *)" << endl;
      peering->printClass(os, indent + 2);
      INDENT(indent); os << "action (PolicyActionList *)" << endl;
      action->printClass(os, indent + 2);
   }
#endif // DEBUG
};

class PolicyFactor: public Policy, public ListNode {
public:
   List<PolicyPeeringAction> *peeringActionList;
   Filter        *filter;
public:
   PolicyFactor(List<PolicyPeeringAction> *pAL, Filter *_filter) : 
      peeringActionList(pAL), filter(_filter) {
   }
   PolicyFactor(const PolicyFactor &pt) {
      peeringActionList = new List<PolicyPeeringAction>(*pt.peeringActionList);
      filter  = (Filter *)pt.filter->dup();
   }
   virtual ~PolicyFactor() {
      delete peeringActionList;
      delete filter;
   }
   virtual ostream& print(ostream &out) const;
   virtual Policy *dup() const {
      return new PolicyFactor(*this);
   }
#ifdef DEBUG
   virtual const char *className(void) const {
      return "PolicyFactor";
   }
   virtual void printClass(ostream &os, int indent) const {
      // Action
      INDENT(indent); os << "peeringActionList" << endl;
      for (PolicyPeeringAction *pc = peeringActionList->head(); 
	   pc;
	   pc = peeringActionList->next(pc)) {
	 pc->printClass(os, indent + 2);
      }
      // Filter
      INDENT(indent); os << "filter (Filter *)" << endl;
      filter->printClass(os, indent + 2);
   }
#endif // DEBUG
};

class PolicyExpr: public Policy {
};

class PolicyTerm: public PolicyExpr, public List<PolicyFactor> {
public:
   // PolicyTerm();
   // PolicyTerm(const PolicyTerm &b);
   virtual ~PolicyTerm() {
      List<PolicyFactor>::clear();
   }
   virtual ostream& print(ostream &out) const;
   virtual Policy *dup() const {
      return new PolicyTerm(*this);
   }
#ifdef DEBUG
   virtual const char *className(void) const {
      return "PolicyTerm";
   }
   virtual void printClass(ostream &os, int indent) const {
      for (PolicyFactor *pf = head(); pf; pf = next(pf)) {
	 pf->printClass(os, indent);
      }
   }
#endif // DEBUG
};

class PolicyRefine: public PolicyExpr {
public:
   PolicyExpr *left;
   PolicyExpr *right; 
public:
   PolicyRefine(PolicyExpr *l, PolicyExpr *r) : left(l), right(r) {}
   PolicyRefine(const PolicyRefine &b) {
      left  = (PolicyExpr *) b.left->dup();
      right = (PolicyExpr *) b.right->dup();
   }
   virtual ~PolicyRefine() {
      delete left;
      delete right;
   }
   virtual ostream& print(ostream &out) const;
   virtual Policy *dup() const {
      return new PolicyRefine(*this);
   }
#ifdef DEBUG
   virtual const char *className(void) const {
      return "PolicyRefine";
   }
   virtual void printClass(ostream &os, int indent) const {
      INDENT(indent); os << "left" << endl;
      left->printClass(os, indent + 2);
      INDENT(indent); os << "right" << endl;
      right->printClass(os, indent + 2);
   }
#endif // DEBUG
};

class PolicyExcept: public PolicyExpr {
public:
   PolicyExpr *left;
   PolicyExpr *right; 
public:
   PolicyExcept(PolicyExpr *l, PolicyExpr *r) : left(l), right(r) {}
   PolicyExcept(const PolicyExcept &b) {
      left  = (PolicyExpr *) b.left->dup();
      right = (PolicyExpr *) b.right->dup();
   }
   virtual ~PolicyExcept() {
      delete left;
      delete right;
   }
   virtual ostream& print(ostream &out) const;
   virtual Policy *dup() const {
      return new PolicyExcept(*this);
   }
#ifdef DEBUG
   virtual const char *className(void) const {
      return "PolicyExcept";
   }
   virtual void printClass(ostream &os, int indent) const {
      INDENT(indent); os << "left" << endl;
      left->printClass(os, indent + 2);
      INDENT(indent); os << "right" << endl;
      right->printClass(os, indent + 2);
   }
#endif // DEBUG
};

#endif   // POLICY_HH
