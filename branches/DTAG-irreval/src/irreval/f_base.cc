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
//  Author(s): Hagen Boehm <hboehm@brutus.nic.dtag.de>

#include "f_base.hh"
#include "normalform/SetOfPrefix.hh"
#include "normalform/SetOfIPv6Prefix.hh"

// option initializing
bool BaseConfig::useAclCaches = false;
bool BaseConfig::forceTilda = false;
bool BaseConfig::singleRouteFilter = true;

void BaseConfig::calculateNegation(SetOfPrefix& nets) {
  // this method should be used by IOSXRConfig and RPSLConfig only!
  // It is used to work out negation of an IPv4 prefix set which
  // is needed to define an IOS XR or RPSL representation of the Set.
  RadixSet inv;
  u_int64_t _rngs = ~(u_int64_t)0;
  inv.insert(0, 0, _rngs);
  inv -= nets.members;
  nets.members = inv;
  nets.not_ = false;
}

void BaseConfig::calculateNegation(SetOfIPv6Prefix& nets) {
  // this method should be used by IOSXRConfig and RPSLConfig only!
  // It is used to work out negation of an IPv6 prefix set which
  // is needed to define an IOS XR or RPSL representation of the Set.
  IPv6RadixSet inv;
  ipv6_addr_t _rngs = ipv6_addr_t(~0, ~0, true);
  inv.insert(ipv6_addr_t((ip_v6word_t)0,(ip_v6word_t)0), 0, _rngs);
  inv -= nets.members;
  nets.members = inv;
  nets.not_ = false;
}

void BaseConfig::printASNDottedNotation(ostream &out, ASt asn) {
  // this method is used to print 32bit decimal AS number in AS dotted
  // decimal notation.
  int ms = asn >> 16;
  int ls = asn & 0xffff;

  out << ms << "\\." << ls;
}

void BaseConfig::setCompression (int complvl) {
  switch (complvl) {
  case COMP_NONE:
    compression = COMP_NONE;
    break;
  case COMP_NORMAL:
    compression = COMP_NORMAL;
    break;
  case COMP_SUPER:
    compression = COMP_SUPER;
    break;
  default:
    compression = COMP_NORMAL;
    cerr << "Error: unknown or undefined level of prefix compression! Using COMP_NORMAL\n" << endl;
  }
}
