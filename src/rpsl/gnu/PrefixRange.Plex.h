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

#ifndef _PrefixRangePlex_h
#ifdef __GNUG__
#pragma interface
#endif
#define _PrefixRangePlex_h 1

#include "std.h"
#include "Pix.h"
#include "PrefixRange.defs.h"

// Plexes are made out of PrefixRangeIChunks

#include <stddef.h>

class PrefixRangeIChunk
{
//public: // kludge until C++ `protected' policies settled
protected:      

  PrefixRange*           data;           // data, from client

  int            base;           // lowest possible index
  int            low;            // lowest valid index
  int            fence;          // highest valid index + 1
  int            top;            // highest possible index + 1

  PrefixRangeIChunk*     nxt;            // circular links
  PrefixRangeIChunk*     prv;

public:

// constructors

                 PrefixRangeIChunk(PrefixRange*     d,       // ptr to array of elements
                        int      base_idx, // initial indices
                        int      low_idx,  
                        int      fence_idx,
                        int      top_idx);

  virtual       ~PrefixRangeIChunk();

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

  int            possible_index(int i) const; // i between base and top
  int            actual_index(int i) const;   // i between low and fence
  inline virtual int    valid_index(int i) const;    // i not deleted (mainly for mchunks)

  int            possible_pointer(const PrefixRange* p) const; // same for ptr
  int            actual_pointer(const PrefixRange* p) const; 
  inline virtual int    valid_pointer(const PrefixRange* p) const; 

  PrefixRange*           pointer_to(int i) const ;   // pointer to data indexed by i
                                      // caution: i is not checked for validity
  int            index_of(const PrefixRange* p) const; // index of data pointed to by p
                                      // caution: p is not checked for validity

  inline virtual int    succ(int idx) const;     // next valid index or fence if none
  inline virtual int    pred(int idx) const;     // previous index or low - 1 if none

  inline virtual PrefixRange*   first_pointer() const;   // pointer to first valid pos or 0
  inline virtual PrefixRange*   last_pointer() const;    // pointer to first valid pos or 0
  inline virtual PrefixRange*   succ(PrefixRange*  p) const;     // next pointer or 0
  inline virtual PrefixRange*   pred(PrefixRange* p) const;     // previous pointer or 0

// modification

  inline virtual PrefixRange*   grow_high ();      // return spot to add an element
  inline virtual PrefixRange*   grow_low ();  

  inline virtual void   shrink_high ();    // logically delete top index
  inline virtual void   shrink_low ();     

  virtual void   clear(int lo);     // reset to empty ch with base = lo
  virtual void   cleardown(int hi); // reset to empty ch with top = hi
  void           re_index(int lo);  // re-index so lo is new low

// chunk traversal

  PrefixRangeIChunk*     next() const;
  PrefixRangeIChunk*     prev() const;

  void           link_to_prev(PrefixRangeIChunk* prev);
  void           link_to_next(PrefixRangeIChunk* next);
  void           unlink();

// state checks

  PrefixRange*           invalidate();     // mark self as invalid; return data
                                   // for possible deletion

  virtual int    OK() const;             // representation invariant

  void   error(const char*) const;
  void   empty_error() const;
  void   full_error() const;
  void   index_error() const;
};

// PrefixRangePlex is a partly `abstract' class: few of the virtuals
// are implemented at the Plex level, only in the subclasses

class PrefixRangePlex
{
protected:      

  PrefixRangeIChunk*       hd;          // a chunk holding the data
  int              lo;          // lowest  index
  int              fnc;         // highest index + 1
  int              csize;       // size of the chunk

  void             invalidate();              // mark so OK() is false
  void             del_chunk(PrefixRangeIChunk*);        // delete a chunk

  PrefixRangeIChunk*       tl() const;                // last chunk;
  int              one_chunk() const;         // true if hd == tl()

public:

// constructors, etc.

                    PrefixRangePlex();                  // no-op

  virtual           ~PrefixRangePlex();

  
// Access functions 
    
  virtual PrefixRange&      operator [] (int idx) = 0; // access by index;
  virtual PrefixRange&      operator () (Pix p) = 0;   // access by Pix;

  virtual PrefixRange&      high_element () = 0;      // access high element
  virtual PrefixRange&      low_element () = 0;       // access low element

// read-only versions for const Plexes

  virtual const PrefixRange& operator [] (int idx) const = 0; // access by index;
  virtual const PrefixRange& operator () (Pix p) const = 0;   // access by Pix;

  virtual const PrefixRange& high_element () const = 0;      // access high element
  virtual const PrefixRange& low_element () const = 0;       // access low element


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

  virtual int       add_high(const PrefixRange& elem) =0;// add new element at high end
                                                // return new high

  virtual int       add_low(const PrefixRange& elem) = 0;   // add new low element,
                                                // return new low

// Shrinkage

  virtual int       del_high() = 0;           // remove the element at high end
                                          // return new high
  virtual int       del_low() = 0;        // delete low element, return new lo

                                          // caution: del_low/high
                                          // does not necessarily 
                                          // immediately call PrefixRange::~PrefixRange


// operations on multiple elements

  virtual void      fill(const PrefixRange& x);          // set all elements = x
  virtual void      fill(const PrefixRange& x, int from, int to); // fill from to to
  virtual void      clear() = 0;                // reset to zero-sized Plex
  virtual int       reset_low(int newlow); // change low index,return old
  virtual void      reverse();                   // reverse in-place
  virtual void      append(const PrefixRangePlex& a);    // concatenate a copy
  virtual void      prepend(const PrefixRangePlex& a);   // prepend a copy

// status

  virtual int       can_add_high() const = 0;
  virtual int       can_add_low() const = 0;
  
  int               length () const;       // number of slots

  int               empty () const;        // is the plex empty?
  virtual int       full() const = 0;      // it it full?

  int               chunk_size() const;    // report chunk size;

  virtual int       OK() const = 0;        // representation invariant

  void		    error(const char* msg) const;
  void		    index_error() const;
  void		    empty_error() const;
  void		    full_error() const;
};


// PrefixRangeIChunk ops

inline int PrefixRangeIChunk:: size() const
{
  return top - base;
}


inline int PrefixRangeIChunk:: base_index() const
{
  return base;
}

inline  int PrefixRangeIChunk:: low_index() const
{
  return low;
}

inline  int  PrefixRangeIChunk:: fence_index() const
{
  return fence;
}

inline  int  PrefixRangeIChunk:: top_index() const
{
  return top;
}

inline  PrefixRange* PrefixRangeIChunk:: pointer_to(int i) const
{
  return &(data[i-base]);
}

inline  int  PrefixRangeIChunk:: index_of(const PrefixRange* p) const
{
  return (p - data) + base;
}

inline  int  PrefixRangeIChunk:: possible_index(int i) const
{
  return i >= base && i < top;
}

inline  int  PrefixRangeIChunk:: possible_pointer(const PrefixRange* p) const
{
  return p >= data && p < &(data[top-base]);
}

inline  int  PrefixRangeIChunk:: actual_index(int i) const
{
  return i >= low && i < fence;
}

inline  int  PrefixRangeIChunk:: actual_pointer(const PrefixRange* p) const
{
  return p >= data && p < &(data[fence-base]);
}

inline  int  PrefixRangeIChunk:: can_grow_high () const
{
  return fence < top;
}

inline  int  PrefixRangeIChunk:: can_grow_low () const
{
  return base < low;
}

inline  PrefixRange* PrefixRangeIChunk:: invalidate()
{
  PrefixRange* p = data;
  data = 0;
  return p;
}


inline PrefixRangeIChunk* PrefixRangeIChunk::prev() const
{
  return prv;
}

inline PrefixRangeIChunk* PrefixRangeIChunk::next() const
{
  return nxt;
}

inline void PrefixRangeIChunk::link_to_prev(PrefixRangeIChunk* prev)
{
  nxt = prev->nxt;
  prv = prev;
  nxt->prv = this;
  prv->nxt = this;
}

inline void PrefixRangeIChunk::link_to_next(PrefixRangeIChunk* next)
{
  prv = next->prv;
  nxt = next;
  nxt->prv = this;
  prv->nxt = this;
}

inline void PrefixRangeIChunk::unlink()
{
  PrefixRangeIChunk* n = nxt;
  PrefixRangeIChunk* p = prv;
  n->prv = p;
  p->nxt = n;
  prv = nxt = this;
}

inline  int PrefixRangeIChunk:: empty() const
{
  return low == fence;
}

inline  int  PrefixRangeIChunk:: full() const
{
  return top - base == fence - low;
}

inline int PrefixRangeIChunk:: first_index() const
{
  return (low == fence)? fence : low;
}

inline int PrefixRangeIChunk:: last_index() const
{
  return (low == fence)? low - 1 : fence - 1;
}

inline  int  PrefixRangeIChunk:: succ(int i) const
{
  return (i < low) ? low : i + 1;
}

inline  int  PrefixRangeIChunk:: pred(int i) const
{
  return (i > fence) ? (fence - 1) : i - 1;
}

inline  int  PrefixRangeIChunk:: valid_index(int i) const
{
  return i >= low && i < fence;
}

inline  int  PrefixRangeIChunk:: valid_pointer(const PrefixRange* p) const
{
  return p >= &(data[low - base]) && p < &(data[fence - base]);
}

inline  PrefixRange* PrefixRangeIChunk:: grow_high ()
{
  if (!can_grow_high()) full_error();
  return &(data[fence++ - base]);
}

inline  PrefixRange* PrefixRangeIChunk:: grow_low ()
{
  if (!can_grow_low()) full_error();
  return &(data[--low - base]);
}

inline  void PrefixRangeIChunk:: shrink_high ()
{
  if (empty()) empty_error();
  --fence;
}

inline  void PrefixRangeIChunk:: shrink_low ()
{
  if (empty()) empty_error();
  ++low;
}

inline PrefixRange* PrefixRangeIChunk::first_pointer() const
{
  return (low == fence)? 0 : &(data[low - base]);
}

inline PrefixRange* PrefixRangeIChunk::last_pointer() const
{
  return (low == fence)? 0 : &(data[fence - base - 1]);
}

inline PrefixRange* PrefixRangeIChunk::succ(PrefixRange* p) const
{
  return ((p+1) <  &(data[low - base]) || (p+1) >= &(data[fence - base])) ? 
    0 : (p+1);
}

inline PrefixRange* PrefixRangeIChunk::pred(PrefixRange* p) const
{
  return ((p-1) <  &(data[low - base]) || (p-1) >= &(data[fence - base])) ? 
    0 : (p-1);
}


// generic Plex operations

inline PrefixRangePlex::PrefixRangePlex() {}

inline int PrefixRangePlex::chunk_size() const
{
  return csize;
}

inline  int PrefixRangePlex::ecnef () const
{
  return lo - 1;
}


inline  int PrefixRangePlex::fence () const
{
  return fnc;
}

inline int PrefixRangePlex::length () const
{
  return fnc - lo;
}

inline  int PrefixRangePlex::empty () const
{
  return fnc == lo;
}

inline PrefixRangeIChunk* PrefixRangePlex::tl() const
{
  return hd->prev();
}

inline int PrefixRangePlex::one_chunk() const
{
  return hd == hd->prev();
}

#endif
