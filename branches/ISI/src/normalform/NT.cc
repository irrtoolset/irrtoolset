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

#include "config.h"
#include <iostream.h>
#include "NT.hh"
#include "util/debug.hh"

CLASS_DEBUG_MEMORY_CC(NormalTerm);

ostream& operator<<(ostream& stream, NormalTerm& nt) {
   int i, flag = 0;

   // stream << nt.pref << " ";
   for (i = 0; i < NormalTerm::FILTER_COUNT; i++) {
      if (nt[i].is_universal())
	 continue;
      
      if (flag)
	 stream << " AND ";

      stream << nt[i];

      flag = 1;
   }

   if (!flag)
      stream << "ANY";

   return stream;
}

int NormalTerm::find_diff(NormalTerm &other) {
   int diff = FILTER_COUNT;

   for (int i = 0; i < FILTER_COUNT; i++)
      if (!((*this)[i] == other[i]))
	 if (diff == FILTER_COUNT)
	    diff = i;
	 else 
	    return -1;       // there are differences in more than one index

   // *this and other are different only on the set indexed by diff
   // or identical if diff == FILTER_COUNT
   return diff;
}
