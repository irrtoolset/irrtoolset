// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988, 1982 Free Software Foundation
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

#ifndef _SymbolConjunctPtrSplayNode
#define _SymbolConjunctPtrSplayNode 1
#ifdef __GNUG__
#pragma interface
#endif
#include "config.h"
#include "SymbolConjunctPtr.defs.h"

struct SymbolConjunctPtrSplayNode
{
  SymbolConjunctPtrSplayNode*   lt;
  SymbolConjunctPtrSplayNode*   rt;
  SymbolConjunctPtrSplayNode*   par;
  SymbolConjunctPtr             item;
                  SymbolConjunctPtrSplayNode(SymbolConjunctPtr  h, SymbolConjunctPtrSplayNode* l=0, SymbolConjunctPtrSplayNode* r=0);
                  ~SymbolConjunctPtrSplayNode();
};


inline SymbolConjunctPtrSplayNode::SymbolConjunctPtrSplayNode(SymbolConjunctPtr  h, SymbolConjunctPtrSplayNode* l, SymbolConjunctPtrSplayNode* r)
:lt(l), rt(r), par(0), item(h) {}

inline SymbolConjunctPtrSplayNode::~SymbolConjunctPtrSplayNode() {}

typedef SymbolConjunctPtrSplayNode* SymbolConjunctPtrSplayNodePtr;

#endif
