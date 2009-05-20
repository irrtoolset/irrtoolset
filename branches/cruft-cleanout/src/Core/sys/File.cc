//
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
// $Id$
//
// system.cc
// Author: Ramesh Govindan <govindan@isi.edu>
//
// Abstracted OS facilities for file system access and
// for communication primitives. This file contains implementations of:
//	- network addresses (Address class)
//	- OS file descriptors and descriptor sets
//	- a common time representation
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cerrno>

extern "C" {
#if HAVE_UNISTD_H
#include <unistd.h>
#endif // HAVE_UNISTD_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/file.h>    
#include <sys/resource.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#ifdef HOST_OS_IS_SOLARIS
#include <sys/systeminfo.h>
#include <netdb.h>
#endif    
}

#include "util/Types.hh"
#include "util/List.hh"
#include "util/Handler.hh"
#include "util/Trail.hh"

#include "sys/Address.hh"
#include "sys/File.hh"
#include "sys/Signal.hh"
#include "sched/Dispatcher.hh"

// Added by wlee to port to FreeBSD and BSDI
#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK         (u_long)0x7F000001
#endif

extern "C" {
#ifndef STDC_HEADERS
extern int socket(...);
extern int bind(...);
extern int connect(...);
extern int read(...);
extern int write(...);
extern int setsockopt(...);
extern int getsockname(...);
extern int close(...);
extern int recvfrom(...);
extern int sendto(...);
extern int listen(...);
extern int accept(...);
extern off_t lseek(...);
extern int flock(...);
extern int ftruncate(...);
extern int stat(...);
#endif
}

// For printing out system error messages: from GNU textutils lib
#if HAVE_STRERROR
#ifndef strerror
extern "C" char *strerror(int);
#endif
#else
extern int sys_nerr;
extern char* sys_errlist[];

static char*
private_strerror(int errnum)
{
    if (errnum > 0 && errnum <= sys_nerr)
        return sys_errlist[errnum];
    return "Unknown system error";
}
#define strerror private_strerror
#endif // HAVE_STRERROR

#ifdef ACCEPT_USES_SOCKLEN_T
#define SOCKLEN_T socklen_t
#elif ACCEPT_USES_SIZE_T
#define SOCKLEN_T size_t
#else
#define SOCKLEN_T int 
#endif

// File local variables
static TraceCode 	traceFile("file");
static Handler 		nullHandler(NULL, NULL);
static int		pidDescriptor = -1;

File::File(int fd,
           FileMode m,
           const Handler& rh,
           const Handler& wh)
        : ListNode()
{
    descriptor_ = fd;
    mode_ = m;

    if (descriptor_ < 0) {
        FATAL("couldn't open file descriptor: %s\n",
              strerror(errno));
        // NotReached
    }
    
    readHandler = rh;
    writeHandler = wh;
    dispatcher.files.inset(this);
}

File::~File()
{
    TRACE(traceFile,
          "closing file %d\n",
          descriptor_);
    close(descriptor_);
    dispatcher.files.outset(this);
}

void
File::setHandlers(Handler& rh,
                  Handler& wh)
{
    dispatcher.files.outset(this);
    readHandler = rh;
    writeHandler = wh;
    dispatcher.files.inset(this);
}

int
File::read(char *buffer,
           int  bufferLength)
{
    int retval;
    
    if (mode_ != FileModeReadOnly
        && mode_ != FileModeReadWrite) {
        ERROR("illegal attempt to read from descriptor\n");
        return FileOpHardError;
    }
    
    TRACE(traceFile,
          "read file %d length %d\n",
          descriptor_,
          bufferLength);
    
    retval = ::read(descriptor_,
                    buffer,
                    bufferLength);
    if (retval < 0) {
        switch (errno) {
            case EWOULDBLOCK:
#if defined(EAGAIN) && EAGAIN != EWOULDBLOCK
            case EAGAIN:
#endif
            case EINTR:
                return FileOpSoftError;
            default:
                return FileOpHardError;
        }
    }
    return retval;
}

int
File::write(char *buffer,
            int  bufferLength)
{
    int retval;
    
    if (mode_ != FileModeReadWrite) {
        ERROR("attempt to write on read-only file descriptor\n");
        return FileOpHardError;
    }
    
    TRACE(traceFile,
          "write file %d length %d\n",
          descriptor_,
          bufferLength);
    
    retval = ::write(descriptor_,
                     buffer,
                     bufferLength);
    if (retval < 0) {
        switch (errno) {
            case EWOULDBLOCK:
#if defined(EAGAIN) && EAGAIN != EWOULDBLOCK
            case EAGAIN:
#endif
            case EINTR:
                return FileOpSoftError;
            default:
                return FileOpHardError;
        }
    }
    return retval;
}

Socket::Socket(int ds,FileMode fm, const Handler &rh, const Handler &wh) :
	File(ds, fm, rh, wh) {
}
int Socket::setopt(int level, int optname, const void *optval, size_t optlen) {
	return ::setsockopt(descriptor_, level, optname, (char *) optval, (SOCKLEN_T) optlen);
}

StreamSocket::StreamSocket(Address& addr,
                           Port port,
                           Handler& rh,
                           Handler& wh)
	: Socket(::socket(AF_INET, SOCK_STREAM, 0),
               FileModeReadWrite, 
               rh, 
               wh)
{
    struct sockaddr_in sin;
    int retval;

    // Only stream sockets are non-blocking
    if (fcntl(descriptor_,
              F_SETFL,
              O_NDELAY) < 0) {
        FATAL("non-blocking descriptor error: %s\n",
              strerror(errno));
        // Not Reached
    }

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(addr.get());
    sin.sin_port = htons(port);
    
    retval = ::connect(descriptor_,
                       (struct sockaddr *) &sin,
                       sizeof(sin));
    //    ASSERT((retval < 0) && (errno == EWOULDBLOCK));	// XXX
}

StreamSocket::StreamSocket(int d,
                           Handler& rh,
                           Handler& wh)
	: Socket(d, FileModeReadWrite, rh, wh)
{
    // Only stream sockets are non-blocking
    if (fcntl(descriptor_,
              F_SETFL,
              O_NDELAY) < 0) {
        FATAL("non-blocking descriptor error: %s\n",
              strerror(errno));
        // Not Reached
    }
}

StreamSocket::~StreamSocket()
{
    // Empty
}

ListenSocket::ListenSocket(Port port,
                           Handler& lh,
                           Boolean local)
	: Socket(::socket(AF_INET, SOCK_STREAM, 0),
               FileModeReadWrite,
               lh,
               nullHandler)
{
    int 	retval;
    int 	reuse;
    sockaddr_in	sin;
    uid_t savedEUID = geteuid(); 

    if (port <= 1024 && savedEUID != 0) // we need to be root to do this
       seteuid(0); 

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    if (local) {
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    } else {
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    
    TRACE(traceFile,
          "binding socket %d to port %d\n",
          descriptor_,
          port);

    reuse = 1;
    if (setopt(SOL_SOCKET,
	       SO_REUSEADDR,
	       (char*) &reuse,
	       sizeof(reuse)) < 0) {
	    FATAL("failed to set %d to be reusable: %s\n",
		  descriptor_,
		  strerror(errno));
        // NotReached
    }

    if (::bind(descriptor_,
               (struct sockaddr *) &sin,
               sizeof(sin)) < 0) {
        FATAL("failed bind on %d: %s\n",
              descriptor_,
              strerror(errno));
        // NotReached
    }

    TRACE(traceFile,
          "setting file %d to listen\n",
          descriptor_);

    retval = ::listen(descriptor_, 5);
    if (retval < 0) {
        FATAL("listen failed on file %d\n",
              descriptor_);
        // NotReached
    }

    if (port <= 1024 && savedEUID != 0) // we need to be root to do this
       seteuid(savedEUID); 

    return;
}

ListenSocket::~ListenSocket()
{
    // Empty
}

int
ListenSocket::accept(StreamSocket** sock,
                     Address& remote,
                     Port *remotePort,
                     Handler& read,
                     Handler& write)
{
    int retval;
    struct sockaddr_in sin;
    size_t socklen;

    TRACE(traceFile,
          "accepting incoming connection on file %d\n",
          descriptor_);

    socklen = sizeof(sin);
    retval = ::accept(descriptor_,
                      (struct sockaddr *) &sin,
                      (SOCKLEN_T*)&socklen);
    if (retval < 0) {
        switch (errno) {
            case EWOULDBLOCK:
#if defined(EAGAIN) && EAGAIN != EWOULDBLOCK
            case EAGAIN:
#endif
            case EINTR:
                return FileOpSoftError;
            default:
                return FileOpHardError;
        }
    }

    remote.set(ntohl(sin.sin_addr.s_addr));
    *remotePort = ntohs(sin.sin_port);

    *sock = new StreamSocket(retval, read, write);
    return FileOpOK;
}

void
createDir(char* name)
{
    int	retval;
    struct stat statbuf;

    retval = stat(name, &statbuf);
    if (retval < 0) {	// Name doesn't exist
        retval = mkdir(name, 0755);
        if (retval < 0) {
            FATAL("couldn't create directory %s: %s\n", name,
                  strerror(errno));
            // NotReached
        }
        return;
    }
    if (!S_ISDIR(statbuf.st_mode)) {
        FATAL("expected %s to be a directory, it isn't\n",
              name);
        // NotReached
    }
}

static char*	pidFileName = NULL;

void
openPidFile(const char* name)
{
    int		retval;
    char	buf[20];
    int		pid;
    int		len;

    pidFileName = new char[strlen(name) + 1];
    memcpy(pidFileName, name, strlen(name) + 1);

    pidDescriptor = open(name, O_RDWR | O_CREAT | 
#ifdef HOST_OS_IS_FREEBSD
			 O_FSYNC
#else
			 O_SYNC
#endif 
			 , 0664);
    if (pidDescriptor < 0) {
        ERROR("couldn't open pid file %s: %s\n", name, strerror(errno));
        exit(2);
    }

#ifdef HAVE_LOCKF
    retval = ::lockf(pidDescriptor, F_LOCK, 0);
#elif HAVE_FLOCK
    retval = ::flock(pidDescriptor, LOCK_EX|LOCK_NB);
#else
    retval = 0;
#endif

    if (retval < 0) {
        switch (errno) {
            case EWOULDBLOCK:
#if defined(EAGAIN) && EAGAIN != EWOULDBLOCK
            case EAGAIN:
#endif
                len = read(pidDescriptor, buf, sizeof buf);
                if (len > 0 &&  (pid = atoi(buf))) {
                    ERROR("another already running, pid %d\n",
                          pid);
                } else {
                    ERROR("is some other %s running?");
                }
                break;

            default:
                ERROR("flock failed: %s\n", strerror(errno));
        }
        close(pidDescriptor);
        pidDescriptor = -1;
        exit(2);
    }

    (void) sprintf(buf, "%d\n", getpid());
    len = strlen(buf);

#ifndef	SEEK_SET
#define	SEEK_SET	L_SET
#endif	/* SEEK_SET */
    /* Back up to the beginning and truncate the file */
    if (lseek(pidDescriptor, (off_t) 0, SEEK_SET) < 0
        || ftruncate(pidDescriptor, (off_t) 0) < 0
        || write(pidDescriptor, buf, len) != len) {
        ERROR("couldn't write to %s: %s\n", name, strerror(errno));
        exit(2);
    }

    return;
}

void
closePidFile()
{
    if (!pidFileName) {
        return;
    }

    if (pidDescriptor > -1) {
        if (close(pidDescriptor) == -1
            || unlink(pidFileName) == -1) {
            ERROR("could not close or remove %s: %s\n",
                  pidFileName, strerror(errno));
	    delete [] pidFileName;
            exit(2);
        }
        pidDescriptor = -1;
    }

    delete [] pidFileName;
    return;
}

volatile void daemonize()
{
    int		t;
    Signal*	sig;

    // Fork once
    switch (fork()) {
        case 0:
            break;
        case -1:
            perror("daemonize: fork");
            exit(1);
        default:
            exit(0);
    }

    int fd = open("/dev/null", O_RDWR, 0);
    if (fd == -1) {
       perror("daemonize: fork");
       exit(1);
    }

    if (setsid() == -1) {
       perror("daemonize: fork");
       exit(1);
    }

    if (isatty(STDIN_FILENO))
       dup2(fd, STDIN_FILENO);
    if (isatty(STDOUT_FILENO))
       dup2(fd, STDOUT_FILENO);
    if (isatty(STDERR_FILENO))
       dup2(fd, STDERR_FILENO);
    if (fd > 2) close(fd);
	    
    /* Reset umask */
    umask(022);
}

//
//  Copyright (c) 1994 by the University of Southern California.
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
//  California and/or Information Sciences Institute.
//  The name of the University of Southern California may not
//  be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THE UNIVERSITY OF SOUTHERN CALIFORNIA DOES NOT MAKE ANY REPRESENTATIONS
//  ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  THIS SOFTWARE IS
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
