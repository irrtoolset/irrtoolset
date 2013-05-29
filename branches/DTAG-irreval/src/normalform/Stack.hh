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

#ifndef STACK_H
#define STACK_H

#include "config.h"

// the following queue and stack implementations assume that they never overflow
// use with caution!

#include <cassert>

template <class TYPE>
class BoundedQueue {
private:
   BoundedQueue(BoundedQueue& a);

public:
   BoundedQueue(int size) {
      q = new TYPE[size]; 
      first = 0;
      last = 0;
      BoundedQueue::size = size;
   }
   ~BoundedQueue() {
      delete [] q; 
   }

   void enq(TYPE elt) { 
      assert(last < size);
      q[last++] = elt; 
   }
   TYPE deq() { return (q[first++]); }
   int empty() { return (first == last); }
   void clear() {
      first = 0;
      last = 0;
   }

protected:
   TYPE *q; 
   int first;
   int last;
   int size;
};

template <class TYPE>
class BoundedStack : BoundedQueue<TYPE> {
public:
   BoundedStack(int size) : BoundedQueue<TYPE>(size) {}
   void push(TYPE elt) { this->enq(elt); }
   TYPE pop() { return this->q[--(this->last)]; }
   int getPosition() { return this->last; }
   void setPosition(int pos) { this->last = pos; }
   void clear() {
      BoundedQueue<TYPE>::clear();
   }
};

#endif   // STACK_H
