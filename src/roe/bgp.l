/* 
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
*/


/* definitions */
%x SKIP_REST

%{

#include "config.h"
// Added by wlee@isi.edu
#include "normalform/RadixSet.hh"
#include <cstring>

#undef YY_DECL
#define YY_DECL int readbgpdump(RadixSet& routes)

extern "C" {
int yywrap () {
   return 1;
}
}

static char bgp_buffer[64];
int count = 0;

%}

%%

%{
/* Rules */
%}

[0-9]+(\.[0-9]+){3,3}\/[0-9]+ {
// Modified by wlee@ISI.EDU
//   routes.add(Prefask_map.add_entry(yytext));
   routes.insert(PrefixRange(yytext));
   BEGIN(SKIP_REST);
}

[0-9]+\.0\.0\.0 {  // /8
   strcpy(bgp_buffer, yytext);
   strcat(bgp_buffer, "/8");
// Modified by wlee@ISI.EDU
//   routes.add(Prefask_map.add_entry(bgp_buffer));
   routes.insert(PrefixRange(bgp_buffer));
   BEGIN(SKIP_REST);
}

[0-9]+(\.[0-9]+){1,1}\.0\.0 { // /16
   strcpy(bgp_buffer, yytext);
   strcat(bgp_buffer, "/16");
// Modified by wlee@ISI.EDU
//   routes.add(Prefask_map.add_entry(bgp_buffer));
   routes.insert(PrefixRange(bgp_buffer));
   BEGIN(SKIP_REST);
}

[0-9]+(\.[0-9]+){2,2}\.0 { // /24
   strcpy(bgp_buffer, yytext);
   strcat(bgp_buffer, "/24");
// Modified by wlee@ISI.EDU
//   routes.add(Prefask_map.add_entry(bgp_buffer));
   routes.insert(PrefixRange(bgp_buffer));
   BEGIN(SKIP_REST);
}

[0-9]+(\.[0-9]+){3,3} { // /32
   strcpy(bgp_buffer, yytext);
   strcat(bgp_buffer, "/32");
// Modified by wlee@ISI.EDU
//   routes.add(Prefask_map.add_entry(bgp_buffer));
   routes.insert(PrefixRange(bgp_buffer));
   BEGIN(SKIP_REST);
}

<SKIP_REST,INITIAL>\n {
   BEGIN(INITIAL);
   count = 0;
}

. {
   count++;
   if (count > 5)
      BEGIN(SKIP_REST);
}

<SKIP_REST>.* {
   /* simply skip this line, it is not interesting to us */
}

%%

/* User Code if any */

