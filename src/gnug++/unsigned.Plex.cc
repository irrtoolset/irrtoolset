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
#include <builtin.h>
#include "unsigned.Plex.h"

// IChunk support

void unsignedIChunk::error(const char* msg) const
{
  (*lib_error_handler)("unsignedIChunk", msg);
}

void unsignedIChunk::index_error() const
{
  error("attempt to use invalid index");
}

void unsignedIChunk::empty_error() const
{
  error("invalid use of empty chunk");
}

void unsignedIChunk::full_error() const
{
  error("attempt to extend chunk beyond bounds");
}

unsignedIChunk:: ~unsignedIChunk() {}

unsignedIChunk::unsignedIChunk(unsigned*     d,    
                     int      baseidx,
                     int      lowidx,
                     int      fenceidx,
                     int      topidx)
{
  if (d == 0 || baseidx > lowidx || lowidx > fenceidx || fenceidx > topidx)
    error("inconsistent specification");
  data = d;
  base = baseidx;
  low = lowidx;
  fence = fenceidx;
  top = topidx;
  nxt = prv = this;
}

void unsignedIChunk:: re_index(int lo)
{
  int delta = lo - low;
  base += delta;
  low += delta;
  fence += delta;
  top += delta;
}


void unsignedIChunk::clear(int lo)
{
  int s = top - base;
  low = base = fence = lo;
  top = base + s;
}

void unsignedIChunk::cleardown(int hi)
{
  int s = top - base;
  low = top = fence = hi;
  base = top - s;
}

int unsignedIChunk:: OK() const
{
  int v = data != 0;             // have some data
  v &= base <= low;              // ok, index-wise
  v &= low <= fence;
  v &= fence <= top;

  v &=  nxt->prv == this;      // and links are OK
  v &=  prv->nxt == this;
  if (!v) error("invariant failure");
  return(v);
}


// error handling


void unsignedPlex::error(const char* msg) const
{
  (*lib_error_handler)("Plex", msg);
}

void unsignedPlex::index_error() const
{
  error("attempt to access invalid index");
}

void unsignedPlex::empty_error() const
{
  error("attempted operation on empty plex");
}

void unsignedPlex::full_error() const
{
  error("attempt to increase size of plex past limit");
}

// generic plex ops

unsignedPlex:: ~unsignedPlex()
{
  invalidate();
}  


void unsignedPlex::append (const unsignedPlex& a)
{
  for (int i = a.low(); i < a.fence(); a.next(i)) add_high(a[i]);
}

void unsignedPlex::prepend (const unsignedPlex& a)
{
  for (int i = a.high(); i > a.ecnef(); a.prev(i)) add_low(a[i]);
}

void unsignedPlex::reverse()
{
  unsigned tmp;
  int l = low();
  int h = high();
  while (l < h)
  {
    tmp = (*this)[l];
    (*this)[l] = (*this)[h];
    (*this)[h] = tmp;
    next(l);
    prev(h);
  }
}


void unsignedPlex::fill(const unsigned  x)
{
  for (int i = lo; i < fnc; ++i) (*this)[i] = x;
}

void unsignedPlex::fill(const unsigned  x, int lo, int hi)
{
  for (int i = lo; i <= hi; ++i) (*this)[i] = x;
}


void unsignedPlex::del_chunk(unsignedIChunk* x)
{
  if (x != 0)
  {
    x->unlink();
    unsigned* data = (unsigned*)(x->invalidate());
    delete [] data;
    delete x;
  }
}


void unsignedPlex::invalidate()
{
  unsignedIChunk* t = hd;
  if (t != 0)
  {
    unsignedIChunk* tail = tl();
    while (t != tail)
    {
      unsignedIChunk* nxt = t->next();
      del_chunk(t);
      t = nxt;
    } 
    del_chunk(t);
    hd = 0;
  }
}

int unsignedPlex::reset_low(int l)
{
  int old = lo;
  int diff = l - lo;
  if (diff != 0)
  {
    lo += diff;
    fnc += diff;
    unsignedIChunk* t = hd;
    do
    {
      t->re_index(t->low_index() + diff);
      t = t->next();
    } while (t != hd);
  }
  return old;
}




