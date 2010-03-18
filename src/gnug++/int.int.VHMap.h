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


#ifndef _intintVHMap_h
#ifdef __GNUG__
#pragma interface
#endif
#define _intintVHMap_h 1

#include "int.int.Map.h"


class intintVHMap : public intintMap
{
protected:
  int*           tab;
  int*           cont;
  char*          status;
  unsigned int   size;

public:
                intintVHMap(int  dflt,unsigned int sz=DEFAULT_INITIAL_CAPACITY);
                intintVHMap(intintVHMap& a);
                ~intintVHMap();

  int&          operator [] (int  key);

  void          del(int  key);

  Pix           first();
  void          next(Pix& i);
  int&          key(Pix i);
  int&          contents(Pix i);

  Pix           seek(int  key);
  int           contains(int  key);

  void          clear(); 
  void          resize(unsigned int newsize = 0);

  int           OK();
};

inline intintVHMap::~intintVHMap()
{
  delete [] tab;
  delete [] cont;
  delete [] status;
}

inline int intintVHMap::contains(int  key)
{
  return seek(key) != 0;
}

inline int& intintVHMap::key(Pix i)
{
  if (i == 0) error("null Pix");
  return *((int*)i);
}

inline int& intintVHMap::contents(Pix i)
{
  if (i == 0) error("null Pix");
  return cont[((addr_t)(i) - (addr_t)(tab)) / sizeof(addr_t)];
}

#endif
