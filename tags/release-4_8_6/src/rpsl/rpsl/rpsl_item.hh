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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

// Comments:
// any dynamic memory allocated by lexer or parser 
// is claimed by the constructors here
// They are either used internally or deleted using delete!
// if a pointer points to something in the dictionary, that is not claimed

#ifndef ITEM_HH
#define ITEM_HH

#include "config.h"
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <ctime>
#include <iomanip>
#include <string>
extern "C" {
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif // HAVE_MALLOC_H
}
#include "util/List.hh"
#include "util/Buffer.hh"
#include "symbols.hh"
#include "prefix.hh"
#include "afi.hh"

// Added by wlee@isi.edu
#ifdef DEBUG
#define INDENT(indent) for (int iii = 0; iii < indent; iii++) os << " "
#endif // DEBUG

typedef unsigned int ASt;

class Filter;
class Buffer;

class Item : public ListNode {
public:
   virtual ~Item() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const = 0;
   virtual bool operator <=(Item &b) {
      return false;
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const = 0;
#ifdef DEBUG
   virtual const char *className(void) const = 0;
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "(*** Need more work here ***)" << std::endl;
   }
#endif // DEBUG
};

inline std::ostream &operator<<(std::ostream &out, const Item &item) {
   return item.print(out);
}

class ItemASNO : public Item {
public:
   ASt asno;
public:
   ItemASNO(ASt asn) {
      asno = asn;
   }
   virtual ~ItemASNO() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item *dup() const {
      return new ItemASNO(*this);
   }
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemASNO&) 
	 && asno <= ((ItemASNO&) b).asno;
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemASNO";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "asno = " << asno << std::endl;
   }
#endif // DEBUG
};

class ItemMSItem : public Item {
public:
   int code;			// 0 for ^-, 1 for ^+, 2 for ^n and ^n-m
   int n;
   int m;
   Item *item;
public:
   ItemMSItem(Item *i, int _code) : item(i), code(_code) {
   }
   ItemMSItem(Item *i, int _code, int _n, int _m) : item(i), code(_code),
      n(_n), m(_m) {
   }
   ItemMSItem(const ItemMSItem &b) {
      item = b.item->dup();
      code = b.code;
      n    = b.n;
      m    = b.m;
   }
   virtual ~ItemMSItem() {
      if (item)
	 delete item;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item *dup() const {
      return new ItemMSItem(*this);
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemMSItem";
   }
#endif // DEBUG
};

class ItemFilter : public Item {
public:
   Filter *filter;
public:
   ItemFilter(Filter *_filter) {
      filter = _filter;
   }
   virtual ~ItemFilter();
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item *dup() const {
      return new ItemFilter(*this);
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemFilter";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "filter = " << filter << std::endl;
   }
#endif // DEBUG
};

class ItemINT : public Item {
public:
   long long int i;
public:
   ItemINT(const long long int _i) : i(_i) {}
   virtual ~ItemINT() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemINT(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemINT&) 
	 && i <= ((ItemINT&) b).i;
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemINT";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "i = " << i << std::endl;
   }
#endif // DEBUG
};

class ItemTimeStamp : public Item {
public:
   time_t stamp;
public:
   ItemTimeStamp(time_t ts) : stamp(ts) {
   }
   ItemTimeStamp(const ItemTimeStamp &pt) : stamp(pt.stamp) {
   }
   virtual ~ItemTimeStamp() {
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemTimeStamp(*this);
   } 
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemTimeStamp";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "stamp = " << stamp << std::endl;
   }
#endif // DEBUG
};

class ItemREAL : public Item {
public:
   double real;
public:
   ItemREAL(const double r) : real(r) {}
   virtual ~ItemREAL() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemREAL(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemREAL&) 
	 && real <= ((ItemREAL&) b).real;
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemREAL";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "real = " << real << std::endl;
   }
#endif // DEBUG
};

class ItemSTRING : public Item {
public:
   char *string;
public:
   ItemSTRING(char *txt) {
      string = txt;
   }
   ItemSTRING(const ItemSTRING &pt) {
      string = strdup(pt.string);
   }
   virtual ~ItemSTRING() {
      free(string);
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemSTRING(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemSTRING&) 
	 && strcasecmp(string, ((ItemSTRING&) b).string) < 0;
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemSTRING";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "string = \"" << string << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemBLOB : public Item {
public:
   char *blob;
public:
   ItemBLOB(char *txt) : blob(txt) {
   }
   ItemBLOB(const ItemBLOB &pt) {
      blob = strdup(pt.blob);
   }
   virtual ~ItemBLOB() {
      free(blob);
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemBLOB(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemBLOB&) 
	 && strcasecmp(blob, ((ItemBLOB&) b).blob) < 0;
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemBLOB";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "blob = \"" << blob << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemBUFFER : public Item {
public:
   Buffer *buffer;
public:
   ItemBUFFER(Buffer *buf) : buffer(buf) {
   }
   ItemBUFFER(const ItemBUFFER &pt) {
      buffer = new Buffer(*pt.buffer);
   }
   virtual ~ItemBUFFER() {
      delete buffer;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemBUFFER(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemBUFFER&) 
	 && strncasecmp(buffer->contents, ((ItemBUFFER&) b).buffer->contents,
			buffer->size) < 0;
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemBUFFER";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "buffer = \"" << buffer << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemIPV4 : public Item {
public:
   IPAddr *ipv4;
public:
   ItemIPV4(IPAddr *ip) {
      ipv4 = ip;
   }
   ItemIPV4(const ItemIPV4 &pt) {
      ipv4 = new IPAddr(*pt.ipv4);
   }
   virtual ~ItemIPV4() {
      delete ipv4;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemIPV4(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemIPV4&) 
	 && ipv4->get_ipaddr() <= ((ItemIPV4&) b).ipv4->get_ipaddr();
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemIPV4";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "ipv4 = \"" << *ipv4 << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemPRFXV4 : public Item {
public:
   Prefix *prfxv4;
public:
   ItemPRFXV4(Prefix *prfx) {
      prfxv4 = prfx;
   }
   virtual ~ItemPRFXV4() {
      delete prfxv4;
   }
   // Added by wlee@isi.edu
   ItemPRFXV4(const ItemPRFXV4 &pt) {
      prfxv4 = new Prefix(*pt.prfxv4);
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemPRFXV4(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemPRFXV4&) 
	 && prfxv4->get_ipaddr() <= ((ItemPRFXV4&) b).prfxv4->get_ipaddr()
	 && prfxv4->get_length() == ((ItemPRFXV4&) b).prfxv4->get_length();
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemPRFXV4";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "prfxv4 = \"" << *prfxv4 << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemPRFXV4Range : public Item {
public:
   PrefixRange *prfxv4;
public:
   ItemPRFXV4Range(PrefixRange *prfx) {
      prfxv4 = prfx;
   }
   virtual ~ItemPRFXV4Range() {
      delete prfxv4;
   }
   // Added by wlee@isi.edu
   ItemPRFXV4Range(const ItemPRFXV4Range &pt) {
      prfxv4 = new PrefixRange(*pt.prfxv4);
   }
    virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemPRFXV4Range(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemPRFXV4Range&) 
	 && prfxv4->get_ipaddr() <= ((ItemPRFXV4Range&) b).prfxv4->get_ipaddr()
	 && prfxv4->get_n()      == ((ItemPRFXV4Range&) b).prfxv4->get_n()
	 && prfxv4->get_m()      == ((ItemPRFXV4Range&) b).prfxv4->get_m()
	 && prfxv4->get_length() == ((ItemPRFXV4Range&)b).prfxv4->get_length();
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemPRFXV4Range";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "prfxv4 = \"" << *prfxv4 << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemIPV6 : public Item {
public:
   IPv6Addr *ipv6;
public:
   ItemIPV6(IPv6Addr *ip) {
      ipv6 = ip;
   }
   ItemIPV6(const ItemIPV6 &pt) {
      ipv6 = new IPv6Addr(*pt.ipv6);
   }
   virtual ~ItemIPV6() {
      delete ipv6;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemIPV6(*this);
   } 
   virtual bool operator <=(Item &b) {
        return typeid(b) == typeid(ItemIPV6&) &&
        ipv6 <= ((ItemIPV6&) b).ipv6;
   }

   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemIPV6";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "ipv6 = \"" << *ipv6 << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemPRFXV6 : public Item {
public:
   IPv6Prefix *prfxv6;
public:
   ItemPRFXV6(IPv6Prefix *prfx) {
      prfxv6 = prfx;
   }
   virtual ~ItemPRFXV6() {
      delete prfxv6;
   }
   // Added by wlee@isi.edu
   ItemPRFXV6(const ItemPRFXV6 &pt) {
      prfxv6 = new IPv6Prefix(*pt.prfxv6);
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemPRFXV6(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemPRFXV6&) 
         && prfxv6 <= ((ItemPRFXV6&) b).prfxv6;
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemPRFXV6";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "prfxv6 = \"" << *prfxv6 << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemPRFXV6Range : public Item {
public:
   IPv6PrefixRange *prfxv6;
public:
   ItemPRFXV6Range(IPv6PrefixRange *prfx) {
      prfxv6 = prfx;
   }
   virtual ~ItemPRFXV6Range() {
      delete prfxv6;
   }
   // Added by wlee@isi.edu
   ItemPRFXV6Range(const ItemPRFXV6Range &pt) {
      prfxv6 = new IPv6PrefixRange(*pt.prfxv6);
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemPRFXV6Range(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemPRFXV6Range&) 
      && prfxv6->get_ipaddr() <= ((ItemPRFXV6Range&) b).prfxv6->get_ipaddr()
      && prfxv6->get_n()      == ((ItemPRFXV6Range&) b).prfxv6->get_n()
      && prfxv6->get_m()      == ((ItemPRFXV6Range&) b).prfxv6->get_m()
      && prfxv6->get_length() == ((ItemPRFXV6Range&) b).prfxv6->get_length();

   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemPRFXV6Range";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "prfxrangev6 = \"" << *prfxv6 << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemAFI : public Item , public AddressFamily {

// char *afi inherited from 2nd parent

public:
   ItemAFI() {
   }
   ItemAFI(AddressFamily *af) {
      afi = af->afi;
   }
   ItemAFI(char *af_text) {
      afi = af_text;
   }
   ItemAFI(const ItemAFI &pt) {
      afi = strdup(pt.afi);
   }
   virtual ~ItemAFI() {
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemAFI(*this);
   } 
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemAFI";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "afi = \"" << *afi << "\"" << std::endl;
   }
#endif // DEBUG
   int operator==(ItemAFI &af) {
      return ((AddressFamily &) af == (AddressFamily &) *this);
   }
   int operator!=(ItemAFI &af) {
      return ! ((AddressFamily &) af == (AddressFamily &) *this);
   }
};

class ItemConnection : public Item {
public:
   IPAddr *ip;
   SymID   host;
   int     port;
public:
   ItemConnection(IPAddr *_ip, int _port = 0) {
      host = NULL;
      ip   = _ip;
      port = _port;
   }
   ItemConnection(SymID _host, int _port = 0) {
      host = _host;
      ip   = NULL;
      port = _port;
   }
   ItemConnection(const ItemConnection &pt) {
      ip = new IPAddr(*pt.ip);
      host = pt.host;
      port = pt.port;
   }
   virtual ~ItemConnection() {
      if (ip)
	 delete ip;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemConnection(*this);
   } 
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemConnection";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "ip   = \"" << *ip  << "\"" << std::endl;
      INDENT(indent); os << "host = \"" << host << "\"" << std::endl;
      INDENT(indent); os << "port = \"" << port << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemSID : public Item {
public:
   SymID name;
public:
   ItemSID(SymID sid) {
      name = sid;
   }
   virtual ~ItemSID() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemSID(*this);
   } 
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemSID";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "name (SymID) = \"" << name << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemASNAME : public ItemSID {
public:
   ItemASNAME(SymID sid) : ItemSID(sid) {}
};

class ItemRSNAME : public ItemSID {
public:
   ItemRSNAME(SymID sid) : ItemSID(sid) {}
};

class ItemRTRSNAME : public ItemSID {
public:
   ItemRTRSNAME(SymID sid) : ItemSID(sid) {}
};

class ItemPRNGNAME : public ItemSID {
public:
   ItemPRNGNAME(SymID sid) : ItemSID(sid) {}
};

class ItemFLTRNAME : public ItemSID {
public:
   ItemFLTRNAME(SymID sid) : ItemSID(sid) {}
};

class ItemDNS : public ItemSID {
public:
   ItemDNS(SymID sid) : ItemSID(sid) {}
};

class ItemBOOLEAN : public Item {
public:
   int i;
public:
   ItemBOOLEAN(int _i) : i(_i) {}
   virtual ~ItemBOOLEAN() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemBOOLEAN(*this);
   } 
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemBOOLEAN";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "i = " << i << std::endl;
   }
#endif // DEBUG
};

class ItemWORD : public Item {
public:
   char *word;
public:
   ItemWORD(char *txt) {
      word = txt;
   }
   ItemWORD(const ItemWORD &pt) {
      word = strdup(pt.word);
   }
   virtual ~ItemWORD() {
      free(word);
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemWORD(*this);
   } 
   virtual bool operator <=(Item &b) {
      return typeid(b) == typeid(ItemWORD&) 
	 && strcasecmp(word, ((ItemWORD&) b).word) < 0;
   }
   virtual bool operator ==(Item &b) {
      return typeid(b) == typeid(ItemWORD&)
   && strcasecmp(word, ((ItemWORD&) b).word) == 0;
   }
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemWORD";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "word = \"" << word << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemEMAIL : public Item {
public:
   char *email;
public:
   ItemEMAIL(char *txt) {
      email = txt;
   }
   ItemEMAIL(const ItemEMAIL &pt) {
      email = strdup(pt.email);
   }
   virtual ~ItemEMAIL() {
      free(email);
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemEMAIL(*this);
   } 
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemEMAIL";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "email = \"" << email << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemFREETEXT : public Item {
public:
   char *text;
   int   length;
public:
   ItemFREETEXT(char *txt, int len) : text(txt), length(len) {
   }
   virtual ~ItemFREETEXT() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemFREETEXT(*this);
   } 
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemFREETEXT";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "\"";
#define DISPLAYFREETEXTLENGTH  63
      if (length > DISPLAYFREETEXTLENGTH) {
	char buf[DISPLAYFREETEXTLENGTH + 1];
	strncpy(buf, text, DISPLAYFREETEXTLENGTH - 3);
	buf[DISPLAYFREETEXTLENGTH - 3] = 0;
	strcat(buf, "...");
	os << buf << std::endl;
      } else {
	for (int i = 0; i < length; i++) {
	  char ch = *(text + i);
	  if (ch == '\n')
	    os << "\\n";
	  else
	    os << ch;
        }
      }
      os << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemList: public Item, public List<Item> {
public:
   ItemList() {
   }
   ItemList(const ItemList &pt) : List<Item>() {
      for (Item *item = pt.head(); item; item = pt.next(item))
	 append(item->dup());
   }
   virtual ~ItemList() {
      List<Item>::clear();
   }
   virtual std::ostream& print(std::ostream &out) const {
      return print(out, ", ");
   }
   std::ostream& print(std::ostream &out, char *delim) const;
   virtual Item *dup() const {
      return new ItemList(*this);
   }
   ItemList &operator+=(const ItemList &pt) {
      for (Item *item = pt.head(); item; item = pt.next(item))
         append(item->dup());
      return *this;
   }
   // for afi
   bool contains(Item *t) {
     for (Item *item = head(); item; item = next(item))
       if ((typeid(*item) == typeid(*t)) && ( typeid(*t) == typeid(ItemAFI)))
         if (((ItemAFI &) *item) == ((ItemAFI &) *t))
           return true;
     return false;
   }
   // merge for afi
   void merge(ItemList &list) {
     for (Item *item = list.head(); item; item = list.next(item))
       if (! contains((ItemAFI *) item))
          append((ItemAFI *) item->dup());
   }

   void _and(ItemList &list) {
     ItemList *result = new ItemList;
     for (Item *item = head(); item; item = next(item))
       if (list.contains((ItemAFI *) item))
         result->append((ItemAFI *) item->dup());
     clear();
     merge(*result);
     delete result;
   }

   void _or(ItemList &list) {
     merge(list);
   }
  
   void _not() {
     // create complete afi list
     ItemList *full = new ItemList;
     full->append(new ItemAFI("any"));
     full = full->expand();
     ItemList *result = new ItemList;
     for (Item *item = full->head(); item; item = full->next(item))
       if (! contains ((ItemAFI *) item))
         result->append((ItemAFI *) item->dup());
     clear();
     merge(*result);
     delete full;
     delete result;
   }

   virtual ItemList *expand();

   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemList";
   }
   virtual void printClass(std::ostream &os, int indent) const {
     for (Item *item = head(); item; item = next(item)) {
        INDENT(indent); os << item->className() << std::endl;
        item->printClass(os, indent + 2);
     }
   }
#endif // DEBUG
};

class ItemRange : public Item {
public:
   Item *begin;
   Item *end;
public:
   ItemRange(Item *b, Item *e) : begin(b), end(e) {
   }
   ItemRange(const ItemRange &pt) {
      begin = pt.begin->dup();
      end   = pt.end->dup();
   }
   virtual ~ItemRange() {
      delete begin;
      delete end;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Item* dup() const {
      return new ItemRange(*this);
   } 
   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemRange";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "begin = \"" << "\"" << std::endl;
      INDENT(indent); os << "end = \"" << "\"" << std::endl;
   }
#endif // DEBUG
};

class ItemSequence: public Item, public List<Item> {
public:
   ItemSequence() {
   }
   ItemSequence(const ItemSequence &pt) : List<Item>() {
      for (Item *item = pt.head(); item; item = pt.next(item))
	 append(item->dup());
   }
   virtual ~ItemSequence() {
      List<Item>::clear();
   }
   std::ostream& print(std::ostream &out) const;
   virtual Item *dup() const {
      return new ItemSequence(*this);
   }
   ItemSequence &operator+=(const ItemSequence &pt) {
      for (Item *item = pt.head(); item; item = pt.next(item))
	 append(item->dup());
      return *this;
   }

   virtual Buffer *bufferize(Buffer *buf = NULL, bool lcase = false) const;
#ifdef DEBUG
   virtual const char *className(void) const {
      return "ItemSequence";
   }
   virtual void printClass(std::ostream &os, int indent) const {
     for (Item *item = head(); item; item = next(item)) {
        INDENT(indent); os << item->className() << std::endl;
        item->printClass(os, indent + 2);
     }
   }
#endif // DEBUG
};



#endif   // ITEM_HH
