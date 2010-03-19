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
// Author(s)         Katie Petrusha <katie@ripe.net>

#include "config.h"
#include <sys/types.h>
#include <rpsl/prefix.hh>
#include <sstream>

class PrefixRange;
class IPv6PrefixRange;
class MPPrefix;


class AddressFamily {
 public:
   static bool noDefaultAfi;
   const char *afi;

 public:
   AddressFamily();
   AddressFamily(const AddressFamily &p);
   AddressFamily(const char *name); 
   const char *name() {
      return afi;
   }
   const char *name_afi();
   const char *name_safi();

   AddressFamily *dup() const {
     return new AddressFamily(*this);
   }

   bool is_Matching (const char *name); // exact match
   bool is_ipv4 ();  
   bool is_ipv6 ();  
   bool is_default ();
   virtual bool is_valid(MPPrefix *p);
   virtual bool is_valid(PrefixRange *p);
   virtual bool is_valid(IPv6PrefixRange *p);

   bool operator==(AddressFamily &p) {
     if (strcasecmp(afi, p.afi) == 0) {
       return true;
     }
     return false;
   }
   
  friend std::ostream& operator<<(std::ostream& stream, const AddressFamily &p);

  friend class ItemAFI;
  friend class FilterAFI;

};
