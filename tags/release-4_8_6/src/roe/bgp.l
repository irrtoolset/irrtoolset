/* 
// Copyright (c) 2001,2002                        RIPE NCC
//
// All Rights Reserved
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted,
// provided that the above copyright notice appear in all copies and that
// both that copyright notice and this permission notice appear in
// supporting documentation, and that the name of the author not be
// used in advertising or publicity pertaining to distribution of the
// software without specific, written prior permission.
//
// THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
// ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
// AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
// DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
// AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//  $Id$
//
//  Copyright (c) 1994 by the University of Southern California
//  All rights reserved.
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//    in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in
//    all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//    THE SOFTWARE.
//
//  Questions concerning this software should be directed to
//  irrtoolset@cs.usc.edu.
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

