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
// Author(s): Ramesh Govindan

#ifndef _Headers_hh
#define _Headers_hh

#include <sys/types.h>
#include "util/Types.hh"
#include "sys/Address.hh"

// IP Header
struct IP {
#ifndef WORDS_BIGENDIAN
    U8 	headerLength:4;
    U8 	version:4;
#else
    U8  version:4;
    U8 	headerLength:4;
#endif
    U8 	typeOfService;
    U16 totalLength;
    U16 identifier;
    U16 fragmentOffset;
    U8 	timeToLive;
    U8 	protocol;
    U16 checksum;
    U32 source;
    U32 destination;

    void hton() {
#ifndef HOST_OS_IS_FREEBSD
        totalLength = htons(totalLength);
#endif
        identifier = htons(identifier);
        fragmentOffset = htons(fragmentOffset);
        source = htonl(source);
        destination = htonl(destination);
    }

    void ntoh() {
#ifndef HOST_OS_IS_FREEBSD
        totalLength = ntohs(totalLength);
#endif
        identifier = ntohs(identifier);
        fragmentOffset = ntohs(fragmentOffset);
        source = ntohl(source);
        destination = ntohl(destination);
    }
};

struct IPOptLSRR {
    U8		nullopt;
    U8		lsrropt;
    U8		length;
    U8		pointer;
    U32  	gateway;

    void
    hton() {
        gateway = htonl(gateway);
    }

    void
    ntoh() {
        gateway = ntohl(gateway);
    }
};

enum ICMPType {
    ICMPTypeEchoReply = 0,
    ICMPTypeDestinationUnreachable = 3,
    ICMPTypeEcho = 8,
    ICMPTypeTimeExceeded = 11
};

enum ICMPDestUnreachCode {
    ICMPUnreachNet = 0,
    ICMPUnreachHost,
    ICMPUnreachProto,
    ICMPUnreachPort,
    ICMPUnreachNeedFrag,
    ICMPUnreachSrtFail,
    ICMPUnreachNetUnknown,
    ICMPUnreachHostUnknown,
    ICMPUnreachNetUnreachForTOS = 11,
    ICMPUnreachHostUnreachForTOS,
    ICMPUnreachAdminProhib,
    ICMPUnreachHostPrecViol,
    ICMPUnreachPrecCutoff
};

// Simplified ICMP header
struct ICMP {
    U8 	type;                /* message type */
    U8 	code;                /* type sub-code */
    U16 	checksum;
    U32		gateway;

    void 
    hton() {
     //           gateway = htonl(gateway);
    }

    void 
    ntoh() {
      //          gateway = ntohs(gateway);
      //          checksum = ntohs(checksum);
    }
};                                                        

enum IGMPType {
    IGMPDvmrp = 0x13,
    IGMPMtrace = 0x1f,
    IGMPMtraceResp = 0x1e
};

enum IGMPDvmrpCode {
    IGMPDvmrpAskNeighbors1 = 3,
    IGMPDvmrpNeighbors1 = 4,
    IGMPDvmrpAskNeighbors2 = 5,
    IGMPDvmrpNeighbors2 = 6
};

enum IGMPDvmrpFlag {
    IGMPDvmrpTunnelFlag=   0x01,    /* neighbors reached via tunnel */
    IGMPDvmrpSrcrtFlag=    0x02,    /* tunnel uses IP source routing */
    IGMPDvmrpPimFlag=      0x04,    /* neighbor is a PIM neighbor */
    IGMPDvmrpDownFlag=     0x10,    /* kernel state of interface */
    IGMPDvmrpDisabledFlag= 0x20,    /* administratively disabled */
    IGMPDvmrpQuerierFlag=  0x40,    /* I am the subnet's querier */
    IGMPDvmrpLeafFlag=     0x80     /* Neighbor reports that it is a leaf */
};    


struct IGMP {
    U8	type;
    U8	code;
    U16	checksum;
    U32 group;

    void
    hton() {
        group = htonl(group);
    }

    void
    ntoh() {
        group = ntohl(group);
    }
};

struct IGMPTrace {
	U32	source;
	U32	destination;
	U32	responseAddress;
#ifndef WORDS_BIGENDIAN
	U32	queryId:24;
	U32     timeToLive:8; //response ttl
#else
	U32     timeToLive:8; //response ttl
	U32	queryId:24;
#endif

    void
    ntoh() {
        source = ntohl(source);
        destination = ntohl(destination);
        responseAddress = ntohl(responseAddress);
    }

    void
    hton() {
        source = htonl(source);
        destination = htonl(destination);
        responseAddress = htonl(responseAddress);
    }
};

struct IGMPTraceResponse {
    U32	arrivalTime;
    U32 inAddress;
    U32	outAddress;
    U32	parentAddress;
    U32 interfaceInputPkts;
    U32	interfaceOutputPkts;
    U32	sourceGroupPkts;
    U8	routingProtocol;
    U8	forwardingTtl;
    U8	sourceMask;
    U8  forwardingErrorCodes;

    void
    hton() {
        arrivalTime = htonl(arrivalTime);
        inAddress = htonl(inAddress);
        outAddress = htonl(outAddress);
        parentAddress = htonl(parentAddress);
        interfaceInputPkts = htonl(interfaceInputPkts);
        interfaceOutputPkts = htonl(interfaceOutputPkts);
        sourceGroupPkts = htonl(sourceGroupPkts);
    }

    void
    ntoh() {
        arrivalTime = ntohl(arrivalTime);
        inAddress = ntohl(inAddress);
        outAddress = ntohl(outAddress);
        parentAddress = ntohl(parentAddress);
        interfaceInputPkts = ntohl(interfaceInputPkts);
        interfaceOutputPkts = ntohl(interfaceOutputPkts);
        sourceGroupPkts = ntohl(sourceGroupPkts);
    }
};

struct UDP {
    U16 sourcePort;
    U16 destinationPort;
    U16 length;
    U16 checksum;

    void
    hton() {
        sourcePort = htons(sourcePort);
        destinationPort = htons(destinationPort);
        length = htons(length);
    }

    void
    ntoh() {
        sourcePort = ntohs(sourcePort);
        destinationPort = ntohs(destinationPort);
        length = ntohs(length);
    }
};

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
//  scan@isi.edu.
//

#endif // _Headers_hh
