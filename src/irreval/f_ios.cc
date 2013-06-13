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

#include "f_ios.hh"
#include "config.h"
#include "rpsl/schema.hh"
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>

#define DBG_CISCO 7
#define EXPORT 0
#define IMPORT 1

//////////////////////////// caches ////////////////////////////////

AccessLManager<regexp_nf>         iosASpathMgr;
AccessLManager<SetOfPrefix>       iosPrefixMgr(100);
AccessLManager<SetOfIPv6Prefix>   iosIPv6PrefixMgr(100);

//////////////////////////// methods ////////////////////////////////

void IOSConfig::printAccessList(SetOfPrefix& nets) {
      bool save = BaseConfig::useAclCaches;
      BaseConfig::useAclCaches = false;
      printRoutes(nets);
      BaseConfig::useAclCaches = save;
}

void IOSConfig::printAccessList(SetOfIPv6Prefix& nets) {
      bool save = BaseConfig::useAclCaches;
      BaseConfig::useAclCaches = false;
      printRoutes(nets);
      BaseConfig::useAclCaches = save;
}
void IOSConfig::printAspathAccessList(FilterOfASPath& path) {
      bool save = BaseConfig::useAclCaches;
      BaseConfig::useAclCaches = false;
      printASPaths(path);
      BaseConfig::useAclCaches = save;
}

LOf2Ints *IOSConfig::printRoutes(SetOfIPv6Prefix& nets) {
   // return the access list number if something is printed
   LOf2Ints *result;

   if (nets.universal())
      return NULL;

   if (nets.members.isEmpty()) {
     if (nets.negated()) {
       cout << "!\n!IPv6 COMPLETE\n";
       cout << "no ipv6 prefix-list " << prefixListName << "\n";
       cout << "ipv6 prefix-list " << prefixListName << " permit ::/0 le 128\n";
     } else {
       cout << "!\n!IPv6 EMPTY\n";
       cout << "no ipv6 prefix-list " << prefixListName << "\n";
       cout << "ipv6 prefix-list " << prefixListName << " deny ::/0 le 128\n";
     }
     return result;
   }

   int allow_flag = 1;
   const char *permitOrDeny = " permit ";
   if (nets.negated()) {
      allow_flag = 0;
      permitOrDeny = " deny ";
   }

   // clear this prefix list
   cout << "!\nno ipv6 prefix-list " << prefixListName << "\n";

   ipv6_addr_t addr;
   u_int leng;
   char buffer[256];

   if (compression == COMP_NONE) {
     IPv6RadixSet::SortedPrefixIterator spItr(&nets.members);

     for (bool ok = spItr.first(addr, leng); ok; ok = spItr.next(addr, leng)) {
	cout << "ipv6 prefix-list " << prefixListName << permitOrDeny;
	cout << ipv62hex(&addr, buffer) << "/" << leng << endl;
     }
   } else if (compression == COMP_NORMAL) {
     IPv6RadixSet::SortedPrefixRangeIterator sprItr(&nets.members);
     u_int start;
     u_int end;

     for (bool ok = sprItr.first(addr, leng, start, end); ok; ok = sprItr.next(addr, leng, start, end)) {
        cout << "ipv6 prefix-list " << prefixListName << permitOrDeny;
        cout << ipv62hex(&addr, buffer) << "/" << leng;
        if (start != leng)
	  cout << " ge " << start;
        if (end != leng)
	  cout << " le " << end;
        cout << "\n";
     }
   } else if (compression == COMP_SUPER) {
     IPv6RadixSet::SortedIterator sItr(&nets.members);
     ipv6_addr_t rngs;

     for (bool ok = sItr.first(addr, leng, rngs); ok; ok = sItr.next(addr, leng, rngs)) {
	cout << "ipv6 prefix-list " << prefixListName << permitOrDeny;
	cout << ipv62hex(&addr, buffer) << "/" << leng << endl;
     }
   } else {
     cerr << "Error: unknown compression type! Cannot print IOS IPv6 prefix-list!\n";
     exit(1);
   }

   // terminate the prefix-list
   if (allow_flag)
      cout << "ipv6 prefix-list " << prefixListName << " deny ::/0 le 128\n";
   else
      cout << "ipv6 prefix-list " << prefixListName << " permit ::/0 le 128\n";

   return result;
}

LOf2Ints *IOSConfig::printRoutes(SetOfPrefix& nets) {
   // return the access list number if something is printed
   LOf2Ints *result;

   if (nets.universal())
      return NULL;

   if (nets.members.isEmpty()) {
     if (nets.negated()) {
       cout << "!\n!IPv4 COMPLETE\n";
       cout << "no ip prefix-list " << prefixListName << "\n";
       cout << "ip prefix-list " << prefixListName << " permit 0.0.0.0/0 le 32\n";
     } else {
       cout << "!\n!IPv4 EMPTY\n";
       cout << "no ip prefix-list " << prefixListName << "\n";
       cout << "ip prefix-list " << prefixListName << " deny 0.0.0.0/0 le 32\n";
     }
     return result;
   }

   int allow_flag = 1;
   const char *permitOrDeny = " permit ";
   if (nets.negated()) {
      allow_flag = 0;
      permitOrDeny = " deny ";
   }

   // clear this access list
   cout << "!\nno ip prefix-list " << prefixListName << "\n";

   u_int addr;
   u_int leng;
   char buffer[64];

   if (compression == COMP_NONE) {
     RadixSet::SortedPrefixIterator spItr(&nets.members);

     for (bool ok = spItr.first(addr, leng); ok; ok = spItr.next(addr, leng)) {
       cout << "ip prefix-list " << prefixListName << permitOrDeny;
       cout << int2quad(buffer, addr) << "/" << leng << endl;
     }
   } else if (compression == COMP_NORMAL) {
     RadixSet::SortedPrefixRangeIterator sprItr(&nets.members);
     u_int start;
     u_int end;

     for (bool ok = sprItr.first(addr, leng, start, end); ok; ok = sprItr.next(addr, leng, start, end)) {
       cout << "ip prefix-list " << prefixListName << permitOrDeny;
       cout << int2quad(buffer, addr) << "/" << leng;
       if (start != leng)
	 cout << " ge " << start;
       if (end != leng)
	 cout << " le " << end;
       cout << "\n";
     }
   } else if (compression == COMP_SUPER) {
     RadixSet::SortedIterator sItr(&nets.members);
     u_int64_t rngs;

     for (bool ok = sItr.first(addr, leng, rngs); ok; ok = sItr.next(addr, leng, rngs)) {
       cout << "ip prefix-list " << prefixListName << permitOrDeny;
       cout << int2quad(buffer, addr) << "/" << leng << endl;
     }
   } else {
     cerr << "Error: unknown compression type! Cannot print IOS IPv4 prefix-list!\n";
     exit(1);
   }

   // terminate the prefix-list
   if (allow_flag)
      cout << "ip prefix-list " << prefixListName << " deny 0.0.0.0/0 le 32\n";
   else
      cout << "ip prefix-list " << prefixListName << " permit 0.0.0.0/0 le 32\n";

   return result;
}

void IOSConfig::printREASno(ostream& out, const RangeList &no) {
   extern bool opt_asdot;
   RangeList::Range *pi, *qi;
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
  
  if (put_par)
     out << ")";
}
}

int IOSConfig::printRE_(ostream& os, const regexp& r) {
   static bool inAlternate = false;
   int flag = 0;

   if (typeid(r) == typeid(regexp_or)) {
      bool save = inAlternate;

      if (!inAlternate) {
	 inAlternate = true;
	 os << "(";
      }

      flag = IOSConfig::printRE_(os, *((regexp_or &) r).left);
      os << "|";
      flag &= IOSConfig::printRE_(os, *((regexp_or &) r).right);

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
      IOSConfig::printREASno(os, ((regexp_symbol &) r).asnumbers);
      return flag;
   }

   if (typeid(r) == typeid(regexp_star)) {
      bool save = reSplittable;
      reSplittable = BaseConfig::forceTilda;
      hasTilda = BaseConfig::forceTilda;
      inTilda = BaseConfig::forceTilda;
      os << "(";
      flag = IOSConfig::printRE_(os, *((regexp_star &) r).left);
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
      flag = IOSConfig::printRE_(os, *((regexp_plus &) r).left);
      os << ")+";
      reSplittable = save;
      return flag;
   }

   if (typeid(r) == typeid(regexp_tildastar)) {
      hasTilda = true;
      inTilda = true;
      os << "(";
      flag = IOSConfig::printRE_(os, *((regexp_star &) r).left);
      os << ")*";
      inTilda = false;
      return flag;
   }

   if (typeid(r) == typeid(regexp_tildaplus)) {
      hasTilda = true;
      inTilda = true;
      os << "(";
      flag = IOSConfig::printRE_(os, *((regexp_plus &) r).left);
      os << ")+";
      inTilda = false;
      return flag;
   }

   if (typeid(r) == typeid(regexp_question)) {
      os << "(";
      flag = IOSConfig::printRE_(os, *((regexp_question &) r).left);
      os << ")?";
      return flag;
   }

   if (typeid(r) == typeid(regexp_cat)) {
      IOSConfig::printRE_(os, *((regexp_cat &) r).left);
      flag = IOSConfig::printRE_(os, *((regexp_cat &) r).right);
      return flag;
   }

   assert(1);
   os << "REGEXP_UNKNOWN";
   return flag;
}

void IOSConfig::printRE(ostream &s, const regexp &r, int aclID, bool permit){
   ostringstream out;

   out << "ip as-path access-list " << aclID << (permit ? " permit " : " deny ");

   reSplittable = true;
   hasTilda = false;
   inTilda = false;

   if (!IOSConfig::printRE_(out, r)) // true if the expression was missing $
      out << "_";
   out << "\n";

   int lineLen = out.str().length();
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
}

LOf2Ints* IOSConfig::printASPaths(regexp_nf& path) {
// return the access list number if something is printed
   LOf2Ints *result;

   if (path.is_universal())
      return NULL;

   // check to see if we already printed an identical access list
   if (BaseConfig::useAclCaches && (result = iosASpathMgr.search(path)))
      return result;

   result = iosASpathMgr.add(path);

   regexp_nf::RegexpConjunct *rc;
   regexp_nf::RegexpConjunct::ReInt *ri;
   regexp_nf::RegexpConjunct::ReInt *positive_ri = NULL;

   // we will print as few access lists as necessary, often more than one
   // to minimize the number of access lists generated, we will do 3 passes
   // pass 1: RegexpConjuncts w/ one positive as path and no negative
   // pass 2: RegexpConjuncts w/ one or zero positive as path and negatives
   // pass 3: rest

   bool needNewAcl = true;

   // pass 1
   for (rc = path.rclist.head(); rc; rc = path.rclist.next(rc)) {
      ri = rc->regexs.head();
      if (rc->regexs.isSingleton() && !ri->negated) {
	 if (needNewAcl) {
	    needNewAcl = false;
            cout << "!\nno ip as-path access-list " << asPathNumber << "\n"; 
         }
         IOSConfig::printRE(cout, *ri->re, asPathNumber, true);
	 rc->mark = 1;
      } else
	 rc->mark = 0;
   }

   // pass 2
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
	 rc->mark = 1;
	 if (needNewAcl)
	    cout << "!\nno ip as-path access-list " << asPathNumber << "\n"; 

	 for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
	    if (!ri->negated)
	       positive_ri = ri;
	    else
	       IOSConfig::printRE(cout, *ri->re, asPathNumber, false);

	 }
	 if (positive_ri)
	    IOSConfig::printRE(cout, *positive_ri->re, asPathNumber, true);
	 else
	    cout << "ip as-path access-list " << asPathNumber << " permit .*\n";

	 needNewAcl = true;
      }
   }

   // pass 3
   for (rc = path.rclist.head(); rc; rc = path.rclist.next(rc)) {
      if (rc->mark) // done in pass 1 or 2, skip
	 continue;

      cout << "!\nno ip as-path access-list  " << asPathNumber << "\n"; 

      // print negative ones all in one access list
      for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
	 if (ri->negated) {
	    IOSConfig::printRE(cout, *ri->re, asPathNumber, false);
	 }
      }
      // print positives ones each in its own access list, except first
      int first = 1;
      for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
	 if (!ri->negated) {
	    if (first)
	       first = 0;
	    else
	       cout << "!\nno ip as-path access-list  " << asPathNumber << "\n"; 

	    IOSConfig::printRE(cout, *ri->re, asPathNumber, true);
	 }
      }

      if (first) // no positive one found
	 cout << "ip as-path access-list " << asPathNumber << " permit .*\n";

   }

   return result;
}
