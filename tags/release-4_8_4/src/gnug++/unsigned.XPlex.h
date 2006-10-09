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

#ifndef _unsignedXPlex_h
#ifdef __GNUG__
#pragma interface
#endif
#define _unsignedXPlex_h 1

#include "unsigned.Plex.h"

class unsignedXPlex: public unsignedPlex
{
  unsignedIChunk*       ch;           // cached chunk

  void             make_initial_chunks(int up = 1);

  void             cache(int idx) const;
  void             cache(const unsigned* p) const;

  unsigned*             dopred(const unsigned* p) const;
  unsigned*             dosucc(const unsigned* p) const;

  inline void             set_cache(const unsignedIChunk* t) const; // logically, 
                                               // not physically const
public:
                   unsignedXPlex();                 // set low = 0;
                                               // fence = 0;
                                               // csize = default

                   unsignedXPlex(int ch_size);      // low = 0; 
                                               // fence = 0;
                                               // csize = ch_size

                   unsignedXPlex(int lo,            // low = lo; 
                            int ch_size);      // fence=lo
                                               // csize = ch_size

                   unsignedXPlex(int lo,            // low = lo
                            int hi,            // fence = hi+1
                            const unsigned  initval,// fill with initval,
                            int ch_size = 0);  // csize= ch_size
                                               // or fence-lo if 0

                   unsignedXPlex(const unsignedXPlex&);
  
  void             operator= (const unsignedXPlex&);

// virtuals


  inline unsigned&             high_element ();
  inline unsigned&             low_element ();

  inline const unsigned&       high_element () const;
  inline const unsigned&       low_element () const;

  inline Pix              first() const;
  inline Pix              last() const;
  inline void             prev(Pix& ptr) const;
  inline void             next(Pix& ptr) const;
  int              owns(Pix p) const;
  inline unsigned&             operator () (Pix p);
  inline const unsigned&       operator () (Pix p) const;

  inline int              low() const; 
  inline int              high() const;
  inline int              valid(int idx) const;
  inline void             prev(int& idx) const;
  inline void             next(int& x) const;
  inline unsigned&             operator [] (int index);
  inline const unsigned&       operator [] (int index) const;
    
  inline int              Pix_to_index(Pix p) const;
  inline Pix              index_to_Pix(int idx) const;    

  inline int              can_add_high() const;
  inline int              can_add_low() const;
  inline int              full() const;

  int              add_high(const unsigned  elem);
  int              del_high ();
  int              add_low (const unsigned  elem);
  int              del_low ();

  void             fill(const unsigned  x);
  void             fill(const unsigned  x, int from, int to);
  void             clear();
  void             reverse();
    
  int              OK () const; 

};


inline void unsignedXPlex::prev(int& idx) const
{
  --idx;
}

inline void unsignedXPlex::next(int& idx) const
{
  ++idx;
}

inline  int unsignedXPlex::full () const
{
  return 0;
}

inline int unsignedXPlex::can_add_high() const
{
  return 1;
}

inline int unsignedXPlex::can_add_low() const
{
  return 1;
}

inline  int unsignedXPlex::valid (int idx) const
{
  return idx >= lo && idx < fnc;
}

inline int unsignedXPlex::low() const
{
  return lo;
}

inline int unsignedXPlex::high() const
{
  return fnc - 1;
}

inline unsigned& unsignedXPlex:: operator [] (int idx)
{
  if (!ch->actual_index(idx)) cache(idx);
  return *(ch->pointer_to(idx));
}

inline const unsigned& unsignedXPlex:: operator [] (int idx) const
{
  if (!ch->actual_index(idx)) cache(idx);
  return *((const unsigned*)(ch->pointer_to(idx)));
}

inline  unsigned& unsignedXPlex::low_element ()
{
  if (empty()) index_error();
  return *(hd->pointer_to(lo));
}

inline  const unsigned& unsignedXPlex::low_element () const
{
  if (empty()) index_error();
  return *((const unsigned*)(hd->pointer_to(lo)));
}

inline  unsigned& unsignedXPlex::high_element ()
{
  if (empty()) index_error();
  return *(tl()->pointer_to(fnc - 1));
}

inline const unsigned& unsignedXPlex::high_element () const
{
  if (empty()) index_error();
  return *((const unsigned*)(tl()->pointer_to(fnc - 1)));
}

inline  int unsignedXPlex::Pix_to_index(Pix px) const
{
  unsigned* p = (unsigned*)px;
  if (!ch->actual_pointer(p)) cache(p);
  return ch->index_of(p);
}

inline  Pix unsignedXPlex::index_to_Pix(int idx) const
{
  if (!ch->actual_index(idx)) cache(idx);
  return (Pix)(ch->pointer_to(idx));
}

inline Pix unsignedXPlex::first() const
{
  return Pix(hd->unsignedIChunk::first_pointer());
}

inline Pix unsignedXPlex::last() const
{
  return Pix(tl()->unsignedIChunk::last_pointer());
}

inline void unsignedXPlex::prev(Pix& p) const
{
  Pix q = Pix(ch->unsignedIChunk::pred((unsigned*) p));
  p = (q == 0)? Pix(dopred((const unsigned*) p)) : q;
}

inline void unsignedXPlex::next(Pix& p) const
{
  Pix q = Pix(ch->unsignedIChunk::succ((unsigned*) p));
  p = (q == 0)? Pix(dosucc((const unsigned*)p)) : q;
}

inline unsigned& unsignedXPlex:: operator () (Pix p)
{
  return *((unsigned*)p);
}

inline const unsigned& unsignedXPlex:: operator () (Pix p) const
{
  return *((const unsigned*)p);
}

inline void unsignedXPlex::set_cache(const unsignedIChunk* t) const
{
  ((unsignedXPlex*)(this))->ch = (unsignedIChunk*)t;
}

#endif
