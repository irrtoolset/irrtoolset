// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)
    based on code by Marc Shapiro (shapiro@sor.inria.fr)

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

#ifdef __GNUG__
#pragma implementation
#endif
#include "config.h"
#include "unsigned.XPlex.h"


unsignedXPlex:: unsignedXPlex()
{
  lo = fnc = 0;
  csize = DEFAULT_INITIAL_CAPACITY;
  unsigned* data = new unsigned[csize];
  set_cache(new unsignedIChunk(data,  lo, lo, fnc, lo+csize));
  hd = ch;
}

unsignedXPlex:: unsignedXPlex(int chunksize)
{
  if (chunksize == 0) error("invalid constructor specification");
  lo = fnc = 0;
  if (chunksize > 0)
  {
    csize = chunksize;
    unsigned* data = new unsigned[csize];
    set_cache(new unsignedIChunk(data,  lo, lo, fnc, csize));
    hd = ch;
  }
  else
  {
    csize = -chunksize;
    unsigned* data = new unsigned[csize];
    set_cache(new unsignedIChunk(data,  chunksize, lo, fnc, fnc));
    hd = ch;
  }
}


unsignedXPlex:: unsignedXPlex(int l, int chunksize)
{
  if (chunksize == 0) error("invalid constructor specification");
  lo = fnc = l;
  if (chunksize > 0)
  {
    csize = chunksize;
    unsigned* data = new unsigned[csize];
    set_cache(new unsignedIChunk(data,  lo, lo, fnc, csize+lo));
    hd = ch;
  }
  else
  {
    csize = -chunksize;
    unsigned* data = new unsigned[csize];
    set_cache(new unsignedIChunk(data,  chunksize+lo, lo, fnc, fnc));
    hd = ch;
  }
}

void unsignedXPlex::make_initial_chunks(int up)
{
  int need = fnc - lo;
  hd = 0;
  if (up)
  {
    int l = lo;
    do
    {
      int sz;
      if (need >= csize)
        sz = csize;
      else
        sz = need;
      unsigned* data = new unsigned [csize];
      unsignedIChunk* h = new unsignedIChunk(data,  l, l, l+sz, l+csize);
      if (hd != 0)
        h->link_to_next(hd);
      else
        hd = h;
      l += sz;
      need -= sz;
    } while (need > 0);
  }
  else
  {
    int hi = fnc;
    do
    {
      int sz;
      if (need >= csize)
        sz = csize;
      else
        sz = need;
      unsigned* data = new unsigned [csize];
      unsignedIChunk* h = new unsignedIChunk(data,  hi-csize, hi-sz, hi, hi);
      if (hd != 0)
        h->link_to_next(hd);
      hd = h;
      hi -= sz;
      need -= sz;
    } while (need > 0);
  }
  set_cache(hd);
}

unsignedXPlex:: unsignedXPlex(int l, int hi, const unsigned  initval, int chunksize)
{
  lo = l;
  fnc = hi + 1;
  if (chunksize == 0)
  {
    csize = fnc - l;
    make_initial_chunks(1);
  }
  else if (chunksize < 0)
  {
    csize = -chunksize;
    make_initial_chunks(0);
  }
  else
  {
    csize = chunksize;
    make_initial_chunks(1);
  }
  fill(initval);
}

unsignedXPlex::unsignedXPlex(const unsignedXPlex& a)
{
  lo = a.lo;
  fnc = a.fnc;
  csize = a.csize;
  make_initial_chunks();
  for (int i = a.low(); i < a.fence(); a.next(i)) (*this)[i] = a[i];
}

void unsignedXPlex::operator= (const unsignedXPlex& a)
{
  if (&a != this)
  {
    invalidate();
    lo = a.lo;
    fnc = a.fnc;
    csize = a.csize;
    make_initial_chunks();
    for (int i = a.low(); i < a.fence(); a.next(i)) (*this)[i] = a[i];
  }
}


void unsignedXPlex::cache(int idx) const
{
  const unsignedIChunk* tail = tl();
  const unsignedIChunk* t = ch;
  while (idx >= t->fence_index())
  {
    if (t == tail) index_error();
    t = (t->next());
  }
  while (idx < t->low_index())
  {
    if (t == hd) index_error();
    t = (t->prev());
  }
  set_cache(t);
}


void unsignedXPlex::cache(const unsigned* p) const
{
  const unsignedIChunk* old = ch;
  const unsignedIChunk* t = ch;
  while (!t->actual_pointer(p))
  {
    t = (t->next());
    if (t == old) index_error();
  }
  set_cache(t);
}

int unsignedXPlex::owns(Pix px) const
{
  unsigned* p = (unsigned*)px;
  const unsignedIChunk* old = ch;
  const unsignedIChunk* t = ch;
  while (!t->actual_pointer(p))
  {
    t = (t->next());
    if (t == old) { set_cache(t); return 0; }
  }
  set_cache(t);
  return 1;
}


unsigned* unsignedXPlex::dosucc(const unsigned* p) const
{
  if (p == 0) return 0;
  const unsignedIChunk* old = ch;
  const unsignedIChunk* t = ch;
 
  while (!t->actual_pointer(p))
  {
    t = (t->next());
    if (t == old)  return 0;
  }
  int i = t->index_of(p) + 1;
  if (i >= fnc) return 0;
  if (i >= t->fence_index()) t = (t->next());
  set_cache(t);
  return (t->pointer_to(i));
}

unsigned* unsignedXPlex::dopred(const unsigned* p) const
{
  if (p == 0) return 0;
  const unsignedIChunk* old = ch;
  const unsignedIChunk* t = ch;
  while (!t->actual_pointer(p))
  {
    t = (t->prev());
    if (t == old) return 0;
  }
  int i = t->index_of(p) - 1;
  if (i < lo) return 0;
  if (i < t->low_index()) t = (t->prev());
  set_cache(t);
  return (t->pointer_to(i));
}


int unsignedXPlex::add_high(const unsigned  elem)
{
  unsignedIChunk* t = tl();
  if (!t->can_grow_high())
  {
    if (t->unsignedIChunk::empty() && one_chunk())
      t->clear(fnc);
    else
    {
      unsigned* data = new unsigned [csize];
      t = (new unsignedIChunk(data,  fnc, fnc, fnc,fnc+csize));
      t->link_to_prev(tl());
    }
  }
  *((t->unsignedIChunk::grow_high())) = elem;
  set_cache(t);
  return fnc++;
}

int unsignedXPlex::del_high ()
{
  if (empty()) empty_error();
  unsignedIChunk* t = tl();
  t->unsignedIChunk::shrink_high();
  if (t->unsignedIChunk::empty() && !one_chunk())
  {
    unsignedIChunk* pred = t->prev();
    del_chunk(t);
    t = pred;
  }
  set_cache(t);
  return --fnc - 1;
}

int unsignedXPlex::add_low (const unsigned  elem)
{
  unsignedIChunk* t = hd;
  if (!t->can_grow_low())
  {
    if (t->unsignedIChunk::empty() && one_chunk())
      t->cleardown(lo);
    else
    {
      unsigned* data = new unsigned [csize];
      hd = new unsignedIChunk(data,  lo-csize, lo, lo, lo);
      hd->link_to_next(t);
      t = hd;
    }
  }
  *((t->unsignedIChunk::grow_low())) = elem;
  set_cache(t);
  return --lo;
}


int unsignedXPlex::del_low ()
{
  if (empty()) empty_error();
  unsignedIChunk* t = hd;
  t->unsignedIChunk::shrink_low();
  if (t->unsignedIChunk::empty() && !one_chunk())
  {
    hd = t->next();
    del_chunk(t);
    t = hd;
  }
  set_cache(t);
  return ++lo;
}

void unsignedXPlex::reverse()
{
  unsigned tmp;
  int l = lo;
  int h = fnc - 1;
  unsignedIChunk* loch = hd;
  unsignedIChunk* hich = tl();
  while (l < h)
  {
    unsigned* lptr = loch->pointer_to(l);
    unsigned* hptr = hich->pointer_to(h);
    tmp = *lptr;
    *lptr = *hptr;
    *hptr = tmp;
    if (++l >= loch->fence_index()) loch = loch->next();
    if (--h < hich->low_index()) hich = hich->prev();
  }
}

void unsignedXPlex::fill(const unsigned  x)
{
  for (int i = lo; i < fnc; ++i) (*this)[i] = x;
}

void unsignedXPlex::fill(const unsigned  x, int l, int hi)
{
  for (int i = l; i <= hi; ++i) (*this)[i] = x;
}


void unsignedXPlex::clear()
{
  if (fnc != lo)
  {
    unsignedIChunk* t = tl();
    while (t != hd)
    {
      unsignedIChunk* prv = t->prev();
      del_chunk(t);
      t = prv;
    }
    t->unsignedIChunk::clear(lo);
    set_cache(t);
    fnc = lo;
  }
}


int unsignedXPlex::OK () const
{
  int v = hd != 0 && ch != 0;     // at least one chunk

  v &= fnc == tl()->fence_index();// last chunk fence == plex fence
  v &= lo == ((hd))->unsignedIChunk::low_index();    // first lo == plex lo

// loop for others:
  int found_ch = 0;                   // to make sure ch is in list;
  const unsignedIChunk* t = (hd);
  for (;;)
  {
    if (t == ch) ++found_ch;
    v &= t->unsignedIChunk::OK();              // each chunk is OK
    if (t == tl())
      break;
    else                              // and has indices contiguous to succ
    {
      v &= t->top_index() == t->next()->base_index();
      if (t != hd)                  // internal chunks full
      {
        v &= !t->empty();
        v &= !t->can_grow_low();
        v &= !t->can_grow_high();
      }
      t = t->next();
    }
  }
  v &= found_ch == 1;
  if (!v) error("invariant failure");
  return v;
}
