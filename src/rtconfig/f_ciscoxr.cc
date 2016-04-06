//  $Id: f_ciscoxr.cc 319 2010-03-19 22:19:39Z nick $
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

#include "config.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include "normalform/NE.hh"
#include "rtconfig.hh"
#include "f_ciscoxr.hh"
#include "f_cisco.hh"
#include "rpsl/schema.hh"

using namespace std;

#define EXPORT 0
#define IMPORT 1

char CiscoXRConfig::mapName[80] = "MyMap"; 
char CiscoXRConfig::mapNameFormat[80] = "MyMap_%d_%d"; 
bool CiscoXRConfig::forcedInboundMatchIP = true;
bool CiscoXRConfig::useAclCaches = true;
bool CiscoXRConfig::compressAcls = true;
bool CiscoXRConfig::usePrefixLists = true;
bool CiscoXRConfig::eliminateDupMapParts = false;
bool CiscoXRConfig::forceTilda = false;
bool CiscoXRConfig::emptyLists = false;
int  CiscoXRConfig::mapIncrements = 1;
int  CiscoXRConfig::mapCount = 1;
int  CiscoXRConfig::mapNumbersStartAt = 1;
bool CiscoXRConfig::firstCommunityList = true;
bool CiscoXRConfig::printRouteMap = true;
string BUFFER;
ostringstream delayedout;
ostringstream delayedout2;
ostringstream delayedout3;
bool done_deny[1000];
bool done_permit[1000];
bool firstRE = true;

/// access-list and prefix-list names
char xripv6_acl[6] = "ipv6-";
char xripv6_pl[8] = "ipv6-pl";

//
int ifcount = 0 ;

unsigned int XRones(unsigned char from, unsigned char to)
{
  unsigned int result = 0;
  for (int i = 32 - to; i <= 32 - from; i++) result |= (1L << i);
  return result;
}

const char *CiscoXRConfig::returnPermitOrDeny(bool allow_flag) {
   if (allow_flag)
      return "permit";
   else
      return "deny";
}
                  
ListOf2Ints *CiscoXRConfig::printRoutes(SetOfIPv6Prefix& nets) {
   if (usePrefixLists)
      return printPrefixList(nets);

   // return the access list number if something is printed
   ListOf2Ints *result;

   if (nets.universal())
      return NULL;

   // check to see if we already printed an identical access list, 
   if (useAclCaches && (result = ipv6prefixMgr.search(nets)))
      return result;

   result = ipv6prefixMgr.add(nets);
   int aclID = ipv6prefixMgr.newID();
   result->add(aclID, aclID);

   int allow_flag = 1;
   if (nets.negated())
      allow_flag = 0;

      IPv6RadixSet::SortedPrefixIterator itr(&nets.members);
      ipv6_addr_t addr;
      u_int leng;
      char buffer[256];

      for (bool ok = itr.first(addr, leng);
	   ok;
	   ok = itr.next(addr, leng)) {
	 delayedout << "ipv6 access-list " << xripv6_acl << aclID << returnPermitOrDeny(allow_flag);
	 delayedout << ipv62hex(&addr, buffer) << "/" << leng << " any" << endl;
      }
  // }

   // terminate the access list
   delayedout << "ipv6 access-list " << xripv6_acl << aclID << returnPermitOrDeny(!allow_flag) << "any any" << endl;

   return result;
}

ListOf2Ints *CiscoXRConfig::printRoutes(SetOfPrefix& nets) {
   if (usePrefixLists)
      return printPrefixList(nets);

   // return the access list number if something is printed
   ListOf2Ints *result;

   if (nets.universal())
      return NULL;

   // check to see if we already printed an identical access list, 
   if (useAclCaches && (result = prefixMgr.search(nets)))
      return result;

   result = prefixMgr.add(nets);
   int aclID = prefixMgr.newID();
   result->add(aclID, aclID);

   int allow_flag = 1;
   if (nets.negated())
      allow_flag = 0;

   if (compressAcls) {
      RadixSet::SortedPrefixRangeIterator itr(&nets.members);
      u_int addr;
      u_int leng;
      u_int start;
      u_int end;
      char buffer[64];

      for (bool ok = itr.first(addr, leng, start, end);
	   ok;
	   ok = itr.next(addr, leng, start, end)) {
	 delayedout << "access-list " << aclID << returnPermitOrDeny(allow_flag) << "ip ";

	 /* need to look at WeeSan's code */
	 delayedout << int2quad(buffer, addr) << "   ";
	 delayedout << int2quad(buffer, XRones(leng + 1, end)) << "   ";
	 delayedout << int2quad(buffer, masks[start]) << "   ";
	 delayedout << int2quad(buffer, XRones(start + 1, end))
	      << endl;
      }
   } else {
      RadixSet::SortedPrefixIterator itr(&nets.members);
      u_int addr;
      u_int leng;
      char buffer[64];

      for (bool ok = itr.first(addr, leng);
	   ok;
	   ok = itr.next(addr, leng)) {
	 delayedout << "access-list " << aclID << returnPermitOrDeny(allow_flag) << "ip ";

	 delayedout << int2quad(buffer, addr) << "   0.0.0.0   ";
	 delayedout << int2quad(buffer, masks[leng]) << "   0.0.0.0" << endl;
      }
   }

   // terminate the access list
   delayedout << "access-list " << aclID  << returnPermitOrDeny(!allow_flag)
        << "ip 0.0.0.0 255.255.255.255 0.0.0.0 255.255.255.255" << endl;

   return result;
}

ListOf2Ints *CiscoXRConfig::printPrefixList(SetOfIPv6Prefix& nets) {

   // return the access list number if something is printed
   ListOf2Ints *result;

   if (nets.universal())
      return NULL;

   // check to see if we already printed an identical access list, 
   if (useAclCaches && (result = ipv6prefixMgr.search(nets)))
      return result;

   result = ipv6prefixMgr.add(nets);
   int aclID = ipv6prefixMgr.newID();
   result->add(aclID, aclID);

   int allow_flag = 1;
   if (nets.negated())
      allow_flag = 0;

    delayedout << "prefix-set " << xripv6_pl << aclID << "-" << returnPermitOrDeny(allow_flag) << endl << "  ";

   IPv6RadixSet::SortedPrefixRangeIterator itr(&nets.members);
   ipv6_addr_t addr;
   u_int leng;
   u_int start;
   u_int end;
   char buffer[256];
   bool first = true;

   for (bool ok = itr.first(addr, leng, start, end);
	ok;
	ok = itr.next(addr, leng, start, end)) {
      if (!first) {
        delayedout <<"," << endl << "  ";
      } else {
        first = false;
      }
      delayedout << ipv62hex(&addr, buffer) << "/" << leng;
      if (start != leng)
	      delayedout << " ge " << start;
      if (end != leng)
	      delayedout << " le " << end;
   }

   delayedout << endl;
   delayedout << "end-set" << endl << endl;

   delayedout << "prefix-set " << xripv6_pl << aclID << "-" << returnPermitOrDeny(!allow_flag) << endl;
   if (!allow_flag) delayedout << "  ::/0 le 128" << endl;
   delayedout << "end-set" << endl << endl;

   return result;

}

ListOf2Ints *CiscoXRConfig::printPrefixList(SetOfPrefix& nets) {
   // return the access list number if something is printed
   ListOf2Ints *result;

   if (nets.universal())
      return NULL;

   // check to see if we already printed an identical access list, 
   if (useAclCaches && (result = prefixMgr.search(nets)))
      return result;

   result = prefixMgr.add(nets);
   int aclID = prefixMgr.newID();
   result->add(aclID, aclID);

   int allow_flag = 1;
   if (nets.negated())
      allow_flag = 0;

   RadixSet::SortedPrefixRangeIterator itr(&nets.members);
   u_int addr;
   u_int leng;
   u_int start;
   u_int end;
   char buffer[64];

   delayedout << "\nprefix-set pl" << aclID << "-" << returnPermitOrDeny(!allow_flag) << endl;
   if (!allow_flag) delayedout << "  0.0.0.0/0 le 32" << endl;
   delayedout << "end-set" << endl;

   delayedout << "\nprefix-set pl" << aclID << "-" << returnPermitOrDeny(allow_flag) << endl;
   bool first = true;

   for (bool ok = itr.first(addr, leng, start, end);
	ok;
	ok = itr.next(addr, leng, start, end)) {

        if (!first) {
          delayedout <<"," << endl;
        } else {
          first = false;
        }

      /* need to look at WeeSan's code */
      delayedout << "  " << int2quad(buffer, addr) << "/" << leng;
      if (start != leng)
	 delayedout << " ge " << start;
      if (end != leng)
	 delayedout << " le " << end;
   }

   delayedout << endl;
   delayedout << "end-set" << endl << endl;

   return result;
}
ListOf2Ints *CiscoXRConfig::printCommunities(FilterOfCommunity& cm) {
   // return the access list numbers if something is printed
   ListOf2Ints *result;

   if (cm.is_universal())
      return 0;

   // check to see if we already printed an identical access list,
   if (useAclCaches && (result = communityMgr.search(cm)))
      return result;

   result = communityMgr.add(cm);
   int aclID;

   // now print the communities
   // first print the conjuncts which are only positive contains
   CommunityConjunct *cc;
   bool first_time = true;
   bool first = true;

   for (cc = cm.conjuncts.head(); cc; cc = cm.conjuncts.next(cc)) {
      if (cc->pe->isEmpty() && cc->n->isEmpty() && cc->ne.isEmpty()
          && ! cc->p->isEmpty()) {
         if (first_time) {
            first_time = false;
            // clear this access list
           
            aclID = communityMgr.newID();
            result->add(aclID, aclID);
              delayedout << "community-set commset" << aclID << "-permit" << endl;
              done_permit[aclID] = true;
         }

         for (Pix pi = cc->p->first(); pi; cc->p->next(pi)) {
            if (!first) {
              delayedout <<"," << endl << "  ";
            } else {
              first = false;
            }
              CiscoXRConfig::printCommunity(delayedout, (*cc->p)(pi));
         }
           
      }
   }
   if (done_permit[aclID]) {
     delayedout << endl << "end-set" << endl << endl;
   }

   for (cc = cm.conjuncts.head(); cc; cc = cm.conjuncts.next(cc)) {
      if (cc->pe->isEmpty() && cc->n->isEmpty() && cc->ne.isEmpty()
          && ! cc->p->isEmpty()) // this case is done above
         continue;

       if ((done_permit[aclID]) && (!done_deny[aclID])) delayedout << "community-set commset" << aclID << "-deny" << endl << "end-set" << endl << endl;
       if ((done_deny[aclID]) && (!done_permit[aclID])) delayedout << "community-set commset" << aclID << "-permit" << endl << "end-set" << endl << endl;

      // clear this access list
      aclID = communityMgr.newID();

      if (! cc->pe->isEmpty()) {
         delayedout << "community-set commset" << aclID << "-permit" << endl;
         done_permit[aclID] = true;
            delayedout << " ";
         for (Pix pi = cc->pe->first(); pi; cc->pe->next(pi)) {
            CiscoXRConfig::printCommunity(delayedout, (*cc->pe)(pi));
            delayedout << " ";
         }
         delayedout << endl << "end-set" << endl << endl;
         result->add(aclID, aclID, 1);
         continue; // if pe is non-empty then p, n, ne are all empty
      }

      // TGG
      // ne needs an exact match too. Each element of cc
      // requires its own (exactly matched) community list
      //
      CommunitySet *cs;
      for (cs = cc->ne.head(); cs; cs = cc->ne.next(cs)) {
	 delayedout << "community-set commset" << aclID << "-deny" << endl;
         done_deny[aclID] = true;
	 delayedout << "  " << aclID << "," << endl;
	 for (Pix pi = cs->first(); pi; cs->next(pi)) {
            CiscoXRConfig::printCommunity(delayedout, (*cs)(pi));
            delayedout << " ";
	 }
	 delayedout << endl << "end-set" << endl << endl;
	 result->add(aclID, aclID, 1);
	 if (!(cc->p->isEmpty()) ||
	     !(cc->n->isEmpty()) ||
	     cc->ne.next(cs)) {
       if ((done_permit[aclID]) && (!done_deny[aclID])) delayedout << "community-set commset" << aclID << "-deny" << endl << "end-set" << endl << endl;
       if ((done_deny[aclID]) && (!done_permit[aclID])) delayedout << "community-set commset" << aclID << "-permit" << endl << "end-set" << endl << endl;
            aclID = communityMgr.newID();
	 }
      }

      // TGG
      // if there will not follow at least one positive match,
      // and an ne was encountered, then we must add a community
      // list that will match anything
      //
      if (cc->pe->isEmpty() && cc->p->isEmpty() && cc->n->isEmpty()) {
       if ((done_permit[aclID]) && (!done_deny[aclID])) delayedout << "community-set commset" << aclID << "-deny" << endl << "end-set" << endl << endl;
       if ((done_deny[aclID]) && (!done_permit[aclID])) delayedout << "community-set commset" << aclID << "-permit" << endl << "end-set" << endl << endl;
        aclID = communityMgr.newID();
        delayedout << "ip community-list " << aclID << " permit internet" << endl;
	result->add(aclID, aclID);
	continue;
      }

      bool first = true;
      Pix pi;
         delayedout << "community-set commset" << aclID << "-deny" << endl;
         done_deny[aclID] = true;
      for (pi = cc->n->first(); pi; cc->n->next(pi)) {
         if (!first) {
           delayedout <<"," << endl;
         } else {
           first = false;
         }
         delayedout << "  "; 
         CiscoXRConfig::printCommunity(delayedout, (*cc->n)(pi));
      }
      delayedout << endl << "end-set" << endl << endl;



      if (! cc->p->isEmpty()) {
         delayedout << "community-set commset" << aclID << "-permit" << endl;
         done_permit[aclID] = true;
         for (pi = cc->p->first(); pi; cc->p->next(pi)) {
            delayedout << "  ";
            CiscoXRConfig::printCommunity(delayedout, (*cc->p)(pi));
            delayedout << "";
         }
         delayedout << endl << "end-set" << endl << endl;
      }

      // TGG
      // don't do this for ne's, only for n's
      if (cc->p->isEmpty() && !cc->n->isEmpty()) {
         delayedout << "community-set commset" << aclID << "-permit" << endl << "  *" << endl << "end-set" << endl;
         done_permit[aclID] = true;
      }
      result->add(aclID, aclID);
   }

   if ((done_permit[aclID]) && (!done_deny[aclID])) delayedout << "community-set commset" << aclID << "-deny" << endl << "end-set" << endl << endl;
   if ((done_deny[aclID]) && (!done_permit[aclID])) delayedout << "community-set commset" << aclID << "-permit" << endl << "  *" << endl << "end-set" << endl << endl;

   return result;
}

void CiscoXRConfig::printREASno(ostream& out, const RangeList &no) {
   RangeList::Range *pi;
   int first = 1;
   int put_par = 0;

   if (no.universal())
      out << "(_[0-9]+)";
   else {
      out << "_";
      pi = no.ranges.head();
      put_par = ! no.ranges.isSingleton() || pi->high != pi->low || inTilda;
      if (put_par)
	 if (reSplittable)
	    if (inTilda)
	       out << "&";
	    else
	       out << "@";
	 else
	    out << "("; 

      for (; pi; pi = no.ranges.next(pi)) {
	 for (int i = pi->low; i <= pi->high; ++i) {
	    if (!first) 
	       out << "|";
	    else
	       first = 0;
	    out << i;
	 } 
      }
      
      if (put_par)
         out << ")";
   }
}

int CiscoXRConfig::printRE_(ostream& os, const regexp& r) {
   static bool inAlternate = false;
   int flag = 0;

   if (typeid(r) == typeid(regexp_or)) {
      bool save = inAlternate;

      if (!inAlternate) {
	 inAlternate = true;
	 os << "(";
      }

      flag = CiscoXRConfig::printRE_(os, *((regexp_or &) r).left);
      os << "|";
      flag &= CiscoXRConfig::printRE_(os, *((regexp_or &) r).right);

      inAlternate = save;
      if (!inAlternate)
	 os  << ")";
      return flag;
   }

   inAlternate = false;

   if (typeid(r) == typeid(regexp_bol)) {
      os << "^";
      return flag;
   }

   if (typeid(r) == typeid(regexp_eol)) {
      os << "$";
      return 1;
   }

   if (typeid(r) == typeid(regexp_symbol)) {
      CiscoXRConfig::printREASno(os, ((regexp_symbol &) r).asnumbers);
      return flag;
   }

   if (typeid(r) == typeid(regexp_star)) {
      bool save = reSplittable;
      reSplittable = forceTilda;
      hasTilda = forceTilda;
      inTilda = forceTilda;
      os << "(";
      flag = CiscoXRConfig::printRE_(os, *((regexp_star &) r).left);
      os << ")*";
      reSplittable = save;
      return flag;
   }

   if (typeid(r) == typeid(regexp_plus)) {
      bool save = reSplittable;
      reSplittable = forceTilda;
      hasTilda = forceTilda;
      inTilda = forceTilda;
      os << "(";
      flag = CiscoXRConfig::printRE_(os, *((regexp_plus &) r).left);
      os << ")+";
      reSplittable = save;
      return flag;
   }

   if (typeid(r) == typeid(regexp_tildastar)) {
      hasTilda = true;
      inTilda = true;
      os << "(";
      flag = CiscoXRConfig::printRE_(os, *((regexp_star &) r).left);
      os << ")*";
      inTilda = false;
      return flag;
   }

   if (typeid(r) == typeid(regexp_tildaplus)) {
      hasTilda = true;
      inTilda = true;
      os << "(";
      flag = CiscoXRConfig::printRE_(os, *((regexp_plus &) r).left);
      os << ")+";
      inTilda = false;
      return flag;
   }

   if (typeid(r) == typeid(regexp_question)) {
      os << "(";
      flag = CiscoXRConfig::printRE_(os, *((regexp_question &) r).left);
      os << ")?";
      return flag;
   }

   if (typeid(r) == typeid(regexp_cat)) {
      CiscoXRConfig::printRE_(os, *((regexp_cat &) r).left);
      flag = CiscoXRConfig::printRE_(os, *((regexp_cat &) r).right);
      return flag;
   }

   assert(1);
   os << "REGEXP_UNKNOWN";
   return flag;
}

void CiscoXRConfig::printRE(ostream &s, 
		          const regexp &r, 
			  int aclID, 
			  bool permit){
   ostringstream out;
   if (!firstRE) {
     out << ",\n";
   } else {
//     out << endl;
     firstRE = false;
   } 

   out << " ios-regex '";
   reSplittable = true;
   hasTilda = false;
   inTilda = false;
   if (!CiscoXRConfig::printRE_(out, r)) {// true if the expression was missing $
      out << "_'";
   } else {
      out <<"'";
   }
//   out << endl;

   int lineLen = out.str().length();

   // for string.h, we need to ensure that p is a char *, not const char *
   char *p = strdup(out.str().c_str());

   if (lineLen < 240 && ! hasTilda) {
      for (char *q = strchr(p, '@'); q; q = strchr(q, '@'))
         *q = '(';
      s << p;
   } else { // need to split into multiple lines
      if (hasTilda) {
         for (char *q = strchr(p, '@'); q; q = strchr(q, '@'))
            *q = '(';
         for (char *q = strchr(p, '&'); q; q = strchr(q, '&'))
            *q = '@';
      }

      char *q, *q2;
      char *r = NULL;
      int size = 0;
      for (q = strchr(p, '@'); q; q = strchr(q, '@')) {
         q2 = strchr(q, ')');
         if (q2 - q > size) {
            r = q;
            size = q2 - q;
         }
         *q = '(';
         q = q2;
      }
      if (!r) {
         s << out.str();
         cerr << "Warning: ip as-path access-list is too long for ciscoxr to handle" << endl;
      } else {
         char *r2;
	 int inc;
	 if (hasTilda) {
	     inc = 1;
	 } else {
	     if (240 - lineLen + size > 5) {
	         inc = 240 - lineLen + size;
	     } else {
	         inc = 5;
	     }
	 }
	 q = strchr(r, ')') + 1;
	 *r = 0;
	 r++;
	 while (r < q) {
	    r2 = r + inc;
	    if (r2 > q - 2)
	       r2 = q - 1;
	    else
	       while (*r2 != '|' && r2 < q - 1)
		  r2++;
	    *r2 = 0;
	    s << p << "(" << r << ")" << q;	
	    r = r2 + 1;
            if (r>=q) {
              s << "";
            } else {
              s << ",\n";
            }
	 } 
      }
   }
   free(p);
}

ListOf2Ints* CiscoXRConfig::printASPaths(regexp_nf& path) {
// return the access list number if something is printed
   ListOf2Ints *result;

   if (path.is_universal())
      return NULL;


   // check to see if we already printed an identical access list
   if (useAclCaches && (result = aspathMgr.search(path)))
      return result;

   result = aspathMgr.add(path);
   int aclID;

   regexp_nf::RegexpConjunct *rc;
   regexp_nf::RegexpConjunct::ReInt *ri;
   regexp_nf::RegexpConjunct::ReInt *positive_ri = NULL;
   int sno = 0; 

   // we will print as few access lists as necessary, often more than one
   // to minimize the number of access lists generated, we will do 3 passes
   // pass 1: RegexpConjuncts w/ one positive as path and no negative
   // pass 2: RegexpConjuncts w/ one or zero positive as path and negatives
   // pass 3: rest

   bool needNewAclpermit = true;
   bool needNewAcldeny = true;
   int pass1_found = 0;
   for (rc = path.rclist.head(); rc; rc = path.rclist.next(rc)) {
      ri = rc->regexs.head();
      if (rc->regexs.isSingleton() && !ri->negated) {
	 pass1_found = 1;
	 if (needNewAclpermit) {
	    needNewAclpermit = false;
	    aclID = aspathMgr.newID();
            firstRE = true;
	    delayedout << "exit" << endl << "as-path-set as-set" << aclID <<  "-permit" << endl;
         }
         CiscoXRConfig::printRE(delayedout, *ri->re, aclID, true);
	 rc->mark = 1;


      } else {
	 rc->mark = 0;
      }
   }

   if (!needNewAclpermit) {
     delayedout << "\nend-set" << endl << endl;
     delayedout "as-path-set as-set" << aclID << "-deny" << endl;
     delayedout << "end-set" << endl << endl;
   }

   // pass 2
   int pass2_found = 0;
   for (rc = path.rclist.head(); rc; rc = path.rclist.next(rc)) {
      if (rc->mark) // done in pass 1, skip
	 continue;

      int found = 0;
      for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
	 if (!ri->negated)
	    found++;
	 if (found > 1) 
	    break;
      }
      if (found <= 1) { // really found 
	 pass2_found = 1;
	 rc->mark = 1;
	 if (needNewAcldeny) {
	    aclID = aspathMgr.newID();
            firstRE = true;
	    delayedout << "exit" << endl << "as-path-set as-set" << aclID << "-deny" << endl;
	 }
	 for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
	    if (!ri->negated)
	       positive_ri = ri;
	    else {
	       CiscoXRConfig::printRE(delayedout, *ri->re, aclID, false);
	    }
	 }

         delayedout << endl << "end-set" << endl << endl;
	 if (positive_ri) {
  	   if (needNewAcldeny) {
              firstRE = true;
	      delayedout << "as-path-set as-set" << aclID << "-permit" << endl;
	   }
	    CiscoXRConfig::printRE(delayedout, *positive_ri->re, aclID, true);
	 } else {
	    delayedout << "as-path-set as-set" << aclID << "-permit" << endl << " permit .*" << endl;
	 }

         delayedout << endl << "end-set" << endl << endl;
	 result->add(aclID, aclID);
	 needNewAclpermit = true;
	 needNewAcldeny = true;
     }
   }

   if (pass1_found && !pass2_found) {
      result->add(aclID, aclID);
   }

   // pass 3
   for (rc = path.rclist.head(); rc; rc = path.rclist.next(rc)) {
      if (rc->mark) // done in pass 1 or 2, skip
	 continue;

      aclID = aspathMgr.newID();

      // print negative ones all in one access list
      sno = aclID;
      for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
	 if (ri->negated) {
	    CiscoXRConfig::printRE(delayedout, *ri->re, aclID, false);
	 }
      }
      // print positives ones each in its own access list, except first
      int first = 1;
      for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
	 if (!ri->negated) {
	    if (first)
	       first = 0;
	    else {
	       aclID = aspathMgr.newID();
 	    }
	    CiscoXRConfig::printRE(delayedout, *ri->re, aclID, true);
	 }
      }

      if (first) { // no positive one found
	 delayedout << "as-path-set as-set" << aclID << "-permit" << endl << " ios-regex '.*'" << endl;
      }

      result->add(sno, aclID);
   }

   return result;
}
   
inline void CiscoXRConfig::printCommunity(ostream &os, unsigned int i) {
   if (i == COMMUNITY_INTERNET)
      os << "internet";
   else if (i == COMMUNITY_NO_EXPORT)
      os << "no-export";
   else if (i == COMMUNITY_NO_EXPORT_SUBCONFED)
      os << "no-export-subconfed";
   else if (i == COMMUNITY_NO_ADVERTISE)
      os << "no-advertise";
   else {
      int high = i >> 16;
      int low  = i & 0xFFFF;
      if (high == 0xFFFF)
	 os << i;
      else
	 os << high << ":" << low;
   }
}

int CiscoXRConfig::printCommunitySetList(ostream &os, ItemList *args) {

 int aclID = communityMgr.newID();
 os << "community-set commset" << aclID << "-permit " << endl;
 CiscoXRConfig::printCommunityList(os, args);
 os << endl << "end-set" << endl;

return aclID;
}

void CiscoXRConfig::printCommunityList(ostream &os, ItemList *args) {

bool first = true;

   for (Item *cmnty = args->head(); cmnty; cmnty = args->next(cmnty)) {

      if (typeid(*cmnty) == typeid(ItemINT)) {
         if (first) {
           first = false;
         }  else {
           os << ", ";
         }
           
  
	 printCommunity(os, ((ItemINT *) cmnty)->i);
	 continue;
      }

      if (typeid(*cmnty) == typeid(ItemWORD)) {
	 if (!strcasecmp(((ItemWORD *)cmnty)->word, "no_advertise")) {
            if (first) first=false; else os << ", ";
	    printCommunity(os, COMMUNITY_NO_ADVERTISE);
	 } else if (!strcasecmp(((ItemWORD *)cmnty)->word, "no_export")) {
            if (first) first=false; else os << ", ";
	    printCommunity(os, COMMUNITY_NO_EXPORT);
	 } else if (!strcasecmp(((ItemWORD *)cmnty)->word,"no_export_subconfed")) {
            if (first) first=false; else os << ", ";
	    printCommunity(os, COMMUNITY_NO_EXPORT_SUBCONFED);
	 } else {
            if (first) first=false; else os << ", ";
	    printCommunity(os, COMMUNITY_INTERNET);
         }
	 continue;
      }

      if (typeid(*cmnty) == typeid(ItemList)) {
	 int high = ((ItemINT *) ((ItemList *) cmnty)->head())->i;
	 int low  = ((ItemINT *) ((ItemList *) cmnty)->tail())->i;
         if (first) first=false; else os << ", ";
	 printCommunity(os, (high << 16) + low);
	 continue;
      }
   }
}

void CiscoXRConfig::printActions(ostream &os, PolicyActionList *actions, ItemAFI *afi) {
#define UNIMPLEMENTED_METHOD \
   cerr << "Warning: unimplemented method " \
	<< actn->rp_attr->name << "." << actn->rp_method->name << endl

   PolicyAction *actn;
   for (actn = actions->head(); actn; actn = actions->next(actn)) {
      if (actn->rp_attr == dctn_rp_pref) {
	 if (actn->rp_method == dctn_rp_pref_set) {
	    int pref = ((ItemINT *) actn->args->head())->i;
            for (int j=0; j < ifcount; j++) os << " ";
	    os << "set local-preference " << (preferenceCeiling-pref) << endl;
	 } else 
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_nhop) {
	 if (actn->rp_method == dctn_rp_nhop_set) {

      if (afi->is_ipv4() && (typeid(*(actn->args->head())) == typeid(ItemIPV4))) {
        char buffer[32];
        IPAddr *ip = ((ItemIPV4 *) actn->args->head())->ipv4;
        ip->get_text(buffer);
        for (int j=0; j < ifcount; j++) os << " ";
        os << "set next-hop " << buffer << endl;
      } else if (afi->is_ipv6() && (typeid(*(actn->args->head())) == typeid(ItemIPV6))) {
        char buffer[50];
        IPv6Addr *ip = ((ItemIPV6 *) actn->args->head())->ipv6;
        ip->get_text(buffer);
        for (int j=0; j < ifcount; j++) os << " ";
        os << "set next-hop " << buffer << endl;
      } else {
        delayedout2 << "Warning: next-hop address family doesn't match protocol address family, ignoring next-hop..." << endl;
      }
	 } else 
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_dpa) {
//	 if (actn->rp_method == dctn_rp_dpa_set) {
//	    int dpa = ((ItemINT *) actn->args->head())->i;
//	    os << " set dpa " << dpa << "\n";
//	 } else 
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_med) {
	 if (actn->rp_method == dctn_rp_med_set) {
	    Item *item = actn->args->head();
            for (int j=0; j < ifcount; j++) os << " ";
	    if (typeid(*item) == typeid(ItemINT)) {
	       os << "set med " << ((ItemINT *) item)->i << endl;
	    } else
	       os << "set metric-type internal" << endl;
	 } else 
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_community) {
	 if (actn->rp_method == dctn_rp_community_setop) {
//            for (int j=0; j < ifcount; j++) delayedout2 << " ";
//	    os << "set community (";
            if (firstCommunityList) {
              firstCommunityList = false;
            } else {
              delayedout3 << ", ";
            } 
	    printCommunityList(delayedout3, (ItemList *) actn->args->head());
//	    os << ")" << endl;
	 } else if (actn->rp_method == dctn_rp_community_appendop) {
            for (int j=0; j < ifcount; j++) os << " ";
	    os << "set community (";
	    printCommunityList(os, (ItemList *) actn->args->head());
	    os << ") additive" << endl;
	 } else if (actn->rp_method == dctn_rp_community_append) {
            for (int j=0; j < ifcount; j++) os << " ";
	    os << "set community (";
	    printCommunityList(os, actn->args);
	    os << ") additive" << endl;
         } else if (actn->rp_method == dctn_rp_community_delete) {
            for (int j=0; j < ifcount; j++) os << " ";
            int commlist = printCommunitySetList(delayedout, actn->args);
            os << "delete community in commset-" << commlist << "-permit" << endl;
	 } else
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_aspath) {
	 if (actn->rp_method == dctn_rp_aspath_prepend) {
            for (int j=0; j < ifcount; j++) os << " ";
 	    os << " prepend as-path ";
            Item *plnd = actn->args->head();
	    os << ((ItemASNO *) plnd)->asno << " ";
            int times = 0; 
	    for (Item *plnd = actn->args->head(); plnd; plnd = actn->args->next(plnd))
               times++;
	    os << times << endl;
	 } else
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      cerr << "Warning: unimplemented attribute " 
	   << *actn->rp_attr << endl;
   }
   if (!firstCommunityList) {
     firstCommunityList = true;
     for (int j=0; j < ifcount; j++) delayedout2 << " ";
     delayedout2 << "set community (" << delayedout3.str() << ")" << endl;
     delayedout3.str("");
   }

}

int CiscoXRConfig::print(NormalExpression *ne, 
		       PolicyActionList *actn, 
		       int import_flag, 
		       ItemAFI *afi) {
   int last = 0;
   static ListOf2Ints empty_list(1);

   Debug(Channel(DBG_RTC_CISCO) << "# ne: " << *ne << endl);

   
   // modified by katie@ripe.net to handle any/not any expr
   // for prefix filters, cisco only

   if (ne->is_any() != NEITHER)	{
     cerr << "Warning: filter matches ANY/NOT ANY:" << mapName << endl;
     for (int j=0; j < ifcount; j++) delayedout2 << " " ;
     delayedout2 << " drop" << endl;
   }

//   if (ne->isEmpty() || ne->is_any())
   if (ne->isEmpty())
   {
	if (ne->singleton_flag == -1)
	{
		if (emptyLists)
		{
			// generate an empty filter; no afi means ipv4
      if (!afi || (afi && afi->is_ipv4())) {
        FilterOfPrefix *fltr = (FilterOfPrefix *) new SetOfPrefix;
        NormalTerm *nt = new NormalTerm;
        fltr->not_ = true;
        nt->prfx_set = *fltr;
        nt->make_universal(NormalTerm::PRFX);
        *ne += nt;
      } else {
        FilterOfIPv6Prefix *fltr = (FilterOfIPv6Prefix *) new SetOfIPv6Prefix;
        NormalTerm *nt = new NormalTerm;
        nt->ipv6_prfx_set = *fltr;
        nt->make_universal(NormalTerm::IPV6_PRFX);
        *ne += nt;
      }
		}
	}
	else {
  
	return last;
      }
   }

   if (ne->is_universal())
   {
	if (ne->singleton_flag == -1)
	{
		if (emptyLists)
		{
			// generate empty filter and negate to enable 'permit'
      if (!afi || (afi && afi->is_ipv4())) {
        FilterOfPrefix *fltr = (FilterOfPrefix *) new SetOfPrefix;
        fltr->not_ = true;
        ne->first()->prfx_set = *fltr;
      } else {
        FilterOfIPv6Prefix *fltr = (FilterOfIPv6Prefix *) new SetOfIPv6Prefix;
        fltr->not_ = true;
        ne->first()->ipv6_prfx_set = *fltr;
      }
		}
	}
	else
		last = 1;
   }

//if (ne->isEmpty())
//  return last;
//
//if (ne->is_universal())
//  last = 1;
	
   bool needToPrintNoRouteMap = false;
   if (strcmp(mapName, lastMapName)) {
      strcpy(lastMapName, mapName);
      needToPrintNoRouteMap = true;
      routeMapID = mapNumbersStartAt;
   }
   routeMapGenerated = true;
   prefixListGenerated = false;

   for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) {
      // only ipv4 OR ipv6 list gets printed in every loop
      ListOf2Ints *prfx_acls   = printRoutes(nt->prfx_set);
      ListOf2Ints *ipv6_prfx_acls   = printRoutes(nt->ipv6_prfx_set);
      ListOf2Ints *aspath_acls = printASPaths(nt->as_path);
      ListOf2Ints *comm_acls   = printCommunities(nt->community);

  delayedout << endl;
  BUFFER = delayedout.str();
  cout << BUFFER << endl;            
  delayedout.str("");
//  delayedout2 << endl;
//  BUFFER = delayedout2.str();
//  cout << BUFFER << endl;            
//  delayedout2.str("");

      if (prfx_acls && !ipv6_prfx_acls) {
	      prefixListGenerated = true;
        ipv6_prfx_acls = &empty_list;
        distributeListNo = prfx_acls->head()->start;
      } else if (!prfx_acls && ipv6_prfx_acls) {
	      prefixListGenerated = true;
        prfx_acls = &empty_list;
        distributeListNo = ipv6_prfx_acls->head()->start;
      } else {
        prfx_acls = &empty_list;
        ipv6_prfx_acls = &empty_list;
      }
      
      if (!aspath_acls)
	      aspath_acls = &empty_list;
      if (!comm_acls)
	      comm_acls = &empty_list;
      
      ListNodeOf2Ints *asp, *cmp, *prp; 
      int i;

      if (printRouteMap) {
	      if (needToPrintNoRouteMap) {
	         needToPrintNoRouteMap = false;
	      }

	    for (asp = aspath_acls->head(); asp; asp = aspath_acls->next(asp)) {
	      for (cmp = comm_acls->head(); cmp; cmp = comm_acls->next(cmp)) {
	        routeMapID += mapIncrements;
	        for (i = asp->start; i <= asp->end; i++) {
                  for (int j=0; j < ifcount; j++) delayedout2 << " " ;
      		    delayedout2 << "if as-path in as-set" << i << "-permit and not as-path in as-set" << i << "-deny then" << endl;
                    ifcount++;
                }
	        for (i = cmp->start; i <= cmp->end; i++) {
                  for (int j=0; j < ifcount; j++) delayedout2 << " ";
	          delayedout2 << "if community matches-any commset" << i << "-permit and not community matches-any commset" <<i << "-deny then" << endl; 
                  ifcount++;
               }
	       if (!import_flag || forcedInboundMatchIP) {
		       for (prp = prfx_acls->head(); prp; prp =prfx_acls->next(prp)) {
		         for (i = prp->start; i <= prp->end; i++) {
			         if (usePrefixLists) {
                                   for (int j=0; j < ifcount; j++) delayedout2 << " ";
			           delayedout2 << "if destination in pl" 
				         << i <<"-permit and not destination in pl" << i << "-deny then" << endl;
                                   ifcount++; 
			         } else {
                                   for (int j=0; j < ifcount; j++) delayedout2 << " ";
  			           delayedout2 << " match ip address " << i << endl;
                                 }

               }
           }
           for (prp = ipv6_prfx_acls->head(); prp; prp =ipv6_prfx_acls->next(prp)) {
             for (i = prp->start; i <= prp->end; i++) {
               if (usePrefixLists) {
                 for (int j=0; j < ifcount; j++) delayedout2 << " ";
                 delayedout2 << "if destination in " 
	         << xripv6_pl << i <<"-permit and not destination in " << xripv6_pl << i << "-deny then" << endl;
                 ifcount++;
               } else  {
                  for (int j=0; j < ifcount; j++) delayedout2 << " ";
                  delayedout2 << " match ipv6 address " << xripv6_acl << i << endl;
               }
               }
           }
           CiscoXRConfig::printActions(delayedout2, actn, afi);
           for (int j=0; j < ifcount; j++) delayedout2 << " ";
           delayedout2 << "done" << endl;
           for (int l=0; l <= ifcount+1; l++) {
             for (int k=0; k < ifcount-1; k++) delayedout2 << " ";
               if (ifcount > 0) {
                 delayedout2 << "endif" << endl;
                 ifcount--;
               }
             }
           }
        } 
      }
    }
  }
  return last;
}

// Reimplemented to handle different afi's
bool CiscoXRConfig::printNeighbor(int import, 
				ASt asno, 
				ASt peerAS, 
				char *neighbor, 
				bool peerGroup, 
				ItemAFI *peer_afi, 
				ItemAFI *filter_afi) {
   bool afi_activate = false;

   if (! printRouteMap)
      return false;

   if (!peerGroup && (!filter_afi->is_default() || !peer_afi->is_default()))
      afi_activate = true;

   const char *indent = (afi_activate) ? " " : "";
   const char *direction = (import == IMPORT) ? "in" : "out";

   if (!routeMapGenerated && (!import || ! prefixListGenerated)) {
      // yes we are conciously printing an "not any" policy
      cout << "!" << endl << "route-policy " << mapName << endl; // << " deny " << mapNumbersStartAt << endl;
      cout << " drop" << endl;
      cout << "end-policy" << endl;
      routeMapGenerated = true;
   }

   // create bgp process 
   cout << "!" <<endl;
   cout << "router bgp " << asno << endl;
   cout << "!" <<endl;

   if (!peerGroup) {
     cout << " neighbor "   << neighbor << endl;
     cout << indent << " remote-as " << peerAS << endl;
   }


   if (peerGroup) {
     cout << indent << "neighbor-group " << neighbor << endl;
   }
//   if (routeMapGenerated) {
       if (strcmp(filter_afi->name(),"ipv4.unicast") == 0) { 
         cout << "  address-family " << "ipv4 unicast" << endl; 
       } else if (strcmp(filter_afi->name(),"ipv6.unicast") == 0) {
         cout << "  address-family " << "ipv6 unicast" << endl; 
       } else if (strcmp(filter_afi->name(),"ipv4.multicast") == 0) {
         cout << "  address-family " << "ipv4 multicast" << endl; 
       } else if (strcmp(filter_afi->name(),"ipv6.multicast") == 0) {
       cout << "  address-family " << "ipv6 multicast" << endl; 
       }
     cout   << indent << "   route-policy " << mapName << " " << direction << endl;
 //  }

 
   
   if (import && ! forcedInboundMatchIP && prefixListGenerated)
      if (usePrefixLists)
	 cout << indent << " neighbor " << neighbor
	      << " prefix-list pl" << distributeListNo << " in" << endl;
      else
	 cout << indent << " neighbor " << neighbor
	      << " distribute-list " << distributeListNo << " in" << endl;

   if (afi_activate)
      cout << " exit" << endl; // exit address-family config mode

   if (!peerGroup)
      cout << "exit" << endl ; // exit neighbor config mode explicitly if not peer group

   return true;
}

void CiscoXRConfig::exportP(ASt asno, MPPrefix *addr, 
			 ASt peerAS, MPPrefix *peer_addr) {

   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);

   if (!autnum) {
      cerr << "Error: no object for AS" << asno << endl;
      return;
    }

   // get matching export & mp-export attributes
   AutNumSelector<AttrExport> itr(autnum, "export", 
                                  NULL, peerAS, peer_addr, addr);
   AutNumSelector<AttrExport> itr1(autnum, "mp-export",
          NULL, peerAS, peer_addr, addr);

   List<FilterAction> *common_list = itr.get_fa_list();
   common_list->splice(*(itr1.get_fa_list()));

   FilterAction *fa = common_list->head();
   if (! fa) {
     printPolicyWarning(asno, addr, peerAS, peer_addr, "export/mp-export");
     return;
   }
   ItemList *afi_list = itr.get_afi_list();
   afi_list->merge(*(itr1.get_afi_list()));

   if (afi_list->isEmpty()) {
     cout << "Warning: No AFI resulted from policy" << endl;
   }

   NormalExpression *ne;
   NormalExpression done;
   int last = 0;

   for (Item *afi = afi_list->head(); afi; afi = afi_list->next(afi)) {
     // Made asno part of the map name if it's not changed by users
     sprintf(mapName, mapNameFormat, peerAS, mapCount++);

     delayedout2 << "!" << endl << "route-policy " << mapName << endl;
     routeMapGenerated = true;

     for (fa = common_list->head(); fa && !last; fa = common_list->next(fa)) {
       ne = NormalExpression::evaluate(new FilterAFI((ItemAFI *) afi->dup(), fa->filter), peerAS);

       if (eliminateDupMapParts) {
               NormalExpression ne2(*ne);
         NormalExpression done2(done);
         done2.do_not();
         ne2.do_and(done2);

           if (! ne2.isEmpty()) {
           last = print(ne, fa->action, EXPORT, (ItemAFI *) afi);
         }

               done.do_or(*ne);
       } else {
                 last = print(ne, fa->action, EXPORT, (ItemAFI *) afi);
       }

       delete ne;
     }

  BUFFER = delayedout2.str();
  cout << BUFFER;            
  delayedout2.str("");
     cout << "end-policy" << endl;
     ifcount = 0;

     // peer_afi should be afi of peer IPs, filter_afi is the one specified in filter
     ItemAFI *peer_afi = new ItemAFI(peer_addr->get_afi());
     printNeighbor(EXPORT, asno, peerAS, peer_addr->get_ip_text(), false, (ItemAFI *) peer_afi, (ItemAFI *) afi);

     routeMapGenerated = false;
     prefixListGenerated = false;
  }

  delayedout << endl;
  BUFFER = delayedout.str();
  cout << BUFFER << endl;            
  delayedout.str("");
}

void CiscoXRConfig::importP(ASt asno, MPPrefix *addr, 
       ASt peerAS, MPPrefix *peer_addr) {

   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);

   if (!autnum) {
      cerr << "Error: no object for AS" << asno << endl;
      return;
    }

   // get matching import & mp-import attributes
   AutNumSelector<AttrImport> itr(autnum, "import", 
				  NULL, peerAS, peer_addr, addr);
   AutNumSelector<AttrImport> itr1(autnum, "mp-import",
          NULL, peerAS, peer_addr, addr);

   List<FilterAction> *common_list = itr.get_fa_list();
   common_list->splice(*(itr1.get_fa_list()));

   FilterAction *fa = common_list->head();
   if (! fa) {
     printPolicyWarning(asno, addr, peerAS, peer_addr, "import/mp-import");
     return;
   }
   ItemList *afi_list = itr.get_afi_list();
   afi_list->merge(*(itr1.get_afi_list()));

   if (afi_list->isEmpty()) {
     cout << "Warning: No AFI resulted from policy" << endl;
   }


   NormalExpression *ne;
   NormalExpression done;
   int last = 0;

   for (Item *afi = afi_list->head(); afi; afi = afi_list->next(afi)) {
     // Made asno part of the map name if it's not changed by users
     sprintf(mapName, mapNameFormat, peerAS, mapCount++);

     delayedout2 << "exit" << endl;
     delayedout2 << "!" << endl << "route-policy " << mapName << endl;
     routeMapGenerated = true;

     for (fa = common_list->head(); fa && !last; fa = common_list->next(fa)) {
       ne = NormalExpression::evaluate(new FilterAFI((ItemAFI *) afi->dup(), fa->filter), peerAS);

       if (eliminateDupMapParts) {
	       NormalExpression ne2(*ne);
      	 NormalExpression done2(done);
      	 done2.do_not();
      	 ne2.do_and(done2);

     	   if (! ne2.isEmpty()) {
      	   last = print(ne, fa->action, IMPORT, (ItemAFI *) afi);
         }

	       done.do_or(*ne);
       } else {
	         last = print(ne, fa->action, IMPORT, (ItemAFI *) afi);
       }

       delete ne;
     }
  BUFFER = delayedout2.str();
  cout << BUFFER;            
  delayedout2.str("");
     cout << "end-policy" << endl;

     // peer_afi should be afi of peer IPs, filter_afi is the one specified in filter
     ItemAFI *peer_afi = new ItemAFI(peer_addr->get_afi());
     printNeighbor(IMPORT, asno, peerAS, peer_addr->get_ip_text(), false, (ItemAFI *) peer_afi, (ItemAFI *) afi);

     routeMapGenerated = false;
     prefixListGenerated = false;
  }

  delayedout << endl;
  BUFFER = delayedout.str();
  cout << BUFFER << endl;            
  delayedout.str("");

}

void CiscoXRConfig::static2bgp(ASt asno, MPPrefix *addr) {

   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);
   
   if (!autnum) {
      cerr << "Error: no object for AS" << asno << endl;
      return;
   }

   // get matching import attributes
   AutNumSelector<AttrImport> itr(autnum, "import", 
				  NULL, asno, addr, addr, "STATIC");
   AutNumSelector<AttrImport> itr1(autnum, "mp-import",
          NULL, asno, addr, addr, "STATIC");

   List<FilterAction> *common_list = itr.get_fa_list();
   common_list->splice(*(itr1.get_fa_list()));

   const FilterAction *fa = common_list->head();
   if (! fa)
      cerr << "Warning: AS" << asno 
	   << " has no static2bgp policy for AS" << asno << endl;

   NormalExpression *ne;
   NormalExpression done;
   int last = 0;

   ItemList *afi_list = itr.get_afi_list();
   afi_list->merge(*(itr1.get_afi_list()));

   for (Item *afi = afi_list->head(); afi; afi = afi_list->next(afi)) {
     // Made asno part of the map name if it's not changed by users
     sprintf(mapName, mapNameFormat, asno, mapCount++);

     for (; fa; fa = common_list->next(fa)) {
        ne = NormalExpression::evaluate(new FilterAFI((ItemAFI *) afi->dup(), fa->filter), asno);

      if (eliminateDupMapParts) {
	      NormalExpression ne2(*ne);
	      NormalExpression done2(done);
      	done2.do_not();
	      ne2.do_and(done2);

	      if (! ne2.isEmpty())
	        last = print(ne, fa->action, EXPORT, (ItemAFI *) afi);

	      done.do_or(*ne);
      } else
	      last = print(ne, fa->action, EXPORT, (ItemAFI *) afi);

      delete ne;
    }
    cout << "end-policy" << endl;
    cout << "router bgp " << asno << endl << "!" << endl;
    bool afi_activate = false;
    if (!((ItemAFI *) afi)->is_default()) 
      afi_activate = true;
    const char *indent = (afi_activate) ? " " : "";
    if (routeMapGenerated) {
      if (afi_activate)
         cout << " address-family " << *afi << endl;  
      cout << indent << " redistribute static route-map " << mapName << endl;
      if (afi_activate)
         cout << " exit" << endl << "!" << endl;
    }
    routeMapGenerated = false;
    prefixListGenerated = false;

   }
}

void CiscoXRConfig::deflt(ASt asno, ASt peerAS) {
   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);

   if (!autnum) {
      cerr << "Error: no object for AS" << asno << endl;
      return;
    }

   // get matching default attributes
   AutNumDefaultIterator itr(autnum, peerAS);
   const AttrDefault *deflt = itr.first();
   if (! deflt) {
      cerr << "Warning: AS" << asno 
           << " has no default policy for AS" << peerAS << endl;
      return;
   }

   cout << "!" << endl;

   NormalExpression *ne;
   int last = 0;
   for (; deflt && !last; deflt = itr.next()) {
      ne = NormalExpression::evaluate(deflt->filter, peerAS);

      if (ne->is_universal()) {
         cout << "ip default-network 0.0.0.0" << endl;
      } else {
         RadixSet::SortedPrefixIterator itr(&(ne->first()->prfx_set.members));
         u_int addr;
         u_int leng;
         char buffer[64];

         for (bool ok = itr.first(addr, leng);
              ok;
              ok = itr.next(addr, leng)) {
            cout << "ip default-network " 
                 << int2quad(buffer, addr)
                 << endl;
         }
      }
   }
}

void CiscoXRConfig::networks(ASt asno) {
   const MPPrefixRanges *nets = irr->expandAS(asno);
   MPPrefixRanges::const_iterator p;
   static char buffer[128];

   if (nets) {
     cout << "!" << endl;
     for (p = nets->begin(); p != nets->end(); ++p) {
       if (p->ipv4) {
         cout << "network " << p->ipv4->get_ip_text()
            << " mask " << int2quad(buffer, p->ipv4->get_mask()) 
            << endl;
       }
     }
   }
}

void CiscoXRConfig::IPv6networks(ASt asno) {
   const MPPrefixRanges *nets = irr->expandAS(asno);
   MPPrefixRanges::const_iterator p;

   if (nets) {
     cout << "!" << endl;
     bool afi_activate = true;
     for (p = nets->begin(); p != nets->end(); ++p) {
       if (p->ipv6) {
         if (afi_activate) {
           cout << "address-family ipv6" << endl; /// unicast/multicast?
           afi_activate = false;
         }
         cout << " network " << p->ipv6->get_ip_text()
              << " mask " << p->ipv6->get_mask()
              << endl;
       }
     }
     if (!afi_activate)
        cout << "exit" << endl;
   }
}

int CiscoXRConfig::printPacketFilter(SetOfPrefix &set) {
   RadixSet::SortedPrefixIterator itr(&set.members);
   u_int addr;
   u_int leng;
   char buffer[64];
   bool allow_flag = ! set.negated();
   ListOf2Ints *result;

   // check to see if we already printed an identical access list, 
   if (useAclCaches && (result = pktFilterMgr.search(set)))
      return result->head()->start;

   result = pktFilterMgr.add(set);
   int aclID = prefixMgr.newID();
   result->add(aclID, aclID);

   cout << "!" << endl << "no access-list " << aclID << endl;

   for (bool ok = itr.first(addr, leng); ok; ok = itr.next(addr, leng)) {
      if (!addr && !leng) // skip 0.0.0.0/0
	 continue;

      cout << "access-list " << aclID << returnPermitOrDeny(allow_flag) << "ip ";

      cout << int2quad(buffer, addr) << " ";
      cout << int2quad(buffer, ~masks[leng]) << " any " << endl;
   }

   if (set.universal()) // handle 0.0.0.0/0
     allow_flag = false;

   cout << "access-list " << aclID << returnPermitOrDeny(!allow_flag) << "ip any any" << endl;

   return aclID;
}

int CiscoXRConfig::printPacketFilter(SetOfIPv6Prefix &set) {
   IPv6RadixSet::SortedPrefixIterator itr(&set.members);
   ipv6_addr_t addr;
   u_int leng;
   ipv6_addr_t rngs;
   char buffer[256];
   bool allow_flag = ! set.negated();
   ListOf2Ints *result;

   // check to see if we already printed an identical access list, 
   if (useAclCaches && (result = ipv6pktFilterMgr.search(set)))
      return result->head()->start;

   result = ipv6pktFilterMgr.add(set);
   int aclID = ipv6prefixMgr.newID();
   result->add(aclID, aclID);

   cout << "!" << endl << "no ipv6 access-list " << xripv6_acl << aclID << endl;

   for (bool ok = itr.first(addr, leng); ok; ok = itr.next(addr, leng)) {
      if (!addr && !leng)
        continue;

      cout << "ipv6 access-list " << xripv6_acl << aclID << returnPermitOrDeny(allow_flag) << "ip ";

      cout << ipv62hex(&addr, buffer) << " ";
      cout << ipv62hex(&(addr.getmask(leng)), buffer) << " any " << endl;
   }

   if (set.universal()) // handle ::/0
     allow_flag = false;

   cout << "ipv6 access-list " << xripv6_acl << aclID << returnPermitOrDeny(!allow_flag) << "ip any any" << endl;

   return aclID;
}


void CiscoXRConfig::inboundPacketFilter(char *ifname, ASt as, MPPrefix* addr, 
			       ASt peerAS, MPPrefix* peerAddr) {
   int import = IMPORT;
   const char *rpsltag = (import == IMPORT) ? "import" : "export";
   const char *mp_rpsltag = (import == IMPORT) ? "mp-import" : "mp-export";
   const char *direction = (import == IMPORT) ? " in" : " out";

   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(as);

   if (!autnum) {
      cerr << "Error: no object for AS" << as << endl;
      return;
    }

   // get matching import & mp-import attributes
   AutNumSelector<AttrImport> itr(autnum, rpsltag, NULL, peerAS, peerAddr, addr);
   AutNumSelector<AttrImport> itr1(autnum, mp_rpsltag, NULL, peerAS, peerAddr, addr);

   List<FilterAction> *common_list = itr.get_fa_list();
   common_list->splice(*(itr1.get_fa_list()));
   
   FilterAction *fa = common_list->head();
   if (! fa) {
     printPolicyWarning(as, addr, peerAS, peerAddr, mp_rpsltag);
     return;
   }
   ItemList *afi_list = itr.get_afi_list();
   afi_list->merge(*(itr1.get_afi_list()));
   int last = 0;
   SetOfPrefix set;
   SetOfIPv6Prefix ipv6_set;

   for (fa = common_list->head(); fa && !last; fa = common_list->next(fa)) {
     NormalExpression *ne = NormalExpression::evaluate(new FilterAFI(afi_list,fa->filter), peerAS);
     for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) { //either v4 or v6
       if (nt->ipv6_prfx_set.universal())
         set |= nt->prfx_set;
       else 
         ipv6_set |= nt->ipv6_prfx_set;
     }
   }
  
   int aclid;
   int ipv6_aclid;

   if (!set.isEmpty())
      aclid = printPacketFilter(set);
   if (!ipv6_set.isEmpty())
      ipv6_aclid = printPacketFilter(ipv6_set);

   if (!set.isEmpty())
     cout << "\n!\ninterface " << ifname << "\n ip access-group " << aclid << direction << endl;

   if (!ipv6_set.isEmpty()) {
     cout << "address family ipv6" << endl;
     cout << " interface " << ifname << "\n ipv6 access-group " << xripv6_acl << ipv6_aclid << direction << endl;
     cout << "exit" << endl;
   }
}

void CiscoXRConfig::outboundPacketFilter(char *ifname, ASt as, MPPrefix* addr, 
				       ASt peerAS, MPPrefix* peerAddr) {
   int import = EXPORT;
   const char *rpsltag = (import == IMPORT) ? "import" : "export";
   const char *mp_rpsltag = (import == IMPORT) ? "mp-import" : "mp-export";
   const char *direction = (import == IMPORT) ? " in" : " out";

   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(as);

   if (!autnum) {
      cerr << "Error: no object for AS" << as << endl;
      return;
    }

   // get matching import & mp-import attributes
   AutNumSelector<AttrExport> itr(autnum, rpsltag, NULL, peerAS, peerAddr, addr);
   AutNumSelector<AttrExport> itr1(autnum, mp_rpsltag, NULL, peerAS, peerAddr, addr);

   List<FilterAction> *common_list = itr.get_fa_list();
   common_list->splice(*(itr1.get_fa_list()));
   
   FilterAction *fa = common_list->head();
   if (! fa) {
     printPolicyWarning(as, addr, peerAS, peerAddr, mp_rpsltag);
     return;
   }
   ItemList *afi_list = itr.get_afi_list();
   afi_list->merge(*(itr1.get_afi_list()));
   int last = 0;
   SetOfPrefix set;
   SetOfIPv6Prefix ipv6_set;

   for (fa = common_list->head(); fa && !last; fa = common_list->next(fa)) {
     NormalExpression *ne = NormalExpression::evaluate(new FilterAFI(afi_list,fa->filter), peerAS);
     for (NormalTerm *nt = ne->first(); nt; nt = ne->next()) { //either v4 or v6
       if (nt->ipv6_prfx_set.universal())
         set |= nt->prfx_set;
       else 
         ipv6_set |= nt->ipv6_prfx_set;
     }
   }
  
   int aclid;
   int ipv6_aclid;

   if (!set.isEmpty())
      aclid = printPacketFilter(set);
   if (!ipv6_set.isEmpty())
      ipv6_aclid = printPacketFilter(ipv6_set);

   if (!set.isEmpty())
     cout << "\n!\ninterface " << ifname << "\n ip access-group " << aclid << direction << endl;

   if (!ipv6_set.isEmpty()) {
     cout << "address family ipv6" << endl;
     cout << " interface " << ifname << "\n ipv6 access-group " << xripv6_acl << ipv6_aclid << direction << endl;
     cout << "exit" << endl;
   }
}

void CiscoXRConfig::importGroup(ASt asno, char * pset) {
   int import = IMPORT;
   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);
   const char *rpsltag = (import == IMPORT) ? "import" : "export";
   const char *mp_rpsltag = (import == IMPORT) ? "mp-import" : "mp-export";

   if (!autnum) {
      cerr << "Error: no object for AS" << asno << endl;
      return;
    }

   SymID psetID = symbols.symID(pset);
   // get matching import attributes
   AutNumSelector<AttrImport> itr(autnum, rpsltag, psetID, ~0, NULL, NULL);
   AutNumSelector<AttrImport> itr1(autnum, mp_rpsltag, psetID, ~0, NULL, NULL);

   List<FilterAction> *common_list = itr.get_fa_list();
   common_list->splice(*(itr1.get_fa_list()));

   FilterAction *fa = common_list->head();
   if (! fa) {
     cerr << "Warning: AS" << asno << " has no " << rpsltag << "/" << mp_rpsltag
          << " policy for " << pset << endl;
     return;
   }

   ItemList *afi_list = itr.get_afi_list();
   afi_list->merge(*(itr1.get_afi_list()));

   NormalExpression *ne;
   int last = 0;

   for (Item *afi = afi_list->head(); afi; afi = afi_list->next(afi)) {
     // Made asno part of the map name if it's not changed by users
     sprintf(mapName, mapNameFormat, asno, mapCount++);
     for (fa = common_list->head(); fa && !last; fa = common_list->next(fa)) {
       ne = NormalExpression::evaluate(new FilterAFI((ItemAFI *) afi->dup(), fa->filter), ~0);
       last = print(ne, fa->action, import, (ItemAFI *) afi);;
       delete ne;
     }
   }  

   cout << "end-policy\n";

   // afi was ignored here
   printNeighbor(import, asno, 0, pset, true, NULL, (ItemAFI *) afi_list->head());

   const PeeringSet *prngSet = irr->getPeeringSet(psetID);
   if (prngSet) {
     AutNumPeeringIterator aut_itr(autnum);

     for (AttrIterator<AttrPeering> itr(prngSet, "peering"); itr; itr++)
       if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
            && typeid(*itr()->peering->peerASes) == typeid(FilterASNO))
         cout << " neighbor " << *itr()->peering->peerRtrs  
              << "\n  remote-as " << ((FilterASNO *)itr()->peering->peerASes)->asno << endl;
     for (AttrIterator<AttrPeering> itr(prngSet, "mp-peering"); itr; itr++)
       if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
            && typeid(*itr()->peering->peerASes) == typeid(FilterASNO))
         cout << " neighbor " << *itr()->peering->peerRtrs  
              << "\n  remote-as " << ((FilterASNO *)itr()->peering->peerASes)->asno << endl;

     for (Item *afi = afi_list->head(); afi; afi = afi_list->next(afi)) {
       bool afi_activate = false;
       ItemAFI *iafi = (ItemAFI *)afi;
       if (!iafi->is_default())
         afi_activate = true;
       const char *indent = (afi_activate) ? " " : "";
       if (afi_activate)
         cout << " address-family " << iafi->name_afi() << " " << iafi->name_safi() << endl;

       for (AttrIterator<AttrPeering> itr(prngSet, "peering"); itr; itr++) {
         if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
              && typeid(*itr()->peering->peerASes) == typeid(FilterASNO)) {
//           if (afi_activate)
//             cout << indent << " neighbor " << *itr()->peering->peerRtrs << " activate\n";
           cout << indent << " neighbor " << *itr()->peering->peerRtrs  << "\n  use neighbor-group " << pset << endl << "!" << endl;
         }
       }
       for (AttrIterator<AttrPeering> itr(prngSet, "mp-peering"); itr; itr++) {
         if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
              && typeid(*itr()->peering->peerASes) == typeid(FilterASNO)) {
//           if (afi_activate)
//             cout << indent << " neighbor " << *itr()->peering->peerRtrs << " activate\n";
           cout << indent << " neighbor " << *itr()->peering->peerRtrs  << "\n  use neighbor-group " << pset << endl << "!" << endl;
         }
       }

       if (afi_activate)
          cout << " exit\n!\n";
     }
     cout << "exit\n!\n";
   }

   routeMapGenerated = false;
   prefixListGenerated = false;
} 

void CiscoXRConfig::exportGroup(ASt asno, char * pset) {
   int import = EXPORT;
   // get the aut-num object
   const AutNum *autnum = irr->getAutNum(asno);
   const char *rpsltag = (import == IMPORT) ? "import" : "export";
   const char *mp_rpsltag = (import == IMPORT) ? "mp-import" : "mp-export";

   if (!autnum) {
      cerr << "Error: no object for AS" << asno << endl;
      return;
    }

   SymID psetID = symbols.symID(pset);
   // get matching import attributes
   AutNumSelector<AttrExport> itr(autnum, rpsltag, psetID, ~0, NULL, NULL);
   AutNumSelector<AttrExport> itr1(autnum, mp_rpsltag, psetID, ~0, NULL, NULL);

   List<FilterAction> *common_list = itr.get_fa_list();
   common_list->splice(*(itr1.get_fa_list()));

   FilterAction *fa = common_list->head();
   if (! fa) {
     cerr << "Warning: AS" << asno << " has no " << rpsltag << "/" << mp_rpsltag
          << " policy for " << pset << endl;
     return;
   }

   ItemList *afi_list = itr.get_afi_list();
   afi_list->merge(*(itr1.get_afi_list()));

   NormalExpression *ne;
   int last = 0;


   for (Item *afi = afi_list->head(); afi; afi = afi_list->next(afi)) {
     // Made asno part of the map name if it's not changed by users
     sprintf(mapName, mapNameFormat, asno, mapCount++);
     delayedout2 << "!\nroute-policy " << mapName <<"\n";
     routeMapGenerated = true;
     for (fa = common_list->head(); fa && !last; fa = common_list->next(fa)) {
       ne = NormalExpression::evaluate(new FilterAFI((ItemAFI *) afi->dup(), fa->filter), ~0);
       last = print(ne, fa->action, import, (ItemAFI *) afi);;
       delete ne;
     }
   }

  BUFFER = delayedout2.str();
  cout << BUFFER;            
  delayedout2.str("");
  cout << "end-policy" << endl;

   // afi was ignored here
   printNeighbor(import, asno, 0, pset, true, NULL, (ItemAFI *) afi_list->head());

   const PeeringSet *prngSet = irr->getPeeringSet(psetID);
   if (prngSet) {
     AutNumPeeringIterator aut_itr(autnum);

     for (AttrIterator<AttrPeering> itr(prngSet, "peering"); itr; itr++)
       if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
            && typeid(*itr()->peering->peerASes) == typeid(FilterASNO)) {
         cout << "address-family ipv4 unicast" << endl;
         cout << " neighbor " << *itr()->peering->peerRtrs  
              << "\n  remote-as " << ((FilterASNO *)itr()->peering->peerASes)->asno << endl;
       }
     for (AttrIterator<AttrPeering> itr(prngSet, "mp-peering"); itr; itr++)
       if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
            && typeid(*itr()->peering->peerASes) == typeid(FilterASNO)) {
         cout << "address-family ipv6 unicast" << endl;
         cout << " neighbor " << *itr()->peering->peerRtrs  
              << "\n  remote-as " << ((FilterASNO *)itr()->peering->peerASes)->asno << endl;
       }
     for (Item *afi = afi_list->head(); afi; afi = afi_list->next(afi)) {
       bool afi_activate = false;
       ItemAFI *iafi = (ItemAFI *)afi;
       if (!iafi->is_default())
         afi_activate = true;
       const char *indent = (afi_activate) ? " " : "";
       if (afi_activate)
         cout << " address-family " << iafi->name_afi() << " " << iafi->name_safi() << endl;

       for (AttrIterator<AttrPeering> itr(prngSet, "peering"); itr; itr++) {
         if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
              && typeid(*itr()->peering->peerASes) == typeid(FilterASNO)) {
//           if (afi_activate)
//             cout << indent << " neighbor " << *itr()->peering->peerRtrs << " activate\n";
           cout << indent << " neighbor " << *itr()->peering->peerRtrs  << "\n  use neighbor-group " << pset << endl << "!" << endl;
         }
       }
       for (AttrIterator<AttrPeering> itr(prngSet, "mp-peering"); itr; itr++) {
         if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
              && typeid(*itr()->peering->peerASes) == typeid(FilterASNO)) {
//           if (afi_activate)
//             cout << indent << " neighbor " << *itr()->peering->peerRtrs << " activate\n";
           cout << indent << " neighbor " << *itr()->peering->peerRtrs  << "\n  use neighbor-group " << pset << endl << "!" << endl;
         }
       }

       if (afi_activate)
          cout << " exit\n!\n";
     }
     cout << "exit\n!\n";
   }
   
   routeMapGenerated = false;
   prefixListGenerated = false;
} 
