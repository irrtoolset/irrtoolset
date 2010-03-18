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


#ifndef _intintMap_h
#ifdef __GNUG__
#pragma interface
#endif
#define _intintMap_h 1

#include "Pix.h"
#include "int.defs.h"

class intintMap
{
protected:
  int                   count;
  int                   def;

public:
                        intintMap(int  dflt);
  virtual              ~intintMap();

  int                   length() const; // current number of items
  int                   empty() const;

  virtual int           contains(int  key);      // is key mapped?

  virtual void          clear();                 // delete all items

  virtual int&          operator [] (int  key) = 0; // access contents by key

  virtual void          del(int  key) = 0;       // delete entry

  virtual Pix           first() = 0;             // Pix of first item or 0
  virtual void          next(Pix& i) = 0;        // advance to next or 0
  virtual int&          key(Pix i) = 0;          // access key at i
  virtual int&          contents(Pix i) = 0;     // access contents at i

  virtual int           owns(Pix i);             // is i a valid Pix  ?
  virtual Pix           seek(int  key);          // Pix of key

  int&                  dflt();                  // access default val

  void                  error(const char* msg);
  virtual int           OK() = 0;                // rep invariant
};


inline intintMap::~intintMap() {}

inline int intintMap::length() const
{
  return count;
}

inline int intintMap::empty() const
{
  return count == 0;
}

inline int& intintMap::dflt()
{
  return def;
}

inline intintMap::intintMap(int  dflt) :def(dflt)
{
  count = 0;
}

#endif
