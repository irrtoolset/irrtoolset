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
#include "CacheKey.CacheValue.Map.h"
#include <builtin.h>


Pix CacheKeyCacheValueMap::seek(CacheKey  item)
{
  Pix i;
  for (i = first(); i != 0 && !(CacheKeyEQ(key(i), item)); next(i));
  return i;
}

int CacheKeyCacheValueMap::owns(Pix idx)
{
  if (idx == 0) return 0;
  for (Pix i = first(); i; next(i)) if (i == idx) return 1;
  return 0;
}

void CacheKeyCacheValueMap::clear()
{
  Pix i = first(); 
  while (i != 0)
  {
    del(key(i));
    i = first();
  }
}

int CacheKeyCacheValueMap::contains (CacheKey  item)
{
  return seek(item) != 0;
}


void CacheKeyCacheValueMap::error(const char* msg)
{
  (*lib_error_handler)("Map", msg);
}
