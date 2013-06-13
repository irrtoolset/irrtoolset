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

#include "f_iosxr.hh"
#include "config.h"
#include "rpsl/schema.hh"
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>

//////////////////////////// caches ////////////////////////////////

AccessLManager<regexp_nf>         iosxrASpathMgr;

//////////////////////////// methods ////////////////////////////////

void IOSXRConfig::printAccessList(SetOfPrefix& nets) {
      bool save = BaseConfig::useAclCaches;
      BaseConfig::useAclCaches = false;
      printRoutes(nets);
      BaseConfig::useAclCaches = save;
}

void IOSXRConfig::printAccessList(SetOfIPv6Prefix& nets) {
      bool save = BaseConfig::useAclCaches;
      BaseConfig::useAclCaches = false;
      printRoutes(nets);
      BaseConfig::useAclCaches = save;
}
void IOSXRConfig::printAspathAccessList(FilterOfASPath& path) {
      bool save = BaseConfig::useAclCaches;
      BaseConfig::useAclCaches = false;
      printASPaths(path);
      BaseConfig::useAclCaches = save;
}

LOf2Ints *IOSXRConfig::printRoutes(SetOfIPv6Prefix& nets) {
   // return the access list number if something is printed
   LOf2Ints *result;

   if (nets.universal())
      return NULL;

   // clear and initialize the prefix-set
   cout << "\nprefix-set " << prefixListName << "\n";

   if (nets.members.isEmpty()) {
     if (nets.negated())
       cout << "   # IPv6 COMPLETE\n   ::/0 le 128\n";
     else
       cout << "   # IPv6 EMPTY\n";
     cout << "end-set\n";
     return result;
   }

   // next is a hack to represent a negated ipv6 prefix-set correctly in IOS XR. (hagen)
   if (nets.negated()) calculateNegation(nets);

   ipv6_addr_t addr;
   u_int leng;
   char buffer[256];
   bool printComma = false;

   if (compression == COMP_NONE) {
     IPv6RadixSet::SortedPrefixIterator spItr(&nets.members);

     for (bool ok = spItr.first(addr, leng); ok; ok = spItr.next(addr, leng)) {
       if (printComma)
         cout << ",\n";
       else
         printComma = true;
       cout << "   " << ipv62hex(&addr, buffer) << "/" << leng;
     }
   } else if (compression == COMP_NORMAL) {
     IPv6RadixSet::SortedPrefixRangeIterator sprItr(&nets.members);
     u_int start;
     u_int end;

     for (bool ok = sprItr.first(addr, leng, start, end); ok; ok = sprItr.next(addr, leng, start, end)) {
       if (printComma)
         cout << ",\n";
       else
         printComma = true;
       cout << "   " << ipv62hex(&addr, buffer) << "/" << leng;
       if (start != leng)
         cout << " ge " << start;
       if (end != leng)
         cout << " le " << end;
     }
   } else if (compression == COMP_SUPER) {
     IPv6RadixSet::SortedIterator sItr(&nets.members);
     ipv6_addr_t rngs;

     for (bool ok = sItr.first(addr, leng, rngs); ok; ok = sItr.next(addr, leng, rngs)) {
       if (printComma)
         cout << ",\n";
       else
         printComma = true;
       cout << "   " << ipv62hex(&addr, buffer) << "/" << leng;
     }
   } else {
     cerr << "Error: unknown compression type! Cannot print IOS XR IPv6 prefix-list!\n";
     exit(1);
   }

   // terminate the prefix-set
   cout << "\nend-set\n";

   return result;
}

LOf2Ints *IOSXRConfig::printRoutes(SetOfPrefix& nets) {
   // return the access list number if something is printed
   LOf2Ints *result;

   if (nets.universal())
      return NULL;

   // clear and initialize the prefix-set
   cout << "\nprefix-set " << prefixListName << "\n";

   if (nets.members.isEmpty()) {
     if (nets.negated())
       cout << "   # IPv4 COMPLETE\n   0.0.0.0/0 le 32\n";
     else
       cout << "   # IPv4 EMPTY\n";
     cout << "end-set\n";
     return result;
   }

   // next is a hack to represent a negated prefix-set correctly in IOS XR. (hagen)
   if (nets.negated()) calculateNegation(nets);

   u_int addr;
   u_int leng;
   char buffer[64];
   bool printComma = false;

   if (compression == COMP_NONE) {
     RadixSet::SortedPrefixIterator spItr(&nets.members);

     for (bool ok = spItr.first(addr, leng); ok; ok = spItr.next(addr, leng)) {
       if (printComma)
         cout << ",\n";
       else
         printComma = true;
       cout << "   " << int2quad(buffer, addr) << "/" << leng;
     }
   } else if (compression == COMP_NORMAL) {
     RadixSet::SortedPrefixRangeIterator sprItr(&nets.members);
     u_int start;
     u_int end;

     for (bool ok = sprItr.first(addr, leng, start, end); ok; ok = sprItr.next(addr, leng, start, end)) {
       if (printComma)
         cout << ",\n";
       else
         printComma = true;
       cout << "   " << int2quad(buffer, addr) << "/" << leng;
       if (start != leng)
	 cout << " ge " << start;
       if (end != leng)
	 cout << " le " << end;
     }
   } else if (compression == COMP_SUPER) {
     RadixSet::SortedIterator sItr(&nets.members);
     u_int64_t rngs;

     for (bool ok = sItr.first(addr, leng, rngs); ok; ok = sItr.next(addr, leng, rngs)) {
       if (printComma)
         cout << ",\n";
       else
         printComma = true;
       cout << "   " << int2quad(buffer, addr) << "/" << leng;
     }
   } else {
     cerr << "Error: unknown compression type! Cannot print IOS XR IPv4 prefix-list!\n";
     exit(1);
   }

   // terminate the prefix-list
   cout << "\nend-set\n";

   return result;
}

void IOSXRConfig::printREASno(ostream& out, const RangeList &no) {
   extern bool opt_asdot;
   RangeList::Range *pi;
   int first = 1;
   int put_par = 0;

   if (no.universal())
      out << "(_[0-9]+)";
   else {
      out << "_";

      pi = no.ranges.head();
      put_par = ! no.ranges.isSingleton() || pi->high != pi->low || inTilda;
      if (put_par) out << "("; 

      for (; pi; pi = no.ranges.next(pi)) {
	 if (pi->low != pi->high) {
	     for (unsigned int i = pi->low; (i <= pi->high); ++i) {
		if (!first) 
		   out << "|";
		else
		   first = 0;

		if (opt_asdot && (i > 65535))
		   printASNDottedNotation(out, i);
		else
		   out << i;
	     } 
	 }
	 else {
	     if (!first) 
		out << "|";
	     else
		first = 0;

             if (opt_asdot && (pi->low > 65535))
                printASNDottedNotation(out, pi->low);
             else
                out << pi->low;
	 }
      }
      
      if (put_par) out << ")";
   }
}

int IOSXRConfig::printRE_(ostream& os, const regexp& r) {
   static bool inAlternate = false;
   int flag = 0;

   if (typeid(r) == typeid(regexp_or)) {
      bool save = inAlternate;

      if (!inAlternate) {
	 inAlternate = true;
	 os << "(";
      }

      flag = IOSXRConfig::printRE_(os, *((regexp_or &) r).left);
      os << "|";
      flag &= IOSXRConfig::printRE_(os, *((regexp_or &) r).right);

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
      IOSXRConfig::printREASno(os, ((regexp_symbol &) r).asnumbers);
      return flag;
   }

   if (typeid(r) == typeid(regexp_star)) {
      bool save = reSplittable;
      reSplittable = BaseConfig::forceTilda;
      hasTilda = BaseConfig::forceTilda;
      inTilda = BaseConfig::forceTilda;
      os << "(";
      flag = IOSXRConfig::printRE_(os, *((regexp_star &) r).left);
      os << ")*";
      reSplittable = save;
      return flag;
   }

   if (typeid(r) == typeid(regexp_plus)) {
      bool save = reSplittable;
      reSplittable = BaseConfig::forceTilda;
      hasTilda = BaseConfig::forceTilda;
      inTilda = BaseConfig::forceTilda;
      os << "(";
      flag = IOSXRConfig::printRE_(os, *((regexp_plus &) r).left);
      os << ")+";
      reSplittable = save;
      return flag;
   }

   if (typeid(r) == typeid(regexp_tildastar)) {
      hasTilda = true;
      inTilda = true;
      os << "(";
      flag = IOSXRConfig::printRE_(os, *((regexp_star &) r).left);
      os << ")*";
      inTilda = false;
      return flag;
   }

   if (typeid(r) == typeid(regexp_tildaplus)) {
      hasTilda = true;
      inTilda = true;
      os << "(";
      flag = IOSXRConfig::printRE_(os, *((regexp_plus &) r).left);
      os << ")+";
      inTilda = false;
      return flag;
   }

   if (typeid(r) == typeid(regexp_question)) {
      os << "(";
      flag = IOSXRConfig::printRE_(os, *((regexp_question &) r).left);
      os << ")?";
      return flag;
   }

   if (typeid(r) == typeid(regexp_cat)) {
      IOSXRConfig::printRE_(os, *((regexp_cat &) r).left);
      flag = IOSXRConfig::printRE_(os, *((regexp_cat &) r).right);
      return flag;
   }

   assert(1);
   os << "REGEXP_UNKNOWN";
   return flag;
}

void IOSXRConfig::printRE(ostream &s, const regexp &r, int aclID, bool permit){}

LOf2Ints* IOSXRConfig::printASPaths(regexp_nf& path) {
// return the access list number if something is printed
   LOf2Ints *result;

   if (path.is_universal())
      return NULL;

   // check to see if we already printed an identical access list
   if (BaseConfig::useAclCaches && (result = iosxrASpathMgr.search(path)))
      return result;

   result = iosxrASpathMgr.add(path);

   regexp_nf::RegexpConjunct *rc;
   regexp_nf::RegexpConjunct::ReInt *ri;
 
   for (rc = path.rclist.head(); rc; rc = path.rclist.next(rc)) {
      ri = rc->regexs.head();
      if (!(rc->regexs.isSingleton()) || ri->negated) {
         cerr << "Warning: unknown as-path expression! Only regular expressions allowed for IOS XR!\n";
         cerr << "Warning: No as-path generated for \"" << asPathName << "\"\n";
         exit(1);
      }
   }

   bool printComma = false;

   cout << "\nas-path-set " << asPathName << endl;

   for (rc = path.rclist.head(); rc; rc = path.rclist.next(rc)) {
     if (printComma)
       cout << "\',\n";
     else
       printComma = true;
     ri = rc->regexs.head();
     cout << "    ios-regex \'";
     if (!IOSXRConfig::printRE_(cout, *ri->re)) // true if the expression was missing $
       cout << "_";
   }

   cout << "'\nend-set\n";

   return result;
}
