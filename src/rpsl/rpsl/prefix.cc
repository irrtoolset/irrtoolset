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
#include <algorithm>
#include <iterator>

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
0x0000000000000000ULL, // extra bit is used to cover numbers > 64 bit

0x8000000000000000ULL,0x4000000000000000ULL,0x2000000000000000ULL, 0x1000000000000000ULL,
0x0800000000000000ULL,0x0400000000000000ULL,0x0200000000000000ULL, 0x0100000000000000ULL,
0x0080000000000000ULL,0x0040000000000000ULL,0x0020000000000000ULL, 0x0010000000000000ULL,
0x0008000000000000ULL,0x0004000000000000ULL,0x0002000000000000ULL, 0x0001000000000000ULL,
0x0000800000000000ULL,0x0000400000000000ULL,0x0000200000000000ULL, 0x0000100000000000ULL,
0x0000080000000000ULL,0x0000040000000000ULL,0x0000020000000000ULL, 0x0000010000000000ULL,
0x0000008000000000ULL,0x0000004000000000ULL,0x0000002000000000ULL, 0x0000001000000000ULL,
0x0000000800000000ULL,0x0000000400000000ULL,0x0000000200000000ULL, 0x0000000100000000ULL,
0x0000000080000000ULL,0x0000000040000000ULL,0x0000000020000000ULL, 0x0000000010000000ULL,
0x0000000008000000ULL,0x0000000004000000ULL,0x0000000002000000ULL, 0x0000000001000000ULL,
0x0000000000800000ULL,0x0000000000400000ULL,0x0000000000200000ULL, 0x0000000000100000ULL,
0x0000000000080000ULL,0x0000000000040000ULL,0x0000000000020000ULL, 0x0000000000010000ULL,
0x0000000000008000ULL,0x0000000000004000ULL,0x0000000000002000ULL, 0x0000000000001000ULL,
0x0000000000000800ULL,0x0000000000000400ULL,0x0000000000000200ULL, 0x0000000000000100ULL,
0x0000000000000080ULL,0x0000000000000040ULL,0x0000000000000020ULL, 0x0000000000000010ULL,
0x0000000000000008ULL,0x0000000000000004ULL,0x0000000000000002ULL, 0x0000000000000001ULL
};

static ip_v6word_t ipv6masks[] = {
0x0000000000000000ULL,
0x8000000000000000ULL,0xC000000000000000ULL,0xE000000000000000ULL,0xF000000000000000ULL, 
0xF800000000000000ULL,0xFC00000000000000ULL,0xFE00000000000000ULL,0xFF00000000000000ULL, 
0xFF80000000000000ULL,0xFFC0000000000000ULL,0xFFE0000000000000ULL,0xFFF0000000000000ULL, 
0xFFF8000000000000ULL,0xFFFC000000000000ULL,0xFFFE000000000000ULL,0xFFFF000000000000ULL, 
0xFFFF800000000000ULL,0xFFFFC00000000000ULL,0xFFFFE00000000000ULL,0xFFFFF00000000000ULL, 
0xFFFFF80000000000ULL,0xFFFFFC0000000000ULL,0xFFFFFE0000000000ULL,0xFFFFFF0000000000ULL, 
0xFFFFFF8000000000ULL,0xFFFFFFC000000000ULL,0xFFFFFFE000000000ULL,0xFFFFFFF000000000ULL, 
0xFFFFFFF800000000ULL,0xFFFFFFFC00000000ULL,0xFFFFFFFE00000000ULL,0xFFFFFFFF00000000ULL, 
0xFFFFFFFF80000000ULL,0xFFFFFFFFC0000000ULL,0xFFFFFFFFE0000000ULL,0xFFFFFFFFF0000000ULL, 
0xFFFFFFFFF8000000ULL,0xFFFFFFFFFC000000ULL,0xFFFFFFFFFE000000ULL,0xFFFFFFFFFF000000ULL, 
0xFFFFFFFFFF800000ULL,0xFFFFFFFFFFC00000ULL,0xFFFFFFFFFFE00000ULL,0xFFFFFFFFFFF00000ULL, 
0xFFFFFFFFFFF80000ULL,0xFFFFFFFFFFFC0000ULL,0xFFFFFFFFFFFE0000ULL,0xFFFFFFFFFFFF0000ULL, 
0xFFFFFFFFFFFF8000ULL,0xFFFFFFFFFFFFC000ULL,0xFFFFFFFFFFFFE000ULL,0xFFFFFFFFFFFFF000ULL, 
0xFFFFFFFFFFFFF800ULL,0xFFFFFFFFFFFFFC00ULL,0xFFFFFFFFFFFFFE00ULL,0xFFFFFFFFFFFFFF00ULL, 
0xFFFFFFFFFFFFFF80ULL,0xFFFFFFFFFFFFFFC0ULL,0xFFFFFFFFFFFFFFE0ULL,0xFFFFFFFFFFFFFFF0ULL, 
0xFFFFFFFFFFFFFFF8ULL,0xFFFFFFFFFFFFFFFCULL,0xFFFFFFFFFFFFFFFEULL,0xFFFFFFFFFFFFFFFFULL
};

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
  } else if (strstr(name, "/"))
     sscanf(name, "%u.%u.%u.%u/%u", &i1, &i2, &i3, &i4, &uiLength);
  else {
     // normal IP
     sscanf(name, "%u.%u.%u.%u", &i1, &i2, &i3, &i4);
     uiN = 32;
     uiM = 32;
     uiLength = 32;
  }
   

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

char *PrefixRange::get_ip_text(char *buffer) const
{
  int2quad(buffer, ipaddr);
  return buffer;
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
PrefixRange MulticastPrefixRange("224.0.0.0/4^+");

IPv6PrefixRange NullIPv6PrefixRange("::/128^128-128");
IPv6Prefix      NullIPv6Prefix("::/128");
IPv6Addr        NullIPv6Addr("::");
ipv6_addr_t     NullIPv6(0,0);
IPv6PrefixRange MulticastIPv6PrefixRange("ff00::/8^+");

/* IPv6 stuff */

// ipv6_addr_t to string - pointers should be provided
char* ipv62hex(ipv6_addr_t *ip, char *buffer){
   unsigned int i[8];
   int j = 0;

   ip_v6word_t high = ip->high;
   ip_v6word_t low = ip->low;

   i[0] = (high >> 48) & 0xFFFF;
   i[1] = (high >> 32) & 0xFFFF;
   i[2] = (high >> 16) & 0xFFFF;
   i[3] = high & 0xFFFF;
   i[4] = (low >> 48) & 0xFFFF;
   i[5] = (low >> 32) & 0xFFFF;
   i[6] = (low >> 16) & 0xFFFF;
   i[7] = low & 0xFFFF;

   memset(buffer, 0, strlen(buffer));
 

   // do non-compressed

  /* while (j <= 7) {
     sprintf(buffer + strlen(buffer), "%X", i[j]); 
     if (j != 7)
       sprintf(buffer + strlen(buffer), ":");
     j++;
   }
  */

   while ((j <= 7) && (i[j] != 0)) {
     sprintf(buffer + strlen(buffer), "%X", i[j]); 
     if (j != 7)
       sprintf(buffer + strlen(buffer), ":");
     j++;
   }

   if (i[j] == 0) {
     while ((j <= 7) && (i[j] == 0)) {
       if (j == 0)
         sprintf(buffer + strlen(buffer), ":");
       j++;
     }
     sprintf(buffer + strlen(buffer), ":");
   }

   while (j <= 7) {
     sprintf(buffer + strlen(buffer), "%X", i[j]);
     if (j != 7)
      sprintf(buffer + strlen(buffer), ":");
     j++;
   } 
   return(buffer);

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
  char ch = ' ';
  unsigned int uLength, uN, uM;
  uLength = 0;
  uN = 0;
  uM = 0;

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
      sscanf(slash, "%u^%c", &uLength, &ch);
    }
    else if (strstr(slash, "^-")) {
      sscanf(slash, "%u^%c", &uLength, &ch);
    } 
    else if (strstr(slash, "-")) {
      sscanf(slash, "%u^%u-%u", &uLength, &uN, &uM);
    }
    else if (strstr(slash, "^")) {
      sscanf(slash, "%u^%u", &uLength, &uN);
      uM = uN;
    }
    else {
      sscanf(slash, "%u", &uLength);
    }
    switch (ch)
    {
    case '+':
      // inclusive more specifics operator
      uN = uLength;
      uM = 128;
      break;
    case '-':
      // exclusive more specifics operator
      uN = uLength + 1;
      uM = 128;
      break;
    default:
      if (uN == 0 || uN < uLength)
        uN = uLength;
      if (uM == 0 || uM < uN)
        uM = uN;
      break;
    }
  } 
  else {
    // just an IP address
    strncat (address, name, strlen (name));
    hex2ipv6(address, ipaddr);
    ipv62hex(ipaddr, address);
    //printf ("normalized: %s\n", address);
    uLength = 128;
    uN = 128;
    uM = 128;
  }
  t = *ipaddr;
  t = t & (t.getmask(uLength));
  *ipaddr = t;

  n = uN;
  m = uM;
  length = uLength;

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

char *IPv6PrefixRange::get_ip_text(char *buffer) const
{
  ipv62hex(ipaddr, buffer);
  return buffer;
}

char *IPv6PrefixRange::get_text(char *buffer) const
{

  ipv62hex(ipaddr, buffer);
  sprintf(buffer + strlen(buffer), "/%u", length);

  if ((length == n) && (n == m)) {
    // just one route
    ;
  }
  else {
    if ((length == n) && (m == 128)) {
      // inclusive more specifics operator
      strcat(buffer, "^+");
    }
    else {
      if ((length == n - 1) && (m == 128)) {
        // exclusive more specifics operator
        strcat(buffer, "^-");
      }
      else {
        if (n == m)
          sprintf(buffer + strlen(buffer), "^%u", n);
        else
          sprintf(buffer + strlen(buffer), "^%u-%u", n, m);
      }
    }
  }
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
    return (*ipaddr < *(other.ipaddr) ||
             ( (*ipaddr == *(other.ipaddr)) &&
               (length < other.length) )
           );
}
int IPv6PrefixRange::operator<=(const IPv6PrefixRange& other) const
{
    return (*ipaddr < *(other.ipaddr) ||
             ( (*ipaddr == *(other.ipaddr)) &&
               (length <= other.length) )
           );
}

int IPv6PrefixRange::operator==(const IPv6PrefixRange& other) const
{
  return *ipaddr == *(other.ipaddr) &&
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
   ipv6_addr_t *mask = new ipv6_addr_t(0,0,0);
 
   if (length <= 64) {
      mask->high = ipv6masks[length];
      mask->low = ipv6masks[0];
   } else {
     mask->high = ipv6masks[64];
     mask->low = ipv6masks[length-64];
   }

   return *mask;

}

ipv6_addr_t IPv6PrefixRange::get_range() const {
   ipv6_addr_t *range = new ipv6_addr_t(0,0,0);
   int i;
   
   if ( (n <= 64) && (m <= 64) ) {
      range->low |= 0;
      for (i = n; i <= m; ++i) {
        range->high |= ipv6bits[i];
      }
   }
   if ( (n <= 64) && (m > 64) )  {
      for (i = n; i <= 64; ++i) {
        range->high |= ipv6bits[i];
      }
      for (i = 0; i <= (m-64); ++i) {
        range->low |= ipv6bits[i];
      }

   }
   if ( (n > 64) && (m > 64) )  {
      range->high |= 0;
      for (i = (n-64); i <= (m-64); ++i) {
        range->low |= ipv6bits[i];
      }

   }
   if (n == 0)
     range->xbit = true;

   return *range;
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

    if (p.ipv4 != NULL)
      stream << *p.ipv4;
    else 
      stream << *p.ipv6;
    return stream;
  }

MPPrefix::MPPrefix(char *name) {
  if (strstr(name, ":") != NULL) {
     ipv6 = new IPv6PrefixRange(name);
     ipv4 = NULL;
  } else {
    ipv4 = new PrefixRange(name);
    ipv6 = NULL;
  }
}

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

char *MPPrefix::get_text() const {
  if (ipv4)
     return ipv4->get_text();
  return ipv6->get_text();
}

char *MPPrefix::get_ip_text() const {
  if (ipv4)
     return ipv4->get_ip_text();
  return ipv6->get_ip_text();
}

char *MPPrefix::get_afi() const {
  if (ipv4) {
     if (MulticastPrefixRange.contains(*ipv4)) 
       return strdup("ipv4.multicast");
     return strdup("ipv4.unicast");
  }
  if (MulticastIPv6PrefixRange.contains(*ipv6))
       return strdup("ipv6.multicast");
  return strdup("ipv6.unicast");
}

bool MPPrefix::makeMoreSpecific(int code, int n, int m) {
  if (ipv6)
     return ipv6->makeMoreSpecific(code, n, m);
  return ipv4->makeMoreSpecific(code, n, m);
}

int operator<(ipv6_addr_t one, ipv6_addr_t two)
{
  if ( (one.xbit < two.xbit) 
     || (one.high < two.high) 
     || ( (one.xbit == two.xbit) && (one.high == two.high) && (one.low < two.low) )) 
    return 1;
  else 
    return 0;
}

int operator!=(ipv6_addr_t one, ipv6_addr_t two)
{
  if ((one.xbit != two.xbit) ||(one.high != two.high) || (one.low != two.low))
    return 1;
  else 
    return 0;
}

int operator==(ipv6_addr_t one, ipv6_addr_t two)
{  
  if ((one.xbit == two.xbit) && (one.high == two.high) && (one.low == two.low))
    return 1;
  else 
    return 0;
}

ipv6_addr_t& operator&(ipv6_addr_t one, ipv6_addr_t two)
{
  ipv6_addr_t *t = new ipv6_addr_t(0,0,0);
  t->xbit = one.xbit & two.xbit;
  t->high = one.high & two.high;
  t->low = one.low & two.low;

  return *t;
}

ipv6_addr_t& operator|(ipv6_addr_t one, ipv6_addr_t two)
{
  ipv6_addr_t *t = new ipv6_addr_t(0,0,0);

  t->xbit = one.xbit | two.xbit;
  t->high = one.high | two.high;
  t->low = one.low | two.low;
  return *t;

}

ipv6_addr_t& ipv6_addr_t::operator|(u_int64_t i)
{
  ipv6_addr_t *t = new ipv6_addr_t(0,0,0);

  t->xbit = xbit;
  t->high = high;
  t->low  = low | i;

  return *t;
}

ipv6_addr_t& ipv6_addr_t::operator+(u_int64_t i)
{  
  ipv6_addr_t *t = new ipv6_addr_t(0,0,0);

  t->xbit = xbit;
  t->high = high;
  t->low  = low + i;

  return *t;
}

ipv6_addr_t& ipv6_addr_t::operator=(u_int64_t i)
{
  ipv6_addr_t *t = new ipv6_addr_t(0,0,0);

  t->xbit = 0;
  t->high = 0;
  t->low  = i;

  return *t;
}

int ipv6_addr_t::operator==(u_int64_t i)
{
  return (low == i);
}

ipv6_addr_t& ipv6_addr_t::operator<<(unsigned int i)
{
  // i is max 128 in fact, due to type size
  // cyclic shift
  ipv6_addr_t *t = new ipv6_addr_t(*this);
  unsigned int c;

  for (c = 1; c <= i; c++) {
    t->xbit = 0 | (t->high >> 63);
    t->high = (t->high << 1) | (t->low >> 63);
    t->low  = t->low << 1;
  }

  return *t;

}

ipv6_addr_t& ipv6_addr_t::operator>>(unsigned int i)
{
  // i is max 128 in fact, due to type size
  // cyclic shift
  ipv6_addr_t *t = new ipv6_addr_t(*this);
  unsigned int c;

  for (c = 1; c <= i; c++) {
    long long int p = t->xbit;
    p = p << 63;
    t->low  = (t->low >> 1) | (t->high << 63);
    t->high = (t->high >> 1) | p;
    t->xbit = 0;
  }
  
  return *t;

}

bool ipv6_addr_t::operator&&(bool b) {

  if ((xbit || high || low) && b) 
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
  ipv6_addr_t *t = new ipv6_addr_t(0,0,0);  

  t->xbit = ~ xbit;
  t->high = ~ high;
  t->low = ~ low;
  return *t;

}

int ipv6_addr_t::operator!() const
{
  return (!(xbit || high || low));
}

ipv6_addr_t& ipv6_addr_t::getbits(unsigned int len)
{
  ipv6_addr_t *t = new ipv6_addr_t(0,0,0);

  if (len <= 64) {
    t->high = ipv6bits[len];
    t->low = 0;
  } else {
    t->high = 0;
    t->low = ipv6bits[len-64];
  }
  if (len == 0)
    t->xbit = true;

  return *t;

}

ipv6_addr_t& ipv6_addr_t::getmask(unsigned int len)
{ 
   ipv6_addr_t *t = new ipv6_addr_t(0,0,0);

   if (len <= 64) {
      t->high = ipv6masks[len];
      t->low = ipv6masks[0];
   } else {
      t->high = ipv6masks[64];
      t->low = ipv6masks[len-64];
   }

  return *t;

}

ostream& operator<<(ostream& stream, const ipv6_addr_t& p) {
   char buf[256];
   ipv62hex((ipv6_addr_t *) &p, buf); 
 //  stream << "xbit " << xbit << " ";
   stream << buf;
   return stream;
}

// non-duplicated OR
void MPPrefixRanges::append_list(const MPPrefixRanges *src) {
   MPPrefixRanges::const_iterator p;
   for (p = src->begin(); p != src->end(); ++p) {
     if (! this->contains (*p))
       this->push_back(*p);
   }
}

// AND
void MPPrefixRanges::_and(MPPrefixRanges *src) {
   MPPrefixRanges result = new MPPrefixRanges;
   insert_iterator<MPPrefixRanges> res_ins(result, result.begin());
   set_intersection(this->begin(), this->end(), src->begin(), src->end(), res_ins);
   assign(result.begin(), result.end());

}

// EXCEPT
void MPPrefixRanges::except(MPPrefixRanges *src) {
   MPPrefixRanges result = new MPPrefixRanges;
   insert_iterator<MPPrefixRanges> res_ins(result, result.begin());
   set_difference(this->begin(), this->end(), src->begin(), src->end(), res_ins);
   assign(result.begin(), result.end());
}

bool MPPrefixRanges::contains(IPAddr ip) const {
   MPPrefixRanges::const_iterator p;
   for (p = begin(); p != end(); ++p) {
     if (p->ipv4 && (p->ipv4->get_ipaddr() == ip.get_ipaddr())) 
       return true;
   }
   return false;
}

bool MPPrefixRanges::contains(IPv6Addr ip) const {
   MPPrefixRanges::const_iterator p;
   for (p = begin(); p != end(); ++p) {
     if (p->ipv6 && (*(p->ipv6->get_ipaddr()) == *(ip.get_ipaddr())) )
       return true;
   }
   return false;
}

bool MPPrefixRanges::contains(MPPrefix ip) const {
   MPPrefixRanges::const_iterator p;
   for (p = begin(); p != end(); ++p) {
     if (p->ipv6 && ip.ipv6 && (*(p->ipv6->get_ipaddr()) == ip.get_ipaddr()) )
       return true;
     if (p->ipv4 && ip.ipv4 && (p->ipv4->get_ipaddr() == ip.ipv4->get_ipaddr()))
       return true;
   }
   return false;
}

ostream& operator<<(ostream& stream, const MPPrefixRanges& list) {
   MPPrefixRanges::const_iterator p;
   for (p = list.begin(); p != list.end(); ++p) {
      stream << *p << " ";
   }
   stream << endl;
   return stream;
}


