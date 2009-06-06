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

#include "config.h"
#include <cstdio>
#include <sys/types.h>
#include "Stack.hh"
#include "util/List.hh"
#include "rpsl/prefix.hh"

#ifndef foreachchild
#define foreachchild(x) for (int x = 0; x < 2; ++x)
#endif //foreachchild

#ifndef u_int64_t
#define u_int64_t unsigned long long int
#endif

#ifndef u_int
#define u_int unsigned int
#endif

#include "util/FixedSizeAllocator.hh"

extern FixedSizeAllocator IPv6RadixTreeAllocator;
extern ip_v6word_t ipv6masks[];
extern ip_v6word_t ipv6bits[];

class IPv6RadixTree {
public:
   class Iterator {
   private:
      const IPv6RadixTree* last;
      const IPv6RadixTree* root;
      BoundedStack<const IPv6RadixTree*> dfsStack;
      Iterator(const Iterator &);

   public:
      Iterator(const IPv6RadixTree* r) : last((const IPv6RadixTree *) NULL), 
	 root(r), dfsStack(65) {}
      inline const IPv6RadixTree* first();
      inline const IPv6RadixTree* next(const IPv6RadixTree* _last);
   };

public:
   ipv6_addr_t addr;
   u_int leng;
   ipv6_addr_t rngs;
   enum Direction { LEFT = 0, RIGHT = 1, HERE, UP};

private:
   IPv6RadixTree *chld[2];
  
public:

   IPv6RadixTree(ipv6_addr_t _addr, u_int _leng, ipv6_addr_t _rngs) : 
      addr(_addr), leng(_leng), rngs(_rngs) {

      foreachchild(c)
	 chld[c] = (IPv6RadixTree *) NULL;
   }

   IPv6RadixTree(const IPv6RadixTree &b) : addr(b.addr), leng(b.leng), rngs(b.rngs) {
      foreachchild(c)
	 chld[c] = b.chld[c] ? new IPv6RadixTree(*b.chld[c]) : (IPv6RadixTree *) NULL;
    //  cout << "2 " << "ip" << addr << "leng" << leng << "rng" << rngs << endl;
   }

   ~IPv6RadixTree() {
      foreachchild(c)
	 if (chld[c])
	    delete chld[c];
   }

   IPv6RadixTree *insert(ipv6_addr_t addr, u_int leng, ipv6_addr_t rngs);
   IPv6RadixTree *remove(ipv6_addr_t addr, u_int leng, ipv6_addr_t rngs);

   bool contains(ipv6_addr_t addr, u_int leng, ipv6_addr_t rngs) const;

   bool equals(const IPv6RadixTree *b) const;

   IPv6RadixTree *and_(const IPv6RadixTree *b);      // this becomes this and      b
   IPv6RadixTree *or_(const IPv6RadixTree *b);       // this becomes this or       b
   IPv6RadixTree *setminus(const IPv6RadixTree *b); // this becomes this setminus b

   void print() const;
   void *operator new(size_t s) {
      return IPv6RadixTreeAllocator.allocate();
   }
   void operator delete(void *p, size_t s) {
      IPv6RadixTreeAllocator.deallocate(p);
   }
   IPv6RadixTree *makeMoreSpecific(int code, int n, int m);

protected:
   IPv6RadixTree();

private:
   IPv6RadixTree *removeRange(ipv6_addr_t _rngs);
   Direction direction(ipv6_addr_t addr, u_int leng, ipv6_addr_t _addr, u_int _leng) const;
   void commonAnscestor(ipv6_addr_t _addr,  u_int _leng, ipv6_addr_t addr,   u_int leng, ipv6_addr_t &paddr, u_int &pleng) const;
};

class IPv6RadixSet {

public:
   static bool compressedPrint;

   class Iterator {
   private:
      IPv6RadixTree::Iterator itr;
      const IPv6RadixTree *now;     // points to current node during iteration

   public:
      Iterator(const IPv6RadixSet *s) : itr(s->root) {}
      bool first(ipv6_addr_t &_addr, u_int &_leng, ipv6_addr_t &rngs);
      bool next(ipv6_addr_t &_addr, u_int &_leng, ipv6_addr_t &rngs);
   };

   class SortedIterator {
   private:
      class PrefixLNode : public ListNode {
      public:
      	 ipv6_addr_t addr;
      	 u_int leng;
      	 ipv6_addr_t rngs;

      public:
      	 PrefixLNode(ipv6_addr_t _addr, u_int _leng, ipv6_addr_t _rngs) : 
   	    addr(_addr), leng(_leng), rngs(_rngs) {}
      };
      List<PrefixLNode> l;
      const IPv6RadixSet *set;

   public:
      SortedIterator(const IPv6RadixSet *s) : set(s) {}
      ~SortedIterator() {
	       l.clear();
      }
      bool first(ipv6_addr_t &_addr, u_int &_leng, ipv6_addr_t &_rngs);
      bool next(ipv6_addr_t &_addr, u_int &_leng, ipv6_addr_t &_rngs);
   };

   class PrefixIterator {
   private:
      IPv6RadixTree::Iterator itr;
      const IPv6RadixTree *current;
      PrefixIterator(const PrefixIterator &);
      ipv6_addr_t addr;
      u_int leng;
      ipv6_addr_t rngs;
      u_int cleng;
      ipv6_addr_t number;

   public:
      PrefixIterator(const IPv6RadixSet *s) : itr(s->root) {}
      bool first(ipv6_addr_t &_addr, u_int &_leng);
      bool next(ipv6_addr_t &_addr, u_int &_leng);
   };

   class SortedPrefixIterator {
   private:
      class PrefixLNode : public ListNode {
      public:
	 ipv6_addr_t addr;
	 u_int leng;

	 PrefixLNode(ipv6_addr_t _addr, u_int _leng) : 
	    addr(_addr), leng(_leng) {}
      };
      List<PrefixLNode> l;
      const IPv6RadixSet *set;

   public:
      SortedPrefixIterator(const IPv6RadixSet *s) : set(s) {
      }
      ~SortedPrefixIterator() {
	 l.clear();
      }
      bool first(ipv6_addr_t &_addr, u_int &_leng);
      bool next(ipv6_addr_t &_addr, u_int &_leng);
   };

   class PrefixRangeIterator {
   private:
      IPv6RadixTree::Iterator itr;
      const IPv6RadixTree *current;
      PrefixRangeIterator(const PrefixRangeIterator &);
      ipv6_addr_t addr;
      u_int leng;
      ipv6_addr_t rngs;
      u_int cleng;

   public:
      PrefixRangeIterator(const IPv6RadixSet *s) : itr(s->root) {}
      bool first(ipv6_addr_t &_addr, u_int &_leng, u_int &_start, u_int &_end);
      bool next(ipv6_addr_t &_addr, u_int &_leng, u_int &_start, u_int &_end);
      // Added by wlee@isi.edu for roe
/*
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
*/
   };

   class SortedPrefixRangeIterator {
   private:
      class PrefixLNode : public ListNode {
      public:
	 ipv6_addr_t addr;
	 u_int leng;
	 u_int start;
	 u_int end;

	 PrefixLNode(ipv6_addr_t _addr, u_int _leng, u_int _start, u_int _end) : 
	    addr(_addr), leng(_leng), start(_start), end(_end) {}
      };
      List<PrefixLNode> l;
      const IPv6RadixSet *set;

   public:
      SortedPrefixRangeIterator(const IPv6RadixSet *s) : set(s) {}
      ~SortedPrefixRangeIterator() {
	 l.clear();
      }
      bool first(ipv6_addr_t &_addr, u_int &_leng, u_int &_start, u_int &_end);
      bool next(ipv6_addr_t &_addr, u_int &_leng, u_int &_start, u_int &_end);
   };

private:
   IPv6RadixTree *root;


public:
   IPv6RadixSet() {
     root = (IPv6RadixTree *) NULL;
   }

   IPv6RadixSet(const IPv6RadixSet &b) {
      if (b.root) {
         root = new IPv6RadixTree(*b.root);
      } else {
         root = (IPv6RadixTree *) NULL;
      }
   }

   ~IPv6RadixSet() {
      if (root)
	 delete root;
   }

   void insert(ipv6_addr_t addr, u_int leng, ipv6_addr_t rngs) {
     root = root->insert(addr, leng, rngs);
   }
   void insert(ipv6_addr_t addr, u_int leng) {
     root = root->insert(addr, leng, addr.getbits(leng) );
   }
   void remove(ipv6_addr_t addr, u_int leng, ipv6_addr_t rngs) {
     root = root->remove(addr, leng, rngs);
   }
   void remove(ipv6_addr_t addr, u_int leng) {
     root = root->remove(addr, leng, addr.getbits(leng));
   }
   bool contains(ipv6_addr_t addr, u_int leng, ipv6_addr_t rngs) const {
      return root->contains(addr, leng, rngs);
   }
   bool contains(ipv6_addr_t addr, u_int leng) const {
      return root->contains(addr, leng, addr.getbits(leng));
   }

   void makeMoreSpecific(int code, int n, int m) {
      root = root->makeMoreSpecific(code, n, m);
   }

   // Added by wlee@isi.edu, used by roe
/*
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
*/
   void clear() {
      if (root) {
	 delete root;
	 root = (IPv6RadixTree *) NULL;
      }
   }

   bool isEmpty() const {
      return !root;
   }

   void operator |= (const IPv6RadixSet& b) {
      root = root->or_(b.root);
   }
   void operator &= (const IPv6RadixSet& b) {
      root = root->and_(b.root);
   }
   void operator -= (const IPv6RadixSet& b) {
      // TBD: fix the bug in remove (see setminus) and use setminus instead (quicker!)
      //root = root->setminus(b.root); 

      if (*this == b) {
        this->clear();
        return;
      }

      if (!b.root || !this->root)
        return;

      IPv6RadixSet::PrefixIterator itr(this);
      IPv6RadixSet *result = new IPv6RadixSet;
      ipv6_addr_t addr;
      u_int leng;
      for (bool flag = itr.first(addr, leng); flag; 
                flag = itr.next(addr, leng)) {
        if (! b.contains(addr, leng)) {
          result->insert(addr, leng);
        } 
      }
      if (result->root)
        *this = *result;
      else 
        this->clear();
   }
   int  operator == (const IPv6RadixSet& b) const {
      return root->equals(b.root);
   }
   void operator =  (const IPv6RadixSet& b) {
      if (root == b.root)
	 return;
      delete root;
      root = new IPv6RadixTree(*b.root);
   }

   friend std::ostream& operator<<(std::ostream&, const IPv6RadixSet &set);
};

