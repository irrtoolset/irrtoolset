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

#ifdef __GNUG__
#pragma implementation
#endif
#include "config.h"
#include "SymbolConjunctPtr.Bag.h"
#include "irrutil/errhandler.h"

// error handling

void SymbolConjunctPtrBag::error(const char* msg)
{
  lib_error_handler("Bag", msg);
}


Pix SymbolConjunctPtrBag::seek(SymbolConjunctPtr  item, Pix i)
{
  if (i == 0) 
    i = first();
  else
    next(i);
  for (;i != 0 && (!(SymbolConjunctPtrEQ((*this)(i), item))); next(i));
  return i;
}

int SymbolConjunctPtrBag::owns(Pix p)
{
  if (p == 0) return 0;
  for (Pix i = first(); i != 0; next(i)) if (i == p) return 1;
  return 0;
}

void SymbolConjunctPtrBag::clear()
{
  Pix i = first(); 
  while (i != 0)
  {
    del((*this)(i));
    i = first();
  }
}


