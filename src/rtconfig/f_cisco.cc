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

#include "config.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include "normalform/NE.hh"
#include "rtconfig.hh"
#include "f_cisco.hh"
#include "rpsl/schema.hh"

using namespace std;

#define EXPORT 0
#define IMPORT 1

char CiscoConfig::mapName[80] = "MyMap"; 
char CiscoConfig::mapNameFormat[80] = "MyMap_%d_%d"; 
bool CiscoConfig::forcedInboundMatchIP = true;
bool CiscoConfig::useAclCaches = true;
bool CiscoConfig::compressAcls = true;
bool CiscoConfig::usePrefixLists = false;
bool CiscoConfig::eliminateDupMapParts = false;
bool CiscoConfig::forceTilda = false;
bool CiscoConfig::emptyLists = false;
int  CiscoConfig::mapIncrements = 1;
int  CiscoConfig::mapCount = 1;
int  CiscoConfig::mapNumbersStartAt = 1;
bool CiscoConfig::firstCommunityList = true;
bool CiscoConfig::printRouteMap = true;
ostringstream delayoutput;

//////////////////////////// caches ////////////////////////////////

AccessListManager<regexp_nf>         aspathMgr(50);
AccessListManager<SetOfPrefix>       prefixMgr(150);
AccessListManager<SetOfIPv6Prefix>   ipv6prefixMgr(150);
AccessListManager<SetOfPrefix>       pktFilterMgr(150);
AccessListManager<SetOfIPv6Prefix>   ipv6pktFilterMgr(150);
AccessListManager<FilterOfCommunity> communityMgr(50);

/// access-list and prefix-list names
char ipv6_acl[6] = "ipv6-";
char ipv6_pl[8] = "ipv6-pl";

unsigned int ones(unsigned char from, unsigned char to)
{
  unsigned int result = 0;
  for (int i = 32 - to; i <= 32 - from; i++) result |= (1L << i);
  return result;
}

const char *CiscoConfig::returnPermitOrDeny(bool allow_flag) {
   if (allow_flag)
      return " permit ";
   else
      return " deny ";
}
                  
ListOf2Ints *CiscoConfig::printRoutes(SetOfIPv6Prefix& nets) {
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

   // clear this access list
   cout << "!\nno ipv6 access-list " << ipv6_acl << aclID << "\n";

   // no compressed acls

/*   if (compressAcls) {
      IPv6RadixSet::SortedPrefixRangeIterator itr(&nets.members);
      ipv6_addt_t addr;
      u_int leng;
      u_int start;
      u_int end;
      char buffer[256];

      for (bool ok = itr.first(addr, leng, start, end);
	   ok;
	   ok = itr.next(addr, leng, start, end)) {
	   cout << "ipv6 access-list " << aclID;
	    if (allow_flag)
	      cout << " permit ";
	    else 
	      cout << " deny ";

	    cout << ipv62hex(&addr, buffer) << "   ";
	    cout << ipv62hex(buffer, ones(leng + 1, end)) << "   ";
	      << "\n";
      }
   } else {
   */
      IPv6RadixSet::SortedPrefixIterator itr(&nets.members);
      ipv6_addr_t addr;
      u_int leng;
      char buffer[256];

      for (bool ok = itr.first(addr, leng);
	   ok;
	   ok = itr.next(addr, leng)) {
	 cout << "ipv6 access-list " << ipv6_acl << aclID << returnPermitOrDeny(allow_flag);
	 cout << ipv62hex(&addr, buffer) << "/" << leng << " any\n";
      }
  // }

   // terminate the access list
   cout << "ipv6 access-list " << ipv6_acl << aclID << returnPermitOrDeny(!allow_flag) << "any any" << endl;

   return result;
}

ListOf2Ints *CiscoConfig::printRoutes(SetOfPrefix& nets) {
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

   // clear this access list
   cout << "!\nno access-list " << aclID << "\n";

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
	 cout << "access-list " << aclID << returnPermitOrDeny(allow_flag) << "ip ";

	 /* need to look at WeeSan's code */
	 cout << int2quad(buffer, addr) << "   ";
	 cout << int2quad(buffer, ones(leng + 1, end)) << "   ";
	 cout << int2quad(buffer, masks[start]) << "   ";
	 cout << int2quad(buffer, ones(start + 1, end))
	      << "\n";
      }
   } else {
      RadixSet::SortedPrefixIterator itr(&nets.members);
      u_int addr;
      u_int leng;
      char buffer[64];

      for (bool ok = itr.first(addr, leng);
	   ok;
	   ok = itr.next(addr, leng)) {
	 cout << "access-list " << aclID << returnPermitOrDeny(allow_flag) << "ip ";

	 cout << int2quad(buffer, addr) << "   0.0.0.0   ";
	 cout << int2quad(buffer, masks[leng]) << "   0.0.0.0\n";
      }
   }

   // terminate the access list
   cout << "access-list " << aclID  << returnPermitOrDeny(!allow_flag)
        << "ip 0.0.0.0 255.255.255.255 0.0.0.0 255.255.255.255\n";

   return result;
}

ListOf2Ints *CiscoConfig::printPrefixList(SetOfIPv6Prefix& nets) {

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

   // clear this prefix list
   cout << "!\nno ipv6 prefix-list " << ipv6_pl << aclID << "\n";

   IPv6RadixSet::SortedPrefixRangeIterator itr(&nets.members);
   ipv6_addr_t addr;
   u_int leng;
   u_int start;
   u_int end;
   char buffer[256];

   for (bool ok = itr.first(addr, leng, start, end);
	ok;
	ok = itr.next(addr, leng, start, end)) {
      cout << "ipv6 prefix-list " << ipv6_pl << aclID << returnPermitOrDeny(allow_flag);
      cout << ipv62hex(&addr, buffer) << "/" << leng;
      if (start != leng)
	      cout << " ge " << start;
      if (end != leng)
	      cout << " le " << end;
      cout << "\n";
   }

   cout << "ipv6 prefix-list " << ipv6_pl << aclID << returnPermitOrDeny(!allow_flag) << "::/0 le 128" << endl;

   return result;

}

ListOf2Ints *CiscoConfig::printPrefixList(SetOfPrefix& nets) {
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

   // clear this access list
   cout << "!\nno ip prefix-list pl" << aclID << "\n";

   RadixSet::SortedPrefixRangeIterator itr(&nets.members);
   u_int addr;
   u_int leng;
   u_int start;
   u_int end;
   char buffer[64];

   for (bool ok = itr.first(addr, leng, start, end);
	ok;
	ok = itr.next(addr, leng, start, end)) {
      cout << "ip prefix-list pl" << aclID << returnPermitOrDeny(allow_flag);

      /* need to look at WeeSan's code */
      cout << int2quad(buffer, addr) << "/" << leng;
      if (start != leng)
	 cout << " ge " << start;
      if (end != leng)
	 cout << " le " << end;
      cout << "\n";
   }

   // terminate the access list
   cout << "ip prefix-list pl" << aclID << returnPermitOrDeny(!allow_flag) << "0.0.0.0/0 le 32\n";

   return result;
}
ListOf2Ints *CiscoConfig::printCommunities(FilterOfCommunity& cm) {
   // return the access list numbers if something is printed
   ListOf2Ints *result;

   if (cm.is_universal())
      return 0;

   // check to see if we already printed an identical access list,
   if (useAclCaches && (result = communityMgr.search(cm)))
      return result;

   result = communityMgr.add(cm);
   int aclID;

   if (firstCommunityList) {
      firstCommunityList = false;
      cout << "!\nip bgp-community new-format\n";
   }

   // now print the communities
   // first print the conjuncts which are only positive contains
   CommunityConjunct *cc;
   bool first_time = true;
   for (cc = cm.conjuncts.head(); cc; cc = cm.conjuncts.next(cc)) {
      if (cc->pe->isEmpty() && cc->n->isEmpty() && cc->ne.isEmpty()
          && ! cc->p->isEmpty()) {
         if (first_time) {
            first_time = false;
            // clear this access list
            aclID = communityMgr.newID();
            result->add(aclID, aclID);
            cout << "!\nno ip community-list " << aclID << "\n";
         }
         cout << "ip community-list " << aclID << " permit ";
         for (Pix pi = cc->p->first(); pi; cc->p->next(pi)) {
            CiscoConfig::printCommunity(cout, (*cc->p)(pi));
            cout << " ";
         }
         cout << "\n";
      }
   }

   for (cc = cm.conjuncts.head(); cc; cc = cm.conjuncts.next(cc)) {
      if (cc->pe->isEmpty() && cc->n->isEmpty() && cc->ne.isEmpty()
          && ! cc->p->isEmpty()) // this case is done above
         continue;

      // clear this access list
      aclID = communityMgr.newID();
      cout << "!\nno ip community-list " << aclID << "\n";

      if (! cc->pe->isEmpty()) {
         cout << "ip community-list " << aclID << " permit ";
         for (Pix pi = cc->pe->first(); pi; cc->pe->next(pi)) {
            CiscoConfig::printCommunity(cout, (*cc->pe)(pi));
            cout << " ";
         }
         cout << "\n";
         result->add(aclID, aclID, 1);
         continue; // if pe is non-empty then p, n, ne are all empty
      }

      // TGG
      // ne needs an exact match too. Each element of cc
      // requires its own (exactly matched) community list
      //
      CommunitySet *cs;
      for (cs = cc->ne.head(); cs; cs = cc->ne.next(cs)) {
	 cout << "ip community-list " << aclID << " deny ";
	 for (Pix pi = cs->first(); pi; cs->next(pi)) {
            CiscoConfig::printCommunity(cout, (*cs)(pi));
            cout << " ";
	 }
	 cout << "\n";
	 result->add(aclID, aclID, 1);
	 if (!(cc->p->isEmpty()) ||
	     !(cc->n->isEmpty()) ||
	     cc->ne.next(cs)) {
            aclID = communityMgr.newID();
            cout << "!\nno ip community-list " << aclID << "\n";
	 }
      }

      // TGG
      // if there will not follow at least one positive match,
      // and an ne was encountered, then we must add a community
      // list that will match anything
      //
      if (cc->pe->isEmpty() && cc->p->isEmpty() && cc->n->isEmpty()) {
        aclID = communityMgr.newID();
        cout << "!\nno ip community-list " << aclID << "\n";
        cout << "ip community-list " << aclID << " permit internet\n";
	result->add(aclID, aclID);
	continue;
      }

      Pix pi;
      for (pi = cc->n->first(); pi; cc->n->next(pi)) {
         cout << "ip community-list " << aclID << " deny ";
         CiscoConfig::printCommunity(cout, (*cc->n)(pi));
         cout << "\n";
      }

      if (! cc->p->isEmpty()) {
         cout << "ip community-list " << aclID << " permit ";
         for (pi = cc->p->first(); pi; cc->p->next(pi)) {
            CiscoConfig::printCommunity(cout, (*cc->p)(pi));
            cout << " ";
         }
         cout << "\n";
      }

      // TGG
      // don't do this for ne's, only for n's
      if (cc->p->isEmpty() && !cc->n->isEmpty())
         cout << "ip community-list " << aclID << " permit internet\n";

      result->add(aclID, aclID);
   }

   return result;
}

void CiscoConfig::printREASno(ostream& out, const RangeList &no) {
   RangeList::Range *pi;
   int first = 1;
   int put_par = 0;

   if (no.universal())
      out << "(_[0-9]+)";
   else {
      out << "_";
      pi = no.ranges.head();
      put_par = ! no.ranges.isSingleton() || pi->high != pi->low || inTilda;
      if (put_par) {
	 if (reSplittable) {
	    if (inTilda)
	       out << "&";
	    else
	       out << "@";
	 } else {
	    out << "("; 
         }
      }

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

int CiscoConfig::printRE_(ostream& os, const regexp& r) {
   static bool inAlternate = false;
   int flag = 0;

   if (typeid(r) == typeid(regexp_or)) {
      bool save = inAlternate;

      if (!inAlternate) {
	 inAlternate = true;
	 os << "(";
      }

      flag = CiscoConfig::printRE_(os, *((regexp_or &) r).left);
      os << "|";
      flag &= CiscoConfig::printRE_(os, *((regexp_or &) r).right);

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
      CiscoConfig::printREASno(os, ((regexp_symbol &) r).asnumbers);
      return flag;
   }

   if (typeid(r) == typeid(regexp_star)) {
      bool save = reSplittable;
      reSplittable = forceTilda;
      hasTilda = forceTilda;
      inTilda = forceTilda;
      os << "(";
      flag = CiscoConfig::printRE_(os, *((regexp_star &) r).left);
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
      flag = CiscoConfig::printRE_(os, *((regexp_plus &) r).left);
      os << ")+";
      reSplittable = save;
      return flag;
   }

   if (typeid(r) == typeid(regexp_tildastar)) {
      hasTilda = true;
      inTilda = true;
      os << "(";
      flag = CiscoConfig::printRE_(os, *((regexp_star &) r).left);
      os << ")*";
      inTilda = false;
      return flag;
   }

   if (typeid(r) == typeid(regexp_tildaplus)) {
      hasTilda = true;
      inTilda = true;
      os << "(";
      flag = CiscoConfig::printRE_(os, *((regexp_plus &) r).left);
      os << ")+";
      inTilda = false;
      return flag;
   }

   if (typeid(r) == typeid(regexp_question)) {
      os << "(";
      flag = CiscoConfig::printRE_(os, *((regexp_question &) r).left);
      os << ")?";
      return flag;
   }

   if (typeid(r) == typeid(regexp_cat)) {
      CiscoConfig::printRE_(os, *((regexp_cat &) r).left);
      flag = CiscoConfig::printRE_(os, *((regexp_cat &) r).right);
      return flag;
   }

   assert(1);
   os << "REGEXP_UNKNOWN";
   return flag;
}

void CiscoConfig::printRE(ostream &s, 
		          const regexp &r, 
			  int aclID, 
			  bool permit){
   ostringstream out;
   out << "ip as-path access-list "
       << aclID 
       << (permit ? " permit " : " deny ");
   reSplittable = true;
   hasTilda = false;
   inTilda = false;
   if (!CiscoConfig::printRE_(out, r)) // true if the expression was missing $
      out << "_";
   out << endl;

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
         cerr << "Warning: ip as-path access-list is too long for cisco to handle" << endl;
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
	 } 
      }
   }
   free(p);
}

ListOf2Ints* CiscoConfig::printASPaths(regexp_nf& path) {
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

   bool needNewAcl = true;
   int pass1_found = 0;
   for (rc = path.rclist.head(); rc; rc = path.rclist.next(rc)) {
      ri = rc->regexs.head();
      if (rc->regexs.isSingleton() && !ri->negated) {
	 pass1_found = 1;
	 if (needNewAcl) {
	    needNewAcl = false;
	    aclID = aspathMgr.newID();
            cout << "!\nno ip as-path access-list  " << aclID << "\n"; 
         }
         CiscoConfig::printRE(cout, *ri->re, aclID, true);
	 rc->mark = 1;
      } else
	 rc->mark = 0;
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
	 if (needNewAcl) {
	    aclID = aspathMgr.newID();
	    cout << "!\nno ip as-path access-list  " << aclID << "\n"; 
	 }
	 for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
	    if (!ri->negated)
	       positive_ri = ri;
	    else {
	       CiscoConfig::printRE(cout, *ri->re, aclID, false);
	    }
	 }
	 if (positive_ri) {
	    CiscoConfig::printRE(cout, *positive_ri->re, aclID, true);
	 } else {
	    cout << "ip as-path access-list " << aclID 
		 << " permit .*\n";
	 }
	 result->add(aclID, aclID);
	 needNewAcl = true;
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
      cout << "!\nno ip as-path access-list  " << aclID << "\n"; 

      // print negative ones all in one access list
      sno = aclID;
      for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
	 if (ri->negated) {
	    CiscoConfig::printRE(cout, *ri->re, aclID, false);
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
	       cout << "!\nno ip as-path access-list  " << aclID << "\n"; 
 	    }
	    CiscoConfig::printRE(cout, *ri->re, aclID, true);
	 }
      }

      if (first) { // no positive one found
	 cout << "ip as-path access-list " << aclID 
	      << " permit .*\n";
      }

      result->add(sno, aclID);
   }

   return result;
}
   
inline void CiscoConfig::printCommunity(ostream &os, unsigned int i) {
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

void CiscoConfig::printCommunityList(ostream &os, ItemList *args) {
   for (Item *cmnty = args->head(); cmnty; cmnty = args->next(cmnty)) {
      os << " ";

      if (typeid(*cmnty) == typeid(ItemINT)) {
	 printCommunity(os, ((ItemINT *) cmnty)->i);
	 continue;
      }

      if (typeid(*cmnty) == typeid(ItemWORD)) {
	 if (!strcasecmp(((ItemWORD *)cmnty)->word, "no_advertise"))
	    printCommunity(os, COMMUNITY_NO_ADVERTISE);
	 else if (!strcasecmp(((ItemWORD *)cmnty)->word, "no_export"))
	    printCommunity(os, COMMUNITY_NO_EXPORT);
	 else if (!strcasecmp(((ItemWORD *)cmnty)->word,"no_export_subconfed"))
	    printCommunity(os, COMMUNITY_NO_EXPORT_SUBCONFED);
	 else
	    printCommunity(os, COMMUNITY_INTERNET);
	 continue;
      }

      if (typeid(*cmnty) == typeid(ItemList)) {
	 int high = ((ItemINT *) ((ItemList *) cmnty)->head())->i;
	 int low  = ((ItemINT *) ((ItemList *) cmnty)->tail())->i;
	 printCommunity(os, (high << 16) + low);
	 continue;
      }
   }
}

int CiscoConfig::printCommunitySetList(ostream &os, ItemList *args) {

   int aclID = communityMgr.newID();
   os << "!\nno ip community-list " << aclID << "\n";
   os << "ip community-list " << aclID << " permit ";
   CiscoConfig::printCommunityList(os, args);
   os << "!\n";

return aclID;
}


void CiscoConfig::printActions(ostream &os, PolicyActionList *actions, ItemAFI *afi) {
#define UNIMPLEMENTED_METHOD \
   cerr << "Warning: unimplemented method " \
	<< actn->rp_attr->name << "." << actn->rp_method->name << endl

   PolicyAction *actn;
   for (actn = actions->head(); actn; actn = actions->next(actn)) {
      if (actn->rp_attr == dctn_rp_pref) {
	 if (actn->rp_method == dctn_rp_pref_set) {
	    int pref = ((ItemINT *) actn->args->head())->i;
	    os << " set local-preference " << (preferenceCeiling-pref) << "\n";
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
        os << " set ip next-hop " << buffer << "\n";
      } else if (afi->is_ipv6() && (typeid(*(actn->args->head())) == typeid(ItemIPV6))) {
        char buffer[50];
        IPv6Addr *ip = ((ItemIPV6 *) actn->args->head())->ipv6;
        ip->get_text(buffer);
        os << " set ipv6 next-hop " << buffer << "\n";
      } else {
        cout << "Warning: next-hop address family doesn't match protocol address family, ignoring next-hop..." << endl;
      }
	 } else 
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_dpa) {
	 if (actn->rp_method == dctn_rp_dpa_set) {
	    int dpa = ((ItemINT *) actn->args->head())->i;
	    os << " set dpa " << dpa << "\n";
	 } else 
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_med) {
	 if (actn->rp_method == dctn_rp_med_set) {
	    Item *item = actn->args->head();
	    if (typeid(*item) == typeid(ItemINT))
	       os << " set metric " << ((ItemINT *) item)->i << "\n";
	    else
	       os << " set metric-type internal\n";
	 } else 
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_community) {
	 if (actn->rp_method == dctn_rp_community_setop) {
	    os << " set community ";
	    printCommunityList(os, (ItemList *) actn->args->head());
	    os << "\n";
	 } else if (actn->rp_method == dctn_rp_community_appendop) {
	    os << " set community ";
	    printCommunityList(os, (ItemList *) actn->args->head());
	    os << " additive\n";
	 } else if (actn->rp_method == dctn_rp_community_append) {
	    os << " set community ";
	    printCommunityList(os, actn->args);
	    os << " additive\n";
         } else if (actn->rp_method == dctn_rp_community_delete) {
            int commlist = printCommunitySetList(delayoutput, actn->args);
            os << " set comm-list " << commlist << " delete\n";
	 } else
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      if (actn->rp_attr == dctn_rp_aspath) {
	 if (actn->rp_method == dctn_rp_aspath_prepend) {
	    os << " set as-path prepend ";
	    for (Item *plnd = actn->args->head();
		 plnd;
		 plnd = actn->args->next(plnd))
	       os << ((ItemASNO *) plnd)->asno << " ";
	    os << "\n";
	 } else
	    UNIMPLEMENTED_METHOD;
	 continue;
      }

      cerr << "Warning: unimplemented attribute " 
	   << *actn->rp_attr << endl;
   }

   os << "exit\n"; // exit route map config mode 
   os << delayoutput.str() << endl;
   delayoutput.str("");

}

int CiscoConfig::print(NormalExpression *ne, 
		       PolicyActionList *actn, 
		       int import_flag, 
		       ItemAFI *afi) {
   int last = 0;
   static ListOf2Ints empty_list(1);

   Debug(Channel(DBG_RTC_CISCO) << "# ne: " << *ne << "\n");
   
   // modified by katie@ripe.net to handle any/not any expr
   // for prefix filters, cisco only

   if (ne->is_any() != NEITHER)	
	cerr << "Warning: filter \"" << mapName << "\" matches ANY/NOT ANY" << endl;

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
	else
		return last;
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
	         cout << "!\nno route-map " << mapName << "\n";
	         needToPrintNoRouteMap = false;
	      }

	    for (asp = aspath_acls->head(); asp; asp = aspath_acls->next(asp)) {
	      for (cmp = comm_acls->head(); cmp; cmp = comm_acls->next(cmp)) {
	        cout << "!\nroute-map " << mapName
		      << " permit " << routeMapID << "\n";
	        routeMapID += mapIncrements;
	        for (i = asp->start; i <= asp->end; i++)
      		  cout << " match as-path " << i << "\n";
	        for (i = cmp->start; i <= cmp->end; i++)
		        cout << " match community " << i 
		        << (cmp->flag ? " exact\n" : "\n");
	       if (!import_flag || forcedInboundMatchIP) {
		       for (prp = prfx_acls->head(); prp; prp =prfx_acls->next(prp)) {
		         for (i = prp->start; i <= prp->end; i++) {
			         if (usePrefixLists)
			           cout << " match ip address prefix-list pl" 
				         << i <<"\n";
			         else
			         cout << " match ip address " << i << "\n";
               }
           }
           if (afi->is_ipv6()) {
             for (prp = ipv6_prfx_acls->head(); prp; prp =ipv6_prfx_acls->next(prp)) {
               for (i = prp->start; i <= prp->end; i++) {
                 if (usePrefixLists)
                   cout << " match ipv6 address prefix-list " << ipv6_pl
                   << i <<"\n";
                 else
                   cout << " match ipv6 address " << ipv6_acl << i << "\n";
               }
             }
           }
           CiscoConfig::printActions(cout, actn, afi);
         }
	      }
	    }
    }
  }
   

   return last;
}

// Reimplemented to handle different afi's
bool CiscoConfig::printNeighbor(int import, 
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
      cout << "!\nno route-map " << mapName << "\n";
      cout << "!\nroute-map " << mapName << " deny " << mapNumbersStartAt << "\n";
      routeMapGenerated = true;
   }

   // create bgp process 
   cout << "!\nrouter bgp " << asno << "\n!\n";

   if (peerGroup)
     cout << " neighbor "   << neighbor << " peer-group\n";
   else 
     cout << " neighbor "   << neighbor << " remote-as " << peerAS << "\n";

   if (afi_activate && !peerGroup) {
     if (strcmp(filter_afi->name(),"ipv6") == 0) {
       cout << " address-family " << "ipv4" << endl;
       cout << indent <<" no neighbor " << neighbor << " activate\n"; 
       cout << " address-family " << "ipv6 unicast" << endl;
       cout << indent <<" neighbor " << neighbor << " activate\n"; 
       cout << " address-family " << "ipv6 multicast" << endl;
       cout << indent <<" neighbor " << neighbor << " activate\n"; 
     } else if (strcmp(filter_afi->name(),"ipv6.unicast") == 0) {
       cout << " address-family " << "ipv4" << endl;
       cout << indent <<" no neighbor " << neighbor << " activate\n"; 
       cout << " address-family " << "ipv6 unicast" << endl;
       cout << indent <<" neighbor " << neighbor << " activate\n"; 
     } else if (strcmp(filter_afi->name(),"ipv4.multicast") == 0) {
       cout << " address-family " << "ipv4 multicast" << endl;
       cout << indent <<" neighbor " << neighbor << " activate\n"; 
     } else if (strcmp(filter_afi->name(),"ipv6.multicast") == 0) {
       cout << " address-family " << "ipv4" << endl;
       cout << indent <<" no neighbor " << neighbor << " activate\n"; 
       cout << " address-family " << "ipv6 multicast" << endl;
       cout << indent <<" neighbor " << neighbor << " activate\n"; 
     } else if (strcmp(filter_afi->name(),"ipv4.unicast") == 0) { 
       cout << " address-family " << "ipv4 unicast" << endl; 
       cout << indent <<" neighbor " << neighbor << " activate\n"; 
     }
     // ought to handle afi any too, but that's not just done by naming it here
   }

   if (routeMapGenerated) 
      cout << indent << " neighbor " << neighbor 
	   << " route-map " << mapName << " " << direction << "\n";
   
   if (import && ! forcedInboundMatchIP && prefixListGenerated) {
      if (usePrefixLists)
	 cout << indent << " neighbor " << neighbor
	      << " prefix-list pl" << distributeListNo << " in\n";
      else
	 cout << indent << " neighbor " << neighbor
	      << " distribute-list " << distributeListNo << " in\n";
   }

   if (afi_activate)
      cout << " exit\n"; // exit address-family config mode
   cout << "!\n";

   if (!peerGroup)
   cout << "exit\n" ; // exit neighbor config mode explicitly if not peer group

   return true;
}

void CiscoConfig::exportP(ASt asno, MPPrefix *addr, 
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

     // peer_afi should be afi of peer IPs, filter_afi is the one specified in filter
     ItemAFI *peer_afi = new ItemAFI(peer_addr->get_afi());
     printNeighbor(EXPORT, asno, peerAS, peer_addr->get_ip_text(), false, (ItemAFI *) peer_afi, (ItemAFI *) afi);

     routeMapGenerated = false;
     prefixListGenerated = false;
  }
}

void CiscoConfig::importP(ASt asno, MPPrefix *addr, 
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

     // peer_afi should be afi of peer IPs, filter_afi is the one specified in filter
     ItemAFI *peer_afi = new ItemAFI(peer_addr->get_afi());
     printNeighbor(IMPORT, asno, peerAS, peer_addr->get_ip_text(), false, (ItemAFI *) peer_afi, (ItemAFI *) afi);

     routeMapGenerated = false;
     prefixListGenerated = false;
  }
}

void CiscoConfig::static2bgp(ASt asno, MPPrefix *addr) {

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
    cout << "router bgp " << asno << "\n!\n";
    bool afi_activate = false;
    if (!((ItemAFI *) afi)->is_default()) 
      afi_activate = true;
    const char *indent = (afi_activate) ? " " : "";
    if (routeMapGenerated) {
      if (afi_activate)
         cout << " address-family " << *afi << endl;  
      cout << indent << " redistribute static route-map " << mapName << "\n";
      if (afi_activate)
         cout << " exit\n!\n" << endl;
    }
    routeMapGenerated = false;
    prefixListGenerated = false;

   }
}

void CiscoConfig::deflt(ASt asno, ASt peerAS) {
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

   cout << "!\n";

   NormalExpression *ne;
   int last = 0;
   for (; deflt && !last; deflt = itr.next()) {
      ne = NormalExpression::evaluate(deflt->filter, peerAS);

      if (ne->is_universal()) {
         cout << "ip default-network 0.0.0.0\n";
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
                 << "\n";
         }
      }
   }
}

void CiscoConfig::networks(ASt asno) {
   const MPPrefixRanges *nets = irr->expandAS(asno);
   MPPrefixRanges::const_iterator p;
   static char buffer[128];

   if (nets) {
     cout << "!\n";
     for (p = nets->begin(); p != nets->end(); ++p) {
       if (p->ipv4) {
         cout << "network " << p->ipv4->get_ip_text()
            << " mask " << int2quad(buffer, p->ipv4->get_mask()) 
            << "\n";
       }
     }
   }
}

void CiscoConfig::IPv6networks(ASt asno) {
   const MPPrefixRanges *nets = irr->expandAS(asno);
   MPPrefixRanges::const_iterator p;

   if (nets) {
     cout << "!\n";
     bool afi_activate = true;
     for (p = nets->begin(); p != nets->end(); ++p) {
       if (p->ipv6) {
         if (afi_activate) {
           cout << "address-family ipv6" << endl; /// unicast/multicast?
           afi_activate = false;
         }
         cout << " network " << p->ipv6->get_ip_text()
              << " mask " << p->ipv6->get_mask()
              << "\n";
       }
     }
     if (!afi_activate)
        cout << "exit\n";
   }
}

int CiscoConfig::printPacketFilter(SetOfPrefix &set) {
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

   cout << "!\nno access-list " << aclID << "\n";

   for (bool ok = itr.first(addr, leng); ok; ok = itr.next(addr, leng)) {
      if (!addr && !leng) // skip 0.0.0.0/0
	 continue;

      cout << "access-list " << aclID << returnPermitOrDeny(allow_flag) << "ip ";

      cout << int2quad(buffer, addr) << " ";
      cout << int2quad(buffer, ~masks[leng]) << " any \n";
   }

   if (set.universal()) // handle 0.0.0.0/0
     allow_flag = false;

   cout << "access-list " << aclID << returnPermitOrDeny(!allow_flag) << "ip any any" << endl;

   return aclID;
}

int CiscoConfig::printPacketFilter(SetOfIPv6Prefix &set) {
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

   cout << "!\nno ipv6 access-list " << ipv6_acl << aclID << "\n";

   for (bool ok = itr.first(addr, leng); ok; ok = itr.next(addr, leng)) {
      if (!addr && !leng)
        continue;

      cout << "ipv6 access-list " << ipv6_acl << aclID << returnPermitOrDeny(allow_flag) << "ip ";

      cout << ipv62hex(&addr, buffer) << " ";
      cout << ipv62hex(&(addr.getmask(leng)), buffer) << " any \n";
   }

   if (set.universal()) // handle ::/0
     allow_flag = false;

   cout << "ipv6 access-list " << ipv6_acl << aclID << returnPermitOrDeny(!allow_flag) << "ip any any" << endl;

   return aclID;
}


void CiscoConfig::inboundPacketFilter(char *ifname, ASt as, MPPrefix* addr, 
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
     cout << " interface " << ifname << "\n ipv6 access-group " << ipv6_acl << ipv6_aclid << direction << endl;
     cout << "exit" << endl;
   }
}

void CiscoConfig::outboundPacketFilter(char *ifname, ASt as, MPPrefix* addr, 
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
     cout << " interface " << ifname << "\n ipv6 access-group " << ipv6_acl << ipv6_aclid << direction << endl;
     cout << "exit" << endl;
   }
}

void CiscoConfig::importGroup(ASt asno, char * pset) {
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
   // afi is ignored here
   printNeighbor(import, asno, 0, pset, true, NULL, NULL);

   const PeeringSet *prngSet = irr->getPeeringSet(psetID);
   if (prngSet) {
     AutNumPeeringIterator aut_itr(autnum);

     for (AttrIterator<AttrPeering> itr(prngSet, "peering"); itr; itr++)
       if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
            && typeid(*itr()->peering->peerASes) == typeid(FilterASNO))
         cout << " neighbor " << *itr()->peering->peerRtrs  
              << " remote-as " << ((FilterASNO *)itr()->peering->peerASes)->asno << endl;
     for (AttrIterator<AttrPeering> itr(prngSet, "mp-peering"); itr; itr++)
       if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
            && typeid(*itr()->peering->peerASes) == typeid(FilterASNO))
         cout << " neighbor " << *itr()->peering->peerRtrs  
              << " remote-as " << ((FilterASNO *)itr()->peering->peerASes)->asno << endl;

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
           if (afi_activate)
             cout << indent << " neighbor " << *itr()->peering->peerRtrs << " activate\n";
           cout << indent << " neighbor " << *itr()->peering->peerRtrs  << " peer-group " << pset << "\n";
         }
       }
       for (AttrIterator<AttrPeering> itr(prngSet, "mp-peering"); itr; itr++) {
         if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
              && typeid(*itr()->peering->peerASes) == typeid(FilterASNO)) {
           if (afi_activate)
             cout << indent << " neighbor " << *itr()->peering->peerRtrs << " activate\n";
           cout << indent << " neighbor " << *itr()->peering->peerRtrs  << " peer-group " << pset << "\n";
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

void CiscoConfig::exportGroup(ASt asno, char * pset) {
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
     for (fa = common_list->head(); fa && !last; fa = common_list->next(fa)) {
       ne = NormalExpression::evaluate(new FilterAFI((ItemAFI *) afi->dup(), fa->filter), ~0);
       last = print(ne, fa->action, import, (ItemAFI *) afi);;
       delete ne;
     }
   }
   // afi is ignored here
   printNeighbor(import, asno, 0, pset, true, NULL, NULL);

   const PeeringSet *prngSet = irr->getPeeringSet(psetID);
   if (prngSet) {
     AutNumPeeringIterator aut_itr(autnum);

     for (AttrIterator<AttrPeering> itr(prngSet, "peering"); itr; itr++)
       if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
            && typeid(*itr()->peering->peerASes) == typeid(FilterASNO))
         cout << " neighbor " << *itr()->peering->peerRtrs  
              << " remote-as " << ((FilterASNO *)itr()->peering->peerASes)->asno << endl;
     for (AttrIterator<AttrPeering> itr(prngSet, "mp-peering"); itr; itr++)
       if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
            && typeid(*itr()->peering->peerASes) == typeid(FilterASNO))
         cout << " neighbor " << *itr()->peering->peerRtrs  
              << " remote-as " << ((FilterASNO *)itr()->peering->peerASes)->asno << endl;

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
           if (afi_activate)
             cout << indent << " neighbor " << *itr()->peering->peerRtrs << " activate\n";
           cout << indent << " neighbor " << *itr()->peering->peerRtrs  << " peer-group " << pset << "\n";
         }
       }
       for (AttrIterator<AttrPeering> itr(prngSet, "mp-peering"); itr; itr++) {
         if (typeid(*itr()->peering->peerRtrs) == typeid(FilterRouter)  
              && typeid(*itr()->peering->peerASes) == typeid(FilterASNO)) {
           if (afi_activate)
             cout << indent << " neighbor " << *itr()->peering->peerRtrs << " activate\n";
           cout << indent << " neighbor " << *itr()->peering->peerRtrs  << " peer-group " << pset << "\n";
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
