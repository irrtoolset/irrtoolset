// This may look like C code, but it is really -*- C++ -*-
// WARNING: This file is obsolete.  Use ../DLList.h, if you can.
/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#ifndef _NormalTermPtrDLList_h
#ifdef __GNUG__
#pragma interface
#endif
#define _NormalTermPtrDLList_h 1

#include "config.h"
#include "Pix.h"
#include "NormalTermPtr.defs.h"

#ifndef _NormalTermPtrDLListNode_h
#define _NormalTermPtrDLListNode_h 1

struct NormalTermPtrDLListNode
{
  NormalTermPtrDLListNode*         bk;
  NormalTermPtrDLListNode*         fd;
  NormalTermPtr                    hd;
                         NormalTermPtrDLListNode();
                         NormalTermPtrDLListNode(const NormalTermPtr  h, 
                                       NormalTermPtrDLListNode* p = 0,
                                       NormalTermPtrDLListNode* n = 0);
                         ~NormalTermPtrDLListNode();
};

inline NormalTermPtrDLListNode::NormalTermPtrDLListNode() {}

inline NormalTermPtrDLListNode::NormalTermPtrDLListNode(const NormalTermPtr  h, NormalTermPtrDLListNode* p,
                                    NormalTermPtrDLListNode* n)
  :bk(p), fd(n), hd(h) {}

inline NormalTermPtrDLListNode::~NormalTermPtrDLListNode() {}

typedef NormalTermPtrDLListNode* NormalTermPtrDLListNodePtr;

#endif

class NormalTermPtrDLList
{
  friend class          NormalTermPtrDLListTrav;

  NormalTermPtrDLListNode*        h;

public:
                        NormalTermPtrDLList();
                        NormalTermPtrDLList(const NormalTermPtrDLList& a);
                        ~NormalTermPtrDLList();

  NormalTermPtrDLList&            operator = (const NormalTermPtrDLList& a);

  int                   empty();
  int                   length();

  void                  clear();

  Pix                   prepend(NormalTermPtr  item);
  Pix                   append(NormalTermPtr  item);
  void                  join(NormalTermPtrDLList&);

  NormalTermPtr&                  front();
  void                  del_front();

  NormalTermPtr&                  rear();

  NormalTermPtr&                  operator () (Pix p);
  Pix                   first();
  Pix                   last();
  void                  next(Pix& p);
  void                  prev(Pix& p);
  int                   owns(Pix p);
  void                  del(Pix& p, int dir = 1);

  void                  error(const char* msg);
  int                   OK();
};


inline NormalTermPtrDLList::~NormalTermPtrDLList()
{
  clear();
}

inline NormalTermPtrDLList::NormalTermPtrDLList()
{
  h = 0;
}

inline int NormalTermPtrDLList::empty()
{
  return h == 0;
}


inline void NormalTermPtrDLList::next(Pix& p)
{
  p = (p == 0 || p == h->bk)? 0 : Pix(((NormalTermPtrDLListNode*)p)->fd);
}

inline void NormalTermPtrDLList::prev(Pix& p)
{
  p = (p == 0 || p == h)? 0 : Pix(((NormalTermPtrDLListNode*)p)->bk);
}

inline Pix NormalTermPtrDLList::first()
{
  return Pix(h);
}

inline Pix NormalTermPtrDLList::last()
{
  return (h == 0)? 0 : Pix(h->bk);
}

inline NormalTermPtr& NormalTermPtrDLList::operator () (Pix p)
{
  if (p == 0) error("null Pix");
  return ((NormalTermPtrDLListNode*)p)->hd;
}

inline NormalTermPtr& NormalTermPtrDLList::front()
{
  if (h == 0) error("front: empty list");
  return h->hd;
}

inline NormalTermPtr& NormalTermPtrDLList::rear()
{
  if (h == 0) error("rear: empty list");
  return h->bk->hd;
}

#endif
