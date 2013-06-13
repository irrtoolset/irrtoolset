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

#include "f_rpsl.hh"
#include "config.h"
#include "rpsl/schema.hh"
#include "irrutil/version.hh"
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cmath>

//////////////////////////// methods ////////////////////////////////

void RPSLConfig::setRPSLExpr(const char *expr) {
   rpslExpr = expr;
}

void RPSLConfig::rsWrapUp() {
  // this method does postprocessing after a route-set object was printed.
  // printHeader is set to true for the next route-set object to print its header, and
  // a new line is printed to indicate that the object just printed terminates here.
  printHeader = true;
  cout << endl;
}

void RPSLConfig::printAccessList(SetOfPrefix& nets) {
      printRoutes(nets);
}

void RPSLConfig::printAccessList(SetOfIPv6Prefix& nets) {
      printRoutes(nets);
}
void RPSLConfig::printAspathAccessList(FilterOfASPath& path) {
      printASPaths(path);
}

void RPSLConfig::printHeaderLines(const char *setType, const char *setName) {
   time_t *theTime = (time_t *)calloc(sizeof(time_t), 1);
   tm *t;
   int month, day, hour, min, sec;

   // compute time stamp
   time(theTime);
   t = gmtime(theTime);
   char timeStamp[80] = "\0";

   month = t->tm_mon +1;
   day = t->tm_mday;
   hour = t->tm_hour;
   min = t->tm_min;
   sec = t->tm_sec;

   sprintf(timeStamp, "%04d-%02d-%02dT%02d:%02d:%02dZ\n", t->tm_year + 1900, month, day, hour, min, sec);

   cout << setType << "\t" << setName << "\n";
   cout << "descr:\t\tgenerated for: " << rpslExpr << "\n";
   cout << "remarks:\tgenerated from IRR      " << timeStamp;
   cout << "remarks:\tprogram used:  " << ProjectGoal << "  (" << ProjectVersion << ")" << endl;
   cout << "remarks:\t$Id$\n";
   cout << "remarks:\tsources: ";
   if (strcmp(IRR::dflt_sources, "") != 0)
      cout << IRR::dflt_sources << endl;
   else
     cout << "all available\n";
   cout << "remarks:\tdbserver: " << IRR::dflt_host << endl;
   cout << "remarks:\tflags used: -protocol " << protocolName[IRR::dflt_protocol].name
        << " -compression " << compTypes[compression].name << endl;
   printHeader = false;
}

LOf2Ints *RPSLConfig::printRoutes(SetOfIPv6Prefix& nets) {
   // return the access list number if something is printed
   LOf2Ints *result;
   u_int numLines = 0;

   if (nets.universal())
      return NULL;

   if (printHeader)
      printHeaderLines("route-set:", prefixListName);

#ifdef DIAG
   if (IRR::rpslInfo.getSize() > 0) {
     cout << "remarks:\tno routes found for the following RPSL object references:\n";
     cout << IRR::rpslInfo.getContents();
     IRR::rpslInfo.contents[0] = '\0';
     IRR::rpslInfo.size = 0;
     IRR::rpslInfo.offset = 0;
     IRR::rpslInfo.callerAllocated = false;
   }
#endif /* DIAG */

   if (nets.members.isEmpty()) {
     if (nets.negated()) {
       cout << "remarks:\tIPv6 COMPLETE\n";
       cout << "mp-members:\t::/0 le 128\n";
       cout << "#Counters\t??? prefixes\t1 lines\n";
     } else {
       cout << "remarks:\tIPv6 EMPTY\n";
       cout << "#Counters\t0" << " prefixes\t" << numLines << " lines\n";
     }
     return result;
   }

   // next is a hack to represent a negated ipv6 prefix-set correctly in RPSL. (hagen)
   if (nets.negated()) calculateNegation(nets);

   ipv6_addr_t addr;
   u_int leng;
   char buffer[256];

   if (compression == COMP_NONE) {
     IPv6RadixSet::SortedPrefixIterator spItr(&nets.members);

     for (bool ok = spItr.first(addr, leng); ok; ok = spItr.next(addr, leng)) {
       printIPv6Line(ipv62hex(&addr, buffer), leng);
       numLines++;
     }
     cout << "#Counters\t??? prefixes\t" << numLines << " lines\n";
   } else if (compression == COMP_NORMAL) {
     IPv6RadixSet::SortedPrefixRangeIterator sprItr(&nets.members);
     u_int start;
     u_int end;

     for (bool ok = sprItr.first(addr, leng, start, end); ok; ok = sprItr.next(addr, leng, start, end)) {
       if (leng == end)
         printIPv6Line(ipv62hex(&addr, buffer), leng);
       else
         printIPv6Line(ipv62hex(&addr, buffer), leng, start, end);
       numLines++;
     }
     cout << "#Counters\t??? prefixes\t" << numLines << " lines\n";
   } else if (compression == COMP_SUPER) {
     IPv6RadixSet::SortedIterator sItr(&nets.members);
     ipv6_addr_t rngs;

     for (bool ok = sItr.first(addr, leng, rngs); ok; ok = sItr.next(addr, leng, rngs)) {
       printIPv6Line(ipv62hex(&addr, buffer), leng);
       numLines++;
     }
     cout << "#Counters\t??? prefixes\t" << numLines << " lines\n";
   } else {
     cerr << "Error: unknown compression type! Cannot print RPSL IPv6 route-set!\n";
     exit(1);
   }

   return result;
}

void RPSLConfig::printIPv4Line(const char *ipAddr, int l, const char *spPtr) {
  int length = strlen(ipAddr) + 3;
  if (l >= 10) length++;

  spPtr += length;
  cout << "members:\t" << ipAddr << "/" << l << spPtr << "#1:\n";
}

void RPSLConfig::printIPv6Line(const char *ipAddr, int l) {
  cout << "mp-members:\t" << ipAddr << "/" << l << endl;
}

u_int64_t RPSLConfig::printIPv4Line(const char *ipAddr, int l, int s, int e, const char *spPtr) {
  int length = strlen(ipAddr) + 5;
  u_int64_t counter = (u_int64_t)(pow(2.0, (int)(1+e-l)) - pow(2.0, (int)(s-l)));

  if (l >= 10) length++;
  if (s >= 10) length++;

  if (s == e) {
    spPtr += length;
    cout << "members:\t" << ipAddr << "/" << l << "^" << s << spPtr << "#" << counter << ":\n";
  } else {
    if (e < 10)
      spPtr += length + 2;
    else
      spPtr += length + 3;
    cout << "members:\t" << ipAddr << "/" << l << "^" << s << "-" << e << spPtr << "#" << counter << ":\n";
  }
  return counter;
}

void RPSLConfig::printIPv6Line(const char *ipAddr, int l, int s, int e) {
  if (s == e)
    cout << "mp-members:\t" << ipAddr << "/" << l << "^" << s << endl;
  else
    cout << "mp-members:\t" << ipAddr << "/" << l << "^" << s << "-" << e << endl;
}

LOf2Ints *RPSLConfig::printRoutes(SetOfPrefix& nets) {
   // return the access list number if something is printed
   LOf2Ints *result;
   u_int64_t numPrefixes = 0;
   u_int numLines = 0;
   const char *spaces = "                         ";

   if (nets.universal())
      return NULL;

   if (printHeader)
      printHeaderLines("route-set:", prefixListName);

#ifdef DIAG
   if (IRR::rpslInfo.getSize() > 0) {
     cout << "remarks:\tno routes found for the following RPSL object references:\n";
     cout << IRR::rpslInfo.getContents();
     IRR::rpslInfo.contents[0] = '\0';
     IRR::rpslInfo.size = 0;
     IRR::rpslInfo.offset = 0;
     IRR::rpslInfo.callerAllocated = false;
   }
#endif /* DIAG */

   if (nets.members.isEmpty()) {
     if (nets.negated()) {
       numPrefixes = (u_int64_t)(pow(2.0, (int)(33)) - pow(2.0, (int)(0)));
       numLines = 1;
       cout << "remarks:\tIPv4 COMPLETE\n";
       cout << "members:\t0.0.0.0/0^0-32            #" << numPrefixes << ":\n";
     } else {
       cout << "remarks:\tIPv4 EMPTY\n";
     }
     cout << "#Counters\t" << numPrefixes << " prefixes\t" << numLines << " lines\n";
     return result;
   }

   // next is a hack to represent a negated prefix-set correctly in RPSL. (hagen)
   if (nets.negated()) calculateNegation(nets);

   u_int addr;
   u_int leng;
   char buffer[64];

   if (compression == COMP_NONE) {
     RadixSet::SortedPrefixIterator spItr(&nets.members);

     for (bool ok = spItr.first(addr, leng); ok; ok = spItr.next(addr, leng)) {
       printIPv4Line(int2quad(buffer, addr), leng, spaces);
       numPrefixes++;
       numLines++;
     }
     cout << "#Counters\t" << numPrefixes << " prefixes\t" << numLines << " lines\n";
   } else if (compression == COMP_NORMAL) {
     RadixSet::SortedPrefixRangeIterator sprItr(&nets.members);
     u_int start;
     u_int end;

     for (bool ok = sprItr.first(addr, leng, start, end); ok; ok = sprItr.next(addr, leng, start, end)) {
       if (leng == end) {
         printIPv4Line(int2quad(buffer, addr), leng, spaces);
         numPrefixes++;
       } else {
         numPrefixes += printIPv4Line(int2quad(buffer, addr), leng, start, end, spaces);
       }
       numLines++;
     }
     cout << "#Counters\t" << numPrefixes << " prefixes\t" << numLines << " lines\n";
   } else if (compression == COMP_SUPER) {
     RadixSet::SortedIterator sItr(&nets.members);
     u_int64_t rngs;

     for (bool ok = sItr.first(addr, leng, rngs); ok; ok = sItr.next(addr, leng, rngs)) {
       printIPv4Line(int2quad(buffer, addr), leng, spaces);
       numPrefixes++;
       numLines++;
     }
     cout << "#Counters\t" << numPrefixes << " prefixes\t" << numLines << " lines\n";
   } else {
     cerr << "Error: unknown compression type! Cannot print RPSL IPv4 route-set!\n";
     exit(1);
   }

   return result;
}

void RPSLConfig::printREASno(ostream& out, const RangeList &no) {}

int RPSLConfig::printRE_(ostream& os, const regexp& r) {
   return 0;
}

void RPSLConfig::printRE(ostream &s, const regexp &r, int aclID, bool permit){}

void RPSLConfig::printASSet(FilterOfSymbol symbs) {
   printHeaderLines("as-set:\t", asPathName);

#ifdef DIAG
   if (IRR::rpslInfo.getSize() > 0) {
     cout << "remarks:\tno AS numbers found for the following RPSL object references:\n";
     cout << IRR::rpslInfo.getContents();
     IRR::rpslInfo.contents[0] = '\0';
     IRR::rpslInfo.size = 0;
     IRR::rpslInfo.offset = 0;
     IRR::rpslInfo.callerAllocated = false;
   }
#endif /* DIAG */
   symbs.do_print(cout);
}

LOf2Ints* RPSLConfig::printASPaths(regexp_nf& path) {
// return the access list number if something is printed
   LOf2Ints *result;

   if (path.is_universal())
      return NULL;

   regexp_nf::RegexpConjunct *rc;
   regexp_nf::RegexpConjunct::ReInt *ri;
   RangeList numbers;
   bool value, complement;
   int i;

   // check whether there are no regular expressions in the path
   for (rc = path.rclist.head(); rc; rc = path.rclist.next(rc)) {
      for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
         if (typeid(*ri->re) != typeid(regexp_symbol)) {
            cerr << "Warning: unknown as-path expression! No regular expressions allowed for RPSL!\n";
            return NULL;
         }
      }
   }

   // compute bit-vector of final as-set.
   for (rc = path.rclist.head(); rc; rc = path.rclist.next(rc)) {
      for (ri = rc->regexs.head(); ri; ri = rc->regexs.next(ri)) {
 	 numbers = ((regexp_symbol &)(*ri->re)).asnumbers;

         if (ri->negated) {
            value = false;
            complement = true;
         } else {
            value = true;
            complement = false;
         }

         for (i=0; i<=65535; i++)
            literal[i] = complement;

         if (!numbers.universal()) {
            for (RangeList::Range *pi = numbers.ranges.head(); pi; pi = numbers.ranges.next(pi))
               for (int i = pi->low; i <= pi->high; ++i)
                  literal[i] = value;
         }

         for (i=0; i<=65535; i++)
            cterm[i] &= literal[i];
      }
      for (i=0; i<=65535; i++) {
 	 disjunction[i] |= cterm[i];
         cterm[i] = true;
      }
   }

   printHeaderLines("as-set:\t", asPathName);

#ifdef DIAG
   if (IRR::rpslInfo.getSize() > 0) {
     cout << "remarks:\tno AS numbers found for the following RPSL object references:\n";
     cout << IRR::rpslInfo.getContents();
     IRR::rpslInfo.contents[0] = '\0';
     IRR::rpslInfo.size = 0;
     IRR::rpslInfo.offset = 0;
     IRR::rpslInfo.callerAllocated = false;
   }
#endif /* DIAG */

   for (i=0; i<=65535; i++) {
     if (disjunction[i]) {
       cout << "members:\tAS" << i << "\n";
       disjunction[i] = false;
     }
   }

   return result;
}
