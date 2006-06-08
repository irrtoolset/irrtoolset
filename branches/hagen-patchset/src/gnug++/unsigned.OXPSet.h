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


#ifndef _unsignedOXPSet_h
#ifdef __GNUG__
#pragma interface
#endif
#define _unsignedOXPSet_h 1

#include "unsigned.Set.h"
#include "unsigned.XPlex.h"

class unsignedOXPSet : public unsignedSet
{
protected:
  unsignedXPlex       p;

public:
                unsignedOXPSet(int chunksize = DEFAULT_INITIAL_CAPACITY);
                unsignedOXPSet(const unsignedOXPSet&);

  Pix           add(unsigned  item);
  void          del(unsigned  item);
  inline int           contains(unsigned  item);

  inline void          clear();

  inline Pix           first() const;
  inline void          next(Pix& i) const;
  inline const unsigned&          operator () (Pix i) const;
  inline int           owns(Pix i);
  Pix           seek(unsigned  item);

  void          operator |= (const unsignedOXPSet& b);
  void          operator -= (const unsignedOXPSet& b);
  void          operator &= (const unsignedOXPSet& b);

  int           operator == (const unsignedOXPSet& b);
  int           operator != (const unsignedOXPSet& b);
  int           operator <= (const unsignedOXPSet& b); 

  int           OK();
};


inline unsignedOXPSet::unsignedOXPSet(int chunksize) 
     : p(chunksize) { count = 0; }

inline unsignedOXPSet::unsignedOXPSet(const unsignedOXPSet& s) : p(s.p) { count = s.count; }

inline Pix unsignedOXPSet::first() const
{
  return p.first();
}

inline void unsignedOXPSet::next(Pix  & idx) const
{
  p.next(idx);
}

inline const unsigned& unsignedOXPSet::operator ()(Pix   idx) const
{
  return p(idx);
}

inline void unsignedOXPSet::clear()
{
  count = 0;  p.clear();
}

inline int unsignedOXPSet::contains (unsigned  item)
{
  return seek(item) != 0;
}

inline int unsignedOXPSet::owns (Pix   idx)
{
  return p.owns(idx);
}

inline int unsignedOXPSet::operator != (const unsignedOXPSet& b)
{
  return !(*this == b);
}

#endif
