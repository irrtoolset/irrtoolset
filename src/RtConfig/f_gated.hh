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

#ifndef F_GATED_H
#define F_GATED_H

#include "config.h"
#include "util/List.hh"
#include "normalform/FilterOfASPath.hh"
#include "normalform/NE.hh"
#include "irr/autnum.hh"

class PolicyActionList;
class PTree;
//class NormalExpression;

class f_gated_node : public ListNode {
public:
   f_gated_node()  {}
   FilterOfASPath  as_path;
   int             contains;
};

class Result : public ListNode {
public:
   Result() {
      ne = (NormalExpression *) NULL;
      action = (PolicyActionList *) NULL;
   }
   ~Result() {
      if (ne)
	 delete ne;
      if (action)
	 delete action;
   }
    
   FilterOfASPath* as_path() {
      if (ne && ne->first())
	 return & ne->first()->as_path;
      return (FilterOfASPath *) NULL;
   }

   SetOfPrefix* prfx_set() {
      if (ne && ne->first())
	 return & ne->first()->prfx_set;
      return (SetOfPrefix *) NULL;
   }

   NormalExpression *ne;
   PolicyActionList      *action;
};

class GatedConfig : public RtConfig {
public:
   GatedConfig() : RtConfig() {
   }
   void importP(ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr);
   void exportP(ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr);
   //void deflt(ASt as, ASt peerAS);
   //void static2bgp(ASt as, IPAddr* addr);
   //void networks(ASt as);

protected:
   void printASPath(FilterOfASPath &re);
   virtual void printActions(PolicyActionList *actions);
   void printRoutes(SetOfPrefix &nets, PolicyActionList *action, int import_flag);
   void printRoutes(int include, List<Result> &rlist, int import_flag);
   void printAccessList(SetOfPrefix& nets) {
      PolicyActionList al;
      printRoutes(nets, &al, true);
   }
   void printAspathAccessList(FilterOfASPath& path) {
      printASPath(path);
   }
   void normalize(List<f_gated_node> &result, List<Result> &rlist);

   void fillPrefMap(const AutNum *an);
   void _fillPrefMap(PolicyExpr *policy);
   void _fillPrefMap(int pref);
   int  normalizePref(int pref);
protected:
   int prefMap[256];
};

#endif   // F_GATED_H
