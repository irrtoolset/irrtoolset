//  $Id$
//
//  Copyright (c) 1994 by the University of Southern California
//  All rights reserved.
//
//  Permission to use, copy, modify, and distribute this software and its
//  documentation in source and binary forms for lawful non-commercial
//  purposes and without fee is hereby granted, provided that the above
//  copyright notice appear in all copies and that both the copyright
//  notice and this permission notice appear in supporting documentation,
//  and that any documentation, advertising materials, and other materials
//  related to such distribution and use acknowledge that the software was
//  developed by the University of Southern California, Information
//  Sciences Institute. The name of the USC may not be used to endorse or
//  promote products derived from this software without specific prior
//  written permission.
//
//  THE UNIVERSITY OF SOUTHERN CALIFORNIA DOES NOT MAKE ANY
//  REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY
//  PURPOSE.  THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
//  TITLE, AND NON-INFRINGEMENT.
//
//  IN NO EVENT SHALL USC, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT, TORT,
//  OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH, THE USE
//  OR PERFORMANCE OF THIS SOFTWARE.
//
//  Questions concerning this software should be directed to
//  ratoolset@isi.edu.

// Copyright (C) 1993 Cengiz Alaettinoglu <ca@cs.umd.edu>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You didn't receive a copy of the GNU General Public License along
// with this program; so, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
//
// Author(s): Cengiz Alaettinoglu <ca@cs.umd.edu>
//            Computer Science Department
//            University of Maryland
//            College Park, MD 20742

#include "config.h"
#ifdef DEBUG
#include <cstdlib>
#include "debug.hh"

dbgstream dbg;

dbgstream::dbgstream() { 
   level=0; 
   enabled=0; 
   // only DBG_ERR and DBG_INFO levels are initially enabled
   enable(DBG_ERR);
   enable(DBG_INFO);
}

void dbgstream::enable() {
   enabled |= (1 << level);
}
void dbgstream::disable() {
   enabled &= ~(1 << level);
}
void dbgstream::enable(int level) {
   enabled |= (1 << level);
}
void dbgstream::disable(int level) {
   enabled &= ~(1 << level);
}
int dbgstream::enabledp() {
   return (enabled & (1 << level));
}
int dbgstream::enabledp(int level) {
   return (enabled & (1 << level));
}
void Abort() {
   abort();
}

void copy_constructor(char *p) {
   cerr << "Copy constructor called for " << p << "\n";
}

#endif /* DEBUG */

