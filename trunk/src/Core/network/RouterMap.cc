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
