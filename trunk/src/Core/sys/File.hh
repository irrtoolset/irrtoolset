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
// File.hh
// Author(s): Ramesh Govindan
//
// This file describes machine-independent interfaces to files and sockets.

#ifndef _File_hh
#define _File_hh

extern "C" {
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
}

#include "util/List.hh"
#include "util/Handler.hh"
#include "sys/Address.hh"

// Classes defined elsewhere

// All file operations return an integer value:
//	a non-negative integer if the operation is successful
//	-1 if the operation should be retried later
//	-2 if the operation failed.
//

const int FileOpOK = 0;
const int FileOpSoftError = -1;
const int FileOpHardError = -2;

enum FileMode {
    FileModeNone,
    FileModeReadOnly,
    FileModeReadWrite
};

enum RawSocketType {
    RawSocketICMP = IPPROTO_ICMP,
    RawSocketIGMP = IPPROTO_IGMP,
    RawSocketRAW = IPPROTO_RAW
};

class File : public DListNode {
    
  public:
    // Create a new file
    File(int,				// In: descriptor to set file
         FileMode,			// In: mode of file
         const Handler&,		// In: read call back function
         const Handler&);		// In: write call back function

    // Remove a file
    ~File();

    // Read from a connected socket or file
    int
    read(char*,				// In: pointer to buffer to read into
         int);				// In: size of buffer

    // Write to a connected socket or file
    int
    write(char*,			// In: pointer to buffer to write from
          int);				// In: size of data

    // Set handlers
    void
    setHandlers(Handler&,		// In: read callback function
                Handler&);		// In: write callback function

    void handle_r() { readHandler.callBack((void*)this); }
    void handle_w() { writeHandler.callBack((void*)this); }
    bool has_rh() const { return !readHandler.null(); }
    bool has_wh() const { return !writeHandler.null(); }
    int descriptor() const { return descriptor_; }
    FileMode mode() const { return mode_; }
    
protected:
    Handler	readHandler;
    Handler	writeHandler;
    int		descriptor_;
    FileMode	mode_;
};

class Socket : public File {
protected:
	Socket(int,				// In: descriptor to set file
	       FileMode,			// In: mode of file
	       const Handler&,		// In: read call back function
	       const Handler&);		// In: write call back function
public:
	int setopt(int level, int optname, const void *optval, size_t optlen);
};

class DatagramSocket : public Socket {
  public:
    // Create a new UDP socket
    DatagramSocket(const Handler&,	// In: read handler
                   const Handler&);	// In: write handler

    // Delete the UDP socket
    ~DatagramSocket();

    // Receive data on a unconnected socket
    int
    recvFrom(char*,			// In: pointer to buffer
             int,			// In: size of buffer
             Address&,			// Out: address of sender
             Port*);			// Out: port number of sender

    // Send data on unconnected socket
    int
    sendTo(char*,			// In: pointer to data to send
           int,				// In: size of buffer
           Address&,			// In: destination address
           Port);			// In: destination port

    static int
    defaultInterface(Address& localAddress, const Address &dst); // ret local address for sending to dst 
};

class RawSocket : public Socket {
  public:
    // Create a new UDP socket
    RawSocket(Handler&,		// In: read handler
              Handler&,		// In: write handler
              RawSocketType);	// In: type of raw socket

    // Delete the UDP socket
    ~RawSocket();

    // Receive data on a unconnected socket
    // Buffer must be large enough to include IP header
    int
    recvFrom(char*,			// In: pointer to buffer
             int,			// In: size of buffer
             Address&,			// Out: address of sender
             Port*);			// Out: port number of sender

    // Send data on unconnected socket
    // Data must include IP header!
    int
    sendTo(char*,			// In: pointer to data to send
           int,				// In: size of buffer
           Address&,			// In: destination address
           Port) const;			// In: destination port

    // Join a multicast group address
    int
    join(const Address&);		// In: group address to join

};

class MulticastSocket : public DatagramSocket {
  public:
    // Create a new multicast socket
    MulticastSocket(const Address&,	// In: group address to join on
                    Port,		// In: local port number
                    const Handler&,	// In: read handler
                    const Handler&);	// In: write handler

    // Delete the multicast socket
    ~MulticastSocket();

    // Get default multicast interface address
    int
    defaultInterface(Address&);
    
    // Set a TTL value for the multicast group
    int
    setTTL(int ttl);			// In: ttl value

    int
    sendTo(char* buf, int sz) {
	return DatagramSocket::sendTo(buf, sz, group, port);
    }

    Address	group;
    Port	port;
};

class StreamSocket : public Socket {
  public:
    // Create a new socket
    StreamSocket(Address&,		// In: address of remote end
                 Port,			// In: well-known remote listen port
                 Handler&,		// In: read handler
                 Handler&);		// In: write handler

    // Create a new connected stream socket
    StreamSocket(int,			// In: descriptor to accord to socket
                 Handler&,		// In: read handler
                 Handler&);		// In: write handler

    // Delete a stream socket
    ~StreamSocket();
};

class ListenSocket : public Socket {
  public:
    // Create a connected socket listening for connections
    ListenSocket(Port,			// In: port number to listen on
                 Handler&,		// In: handler to call for conn reqs
                 Boolean);		// In: local connections only

    // Delete a connected socket listening for connections
    ~ListenSocket();

    // Accept an incoming connection on socket marked for listening
    int
    accept(StreamSocket**,		// Out: instance of connection
           Address&,			// Out: remote address
           Port*,			// Out: remote port
           Handler&,			// In: read handler for connection
           Handler&);			// In: write handler for connection
};

class DiskFile : public File {
  public:
    // Create a new disk file, possibly in read-only mode
    DiskFile(char*,			// In: name of file
             Boolean,			// In: true if read only
             Handler&,			// In: read handler
             Handler&);			// In: write handler

    // Close a disk file
    ~DiskFile();
    
    // Seek to a particular offset within file
    int
    seekTo(u_long);			// In: offset to seek to

    // Seek to end of file
    int
    seekEnd();

    // Set an advisory lock on a file
    int
    lock();

    // Truncate a file
    int
    truncate(u_int);			// In: size to truncate file to
};

// Sundry externs
extern void createDir(char*);
extern void openPidFile(const char*);
extern void closePidFile();
extern volatile void daemonize(void);

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
//  Author(s): Ramesh Govindan <govindan@isi.edu>

#endif // _File_hh
