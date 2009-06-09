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
#include "PrefixRange.Plex.h"
#include "irrutil/errhandler.h"

// IChunk support

void PrefixRangeIChunk::error(const char* msg) const
{
  lib_error_handler("PrefixRangeIChunk", msg);
}

void PrefixRangeIChunk::index_error() const
{
  error("attempt to use invalid index");
}

void PrefixRangeIChunk::empty_error() const
{
  error("invalid use of empty chunk");
}

void PrefixRangeIChunk::full_error() const
{
  error("attempt to extend chunk beyond bounds");
}

PrefixRangeIChunk:: ~PrefixRangeIChunk() {}

PrefixRangeIChunk::PrefixRangeIChunk(PrefixRange*     d,    
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

void PrefixRangeIChunk:: re_index(int lo)
{
  int delta = lo - low;
  base += delta;
  low += delta;
  fence += delta;
  top += delta;
}


void PrefixRangeIChunk::clear(int lo)
{
  int s = top - base;
  low = base = fence = lo;
  top = base + s;
}

void PrefixRangeIChunk::cleardown(int hi)
{
  int s = top - base;
  low = top = fence = hi;
  base = top - s;
}

int PrefixRangeIChunk:: OK() const
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


void PrefixRangePlex::error(const char* msg) const
{
  lib_error_handler("Plex", msg);
}

void PrefixRangePlex::index_error() const
{
  error("attempt to access invalid index");
}

void PrefixRangePlex::empty_error() const
{
  error("attempted operation on empty plex");
}

void PrefixRangePlex::full_error() const
{
  error("attempt to increase size of plex past limit");
}

// generic plex ops

PrefixRangePlex:: ~PrefixRangePlex()
{
  invalidate();
}  


void PrefixRangePlex::append (const PrefixRangePlex& a)
{
  for (int i = a.low(); i < a.fence(); a.next(i)) add_high(a[i]);
}

void PrefixRangePlex::prepend (const PrefixRangePlex& a)
{
  for (int i = a.high(); i > a.ecnef(); a.prev(i)) add_low(a[i]);
}

void PrefixRangePlex::reverse()
{
  PrefixRange tmp;
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


void PrefixRangePlex::fill(const PrefixRange& x)
{
  for (int i = lo; i < fnc; ++i) (*this)[i] = x;
}

void PrefixRangePlex::fill(const PrefixRange& x, int lo, int hi)
{
  for (int i = lo; i <= hi; ++i) (*this)[i] = x;
}


void PrefixRangePlex::del_chunk(PrefixRangeIChunk* x)
{
  if (x != 0)
  {
    x->unlink();
    PrefixRange* data = (PrefixRange*)(x->invalidate());
    delete [] data;
    delete x;
  }
}


void PrefixRangePlex::invalidate()
{
  PrefixRangeIChunk* t = hd;
  if (t != 0)
  {
    PrefixRangeIChunk* tail = tl();
    while (t != tail)
    {
      PrefixRangeIChunk* nxt = t->next();
      del_chunk(t);
      t = nxt;
    } 
    del_chunk(t);
    hd = 0;
  }
}

int PrefixRangePlex::reset_low(int l)
{
  int old = lo;
  int diff = l - lo;
  if (diff != 0)
  {
    lo += diff;
    fnc += diff;
    PrefixRangeIChunk* t = hd;
    do
    {
      t->re_index(t->low_index() + diff);
      t = t->next();
    } while (t != hd);
  }
  return old;
}




