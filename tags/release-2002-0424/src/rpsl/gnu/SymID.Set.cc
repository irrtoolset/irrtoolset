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

#ifdef __GNUG__
#pragma implementation
#endif
#include "config.h"
#include <builtin.h>
#include "SymID.Set.h"


Pix SymIDSet::seek(SymID  item) const
{
  Pix i;
  for (i = first(); i != 0 && !(SymIDEQ((*this)(i), item)); next(i));
  return i;
}

int SymIDSet::owns(Pix idx)
{
  if (idx == 0) return 0;
  for (Pix i = first(); i; next(i)) if (i == idx) return 1;
  return 0;
}

void SymIDSet::clear()
{
  Pix i = first(); 
  while (i != 0)
  {
    del((*this)(i));
    i = first();
  }
}

int SymIDSet::contains (SymID  item) const
{
  return seek(item) != 0;
}

int SymIDSet::operator <= (const SymIDSet& b) const
{
  if (count > b.count) return 0;
  if (count == 0) return 1;
  for (Pix i = first(); i; next(i)) if (b.seek((*this)(i)) == 0) return 0;
  return 1;
}

int SymIDSet::operator == (const SymIDSet& b) const
{
  int n = count;
  if (n != b.count) return 0;
  if (n == 0) return 1;
  Pix i = first();
  Pix j = b.first();
  while (n-- > 0)
  {
    if ((b.seek((*this)(i)) == 0) || (seek(b(j)) == 0)) return 0;
    next(i);
    b.next(j);
  }
  return 1;
}

int SymIDSet::operator != (const SymIDSet& b) const
{
  return !(*this == b);
}

void SymIDSet::operator |= (SymIDSet& b)
{
  if (&b != this)
    for (Pix i = b.first(); i; b.next(i)) add(b(i));
}

void SymIDSet::operator -= (SymIDSet& b)
{
  if (&b == this)
    clear();
  else
    for (Pix i = b.first(); i; b.next(i)) del(b(i));
}


void SymIDSet::operator &= (SymIDSet& b)
{
  if (&b != this)
  {
    Pix i = first();
    Pix n = i;
    while (i != 0)
    {
      next(n);
      if (b.seek((*this)(i)) == 0) del((*this)(i));
      i = n;
    }
  }
}

void SymIDSet::error(const char* msg) const
{
  (*lib_error_handler)("Set", msg);
}
