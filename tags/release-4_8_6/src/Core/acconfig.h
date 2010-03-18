/* $Id$
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


#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

/************************* SITE CUSTOMIZATION *********************/
/************************* END SITE CUSTOMIZATION ****************/

@TOP@

/* Define if you have the timestruc_t defined in sys/time.h.  */
#undef HAVE_TIMESTRUC_T

/* Define if you have the timeval defined in sys/time.h.  */
#undef HAVE_TIMEVAL

/* Define if rusage type uses timestruc_t to define ru_utime.  */
#undef RUSAGE_USES_TIMESTRUC_T

/* Define if rusage type uses timeval to define ru_utime.  */
#undef RUSAGE_USES_TIMEVAL

/* Define if a prototype "extern u_short htons(...)" is needed */
#undef DEFINE_HTONS_WITH_ELLIPSIS

/* Define if a prototype "extern u_short htons(unsigned short)" is needed */
#undef DEFINE_HTONS_WITH_USHORT

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

/* Define HAVE_SIGVEC if the kernel has the BSD signal interface */
#undef HAVE_BSD_SIGNALS

/* Define HAVE_SIGACTION if the kernel has the POSIX signal interface */
#undef HAVE_SIGACTION

/* Define HAVE_SIGSET if the kernel has the SYSV signal interface */
#undef HAVE_SIGSET

/* Define SETPGRP_VOID if the kernel has a POSIX compliant setpgrp() */
#undef SETPGRP_VOID

/* Define HOST_OS_IS_SOLARIS if we are configured for SunOS 5.x */
#undef HOST_OS_IS_SOLARIS

/* Define if config found pgp 5.0 installed in path */
#undef PGP_IS_INSTALLED

/* Define HOST_OS_IS_FREEBSD if we are configured for FreeBSD */
#undef HOST_OS_IS_FREEBSD

/* Define if we need to compile in tracing */
#undef HAVE_TRACING

/* Define if we need to compile in the network module */
#undef COMPILE_NETWORK

/* Define if we accept/recvfrom/... uses socklen_t instead of size_t as arg */
#undef ACCEPT_USES_SOCKLEN_T

/* Define if we accept/recvfrom/... uses socklen_t instead of size_t as arg */
#undef ACCEPT_USES_SIZE_T

/* Define if malloc.h has memalign definition */
#undef HAVE_MEMALIGN_DEFINITION

@BOTTOM@

#ifdef __cplusplus
#include <typeinfo>
#endif
#endif   /* CORE_CONFIG_H */
