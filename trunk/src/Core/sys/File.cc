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
#include "network/Headers.hh"

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

DatagramSocket::DatagramSocket(const Handler& rh,
                               const Handler& wh)
	: Socket(::socket(AF_INET, SOCK_DGRAM, 0),
               FileModeReadWrite,
               rh,
               wh)
{
    // Empty
}

DatagramSocket::~DatagramSocket()
{
    // Empty
}

int
DatagramSocket::recvFrom(char* buffer,
                         int bufferLength,
                         Address& addr,
                         Port *port)
{
    struct sockaddr_in sin;
    int retval;
    size_t socklen = sizeof(sin);
    
    retval = ::recvfrom(descriptor_,
                        buffer,
                        bufferLength,
                        0,
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

    addr.set((U32) ntohl(sin.sin_addr.s_addr));
    *port = ntohs(sin.sin_port);
    TRACE(traceFile,
          "read %d bytes on socket %d from %s\n",
          retval,
          descriptor_,
          addr.name());
    return retval;
}

int
DatagramSocket::sendTo(char* buffer,
                       int bufferLength,
                       Address& addr,
                       Port port)
{
    struct sockaddr_in sin;
    int retval;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(addr.get());
    sin.sin_port = htons(port);
    retval = ::sendto(descriptor_,
                      buffer,
                      bufferLength,
                      0,
                      (struct sockaddr *) &sin,
                      sizeof(sin));
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

    TRACE(traceFile,
          "wrote %d bytes on socket %d from %s\n",
          retval,
          descriptor_,
          addr.name());
    
    return retval;
}

int
DatagramSocket::defaultInterface(Address& localAddress, const Address &dst) {
#define ALLOW_MULTIPLE_IFACES
#ifndef ALLOW_MULTIPLE_IFACES
	static bool hashed= false;
	static Address myOnlyIface;
	if (hashed) {
		localAddress.set(myOnlyIface.get());
		return FileOpOK;
	}
#endif
    int sock;
    struct sockaddr_in sin;
    size_t socklen = sizeof(sin);

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(dst.get());
    sin.sin_port = htons(2000);		// We can then use write
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        ERROR("couldn't open socket for getting local address: %s\n",
              strerror(errno));
        return FileOpHardError;
    }

    if (connect(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        ERROR("couldn't connect for getting local address: %s\n",
              strerror(errno));
        return FileOpHardError;
    }

    if (getsockname(sock, 
                    (struct sockaddr *) &sin, 
		    (SOCKLEN_T*)&socklen) < 0) {
        ERROR("couldn't get local address: %s\n",
              strerror(errno));
        return FileOpHardError;
    }
    
#ifdef HOST_OS_IS_SOLARIS
    if (sin.sin_addr.s_addr == 0) {
	char myhostname[256];
	struct hostent *hp;
	int error;
    
	error = sysinfo(SI_HOSTNAME, myhostname, sizeof(myhostname));
	if (error == -1) {
            ERROR("failed on sysinfo: %s\n",
                  strerror(errno));
	    exit(-1);
	}

	hp = gethostbyname(myhostname);
	if (hp == NULL || hp->h_addrtype != AF_INET ||
	    hp->h_length != sizeof(sin.sin_addr)) {
            ERROR("failed on gethostbyname: %s\n",
                  strerror(errno));
	    exit(-1);
	}
	memcpy((char *)&sin.sin_addr.s_addr, hp->h_addr, hp->h_length);
    }
#endif

    localAddress.set(ntohl(sin.sin_addr.s_addr));
    close(sock);
    TRACE(traceFile,
          "default interface for dst %s is %s\n",
          dst.name(),
          localAddress.name());
#ifndef ALLOW_MULTIPLE_IFACES
    myOnlyIface= localAddress.get();
    hashed= true;
#endif
    return FileOpOK;
}	

RawSocket::RawSocket(Handler& rh,
                     Handler& wh,
                     RawSocketType type)
	: Socket(::socket(AF_INET, SOCK_RAW, type),
               FileModeReadWrite,
               rh,
               wh)
{
    int bufSize = 48*1024;
    int bval = 1;

#ifdef	IP_HDRINCL
    if(setopt(IPPROTO_IP, IP_HDRINCL,
	      (char *)&bval, sizeof(bval)) < 0) {
	    FATAL("setsockopt IP_HDRINCL %u", bval);
	    // Not Reached
    }
#endif
    if(setopt(SOL_SOCKET, SO_RCVBUF, (char *)&bufSize,
	      sizeof(bufSize)) < 0) {
	    FATAL("setsockopt SO_RCVBUF %u", bufSize);
	    // Not Reached
    }
}

RawSocket::~RawSocket()
{
    // Empty
}

int
RawSocket::recvFrom(char* buffer,
                    int bufferLength,
                    Address& addr,
                    Port *port)
{
    struct sockaddr_in sin;
    int retval;
    size_t socklen = sizeof(sin);
    
    retval = ::recvfrom(descriptor_,
                        buffer,
                        bufferLength,
                        0,
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

    addr.set((U32) ntohl(sin.sin_addr.s_addr));
    *port = ntohs(sin.sin_port);
    TRACE(traceFile,
          "read %d bytes on raw socket %d from %s\n",
          retval,
          descriptor_,
          addr.name());
#ifdef HOST_OS_IS_FREEBSD
    IP* iph= (IP*) buffer;
    iph->totalLength+= sizeof(IP);
#endif
    return retval;
}

int
RawSocket::sendTo(char* buffer,
                  int bufferLength,
                  Address& addr,
                  Port port) const
{
    struct sockaddr_in sin;
    int retval;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(addr.get());
    sin.sin_port = htons(port);
    retval = ::sendto(descriptor_,
                      buffer,
                      bufferLength,
                      0,
                      (struct sockaddr *) &sin,
                      sizeof(sin));
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

    TRACE(traceFile,
          "wrote %d bytes on socket %d from %s\n",
          retval,
          descriptor_,
          addr.name());
    
    return retval;
}

int
RawSocket::join(const Address& group)
{
    struct ip_mreq mreq;

    mreq.imr_multiaddr.s_addr = htonl(group.get());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    TRACE(traceFile,
          "joining group %s on %d\n",
          group.name(),
          descriptor_);

    if (setopt(IPPROTO_IP,
	       IP_ADD_MEMBERSHIP,
	       (char *) &mreq,
	       sizeof(mreq)) < 0) {
	    FATAL("failed group join on %d: %s\n",
		  descriptor_,
		  strerror(errno));
	    // NotReached
    }

    return FileOpOK;
}

MulticastSocket::MulticastSocket(const Address& addr,
                                 Port localPort,
                                 const Handler& rh,
                                 const Handler& wh)
	: DatagramSocket(rh, wh)
{
    struct ip_mreq mreq;
    struct sockaddr_in sin;
    char loop;

    group = addr;
    port = localPort;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(localPort);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    
    TRACE(traceFile,
          "binding socket %d to port %d\n",
          descriptor_,
          localPort);

    if (::bind(descriptor_,
               (struct sockaddr *) &sin,
               sizeof(sin)) < 0) {
        FATAL("failed bind on %d: %s\n",
              descriptor_,
              strerror(errno));
        // NotReached
    }

    mreq.imr_multiaddr.s_addr = htonl(group.get());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    TRACE(traceFile,
          "joining group %s on %d\n",
          group.name(),
          descriptor_);

    if (setopt(IPPROTO_IP,
	       IP_ADD_MEMBERSHIP,
	       (char *) &mreq,
	       sizeof(mreq)) < 0) {
	    FATAL("failed group join on %d: %s\n",
		  descriptor_,
		  strerror(errno));
	    // NotReached
    }

    // Disable loopback of packets
    loop = 0;
    if (setopt(IPPROTO_IP,
	       IP_MULTICAST_LOOP,
	       &loop,
	       sizeof(loop)) < 0) {
	    FATAL("unable to disable multicast loopback on %d: %s\n",
		  descriptor_,
		  strerror(errno));
	    // NotReached
    }
}

MulticastSocket::~MulticastSocket()
{
    struct ip_mreq mreq;

    mreq.imr_multiaddr.s_addr = htonl(group.get());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    
    TRACE(traceFile,
          "deleting multicast socket for group %s on %d\n",
          group.name(),
          descriptor_);
    
    setopt(IPPROTO_IP,
	   IP_DROP_MEMBERSHIP,
	   (char *) &mreq,
	   sizeof(mreq));
    return;
}

int
MulticastSocket::defaultInterface(Address& localAddress)
{
    int sock;
    struct sockaddr_in sin;
    size_t socklen = sizeof(sin);

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(group.get());
    sin.sin_port = htons(port);		// We can then use write
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        ERROR("couldn't open socket for getting local address: %s\n",
              strerror(errno));
        return FileOpHardError;
    }

    if (connect(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        ERROR("couldn't connect for getting local address: %s\n",
              strerror(errno));
        return FileOpHardError;
    }

    if (getsockname(sock, 
                    (struct sockaddr *) &sin, 
		    (SOCKLEN_T*)&socklen) < 0) {
        ERROR("couldn't get local address: %s\n",
              strerror(errno));
        return FileOpHardError;
    }
    
#ifdef HOST_OS_IS_SOLARIS
    if (sin.sin_addr.s_addr == 0) {
	char myhostname[256];
	struct hostent *hp;
	int error;
    
	error = sysinfo(SI_HOSTNAME, myhostname, sizeof(myhostname));
	if (error == -1) {
            ERROR("failed on sysinfo: %s\n",
                  strerror(errno));
	    exit(-1);
	}

	hp = gethostbyname(myhostname);
	if (hp == NULL || hp->h_addrtype != AF_INET ||
	    hp->h_length != sizeof(sin.sin_addr)) {
            ERROR("failed on gethostbyname: %s\n",
                  strerror(errno));
	    exit(-1);
	}
	memcpy((char *)&sin.sin_addr.s_addr, hp->h_addr, hp->h_length);
    }
#endif

    localAddress.set(ntohl(sin.sin_addr.s_addr));
    close(sock);
    TRACE(traceFile,
          "default interface for group %s is %s\n",
          group.name(),
          localAddress.name());
    
    return FileOpOK;
}

int
MulticastSocket::setTTL(int ttl)
{
    char t = ttl;

    TRACE(traceFile,
          "setting ttl %d on %d\n",
          ttl,
          descriptor_);
    
    if (setopt(IPPROTO_IP,
	       IP_MULTICAST_TTL,
	       (char *) &t,
	       sizeof(t)) < 0) {
	    ERROR("failed ttl set on %d: %s\n",
		  descriptor_,
		  strerror(errno));
	    return FileOpHardError;
    }
    return FileOpOK;
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

DiskFile::DiskFile(char* fileName,
                   Boolean readOnly,
                   Handler& rh,
                   Handler& wh)
	: File(::open(fileName,
                      (readOnly) ? O_RDONLY : (O_CREAT | O_RDWR),
                      0644),
               (readOnly) ? FileModeReadOnly : FileModeReadWrite,
               rh,
               wh)
{
    // Empty
}

DiskFile::~DiskFile()
{
    // Empty
}

int
DiskFile::seekTo(u_long offset)
{
    int retval;
    
    TRACE(traceFile,
          "seeking to off %u on descriptor %d\n",
          offset,
          descriptor_);
    
    retval = ::lseek(descriptor_, (off_t) offset, SEEK_SET);
    if (retval < 0) {
        return FileOpHardError;
    }
    return retval;
}

int
DiskFile::seekEnd()
{
    int retval;
    
    TRACE(traceFile,
          "seeking to end on descriptor %d\n",
          descriptor_);
    
    retval = ::lseek(descriptor_, (off_t) 0, SEEK_END);
    if (retval < 0) {
        return FileOpHardError;
    }
    return retval;
}

int
DiskFile::lock()
{
    int	retval;

#ifdef HAVE_LOCKF
    retval = ::lockf(descriptor_, F_LOCK, 0);
#elif HAVE_FLOCK
    retval = ::flock(descriptor_, LOCK_EX|LOCK_NB);
#else
    retval = 0;
#endif
    
    if (retval < 0) {
        switch (errno) {
            case EWOULDBLOCK:
#if defined(EAGAIN) && EAGAIN != EWOULDBLOCK
            case EAGAIN:
#endif
                TRACE(traceFile,
                      "descriptor %d already locked\n",
                      descriptor_);
                
                return FileOpSoftError;
            default:
                break;
        }
        return FileOpHardError;
    }

    TRACE(traceFile,
          "descriptor %d not locked by another process\n",
          descriptor_);
    
    return FileOpOK;
}

int
DiskFile::truncate(u_int size)
{
    int	retval;

    retval = ::ftruncate(descriptor_, size);
    if (retval < 0) {
        return FileOpHardError;
    }

    TRACE(traceFile,
          "truncated descriptor %d to size %u\n",
          descriptor_,
          size);
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
