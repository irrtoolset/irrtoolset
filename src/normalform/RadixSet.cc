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

#include <cassert>
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "RadixSet.hh"

using namespace std;

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

u_int masks[] ={ 0x00000000,
		 0x80000000, 0xC0000000, 0xE0000000, 0xF0000000,
		 0xF8000000, 0xFC000000, 0xFE000000, 0xFF000000,
		 0xFF800000, 0xFFC00000, 0xFFE00000, 0xFFF00000,
		 0xFFF80000, 0xFFFC0000, 0xFFFE0000, 0xFFFF0000,
		 0xFFFF8000, 0xFFFFC000, 0xFFFFE000, 0xFFFFF000, 
		 0xFFFFF800, 0xFFFFFC00, 0xFFFFFE00, 0xFFFFFF00,
		 0xFFFFFF80, 0xFFFFFFC0, 0xFFFFFFE0, 0xFFFFFFF0,
		 0xFFFFFFF8, 0xFFFFFFFC, 0xFFFFFFFE, 0xFFFFFFFF 
};

u_int64_t bits[] = { 0x100000000ULL,
		     0x080000000, 0x040000000, 0x020000000, 0x010000000,
		     0x008000000, 0x004000000, 0x002000000, 0x001000000,
		     0x000800000, 0x000400000, 0x000200000, 0x000100000,
		     0x000080000, 0x000040000, 0x000020000, 0x000010000,
		     0x000008000, 0x000004000, 0x000002000, 0x000001000,
		     0x000000800, 0x000000400, 0x000000200, 0x000000100,
		     0x000000080, 0x000000040, 0x000000020, 0x000000010,
		     0x000000008, 0x000000004, 0x000000002, 0x000000001
};

FixedSizeAllocator RadixTreeAllocator(sizeof(RadixTree), 1000);
bool RadixSet::compressedPrint = false;

inline const RadixTree* RadixTree::Iterator::first() {
   dfsStack.clear();
   dfsStack.push((RadixTree *) NULL);

   last = root;

   if (!last)
      return (const RadixTree *) NULL;

   foreachchild(c)
      if (last->chld[c])
	 dfsStack.push(last->chld[c]);

   return last;
}

inline const RadixTree* RadixTree::Iterator::next(const RadixTree* _last) {
   assert(last && last == _last);

   last = dfsStack.pop();

   if (!last)
      return (const RadixTree *) NULL;

   foreachchild(c)
      if (last->chld[c])
	 dfsStack.push(last->chld[c]);

   return last;
}

bool RadixSet::Iterator::first(u_int &addr, u_int &leng, u_int64_t &rngs) {
   now = itr.first();
   if (!now)
      return false;

   addr = now->addr;
   leng = now->leng;
   rngs = now->rngs;
   return true;
}

bool RadixSet::Iterator::next(u_int &addr, u_int &leng, u_int64_t &rngs) {
   now = itr.next(now);
   if (!now)
      return false;

   addr = now->addr;
   leng = now->leng;
   rngs = now->rngs;
   return true;
}

bool RadixSet::SortedIterator::first(u_int &_addr, u_int &_leng, u_int64_t &_rngs) {
   RadixSet::Iterator itr(set);
   PrefixLNode *p;
   u_int addr, leng;
   u_int64_t rngs;

   l.clear();

   for (bool ok = itr.first(addr, leng, rngs);
	ok;
	ok = itr.next(addr, leng, rngs)) {
      if (!rngs)
	 continue;
      // this is insertion sorting
      // it can be as bad as O(n^2)
      // but since radix tree is almost sorted, it will be around (O(n))
      for (p = l.head(); 
	   p && (p->addr < addr || (p->addr == addr && p->leng < leng)); 
	   p = l.next(p))
	 ;
      if (p)
	 l.insertBefore(p, (new PrefixLNode(addr, leng, rngs)));
      else
	 l.append((new PrefixLNode(addr, leng, rngs)));
   }

   return next(_addr, _leng, _rngs);
}

bool RadixSet::SortedIterator::next(u_int &_addr, u_int &_leng, u_int64_t &_rngs) {
   PrefixLNode *p = l.head();
   
   if (!p)
      return false;
   
   _addr = p->addr;
   _leng  = p->leng;
   _rngs = p->rngs;
   l.remove(p);
   delete p;

   return true;
}

bool RadixSet::PrefixIterator::first(u_int &_addr, u_int &_leng) {
   for (current = itr.first(); 
	current && !current->rngs; 
	current = itr.next(current)) ;

   if (!current)
      return false;

   addr = current->addr;
   leng = current->leng;
   rngs = current->rngs;

   for (cleng = leng; cleng <= 32 && ! (bits[cleng] & rngs); cleng++);
   number = 0;
	 
   _addr = addr;
   _leng  = cleng;
   number++;

   return true;
}

bool RadixSet::PrefixIterator::next(u_int &_addr, u_int &_leng) {
   if (number == (1 << (cleng - leng))) {
      number = 0;
      for (cleng++; cleng <= 32 && ! (bits[cleng] & rngs); cleng++) ;
      if (cleng > 32) {
	 for (current = itr.next(current); 
	      current && !current->rngs; 
	      current = itr.next(current)) ;

	 if (!current)
	    return false;

	 addr = current->addr;
	 leng = current->leng;
	 rngs = current->rngs;
	 for (cleng = leng; 
	      cleng <= 32 && ! (bits[cleng] & rngs); 
	      cleng++) ;
      }
   }

   _addr = addr | (number << (32 - cleng));
   _leng  = cleng;
   number++;

   return true;
}

bool RadixSet::SortedPrefixIterator::first(u_int &_addr, u_int &_leng) {
   RadixSet::PrefixIterator itr(set);
   PrefixLNode *p;
   u_int addr, leng;

   l.clear();

   for (bool ok = itr.first(addr, leng);
	ok;
	ok = itr.next(addr, leng)) {
      // this is insertion sorting
      // it can be as bad as O(n^2)
      // but since radix tree is almost sorted, it will be around (O(n))
      for (p = l.head(); 
	   p && (p->addr < addr || (p->addr == addr && p->leng < leng)); 
	   p = l.next(p))
	 ;
      if (p)
	 l.insertBefore(p, (new PrefixLNode(addr, leng)));
      else
	 l.append((new PrefixLNode(addr, leng)));
   }

   return next(_addr, _leng);
}

bool RadixSet::SortedPrefixIterator::next(u_int &_addr, u_int &_leng) {
   PrefixLNode *p = l.head();

   if (!p)
      return false;

   _addr = p->addr;
   _leng  = p->leng;
   l.remove(p);
   delete p;

   return true;
}

bool RadixSet::SortedPrefixRangeIterator::first(u_int &_addr, u_int &_leng, 
						u_int &_start, u_int &_end) {
   RadixSet::PrefixRangeIterator itr(set);
   PrefixLNode *p;
   u_int addr, leng, start, end;

   l.clear();

   for (bool ok = itr.first(addr, leng, start, end);
	ok;
	ok = itr.next(addr, leng, start, end)) {
      // this is insertion sorting
      // it can be as bad as O(n^2)
      // but since radix tree is almost sorted, it will be around (O(n))
      for (p = l.head(); 
	   p && (p->addr < addr 
		 || (p->addr == addr && p->leng < leng)
		 || (p->addr == addr && p->leng == leng && p->start < start)
		 || (p->addr == addr && p->leng == leng && p->start == start
		     && p->end < end)
		 ); 
	   p = l.next(p))
	 ;
      if (p)
	 l.insertBefore(p, new PrefixLNode(addr, leng, start, end));
      else
	 l.append(new PrefixLNode(addr, leng, start, end));
   }

   return next(_addr, _leng, _start, _end);
}

bool RadixSet::SortedPrefixRangeIterator::next(u_int &_addr, u_int &_leng, 
					       u_int &_start, u_int &_end) {
   PrefixLNode *p = l.head();

   if (!p)
      return false;

   _addr = p->addr;
   _leng  = p->leng;
   _start = p->start;
   _end = p->end;
   l.remove(p);
   delete p;

   return true;
}

bool RadixSet::PrefixRangeIterator::first(u_int &_addr, u_int &_leng, 
					  u_int &_start, u_int &_end) {

   for (current = itr.first(); 
	current && !current->rngs; 
	current = itr.next(current)) ;

   if (!current)
      return false;

   addr = current->addr;
   leng = current->leng;
   rngs = current->rngs;
	 
   for (cleng = leng; cleng <= 32 && ! (bits[cleng] & rngs); cleng++) ;
   _addr  = addr;
   _leng  = leng;
   _start = cleng;
   for (; cleng <= 32 && (bits[cleng] & rngs); cleng++) ;
   _end   = cleng - 1;

   return true;
}

bool RadixSet::PrefixRangeIterator::next(u_int &_addr, u_int &_leng, 
					 u_int &_start, u_int &_end) {

   for (; cleng <= 32 && ! (bits[cleng] & rngs); cleng++) ;

   if (cleng > 32) {
      for (current = itr.next(current); 
	   current && !current->rngs; 
	   current = itr.next(current)) ;

      if (!current)
	 return false;

      addr = current->addr;
      leng = current->leng;
      rngs = current->rngs;

      for (cleng = leng; cleng <= 32 && ! (bits[cleng] & rngs); cleng++) ;
   }

   _addr  = addr;
   _leng  = leng;
   _start = cleng;
   for (; cleng <= 32 && (bits[cleng] & rngs); cleng++) ;
   _end   = cleng - 1;

   return true;
}

static BoundedStack<RadixTree*> pStack(65);
static BoundedStack<RadixTree::Direction> dStack(65);

RadixTree::Direction RadixTree::direction(u_int addr, u_int leng, 
					  u_int _addr, u_int _leng) const {
   // assumes addr is already anded with mask

   if (leng == _leng && addr == _addr)
      return HERE;

   if (leng >= _leng) // a parent or a sibling or aunt :-)
      return UP;

   // now his mask is less than mine, either a child or UP
   if ((_addr & masks[leng]) == addr)  // my child
      return (_addr & bits[leng+1]) ? RIGHT : LEFT; 

   return UP;
}

void RadixTree::commonAnscestor(u_int _addr,  u_int _leng, 
				u_int addr,   u_int leng,
				u_int &paddr, u_int &pleng) const {

   pleng = MIN(leng, _leng);
   while (pleng && (addr & masks[pleng]) != (_addr & masks[pleng]))
      pleng--;

   paddr = addr & masks[pleng];
}

// inserts arguments to the tree
RadixTree *RadixTree::insert(u_int _addr, u_int _leng, u_int64_t _rngs) {
   if (! _rngs) // nothing to insert
      return this;

   if (!this) 
      return new RadixTree(_addr, _leng, _rngs);

   int pStackPos = pStack.getPosition();

   RadixTree *root = this;
   Direction dir, pdir;
   RadixTree *parent = (RadixTree *) NULL;
   RadixTree *now = this;
   pStack.push(parent);

   RadixTree *cparent = (RadixTree *) NULL;
   u_int caddr, cleng;

   while (_rngs) {
      dir = direction(now->addr, now->leng, _addr, _leng);
      switch (dir) {
      case HERE:
	 now->rngs |= _rngs;
	 goto unwind;
	 break;

      case LEFT:
      case RIGHT:
	 _rngs = _rngs & ~ now->rngs;
	 if (! _rngs)
	    goto cleanup; // need to restore stack here

	 pdir = dir;
	 parent = now;
	 pStack.push(parent);

	 if (! now->chld[dir])
	    now->chld[dir] = new RadixTree(_addr, _leng, 0);
	 now = now->chld[dir];

	 continue;
	 break;

      case UP:
	 commonAnscestor(_addr, _leng, 
			 now->addr, now->leng,
			 caddr, cleng);

	 cparent = new RadixTree(caddr, cleng, 0);
	 if (parent)
	    parent->chld[pdir] = cparent;
	 else
	    root = cparent;

	 dir = direction(caddr, cleng, now->addr, now->leng);
	 assert(dir == LEFT || dir == RIGHT);
	 cparent->chld[dir] = now;

	 now = cparent;
	 continue;
	 break;

      default:
	 ;
      }
   }
   
   unwind: ;
   
   // delete subtree of now that is included in already in _rngs
   foreachchild(c)
      now->chld[c] = now->chld[c]->removeRange(_rngs);

   // optimize up!
   // the prefix we inserted may be the sibling of a prefix in the tree,
   // in which case both can be deleted and included in the parents range
   // well not quite deleted...
   u_int64_t common_rngs;
   RadixTree *tmp;
   for (now = pStack.pop(); now; now = pStack.pop()) {
      //root->print();
      if (now->chld[0] && now->chld[1] 
	  && now->chld[0]->leng == now->leng+1
	  && now->chld[1]->leng == now->leng+1) {
	 // found sibling not a cousin
	 common_rngs = now->chld[0]->rngs & now->chld[1]->rngs;
	 if (common_rngs) {
	    now->rngs |= common_rngs;
	    foreachchild(c) {
	       now->chld[c]->rngs &= ~common_rngs;
	       if (! now->chld[c]->rngs)
		  foreachchild(gc)
		     if (! now->chld[c]->chld[gc]) {
			tmp = now->chld[c];
			now->chld[c] = tmp->chld[1-gc];
			tmp->chld[1-gc] = (RadixTree *) NULL;
			delete tmp;
			break;
		     }
	    }
	 } else
	    break;
      } else
	 break;
   }

 cleanup:
   pStack.setPosition(pStackPos);
   return root;
}

bool RadixTree::contains(u_int _addr, u_int _leng, u_int64_t _rngs) const {
   if (! _rngs) 
      return true;

   if (!this)
      return false;

   const RadixTree *now;
   Direction dir;

   for (now = this; now; ) {
      dir = direction(now->addr, now->leng, _addr, _leng);
      switch (dir) {
      case UP:
	 return false;
	 break;

      case HERE:
	 _rngs &= ~now->rngs;
	 return ! _rngs;
	 break;

      case LEFT:
      case RIGHT:
	 _rngs &= ~now->rngs;
	 if (! _rngs)
	    return true;

	 now = now->chld[dir];
	 break;
      default:
	 ;
      }
   }

   return false;
}


RadixTree *RadixTree::remove(u_int _addr, u_int _leng, u_int64_t _rngs) {
   if (!this || !_rngs) // nothing to delete from or nothing to delete
      return this;

   int pStackPos = pStack.getPosition();
   int dStackPos = dStack.getPosition();

   pStack.push((RadixTree *) NULL);
   dStack.push(HERE);

   u_int64_t extraRngs = 0;

   RadixTree *now, *parent, *tmp;
   Direction dir;
   bool found = false;

   for (now = this; now && !found; ) {
      dir = direction(now->addr, now->leng, _addr, _leng);
      switch (dir) {
      case UP:
      case HERE:
	 found = true; 
	 break;

      case LEFT:
      case RIGHT:
	 if (now->rngs & _rngs) {
	    // delete _rngs from now and add it to the other child
	    extraRngs = (now->rngs & _rngs);
	    now->rngs &= ~_rngs;
	    foreachchild(c)
	       now->chld[c] = now->chld[c]->
	          insert(now->addr | (c ? bits[now->leng+1] : 0), 
			 now->leng+1, extraRngs);
	 }

	 // descent
	 pStack.push(now);
	 dStack.push(dir);
	 now = now->chld[dir];
	 continue;
      default:
	 ;
      }
   }
   
   if (now) {
      Direction rdir = direction(_addr, _leng, now->addr, now->leng);
      if (rdir != UP)
	 now = now->removeRange(_rngs);

      // there may be nodes in the tree which have empty rngs and no or
      // only one child

      for (parent = pStack.pop(), dir = dStack.pop();
	   parent; 				    
	   parent = pStack.pop(), dir = dStack.pop()) {
	 parent->chld[dir] = now;
	 now = parent;
	 if (!now->rngs)
	    foreachchild(c)
	       if (! now->chld[c]) {
		  tmp = now->chld[1-c];
		  now->chld[1-c] = (RadixTree *) NULL;
		  delete now;
		  now = tmp;
		  break;
	       }  
      }
   } else
      now = this;
   
   pStack.setPosition(pStackPos);
   dStack.setPosition(dStackPos);

   if (now && !now->rngs && !now->chld[0] && !now->chld[1]) {
      delete now;
      return (RadixTree *) NULL;
   }

   return now;
}

RadixTree *RadixTree::removeRange(u_int64_t _rngs) {
   if (!this || ! _rngs) // nothing to delete from or nothing to delete
      return this;

   RadixTree *tmp;

   // delete anything with rngs in _rngs
   rngs &= ~_rngs;
   foreachchild(c)
      chld[c] = chld[c]->removeRange(_rngs);
   if (!rngs)
      foreachchild(c)
	 if (! chld[c]) {
	    tmp = chld[1-c];
	    chld[1-c] = (RadixTree *) NULL;
	    delete this;
	    return tmp;
	 }
   return this;
}

RadixTree *RadixTree::and_(const RadixTree *b) {
   if (this == b)
      return this;

   if (!this)
      return this;

   if (!b) {
      delete this;
      return (RadixTree *) NULL;
   }

   static BoundedStack<RadixTree*>       pStack(65);
   static BoundedStack<const RadixTree*> bStack(65);
   static BoundedStack<u_int>            rStack(65);
   RadixTree *result = (RadixTree *) NULL;
   RadixTree *now;
   const RadixTree *othr, *p;
   bool found;
   u_int64_t incRngs, leftRngs;
   Direction dir, rdir;

   pStack.push((RadixTree *) NULL);
   pStack.push(this);
   bStack.push((RadixTree *) NULL);
   bStack.push(b);
   rStack.push(0);
   rStack.push(0);

   for (now = pStack.pop(), othr = bStack.pop(), incRngs = rStack.pop(); 
	now; 				                               
	now = pStack.pop(), othr = bStack.pop(), incRngs = rStack.pop()) {

      for (p = othr, found = false; p && !found; ) {
	 dir = direction(p->addr, p->leng, now->addr, now->leng);
	 switch (dir) {
	 case HERE:
	    incRngs |= p->rngs;
	 case UP:
	    found = true; 
	    break;

	 case LEFT:
	 case RIGHT:
	    incRngs |= p->rngs;
	    p = p->chld[dir];
	    break;
	 default:
	    ;
	 }
      }

      leftRngs = now->rngs;
      now->rngs &= incRngs;
      leftRngs &= ~incRngs;

      if (now->rngs)
	 result = result->insert(now->addr, now->leng, now->rngs);

      if (p && leftRngs) { 
	 rdir = direction(now->addr, now->leng, p->addr, p->leng);
	 if (rdir == LEFT || rdir == RIGHT || rdir == HERE) {
	    // insert any child of p in leftRngs under now
	    Iterator itr(p);
	    for (const RadixTree* r = itr.first(); r; r = itr.next(r))
	       if (r->rngs & leftRngs)
		  result = result->insert(r->addr, r->leng, r->rngs & leftRngs);
	 }
      }

      foreachchild(c)
	 if (now->chld[c]) {
	    pStack.push(now->chld[c]);
	    rStack.push(incRngs);
	    if (!p || dir == UP)
	       bStack.push(p);
	    else
	       bStack.push(p->chld[c]);
	 }
   }

   delete this;
   return result;
}

RadixTree *RadixTree::setminus(const RadixTree *b) {
   if (this == b) {
      delete this;
      return (RadixTree *) NULL;
   }

   if (!b || !this)
      return this;

   Iterator itr(b);
   RadixTree *result = this;

   for (const RadixTree *now = itr.first(); now; now = itr.next(now))
      if (now->rngs)
         result = result->remove(now->addr, now->leng, now->rngs);

   return result;
}

RadixTree *RadixTree::or_(const RadixTree *b) {
   if (this == b)
      return this;

   if (!b)
      return this;

   Iterator itr(b);
   RadixTree *result = this;

   for (const RadixTree *now = itr.first(); now; now = itr.next(now))
      if (now->rngs)
         result = result->insert(now->addr, now->leng, now->rngs);

   return result;

//    RadixTree *result = this;
//
//    aStack.push(NULL);
//    aStack.push(this);
//    bStack.push(NULL);
//    bStack.push(b);
//    rStack.push(0);
//    rStack.push(0);
// pStack.push(&result);
//
//    for (now = bStack.pop(), othr = aStack.pop(), 
//         prnt = pStack.pop(), incRngs = rStack.pop(); 
// 	now; 				                               
// 	now = bStack.pop(), othr = aStack.pop(), 
//         prnt = pStack.pop(), incRngs = rStack.pop()) {
//      
//       for (p = othr; p && !found; ) {
// 	 dir = direction(p->addr, p->leng, now->addr, now->leng);
// 	 switch (dir) {
// 	 case HERE:
// 	    incRngs |= p->rngs;
// 	 case UP:
// 	    found = true; 
// 	    break;
//
// 	 case LEFT:
// 	 case RIGHT:
// 	    incRngs |= p->rngs;
// 	    p = p->chld[dir];
// 	    continue;
// 	 default:
// 	    ;
// 	 }
//       }
//
//       now->rngs &= ~incRngs;
//
//       if (now->rngs)
//          *prnt = p->insert(now->addr, nor->leng, now->rngs);
//
//       foreachchild(c)
// 	 if (now->chld[c]) {
// 	    bStach.push(now->chld[c]);
// pStack.push(&(now->chld[c]));
// 	    rStack.push(incRngs);
// 	    if (!p || dir == UP)
// 	       aStack.push(p);
// 	    else
// 	       aStach.push(p->chld[c]);
// 	 }
//       }
//
//
//return result;
}

bool RadixTree::equals(const RadixTree *b) const {
   if (this == b)
      return true;

   Iterator itr1(this);
   Iterator itr2(b);
   const RadixTree *n1;
   const RadixTree *n2;

   for (n1 = itr1.first(),  n2 = itr2.first();
	n1 && n2;
	n1 = itr1.next(n1), n2 = itr2.next(n2))
      if (! (n1->addr == n2->addr 
	     && n1->leng == n2->leng 
	     && n1->rngs == n2->rngs))
	 return false;

   return n1 == n2;
}

ostream& operator<<(ostream& o, const RadixSet &set) {
   char buffer[64];
   bool need_comma = false;
   u_int addr, leng, n, m;

   o << "{";
   if (set.root) {
      if (RadixSet::compressedPrint) {
      	 RadixSet::PrefixRangeIterator itr(&set);
      	 for (bool flag = itr.first(addr, leng, n, m); flag; flag = itr.next(addr, leng, n, m)) {
      	    if (need_comma)
       	       o << ", ";
      	    else
      	       need_comma = true;
      	    o << int2quad(buffer, addr) << "/" << leng << "^" << n << "-" << m;
      	 } // end of for loop
      } else { // not a compressed print
       	 RadixSet::PrefixIterator itr(&set);
	       for (bool flag = itr.first(addr, leng); flag; flag = itr.next(addr, leng)) {
           if (need_comma)
	            o << ", ";
    	     else
    	        need_comma = true;
        	  o << int2quad(buffer, addr) << "/" << leng;
       	 } // end of foor loop
      } // end of else statement
   } // end of if statement

   o << "}";
   return o;
}

// make tree include any more specifics, i.e. ^+, ^-, ^n-m, ^n (i.e. ^n-n)
RadixTree *RadixTree::makeMoreSpecific(int code, int n, int m) {
   u_int64_t _rngs;
   int i;

   if (!this)
      return this;

   if (rngs) {
      switch (code) {
      case 0: // ^-
	 // clear first set bit make the following bits all 1's
	 rngs = rngs >> 1;
      case 1: // ^+
	 // find first set bit make the following bits all 1's
	 for (i = 0; i < 32; ++i)
	    if (rngs & bits[i])
	       break;
	 rngs |= ~masks[i];
	 break;
      case 2: // ^n-m
	 for (i = 0; i < 32; ++i)
	    if (rngs & bits[i])
	       break;
	 if (i > m) {
	    rngs = 0;
	    break;
	 }
	 if (i > n)
	    n = i;
	 if (n == 0)
	    n = 1;
	 rngs = masks[m] & ~masks[n-1];
      }

      // delete subtree of now that is included in already in _rngs
      if (rngs)
	 foreachchild(c)
	    chld[c] = chld[c]->removeRange(rngs);
   }

   foreachchild(c)
      if (chld[c])
	 chld[c] = chld[c]->makeMoreSpecific(code, n, m);

   if (!chld[0] && !chld[1] && !rngs) {
      delete this;
      return (RadixTree *) NULL;
   }

   return this;
}

void RadixTree::print() const {
   if (!this)
      return;

   static int offset = 1;
   char buffer[64];

   cout << setw(8) << setfill('0') << hex << rngs << dec << setfill(' ') 
	<< setw(offset) << " " << int2quad(buffer, addr) << "/" << leng << "\n";

   offset += 3;
   foreachchild(c)
      if (chld[c])
	 chld[c]->print();
   offset -= 3;
}


#ifdef MAIN

char* int2quad(char *buffer, unsigned int i) {
   sprintf(buffer, "%d.%d.%d.%d", 
     (i >> 24) & 0xFF, (i >> 16) & 0xFF, (i >> 8)  & 0xFF, i & 0xFF);
   return buffer;
}

RadixTree *insert(RadixTree *root, char *prfx) {
   unsigned int i, rngs;
   unsigned int i1, i2, i3, i4, i5, i6, i7;

#if 0
   sscanf(prfx, "%u.%u.%u.%u/%u", &i1, &i2, &i3, &i4, &i5);
   rngs = bits[i5];
#else
   sscanf(prfx, "%u.%u.%u.%u/%u^%u-%u", &i1, &i2, &i3, &i4, &i5, &i6, &i7);
 
   rngs = 0;
   while (i6 <= i7)
      rngs |= bits[i6++];
#endif

   i = i4 + (i3 << 8) + (i2 << 16) + (i1 << 24);
   i &= masks[i5];

   return root->insert(i, i5, rngs);
}

RadixTree *remove(RadixTree *root, char *prfx) {
   unsigned int i, rngs;
   unsigned int i1, i2, i3, i4, i5, i6, i7;

#if 1
   sscanf(prfx, "%x@%u.%u.%u.%u/%u", &rngs, &i1, &i2, &i3, &i4, &i5);
#else
   sscanf(prfx, "%u.%u.%u.%u/%u^%u-%u", &i1, &i2, &i3, &i4, &i5, &i6, &i7);
 
   rngs = 0;
   while (i6 <= i7)
      rngs |= bits[i6++];
#endif

   i = i4 + (i3 << 8) + (i2 << 16) + (i1 << 24);
   i &= masks[i5];

   return root->remove(i, i5, rngs);
}

RadixTree *root[2] = {NULL, NULL};

void pr() {
   root[0]->print();
}

main() {
   char buffer[80];
   enum { Insert, Remove } flag;
   flag = Insert;
   int set = 0;

   while (cin) {
      cin >> buffer;
      //      cout << "=>" << (char *) buffer << "\n";
      switch (buffer[0]) {
      case '#':
	 break;
      case '+':
	 set ++;
	 break;
      case '!':
	 if (root[set])
	    delete root[set];
	 root[set] = NULL;
	 break;
      case '%':
	 if (flag == Insert)
	    flag = Remove;
	 else
	    flag = Insert;
	 break;
      case '&':
	 cout << "Tree 0\n";
	 root[0]->print();   
	 cout << "Tree 1\n";
	 root[1]->print();   

	 root[0] = root[0]->and(root[1]);
	 if (root[1])
	    delete root[1];
	 root[1] = NULL;
	 cout << "Tree &\n";
	 root[0]->print();   
	 break;
      case '|':
	 cout << "Tree 0\n";
	 root[0]->print();   
	 cout << "Tree 1\n";
	 root[1]->print();   

	 root[0] = root[0]->or(root[1]);
	 if (root[1])
	    delete root[1];
	 root[1] = NULL;
	 cout << "Tree 0\n";
	 root[0]->print();   
	 break;
      case '-':
	 cout << "Tree 0\n";
	 //	 root[0]->print();   
	 cout << "Tree 1\n";
	 //	 root[1]->print();   

	 root[0] = root[0]->setminus(root[1]);
	 if (root[1])
	    delete root[1];
	 root[1] = NULL;
	 cout << "Tree 0\n";
	 root[0]->print();   
	 break;
      case 0:
	 exit(0);
	 break;
      default:
	 if (flag == Insert)
	    root[set] = insert(root[set], buffer);
	 else
	    root[set] = remove(root[set], buffer);
	 //	 root->print();
      }
   }
}

#endif
