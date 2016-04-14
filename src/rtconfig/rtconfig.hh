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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#ifndef RTCONFIG_H
#define RTCONFIG_H

#include "config.h"
#include <iostream>
#include "rpsl/rpsl.hh"
#include "normalform/NE.hh"

class AttrRPAttr;
class AttrMethod;
class SetOfPrefix;
class SetOfIPv6Prefix;
class FilterOfASPath;
class MPPrefix;

class RtConfig {
public:
   virtual void importP(ASt as,
                        MPPrefix* addr,
                        ASt peerAS,
                        MPPrefix* peerAddr) {
      std::cerr << "Error: import not implemented" << std::endl;
   }
   virtual void importPeerGroup(ASt as,
                        MPPrefix* addr,
                        ASt peerAS,
                        MPPrefix* peerAddr,
			char *pset) {
      std::cerr << "Error: import not implemented" << std::endl;
   }
   virtual void exportP(ASt as,
                        MPPrefix* addr,
                        ASt peerAS,
                        MPPrefix* peerAddr) {
      std::cerr << "Error: export not implemented" << std::endl;
   }

   virtual void exportGroup(ASt as, char *pset) {
      std::cerr << "Error: exportGroup not implemented" << std::endl;
   }
   virtual void importGroup(ASt as, char *pset) {
      std::cerr << "Error: importGroup not implemented" << std::endl;
   }
   
   virtual const char *returnPermitOrDeny(bool) {
      std::cerr << "Error: returnPermitOrDeny not implemented" << std::endl;
      return NULL;
   }

   void printPrefixes(char *filter, char *fmt);
   void printPrefixRanges(char *filter, char *fmt);
   void printSuperPrefixRanges(char *filter, char *fmt);

   void accessList(char *filter);
   void aspathAccessList(char *filter);
   virtual void static2bgp(ASt as, MPPrefix* addr) {
      std::cerr << "Error: static2bgp not implemented" << std::endl;
   }
   virtual void deflt(ASt as, ASt peerAS) {
      std::cerr << "Error: default not implemented" << std::endl;
   }
   virtual void networks(ASt as) {
      std::cerr << "Error: networks not implemented" << std::endl;
   }
   virtual void IPv6networks(ASt as) {
      std::cerr << "Error: v6networks not implemented" << std::endl;
   }
   virtual void inboundPacketFilter
   (char *ifname, ASt as, MPPrefix* addr, ASt peerAS, MPPrefix* peerAddr) {
      std::cerr << "Error: inbound_pkt_filter not implemented" << std::endl;
   }
   virtual void outboundPacketFilter
   (char *ifname, ASt as, MPPrefix* addr, ASt peerAS, MPPrefix* peerAddr) {
      std::cerr << "Error: outbound_pkt_filter not implemented" << std::endl;
   } 

   virtual void configureRouter(char *name);
   void printPolicyWarning(ASt as, MPPrefix* addr, ASt peerAS, MPPrefix* peerAddr, const char* policy);

protected:
   void printPrefixes_(SetOfPrefix& nets, char *fmt);
   void printPrefixRanges_(SetOfPrefix& nets, char *fmt);
   void printSuperPrefixRanges_(SetOfPrefix& nets, char *fmt);

   void printPrefixes_(SetOfIPv6Prefix& nets, char *fmt);
   void printPrefixRanges_(SetOfIPv6Prefix& nets, char *fmt);
   void printSuperPrefixRanges_(SetOfIPv6Prefix& nets, char *fmt);

   virtual void printAccessList(SetOfPrefix& nets) {
      std::cerr << "Error: accessList not implemented" << std::endl;
   }
   virtual void printAccessList(SetOfIPv6Prefix& nets) {
      std::cerr << "Error: accessList not implemented" << std::endl;
   }
   virtual void printAspathAccessList(FilterOfASPath& path) {
      std::cerr << "Error: aspathAccessList not implemented" << std::endl;
  }

public:
   static bool supressMartians;
   static int  preferenceCeiling;

   static void loadDictionary();

   static const AttrRPAttr *dctn_rp_pref;
   static const AttrMethod     *dctn_rp_pref_set;
   static const AttrRPAttr *dctn_rp_nhop;
   static const AttrMethod     *dctn_rp_nhop_set;
   static const AttrRPAttr *dctn_rp_dpa;
   static const AttrMethod     *dctn_rp_dpa_set;
   static const AttrRPAttr *dctn_rp_med;
   static const AttrMethod     *dctn_rp_med_set;
   static const AttrRPAttr *dctn_rp_community;
   static const AttrMethod     *dctn_rp_community_setop;
   static const AttrMethod     *dctn_rp_community_appendop;
   static const AttrMethod     *dctn_rp_community_append;
   static const AttrMethod     *dctn_rp_community_delete;
   static const AttrRPAttr *dctn_rp_aspath;
   static const AttrMethod     *dctn_rp_aspath_prepend;
}
;

extern RtConfig *rtConfig;

extern char *opt_prompt;

#endif // RTCONFIG_H
