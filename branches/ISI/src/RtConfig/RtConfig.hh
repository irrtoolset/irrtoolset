//  $Id$
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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#ifndef RTCONFIG_H
#define RTCONFIG_H

#include "config.h"
#include <iostream.h>
#include "rpsl/rpsl.hh"

class AttrRPAttr;
class AttrMethod;
class SetOfPrefix;
class FilterOfASPath;

class RtConfig {
public:
   virtual void importP(ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr) {
      cerr << "Error: import not implemented" << endl;
   }
   virtual void exportP(ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr) {
      cerr << "Error: export not implemented" << endl;
   }
   virtual void exportGroup(ASt as, char *pset) {
      cerr << "Error: exportGroup not implemented" << endl;
   }
   virtual void importGroup(ASt as, char *pset) {
      cerr << "Error: importGroup not implemented" << endl;
   }
   void printPrefixes(char *filter, char *fmt);
   void printPrefixRanges(char *filter, char *fmt);
   void printSuperPrefixRanges(char *filter, char *fmt);
   void accessList(char *filter);
   void aspathAccessList(char *filter);
   virtual void static2bgp(ASt as, IPAddr* addr) {
      cerr << "Error: static2bgp not implemented" << endl;
   }
   virtual void deflt(ASt as, ASt peerAS) {
      cerr << "Error: default not implemented" << endl;
   }
   virtual void networks(ASt as) {
      cerr << "Error: networks not implemented" << endl;
   }
   virtual void packetFilter
   (char *ifname, ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr) {
      cerr << "Error: pkt_filter not implemented" << endl;
   }
   virtual void outboundPacketFilter
   (char *ifname, ASt as, IPAddr* addr, ASt peerAS, IPAddr* peerAddr) {
      cerr << "Error: outbound_pkt_filter not implemented" << endl;
   }
   virtual void configureRouter(char *name);

protected:
   void printPrefixes_(SetOfPrefix& nets, char *fmt);
   void printPrefixRanges_(SetOfPrefix& nets, char *fmt);
   void printSuperPrefixRanges_(SetOfPrefix& nets, char *fmt);
   virtual void printAccessList(SetOfPrefix& nets) {
      cerr << "Error: accessList not implemented" << endl;
   }
   virtual void printAspathAccessList(FilterOfASPath& path) {
       cerr << "Error: aspathAccessList not implemented" << endl;
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
