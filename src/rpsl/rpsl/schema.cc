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

#include <ostream>
#include <cstring>
#include <cstdio>
#include "gnu/std.h"

#include "object.hh"
#include "schema.hh"

using namespace std;

Schema::~Schema()  {
   for (int i = 0; i < lastClass; ++i)
      delete classes[i];
   if (dictionary)
      delete dictionary;
}

AttrClass* Schema::searchClass(const char *name) {
   for (int i = 0; i < lastClass; ++i)
      if (!strcasecmp(name, classes[i]->name)) {
	 classes[i]->touch();
	 classes[i]->reset();
	 return classes[i];
      }

   return NULL;
}

void Schema::printClassStats() {
   for (int i = 0; i < lastClass; ++i)
      cout << classes[i]->name << " " << classes[i]->ref_cnt << "\n";
}

int Schema::searchKeyword(const char *word) { // return token_id or -1
   for (int i = 0; keywords[i]._name; i++)
      if (!strcasecmp(keywords[i]._name, word)) {
	 if (!is_context_active || keywords[i].is_reserved)
	    return keywords[i].token_id;
	 else
	    break;
      }

   return -1;
}

int Schema::searchAttrSyntax(const char *word) { // return token_id or -1
   for (int i = 0; attrSyntax[i]._name; i++)
      if (!strcasecmp(attrSyntax[i]._name, word)) {
	 if (!is_context_active || attrSyntax[i].is_reserved)
	    return attrSyntax[i].token_id;
	 else
	    break;
      }

   return -1;
}

RPType* Schema::searchTypedef(const char *name) {
   AttrIterator<AttrTypedef> itr(dictionary, "typedef");
   const AttrTypedef *tdef;

   for (tdef = itr.first(); tdef; tdef = itr.next())
      if (! strcasecmp(tdef->name, name))
	 return tdef->type;

   return NULL;
}

const AttrProtocol* Schema::searchProtocol(const char *name) {
   AttrIterator<AttrProtocol> itr(dictionary, "protocol");
   const AttrProtocol *prot;

   for (prot = itr.first(); prot; prot = itr.next())
      if (! strcasecmp(prot->name, name))
	 return prot;

   return (AttrProtocol *) NULL;
}

const AttrRPAttr *Schema::searchRPAttr(const char *word) { 
   // return token_id or -1
   if (!dictionary)
      return NULL;

   AttrIterator<AttrRPAttr> itr(dictionary, "rp-attribute");
   const AttrRPAttr *attr;

   for (attr = itr.first(); attr; attr = itr.next())
      if (!strcasecmp(attr->name, word))
	 return attr;

   return (AttrRPAttr *) NULL;
}

