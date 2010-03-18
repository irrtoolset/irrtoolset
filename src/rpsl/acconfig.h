/*  $Id$
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
//  Author(s): Cengiz Alaettinoglu <cengiz@isi.edu>
*/

#ifndef RPSL_CONFIG_H
#define RPSL_CONFIG_H

@TOP@

/* Define if you have the gnu readline and history libraries.  */
#undef HAVE_LIBREADLINE
#undef HAVE_LIBHISTORY
#undef HAVE_LIBTERMCAP

/* Define if you have the timestruc_t defined in sys/time.h.  */
#undef HAVE_TIMESTRUC_T

/* Define if you have the timeval defined in sys/time.h.  */
#undef HAVE_TIMEVAL

/* Define if your machine has BSD style udp headers, 
   you can tell this if it has field uh_ulen instead of len */
#undef HAVE_BSD_STYLE_UDPHDR

/* Define if accept in your system uses (socklen_t *) as the last parameter, 
   as opposed to (int *) */
#undef ACCEPT_USES_SOCKLEN_T

/* Define if rusage type uses timestruc_t to define ru_utime.  */
#undef RUSAGE_USES_TIMESTRUC_T

/* Define if rusage type uses timeval to define ru_utime.  */
#undef RUSAGE_USES_TIMEVAL

/* Define if a prototype "extern u_short htons(...)" is needed */
#undef DEFINE_HTONS_WITH_ELLIPSIS

/* Define if a prototype "extern u_short htons(unsigned short)" is needed */
#undef DEFINE_HTONS_WITH_USHORT

/* Define LD_SWITCH_X_SITE to contain any special flags your loader
   may need to deal with X Windows.  For instance, if you've defined
   HAVE_X_WINDOWS above and your X libraries aren't in a place that
   your loader can find on its own, you might want to add "-L/..." or
   something similar.  */
#undef LD_SWITCH_X_SITE

/* Define C_SWITCH_X_SITE to contain any special flags your compiler
   may need to deal with X Windows.  For instance, if you've defined
   HAVE_X_WINDOWS above and your X include files aren't in a place that
   your loader can find on its own, you might want to add "-I/..." or
   something similar.  */

#undef C_SWITCH_X_SITE

/* Define EXTRA_INCLUDE_DIRS to contain any special include directories
   to find certain header files. */
#undef EXTRA_INCLUDE_DIRS

/* Define EXTRA_LIBS to contain any special libraries that need to be
   linked. */
#undef EXTRA_LIBS

/* Define NEED_TO_DECLARE_SYS_ERRLIST if sys_errlist is not declared in 
   cerrno or stdio.h (defined in stdio.h in bsdi */ 
#undef NEED_TO_DECLARE_SYS_ERRLIST

/* Define STRUCT_IP_USES_VHL if struct ip contains bit fields ip_v and
   ip_hl or ip_vhl */
#undef STRUCT_IP_USES_VHL

/* Define if you have the Tk_CreateMainWindow function */
#undef HAVE_TK_CREATEMAINWINDOW

/* Define if malloc.h has memalign definition */
#undef HAVE_MEMALIGN_DEFINITION

/* Define if string.h has strcasecmp definition */
#undef HAVE_STRCASECMP_DEFINITION

@BOTTOM@

#ifdef __cplusplus
#include <typeinfo>
#endif /* __cplusplus */

#endif   /* RPSL_CONFIG_H */
