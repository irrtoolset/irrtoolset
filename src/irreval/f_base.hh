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
//  Author(s): Hagen Boehm <hboehm@brutus.nic.dtag.de>

#ifndef BASECONFIG_H
#define BASECONFIG_H

#include "config.h"
#include "rpsl/List.hh"
#include "normalform/FilterOfASPath.hh"
#include "irr/irr.hh"
#include <sstream>

using namespace std;

class RangeList;
class regexp;
class SetOfPrefix;
class SetOfIPv6Prefix;
class regexp_nf;

static const int COMP_UNDEF  = -1;
static const int COMP_NONE   = 0;
static const int COMP_NORMAL = 1;
static const int COMP_SUPER  = 2;

typedef struct {
   const char *name;
   int  type;
} compression_format;

static compression_format compTypes[] = {
   { "none", COMP_NONE },
   { "normal", COMP_NORMAL },
   { "super", COMP_SUPER },
   { "", COMP_UNDEF }
};

class LNodeOf2Ints : public ListNode {
public:
   int start;
   int end;
   int flag;    // flag is used for exact community matching

   LNodeOf2Ints() {}
   LNodeOf2Ints(int s, int e, int f) : start(s), end(e), flag(f) {}
   LNodeOf2Ints(const LNodeOf2Ints& b) : 
      start(b.start), end(b.end), flag(b.flag) {}
};

class LOf2Ints : public List<LNodeOf2Ints> {
public:
   LOf2Ints(int flag) : List<LNodeOf2Ints>() {
      // create a list whose end is less than its start
      // so it wont print anything!
      add(2,1);
   }
   LOf2Ints() : List<LNodeOf2Ints>() {}
   void add(int s, int e, int flag = 0) {
      append((new LNodeOf2Ints(s, e, flag)));
   }
};

template <class T>
class AccessLManager {
private :
   class Node : public ListNode {
   public:
      Node()              : data(),       lof2i()        {}
      Node(const Node &b) : data(b.data), lof2i(b.lof2i) {}
      Node(T& _data)      : data(_data),  lof2i()        {}

   public:
      T data;
      LOf2Ints lof2i;
   };

   List<Node> cache;
   int next_id;

public:
   AccessLManager() : cache(), next_id(1) {
   }

   AccessLManager(int firstID) : cache(), next_id(firstID) {
   }

   LOf2Ints* search(T& _data) {
      for (Node *p = cache.head(); p; p = cache.next(p))
	 if (p->data == _data)
	    return &(p->lof2i);
      return NULL;
   }

   LOf2Ints* add(T& _data) {
      Node *p = new Node(_data);
      cache.append(p);
      return &(p->lof2i);
   }

   void setNextID(int id) {
      next_id = id;
   }

   int newID() {
      return next_id++;
   }
   int lastID() {
      return next_id - 1;
   }
};

class BaseConfig {
public:
   BaseConfig() {
      reSplittable = false;
      prefixListName = "\0";
      asPathName = "\0";
      asPathNumber = 0;
      compression = COMP_NORMAL;
   }

public:
   // options
   static bool useAclCaches;
   static bool forceTilda;
   static bool singleRouteFilter;

public:
   int compression;
   const char *prefixListName;
   const char *asPathName;
   int asPathNumber;

protected:
   bool reSplittable;

   void calculateNegation(SetOfPrefix& nets);
   void calculateNegation(SetOfIPv6Prefix& nets);
   void printASNDottedNotation(ostream &out, ASt asn);

public:
   virtual void printAccessList(SetOfPrefix& nets) {
      cerr << "Error: accessList not implemented" << endl;
   }
   virtual void printAccessList(SetOfIPv6Prefix& nets) {
      cerr << "Error: accessList not implemented" << endl;
   }
   virtual void printAspathAccessList(FilterOfASPath& path) {
      cerr << "Error: aspathAccessList not implemented" << endl;
   }
   virtual LOf2Ints *printRoutes(SetOfPrefix& nets) {
      cerr << "Error: printRoutes not implemented" << endl;
   }
   virtual LOf2Ints *printRoutes(SetOfIPv6Prefix& nets) {
      cerr << "Error: printRoutes not implemented" << endl;
   }
   virtual LOf2Ints *printASPaths(regexp_nf& path) {
      cerr << "Error: printASPaths not implemented" << endl;
   }
   virtual void printREASno(ostream& out, const RangeList &no) {
      cerr << "Error: printREASno not implemented" << endl;
   }
   virtual int printRE_(ostream& os, const regexp& r) {
      cerr << "Error: printRE_ not implemented" << endl;
   }
   virtual void printRE(ostream& os, const regexp& r, int aclID, bool permit) {
      cerr << "Error: printRE not implemented" << endl;
   }

   void setCompression (int complvl);
};

#endif   // BASECONFIG_H
