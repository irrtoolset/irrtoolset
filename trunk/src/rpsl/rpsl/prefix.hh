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

#ifndef PREFIX_HH
#define PREFIX_HH

#include "config.h"

class ostream;

char* int2quad(char *buffer, unsigned int i);
unsigned int quad2int(char *quad);

extern class PrefixRange NullPrefixRange;
extern class Prefix      NullPrefix;
extern class IPAddr      NullIPAddr;

class PrefixRange {
 protected:
   static char formattingbuffer[128];

   unsigned int  ipaddr;
   unsigned char length;
   unsigned char n;
   unsigned char m;

  public:
   PrefixRange(void);
   // Added by wlee@isi.edu
   PrefixRange(const PrefixRange &p);
   PrefixRange(unsigned int ipaddr, unsigned char length, 
	  unsigned char n, unsigned char m);

   PrefixRange(char *name);

   void define(unsigned int ipaddr, unsigned char length, 
	       unsigned char n, unsigned char m);

   // return false if it is an invalid operation and do nothing, 
   // or make the prefix more specific
   bool makeMoreSpecific(int code, int n, int m); 

   void print(void);
   int valid(void);

   bool isNull() const {
      return (*this) == NullPrefixRange;
   }

   PrefixRange& operator=(const PrefixRange& other);
   int operator<(const PrefixRange& other) const;
   int operator<=(const PrefixRange& other) const;
   int operator==(const PrefixRange& other) const;
   int operator!=(const PrefixRange& other) const {
      return ! (*this == other);
   }
   int compare(const PrefixRange& other) const;
   int contains(const PrefixRange& other) const;

   char *get_text(char *buffer = formattingbuffer) const;
   unsigned int get_ipaddr() const { return ipaddr; }
   unsigned int get_length() const { return length; }
   unsigned int get_n() const { return n; }
   unsigned int get_m() const { return m; }
   unsigned int get_mask() const;
   unsigned long long int get_range() const;

   friend ostream& operator<<(ostream& stream, const PrefixRange& p);

/*
   // Added const to PrefixRange& p by wlee@isi.edu
   friend const ostream& operator<<(ostream& stream, const PrefixRange& p);
*/

   void parse(char *name);
};

class Prefix : public PrefixRange {
public:
   Prefix() : PrefixRange(0, 0, 0, 0) {
   }
   Prefix(unsigned int ipaddr, unsigned char length) :
      PrefixRange(ipaddr, length, length, length) {
   }
   Prefix(char *name);
   void define(unsigned int ipaddr, unsigned char length) {
      PrefixRange::define(ipaddr, length, length, length);
   }
   char *get_text(char *buffer = formattingbuffer) const;
};

class IPAddr : public Prefix {
public:
   IPAddr() : Prefix(0, 0) {
   }
   IPAddr(unsigned int ipaddr) :
      Prefix(ipaddr, 32) {
   }
   IPAddr(char *name);
   void define(unsigned int ipaddr) {
      Prefix::define(ipaddr, 32);
   }
   char *get_text(char *buffer = formattingbuffer) const;
};

#endif // PREFIX_HH
