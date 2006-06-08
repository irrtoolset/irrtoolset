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

#ifndef RADIXTREE_H
#define RADIXTREE_H

#include "config.h"
#include <cstdio>
#include <sys/types.h>
#include "util/Stack.hh"
#include "util/List.hh"
#include "rpsl/prefix.hh"

#define foreachchild(x) for (int x = 0; x < 2; ++x)

#ifndef u_int64_t
#define u_int64_t unsigned long long
#endif

extern u_int64_t bits[];
extern u_int masks[];

#include "util/FixedSizeAllocator.hh"

extern FixedSizeAllocator RadixTreeAllocator;

class RadixTree {
public:
   friend class Iterator {
   private:
      const RadixTree* last;
      const RadixTree* root;
      BoundedStack<const RadixTree*> dfsStack;
      Iterator(const Iterator &);

   public:
      Iterator(const RadixTree* r) : last((const RadixTree *) NULL), 
	 root(r), dfsStack(65) {}
      inline const RadixTree* first();
      inline const RadixTree* next(const RadixTree* _last);
   };

public:
   u_int addr;
   u_int leng;
   u_int64_t rngs;
   enum Direction { LEFT = 0, RIGHT = 1, HERE, UP};

private:
   RadixTree *chld[2];
  
public:

   RadixTree(u_int _addr, u_int _leng, u_int64_t _rngs) : 
      addr(_addr), leng(_leng), rngs(_rngs) {
      foreachchild(c)
	 chld[c] = (RadixTree *) NULL;
   }

   RadixTree(const RadixTree &b) : addr(b.addr), leng(b.leng), rngs(b.rngs) {
      foreachchild(c)
	 chld[c] = b.chld[c] ? new RadixTree(*b.chld[c]) : (RadixTree *) NULL;
    //  cout << "2 " << addr << leng << rngs;
   }

   ~RadixTree() {
      foreachchild(c)
	 if (chld[c])
	    delete chld[c];
   }

   RadixTree *insert(u_int addr, u_int leng, u_int64_t rngs);
   RadixTree *remove(u_int addr, u_int leng, u_int64_t rngs);

   bool contains(u_int addr, u_int leng, u_int64_t rngs) const;

   bool equals(const RadixTree *b) const;

   RadixTree *and_(const RadixTree *b);      // this becomes this and      b
   RadixTree *or_(const RadixTree *b);       // this becomes this or       b
   RadixTree *setminus(const RadixTree *b); // this becomes this setminus b

   void *operator new(size_t s) {
      return RadixTreeAllocator.allocate();
   }
   void operator delete(void *p, size_t s) {
      RadixTreeAllocator.deallocate(p);
   }
   RadixTree *makeMoreSpecific(int code, int n, int m);

public:
   void RadixTree::print() const;

protected:
   RadixTree();

private:
   RadixTree *removeRange(u_int64_t _rngs);
   Direction direction(u_int addr, u_int leng, u_int _addr, u_int _leng) const;
   void commonAnscestor(u_int _addr,  u_int _leng, u_int addr,   u_int leng,
			u_int &paddr, u_int &pleng) const;
};

class RadixSet {
public:
   static bool compressedPrint;

   friend class Iterator {
   private:
      RadixTree::Iterator itr;
      const RadixTree *now;     // points to current node during iteration

   public:
      Iterator(const RadixSet *s) : itr(s->root) {}
      bool first(u_int &_addr, u_int &_leng, u_int64_t &rngs);
      bool next(u_int &_addr, u_int &_leng, u_int64_t &rngs);
   };

   friend class SortedIterator {
   private:
      class PrefixLNode : public ListNode {
      public:
	 u_int addr;
	 u_int leng;
	 u_int64_t rngs;

	 PrefixLNode(u_int _addr, u_int _leng, u_int64_t _rngs) : 
	    addr(_addr), leng(_leng), rngs(_rngs) {}
      };
      List<PrefixLNode> l;
      const RadixSet *set;

   public:
      SortedIterator(const RadixSet *s) : set(s) {}
      ~SortedIterator() {
	 l.clear();
      }
      bool first(u_int &_addr, u_int &_leng, u_int64_t &_rngs);
      bool next(u_int &_addr, u_int &_leng, u_int64_t &_rngs);
   };

   friend class PrefixIterator {
   private:
      RadixTree::Iterator itr;
      const RadixTree *current;
      PrefixIterator(const PrefixIterator &);
      u_int addr;
      u_int leng;
      u_int64_t rngs;
      u_int cleng;
      u_int number;

   public:
      PrefixIterator(const RadixSet *s) : itr(s->root) {}
      bool first(u_int &_addr, u_int &_leng);
      bool next(u_int &_addr, u_int &_leng);
   };

   friend class SortedPrefixIterator {
   private:
      class PrefixLNode : public ListNode {
      public:
	 u_int addr;
	 u_int leng;

	 PrefixLNode(u_int _addr, u_int _leng) : 
	    addr(_addr), leng(_leng) {}
      };
      List<PrefixLNode> l;
      const RadixSet *set;

   public:
      SortedPrefixIterator(const RadixSet *s) : set(s) {}
      ~SortedPrefixIterator() {
	 l.clear();
      }
      bool first(u_int &_addr, u_int &_leng);
      bool next(u_int &_addr, u_int &_leng);
   };

   friend class PrefixRangeIterator {
   private:
      RadixTree::Iterator itr;
      const RadixTree *current;
      PrefixRangeIterator(const PrefixRangeIterator &);
      u_int addr;
      u_int leng;
      u_int64_t rngs;
      u_int cleng;

   public:
      PrefixRangeIterator(const RadixSet *s) : itr(s->root) {}
      bool first(u_int &_addr, u_int &_leng, u_int &_start, u_int &_end);
      bool next(u_int &_addr, u_int &_leng, u_int &_start, u_int &_end);
      // Added by wlee@isi.edu for roe
      bool first(PrefixRange &pr) {
	u_int _addr, _leng, _start, _end;
	bool b = first(_addr, _leng, _start, _end);
	pr = PrefixRange(_addr, _leng, 
			 (unsigned char)_start, (unsigned char)_end);
	return b;
      }
      bool next(PrefixRange &pr) {
	u_int _addr, _leng, _start, _end;
	bool b = next(_addr, _leng, _start, _end);
	pr = PrefixRange(_addr, _leng, 
			 (unsigned char)_start, (unsigned char)_end);
	return b;
      }
   };

   friend class SortedPrefixRangeIterator {
   private:
      class PrefixLNode : public ListNode {
      public:
	 u_int addr;
	 u_int leng;
	 u_int start;
	 u_int end;

	 PrefixLNode(u_int _addr, u_int _leng, u_int _start, u_int _end) : 
	    addr(_addr), leng(_leng), start(_start), end(_end) {}
      };
      List<PrefixLNode> l;
      const RadixSet *set;

   public:
      SortedPrefixRangeIterator(const RadixSet *s) : set(s) {}
      ~SortedPrefixRangeIterator() {
	 l.clear();
      }
      bool first(u_int &_addr, u_int &_leng, u_int &_start, u_int &_end);
      bool next(u_int &_addr, u_int &_leng, u_int &_start, u_int &_end);
   };

private:
   RadixTree *root;


public:
   RadixSet() {
     root = (RadixTree *) NULL;
   }

   RadixSet(const RadixSet &b) {
      if (b.root)
	 root = new RadixTree(*b.root);
      else
	 root = (RadixTree *) NULL;
   }

   ~RadixSet() {
      if (root)
	 delete root;
   }

   void insert(u_int addr, u_int leng, u_int64_t rngs) {
     root = root->insert(addr, leng, rngs);
   }
   void insert(u_int addr, u_int leng) {
     root = root->insert(addr, leng, bits[leng]);
   }
   void remove(u_int addr, u_int leng, u_int64_t rngs) {
     root = root->remove(addr, leng, rngs);
   }
   void remove(u_int addr, u_int leng) {
     root = root->remove(addr, leng, bits[leng]);
   }
   bool contains(u_int addr, u_int leng, u_int64_t rngs) const {
      return root->contains(addr, leng, rngs);
   }

   void makeMoreSpecific(int code, int n, int m) {
      root = root->makeMoreSpecific(code, n, m);
   }

   // Added by wlee@isi.edu, used by roe
   void insert(const PrefixRange &pr) {
     u_int n = pr.get_n();
     u_int m = pr.get_m();
     u_int64_t range = ~(~(u_int64_t)0 << (33 - n)) & 
                        (~(u_int64_t)0 << (32 - m));
     insert(pr.get_ipaddr(), pr.get_length(), range);
   }
   bool contains(PrefixRange &pr) const {
     u_int n = pr.get_n();
     u_int m = pr.get_m();
     u_int64_t range = ~(~(u_int64_t)0 << (33 - n)) & 
                        (~(u_int64_t)0 << (32 - m));
     return contains(pr.get_ipaddr(), pr.get_length(), range);
   }

   void clear() {
      if (root) {
	 delete root;
	 root = (RadixTree *) NULL;
      }
   }

   bool isEmpty() const {
      return !root;
   }

   void operator |= (const RadixSet& b) {
      root = root->or_(b.root);
   }
   void operator &= (const RadixSet& b) {
      root = root->and_(b.root);
   }
   void operator -= (const RadixSet& b) {
      root = root->setminus(b.root);
   }
   int  operator == (const RadixSet& b) const {
      return root->equals(b.root);
   }
   void operator =  (const RadixSet& b) {
      if (root == b.root)
	 return;
      delete root;
      root = new RadixTree(*b.root);
   }

   friend std::ostream& operator<<(std::ostream&, const RadixSet &set);
};

#endif   // RADIXTREE_H
