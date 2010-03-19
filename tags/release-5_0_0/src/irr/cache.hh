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
//  Author(s): WeeSan Lee <wlee@ISI.EDU>

#ifndef CACHE_H
#define CACHE_H

#include "dataset/CacheKey.CacheValue.AVLMap.h"

template <class K, class V>
class Cache : public CacheKeyCacheValueAVLMap {
protected:
   // Modified from void CacheKeyCacheValueAVLMap::_kill
#define LTHREADBIT        4
#define RTHREADBIT        8
   void _kill(CacheKeyCacheValueAVLNode* t) {
      if (t != 0)
	 {
	    if (!(t->stat & LTHREADBIT)) _kill(t->lt);
	    if (!(t->stat & RTHREADBIT)) _kill(t->rt);
	    if (t->cont.data())
	       delete (V)t->cont.data();
	    delete t;
	 }
   }

public:
   Cache(void) : CacheKeyCacheValueAVLMap(cDefaultValue) {}
   // Add stuff into cache 
   bool add(K aKey, V aValue) {
      (*this)[(CacheKey)(intptr_t)aKey] = CacheValue(aValue);
      return true;
   }
   ~Cache(void) { clear(); }
   // Retrieve stuff from cache
   bool query(K aKey, V &arValue) {
      CacheValue *pcValue = &(*this)[(CacheKey)(intptr_t)aKey];
      // if not found, it always return default value: cDefaultValue
      // we never increase count of default value, so, it's safe to determine
      // cache miss by just checking the reference count to see if it's 0
      if (pcValue->refCount())
	 {
	 pcValue->incRefCount();
	 arValue = (V)pcValue->data();
	 return true;
	 }
      else
	 return false;
   }
   void nullify(K aKey) {
      CacheValue *pcValue = &(*this)[(CacheKey)(intptr_t)aKey];
      pcValue->pvData = NULL;
   }
   // Override the function on the base class
   void clear(void) {
      _kill(root);
      count = 0;
      root = 0;
   }
};

#endif // CACHE_H
