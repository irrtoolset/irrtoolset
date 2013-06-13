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

#include "f_junos.hh"
#include "config.h"
#include "rpsl/schema.hh"
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>

#define DBG_JUNOS 7
#define EXPORT 0
#define IMPORT 1
#define STATIC 2

//////////////////////////// caches ////////////////////////////////

AccessLManager<regexp_nf>         junosASpathMgr;

//////////////////////////// methods ////////////////////////////////

void JUNOSConfig::printAccessList(SetOfIPv6Prefix& nets) {
      bool save = BaseConfig::useAclCaches;
      BaseConfig::useAclCaches = false;
      printRoutes(nets);
      BaseConfig::useAclCaches = save;
}

void JUNOSConfig::printAccessList(SetOfPrefix& nets) {
      bool save = BaseConfig::useAclCaches;
      BaseConfig::useAclCaches = false;
      printRoutes(nets);
      BaseConfig::useAclCaches = save;
}

void JUNOSConfig::printAspathAccessList(FilterOfASPath& path) {
      bool save = BaseConfig::useAclCaches;
      BaseConfig::useAclCaches = false;
      printASPaths(path);
      BaseConfig::useAclCaches = save;
}

LOf2Ints *JUNOSConfig::printRoutes(SetOfIPv6Prefix& nets) {
   // This method has been changed to implement the same work-around that was implemented
   // for the IPv4 version of that method. See comment for IPv4 nets.

   // return the access list number if something is printed
   LOf2Ints *result;

   if (nets.universal())
      return NULL;

   const char *permitOrDeny;
   const char *negatedPermitOrDeny;

   if (nets.negated()) {
      permitOrDeny = "reject";
      negatedPermitOrDeny = "accept";
   } else {
      permitOrDeny = "accept";
      negatedPermitOrDeny = "reject";
   }
   
   cout << "\n   policy-statement " << prefixListName << " {\n";

   if (nets.members.isEmpty()) {
     cout << "      term prefixes {\n";
     if (nets.negated()) {
       cout << "         # IPv6 COMPLETE\n";
       cout << "         then accept;\n      }\n   }\n";
     } else {
       cout << "         # IPv6 EMPTY\n";
       cout << "         then reject;\n      }\n   }\n";
     }
     return result;
   }

   ipv6_addr_t addr;
   u_int leng;
   char buffer[256];
   u_int termCounter = 0; // counts terms to get unique term names
   bool singleRF = BaseConfig::singleRouteFilter;

   if (! singleRF) {
      cout << "      term prefixes {\n         from {\n";
   }
   if (compression == COMP_NONE) {
      IPv6RadixSet::SortedPrefixIterator spItr(&nets.members);

      for (bool ok = spItr.first(addr, leng); ok; ok = spItr.next(addr, leng)) {
         if (singleRF) {
            cout << "      term prefixes" << termCounter << " {\n         from {\n";
         }
         cout << "            route-filter ";
         cout << ipv62hex(&addr, buffer) << "/" << leng << " exact";
         if (singleRF) {
            cout  << " " << permitOrDeny << ";\n";
            cout  << "         }\n      }\n";
         }
         else {
            cout  << ";\n";
         }
         termCounter++;
      }
   } else if (compression == COMP_NORMAL) {
      IPv6RadixSet::SortedPrefixRangeIterator sprItr(&nets.members);
      u_int start;
      u_int end;

      for (bool ok = sprItr.first(addr, leng, start, end); ok; ok = sprItr.next(addr, leng, start, end)) {
         if (singleRF) {
            cout << "      term prefixes" << termCounter << " {\n         from {\n";
         }
         cout << "            route-filter ";
         cout << ipv62hex(&addr, buffer) << "/" << leng;
         if (start != leng) {
            if (end != leng || start < leng)
               cout << " prefix-length-range /" << start << "-/" << end;
            else
               cout << " prefix-length-range /" << start << "-/" << start;
         } else {
            if (end != leng)
               cout << " upto /" << end;
            else
               cout << " exact";
         }
         if (singleRF) {
            cout  << " " << permitOrDeny << ";\n";
            cout  << "         }\n      }\n";
         }
         else {
            cout  << ";\n";
         }
         termCounter++;
      }
   } else if (compression == COMP_SUPER) {
      IPv6RadixSet::SortedIterator sItr(&nets.members);
      ipv6_addr_t rngs;

      for (bool ok = sItr.first(addr, leng, rngs); ok; ok = sItr.next(addr, leng, rngs)) {
         if (singleRF) {
            cout << "      term prefixes" << termCounter << " {\n         from {\n";
         }
         cout << "            route-filter ";
         cout << ipv62hex(&addr, buffer) << "/" << leng << " exact";
         if (singleRF) {
            cout  << " " << permitOrDeny << ";\n";
            cout  << "         }\n      }\n";
         }
         else {
            cout  << ";\n";
         }
         termCounter++;
      }
   } else {
     cerr << "Error: unknown compression type! Cannot print JunOS IPv6 prefix-list!\n";
     exit(1);
   }
   if (! singleRF) {
      cout  << "         }\n";
      cout  << "         then " << permitOrDeny << ";\n";
      cout  << "      }\n";
   }
   
   // terminate the prefix-list
   cout << "      term catch-rest {\n         then " << negatedPermitOrDeny << ";\n      }\n   }\n";

   return result;
}

LOf2Ints *JUNOSConfig::printRoutes(SetOfPrefix& nets) {
   /* This method has been changed to implement a work-around solving a problem with
      the JunOS route list evaluation process. It generates a single term for each
      "route-filter" line. This is done to prevent prefix loss that occured at least
      for prefixes of length 24 having the most-significant bit set in the first octet
      in the following example:
      term prefixes {
         from {
           route-filter 0.0.0.0/0 upto /9 accept;
           route-filter 0.0.0.0/0 prefix-length-range /11-/24 accept;
           route-filter 128.0.0.0/1 prefix-length-range /10-/10 accept;
         }
      }
      Due to JunOS longest-prefix-match, prefix 192.18.0.0/24 is not accepted since the
      third route-filter line matches the prefix but not its prefix-length-range.
   */

   // return the access list number if something is printed
   LOf2Ints *result;

   if (nets.universal())
      return NULL;

   const char *permitOrDeny;
   const char *negatedPermitOrDeny;

   if (nets.negated()) {
      permitOrDeny = "reject";
      negatedPermitOrDeny = "accept";
   } else {
      permitOrDeny = "accept";
      negatedPermitOrDeny = "reject";
   }

   cout << "\n   policy-statement " << prefixListName << " {\n";

   if (nets.members.isEmpty()) {
     cout << "      term prefixes {\n";
     if (nets.negated()) {
       cout << "         # IPv4 COMPLETE\n";
       cout << "         then accept;\n      }\n   }\n";
     } else {
       cout << "         # IPv4 EMPTY\n";
       cout << "         then reject;\n      }\n   }\n";
     }
     return result;
   }

   u_int addr;
   u_int leng;
   char buffer[64];
   u_int termCounter = 0; // counts terms to get unique term names
   bool singleRF = BaseConfig::singleRouteFilter;

   if (! singleRF) {
      cout << "      term prefixes {\n         from {\n";
   }
   if (compression == COMP_NONE) {
      RadixSet::SortedPrefixIterator spItr(&nets.members);

      for (bool ok = spItr.first(addr, leng); ok; ok = spItr.next(addr, leng)) {
         if (singleRF) {
            cout << "      term prefixes" << termCounter << " {\n         from {\n";
         }
         cout << "            route-filter ";
         cout << int2quad(buffer, addr) << "/" << leng << " exact";
         if (singleRF) {
            cout  << " " << permitOrDeny << ";\n";
            cout  << "         }\n      }\n";
         }
         else {
            cout  << ";\n";
         }
         termCounter++;
      }
   } else if (compression == COMP_NORMAL) {
      RadixSet::SortedPrefixRangeIterator sprItr(&nets.members);
      u_int start;
      u_int end;

      for (bool ok = sprItr.first(addr, leng, start, end); ok; ok = sprItr.next(addr, leng, start, end)) {
         if (singleRF) {
            cout << "      term prefixes" << termCounter << " {\n         from {\n";
         }
         cout << "            route-filter ";
         cout << int2quad(buffer, addr) << "/" << leng;
         if (start != leng) {
            if (end != leng || start < leng)
               cout << " prefix-length-range /" << start << "-/" << end;
            else
               cout << " prefix-length-range /" << start << "-/" << start;
         } else {
            if (end != leng)
               cout << " upto /" << end;
            else
               cout << " exact";
         }
         if (singleRF) {
            cout  << " " << permitOrDeny << ";\n";
            cout  << "         }\n      }\n";
         }
         else {
            cout  << ";\n";
         }
         termCounter++;
      }
   } else if (compression == COMP_SUPER) {
      RadixSet::SortedIterator sItr(&nets.members);
      u_int64_t rngs;

      for (bool ok = sItr.first(addr, leng, rngs); ok; ok = sItr.next(addr, leng, rngs)) {
         if (singleRF) {
            cout << "      term prefixes" << termCounter << " {\n         from {\n";
         }
         cout << "            route-filter ";
         cout << int2quad(buffer, addr) << "/" << leng << " exact";
         if (singleRF) {
            cout  << " " << permitOrDeny << ";\n";
            cout  << "         }\n      }\n";
         }
         else {
            cout  << ";\n";
         }
         termCounter++;
      }
   } else {
     cerr << "Error: unknown compression type! Cannot print JunOS IPv4 prefix-list!\n";
     exit(1);
   }
   if (! singleRF) {
      cout  << "         }\n";
      cout  << "         then " << permitOrDeny << ";\n";
      cout  << "      }\n";
   }

   // terminate the policy-statement
   cout << "      term catch-rest {\n         then " << negatedPermitOrDeny << ";\n      }\n   }\n";

   return result;
}

void JUNOSConfig::printREASno(ostream& out, const RangeList &no) {
   extern bool opt_asdot;
   RangeList::Range *pi, *qi;
   int first = 1;
   int put_par = 0;

   if (no.universal())
      out << " .";
   else {
      pi = no.ranges.head();
      put_par = ! no.ranges.isSingleton() || pi->high != pi->low;
      if (put_par)
	 out << "("; 
      else
	 out << " ";

      for (; pi; pi = no.ranges.next(pi)) {
	 if (!first) 
	    out << "|";
	 else
	    first = 0;

         if (opt_asdot && (pi->low > 65535))
            out << (pi->low >> 16) << "." << (pi->low & 0xffff);
         else
	    out << pi->low;

	 if (pi->low != pi->high)
	    if (opt_asdot && (pi->high > 65535))
               out << "-" << (pi->high >> 16) << "." << (pi->high & 0xffff);
	    else
	       out << "-" << pi->high;
      } 
      if (put_par)
         out << ")";
   }
}

int JUNOSConfig::printRE_(ostream& os, const regexp& r,
			  bool &hasBOL, bool &hasEOL) {
   int flag = 0;

   if (typeid(r) == typeid(regexp_bol)) {
      hasBOL = true;
      return flag;
   }

   if (typeid(r) == typeid(regexp_eol)) {
      hasEOL = true;
      return 1;
   }

   if (typeid(r) == typeid(regexp_symbol)) {
      printREASno(os, ((regexp_symbol &) r).asnumbers);
      return flag;
   }

   if (typeid(r) == typeid(regexp_star)) {
      bool save = reSplittable;
      reSplittable = false;
      os << "(";
      flag = printRE_(os, *((regexp_star &) r).left, hasBOL, hasEOL);
      os << ")*";
      reSplittable = save;
      return flag;
   }

   if (typeid(r) == typeid(regexp_plus)) {
      bool save = reSplittable;
      reSplittable = false;
      os << "(";
      flag = printRE_(os, *((regexp_plus &) r).left, hasBOL, hasEOL);
      os << ")+";
      reSplittable = save;
      return flag;
   }

   if (typeid(r) == typeid(regexp_tildastar)) {
      bool save = reSplittable;
      reSplittable = false;
      os << "(";
      flag = printRE_(os, *((regexp_tildastar &) r).left, hasBOL, hasEOL);
      os << ")*";
      reSplittable = save;
      return flag;
   }

   if (typeid(r) == typeid(regexp_tildaplus)) {
      bool save = reSplittable;
      reSplittable = false;
      os << "(";
      flag = printRE_(os, *((regexp_tildaplus &) r).left, hasBOL, hasEOL);
      os << ")+";
      reSplittable = save;
      return flag;
   }

   if (typeid(r) == typeid(regexp_question)) {
      os << "(";
      flag = printRE_(os, *((regexp_question &) r).left, hasBOL, hasEOL);
      os << ")?";
      return flag;
   }

   if (typeid(r) == typeid(regexp_cat)) {
      printRE_(os, *((regexp_cat &) r).left, hasBOL, hasEOL);
      flag = printRE_(os, *((regexp_cat &) r).right, hasBOL, hasEOL);
      return flag;
   }

   if (typeid(r) == typeid(regexp_or)) {
      os << "(";
      flag = printRE_(os, *((regexp_or &) r).left, hasBOL, hasEOL);
      os << "|";
      flag &= printRE_(os, *((regexp_or &) r).right, hasBOL, hasEOL);
      os  << ")";
      return flag;
   }

   assert(1);
   os << "REGEXP_UNKNOWN";
   return flag;
}

int JUNOSConfig::printRE(ostream& os, const regexp& r) {
   ostringstream out;
   bool hasBOL = false;
   bool hasEOL = false;
   printRE_(out, r, hasBOL, hasEOL);
   
   if (!hasBOL)
      os << ".* ";
   os << out.str();
   if (!hasEOL)
      os << " .*";   

   return 1;
}

LOf2Ints* JUNOSConfig::printASPaths(FilterOfASPath &path) {
// return the access list number if something is printed
   LOf2Ints *result;

   if (path.is_universal())
      return NULL;

   // check to see if we already printed an identical access list
   if (BaseConfig::useAclCaches && (result = junosASpathMgr.search(path)))
      return result;

   result = junosASpathMgr.add(path);
   int aclID = junosASpathMgr.newID();
   result->add(aclID, aclID);

   regexp *reg = path.re->construct();
   RangeList::prefix_str = "";
   RangeList::useOrList = true;

   regexp_nf::RegexpConjunct *rc;
   regexp_nf::RegexpConjunct::ReInt *ri;
   regexp_nf::RegexpConjunct::ReInt *positive_ri = NULL;

   cout << "   as-path as-path-" << asPathNumber << " \"";

   if (path.re->rclist.size() == 1) {
      rc = path.re->rclist.head(); 
      ri = rc->regexs.head();
      if (rc->regexs.isSingleton() && !ri->negated)
	 printRE(cout, *ri->re);
      else
	 cout << *reg;
   } else
      cout << *reg;

   cout << "\";\n\n";
   delete reg;

   return result;
}
