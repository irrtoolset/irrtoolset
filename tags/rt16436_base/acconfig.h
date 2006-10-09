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
//  Author(s): Cengiz Alaettinoglu <cengiz@isi.edu>
*/

#ifndef CONFIG_H
#define CONFIG_H

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

/* Define if size_t is used in accept, instead of int */
#undef ACCEPT_USES_SIZE_T

/* Define if you have the Tk_CreateMainWindow function */
#undef in_addr_t

/* Define if malloc.h has memalign definition */
#undef HAVE_MEMALIGN_DEFINITION

/* Define if sys/time.h declares gettimeofday */
#undef HAVE_DECL_GETTIMEOFDAY
/* Define if string.h declares bzero */
#undef HAVE_DECL_BZERO
/* Define if string.h declares bcopy */
#undef HAVE_DECL_BCOPY
/* Define if unistd.h declares gethostname */
#undef HAVE_DECL_GETHOSTNAME
/* Define if netdb.h declares gethostbyname */
#undef HAVE_DECL_GETHOSTBYNAME
/* Define if sys/select.h declares select */
#undef HAVE_DECL_SELECT
/* Define if sys/socket.h declares socket */
#undef HAVE_DECL_SOCKET
/* Define if sys/socket.h declares connect */
#undef HAVE_DECL_CONNECT
/* Define if sys/socket.h declares bind */
#undef HAVE_DECL_BIND
/* Define if sys/socket.h declares listen */
#undef HAVE_DECL_LISTEN
/* Define if sys/socket.h declares accept */
#undef HAVE_DECL_ACCEPT
/* Define if sys/socket.h declares recvfrom */
#undef HAVE_DECL_RECVFROM
/* Define if sys/socket.h declares setsockopt */
#undef HAVE_DECL_SETSOCKOPT
/* Define if sys/socket.h declares send */
#undef HAVE_DECL_SEND
/* Define if sys/socket.h declares sendto */
#undef HAVE_DECL_SENDTO

@BOTTOM@

#ifdef __cplusplus
#include <typeinfo>
#endif /* __cplusplus */

#endif   /* CONFIG_H */
