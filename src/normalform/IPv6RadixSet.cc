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
//  Author(s): Katie Petrusha <katie@ripe.net>

#include "config.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "IPv6RadixSet.hh"

using namespace std;

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

FixedSizeAllocator IPv6RadixTreeAllocator(sizeof(IPv6RadixTree), 1000);
bool IPv6RadixSet::compressedPrint = false;

bool IPv6RadixSet::Iterator::first(ipv6_addr_t &addr, u_int &leng, ipv6_addr_t &rngs) {
   now = itr.first();
   if (!now)
      return false;

   addr = now->addr;
   leng = now->leng;
   rngs = now->rngs;
   return true;
}

bool IPv6RadixSet::Iterator::next(ipv6_addr_t &addr, u_int &leng, ipv6_addr_t &rngs) {
   now = itr.next(now);
   if (!now)
      return false;

   addr = now->addr;
   leng = now->leng;
   rngs = now->rngs;
   return true;
}

bool IPv6RadixSet::SortedIterator::first(ipv6_addr_t &_addr, u_int &_leng, ipv6_addr_t &_rngs) {
   IPv6RadixSet::Iterator itr(set);
   PrefixLNode *p;
   ipv6_addr_t addr;
   u_int leng;
   ipv6_addr_t rngs;

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

bool IPv6RadixSet::SortedIterator::next(ipv6_addr_t &_addr, u_int &_leng, ipv6_addr_t &_rngs) {
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

bool IPv6RadixSet::PrefixIterator::first(ipv6_addr_t &_addr, u_int &_leng) {

   for (current = itr.first(); 
	current && !current->rngs; 
	current = itr.next(current)) ;

   if (!current)
      return false;

   addr = current->addr;
   leng = current->leng;
   rngs = current->rngs;

   for (cleng = leng; cleng <= 128 && (! (addr.getbits(cleng) & rngs)); cleng++);
   number = NullIPv6; 

   _addr = addr;
   _leng  = cleng;

   number = number + 1;

   return true;
}
bool IPv6RadixSet::PrefixIterator::next(ipv6_addr_t &_addr, u_int &_leng) {

   if (number == (1 << (cleng - leng))) {
      number = NullIPv6;
      for (cleng++; cleng <= 128 && (! (addr.getbits(cleng) & rngs)); cleng++) ;
      if (cleng > 128) {
      	 for (current = itr.next(current); 
	      current && !current->rngs; 
	      current = itr.next(current)) ;

	 if (!current)
	    return false;

	 addr = current->addr;
	 leng = current->leng;
	 rngs = current->rngs;
	 for (cleng = leng; 
	      cleng <= 128 && (! (addr.getbits(cleng) & rngs)); 
	      cleng++) ;
      }
   }

   _addr = addr | (number << (128 - cleng));
   _leng  = cleng;

   number = number + 1;

   return true;
}

bool IPv6RadixSet::SortedPrefixIterator::first(ipv6_addr_t &_addr, u_int &_leng) {
   IPv6RadixSet::PrefixIterator itr(set);
   PrefixLNode *p;
   ipv6_addr_t addr;
   u_int leng;

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

bool IPv6RadixSet::SortedPrefixIterator::next(ipv6_addr_t &_addr, u_int &_leng) {
   PrefixLNode *p = l.head();

   if (!p)
      return false;

   _addr = p->addr;
   _leng  = p->leng;
   l.remove(p);
   delete p;

   return true;
}

bool IPv6RadixSet::SortedPrefixRangeIterator::first(ipv6_addr_t &_addr, u_int &_leng, 
						u_int &_start, u_int &_end) {
   IPv6RadixSet::PrefixRangeIterator itr(set);
   PrefixLNode *p;
   ipv6_addr_t addr;
   u_int leng, start, end;

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

bool IPv6RadixSet::SortedPrefixRangeIterator::next(ipv6_addr_t &_addr, u_int &_leng, 
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

bool IPv6RadixSet::PrefixRangeIterator::first(ipv6_addr_t &_addr, u_int &_leng, 
					  u_int &_start, u_int &_end) {

   for (current = itr.first(); 
	current && !current->rngs; 
	current = itr.next(current)) ;

   if (!current)
      return false;

   addr = current->addr;
   leng = current->leng;
   rngs = current->rngs;
	 
   for (cleng = leng; cleng <= 128 && (! (addr.getbits(cleng) & rngs)); cleng++) ;
   _addr  = addr;
   _leng  = leng;
   _start = cleng;
   for (; (addr.getbits(cleng) & rngs) && (cleng <= 128) ; cleng++) ;
   _end   = cleng - 1;

   return true;
}

bool IPv6RadixSet::PrefixRangeIterator::next(ipv6_addr_t &_addr, u_int &_leng, 
					 u_int &_start, u_int &_end) {

   for (; ! (addr.getbits(cleng) & rngs) && (cleng <= 128); cleng++) ;

   if (cleng > 128) {
      for (current = itr.next(current); 
	   current && !current->rngs; 
	   current = itr.next(current)) ;

      if (!current)
	 return false;

      addr = current->addr;
      leng = current->leng;
      rngs = current->rngs;

      for (cleng = leng; cleng <= 128 && (!(addr.getbits(cleng) & rngs)); cleng++) ;
   }

   _addr  = addr;
   _leng  = leng;
   _start = cleng;
   for (; (addr.getbits(cleng) & rngs) && (cleng <= 128) ; cleng++) ;
   _end   = cleng - 1;

   return true;
}

static BoundedStack<IPv6RadixTree*> pStack(65);
static BoundedStack<IPv6RadixTree::Direction> dStack(65);

IPv6RadixTree::Direction IPv6RadixTree::direction(ipv6_addr_t addr, u_int leng, 
					  ipv6_addr_t _addr, u_int _leng) const {
   // assumes addr is already anded with mask

   if (leng == _leng && addr == _addr) {
      return HERE;
   }

   if (leng >= _leng) { // a parent or a sibling or aunt :-)
      return UP;
   }

   // now his mask is less than mine, either a child or UP
   if ((_addr & _addr.getmask(leng)) == addr)  // my child 
   {
      if ((_addr & _addr.getbits(leng+1)).is_true()) {
        return RIGHT;
      }
      else  {
        return LEFT;
      }
   } 

   return UP;
}

void IPv6RadixTree::commonAnscestor(ipv6_addr_t _addr,  u_int _leng, 
				ipv6_addr_t addr,   u_int leng,
				ipv6_addr_t &paddr, u_int &pleng) const {

   pleng = MIN(leng, _leng);
   while ( pleng && ((addr & addr.getmask(pleng)) != (_addr & _addr.getmask(pleng))) ) {
      pleng--;
   }
   paddr = addr & paddr.getmask(pleng);

}

// inserts arguments to the tree
IPv6RadixTree *IPv6RadixTree::insert(ipv6_addr_t _addr, u_int _leng, ipv6_addr_t _rngs) {
 
   if (! _rngs) // nothing to insert
      return this;

   if (!this)  {
      return new IPv6RadixTree(_addr, _leng, _rngs);
   }

   ipv6_addr_t common_rngs;
   int pStackPos = pStack.getPosition();

   IPv6RadixTree *root = this;
   Direction dir, pdir;
   IPv6RadixTree *parent = (IPv6RadixTree *) NULL;
   IPv6RadixTree *now = this;
   pStack.push(parent);

   IPv6RadixTree *cparent = (IPv6RadixTree *) NULL;
   ipv6_addr_t caddr;
   u_int cleng;

   while (_rngs.is_true()) {
      dir = direction(now->addr, now->leng, _addr, _leng);
      switch (dir) {
        case HERE:
        	 now->rngs = now->rngs | _rngs;
        	 goto unwind;
        	 break;
  
        case LEFT:
        case RIGHT:
        	 _rngs = _rngs & ~ now->rngs;
        	 if (! _rngs.is_true())
       	    goto cleanup; // need to restore stack here
  
        	 pdir = dir;
        	 parent = now;
        	 pStack.push(parent);
  
        	 if (! now->chld[dir])
             now->chld[dir] = new IPv6RadixTree(_addr, _leng, NullIPv6);
	         now = now->chld[dir];

        	 continue;
        	 break;

        case UP:
        	 commonAnscestor(_addr, _leng, now->addr, now->leng, caddr, cleng);

        	 cparent = new IPv6RadixTree(caddr, cleng, NullIPv6);
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
        } // end of switch
     } // end of while loop
   
     unwind: ;
   
     // delete subtree of now that is included in already in _rngs
     foreachchild(c)
        now->chld[c] = now->chld[c]->removeRange(_rngs);

     // optimize up!
     // the prefix we inserted may be the sibling of a prefix in the tree,
     // in which case both can be deleted and included in the parents range
     // well not quite deleted...
     IPv6RadixTree *tmp;
     for (now = pStack.pop(); now; now = pStack.pop()) {
        //root->print();
        if (now->chld[0] && now->chld[1] 
       	  && now->chld[0]->leng == now->leng+1
      	  && now->chld[1]->leng == now->leng+1) {
            	// found sibling not a cousin
          	 common_rngs = now->chld[0]->rngs & now->chld[1]->rngs;
          	 if (common_rngs.is_true()) {
          	    now->rngs = now->rngs | common_rngs;
          	    foreachchild(c) {
          	       now->chld[c]->rngs = now->chld[c]->rngs & ~common_rngs;
          	       if (! now->chld[c]->rngs)
              		    foreachchild(gc)
          		     if (! now->chld[c]->chld[gc]) {
                			tmp = now->chld[c];
                			now->chld[c] = tmp->chld[1-gc];
                			tmp->chld[1-gc] = (IPv6RadixTree *) NULL;
                			delete tmp;
                			break;
            	     } // end of if 
	              } // end of for loop
           	 } else // end of if
       	     break;
        } else // end of if 
        break;
     } // end of for loop

     cleanup:
        pStack.setPosition(pStackPos);

   return root;
}

bool IPv6RadixTree::contains(ipv6_addr_t _addr, u_int _leng, ipv6_addr_t _rngs) const {
   if (! _rngs) 
      return true;

   if (!this)
      return false;

   const IPv6RadixTree *now;
   Direction dir;

   for (now = this; now; ) {
      dir = direction(now->addr, now->leng, _addr, _leng);
      switch (dir) {
      case UP:
	 return false;
	 break;

      case HERE:
	 _rngs = _rngs & ~now->rngs;
	 return ! _rngs;
	 break;

      case LEFT:
      case RIGHT:
	 _rngs = _rngs & ~now->rngs;
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


IPv6RadixTree *IPv6RadixTree::remove(ipv6_addr_t _addr, u_int _leng, ipv6_addr_t _rngs) {
   if (!this || !_rngs) // nothing to delete from or nothing to delete
      return this;

   int pStackPos = pStack.getPosition();
   int dStackPos = dStack.getPosition();

   pStack.push((IPv6RadixTree *) NULL);
   dStack.push(HERE);

   ipv6_addr_t extraRngs(0,0);

   IPv6RadixTree *now, *parent, *tmp;
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
	 if ((now->rngs & _rngs).is_true()) {
	    // delete _rngs from now and add it to the other child
	    extraRngs = (now->rngs & _rngs);
	    now->rngs = now->rngs & ~_rngs;
	    foreachchild(c)
	       now->chld[c] = now->chld[c]->
	          insert(now->addr | (c ? (addr.getbits(now->leng+1)).is_true() : 0), 
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
		  now->chld[1-c] = (IPv6RadixTree *) NULL;
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
      return (IPv6RadixTree *) NULL;
   }

   return now;
}

IPv6RadixTree *IPv6RadixTree::removeRange(ipv6_addr_t _rngs) {
   if (!this || ! _rngs) // nothing to delete from or nothing to delete
      return this;

   IPv6RadixTree *tmp;

   // delete anything with rngs in _rngs
   rngs = rngs & ~_rngs;
   foreachchild(c)
      chld[c] = chld[c]->removeRange(_rngs);
   if (!rngs)
      foreachchild(c)
	 if (! chld[c]) {
	    tmp = chld[1-c];
	    chld[1-c] = (IPv6RadixTree *) NULL;
	    delete this;
	    return tmp;
	 }
   return this;
}

IPv6RadixTree *IPv6RadixTree::and_(const IPv6RadixTree *b) {
   if (this == b)
      return this;

   if (!this)
      return this;

   if (!b) {
      delete this;
      return (IPv6RadixTree *) NULL;
   }

   static BoundedStack<IPv6RadixTree*>       pStack(65);
   static BoundedStack<const IPv6RadixTree*> bStack(65);
   static BoundedStack<ipv6_addr_t>            rStack(65);
   IPv6RadixTree *result = (IPv6RadixTree *) NULL;
   IPv6RadixTree *now;
   const IPv6RadixTree *othr, *p;
   bool found;
   ipv6_addr_t incRngs, leftRngs;
   Direction dir, rdir;

   pStack.push((IPv6RadixTree *) NULL);
   pStack.push(this);
   bStack.push((IPv6RadixTree *) NULL);
   bStack.push(b);
   rStack.push(NullIPv6);
   rStack.push(NullIPv6);

   for (now = pStack.pop(), othr = bStack.pop(), incRngs = rStack.pop(); 
	now; 				                               
	now = pStack.pop(), othr = bStack.pop(), incRngs = rStack.pop()) {

      for (p = othr, found = false; p && !found; ) {
	 dir = direction(p->addr, p->leng, now->addr, now->leng);
	 switch (dir) {
	 case HERE:
	    incRngs = incRngs | p->rngs;
	 case UP:
	    found = true; 
	    break;

	 case LEFT:
	 case RIGHT:
	    incRngs = incRngs | p->rngs;
	    p = p->chld[dir];
	    break;
	 default:
	    ;
	 }
      }

      leftRngs = now->rngs;
      now->rngs = now->rngs & incRngs;
      leftRngs = leftRngs & ~incRngs;

      if ((now->rngs).is_true())
	 result = result->insert(now->addr, now->leng, now->rngs);

      if (p && leftRngs.is_true()) { 
	 rdir = direction(now->addr, now->leng, p->addr, p->leng);
	 if (rdir == LEFT || rdir == RIGHT || rdir == HERE) {
	    // insert any child of p in leftRngs under now
	    Iterator itr(p);
	    for (const IPv6RadixTree* r = itr.first(); r; r = itr.next(r))
	       if ((leftRngs & r->rngs).is_true())
		  result = result->insert(r->addr, r->leng, leftRngs & r->rngs);
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

IPv6RadixTree *IPv6RadixTree::setminus(const IPv6RadixTree *b) {
   if (this == b) {
      delete this;
      return (IPv6RadixTree *) NULL;
   }

   if (!b || !this)
      return this;

   Iterator itr(b);
   IPv6RadixTree *result = this;

   result->print();

   for (const IPv6RadixTree *now = itr.first(); now; now = itr.next(now)) {
      if (now->rngs.is_true())
         result = result->remove(now->addr, now->leng, now->rngs);
   }
   result->print();

   return result;
}

IPv6RadixTree *IPv6RadixTree::or_(const IPv6RadixTree *b) {
   if (this == b)
      return this;

   if (!b)
      return this;

   Iterator itr(b);
   IPv6RadixTree *result = this;

   for (const IPv6RadixTree *now = itr.first(); now; now = itr.next(now))
      if (now->rngs.is_true())
         result = result->insert(now->addr, now->leng, now->rngs);

   return result;

}

bool IPv6RadixTree::equals(const IPv6RadixTree *b) const {
   if (this == b)
      return true;

   Iterator itr1(this);
   Iterator itr2(b);
   const IPv6RadixTree *n1;
   const IPv6RadixTree *n2;

   for (n1 = itr1.first(),  n2 = itr2.first();
	n1 && n2;
	n1 = itr1.next(n1), n2 = itr2.next(n2))
      if (! (n1->addr == n2->addr 
	     && n1->leng == n2->leng 
	     && n1->rngs == n2->rngs))
	 return false;

   return n1 == n2;
}

ostream& operator<<(ostream& o, const IPv6RadixSet &set) {
   char buffer[256];
   bool need_comma = false;
   u_int leng=0, n=0, m=0;
   ipv6_addr_t addr = NullIPv6;

   o << "{";
   if (set.root) {
      if (IPv6RadixSet::compressedPrint) {
      	 IPv6RadixSet::PrefixRangeIterator itr(&set);
      	 for (bool flag = itr.first(addr, leng, n, m); flag; flag = itr.next(addr, leng, n, m)) {
       	    if (need_comma)
	            o << ", ";
       	    else
	            need_comma = true;
	          o << ipv62hex(&addr, buffer) << "/" << leng << "^" << n << "-" << m;
	       } // end of for loop
      } else { // not a compressed point
      	 IPv6RadixSet::PrefixIterator itr(&set);
      	 for (bool flag = itr.first(addr, leng); flag; flag = itr.next(addr, leng)) {
           if (need_comma)
             o << ", ";
	         else
	           need_comma = true;
           o << ipv62hex(&addr, buffer) << "/" << leng;
      	 } // end of for loop
       } // end of else statement
   } // end of if statement

   o << "}";
   return o;
}

// make tree include any more specifics, i.e. ^+, ^-, ^n-m, ^n (i.e. ^n-n)
IPv6RadixTree *IPv6RadixTree::makeMoreSpecific(int code, int n, int m) {
   ipv6_addr_t _rngs;
   int i;

   if (!this)
      return this;

   if (rngs.is_true()) {
      switch (code) {
      case 0: // ^-
	 // clear first set bit make the following bits all 1's
	 rngs = rngs >> 1;
      case 1: // ^+
	 // find first set bit make the following bits all 1's
	 for (i = 0; i < 128; ++i)
	    if ((rngs & addr.getbits(i)).is_true())
	       break;
	 rngs = rngs | ~addr.getmask(i);
	 break;
      case 2: // ^n-m
	 for (i = 0; i < 128; ++i)
	    if ((rngs & addr.getbits(i)).is_true())
	       break;
	 if (i > m) {
	    rngs = NullIPv6;
	    break;
	 }
	 if (i > n)
	    n = i;
	 if (n == 0)
	    n = 1;
	 rngs = addr.getmask(m) & ~(addr.getmask(n-1));
      }

      // delete subtree of now that is included in already in _rngs
      if (rngs.is_true())
      	 foreachchild(c)
	    chld[c] = chld[c]->removeRange(rngs);
   }

   foreachchild(c)
      if (chld[c])
	 chld[c] = chld[c]->makeMoreSpecific(code, n, m);

   if (!chld[0] && !chld[1] && !rngs) {
      delete this;
      return (IPv6RadixTree *) NULL;
   }

   return this;
}

void IPv6RadixTree::print() const {
   if (!this)
      return;

   static int offset = 1;

//   cout << setw(8) << setfill('0') << hex << rngs << dec << setfill(' ') 
//	<< setw(offset) << " " << int2quad(buffer, addr) << "/" << leng << "\n";
   cout << setw(0) << setfill('0');
   cout << rngs;
   cout << setfill(' ') << setw(offset) << " ";
   cout << addr;
   cout << "/" << leng << endl;
 
   offset += 3;
   foreachchild(c)
      if (chld[c])
	 chld[c]->print();
   offset -= 3;
}

IPv6RadixTree *insert(IPv6RadixTree *root, char *prfx) {
   ipv6_addr_t rngs;

   IPv6PrefixRange ip(prfx);
   return root->insert(*(ip.get_ipaddr()), ip.get_length(), ip.get_range());
}

IPv6RadixTree *remove(IPv6RadixTree *root, char *prfx) {

   ipv6_addr_t rngs;

   IPv6PrefixRange ip(prfx);
   return root->remove(*(ip.get_ipaddr()), ip.get_length(), ip.get_range());

}

inline const IPv6RadixTree* IPv6RadixTree::Iterator::first() {
   dfsStack.clear();
   dfsStack.push((IPv6RadixTree *) NULL);

   last = root;

   if (!last)
      return (const IPv6RadixTree *) NULL;

   foreachchild(c)
      if (last->chld[c])
         dfsStack.push(last->chld[c]);

   return last;
}

inline const IPv6RadixTree* IPv6RadixTree::Iterator::next(const IPv6RadixTree* _last) {
   assert(last && last == _last);

   last = dfsStack.pop();

   if (!last)
      return (const IPv6RadixTree *) NULL;

   foreachchild(c)
      if (last->chld[c])
         dfsStack.push(last->chld[c]);

   return last;
}

IPv6RadixTree *root[2] = {NULL, NULL};

