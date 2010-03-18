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
 * Copyright (c) 1997
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the Computer Systems
 *	Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /proj/cvs/prod/IRRToolset/src/prtraceroute/ifaddrlist.c,v 4.9 2002/04/09 11:48:47 katie Exp $ (LBL)";
#endif

#include "config.h"

#include <sys/param.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif
#include <sys/time.h>				/* concession to AIX */

#if __STDC__
struct mbuf;
struct rtentry;
#endif

#include <net/if.h>
#include <netinet/in.h>

#include <ctype.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if 0
#include "gnuc.h"
#endif /*0*/

#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

#include "ifaddrlist.h"

#if 0
#include "savestr.h"
#endif 

/* Not all systems have IFF_LOOPBACK */
#ifdef IFF_LOOPBACK
#define ISLOOPBACK(p) ((p)->ifr_flags & IFF_LOOPBACK)
#else
#define ISLOOPBACK(p) (strcmp((p)->ifr_name, "lo0") == 0)
#endif

#define MAX_IPADDR 32

/*
 * Return the interface list
 */
int
ifaddrlist(register struct ifaddrlist **ipaddrp, register char *errbuf)
{
	register int fd, nipaddr;
#ifdef HAVE_SOCKADDR_SA_LEN
	register int n;
#endif
	register struct ifreq *ifrp, *ifend, *ifnext, *mp;
	register struct sockaddr_in *sin;
	register struct ifaddrlist *al;
	struct ifconf ifc;
	struct ifreq ibuf[MAX_IPADDR], ifr;
	char device[sizeof(ifr.ifr_name) + 1];
	static struct ifaddrlist ifaddrlist[MAX_IPADDR];

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		(void)sprintf(errbuf, "socket: %s", strerror(errno));
		return (-1);
	}
	ifc.ifc_len = sizeof(ibuf);
	ifc.ifc_buf = (caddr_t)ibuf;

	if (ioctl(fd, SIOCGIFCONF, (char *)&ifc) < 0 ||
	    ifc.ifc_len < sizeof(struct ifreq)) {
		(void)sprintf(errbuf, "SIOCGIFCONF: %s", strerror(errno));
		(void)close(fd);
		return (-1);
	}
	ifrp = ibuf;
	ifend = (struct ifreq *)((char *)ibuf + ifc.ifc_len);

	al = ifaddrlist;
	mp = NULL;
	nipaddr = 0;
	for (; ifrp < ifend; ifrp = ifnext) {
#ifdef HAVE_SOCKADDR_SA_LEN
		n = ifrp->ifr_addr.sa_len + sizeof(ifrp->ifr_name);
		if (n < sizeof(*ifrp))
			ifnext = ifrp + 1;
		else
			ifnext = (struct ifreq *)((char *)ifrp + n);
		if (ifrp->ifr_addr.sa_family != AF_INET)
			continue;
#else
		ifnext = ifrp + 1;
#endif
		/*
		 * Need a template to preserve address info that is
		 * used below to locate the next entry.  (Otherwise,
		 * SIOCGIFFLAGS stomps over it because the requests
		 * are returned in a union.)
		 */
		strncpy(ifr.ifr_name, ifrp->ifr_name, sizeof(ifr.ifr_name));
		if (ioctl(fd, SIOCGIFFLAGS, (char *)&ifr) < 0) {
			if (errno == ENXIO)
				continue;
			(void)sprintf(errbuf, "SIOCGIFFLAGS: %.*s: %s",
			    (int)sizeof(ifr.ifr_name), ifr.ifr_name,
			    strerror(errno));
			(void)close(fd);
			return (-1);
		}

		/* Must be up and not the loopback */
		if ((ifr.ifr_flags & IFF_UP) == 0 || ISLOOPBACK(&ifr))
			continue;

		(void)strncpy(device, ifr.ifr_name, sizeof(ifr.ifr_name));
		device[sizeof(device) - 1] = '\0';
		if (ioctl(fd, SIOCGIFADDR, (char *)&ifr) < 0) {
			(void)sprintf(errbuf, "SIOCGIFADDR: %s: %s",
			    device, strerror(errno));
			(void)close(fd);
			return (-1);
		}

		sin = (struct sockaddr_in *)&ifr.ifr_addr;
		al->addr = sin->sin_addr.s_addr;
		al->device = strdup(device);
		++al;
		++nipaddr;
	}
	(void)close(fd);

	*ipaddrp = ifaddrlist;
	return (nipaddr);
}
