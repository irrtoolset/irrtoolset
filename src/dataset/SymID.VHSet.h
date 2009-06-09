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


#ifndef _SymIDVHSet_h
#ifdef __GNUG__
#pragma interface
#endif
#define _SymIDVHSet_h 1

#include "SymID.Set.h"



class SymIDVHSet : public SymIDSet
{
protected:
  SymID*          tab;
  char*         status;
  unsigned int  size;

public:
                SymIDVHSet(unsigned int sz = DEFAULT_INITIAL_CAPACITY);
                SymIDVHSet(const SymIDVHSet& a);
  inline               ~SymIDVHSet();

  Pix           add(SymID  item);
  void          del(SymID  item);
  inline int           contains(SymID  item);

  void          clear();

  Pix           first() const;
  void          next(Pix& i) const;
  inline SymID&          operator () (Pix i) const;
  Pix           seek(SymID  item) const;

  void          operator |= (SymIDVHSet& b);
  void          operator -= (SymIDVHSet& b);
  void          operator &= (SymIDVHSet& b);

  int           operator == (const SymIDVHSet& b) const;
  int           operator != (const SymIDVHSet& b) const;
  int           operator <= (const SymIDVHSet& b) const; 

  int           capacity();
  void          resize(unsigned int newsize = 0);

  int           OK();
};


inline SymIDVHSet::~SymIDVHSet()
{
  delete [] tab;
  // Fixed by wlee@isi.edu
  // delete status;
  delete [] status;
}


inline int SymIDVHSet::capacity()
{
  return size;
}

inline int SymIDVHSet::contains(SymID  key)
{
  return seek(key) != 0;
}

inline SymID& SymIDVHSet::operator () (Pix i) const
{
  if (i == 0) error("null Pix");
  return *((SymID*)i);
}

inline int SymIDVHSet::operator != (const SymIDVHSet& b) const
{
  return ! ((*this) == b);
}

#endif
