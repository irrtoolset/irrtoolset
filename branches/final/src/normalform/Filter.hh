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
//
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#ifndef FILTER_H
#define FILTER_H

#include "config.h"
#include <ostream.h>
extern "C" {
#include <sys/types.h>
}

#include "gnug++/Pix.h"
#include "util/debug.hh"

// base type for any filter
class NEFilter {
public:
   virtual int  isEmpty() = 0;
   virtual int  is_universal() = 0;
   virtual void make_universal() = 0;
   virtual void make_empty() = 0;

   virtual void operator |= (NEFilter& b) = 0; // union
   virtual void operator &= (NEFilter& b) = 0; // intersection
   virtual void operator ~  ()       = 0; // complement

   virtual int  operator == (NEFilter& b) = 0; // equivalance
   virtual void operator =  (NEFilter& b) = 0; // assignment

   friend ostream& operator<<(ostream& stream, NEFilter& nt);
   virtual void do_print(ostream& stream) = 0;

   CLASS_DEBUG_MEMORY_HH(NEFilter);

   virtual ~NEFilter() {}
};

#endif   // FILTER_H