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
//  f_bcc.cc : This program reads the input template file, connects to an IRRd
//             to get the required data and outputs a neat router configuration 
//             file (BGP4 policies) for the Bay Routers. Most of the RPSL 
//             specification can be understood by this program.
//
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>
//
//             Srivathsan Sri <vathsan@baynetworks.com>
//             Joe marzot <gmarzot@baynetworks.com>
//             Based on f_cisco.cc, to add BayRS BCC support to RtConfig
//
//  Comment  : I have tried to briefly explain what certain variables and 
//             functions do. And I have left out a lot of them. If anyone 
//             knows about the rest, please let me know. Also feel free to
//             modify/rectify my current explanations too.
//             <vathsan@baynetworks.com>
//////////////////////////////////////////////////////////////////////////

#include "config.h"
#include <cstring>
#include <iostream.h>
#include <iomanip.h>
#include <strstream.h>
#include <cctype>
#include "normalform/NE.hh"
#include "irr/irr.hh"
#include "irr/autnum.hh"
#include "RtConfig.hh"
#include "f_bcc.hh"
#include "rpsl/schema.hh"

#define DBG_BCC 7
#define EXPORT 0
#define IMPORT 1

char BccConfig::mapName[80] = "MyMap"; 
char BccConfig::mapNameFormat[80] = "MyMap_%d_%d"; 
bool BccConfig::forcedInboundMatchIP = true;
bool BccConfig::useAclCaches = true;
bool BccConfig::usePrefixLists = false;
int  BccConfig::mapIncrements = 1;
int  BccConfig::mapCount = 1;
int  BccConfig::mapNumbersStartAt = 1;
bool BccConfig::firstCommunityList = true;
bool BccConfig::printRouteMap = true;

int  BccConfig::bcc_version = 310; // default to 3.10 (BayRS 12.00), well not
				   // used yet in this version
int BccConfig::bcc_max_preference = 1000; // default to 1000 (count down from here)
int BccConfig::bcc_max_prefixes = 66; // default to 66 (SiteManager restriction is
				      // 66 prefixes per policy, restricted
				      // to less than 200 prefixes per policy)

//  flags to influence the advertise list in EXPORT. BCC behaviour:
//  no advertise list: announce summarized natural networks only 
//                     (class A,B or C)
//  advertise list = network 255.255.255.255 mask 255.255.255.255 :
//                   announce all networks classless (as learned from IGP and
//                   EGP.

int BccConfig::bcc_force_back = 0; // default to not use 'back' unless necessary
int BccConfig::bcc_advertise_nets = 0; // default no advertise nets according RADB
int BccConfig::bcc_advertise_all = 0; // 0 (default): to not advertise subnets
				      // 1: advertise 255.255.255.255/32 (see above)

static unsigned int ones(unsigned char from, unsigned char to)
{
   unsigned int result = 0;
   for (int i = 32 - to; i <= 32 - from; i++) result |= (1L << i);
   return result;
}

////////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: printRoutes
// 
// FUNCTIONALITY: This function gets the appropriate data from print() and prints 
//                the policies.
//
// SINGNIFICANT VARIABLES:
// itrp and *itrpp: An instance of the SortedPrefixRangeIterator class. 
// nets  :        A prefix filter (an instance of SetOfPrefix).
// prefix & suffix : These are prepended or appended to the prefixes printed
//                 depending on the other parameters. Prefix may take values
//                 "Advertise" or "network"  and suffix may take "Match exact" 
//                 or "Match Range".
//
// SIGNIFICANT FUNCTIONS: 
// 
// LAST MODIFIED: 13th Sep 1999.
//
///////////////////////////////////////////////////////////////////////////////



ListOf2Ints *BccConfig::printRoutes(RadixSet::SortedPrefixRangeIterator **itrpp,
   SetOfPrefix& nets, char *prefix, char *suffix) {

     // return the access list number if something is printed
   ListOf2Ints *result;
   int count = 0 ;

   if (nets.universal()) 
     return NULL;

   result = prefixMgr.add(nets);
   int aclID = prefixMgr.newID();
   result->add(aclID, aclID);

   int allow_flag = 1;
   if (nets.negated())
     allow_flag = 0;
   
   RadixSet::SortedPrefixRangeIterator *itrp;

   u_int addr;
   u_int leng;
   u_int start;
   u_int end;
   char buffer[64];
   bool ok = false;

   if (itrpp == NULL || *itrpp == NULL) {
     itrp = new RadixSet::SortedPrefixRangeIterator(&nets.members);
     ok = itrp->first(addr, leng, start, end);
   } else {
     itrp = *itrpp;
     ok = itrp->next(addr, leng, start, end);
   }

   while (ok) { 
     cout <<"        "<< prefix << " address " ;
     /* need to look at WeeSan's code */
     cout << int2quad(buffer, addr) << " ";
     cout << "mask" << " ";
     cout << int2quad(buffer, masks[start]) << " " << suffix;
     cout << "\n";
     if (bcc_force_back) cout << "        back\n";	
     if (++count >= bcc_max_prefixes) break;
     ok = itrp->next(addr, leng, start, end);     
   }

   if (ok && count >= bcc_max_prefixes) {
     if (itrpp) *itrpp = itrp;    // to remember the current position of iterator.
   } else {
        if (itrpp) *itrpp = NULL;
   }
   return result;
}

////////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: print
// 
// FUNCTIONALITY: This function makes decision on what to be printed out as policies. 
//                It will prepend and append specific terms to the "to be obtained & 
//                printed" prefixes according to functions it is invoked by. It also 
//                takes care about the maximum number of prefixes per policy and 
//                splits them properly.
//
// SIGNIFICANT VARIABLES:
// ne :           A normal expression is the disjunctive normal form of the normal 
//                terms.Each normal term may have several policy filters.           
// asno:          Autonomous System number
// peerAS:        The AS to which another AS talks to (usually by BGP4 & a TCP connection).
// policy_split_counter: Keeps track of the number of times the same policy has been 
//                splitted according to bcc_max_prefixes and appropriately gives a unique 
//                name me to each of the split policy.
//
// SIGNIFICANT FUNCTIONS:
// ne->first() & ne->next() : These functions returns the first and the next normal terms
//                present in that normal expression.  
// 
// printRoutes(): This function actually prints the policies.
//
// LAST MODIFIED: 13th Sep 1999.
//
////////////////////////////////////////////////////////////////////////////////

int BccConfig::print(NormalExpression *ne, PolicyActionList *actn, 
   ASt asno, ASt peerAS, IPAddr *peer_addr, int import_flag) {
   int last = 0, med = -5, pref = 0, dpa = 0; // INITIALIZE PROPERLY - TBD
   static ListOf2Ints empty_list(1); 
   NormalTerm *nt = ne->first();
   bool allow_flag = !nt->prfx_set.negated();
   int policy_split_counter = 0;
   //int length = ne->length();
   regexp_nf& path = nt->as_path;
   SetOfPrefix& nets = nt->prfx_set;
   char *prefix = "";
   char *suffix = "";

   RadixSet::SortedPrefixRangeIterator *next_itrp,*cur_itrp, itr(&nets.members);
   u_int addr;
   u_int leng;
   u_int start;
   u_int end;
   int no_of_prefixes = 0;

   // This loop finds the number of prefixes in a particular policy.
   for (bool ok = itr.first(addr, leng, start, end);
        ok; ok = itr.next(addr, leng, start, end)) {
   no_of_prefixes++ ;
   }
 
   Debug(Channel(DBG_BCC) << "# ne: " << *ne << "\n");

   if (ne->isEmpty())
      return last; 

   if (ne->is_universal())  
      last = 1;
    
   bool needToPrintNoRouteMap = false;
   if (strcmp(mapName, lastMapName)) {
    strcpy(lastMapName, mapName);
    needToPrintNoRouteMap = true;
    routeMapID = mapNumbersStartAt;
   }

   routeMapGenerated = true;
   prefixListGenerated = false;
   next_itrp = NULL;
   for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) {
     do {
       cur_itrp = next_itrp;
       if (import_flag) {
	 if (!nets.universal()) {
	   cout << "    accept polname Accept";
     
	   if (!allow_flag || (path.is_universal() && nt->prfx_set.universal())) {
	     cout << "Default";
	   } else cout << peerAS;

	   if (no_of_prefixes > bcc_max_prefixes) { 
	     policy_split_counter++;
	     cout << "_" << policy_split_counter;
	   }

	   cout << "\n        action accept\n";
    	   cout << "      match\n";

	   if (allow_flag) {
	     prefix = "network";
	     suffix = "match Range";
	   } else {	// ok, we must add an Ignore list
	     cout << "      back\n";
	     cout << "    accept polname Ignore" << asno;
             
	     if (no_of_prefixes > bcc_max_prefixes) {
	       policy_split_counter++;
	       cout << "_" << policy_split_counter;
	     }
	     cout << "\n        action ignore\n";
	     cout <<   "        precedence " << bcc_max_preference << "\n";
	     cout << "      match\n";
	     cout << "        as asnumber "<< asno + 2 <<"\n";
	     if (bcc_force_back) 
	       cout << "        back\n";
	     prefix = "network";
	     suffix = "match Exact";
	   }
   
	   if (peer_addr->get_text()) { 
	     // if other than 0.0.0.0 add config statement
	     //int2quad(buffer, Prefask_map(peer_addr).get_prefix());
	     cout << "        peer address " << peer_addr->get_text() << "\n";
	     if (bcc_force_back) 
	       cout << "        back\n";
	   }

	   if (!bcc_force_back) 		// this one is needed if we skipped
	     cout << "        back\n";	// them for the lists above
	   cout << "      back\n";	// to close 'match'
	 }
       } else { // Export
	 cout << "    announce polname Announce";
	 regexp_nf& path = nt->as_path; // new
     
	 if (!allow_flag || (path.is_universal() && nt->prfx_set.universal())) {
	   cout << "Default";
	 } else cout << " AS" << peerAS;
  
	 //if (nt->prfx_set.length() > bcc_max_prefixes) {
	 if (no_of_prefixes > bcc_max_prefixes) {
	   policy_split_counter++;
	   cout << "_" << policy_split_counter;
	 }
	 cout << "\n        action announce\n";
	 cout << "      match\n";

	 if (allow_flag) { 
	   if (! nets.isEmpty() && ! nets.universal()) {
	     prefix = "network";
	     suffix = "match Exact";
	   }
	 }
  
	 if (!allow_flag) {	// ok, we must add an Ignore list
	   cout << "    announce polname Ignore" << asno;
	   //if (nt->prfx_set.length() > bcc_max_prefixes) {
	   if (no_of_prefixes > bcc_max_prefixes) {
	     policy_split_counter++;
	     cout << "_" << policy_split_counter;
	   }
	   cout << "\n        action ignore\n";
	   cout << "      match\n";
	   cout << "        as "<< asno + 2 <<"\n";

	   if (! nets.isEmpty() && ! nets.universal()) {
	     prefix = "network";
	     suffix = "match Exact";
	     cout << "\n network & match exact set for ignore list\n";
	   } 

	   cout << "        outbound-as asnumber " << asno + 2 << "\n";
	   if (bcc_force_back) 
	     cout << "        back\n";

	   if (peer_addr->get_text()) { 
	     // if other than 0.0.0.0 add config statement
	     //int2quad(buffer, Prefask_map(peer_addr).get_prefix());
	     cout << 
	  "        outbound-peer address " << peer_addr->get_text()  << "\n";
	     if (bcc_force_back) 
	       cout << "        back\n";
	   }
	 } // end of (!allow_flag)
       }  // end of export

       ListOf2Ints *prfx_acls = printRoutes(&next_itrp,nt->prfx_set, prefix, suffix);
       if (!import_flag && 
	   !strcmp(prefix,"network") && !strcmp(suffix,"match Exact")) {

	 if (bcc_force_back) 
	   cout << "        back\n";
	 cout << "        outbound-as asnumber " << peerAS << "\n";
	 if (peer_addr->get_text()) { 
	   // if other than 0.0.0.0 add config statement
	   //int2quad(buffer, Prefask_map(peer_addr).get_prefix());
	   cout << 
	     "        outbound-peer address " << peer_addr->get_text() << "\n";
	   if (bcc_force_back) 
	     cout << "        back\n";
	 } 

	 if (!bcc_force_back) 		// this one is needed if we skipped
	   cout << "        back\n";	// them for the lists above
	 cout << "      back\n";	

	 if (med > -1 || bcc_advertise_all || bcc_advertise_nets)
	   cout << "      modify\n";   
    
	 if (bcc_advertise_nets) {
	   if (!nt->prfx_set.isEmpty() && !nt->prfx_set.universal()) {
           prefix = "advertise";
	   suffix = "";
	   next_itrp = cur_itrp;
	   printRoutes(&next_itrp,nt->prfx_set, prefix, suffix);  //new
	   }
	 }    

	 if (bcc_advertise_all) {
	   cout << "        advertise address 255.255.255.255 mask 255.255.255.255\n";
	   if (bcc_force_back) 
	     cout << "        back\n";
	 }

	 if (med > -1 || bcc_advertise_all || bcc_advertise_nets)
	   cout << "      back\n";

	 cout << "    back\n";
  
       } // end of if
     } while (next_itrp);
   }  // end of big for loop
   if (!import_flag) { 
     cout << "  back\n";
     cout << "back\n";
   }
   return last;
}

////////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: export
// 
// FUNCTIONALITY: Checks if the ASes are present and have export policies and if
//                so, invokes the print() with appropriate parameters.
//
// SIGNIFICANT VARIABLES:
// autnum :       AutNum Object has contact info and routing policy of that AS.
// fa :           An instance of FilterAction class.
//
// SIGNIFICANT FUNCTIONS: 
// getAutNum():   Retrieves the AutNum object for the specified AS number.
//
// LAST MODIFIED: 13th Sep 1999.
//
///////////////////////////////////////////////////////////////////////////////

void BccConfig::exportP(ASt asno, IPAddr *addr, 
			 ASt peerAS, IPAddr *peer_addr) {

   // Made asno part of the map name if it's not changed by users
   sprintf(mapName, mapNameFormat, asno, mapCount++);

   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);

   if (!autnum) {
     cerr << "Error: no object for AS" << asno << endl;
     return;
   }

   // get matching export attributes
   AutNumSelector<AttrExport> itr(autnum, "export", 
				  NULL, peerAS, peer_addr, addr);
   const FilterAction *fa = itr.first();
   if (! fa) {
     cerr << "Warning: AS" << asno 
	  << " has no export policy for AS" << peerAS << endl;
     return;
   }

   // below is newly added
   cout << "ip\n"
        << "  bgp\n"
        << "    local-as " << asno << "\n";

   if (addr->get_text() && peer_addr->get_text()) {
     cout << "    peer local " << addr->get_text() << " remote " 
	  <<	peer_addr->get_text()
	  << " as " << peerAS << "\n"
	  << "    back\n";
   } 

   NormalExpression *ne;
   int last = 0;
   for (; fa && !last; fa = itr.next()) {
     ne = NormalExpression::evaluate(fa->filter, peerAS);          
     last = print(ne, fa->action, asno, peerAS, peer_addr,EXPORT);
     delete ne;
   }
   
   routeMapGenerated = false;
   prefixListGenerated = false;
}

////////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: import
// 
// FUNCTIONALITY: Checks if the ASes are present and have import policies and if
//                so, invokes the print() with appropriate parameters.
//
// SIGNIFICANT VARIABLES:
// autnum :       AutNum Object has contact info and routing policy of that AS.
// fa :           An instance of FilterAction class.
//
// SIGNIFICANT FUNCTIONS: 
// getAutNum():   Retrieves the AutNum object for the specified AS number.
//
// LAST MODIFIED: 13th Sep 1999.
//
///////////////////////////////////////////////////////////////////////////////

void BccConfig::importP(ASt asno, IPAddr *addr, 
			ASt peerAS, IPAddr *peer_addr) {

   //  Made asno part of the map name if it's not changed by users
   sprintf(mapName, mapNameFormat, asno, mapCount++);
      
   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);

   if (!autnum) {
     cerr << "Error: no object for AS" << asno << endl;
     return;
    }

   int preAclID = prefixMgr.lastID();
   // get matching import attributes
   AutNumSelector<AttrImport> itr(autnum, "import", 
				  NULL, peerAS, peer_addr, addr);
   const FilterAction *fa = itr.first();
   if (! fa) {
     cerr << "Warning: AS" << asno 
	  << " has no import policy for AS" << peerAS << endl;
     return;
   }

   cout << "ip\n"
        << "  bgp\n"
        << "    local-as " << asno << "\n";

   if (addr->get_text() && peer_addr->get_text()) {
     cout << "    peer local " << addr->get_text() << " remote " 
	  <<	peer_addr->get_text()
	  << " as " << peerAS << "\n"
	  << "    back\n";
   } 

   NormalExpression *ne;
   int last = 0;
   for (; fa && !last; fa = itr.next()) {
     ne = NormalExpression::evaluate(fa->filter, peerAS);

     last = print(ne, fa->action, asno, peerAS, peer_addr, IMPORT); 
     delete ne;
   }

   int postAclID = prefixMgr.lastID();
   routeMapGenerated = false;
   prefixListGenerated = false;
   cout << "  back\nback\n#\n";   
}

////////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: deflt
//
// FUNCTIONALITY: Checks if the ASes are present and have default policies and if
//                so, invokes the print() with appropriate parameters.
//
// SIGNIFICANT VARIABLES:
// autnum :       AutNum Object has contact info and routing policy of that AS.
//
// SIGNIFICANT FUNCTIONS: 
// getAutNum():   Retrieves the AutNum object for the specified AS number.
//
// LAST MODIFIED: 13th Sep 1999.
//
///////////////////////////////////////////////////////////////////////////////

void BccConfig::deflt(ASt asno, ASt peerAS) {
   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);

   if (!autnum) {
     cerr << "Error: no object for AS" << asno << endl;
     return;
   }

   // get matching default attributes
   // Modified by wlee
   // AutNumIterator<AttrDefault> itr(autnum, "default", peerAS);
   AutNumDefaultIterator itr(autnum, peerAS);
   const AttrDefault *deflt = itr.first();
   if (! deflt) {
      cerr << "Warning: AS" << asno 
	   << " has no default policy for AS" << peerAS << endl;
      return;
   }

   cout << "ip\n"
        << "  bgp\n"
        << "    local-as " << asno << "\n";

   cout << "    accept polname InjectDefault\n"
        << "        action accept\n"
        << "      match\n"
        << "        as asnumber " << peerAS << "\n"
        << "        back\n"
        << "      back\n"
        << "      modify\n";

   NormalExpression *ne;
   int last = 0;
   char buffer[128];
   for (; deflt && !last; deflt = itr.next()) {
     ne = NormalExpression::evaluate(deflt->filter, peerAS);

     if (ne->is_universal()) {
       cout << "        inject address 0.0.0.0 mask 0.0.0.0\n";
       if (bcc_force_back) 
	 cout << "        back\n";
     } else {
       NormalTerm *nt = ne->first();
       RadixSet::SortedPrefixIterator itr(&(ne->first()->prfx_set.members));
       u_int addr;
       u_int leng;
       char buffer[64];

       for (bool ok = itr.first(addr, leng);
	    ok;
	    ok = itr.next(addr, leng)) {
	 cout << "        inject address " 
	      << int2quad(buffer, addr)
	      << "\n";
         if (bcc_force_back) 
	   cout << "        back\n";
       }
     }
     if (! bcc_force_back) 
       cout << "        back\n";
   }
  
   cout << "      back\n"
        << "   back\n"
        << "  back\n"
        << "back\n";
   cout << "#\n";
}

