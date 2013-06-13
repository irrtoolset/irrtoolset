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

#ifndef RPSLCONFIG_H
#define RPSLCONFIG_H

#include "f_base.hh"
#include "normalform/SetOfPrefix.hh"
#include "normalform/SetOfIPv6Prefix.hh"
#include "normalform/FilterOfSymbol.hh"

class RPSLConfig : public BaseConfig {
public:
   RPSLConfig() : BaseConfig() {
      printHeader = true;
      rpslExpr = "";
      for (int i=0; i<=65535; i++) {
        disjunction[i] = false;
        cterm[i] = true;
      }  
   }

public:
   void setRPSLExpr(const char *expr);
   void printASSet(FilterOfSymbol symbs);
   void rsWrapUp(void);

private:
   void printAccessList(SetOfPrefix& nets);
   void printAccessList(SetOfIPv6Prefix& nets);
   void printAspathAccessList(FilterOfASPath& path);

   LOf2Ints *printRoutes(SetOfPrefix& nets);
   LOf2Ints *printRoutes(SetOfIPv6Prefix& nets);
   LOf2Ints *printASPaths(regexp_nf& path);

   void printREASno(std::ostream& out, const RangeList &no);
   int  printRE_(std::ostream& os, const regexp& r);
   void printRE(std::ostream& os, const regexp& r, int aclID, bool permit);

   void printIPv4Line(const char *ipAddr, int l, const char *spPtr);
   void printIPv6Line(const char *ipAddr, int l);
   u_int64_t printIPv4Line(const char *ipAddr, int l, int s, int e, const char *spPtr);
   void printIPv6Line(const char *ipAddr, int l, int s, int e);
   void printHeaderLines(const char *setType, const char *setName);

private:
   const char *rpslExpr;
   bool printHeader;
   bool disjunction[65536];
   bool cterm[65536];
   bool literal[65536];
};

#endif   // RPSLCONFIG_H
