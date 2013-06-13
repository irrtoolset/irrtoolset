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
//  Author(s): eddy@isi.edu
*/

#ifndef __NET_H__
#define __NET_H__

#include <config.h>

extern "C" {
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
}

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
#include <sys/types.h>

#if TIME_WITH_SYS_TIME
#   include <sys/time.h>
#   include <time.h>
#else
#   if HAVE_SYS_TIME_H
#      include <sys/time.h>
#   else
#      include <time.h>
#   endif
#endif
}

#include <iostream>
#include "Error.hh"

#ifndef MAXPACKETLEN
#define MAXPACKETLEN 4096
#endif  // MAXPACKETLEN

#ifndef INADDR_NONE
#define INADDR_NONE  -1
#endif  // INADDR_NONE 

#define ERRORMSGLEN  80

#ifndef TRUE
#define TRUE 1
#endif  // TRUE

#ifndef FALSE 
#define FALSE 0
#endif  // FALSE

#ifdef ACCEPT_USES_SOCKLEN_T
#define SOCKLEN_T socklen_t
#elif ACCEPT_USES_SIZE_T
#define SOCKLEN_T size_t
#else
#define SOCKLEN_T int 
#endif

////////////////////////////////////////////////////////////////////////
class Timer {
    struct timeval tv;
    struct timezone tz;
    void init_time () {
	bzero ((char *) &tv, sizeof (struct timeval));
	::gettimeofday (&tv, &tz);
    }

  public:
    Timer () { init_time (); }
    Timer (int seconds) {		
	tv.tv_sec = seconds;
	tv.tv_usec = 0;
    }
    Timer  (struct timeval &t) {
	init_time();
	tv.tv_sec = t.tv_sec;
	tv.tv_usec = t.tv_usec;
    }

    struct timeval *gettimeval () { return &tv; }
    void gettimeofday () { ::gettimeofday(&tv, &tz); }

    double delta (Timer &tt) {
	return ((double) (tv.tv_sec - tt.tv.tv_sec) * 1000.0 +
		(double) (tv.tv_usec - tt.tv.tv_usec) / 1000);
    }

    Error error;
};

////////////////////////////////////////////////////////////////////////
//
// the ipAddr class handles four forms of the IP address.
//
// 1. DNS names: i.e. kit.isi.edu
// 2. IP quad  : array of four u_shorts (0 <= i <= 255), i.e. 128.9.160.69
// 3. ip dotted addr: the char string: "128.9.160.69"
// 4. hex form of the ip addr: i.e. 0x8009a028, used by the sockets and packets
//
// XXX - This class needs to whittled down to the hostent struct and
// a tmp var or two.. plus the conversion routines, i'm duplicating too
// much info.?.
//
////////////////////////////////////////////////////////////////////////
class ipAddr {
    char hostname[MAXHOSTNAMELEN]; // should use hp->h_name???
    char dottedaddr[16];	// x.x.x.x notation
    char maskedaddr[19];	// x.x.x.x/len notation
    char asname[8];
    struct hostent *hp;		// official host info
    u_short mask;		// ignore for now; needed...
    in_addr_t hexaddr;		// is in hp->h_addr

    struct hostent *_getHostent (char *addr = (char *) NULL);
    void init_ipAddr();

  public:
    ipAddr ()		{ init_ipAddr (); }
    ipAddr (const char *name)	{ init_ipAddr (); setAddress (name); }
    ipAddr (u_long hex) { init_ipAddr (); setAddress (hex); }

    class ipAddrException { };
    int setAddress (const char *addr);
    int setAddress (u_long hexaddr);

   int setToMyHost() {
      // get our localhostname
      char myname[MAXHOSTNAMELEN];
      if (gethostname ((char *) &myname, MAXHOSTNAMELEN)) {
	 perror ("Cannot determine our hostname");
	 return 0;
      }
      return setAddress(myname);
   }

    char *getName () {
	if (hostname[0] != '\0')
	    return (char *) &hostname;
	else
	    return getIpaddr ();
    }

    char *getIpaddr () {
       if (dottedaddr[0] == '\0' && hexaddr) {
	  strcpy(dottedaddr, inet_ntoa(*(struct in_addr *) &hexaddr));
       }
       if (dottedaddr[0] != '\0') { 
	  return ((char *) &dottedaddr);
       }
       return (char *) NULL;
    }

    char *getIpaddrMask () {
	if (dottedaddr[0] == '\0')
	    getIpaddr();
	
	if (dottedaddr[0] != '\0') {
	    sprintf ((char *) &maskedaddr, "%s/%d", dottedaddr, mask);
	}

	if (maskedaddr[0] != '\0')
	    return ((char *) &maskedaddr);
	return (char *) NULL;
    }

   
    void setAsname (char *name) { if (name) strcpy (asname, name); }

    char *getAsname () {
	static char *ptr;
	ptr = (char *)"(null)";
	if (asname[0] != '\0') ptr = (char *) asname;
	return ptr;
    }
    u_long getInaddr () { return hexaddr; }
    void print () { printf ("%s\n%s\n0x%x\n", hostname, dottedaddr, hexaddr); }

    Error error;
};



////////////////////////////////////////////////////////////////////////
class Socket {
    u_int family;
    u_int type;
    u_int proto;
    int send_flags;
    FILE *in, *out;

  protected:
    int sock;  // Moved from private by wlee@isi.edu
    struct sockaddr_in socksrc;	// src == local
    struct sockaddr_in sockdst;	// dst == remote
    struct sockaddr_in socknew;	// for accept()

  public:
    Socket (u_int family, u_int type, u_int proto);
    Socket (int newsock) {
	bzero ((char *) &socksrc, sizeof (struct sockaddr_in));
	bzero ((char *) &sockdst, sizeof (struct sockaddr_in));
	family = 0;
	type = 0;
	proto = 0;
	send_flags = 0;
	in = out = NULL;
	sock = newsock;
    }

    void set_srcInaddr (u_int addr) { socksrc.sin_addr.s_addr = addr; }
    void set_dstInaddr (u_int addr) { sockdst.sin_addr.s_addr = addr; }
    u_long get_srcInaddr () { return socksrc.sin_addr.s_addr; }
    u_long get_dstInaddr () { return sockdst.sin_addr.s_addr; } 

    void setPort   (u_int port)    { sockdst.sin_port = htons(port); }
    int setsockopt (int, int, char *, int);
    void setSendflags (int flags) { send_flags = flags; }
    int connect() {
	// XXX: exceptions
	error = ::connect (sock, (struct sockaddr *) &sockdst, sizeof (sockdst));
	if (error())
	    error.warning("connect");
	return error();
    }
    int connect(struct sockaddr *d, int l) {
	error = ::connect (sock, (struct sockaddr *) d, l);
	if (error())
	    error.warning("connect");
	return error();
    }
    int bind () {
	// XXX: sanity check!!
	error = ::bind (sock, (struct sockaddr *) &socksrc, sizeof (socksrc));
	if (error())
	    error.warning("bind");
	return error();
    }
    int bind (struct sockaddr *s, int l) {
	error = ::bind (sock, (struct sockaddr *) s, l);
	if (error())
	    error.warning("bind");
	return error();
    }
    int listen (int backlog = 5) {
        error = ::listen (sock, backlog);
	if (error())
	    error.warning("listen");
	return error();
    }
    int accept () {
	int addrlen = sizeof (struct sockaddr);
	bzero ((char *) &socknew, sizeof (socknew));
	int newsock = ::accept (sock, (struct sockaddr *) &socknew,
				(SOCKLEN_T *) &addrlen);
	if (newsock < 0)
	    error.fatal ("accept");
	return newsock;
    }
    int close () {
	error = ::close(sock);
	sock = 0;
	if (error())
	    error.fatal("close socket");
	return error();
    }
    
    int recvfrom (void *packet, int size, int flags = 0) {
	int fromlen = size;
	int c = ::recvfrom (sock, (char *) packet, size, flags,
			    (struct sockaddr *) &socksrc, 
			    (SOCKLEN_T *) &fromlen);
	if (c < 0)
	    error.fatal("recvfrom");
	return c;
    }
    int sendto (char *buf, int len, int flags = 0) {
	if (flags == 0 && send_flags != 0)
	    flags = send_flags;
	int c = ::sendto (sock, (char *) buf, len, flags,
			(struct sockaddr *) &sockdst, sizeof (sockdst));
	if (c < 0)
	    error.fatal("sendto");
	return c;
    }
    int send (char *buf, int len, int flags = 0) { // assumes a bind
	if (flags == 0 && send_flags != 0)
	    flags = send_flags;
	int c = ::send (sock, (char *) buf, len, send_flags);
	if (c < 0)
	    error.fatal("send");
	return c;
    }
    
    int write (char *buf, int len);
    int read  (char *buf, int len);
#ifdef RIPE
    int wait_for_reply (char *buffer, int size, int timeout);
#endif /* RIPE */

    struct sockaddr_in *get_socknew () {
	return (struct sockaddr_in *) &socknew;
    }

    struct sockaddr_in *get_sockdst () {
	return (struct sockaddr_in *) &sockdst;
    }

    void set_sockdst (struct sockaddr_in *saddr) {
	bcopy ((char *) saddr, (char *) &sockdst,
	       sizeof (struct sockaddr_in));
    }

    // FILE descriptor style io support.
    int getsock() { return sock; }
    
    int fdopen() {
	in  = ::fdopen (sock, "r");
	out = ::fdopen (sock, "w");
	if (in == NULL)
	    error.fatal ("fdopen: Could not create input descriptior");
	if (out == NULL)
	    error.fatal ("fdopen: Could not create output descriptior");
	if (errno)
	    error.fatal ("fdopen");
	return error();
    }

    int fgets (char *p, int s) {
	// XXX need to check for filedesc first, create one if
	// it doesn't exist.
	char *ptr = ::fgets (p, s, in);
	if (!ptr) {
	    error.fatal ("getline");
	}
	if (errno)
	    error.fatal ("fgets");
	return error();
    }

    int fread (char *ptr, int count) {
	int c = ::fread (ptr, sizeof (char), count, in);
	if (c < count) {
	    if (feof (in)) 
		error.fatal ("End of File");
	    if (ferror (in))
		error.fatal ("Reading input");
	    if (errno)
		error.fatal ("fread");
	}
        return c;
    }

    int fwrite (char *ptr, int count) {
	int c = ::fwrite (ptr, sizeof (char), count, out);
	if (c < count) {
	    error.fatal ("fwrite: writing output");
	}
	fflush ();
	if (errno)
	    error.fatal ("fwrite: ");
	return c;
    }

    int fprintf (const char *fmt, ...) {
	va_list ap;
	va_start (ap, fmt);

	int ret = vfprintf (out, fmt, ap);

	::fflush(out);
	va_end (ap);

	return ret;
    }

    int fflush () {
	if (out) 
	    error = ::fflush (out);
	if (error())
	    error.fatal ("fwrite: ");
	return (error());
    }

    int PendingData() {
	int status;
	int answer;

	if ((status = fcntl(fileno(in), F_GETFL, 0)) < 0)
	    return TRUE;
	if (errno)
	    error.fatal("Pending: F_GETFL ");
	
	if (fcntl(fileno(in), F_SETFL, O_NDELAY) < 0)
	    return TRUE;

	int c = fgetc(in);
	if (c == EOF) {
	    clearerr(in);
	    answer = FALSE;
	} else {
	    ungetc(c, in);
	    answer = TRUE;
	}

	fcntl(fileno(in), F_SETFL, status);
	if (errno) {
	    if (errno == EWOULDBLOCK)
		errno = 0;
	    else {
		if (errno)
		    error.fatal("Socket::PendingData() ");
	    }
	} 
	return answer;
    }

  
    bool readReady(void) {
      int status;
      bool answer = false;
      int fd = sock;

      fd_set fd_read;
      FD_ZERO(&fd_read);
      FD_SET(fd, &fd_read);
      struct timeval timeout = { 0L, 0L };
      status = select(fd + 1, &fd_read, NULL, NULL, &timeout);
      //      status = select(fd + 1, &fd_read, NULL, NULL, &timeout);
      // 0: timeout;  -1: error
      if (status <= 0) answer = false;
      else
	if (FD_ISSET(fd, &fd_read)) answer = true;
	else
	  answer = false;
      return answer;
    }

    bool writeReady(void) {
      int status;
      bool answer = false;
      int fd = sock;
      fd_set fd_write;
      FD_ZERO(&fd_write);
      FD_SET(fd, &fd_write);
      struct timeval timeout = { 0L, 0L };
      status = select(fd + 1, NULL, &fd_write, NULL, &timeout);
      // 0: timeout;  -1: error
      if (status <= 0) answer = false;
      else
	if (FD_ISSET(fd, &fd_write)) answer = true;
	else
	  answer = false;
      return answer;
    }

    // Added by wlee@isi.edu to support reestablish a closed connection
    void newSock(void) {
      if (sock > 0) close();
      sock = socket (family, type, proto);
      if (sock < 0) 
	error.fatal ("socket");
    }

    int set_debug () {
	int on = 1;
	error = ::setsockopt (sock, SOL_SOCKET, SO_DEBUG,
				(char *) &on, sizeof (int));
	if (error())
	    error.fatal ("setsockopt");
	return error();
    }
    Error error;
};

#endif  // __NET_H__
