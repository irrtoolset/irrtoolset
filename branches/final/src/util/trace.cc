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
#include <cstring>

#include "trace.hh"

Tracer trace;

char *Tracer::tracenames[TR_MAX] = { "all",
				     "whois_query", 
				     "whois_response", 
				     "input", 
				     0 };

Tracer::Tracer() { 
   enabled=0; 
}

void Tracer::enable(int level) {
   if (level) 
      enabled |= (1 << level);
   else // trace all
      enabled = ~0;
}
void Tracer::disable(int level) {
   enabled &= ~(1 << level);
}
int Tracer::enabledp(int level) {
   return (enabled & (1 << level));
}

inline int Tracer::index(char *tracename) {
   for (int i = 0; i < TR_MAX && tracenames[i]; i++)
      if (strcmp(tracename, tracenames[i]) == 0)
	 return i;

   return -1;
}

void Tracer::enable(char *tracename) {
   int i = index(tracename);
   if (i >= 0)
      enable(i);
}

void Tracer::disable(char *tracename) {
   int i = index(tracename);
   if (i >= 0)
      disable(i);
}

int Tracer::enabledp(char *tracename) {
   int i = index(tracename);
   if (i >= 0)
      return enabledp(i);
   else
      return 0;
}
