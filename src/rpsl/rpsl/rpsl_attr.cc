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

#include "config.h"
#include <cstdio>
#include "rpsl_attr.hh"
#include "object.hh"
#include "schema.hh"
#include <string>

using namespace std;

//// printing ////////////////////////////////////////////////////////

ostream &Attr::print(ostream &out) const {
   if (object && object->contents)
      out.write(object->getTextAt(offset), len);
   return out;
}

ostream &AttrGeneric::print(ostream &out) const {
   out << type->name() << ":\t" << *items;
   return out;
}

ostream &AttrImport::print(ostream &out) const {
   out << "import: ";
   if (fromProt && fromProt->name && strcasecmp(fromProt->name, "BGP4"))
      out << "protocol " << fromProt->name << " ";
   if (intoProt && intoProt->name && strcasecmp(intoProt->name, "BGP4"))
      out << "into " << intoProt->name << " ";
   out << *policy;
   return out;
}

ostream &AttrExport::print(ostream &out) const {
   out << "export: ";
   if (fromProt && fromProt->name && strcasecmp(fromProt->name, "BGP4"))
      out << "protocol " << fromProt->name << " ";
   if (intoProt && intoProt->name && strcasecmp(intoProt->name, "BGP4"))
      out << "into " << intoProt->name << " ";
   out << *policy;
   return out;
}

ostream &AttrDefault::print(ostream &out) const {
   out << "default: \t" << *afi_list << " to   " << *peering << "\n";

   if (!action->isEmpty())
      out << "       \t " << *action << "\n";

   if (filter && typeid(filter) != typeid(FilterANY))
      out << "       \t networks " << *filter;

   return out;
}

ostream &AttrFilter::print(ostream &out) const {
   out << "filter: " << *filter;

   return out;
}
ostream &AttrMPPeval::print(ostream &out) const {
   out << "filter: " << *filter;

   return out;
}


ostream &AttrPeering::print(ostream &out) const {
   out << "peering: " << *peering;

   return out;
}

ostream &AttrIfAddr::print(ostream &out) const {
/*   static char buffer[128];
   out << "ifaddr:\t" << int2quad(buffer, ifaddr.get_ipaddr())
       << " masklen " << ifaddr.get_length();
  return out;
*/
   out << "ifaddr/interface:\t" ;
   out << ifaddr->get_ip_text();
   out << " masklen " << ifaddr->get_length();
   if (action)
     out << " action " << *action;
   if (tunnel)
     out << " tunnel " << *tunnel;
   return out;
}

ostream &AttrPeerOption::print(ostream &out) const {
   out << option << "(" << *args << ")";
   return out;
}

/*
ostream &AttrPeer::print(ostream &out) const {
   out << "peer:\t" << protocol->name
       << " " << *peer << " ";
   for (AttrPeerOption *nd = options->head(); nd; ) {
      nd->print(out);
      nd = options->next(nd);
      if (nd)
	 out << ", ";
   }
   return out;
}
*/

ostream &AttrPeer::print(ostream &out) const {
   out << "peer/mp-peer:\t" << protocol->name
       << " " << peer->get_ip_text() << " ";
   for (AttrPeerOption *nd = options->head(); nd; ) {
      nd->print(out);
      nd = options->next(nd);
      if (nd)
   out << ", ";
   }
   return out;
}

ostream &AttrTypedef::print(ostream &out) const {
   if (type)
      out << "typedef: " << name << " " << *type;   
   else 
      out << "typedef: " << name << "<error in type spec>";   
   return out;
}

ostream &AttrProtocol::print(ostream &out) const {
   out << "protocol: " << name;
   for (AttrProtocolOption *nd = options->head(); nd; nd = options->next(nd)) {
      nd->print(out);
      out << "\n             ";
   }
   return out;
}

ostream &AttrProtocolOption::print(ostream &out) const {
   out << (optional ? " optional " : " mandatory ") << *option << " ";
   return out;
}

ostream &AttrMethod::print(ostream &out) const {
   out << name << "(";
   for (RPTypeNode *n = args->head(); n; ) {
      out << *n->type;
      n = args->next(n);
      if (n)
	 out << ", ";
   }   
   if (varargs)
      out << ", ...";
   out << ")";
   return out;
}

ostream &AttrRPAttr::print(ostream &out) const {
   out << "rp-attribute: " << name;
   for (AttrMethod *nd = methods->head(); nd; nd = methods->next(nd))
      out << " " << *nd;
   return out;
}

ostream &AttrAttr::print(ostream &out) const {
   out << "attr: " << _name << " syntax(?) ?";

   return out;
}








const AttrMethod *AttrRPAttr::searchMethod(const char *name) const {
   char buffer[1024] = "";

   if (!isalpha(*name))
      strcpy(buffer, "operator");

   strcat(buffer, name);

   // return method or NULL
   for (AttrMethod *m = methods->head(); m; m = methods->next(m))
      if (!strcasecmp(m->name, buffer))
	 return m;

   return NULL;
}

const AttrMethod *AttrRPAttr::searchNextMethod(const AttrMethod *last) const {
   // return method or NULL
   const AttrMethod *m = last;

   if (!last)
      return NULL;

   for (m = methods->next(m); m; m = methods->next(m))
      if (!strcasecmp(m->name, last->name))
	 return m;

   return NULL;
}

void AttrProtocol::startMandatoryCheck() {
   for (AttrProtocolOption *m = options->head(); m; m = options->next(m))
      m->found = false;
}

const AttrProtocolOption *AttrProtocol::missingMandatoryOption() {
   AttrProtocolOption *m, *n;

   for (m = options->head(); m; m = options->next(m)) {
      if (! m->found && ! m->optional)
	 return m;
      if (m->found)
	 for (n = options->next(m); n; n = options->next(n))
	    if (!strcasecmp(m->option->name, n->option->name))
	       n->found = true;
   }
   
   return NULL;
}

const AttrProtocolOption *AttrProtocol::searchOption(const char *name) const {
   // return method or NULL
   for (AttrProtocolOption *m = options->head(); m; m = options->next(m))
      if (!strcasecmp(m->option->name, name)) {
	 m->found = true;
	 return m;
      }

   return NULL;
}

const AttrProtocolOption *AttrProtocol::searchNextOption
   (const AttrProtocolOption *last) const {
   // return method or NULL
   AttrProtocolOption *m = (AttrProtocolOption *) last;

   if (!last)
      return NULL;

   for (m = options->next(m); m; m = options->next(m))
      if (!strcasecmp(m->option->name, last->option->name)) {
	 m->found = true;
	 return m;
      }

   return NULL;
}

const AttrPeerOption *AttrPeer::searchOption(const char *name) const {
   // return method or NULL
   for (AttrPeerOption *m = options->head(); m; m = options->next(m))
      if (!strcasecmp(m->option, name))
	 return m;

   return NULL;
}

bool AttrMethod::validateArgs(ItemList *actualArgs, int &position, 
			      const RPType *&correctType) const {
   RPTypeNode *tnode = args->head();
   Item *arg = actualArgs->head();
   position = 0;
   Item *arg2;

   if (!tnode && !arg)
      return true;

   if (!tnode)
      return false;

   position = 1;
   correctType = tnode->type;
   while (correctType && arg) {
      if (!correctType->validate(arg)) {
	 arg2 = correctType->typeCast(arg);
	 if (arg2) {
	    actualArgs->insertAfter(arg, arg2);
	    actualArgs->remove(arg);
	    delete arg;
	    arg = arg2;
	 } else
	    return false;
      }

      position++;
      arg = actualArgs->next(arg);
      tnode = args->next(tnode);
      correctType = tnode ? tnode->type : NULL; 
   }
   
   if (!correctType && arg && varargs) { // extra args
      tnode = args->tail();
      correctType = tnode->type;
      while (correctType && arg) {
	 if (!correctType->validate(arg)) {
	    arg2 = correctType->typeCast(arg);
	    if (arg2) {
	       actualArgs->insertAfter(arg, arg2);
	       actualArgs->remove(arg);
	       delete arg;
	       arg = arg2;
	    } else
	       return false;
	 }

	 arg = actualArgs->next(arg);
	 position++;
      }
      correctType = (RPType *) NULL;
   }

   if (!correctType && !arg)
      return true;
   
   position = 0;
   return false;
}

AttrClass::~AttrClass() {
   if (object)
      delete object;
   else
      for (int i = 0; i < attribs.size(); ++i)
	 delete attribs[i];
   free(name);
}

void AttrClass::reset() {
   for (int i = 0; i < attribs.size(); ++i)
      attribs[i]->reset();
}

AttrAttr* AttrClass::searchAttr(const char *name) {
   for (int i = 0; i < attribs.size(); ++i)
      if (!strcasecmp(name, attribs[i]->name())) {
	 attribs[i]->touch();
	 return attribs[i];
      }
      
   return NULL;
}

bool AttrClass::validate(std::string &errors) {
   // return true if valid i.e. no error
   static char buffer[1024];
   bool result = true;

   for (int i = 0; i < attribs.size(); ++i) {
      if (!attribs[i]->testKey()) {
	 sprintf(buffer, "***Error: key attribute %s is not specified.\n",
		 attribs[i]->name());
	 errors += buffer;
	 result = false;
      }
      if ((!schema.isForgiving() || attribs[i]->isKey())
	  && !attribs[i]->testMandatory()) {
	 sprintf(buffer, "***Error: mandatory attribute %s is not specified.\n",
		 attribs[i]->name());
	 errors += buffer;
	 result = false;
      }
      if ((!schema.isForgiving() || attribs[i]->isKey())
	  && !attribs[i]->testSingleValued()) {
	 sprintf(buffer, 
		 "***Error: single-valued attribute %s occurs multiple times.\n",
		 attribs[i]->name());
	 errors += buffer;
	 result = false;
      }
   }
   return result;
}

ostream& AttrClass::print(ostream &out) const {
   return out;
}

ostream& AttrMntRoutes::print(ostream &out) const {
   out << "mnt-routes:";
   for (MntPrfxPair *p = mntPrfxList->head(); p; p = mntPrfxList->next(p)) {
      out << " " << p->mnt;
      if (p->prefixes)
	 out << " " << p->prefixes;
      out << ",\n";
   }
   return out;
}

ostream& AttrTRLabel::print(ostream &out) const {
   out << "transaction-label: " 
       << *source << " " << *seq << " " << *stamp;
   return out;
}

ostream& operator<<(ostream& stream, const Tunnel& p) {
   stream << p.remote_ip->get_ip_text() << " ";
   if (p.encapsulation)
     stream << *p.encapsulation ;
   return stream; 
}

