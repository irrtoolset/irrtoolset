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


#ifndef _SymIDSet_h
#ifdef __GNUG__
#pragma interface
#endif
#define _SymIDSet_h 1

#include "Pix.h"
#include "SymID.defs.h"

class SymIDSet
{
protected:

  int                   count;

public:
  inline virtual              ~SymIDSet();

  int                   length() const;              // current number of items
  int                   empty() const;

  virtual Pix           add(SymID  item) = 0;      // add item; return Pix
  virtual void          del(SymID  item) = 0;      // delete item
  virtual int           contains(SymID  item) const;     // is item in set?

  virtual void          clear();                 // delete all items

  virtual Pix           first() const = 0;             // Pix of first item or 0
  virtual void          next(Pix& i) const = 0;        // advance to next or 0
  virtual SymID&          operator () (Pix i) const = 0; // access item at i

  virtual int           owns(Pix i);             // is i a valid Pix  ?
  virtual Pix           seek(SymID  item) const;         // Pix of item

  void                  operator |= (SymIDSet& b); // add all items in b
  void                  operator -= (SymIDSet& b); // delete items also in b
  void                  operator &= (SymIDSet& b); // delete items not in b

  int                   operator == (const SymIDSet& b) const;
  int                   operator != (const SymIDSet& b) const;
  int                   operator <= (const SymIDSet& b) const; 

  void                  error(const char* msg) const;
  virtual int           OK() = 0;                // rep invariant
};

inline SymIDSet::~SymIDSet() {}

inline int SymIDSet::length() const
{
  return count;
}

inline int SymIDSet::empty() const
{
  return count == 0;
}

#endif
