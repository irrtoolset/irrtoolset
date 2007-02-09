// This may look like C code, but it is really -*- C++ -*-
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
// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988, 1982 Free Software Foundation
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


#ifndef _SymbolConjunctPtrSplayBag_h
#ifdef __GNUG__
#pragma interface
#endif
#define _SymbolConjunctPtrSplayBag_h 1

#include "config.h"
#include "SymbolConjunctPtr.Bag.h"
#include "SymbolConjunctPtr.SplayNode.h"

class SymbolConjunctPtrSplayBag : public SymbolConjunctPtrBag
{
protected:
  SymbolConjunctPtrSplayNode*   root;

  SymbolConjunctPtrSplayNode*   leftmost();
  SymbolConjunctPtrSplayNode*   rightmost();
  SymbolConjunctPtrSplayNode*   pred(SymbolConjunctPtrSplayNode* t);
  SymbolConjunctPtrSplayNode*   succ(SymbolConjunctPtrSplayNode* t);
  void            _kill(SymbolConjunctPtrSplayNode* t);
  SymbolConjunctPtrSplayNode*   _copy(SymbolConjunctPtrSplayNode* t);
  void            _del(SymbolConjunctPtrSplayNode* t);

public:
                  SymbolConjunctPtrSplayBag();
                  SymbolConjunctPtrSplayBag(SymbolConjunctPtrSplayBag& a);
                  ~SymbolConjunctPtrSplayBag();

  Pix           add(SymbolConjunctPtr  item);
  void          del(SymbolConjunctPtr  item);
   void         del_this(Pix i); // added by cengiz alaettinoglu
  void          remove(SymbolConjunctPtr item);
  int           nof(SymbolConjunctPtr  item);
  int           contains(SymbolConjunctPtr  item);

  void          clear();

  Pix           first();
  void          next(Pix& i);
  SymbolConjunctPtr&          operator () (Pix i);
  Pix           seek(SymbolConjunctPtr  item, Pix from = 0);

  Pix           last();
  void          prev(Pix& i);

  int           OK();
};


inline SymbolConjunctPtrSplayBag::~SymbolConjunctPtrSplayBag()
{
  _kill(root);
}

inline SymbolConjunctPtrSplayBag::SymbolConjunctPtrSplayBag()
{
  root = 0;
  count = 0;
}

inline SymbolConjunctPtrSplayBag::SymbolConjunctPtrSplayBag(SymbolConjunctPtrSplayBag& b)
{
  count = b.count;
  root = _copy(b.root);
}

inline Pix SymbolConjunctPtrSplayBag::first()
{
  return Pix(leftmost());
}

inline Pix SymbolConjunctPtrSplayBag::last()
{
  return Pix(rightmost());
}

inline void SymbolConjunctPtrSplayBag::next(Pix& i)
{
  if (i != 0) i = Pix(succ((SymbolConjunctPtrSplayNode*)i));
}

inline void SymbolConjunctPtrSplayBag::prev(Pix& i)
{
  if (i != 0) i = Pix(pred((SymbolConjunctPtrSplayNode*)i));
}

inline SymbolConjunctPtr& SymbolConjunctPtrSplayBag::operator () (Pix i)
{
  if (i == 0) error("null Pix");
  return ((SymbolConjunctPtrSplayNode*)i)->item;
}

inline void SymbolConjunctPtrSplayBag::clear()
{
  _kill(root);
  count = 0;
  root = 0;
}

inline int SymbolConjunctPtrSplayBag::contains(SymbolConjunctPtr  key)
{
  return seek(key) != 0;
}

inline void SymbolConjunctPtrSplayBag::del(SymbolConjunctPtr  key)
{
  _del((SymbolConjunctPtrSplayNode*)(seek(key)));
}

// added by Cengiz Alaettinoglu
inline void SymbolConjunctPtrSplayBag::del_this(Pix i)
{
  _del((SymbolConjunctPtrSplayNode*)i);
}

#endif
