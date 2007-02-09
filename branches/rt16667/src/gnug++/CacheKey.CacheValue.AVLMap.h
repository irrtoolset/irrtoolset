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
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#ifndef _CacheKeyCacheValueAVLMap_h
#ifdef __GNUG__
#pragma interface
#endif
#define _CacheKeyCacheValueAVLMap_h 1

#include "CacheKey.CacheValue.Map.h"

struct CacheKeyCacheValueAVLNode
{
  CacheKeyCacheValueAVLNode*      lt;
  CacheKeyCacheValueAVLNode*      rt;
  CacheKey                 item;
  CacheValue                 cont;
  char                stat;
                      CacheKeyCacheValueAVLNode(CacheKey  h, CacheValue& c, 
                                    CacheKeyCacheValueAVLNode* l=0, CacheKeyCacheValueAVLNode* r=0);
                      ~CacheKeyCacheValueAVLNode();
};

inline CacheKeyCacheValueAVLNode::CacheKeyCacheValueAVLNode(CacheKey  h, CacheValue& c, 
                                    CacheKeyCacheValueAVLNode* l, CacheKeyCacheValueAVLNode* r)
     :lt(l), rt(r), item(h), cont(c), stat(0) {}

inline CacheKeyCacheValueAVLNode::~CacheKeyCacheValueAVLNode() {}

typedef CacheKeyCacheValueAVLNode* CacheKeyCacheValueAVLNodePtr;


class CacheKeyCacheValueAVLMap : public CacheKeyCacheValueMap
{
protected:
  CacheKeyCacheValueAVLNode*   root;

  CacheKeyCacheValueAVLNode*   leftmost();
  CacheKeyCacheValueAVLNode*   rightmost();
  CacheKeyCacheValueAVLNode*   pred(CacheKeyCacheValueAVLNode* t);
  CacheKeyCacheValueAVLNode*   succ(CacheKeyCacheValueAVLNode* t);
  void            _kill(CacheKeyCacheValueAVLNode* t);
  void            _add(CacheKeyCacheValueAVLNode*& t);
  void            _del(CacheKeyCacheValueAVLNode* p, CacheKeyCacheValueAVLNode*& t);

public:
                CacheKeyCacheValueAVLMap(CacheValue& dflt);
                CacheKeyCacheValueAVLMap(CacheKeyCacheValueAVLMap& a);
  inline               ~CacheKeyCacheValueAVLMap();

  CacheValue&          operator [] (CacheKey  key);

  void          del(CacheKey  key);

  inline Pix           first();
  inline void          next(Pix& i);
  inline CacheKey&          key(Pix i);
  inline CacheValue&          contents(Pix i);

  Pix           seek(CacheKey  key);
  inline int           contains(CacheKey  key);

  inline void          clear(); 

  Pix           last();
  void          prev(Pix& i);

  int           OK();
};

inline CacheKeyCacheValueAVLMap::~CacheKeyCacheValueAVLMap()
{
  _kill(root);
}

inline CacheKeyCacheValueAVLMap::CacheKeyCacheValueAVLMap(CacheValue& dflt) :CacheKeyCacheValueMap(dflt)
{
  root = 0;
}

inline Pix CacheKeyCacheValueAVLMap::first()
{
  return Pix(leftmost());
}

inline Pix CacheKeyCacheValueAVLMap::last()
{
  return Pix(rightmost());
}

inline void CacheKeyCacheValueAVLMap::next(Pix& i)
{
  if (i != 0) i = Pix(succ((CacheKeyCacheValueAVLNode*)i));
}

inline void CacheKeyCacheValueAVLMap::prev(Pix& i)
{
  if (i != 0) i = Pix(pred((CacheKeyCacheValueAVLNode*)i));
}

inline CacheKey& CacheKeyCacheValueAVLMap::key(Pix i)
{
  if (i == 0) error("null Pix");
  return ((CacheKeyCacheValueAVLNode*)i)->item;
}

inline CacheValue& CacheKeyCacheValueAVLMap::contents(Pix i)
{
  if (i == 0) error("null Pix");
  return ((CacheKeyCacheValueAVLNode*)i)->cont;
}

inline void CacheKeyCacheValueAVLMap::clear()
{
  _kill(root);
  count = 0;
  root = 0;
}

inline int CacheKeyCacheValueAVLMap::contains(CacheKey  key)
{
  return seek(key) != 0;
}

#endif
