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
//             Katie Petrusha <katie@ripe.net>

#include "config.h"
#include <cstdio>
#include <cstring>
#include <iostream.h>
#include "prefix.hh"
#include <stdlib.h>
#include <sys/types.h>
//#include "afi.hh"

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

static ip_v6word_t ipv6bits[] = { 
0x0000000000000000LL,
0x8000000000000000LL,0x4000000000000000LL,0x2000000000000000LL, 0x1000000000000000LL,
0x0800000000000000LL,0x0400000000000000LL,0x0200000000000000LL, 0x0100000000000000LL,
0x0080000000000000LL,0x0040000000000000LL,0x0020000000000000LL, 0x0010000000000000LL,
0x0008000000000000LL,0x0004000000000000LL,0x0002000000000000LL, 0x0001000000000000LL,
0x0000800000000000LL,0x0000400000000000LL,0x0000200000000000LL, 0x0000100000000000LL,
0x0000080000000000LL,0x0000040000000000LL,0x0000020000000000LL, 0x0000010000000000LL,
0x0000008000000000LL,0x0000004000000000LL,0x0000002000000000LL, 0x0000001000000000LL,
0x0000000800000000LL,0x0000000400000000LL,0x0000000200000000LL, 0x0000000100000000LL,
0x0000000080000000LL,0x0000000040000000LL,0x0000000020000000LL, 0x0000000010000000LL,
0x0000000008000000LL,0x0000000004000000LL,0x0000000002000000LL, 0x0000000001000000LL,
0x0000000000800000LL,0x0000000000400000LL,0x0000000000200000LL, 0x0000000000100000LL,
0x0000000000080000LL,0x0000000000040000LL,0x0000000000020000LL, 0x0000000000010000LL,
0x0000000000008000LL,0x0000000000004000LL,0x0000000000002000LL, 0x0000000000001000LL,
0x0000000000000800LL,0x0000000000000400LL,0x0000000000000200LL, 0x0000000000000100LL,
0x0000000000000080LL,0x0000000000000040LL,0x0000000000000020LL, 0x0000000000000010LL,
0x0000000000000008LL,0x0000000000000004LL,0x0000000000000002LL, 0x0000000000000001LL
};

static ip_v6word_t ipv6masks[] = {
0x0000000000000000LL,
0x8000000000000000LL,0xC000000000000000LL,0xE000000000000000LL,0xF000000000000000LL, 
0xF800000000000000LL,0xFC00000000000000LL,0xFE00000000000000LL,0xFF00000000000000LL, 
0xFF80000000000000LL,0xFFC0000000000000LL,0xFFE0000000000000LL,0xFFF0000000000000LL, 
0xFFF8000000000000LL,0xFFFC000000000000LL,0xFFFE000000000000LL,0xFFFF000000000000LL, 
0xFFFF800000000000LL,0xFFFFC00000000000LL,0xFFFFE00000000000LL,0xFFFFF00000000000LL, 
0xFFFFF80000000000LL,0xFFFFFC0000000000LL,0xFFFFFE0000000000LL,0xFFFFFF0000000000LL, 
0xFFFFFF8000000000LL,0xFFFFFFC000000000LL,0xFFFFFFE000000000LL,0xFFFFFFF000000000LL, 
0xFFFFFFF800000000LL,0xFFFFFFFC00000000LL,0xFFFFFFFE00000000LL,0xFFFFFFFF00000000LL, 
0xFFFFFFFF80000000LL,0xFFFFFFFFC0000000LL,0xFFFFFFFFE0000000LL,0xFFFFFFFFF0000000LL, 
0xFFFFFFFFF8000000LL,0xFFFFFFFFFC000000LL,0xFFFFFFFFFE000000LL,0xFFFFFFFFFF000000LL, 
0xFFFFFFFFFF800000LL,0xFFFFFFFFFFC00000LL,0xFFFFFFFFFFE00000LL,0xFFFFFFFFFFF00000LL, 
0xFFFFFFFFFFF80000LL,0xFFFFFFFFFFFC0000LL,0xFFFFFFFFFFFE0000LL,0xFFFFFFFFFFFF0000LL, 
0xFFFFFFFFFFFF8000LL,0xFFFFFFFFFFFFC000LL,0xFFFFFFFFFFFFE000LL,0xFFFFFFFFFFFFF000LL, 
0xFFFFFFFFFFFFF800LL,0xFFFFFFFFFFFFFC00LL,0xFFFFFFFFFFFFFE00LL,0xFFFFFFFFFFFFFF00LL, 
0xFFFFFFFFFFFFFF80LL,0xFFFFFFFFFFFFFFC0LL,0xFFFFFFFFFFFFFFE0LL,0xFFFFFFFFFFFFFFF0LL, 
0xFFFFFFFFFFFFFFF8LL,0xFFFFFFFFFFFFFFFCLL,0xFFFFFFFFFFFFFFFELL,0xFFFFFFFFFFFFFFFFLL
};

ipv6_addr_t mask;
ipv6_addr_t range;
ipv6_addr_t t;

char PrefixRange::formattingbuffer[128];
char IPv6PrefixRange::formattingbuffer[256];

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

IPv6PrefixRange NullIPv6PrefixRange("0::0/128^128-128");
IPv6Prefix      NullIPv6Prefix("0::0/128");
IPv6Addr        NullIPv6Addr("0::0");
ipv6_addr_t     NullIPv6(0,0);

/* IPv6 stuff */

// ipv6_addr_t to string - pointers should be provided
char* ipv62hex(ipv6_addr_t *ip, char *buffer){
    ip_v6word_t high = ip->high;
    ip_v6word_t low = ip->low;

   sprintf (buffer, "%llX:%llX:%llX:%llX:%llX:%llX:%llX:%llX", (high >> 48) & 0xFFFF, (high >> 32) & 0xFFFF, (high >> 16) & 0xFFFF, high & 0xFFFF, (low >> 48) & 0xFFFF, (low >> 32) & 0xFFFF, (low >> 16) & 0xFFFF, low & 0xFFFF);

  return buffer;
}

// string to ipv6_addr_t - pointers should be provided
ipv6_addr_t* hex2ipv6(char *hex, ipv6_addr_t *ip) {

   char *cur = hex;
   char *dots = NULL;
   char *str = (char *) calloc (IPV6_LENGTH,1);
   int i;
   int count=0;
   ip_v6word_t high;
   ip_v6word_t low;
   unsigned int i1, i2, i3, i4, i5, i6, i7, i8;

   do {
     if ((*cur == ':') && (*(cur+1) == ':')) {
       dots = cur;
       cur = cur + 2;
     }
     else if (*cur != ':') {
       ++count;
       do {
          ++cur;
       } while (cur <= (hex + strlen(hex)) && (*cur != ':'));
     }
     else { // single ':'
       cur ++;
     }
   } while (cur < (hex + strlen(hex)));

   if (dots != NULL) {
     if (dots == hex) { // starting '::'
       for (i=1; i<=(8-count); ++i)
         strcat(str, "0:");
       strcat (str, dots + 2);
     }
     else if (dots == hex + strlen (hex) - 2) { // ending '::'
       strncat (str, hex, strlen (hex) - 2);
       for (i=1; i<=(8-count); ++i)
         strcat(str, ":0");
     }
     else { // middle '::'
       strncat (str, hex, strlen (hex) - strlen(dots) + 1);
       for (i=1; i<=(8-count); ++i)
         strcat(str, "0:");
       strcat (str, dots + 2);
     }
   }
   else { // no '::'
     strcpy (str, hex);
   }

   sscanf(str, "%x:%x:%x:%x:%x:%x:%x:%x", &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8);
//   printf ("INTEGERS: %d %d %d %d %d %d %d %d \n", str, i1,i2,i3,i4,i5,i6,i7,i8);

   high = ((ip_v6word_t) i1 << 48) + ((ip_v6word_t) i2 << 32) + ((ip_v6word_t) i3 << 16) + (ip_v6word_t) i4;
   low = ((ip_v6word_t) i5 << 48) + ((ip_v6word_t) i6 << 32) + ((ip_v6word_t) i7 << 16) + (ip_v6word_t) i8;

   ip->high = high;
   ip->low = low;

   free(str);
   return(ip);

}

// IPv6PrefixRange class
// constructor from void; points ipv6 struct to NULL
IPv6PrefixRange::IPv6PrefixRange(void) :
  ipaddr(NULL),
  length(0),
  n(0),
  m(0)
{
}

// constructor from pointer - copy
IPv6PrefixRange::IPv6PrefixRange(const IPv6PrefixRange &p) :
  ipaddr(NULL),
  length(p.length),
  n(p.n),
  m(p.m)
{
  ipaddr = (ipv6_addr_t *) calloc (sizeof (ipv6_addr_t), 1);
  *ipaddr = *(p.ipaddr);
}

// initialisation
IPv6PrefixRange::IPv6PrefixRange(ipv6_addr_t *ipaddr, unsigned char length,
     unsigned char n, unsigned char m) :
  ipaddr(ipaddr),
  length(length),
  n(n),
  m(m)
{
}

// constructor from string
IPv6PrefixRange::IPv6PrefixRange(char *name) :
  ipaddr(NULL),
  length(0),
  n(0),
  m(0)
{
  parse(name);
}
// parse generic ipv6 prefix range 
void IPv6PrefixRange::parse(char *name)
{
  char *slash;
  char *address =  (char *) calloc (IPV6_LENGTH,1);
  char *p;
  char ch = ' ';

  ipaddr = (ipv6_addr_t *) calloc (sizeof(ipv6_addr_t),1);
  ipv6_addr_t t = *ipaddr;

  if (slash = strstr (name, "/")) {
    strncat (address, name, strlen (name) - strlen(slash));
    //printf ("original: %s\n", name);
    //printf ("address part: %s\n", address);
    hex2ipv6(address, ipaddr);
    //printf ("integers: %lld %lld\n", ipaddr->high, ipaddr->low);
    ipv62hex(ipaddr, address);
    //printf ("normalized: %s\n", address);
    // go further
    slash ++;
    if (strstr(slash, "^+")) {
      sscanf(slash, "%u^%c", &length, &ch);
    }
    else if (p = strstr(slash, "^-")) {
      sscanf(slash, "%u^%c", &length, &ch);
    } 
    else if (p = strchr(slash, '-')) {
      sscanf(slash, "%u^%u-%u", &length, &n, &m);
    }
    else if (p = strchr(slash, '^')) {
      sscanf(slash, "%u^%u", &length, &n);
      m = n;
    }
    else {
      sscanf(slash, "%u", &length);
    }
    switch (ch)
    {
    case '+':
      // inclusive more specifics operator
      n = length;
      m = 128;
      break;
    case '-':
      // exclusive more specifics operator
      n = length + 1;
      m = 128;
      break;
    default:
      if (n == 0 || n < length)
        n = length;
      if (m == 0 || m < n)
        m = n;
      break;
    }
  } 
  else {
    // just an IP address
    strncat (address, name, strlen (name));
    hex2ipv6(address, ipaddr);
    ipv62hex(ipaddr, address);
    //printf ("normalized: %s\n", address);
    length = 128;
    n = 128;
    m = 128;
  }
  t = *ipaddr;
  t = t & (t.getmask(length));
  *ipaddr = t;

  free (address);
}

void IPv6PrefixRange::define(ipv6_addr_t *ipaddr, unsigned char length, 
                     unsigned char n, unsigned char m)
{
  *(this->ipaddr) = *ipaddr;
  this->length = length;
  this->n      = n;
  this->m      = m;
}

bool IPv6PrefixRange::makeMoreSpecific(int code, int _n, int _m) {
   // we got ipaddr/length ^n-m  ^_n-_m
   // not defined if m < n 
   // ipaddr/length^max(n,_n)-_m othewise

   switch (code) {
   case 0: // ^-
      n++;
      m = 128;
      break;
   case 1: // ^+
      m = 128;
      break;
   default: // ^n-m
      if (_m < n || _m < n)
         return false;
      n = (_n >? n);
      m = _m;
   }

   return true;
}

void IPv6PrefixRange::print(void)
{
  cout << get_text();
}

char *IPv6PrefixRange::get_text(char *buffer) const
{
  ipv62hex(ipaddr, buffer);
  sprintf(buffer + strlen(buffer), "/%u", length);

  if ((length == n) && (n == m))
    // just one route
    ;
  else
    if ((length == n) && (m == 128))
      // inclusive more specifics operator
      strcat(buffer, "^+");
    else
      if ((length == n - 1) && (m == 128))
        // exclusive more specifics operator
        strcat(buffer, "^-");
      else
        if (n == m)
          sprintf(buffer + strlen(buffer), "^%u", n);
        else
          sprintf(buffer + strlen(buffer), "^%u-%u", n, m);
  return buffer;
}

int IPv6PrefixRange::valid(void)
{
  if ((length <= n) && (n <= m) && (length <= 128) && (m <= 128) ) return 1;
  return 0;
}

IPv6PrefixRange& IPv6PrefixRange::operator=(const IPv6PrefixRange& other)
{
  *ipaddr = *(other.ipaddr);
  length = other.length;
  n      = other.n;
  m      = other.m;
  
  return *this;
}

int IPv6PrefixRange::operator<(const IPv6PrefixRange& other) const
{
    return (ipaddr < other.ipaddr ||
             ( (ipaddr == other.ipaddr) &&
               (length < other.length) )
           );
}
int IPv6PrefixRange::operator<=(const IPv6PrefixRange& other) const
{
    return (ipaddr < other.ipaddr ||
             ( (ipaddr == other.ipaddr) &&
               (length <= other.length) )
           );
}

int IPv6PrefixRange::operator==(const IPv6PrefixRange& other) const
{
  return ipaddr == other.ipaddr &&
         length == other.length &&
         n == other.n && m == other.m;
}

ostream& operator<<(ostream& stream, const IPv6PrefixRange& p) {
   stream << p.get_text();
   return stream;
}

int IPv6PrefixRange::compare(const IPv6PrefixRange& other) const
{
  if (*this < other)  return -1;
  if (*this == other) return 0;
  return 1;
}

int IPv6PrefixRange::contains(const IPv6PrefixRange& other) const
{
   return (length <= other.length && n <= other.n && m >= other.m
           && (*ipaddr == (*(other.ipaddr) & get_mask() ))
          );

}

ipv6_addr_t IPv6PrefixRange::get_mask() const {  
 
   if (length <= 64) {
      mask.high = ipv6masks[length];
      mask.low = ipv6masks[0];
   } else {
     mask.high = ipv6masks[64];
     mask.low = ipv6masks[length-64];
   }

   return mask;

}

ipv6_addr_t IPv6PrefixRange::get_range() const {
   range.high = 0;
   range.low = 0;
   int i;
   
   if ( (n <= 64) && (m <= 64) ) {
      range.low |= ipv6bits[0];
      for (i = n; i <= m; ++i) {
        range.high |= ipv6bits[i];
      }
   }
   if ( (n <= 64) && (m > 64) )  {
      for (i = n; i <= 64; ++i) {
        range.high |= ipv6bits[i];
      }
      for (i = 0; i <= (m-64); ++i) {
        range.low |= ipv6bits[i];
      }

   }
   if ( (n > 64) && (m > 64) )  {
      range.high |= ipv6bits[0];
      for (i = (n-64); i <= (m-64); ++i) {
        range.low |= ipv6bits[i];
      }

   }

   return range;
}


/* class IPv6Prefix */

// IPv6Prefix constructor 
IPv6Prefix::IPv6Prefix(char *name)
{
  parse(name);
}

char *IPv6Prefix::get_text(char *buffer) const {
  ipv62hex(ipaddr, buffer);
  sprintf(buffer + strlen(buffer), "/%u", length);
  return buffer;  
}

ostream& operator<<(ostream& stream, const IPv6Prefix& p) {
   stream << p.get_text();
   return stream;
}

/* class IPv6Addr */

char *IPv6Addr::get_text(char *buffer) const {
  ipv62hex(ipaddr, buffer);
  return buffer;  
}

// IPv6Addr constructor
IPv6Addr::IPv6Addr(char *name)
{
  parse(name);
}

ostream& operator<<(ostream& stream, const IPv6Addr& p) {
   stream << p.get_text();
   return stream;
}

ostream& operator<<(ostream& stream, const MPPrefix& p) {
//    if (p.afi != NULL) {
//      stream << *(p.afi);
//       stream << ' ';
//    }

    if (p.ipv4 != NULL)
      stream << *p.ipv4;
    else 
      stream << *p.ipv6;
    return stream;
  }

/*
bool MPPrefix::is_valid() {

  if ((ipv4 && afi->is_valid(ipv4)) ||
      (ipv6 && afi->is_valid(ipv6)))
      return true;
  return false;
}
*/

void MPPrefix::define(unsigned int masklen) {
  
  if (ipv4) {
     ipv4->length = masklen;
     ipv4->n = masklen;
     ipv4->m = masklen;
  }
  else {
     ipv6->length = masklen;
     ipv6->n = masklen;
     ipv6->m = masklen;
  }
}

unsigned int MPPrefix::get_length() const{
  if (ipv4)
     return ipv4->get_length();
  else
     return ipv6->get_length();
}

unsigned int MPPrefix::get_n() {
  if (ipv4)
     return ipv4->get_n();
  else
     return ipv6->get_n();
}

unsigned int MPPrefix::get_m() {
  if (ipv4)
     return ipv4->get_m();
  else
     return ipv6->get_m();
}

ipv6_addr_t MPPrefix::get_mask() const {
  if (ipv6)
     return ipv6->get_mask();
}

ipv6_addr_t MPPrefix::get_ipaddr() const {
  if (ipv6)
     return *(ipv6->get_ipaddr());
}

ipv6_addr_t MPPrefix::get_range() const {
  if (ipv6)
     return ipv6->get_range();
}

int operator<(ipv6_addr_t one, ipv6_addr_t two)
{
  if ( (one.high < two.high) || ( (one.high == two.high) && (one.low < two.low) )) 
    return 1;
  else 
    return 0;
}

int operator!=(ipv6_addr_t one, ipv6_addr_t two)
{
  if ((one.high != two.high) || (one.low != two.low))
    return 1;
  else 
    return 0;
}

int operator==(ipv6_addr_t one, ipv6_addr_t two)
{  
  if ((one.high == two.high) && (one.low == two.low))
    return 1;
  else 
    return 0;
}

ipv6_addr_t& operator&(ipv6_addr_t one, ipv6_addr_t two)
{
 
  t.high = one.high & two.high;
  t.low = one.low & two.low;

  return t;
   
}

ipv6_addr_t& operator|(ipv6_addr_t one, ipv6_addr_t two)
{

  t.high = one.high | two.high;
  t.low = one.low | two.low;
  return t;

}

ipv6_addr_t& ipv6_addr_t::operator|(unsigned int i)
{

  t.high = high;
  t.low  = low | i;

  return t;

}

ipv6_addr_t& ipv6_addr_t::operator<<(unsigned int i)
{

  t.high = (high << i) | (low >> (64-i));
  t.low  = low << i;

  return t;

}

ipv6_addr_t& ipv6_addr_t::operator>>(unsigned int i)
{

  t.low = (low >> i) | (high << (64-i));
  t.high  = high >> i;
  
  return t;

}

bool ipv6_addr_t::operator&&(bool b) {

  if ((high && b) && (low && b)) 
    return true;
  return false;

}

bool ipv6_addr_t::is_true() const {

  if (*this == NullIPv6)
    return false;
  return true;

}

ipv6_addr_t& ipv6_addr_t::operator~() const
{

  t.high = ~ high;
  t.low = ~ low;
  return t;

}

int ipv6_addr_t::operator!() const
{
  return (!high && !low);
}

ipv6_addr_t& ipv6_addr_t::getbits(unsigned int len)
{

  if (len <= 64) {
    t.high = ipv6bits[len];
    t.low = ipv6bits[0];
  } else {
    t.high = ipv6bits[0];
    t.low = ipv6bits[len-64];
  }

  return t;

}

ipv6_addr_t& ipv6_addr_t::getmask(unsigned int len)
{ 

   if (len <= 64) {
      t.high = ipv6masks[len];
      t.low = ipv6masks[0];
   } else {
      t.high = ipv6masks[64];
      t.low = ipv6masks[len-64];
   }

  return t;

}

ostream& operator<<(ostream& stream, const ipv6_addr_t& p) {
   char buf[40];
   ipv62hex((ipv6_addr_t *) &p, buf); 
   stream << buf;
   return stream;
}

