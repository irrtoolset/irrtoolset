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

#ifndef SCHEMA_HH
#define SCHEMA_HH

#include "config.h"
#include <vector>
#include <string>
#include "rptype.hh"
#include "rpsl_attr.hh"

class RPSLKeyword {
   friend class Schema;
private:
   const char *_name;		    // token in ASCII, e.g. "accept"
   int  token_id;	    // token as integer, e.g. TKN_ACCEPT
   char is_reserved;        // a reserved word returns token_id to the parser
public:
   RPSLKeyword(const char *n, int id, int is_res) : _name(n), token_id(id), 
      is_reserved(is_res) {}
};

class Object;

class Schema {
private:
   static char dictionary_text[];

   int           lastClass;
   AttrClass     *classes[64];
   RPSLKeyword   *keywords;
   RPSLKeyword   *attrSyntax;

   Object  *dictionary;

   bool forgiving;		// if true mandatory/single valued mismatches
				// can be ignored, 
                                // if *very* true (not yet implemented), 
                                // syntax errors can also be ignored
				// key attributes are always non-forgiving

   char is_context_active;  // parsers context may be active
			    // in this context rp_attributes are active
			    // when active, all words return token_id's
			    // to the parser, when non active words return
			    // TKN_WORD to the parser
			    // active context is usually filters and actions
public:
   Schema(RPSLKeyword *_keywords, RPSLKeyword *_attrSyntax) :
      keywords(_keywords), attrSyntax(_attrSyntax), 
      dictionary((Object *) NULL), forgiving(true) {
      lastClass = 0;
   }
   ~Schema();
   void initialize();
   Object *initializeFromString(char *class_text, const char *lookFor=(const char *) NULL);

   // following searches return NULL/0 on failure
   AttrClass               *searchClass(const char *name); 
   const class  AttrRPAttr *searchRPAttr(const char *name);
   int                      searchKeyword(const char *word);
   RPType                  *searchTypedef(const char *name);
   const AttrProtocol      *searchProtocol(const char *name);   
   int                      searchAttrSyntax(const char *name);
   void                     printClassStats();

   int getLastClass(void) const {
     return lastClass;
   }
   AttrClass *getClass(int i) const {
     return classes[i];
   }

   void beForgiving(bool _forgiving = true) {
      forgiving = _forgiving;
   }
   void beHarsh() {
      forgiving = false;
   }
   bool isForgiving() {
      return forgiving;
   }
   bool isVeryForgiving() {
      return false;
   }

private:
   void initializeBase();
   void addClass(AttrClass *clss);
};

extern Schema schema;

#endif   // SCHEMA_HH
