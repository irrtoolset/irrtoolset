//  $Id$
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
//  Author(s): WeeSan Lee <wlee@ISI.EDU>
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#ifndef RIPEWHOISC_HH
#define RIPEWHOISC_HH

#include "config.h"
#include "irr/irr.hh"
#include "util/net.hh"
#include "util/buffer.hh"
#include "gnug++/SetOfUInt.hh"
#include <cassert>


class RipeWhoisClient : public IRR, public Socket {
  private:
    StringBuffer cProbeBuffer;
    char *pzcSourceOriginReply;

  private:
    char *read(void);
    int write(char *buf, int len);
    int write(int iNoofTry, char *buf, int len);
    bool getObject(char *pzcToken, char *pzcCommand, char *&text, int &len);
    bool getLine(char *pzcToken, char *pzcCommand, char *&text, int &len);
    void parseRoutes(const char *pzcReply, PrefixRanges *pzcResult);
    bool getIndirectMembers(const Object *o, 
			    char *query, char *token,
			    void (*collect)(void *, const Object *), 
			    void *collectArg);
   void recoverFromSleepingServerBug();

  protected:
    virtual bool getAutNum(char *as,          char *&text, int &len);
    virtual bool getSet(SymID sname, char *clss, char *&text, int &len);
    virtual bool getRoute(char *rt, char *as, char *&text, int &len);
    virtual bool getInetRtr(SymID inetrtr,    char *&text, int &len);
    virtual bool expandAS(char *as,           PrefixRanges *result);
    virtual bool expandASSet(SymID sname,     SetOfUInt    *result);
    virtual bool expandRSSet(SymID sname,     PrefixRanges *result);
    virtual bool expandRSSet(SymID sname,     MPPrefixRanges *result);
    virtual bool expandRtrSet(SymID sname,    PrefixRanges *result);

  public:
    RipeWhoisClient(void);
    RipeWhoisClient(const char *host, 
		    const int port,
		    const char *sources = dflt_sources);
    virtual void Open(const char *_host = dflt_host, 
		      const int _port = dflt_port, 
		      const char *_sources = dflt_sources);
    virtual void Close();

    // For compatibility reasons mostly for roe
    int getSourceOrigin(char *&buffer, const char *rt);
    int getSourceOrigin(char *&buffer);
    void querySourceOrigin(const char *rt);
    bool readReady(void) {
      return Socket::readReady() || !cProbeBuffer.empty();
    }
    ASt getOrigin(char *format, ...);
};

#endif // RIPEWHOISC_HH
