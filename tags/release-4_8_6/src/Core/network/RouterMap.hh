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

#ifndef RouterMap_HH
#define RouterMap_HH

#include <cstdio>      // For NULL definition
#include <assert.h>
#include <iostream.h>
#include "util/List.hh"
#include "sys/Address.hh"
#include "TProbe.hh"
#include "sched/Timer.hh"

const TimeShort aliasVerificationInterval(14400,0); // 4 hours

class __RouterMap {
public:
   class RouterEntry : public ListNode {
   public:
      Address ip;
      Address alias;
      void *data;
      TProbe *ping;
      ITimer *itimer;

   public:
      RouterEntry(const Address &_ip, void *_data, __RouterMap *rmap) 
	 : ip(_ip), alias(_ip), data(_data) {
	 //Handler ph(pingHandler, rmap);
	 //ping = new TProbe(&ip, ph, 255);
	 //ping->send();

	 Handler th(timerHandler, this);
	 itimer = new ITimer(th, aliasVerificationInterval);
      }
      
   private:
      RouterEntry(const RouterEntry&);
      static void pingHandler(void *rtr, void *arg); // args: rmap, ping
      static void timerHandler(void *rtr, void *arg); // args: rmap, ping
   };
   
   // End class RouterEntry

   unsigned int capacity;
   List<RouterEntry> *entries;

   int hash(U32 ip) {
      int sum = ip % capacity;
      ip /= capacity;
      while (ip > 0) {
	 sum ^= ip % capacity;
	 ip /= capacity;
      }
      return sum;
   }

   __RouterMap(const __RouterMap &);
   
public:
   __RouterMap(unsigned int _capacity);
   ~__RouterMap();

   void insert(const Address &ip, void *rtr) {
      entries[hash(ip)].append(new RouterEntry(ip, rtr, this));
   }
   RouterEntry *find(const Address &ip) {
      for (ListIterator<RouterEntry> itr(entries[hash(ip)]); itr; ++itr)
	 if (itr->ip == ip)
	    return itr;
      return NULL;
   }

   void __alias(const Address &ip, const Address &alias);
   virtual void alias(void *rtr1, void *rtr2) = 0;
   void checkPoint(char code, FILE *f);
   void recover(char code, FILE *f);
};

// this template class provides typechecking to __RouterMap class
// we needed so that we can have a .o file for __RouterMap that does not
// need to know class Router at compile time
template <class Router>
class RouterMap : private __RouterMap, public List<Router> {
public:
   RouterMap(unsigned int capacity = 1024*4) : __RouterMap(capacity) {
   }
   Router *insert(const Address &ip, Router *rtr) {
      __RouterMap::insert(ip, rtr);
      List<Router>::append(rtr);
      return rtr;
   }
   Router *find(const Address &ip) {
      RouterEntry *re = __RouterMap::find(ip);
      return re ? (Router *) re->data : NULL;
   }
   void checkPoint(char code, FILE *f) {
      __RouterMap::checkPoint(code, f);
   }
   void recover(char code, FILE *f) {
      __RouterMap::recover(code, f);
   }
   virtual void alias(void *rtr1, void *rtr2) {
      Router *rtr = (Router *) rtr1;
      Router *rtrAlias = (Router *) rtr2;

      rtr->inherit(rtrAlias);
      remove(rtrAlias);
      delete rtrAlias;
   }
};

#endif // RouterMap_HH
