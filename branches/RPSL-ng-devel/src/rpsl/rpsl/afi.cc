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
//  Author(s): Katie Petrusha <katie@ripe.net>

#include "config.h"
#include <cstdio>
#include <cstring>
#include <iostream.h>
#include <stdlib.h>
#include "afi.hh"

AddressFamily::AddressFamily(void) :
  afi(NULL)
{
}

AddressFamily::AddressFamily(char *name) : 
  afi(strdup(name))
{
}

AddressFamily::AddressFamily(const AddressFamily &p) :
  afi(strdup(p.afi))
{
}

bool AddressFamily::is_Matching(char *name)  {
  if (strncasecmp (name, afi, sizeof(afi)) == 0 
   && strncasecmp (name, afi, sizeof(name)) == 0)
     return 1;
  return 0; 
}

bool AddressFamily::is_ipv4() {
  if (strstr(afi, "ipv4"))
     return true;
  return false;
}

bool AddressFamily::is_ipv6() {
  if (strstr(afi, "ipv6"))
     return true;
  return false;
}

ostream& operator<<(ostream& stream, const AddressFamily& p) {
   stream << p.afi;
   return stream;
}

bool AddressFamily::is_valid(PrefixRange *p) {
   if (strstr(afi, "ipv4"))
      return true;
   return false;
}

bool AddressFamily::is_valid(IPv6PrefixRange *p) {
   if (strstr(afi, "ipv6"))
      return true;
   return false;
}
