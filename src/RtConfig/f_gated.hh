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
