/* $Id$
// 
//  Copyright (c) 1994 by the University of Southern California
//  All rights reserved.
//
//  Permission to use, copy, modify, and distribute this software and
//  its documentation in source and binary forms for lawful
//  non-commercial purposes and without fee is hereby granted, provided
//  that the above copyright notice appear in all copies and that both
//  the copyright notice and this permission notice appear in supporting
//  documentation, and that any documentation, advertising materials,
//  and other materials related to such distribution and use acknowledge
//  that the software was developed by the University of Southern
//  California, Information Sciences Institute and/or the International
//  Business Machines Corporation.  The name of the USC may not
//  be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  NEITHER THE UNIVERSITY OF SOUTHERN CALIFORNIA NOR INTERNATIONAL
//  BUSINESS MACHINES CORPORATION MAKES ANY REPRESENTATIONS ABOUT
//  THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  THIS SOFTWARE IS
//  PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
//  INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, TITLE, AND 
//  NON-INFRINGEMENT.
//
//  IN NO EVENT SHALL USC, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT,
//  TORT, OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH,
//  THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//  Questions concerning this software should be directed to 
//  info-ra@isi.edu.
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
