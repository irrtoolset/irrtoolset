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

#ifndef _unsignedPlex_h
#ifdef __GNUG__
#pragma interface
#endif
#define _unsignedPlex_h 1

#include "Pix.h"
#include "unsigned.defs.h"

// Plexes are made out of unsignedIChunks

#include <stddef.h>

class unsignedIChunk
{
protected:      

  unsigned*           data;           // data, from client

  int            base;           // lowest possible index
  int            low;            // lowest valid index
  int            fence;          // highest valid index + 1
  int            top;            // highest possible index + 1

  unsignedIChunk*     nxt;            // circular links
  unsignedIChunk*     prv;

public:

// constructors

                 unsignedIChunk(unsigned*     d,       // ptr to array of elements
                        int      base_idx, // initial indices
                        int      low_idx,  
                        int      fence_idx,
                        int      top_idx);

  virtual       ~unsignedIChunk();

// status reports

  int            size() const;    // number of slots

  inline virtual int    empty() const ;
  inline virtual int    full() const ; 

  int            can_grow_high () const ;  // there is space to add data
  int            can_grow_low () const;        
  
  int            base_index() const;   // lowest possible index;
  int            low_index() const;    // lowest actual index;
  inline virtual int    first_index() const;  // lowest valid index or fence if none
  inline virtual int    last_index() const;   // highest valid index or low-1 if none
  int            fence_index() const;  // highest actual index + 1
  int            top_index() const;    // highest possible index + 1

// indexing conversion

  int            actual_index(int i) const;   // i between low and fence
  inline virtual int    valid_index(int i) const;    // i not deleted (mainly for mchunks)

  int            actual_pointer(const unsigned* p) const; 
  inline virtual int    valid_pointer(const unsigned* p) const; 

  unsigned*           pointer_to(int i) const ;   // pointer to data indexed by i
                                      // caution: i is not checked for validity
  int            index_of(const unsigned* p) const; // index of data pointed to by p
                                      // caution: p is not checked for validity

  inline virtual int    succ(int idx) const;     // next valid index or fence if none
  inline virtual int    pred(int idx) const;     // previous index or low - 1 if none

  inline virtual unsigned*   first_pointer() const;   // pointer to first valid pos or 0
  inline virtual unsigned*   last_pointer() const;    // pointer to first valid pos or 0
  inline virtual unsigned*   succ(unsigned*  p) const;     // next pointer or 0
  inline virtual unsigned*   pred(unsigned* p) const;     // previous pointer or 0

// modification

  inline virtual unsigned*   grow_high ();      // return spot to add an element
  inline virtual unsigned*   grow_low ();  

  inline virtual void   shrink_high ();    // logically delete top index
  inline virtual void   shrink_low ();     

  virtual void   clear(int lo);     // reset to empty ch with base = lo
  virtual void   cleardown(int hi); // reset to empty ch with top = hi

// chunk traversal

  unsignedIChunk*     next() const;
  unsignedIChunk*     prev() const;

  void           link_to_prev(unsignedIChunk* prev);
  void           link_to_next(unsignedIChunk* next);
  void           unlink();

// state checks

  unsigned*           invalidate();     // mark self as invalid; return data
                                   // for possible deletion

  virtual int    OK() const;             // representation invariant

  void   error(const char*) const;
  void   empty_error() const;
  void   full_error() const;
  void   index_error() const;
};

// unsignedPlex is a partly `abstract' class: few of the virtuals
// are implemented at the Plex level, only in the subclasses

class unsignedPlex
{
protected:      

  unsignedIChunk*       hd;          // a chunk holding the data
  int              lo;          // lowest  index
  int              fnc;         // highest index + 1
  int              csize;       // size of the chunk

  void             invalidate();              // mark so OK() is false
  void             del_chunk(unsignedIChunk*);        // delete a chunk

  unsignedIChunk*       tl() const;                // last chunk;
  int              one_chunk() const;         // true if hd == tl()

public:

// constructors, etc.

                    unsignedPlex();                  // no-op

  virtual           ~unsignedPlex();

  
// Access functions 
    
  virtual unsigned&      operator [] (int idx) = 0; // access by index;
  virtual unsigned&      operator () (Pix p) = 0;   // access by Pix;

  virtual unsigned&      high_element () = 0;      // access high element
  virtual unsigned&      low_element () = 0;       // access low element

// read-only versions for const Plexes

  virtual const unsigned& operator [] (int idx) const = 0; // access by index;
  virtual const unsigned& operator () (Pix p) const = 0;   // access by Pix;

  virtual const unsigned& high_element () const = 0;      // access high element
  virtual const unsigned& low_element () const = 0;       // access low element


// Index functions

  virtual int       valid (int idx) const = 0;      // idx is an OK index

  virtual int       low() const = 0;         // lowest index or fence if none
  virtual int       high() const = 0;        // highest index or low-1 if none

  int               ecnef() const;         // low limit index (low-1)
  int               fence() const;         // high limit index (high+1)

  virtual void      prev(int& idx) const= 0; // set idx to preceding index
                                          // caution: pred may be out of bounds

  virtual void      next(int& idx) const = 0;       // set to next index
                                          // caution: succ may be out of bounds

  virtual Pix       first() const = 0;        // Pix to low element or 0
  virtual Pix       last() const = 0;         // Pix to high element or 0
  virtual void      prev(Pix& pix) const = 0;  // preceding pix or 0
  virtual void      next(Pix& pix) const = 0;  // next pix or 0
  virtual int       owns(Pix p) const = 0;     // p is an OK Pix

// index<->Pix 

  virtual int       Pix_to_index(Pix p) const = 0;   // get index via Pix
  virtual Pix       index_to_Pix(int idx) const = 0; // Pix via index

// Growth

  virtual int       add_high(const unsigned  elem) =0;// add new element at high end
                                                // return new high

  virtual int       add_low(const unsigned  elem) = 0;   // add new low element,
                                                // return new low

// Shrinkage

  virtual int       del_high() = 0;           // remove the element at high end
                                          // return new high
  virtual int       del_low() = 0;        // delete low element, return new lo

                                          // caution: del_low/high
                                          // does not necessarily 
                                          // immediately call unsigned::~unsigned


// operations on multiple elements

  virtual void      fill(const unsigned  x);          // set all elements = x
  virtual void      fill(const unsigned  x, int from, int to); // fill from to to
  virtual void      clear() = 0;                // reset to zero-sized Plex
  virtual void      append(const unsignedPlex& a);    // concatenate a copy
  virtual void      prepend(const unsignedPlex& a);   // prepend a copy

// status

  virtual int       can_add_high() const = 0;
  virtual int       can_add_low() const = 0;
  
  int               length () const;       // number of slots

  int               empty () const;        // is the plex empty?
  virtual int       full() const = 0;      // it it full?

  virtual int       OK() const = 0;        // representation invariant

  void		    error(const char* msg) const;
  void		    index_error() const;
  void		    empty_error() const;
  void		    full_error() const;
};


// unsignedIChunk ops

inline int unsignedIChunk:: size() const
{
  return top - base;
}


inline int unsignedIChunk:: base_index() const
{
  return base;
}

inline  int unsignedIChunk:: low_index() const
{
  return low;
}

inline  int  unsignedIChunk:: fence_index() const
{
  return fence;
}

inline  int  unsignedIChunk:: top_index() const
{
  return top;
}

inline  unsigned* unsignedIChunk:: pointer_to(int i) const
{
  return &(data[i-base]);
}

inline  int  unsignedIChunk:: index_of(const unsigned* p) const
{
  return (p - data) + base;
}

inline  int  unsignedIChunk:: actual_index(int i) const
{
  return i >= low && i < fence;
}

inline  int  unsignedIChunk:: actual_pointer(const unsigned* p) const
{
  return p >= data && p < &(data[fence-base]);
}

inline  int  unsignedIChunk:: can_grow_high () const
{
  return fence < top;
}

inline  int  unsignedIChunk:: can_grow_low () const
{
  return base < low;
}

inline  unsigned* unsignedIChunk:: invalidate()
{
  unsigned* p = data;
  data = 0;
  return p;
}


inline unsignedIChunk* unsignedIChunk::prev() const
{
  return prv;
}

inline unsignedIChunk* unsignedIChunk::next() const
{
  return nxt;
}

inline void unsignedIChunk::link_to_prev(unsignedIChunk* prev)
{
  nxt = prev->nxt;
  prv = prev;
  nxt->prv = this;
  prv->nxt = this;
}

inline void unsignedIChunk::link_to_next(unsignedIChunk* next)
{
  prv = next->prv;
  nxt = next;
  nxt->prv = this;
  prv->nxt = this;
}

inline void unsignedIChunk::unlink()
{
  unsignedIChunk* n = nxt;
  unsignedIChunk* p = prv;
  n->prv = p;
  p->nxt = n;
  prv = nxt = this;
}

inline  int unsignedIChunk:: empty() const
{
  return low == fence;
}

inline  int  unsignedIChunk:: full() const
{
  return top - base == fence - low;
}

inline int unsignedIChunk:: first_index() const
{
  return (low == fence)? fence : low;
}

inline int unsignedIChunk:: last_index() const
{
  return (low == fence)? low - 1 : fence - 1;
}

inline  int  unsignedIChunk:: succ(int i) const
{
  return (i < low) ? low : i + 1;
}

inline  int  unsignedIChunk:: pred(int i) const
{
  return (i > fence) ? (fence - 1) : i - 1;
}

inline  int  unsignedIChunk:: valid_index(int i) const
{
  return i >= low && i < fence;
}

inline  int  unsignedIChunk:: valid_pointer(const unsigned* p) const
{
  return p >= &(data[low - base]) && p < &(data[fence - base]);
}

inline  unsigned* unsignedIChunk:: grow_high ()
{
  if (!can_grow_high()) full_error();
  return &(data[fence++ - base]);
}

inline  unsigned* unsignedIChunk:: grow_low ()
{
  if (!can_grow_low()) full_error();
  return &(data[--low - base]);
}

inline  void unsignedIChunk:: shrink_high ()
{
  if (empty()) empty_error();
  --fence;
}

inline  void unsignedIChunk:: shrink_low ()
{
  if (empty()) empty_error();
  ++low;
}

inline unsigned* unsignedIChunk::first_pointer() const
{
  return (low == fence)? 0 : &(data[low - base]);
}

inline unsigned* unsignedIChunk::last_pointer() const
{
  return (low == fence)? 0 : &(data[fence - base - 1]);
}

inline unsigned* unsignedIChunk::succ(unsigned* p) const
{
  return ((p+1) <  &(data[low - base]) || (p+1) >= &(data[fence - base])) ? 
    0 : (p+1);
}

inline unsigned* unsignedIChunk::pred(unsigned* p) const
{
  return ((p-1) <  &(data[low - base]) || (p-1) >= &(data[fence - base])) ? 
    0 : (p-1);
}


// generic Plex operations

inline unsignedPlex::unsignedPlex() {}

inline  int unsignedPlex::ecnef () const
{
  return lo - 1;
}


inline  int unsignedPlex::fence () const
{
  return fnc;
}

inline int unsignedPlex::length () const
{
  return fnc - lo;
}

inline  int unsignedPlex::empty () const
{
  return fnc == lo;
}

inline unsignedIChunk* unsignedPlex::tl() const
{
  return hd->prev();
}

inline int unsignedPlex::one_chunk() const
{
  return hd == hd->prev();
}

#endif
