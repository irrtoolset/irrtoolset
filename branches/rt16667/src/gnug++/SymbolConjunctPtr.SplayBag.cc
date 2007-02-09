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

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"

#include "SymbolConjunctPtr.SplayBag.h"


/* 

 struct to simulate the special `null' node in the Sleater & Tarjan JACM 1985
 splay tree algorithms

 All routines use a version of their `simple top-down' splay alg. (p 669)

*/

struct _dummySplayNode
{
  SymbolConjunctPtrSplayNode*    lt;
  SymbolConjunctPtrSplayNode*    rt;
  SymbolConjunctPtrSplayNode*    par;
} _dummy_null;


/*
 traversal primitives
*/


SymbolConjunctPtrSplayNode* SymbolConjunctPtrSplayBag::leftmost()
{
  SymbolConjunctPtrSplayNode* t = root;
  if (t != 0) while (t->lt != 0) t = t->lt;
  return t;
}

SymbolConjunctPtrSplayNode* SymbolConjunctPtrSplayBag::rightmost()
{
  SymbolConjunctPtrSplayNode* t = root;
  if (t != 0) while (t->rt != 0) t = t->rt;
  return t;
}

SymbolConjunctPtrSplayNode* SymbolConjunctPtrSplayBag::succ(SymbolConjunctPtrSplayNode* t)
{
  if (t == 0)
    return 0;
  if (t->rt != 0)
  {
    t = t->rt;
    while (t->lt != 0) t = t->lt;
    return t;
  }
  else
  {
    for (;;)
    {
      if (t->par == 0 || t == t->par->lt)
        return t->par;
      else
        t = t->par;
    }
  }
}

SymbolConjunctPtrSplayNode* SymbolConjunctPtrSplayBag::pred(SymbolConjunctPtrSplayNode* t)
{
  if (t == 0)
    return 0;
  else if (t->lt != 0)
  {
    t = t->lt;
    while (t->rt != 0) t = t->rt;
    return t;
  }
  else
  {
    for (;;)
    {
      if (t->par == 0 || t == t->par->rt)
        return t->par;
      else
        t = t->par;
    }
  }
}



Pix SymbolConjunctPtrSplayBag::seek(SymbolConjunctPtr  key, Pix i)
{
  if (root == 0) return 0;

  SymbolConjunctPtrSplayNode* t = (SymbolConjunctPtrSplayNode*) i;
  if (t != 0)
  {
    int cmp = SymbolConjunctPtrCMP(key, t->item);
    if (cmp == 0)
    {
      t = succ(t);
      if (t != 0 && SymbolConjunctPtrEQ(key, t->item))
        return Pix(t);
      else
        return 0;
    }
    else if (cmp < 0)
      return 0;
  }

  t = root;
  int comp = SymbolConjunctPtrCMP(key, t->item);
  if (comp == 0)
    return Pix(t);

  SymbolConjunctPtrSplayNode* dummy = (SymbolConjunctPtrSplayNode*)(&_dummy_null);
  SymbolConjunctPtrSplayNode* l = dummy;
  SymbolConjunctPtrSplayNode* r = dummy;
  dummy->rt = dummy->lt = dummy->par = 0;

  while (comp != 0)
  {
    if (comp > 0)
    {
      SymbolConjunctPtrSplayNode* tr = t->rt;
      if (tr == 0)
        break;
      else
      {
        comp = SymbolConjunctPtrCMP(key, tr->item);
        if (comp <= 0 || tr->rt == 0)
        {
          l->rt = t; t->par = l;
          l = t;
          t = tr;
          if (comp >= 0)
            break;
        }
        else
        {
          if ((t->rt = tr->lt) != 0) t->rt->par = t;
          tr->lt = t; t->par = tr;
          l->rt = tr; tr->par = l;
          l = tr;
          t = tr->rt;
          comp = SymbolConjunctPtrCMP(key, t->item);
        }
      }
    }
    else
    {
      SymbolConjunctPtrSplayNode* tl = t->lt;
      if (tl == 0)
        break;
      else
      {
        comp = SymbolConjunctPtrCMP(key, tl->item);
        if (comp >= 0 || tl->lt == 0)
        {
          r->lt = t; t->par = r;
          r = t;
          t = tl;
          if (comp <= 0)
            break;
        }
        else
        {
          if ((t->lt = tl->rt) != 0) t->lt->par = t;
          tl->rt = t; t->par = tl;
          r->lt = tl; tl->par = r;
          r = tl;
          t = tl->lt;
          comp = SymbolConjunctPtrCMP(key, t->item);
        }
      }
    }
  }
  if ((r->lt = t->rt) != 0) r->lt->par = r;
  if ((l->rt = t->lt) != 0) l->rt->par = l;
  if ((t->lt = dummy->rt) != 0) t->lt->par = t;
  if ((t->rt = dummy->lt) != 0) t->rt->par = t;
  t->par = 0;
  root = t;
  if (comp != 0)
    return 0;
  else
  {
    l = pred(t);
    while (l != 0 && SymbolConjunctPtrEQ(l->item, key)) { t = l; l = pred(l); }
    return Pix(t);
  }
}

int SymbolConjunctPtrSplayBag::nof(SymbolConjunctPtr  item)
{
  int n = 0;
  SymbolConjunctPtrSplayNode* t = (SymbolConjunctPtrSplayNode*)(seek(item));
  if (t != 0)
  {
    do
    {
      ++n;
      t = succ(t);
    } while (t != 0 && SymbolConjunctPtrEQ(item, t->item));
  }
  return n;
}

Pix SymbolConjunctPtrSplayBag::add(SymbolConjunctPtr  item)
{
  ++count;
  SymbolConjunctPtrSplayNode* newnode = new SymbolConjunctPtrSplayNode(item);
  SymbolConjunctPtrSplayNode* t = root;
  if (t == 0)
  {
    root = newnode;
    return Pix(root);
  }

  int comp = SymbolConjunctPtrCMP(item, t->item);

  SymbolConjunctPtrSplayNode* dummy = (SymbolConjunctPtrSplayNode*)(&_dummy_null);
  SymbolConjunctPtrSplayNode* l = dummy;
  SymbolConjunctPtrSplayNode* r = dummy;
  dummy->rt = dummy->lt = dummy->par = 0;
  
  int done = 0;
  while (!done)
  {
    if (comp >= 0)
    {
      SymbolConjunctPtrSplayNode* tr = t->rt;
      if (tr == 0)
      {
        tr = newnode;
        comp = 0; done = 1;
      }
      else
        comp = SymbolConjunctPtrCMP(item, tr->item);
        
      if (comp <= 0)
      {
        l->rt = t; t->par = l;
        l = t;
        t = tr;
      }
      else 
      {
        SymbolConjunctPtrSplayNode* trr = tr->rt;
        if (trr == 0)
        {
          trr =  newnode;
          comp = 0; done = 1;
        }
        else
          comp = SymbolConjunctPtrCMP(item, trr->item);

        if ((t->rt = tr->lt) != 0) t->rt->par = t;
        tr->lt = t; t->par = tr;
        l->rt = tr; tr->par = l;
        l = tr;
        t = trr;
      }
    }
    else
    {
      SymbolConjunctPtrSplayNode* tl = t->lt;
      if (tl == 0)
      {
        tl = newnode;
        comp = 0; done = 1;
      }
      else
        comp = SymbolConjunctPtrCMP(item, tl->item);

      if (comp >= 0)
      {
        r->lt = t; t->par = r;
        r = t;
        t = tl;
      }
      else
      {
        SymbolConjunctPtrSplayNode* tll = tl->lt;
        if (tll == 0)
        {
          tll = newnode;
          comp = 0; done = 1;
        }
        else
          comp = SymbolConjunctPtrCMP(item, tll->item);

        if ((t->lt = tl->rt) != 0) t->lt->par = t;
        tl->rt = t; t->par = tl;
        r->lt = tl; tl->par = r;
        r = tl;
        t = tll;
      }
    }
  }
  if ((r->lt = t->rt) != 0) r->lt->par = r;
  if ((l->rt = t->lt) != 0) l->rt->par = l;
  if ((t->lt = dummy->rt) != 0) t->lt->par = t;
  if ((t->rt = dummy->lt) != 0) t->rt->par = t;
  t->par = 0;
  root = t;
  return Pix(root);
}

void SymbolConjunctPtrSplayBag::_del(SymbolConjunctPtrSplayNode* t)
{
  if (t == 0) return;

  SymbolConjunctPtrSplayNode* p = t->par;

  --count;
  if (t->rt == 0)
  {
    if (t == root)
    {
      if ((root = t->lt) != 0) root->par = 0;
    }
    else if (t == p->lt)
    {
      if ((p->lt = t->lt) != 0) p->lt->par = p;
    }
    else
      if ((p->rt = t->lt) != 0) p->rt->par = p;
  }
  else
  {
    SymbolConjunctPtrSplayNode* r = t->rt;
    SymbolConjunctPtrSplayNode* l = r->lt;
    for(;;)
    {
      if (l == 0)
      {
        if (t == root)
        {
          root = r;
          r->par = 0;
        }
        else if (t == p->lt) 
        {
          p->lt = r;
          r->par = p;
        }
        else
        {
          p->rt = r;
          r->par = p;
        }
        if ((r->lt = t->lt) != 0) r->lt->par = r;
        break;
      }
      else
      {
        if ((r->lt = l->rt) != 0) r->lt->par = r;
        l->rt = r; r->par = l;
        r = l;
        l = l->lt;
      }
    }
  }
  delete t;
}


void SymbolConjunctPtrSplayBag::remove(SymbolConjunctPtr  key)
{
  SymbolConjunctPtrSplayNode* t = (SymbolConjunctPtrSplayNode*)(seek(key));
  while (t != 0)
  {
    _del(t);
    t = (SymbolConjunctPtrSplayNode*)(seek(key));
  }
}


void SymbolConjunctPtrSplayBag::_kill(SymbolConjunctPtrSplayNode* t)
{
  if (t != 0)
  {
    _kill(t->lt);
    _kill(t->rt);
    delete t;
  }
}


SymbolConjunctPtrSplayNode* SymbolConjunctPtrSplayBag::_copy(SymbolConjunctPtrSplayNode* t)
{
  if (t != 0)
  {
    SymbolConjunctPtrSplayNode* l = _copy(t->lt);
    SymbolConjunctPtrSplayNode* r = _copy(t->rt);
    SymbolConjunctPtrSplayNode* x = new SymbolConjunctPtrSplayNode(t->item, l, r);
    if (l != 0) l->par = x;
    if (r != 0) r->par = x;
    return x;
  }
  else 
    return 0;
}

int SymbolConjunctPtrSplayBag::OK()
{
  int v = 1;
  if (root == 0) 
    v = count == 0;
  else
  {
    int n = 1;
    SymbolConjunctPtrSplayNode* trail = leftmost();
    SymbolConjunctPtrSplayNode* t = succ(trail);
    while (t != 0)
    {
      ++n;
      v &= SymbolConjunctPtrCMP(trail->item, t->item) <= 0;
      trail = t;
      t = succ(t);
    }
    v &= n == count;
  }
  if (!v) error("invariant failure");
  return v;
}

