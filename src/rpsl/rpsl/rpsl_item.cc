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
#include <iostream>
#include <iomanip>
#include "rpsl_item.hh"
#include "rpsl_filter.hh"
#include "Buffer.hh"

using namespace std;

//// printing ////////////////////////////////////////////////////////

ostream &Item::print(ostream &out) const {
   return out;
}

ostream &ItemASNO::print(ostream &out) const {
   char buffer[64];
   asnum_string(buffer, asno);
   out << buffer;
   return out;
}

Buffer *ItemASNO::bufferize(Buffer *buf, bool lcase) const {
   char buffer[64];
   asnum_string(buffer, asno);
   if (!buf)
      buf = new Buffer;
   buf->appendf(buffer);
   return buf;
}

ostream &ItemMSItem::print(ostream &out) const {
   out << *item;
   switch (code) {
   case 0:
      out << "^-";
      break;
   case 1:
      out << "^+";
      break;
   case 2:
      if (n == m)
	 out << "^" << n;
      else
	 out << "^" << n << "-" << m;
   }
   return out;
}

Buffer *ItemMSItem::bufferize(Buffer *buf, bool lcase) const {
   buf = item->bufferize(buf, lcase);
   switch (code) {
   case 0:
      buf->append("^-");
      break;
   case 1:
      buf->append("^+");
      break;
   case 2:
      if (n == m)
	 buf->appendf("^%d", n);
      else
	 buf->appendf("^%d-%d", n, m);
   }
   return buf;
}

ostream &ItemFilter::print(ostream &out) const {
   out << filter;
   return out;
}

Buffer *ItemFilter::bufferize(Buffer *buf, bool lcase) const {
   cerr << "Error: I dont know how to canonicalize filters" << endl;
   if (!buf)
      buf = new Buffer;
   return buf;
}

ostream &ItemINT::print(ostream &out) const {
   out << i;
   return out;
}

Buffer *ItemINT::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->appendf("%d", i);
   return buf;
}

ostream &ItemTimeStamp::print(ostream &out) const {
   tm *ts;
   ts = gmtime(&stamp);

   // YYYYMMDD HH:MM:SS +HH:MM
   char prevfill = out.fill('0');
   streamsize prevwidth = out.width(4);

   out << ts->tm_year + 1900;
   out.width(2);
   out << ts->tm_mon+1;
   out << ts->tm_mday << " ";
   out << ts->tm_hour << ":";
   out << ts->tm_min << ":";
   out << ts->tm_sec << " +00:00";

   out.fill(prevfill);
   out.width(prevwidth);
   
   return out;
}

Buffer *ItemTimeStamp::bufferize(Buffer *buf, bool lcase) const {
   tm *ts;
   ts = gmtime(&stamp);
   if (!buf)
      buf = new Buffer;
   buf->appendf("%04d%02d%02d %02d:%02d:%02d +00:00", 
		ts->tm_year + 1900, ts->tm_mon+1, ts->tm_mday, 
		ts->tm_hour, ts->tm_min, ts->tm_sec);
   return buf;
}

ostream &ItemREAL::print(ostream &out) const {
   out << real;
   return out;
}

Buffer *ItemREAL::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->appendf("%f", real);
   return buf;
}

ostream &ItemSTRING::print(ostream &out) const {
   out << string;
   return out;
}

Buffer *ItemSTRING::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(string);
   if (lcase)
      buf->toLowerCase();
   return buf;
}

ostream &ItemBLOB::print(ostream &out) const {
   out << blob;
   return out;
}

Buffer *ItemBLOB::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(blob);
   if (lcase)
      buf->toLowerCase();
   return buf;
}

// Print contents of buffer.
// The contents are the first buffer->size characters of buffer->contents.
ostream &ItemBUFFER::print(ostream &out) const {
    string s(buffer->contents, 0, buffer->size);
    out << s;
    return out;
}


Buffer *ItemBUFFER::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer(*buffer);
   else
      buf->append(*buffer);
   if (lcase)
      buf->toLowerCase();
   return buf;
}

ostream &ItemIPV4::print(ostream &out) const {
   out << *ipv4;
   return out;
}

Buffer *ItemIPV4::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(ipv4->get_text());
   return buf;
}

ostream &ItemPRFXV4::print(ostream &out) const {
   out << *prfxv4;
   return out;
}

Buffer *ItemPRFXV4::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(prfxv4->get_text());
   return buf;
}

ostream &ItemPRFXV4Range::print(ostream &out) const {
   out << *prfxv4;
   return out;
}

Buffer *ItemPRFXV4Range::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(prfxv4->get_text());
   return buf;
}

ostream &ItemIPV6::print(ostream &out) const {
   out << *ipv6;
   return out;
}

Buffer *ItemIPV6::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(ipv6->get_text());
   return buf;
}

ostream &ItemPRFXV6::print(ostream &out) const {
   out << *prfxv6;
   return out;
}

Buffer *ItemPRFXV6::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(prfxv6->get_text());
   return buf;
}

ostream &ItemPRFXV6Range::print(ostream &out) const {
   out << *prfxv6;
   return out;
}

Buffer *ItemPRFXV6Range::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(prfxv6->get_text());
   return buf;
}

Buffer *ItemAFI::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append( ((AddressFamily *) this)->name());
   return buf;
}

ostream &ItemAFI::print(ostream &out) const {
   out << (AddressFamily &) *this;
   return out;
}

ostream &ItemConnection::print(ostream &out) const {
   if (ip)
      out << *ip;
   else
      out << host;
   if (port)
      out << ":" << port;
   return out;
}

Buffer *ItemConnection::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   if (ip)
      buf->append(ip->get_text());
   else
      buf->append(host);
   if (port)
      buf->appendf(":%d", port);
   if (lcase)
      buf->toLowerCase();
   return buf;
}

ostream &ItemSID::print(ostream &out) const {
   out << name;
   return out;
}

Buffer *ItemSID::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(name);
   if (lcase)
      buf->toLowerCase();
   return buf;
}

ostream &ItemBOOLEAN::print(ostream &out) const {
   out << (i ? "true" : "false");
   return out;
}

Buffer *ItemBOOLEAN::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append((i ? "true" : "false"));
   return buf;
}

ostream &ItemWORD::print(ostream &out) const {
   out << word;
   return out;
}

Buffer *ItemWORD::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(word);
   if (lcase)
      buf->toLowerCase();
   return buf;
}

ostream &ItemEMAIL::print(ostream &out) const {
   out << email;
   return out;
}

Buffer *ItemEMAIL::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(email);
   if (lcase)
      buf->toLowerCase();
   return buf;
}

ostream &ItemRange::print(ostream &out) const {
   out << *begin << " - " << *end;
   return out;
}

Buffer *ItemRange::bufferize(Buffer *buf, bool lcase) const {
   buf = begin->bufferize(buf, false);
   buf->append("-");
   end->bufferize(buf, false);
   if (lcase)
      buf->toLowerCase();
   return buf;
}

ostream &ItemFREETEXT::print(ostream &out) const {
   for (int i = 0; i < length; ++i)
      out << *(text + i);
   return out;
}

Buffer *ItemFREETEXT::bufferize(Buffer *buf, bool lcase) const {
   if (!buf)
      buf = new Buffer;
   buf->append(text, length);
   if (lcase)
      buf->toLowerCase();
   return buf;
}

ostream &ItemList::print(ostream &out, const char *delim) const {
   Item *item = head();
   if (item) {
      item->print(out);
      for (item = next(item); item; item = next(item)) {
	 out << delim;
	 item->print(out);
      }
   }

   return out;
}

Buffer *ItemList::bufferize(Buffer *buf, bool lcase) const {
   cerr << "Error: I dont know how to canonicalize lists" << endl;
   if (!buf)
      buf = new Buffer;
   return buf;
}

ItemList *ItemList::expand() {
  ItemList *list = new ItemList;
  ItemList *list2 = new ItemList;

  // do the mapping
  for (Item *item = this->head();
       item;
       item = this->next(item)) 
  {
    if (strcasecmp (((ItemAFI *) item)->afi, "any") == 0) 
    {
      list->append (new ItemAFI(strdup("ipv4.unicast")));
      list->append (new ItemAFI(strdup("ipv4.multicast")));
      list->append (new ItemAFI(strdup("ipv6.unicast")));
      list->append (new ItemAFI(strdup("ipv6.multicast")));
    }
    else if (strcasecmp (((ItemAFI *) item)->afi, "any.unicast") == 0) 
    {
      list->append (new ItemAFI(strdup("ipv4.unicast")));
      list->append (new ItemAFI(strdup("ipv6.unicast")));
    }
    else if (strcasecmp (((ItemAFI *) item)->afi, "any.multicast") == 0)
    {
      list->append (new ItemAFI(strdup("ipv4.multicast")));
      list->append (new ItemAFI(strdup("ipv6.multicast")));
    }
    else if (strcasecmp (((ItemAFI *) item)->afi, "ipv4") == 0)
    {
      list->append (new ItemAFI(strdup("ipv4.unicast")));
      list->append (new ItemAFI(strdup("ipv4.multicast")));
    }
    else if (strcasecmp (((ItemAFI *) item)->afi, "ipv6") == 0)
    {
      list->append (new ItemAFI(strdup("ipv6.unicast")));
      list->append (new ItemAFI(strdup("ipv6.multicast")));
    }
    else {
      list->append (item->dup());
    }
  }
  // remove duplicates
  for (Item *item = list->head();
       item;
       item = list->next(item)) 
  {
    if (! list2->contains(item))
      list2->append(item->dup());
  }
  
  delete list;
  return list2;
}

ostream &ItemSequence::print(ostream &out) const {
   Item *item = head();
   if (item) {
      item->print(out);
      for (item = next(item); item; item = next(item)) {
	 out << " ";
	 item->print(out);
      }
   }

   return out;
}

Buffer *ItemSequence::bufferize(Buffer *buf, bool lcase) const {
   cerr << "Error: I dont know how to canonicalize sequences" << endl;
   if (!buf)
      buf = new Buffer;
   return buf;
}

ItemFilter::~ItemFilter() {
   if (filter)
      delete filter;
}
