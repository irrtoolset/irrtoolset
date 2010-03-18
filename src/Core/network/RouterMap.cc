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
#include "RouterMap.hh"

void __RouterMap::RouterEntry::pingHandler(void *ptr, void *arg) {
   __RouterMap *rmap = (__RouterMap *) ptr;
   TProbe *ping = (TProbe *) arg;
   if (ping->responder != ping->to && ping->result == TProbeResultReachedNode)
      rmap->__alias(ping->to, ping->responder);
}

void __RouterMap::RouterEntry::timerHandler(void *ptr, void *arg) {
   RouterEntry *re = (RouterEntry *) ptr;
   //re->ping->send();
}

__RouterMap::__RouterMap(unsigned int _capacity) : capacity(_capacity) {
   entries = new List<RouterEntry> [capacity];
}

__RouterMap::~__RouterMap() {
   delete [] entries;
}

void __RouterMap::checkPoint(char code, FILE *f) {
   for (int i = 0; i < capacity; ++i)
      if (! entries[i].isEmpty())
	 for (ListIterator<RouterEntry> itr(entries[i]); itr; ++itr)
	    if (itr->ip != itr->alias)
	       fprintf(f, "%c\t%s\t%s\n", 
		       code, itr->ip.name(), itr->alias.name());
}

void __RouterMap::recover(char code, FILE *f) {
   int c;
   char ip1str[128], ip2str[128];
   while ((c = fgetc(f)) == code) {
      fscanf(f, "%s %s", ip1str, ip2str);
      __alias(ip1str, ip2str);
   }
   if (c != EOF)
      ungetc(c, f);
}

void __RouterMap::__alias(const Address &ip, const Address &alias) {
   if (ip == alias)
      return;

   RouterEntry *re = find(ip);
   re->alias = alias;
   re->itimer->stop(); // stop searching for an alias

   RouterEntry *reAlias = find(alias);
   if (!reAlias) {
      insert(alias, re->data);
      return;
   }

   if (re->data == reAlias->data)
      return;

   void *data = reAlias->data;
   reAlias->data = re->data;

   this->alias(re->data, data);
}
