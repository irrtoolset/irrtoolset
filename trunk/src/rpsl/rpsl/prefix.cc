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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include "config.h"
#include <cstdio>
#include <cstring>
#include <iostream.h>
#include "prefix.hh"

static unsigned int masks[] ={ 0x00000000,
		 0x80000000, 0xC0000000, 0xE0000000, 0xF0000000,
		 0xF8000000, 0xFC000000, 0xFE000000, 0xFF000000,
		 0xFF800000, 0xFFC00000, 0xFFE00000, 0xFFF00000,
		 0xFFF80000, 0xFFFC0000, 0xFFFE0000, 0xFFFF0000,
		 0xFFFF8000, 0xFFFFC000, 0xFFFFE000, 0xFFFFF000, 
		 0xFFFFF800, 0xFFFFFC00, 0xFFFFFE00, 0xFFFFFF00,
		 0xFFFFFF80, 0xFFFFFFC0, 0xFFFFFFE0, 0xFFFFFFF0,
		 0xFFFFFFF8, 0xFFFFFFFC, 0xFFFFFFFE, 0xFFFFFFFF 
};

static unsigned long long int bits[] = { 0x100000000ULL,
		     0x080000000, 0x040000000, 0x020000000, 0x010000000,
		     0x008000000, 0x004000000, 0x002000000, 0x001000000,
		     0x000800000, 0x000400000, 0x000200000, 0x000100000,
		     0x000080000, 0x000040000, 0x000020000, 0x000010000,
		     0x000008000, 0x000004000, 0x000002000, 0x000001000,
		     0x000000800, 0x000000400, 0x000000200, 0x000000100,
		     0x000000080, 0x000000040, 0x000000020, 0x000000010,
		     0x000000008, 0x000000004, 0x000000002, 0x000000001
};

char PrefixRange::formattingbuffer[128];

char* int2quad(char *buffer, unsigned int i) {
   sprintf(buffer, "%d.%d.%d.%d", 
	   (i >> 24) & 0xFF, (i >> 16) & 0xFF, (i >> 8)  & 0xFF, i & 0xFF);
   return buffer;
}

unsigned int quad2int(char *quad) {
   unsigned int i;
   unsigned int i1, i2, i3, i4;

   sscanf(quad, "%u.%u.%u.%u", &i1, &i2, &i3, &i4);
   i = i4 + (i3 << 8) + (i2 << 16) + (i1 << 24);
   return i;
}

ostream& operator<<(ostream& stream, const PrefixRange& p) {
   stream << p.get_text();
   return stream;
}

/*
// Added const to PrefixRange& p by wlee@isi.edu
const ostream& operator<<(ostream& stream, const PrefixRange& p) {
  stream << p.get_text();
   return stream;
}
*/

PrefixRange::PrefixRange(void) : 
  ipaddr(0), 
  length(0), 
  n(0), 
  m(0)
{
}

// Added by wlee@isi.edu
PrefixRange::PrefixRange(const PrefixRange &p) :
  ipaddr(p.ipaddr),
  length(p.length),
  n(p.n),
  m(p.m)
{
}

PrefixRange::PrefixRange(unsigned int ipaddr, unsigned char length, 
		 unsigned char n, unsigned char m) :
  ipaddr(ipaddr),
  length(length),
  n(n),
  m(m)
{
}

PrefixRange::PrefixRange(char *name) : 
  ipaddr(0), 
  length(0), 
  n(0), 
  m(0)
{
  parse(name);
}

void PrefixRange::parse(char *name)
{
  unsigned int i1, i2, i3, i4, uiLength = 0, uiN = 0, uiM = 0;
  unsigned int mask;
  char ch = ' ';

  char *p;
  if (strstr(name, "^+"))
    // Inclusive more specific operation
    sscanf(name, "%u.%u.%u.%u/%u^%c", &i1, &i2, &i3, &i4, &uiLength, &ch);
  else if (p = strstr(name, "^-"))
     sscanf(name, "%u.%u.%u.%u/%u^%c", &i1, &i2, &i3, &i4, &uiLength, &ch);
  else if (p = strchr(name, '-')) 
     sscanf(name, "%u.%u.%u.%u/%u^%u-%u", 
	    &i1, &i2, &i3, &i4, &uiLength, &uiN, &uiM);
  else if (p = strchr(name, '^')) {
     sscanf(name, "%u.%u.%u.%u/%u^%u", 
	    &i1, &i2, &i3, &i4, &uiLength, &uiN);
     uiM = uiN;
  } else
     sscanf(name, "%u.%u.%u.%u/%u", &i1, &i2, &i3, &i4, &uiLength);

  length = uiLength;
  n      = uiN;
  m      = uiM;

  switch (ch)
    {
    case '+':
      // inclusive more specifics operator
      n = length;
      m = 32;
      break;
    case '-':
      // exclusive more specifics operator
      n = length + 1;
      m = 32;
      break;
    default:
      if (n == 0 || n < length) 
	 n = length;
      if (m == 0 || m < n) 
	 m = n;
      break;
    }

  ipaddr = i4 + (i3 << 8) + (i2 << 16) + (i1 << 24);
  
  ipaddr &= masks[length];
}

void PrefixRange::define(unsigned int ipaddr, unsigned char length, 
		     unsigned char n, unsigned char m)
{
  this->ipaddr = ipaddr;
  this->length = length;
  this->n      = n;
  this->m      = m;
}

bool PrefixRange::makeMoreSpecific(int code, int _n, int _m) {
   // we got ipaddr/length ^n-m  ^_n-_m
   // not defined if m < n 
   // ipaddr/length^max(n,_n)-_m othewise

   switch (code) {
   case 0: // ^-
      n++;
      m = 32;
      break;
   case 1: // ^+
      m = 32;
      break;
   default: // ^n-m
      if (_m < n || _m < n)
	 return false;
      n = (_n >? n);
      m = _m;
   }

   return true;
}

void PrefixRange::print(void)
{
  cout << get_text();
}

int PrefixRange::valid(void)
{
  if ((length <= n) && (n <= m)) return 1;
  return 0;
}

PrefixRange& PrefixRange::operator=(const PrefixRange& other)
{
  ipaddr = other.ipaddr;
  length = other.length;
  n      = other.n;
  m      = other.m;
  
  return *this;
}

// Does it make sense for the more/less specifics operators???
int PrefixRange::operator<(const PrefixRange& other) const
{
  return ipaddr < other.ipaddr || 
    (ipaddr == other.ipaddr && length < other.length);
}

// Does it make sense for the more/less specifics operators???
int PrefixRange::operator<=(const PrefixRange& other) const
{
  return ipaddr < other.ipaddr || 
    (ipaddr == other.ipaddr && length <= other.length);
}

int PrefixRange::operator==(const PrefixRange& other) const
{
  return ipaddr == other.ipaddr && length == other.length &&
         n == other.n && m == other.m;
}

int PrefixRange::compare(const PrefixRange& other) const
{
  if (*this < other)  return -1;
  if (*this == other) return 0;
  return 1;
}

int PrefixRange::contains(const PrefixRange& other) const
{
   return (length <= other.length && n <= other.n && m >= other.m
	   && ipaddr == (other.ipaddr & get_mask()));
}

unsigned int PrefixRange::get_mask() const {  
  return masks[length];
}

char *PrefixRange::get_text(char *buffer) const
{
  int2quad(buffer, ipaddr);
  sprintf(buffer + strlen(buffer), "/%u", length);

  if ((length == n) && (n == m))
    // just one route
    ;
  else
    if ((length == n) && (m == 32))
      // inclusive more specifics operator
      strcat(buffer, "^+");
    else
      if ((length == n - 1) && (m == 32))
	// exclusive more specifics operator
	strcat(buffer, "^-");
      else
	if (n == m)
	  sprintf(buffer + strlen(buffer), "^%u", n);
	else
	  sprintf(buffer + strlen(buffer), "^%u-%u", n, m);
  return buffer;
}

unsigned long long int PrefixRange::get_range() const {
   unsigned long long int range = 0;
   for (int i = n; i <= m; ++i)
      range |= bits[i];
   return range;
}


char *Prefix::get_text(char *buffer) const {
  int2quad(buffer, ipaddr);
  sprintf(buffer + strlen(buffer), "/%u", length);
  return buffer;  
}

char *IPAddr::get_text(char *buffer) const {
  int2quad(buffer, ipaddr);
  return buffer;  
}

Prefix::Prefix(char *name) {
   unsigned int i1, i2, i3, i4, i5;
   char slash;

   sscanf(name, "%u.%u.%u.%u%c%u", &i1, &i2, &i3, &i4, &slash, &i5);
   ipaddr = i4 + (i3 << 8) + (i2 << 16) + (i1 << 24);
   if (slash == '/')
      length = n = m = i5;
   else
      if (i4 == 0)
	 if (i3 == 0)
	    if (i2 == 0)
	       if (i1 == 0)
		  length = 0;
	       else
		  length = 8;
	    else
	       length = 16;
	 else
	    length = 24;
      else
	 length = 32;

   ipaddr &= masks[length];
}

IPAddr::IPAddr(char *name) {
   unsigned int i1, i2, i3, i4;

   sscanf(name, "%u.%u.%u.%u", &i1, &i2, &i3, &i4);
   ipaddr = i4 + (i3 << 8) + (i2 << 16) + (i1 << 24);
   length = n = m = 32;
}

PrefixRange NullPrefixRange("0.0.0.0/32^32-32");
Prefix      NullPrefix("0.0.0.0/32");
IPAddr      NullIPAddr("0.0.0.0");
