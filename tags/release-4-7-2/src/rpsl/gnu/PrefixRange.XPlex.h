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

#ifndef _PrefixRangeXPlex_h
#ifdef __GNUG__
#pragma interface
#endif
#define _PrefixRangeXPlex_h 1

#include "PrefixRange.Plex.h"

class PrefixRangeXPlex: public PrefixRangePlex
{
  PrefixRangeIChunk*       ch;           // cached chunk

  void             make_initial_chunks(int up = 1);

  void             cache(int idx) const;
  void             cache(const PrefixRange* p) const;

  PrefixRange*             dopred(const PrefixRange* p) const;
  PrefixRange*             dosucc(const PrefixRange* p) const;

  inline void             set_cache(const PrefixRangeIChunk* t) const; // logically, 
                                               // not physically const
public:
                   PrefixRangeXPlex();                 // set low = 0;
                                               // fence = 0;
                                               // csize = default

                   PrefixRangeXPlex(int ch_size);      // low = 0; 
                                               // fence = 0;
                                               // csize = ch_size

                   PrefixRangeXPlex(int lo,            // low = lo; 
                            int ch_size);      // fence=lo
                                               // csize = ch_size

                   PrefixRangeXPlex(int lo,            // low = lo
                            int hi,            // fence = hi+1
                            const PrefixRange& initval,// fill with initval,
                            int ch_size = 0);  // csize= ch_size
                                               // or fence-lo if 0

                   PrefixRangeXPlex(const PrefixRangeXPlex&);
  
  void             operator= (const PrefixRangeXPlex&);

// virtuals


  inline PrefixRange&             high_element ();
  inline PrefixRange&             low_element ();

  inline const PrefixRange&       high_element () const;
  inline const PrefixRange&       low_element () const;

  inline Pix              first() const;
  inline Pix              last() const;
  inline void             prev(Pix& ptr) const;
  inline void             next(Pix& ptr) const;
  int              owns(Pix p) const;
  inline PrefixRange&             operator () (Pix p);
  inline const PrefixRange&       operator () (Pix p) const;

  inline int              low() const; 
  inline int              high() const;
  inline int              valid(int idx) const;
  inline void             prev(int& idx) const;
  inline void             next(int& x) const;
  inline PrefixRange&             operator [] (int index);
  inline const PrefixRange&       operator [] (int index) const;
    
  inline int              Pix_to_index(Pix p) const;
  inline Pix              index_to_Pix(int idx) const;    

  inline int              can_add_high() const;
  inline int              can_add_low() const;
  inline int              full() const;

  int              add_high(const PrefixRange& elem);
  int              del_high ();
  int              add_low (const PrefixRange& elem);
  int              del_low ();

  void             fill(const PrefixRange& x);
  void             fill(const PrefixRange& x, int from, int to);
  void             clear();
  void             reverse();
    
  int              OK () const; 

};


inline void PrefixRangeXPlex::prev(int& idx) const
{
  --idx;
}

inline void PrefixRangeXPlex::next(int& idx) const
{
  ++idx;
}

inline  int PrefixRangeXPlex::full () const
{
  return 0;
}

inline int PrefixRangeXPlex::can_add_high() const
{
  return 1;
}

inline int PrefixRangeXPlex::can_add_low() const
{
  return 1;
}

inline  int PrefixRangeXPlex::valid (int idx) const
{
  return idx >= lo && idx < fnc;
}

inline int PrefixRangeXPlex::low() const
{
  return lo;
}

inline int PrefixRangeXPlex::high() const
{
  return fnc - 1;
}

inline PrefixRange& PrefixRangeXPlex:: operator [] (int idx)
{
  if (!ch->actual_index(idx)) cache(idx);
  return *(ch->pointer_to(idx));
}

inline const PrefixRange& PrefixRangeXPlex:: operator [] (int idx) const
{
  if (!ch->actual_index(idx)) cache(idx);
  return *((const PrefixRange*)(ch->pointer_to(idx)));
}

inline  PrefixRange& PrefixRangeXPlex::low_element ()
{
  if (empty()) index_error();
  return *(hd->pointer_to(lo));
}

inline  const PrefixRange& PrefixRangeXPlex::low_element () const
{
  if (empty()) index_error();
  return *((const PrefixRange*)(hd->pointer_to(lo)));
}

inline  PrefixRange& PrefixRangeXPlex::high_element ()
{
  if (empty()) index_error();
  return *(tl()->pointer_to(fnc - 1));
}

inline const PrefixRange& PrefixRangeXPlex::high_element () const
{
  if (empty()) index_error();
  return *((const PrefixRange*)(tl()->pointer_to(fnc - 1)));
}

inline  int PrefixRangeXPlex::Pix_to_index(Pix px) const
{
  PrefixRange* p = (PrefixRange*)px;
  if (!ch->actual_pointer(p)) cache(p);
  return ch->index_of(p);
}

inline  Pix PrefixRangeXPlex::index_to_Pix(int idx) const
{
  if (!ch->actual_index(idx)) cache(idx);
  return (Pix)(ch->pointer_to(idx));
}

inline Pix PrefixRangeXPlex::first() const
{
  return Pix(hd->PrefixRangeIChunk::first_pointer());
}

inline Pix PrefixRangeXPlex::last() const
{
  return Pix(tl()->PrefixRangeIChunk::last_pointer());
}

inline void PrefixRangeXPlex::prev(Pix& p) const
{
  Pix q = Pix(ch->PrefixRangeIChunk::pred((PrefixRange*) p));
  p = (q == 0)? Pix(dopred((const PrefixRange*) p)) : q;
}

inline void PrefixRangeXPlex::next(Pix& p) const
{
  Pix q = Pix(ch->PrefixRangeIChunk::succ((PrefixRange*) p));
  p = (q == 0)? Pix(dosucc((const PrefixRange*)p)) : q;
}

inline PrefixRange& PrefixRangeXPlex:: operator () (Pix p)
{
  return *((PrefixRange*)p);
}

inline const PrefixRange& PrefixRangeXPlex:: operator () (Pix p) const
{
  return *((const PrefixRange*)p);
}

inline void PrefixRangeXPlex::set_cache(const PrefixRangeIChunk* t) const
{
  ((PrefixRangeXPlex*)(this))->ch = (PrefixRangeIChunk*)t;
}

#endif
