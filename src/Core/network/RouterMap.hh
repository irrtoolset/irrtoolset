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
//  scan@isi.edu.
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
