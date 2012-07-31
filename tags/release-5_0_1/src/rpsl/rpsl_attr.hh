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

#ifndef PTREE_ATTR_HH
#define PTREE_ATTR_HH

#include "config.h"
#include <string>
#include <vector>
#include <sstream>  // For class ostream
#include <cassert>
#include "rptype.hh"
#include "regexp.hh"
extern "C" {
#ifdef HAVE_REGEX_H
#include <regex.h>
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif // HAVE_MALLOC_H
}
#include "List.hh"
#include "symbols.hh"
#include "prefix.hh"
#include "rpsl_item.hh"
#include "rpsl_policy.hh"

#ifdef ENABLE_DEBUG
#define INDENT(indent) for (int iii = 0; iii < indent; iii++) os << " "
#endif // ENABLE_DEBUG

typedef unsigned int ASt;

class Object;
class AttrAttr;

// Each object contains a list of Attr's
class Attr : public ListNode { 
public:
   Object   *object;        // pointer to the object which has this attr
   const AttrAttr *type;    // type of this attribute, import, export, etc.
   int       offset;        // offset of attribute's ASCII rep in object
   int       len;           // length of attribute's ASCII rep
   std::string errors;      // errors about this object
   int       errorLine;
   int       errorColon;
   int       errorLeng;

public:
   Attr(const AttrAttr *t) : object(NULL), type(t), offset(0), len(0), 
      errorLine(0), errorColon(0), errorLeng(0) {
   }
   Attr() : object(NULL), type(NULL), offset(0), len(0), 
      errorLine(0), errorColon(0), errorLeng(0) {
   }
   Attr(const Attr &b) : object(b.object), type(b.type), 
      offset(b.offset), len(b.len), errors(b.errors), 
      errorLine(b.errorLine), errorColon(b.errorColon), 
      errorLeng(b.errorLeng)  {
   }
   virtual ~Attr() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new Attr(*this);
   }
   Object *getObject(void) const { return object; }
   int getOffset(void) const { return offset; }
   int getLen(void) const { return len; }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "Attr";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "(*** Need more work here ***)" << std::endl;
   }
#endif // ENABLE_DEBUG
};

inline std::ostream &operator<<(std::ostream &out, const Attr &attr) {
   return attr.print(out);
}

///////////////////////// schema ///////////////////////////////////

class AttrAttr: public Attr {
   friend class Schema;
public: 
   enum    Options { OPTIONAL = 1, MULTIVALUED = 2, 
		     LOOKUP = 4, OBSOLETE = 8, KEY = 16, 
		     DELETED = 32, INTERNAL = 64 };
   char   *_name;
   int     _syntax;
   RPType *_subsyntax;
   int     _options;
   int     _refCount;
   regex_t *reComp;
   char    *re;
   int      reIndex;

public:
   AttrAttr() : _name(NULL), _syntax(0), _subsyntax(NULL), 
      _options(0), _refCount(0), reComp(NULL), re(NULL), reIndex(0) {}

   AttrAttr(Options opt) : _name(NULL), _syntax(0), _subsyntax(NULL), 
	 _options(opt), _refCount(0), reComp(NULL), re(NULL), reIndex(0) {}

   AttrAttr(char *__name, int __syntax, RPType *__subsyntax, 
		 int __options) :  
      _name(__name), _syntax(__syntax), _subsyntax(__subsyntax), 
      _options(__options), _refCount(0), reComp(NULL), re(NULL), reIndex(0) {
      strlwr(_name);
   }

   AttrAttr(int __syntax, RPType *__subsyntax) : _name(NULL), 
	 _syntax(__syntax), _subsyntax(__subsyntax), 
	 _options(0), _refCount(0), reComp(NULL), re(NULL), reIndex(0) {}

   AttrAttr(int __syntax, char *_re, regex_t *_reComp, int _reIndex = 0) : 
         _name(NULL), 
	 _syntax(__syntax), _subsyntax(NULL), 
	 _options(0), _refCount(0), 
	 reComp(_reComp), re(_re), reIndex(_reIndex) {}


   AttrAttr(const AttrAttr &b) : Attr(b) {
      if (b._name)
	 _name = strdup(b._name);
      else
	 _name = NULL;
      _syntax    = b._syntax;
      if (b._subsyntax)
	 _subsyntax = b._subsyntax->dup();
      else 
	 _subsyntax = NULL;
      if (b.re) {
	 re = strdup(b.re);
	 reComp = (regex_t *) malloc(sizeof(regex_t));
	 regcomp(reComp, re, REG_EXTENDED | REG_ICASE);
      } else {
	 re = NULL;
	 reComp = NULL;
      }
      reIndex    = b.reIndex;
      _options   = b._options;
      _refCount  = b._refCount;
   }

   virtual ~AttrAttr() {
      if (_name)
	 free(_name);
      if (_subsyntax)
	 delete _subsyntax;
      if (re) {
	 free(re);
	 regfree(reComp);
	 free(reComp);
      }
   }

   void operator|=(const AttrAttr& b) {
      if (!_name && b._name) 
	 _name = strdup(b._name);
      if (b._syntax) {
	 // 	 if (_syntax)
	 // 	    delete _syntax;
 	 if (_subsyntax)
 	    delete _subsyntax;
	 _syntax = b._syntax;
	 if (b._subsyntax)
	    _subsyntax = b._subsyntax->dup();
	 else
	    _subsyntax = NULL;
      }
      if (b.re) {
	 re = strdup(b.re);
	 reComp = (regex_t *) malloc(sizeof(regex_t));
	 regcomp(reComp, re, REG_EXTENDED | REG_ICASE);
	 reIndex = b.reIndex;
      } else {
	 re = NULL;
	 reComp = NULL;
	 reIndex = 0;
      }
      _options |= b._options;
      _refCount += b._refCount;
   }
   void setName(char *__name) {
      if (_name)
	 free(_name);
      _name = __name;
      strlwr(_name);
   }

   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrAttr(*this);
   }

   bool testMandatory() const { 
      // return true if there is no problem
      return ((_options & OPTIONAL) || _refCount > 0);
   }
   bool testKey() const { 
      // return true if there is no problem
      return (! (_options & KEY) || _refCount > 0);
   }
   bool testSingleValued() const { 
      // return true if there is no problem
      return ((_options & MULTIVALUED) || _refCount <= 1);
   }
   bool shouldParse() const {
      // return true if this attribute should be parsed
      return _syntax 
	 && (!_subsyntax || typeid(*_subsyntax) != typeid(RPTypeFreeText));
   }
   int syntax() const {
      return _syntax;
   }
   RPType *subsyntax() const {
      return _subsyntax;
   }
   bool isSingleValued() const {
      return ! (_options & MULTIVALUED);
   }
   bool isObsolete() const {
      return (_options & OBSOLETE);
   }
   bool isKey() const {
      return (_options & KEY);
   }
   bool isDeleted() const {
      return (_options & DELETED);
   }
   bool isInternal() const {
      return (_options & INTERNAL);
   }
   bool isLookup() const {
      return (_options & LOOKUP);
   }
   char *name() const {
      return _name;
   }
   void reset() {
      _refCount= 0;
   }
   void touch() {
      _refCount++;
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrAttr";
   }
   virtual void printClass(std::ostream &os, int indent) const {
     INDENT(indent); os << "_name = \"" << name() << "\"" << std::endl;
   }
#endif // ENABLE_DEBUG
};

class AttrClass: public Attr {
   friend class Object;
   friend class IndexData;
public:
   char *name;		          // name of the class, e.g. "route"
   std::vector<AttrAttr *> attribs;    // table of attributes of this class
   std::vector<AttrAttr *> keyAttribs; // table of attributes that form the key
   int ref_cnt;

public:
   AttrClass(char *n) : name(n) {
      strlwr(name);
      ref_cnt = 0;
   }
   AttrClass(const AttrClass &b) : Attr(b) {
      ref_cnt = 0;
      name = strdup(b.name);

      for (int i = 0; i < b.attribs.size(); ++i) {
	 AttrAttr *attr = new AttrAttr(*b.attribs[i]);
	 attribs.push_back(attr);
	 if (b.attribs[i]->isKey())
	    keyAttribs.push_back(attr);
      }	 
   }
   virtual ~AttrClass();
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() {
      return new AttrClass(*this);
   }

   void addAttr(AttrAttr *attr) {
      attribs.push_back(attr);
      if (attr->isKey())
	 keyAttribs.push_back(attr);
   }
   
   void reset();
   void touch() {
      ref_cnt++;
   }

   char *getName() const {
      return name;
   }
   AttrAttr *searchAttr(const char *word);

   bool validate(std::string &errors); // return true if valid,, i.e. no error
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrClass";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "(*** Need more work here ***)" << std::endl;
   }
#endif // ENABLE_DEBUG
};

///////////////////////// common ///////////////////////////////////

class AttrGeneric: public Attr {
public: 
   ItemList *items;
public:
   AttrGeneric(const AttrAttr *type, ItemList *_items) 
      : Attr(type), items(_items) {
   }
   AttrGeneric(const AttrGeneric &pt) : Attr(pt) {
      items = pt.items ? (ItemList *) pt.items->dup() : (ItemList *) NULL;
   }
   virtual ~AttrGeneric() {
      delete items;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrGeneric(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrGeneric";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "type" << std::endl;
      type->printClass(os, indent + 2);
      INDENT(indent); os << "items (" << items->className() << " *)" << std::endl;
      items->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

///////////////////////// dictionary ///////////////////////////////////

class AttrTypedef: public Attr  {
public:
   char   *name;
   RPType *type;
public:
   AttrTypedef(char *_name, RPType *_type) : name(_name), type(_type) {}
   AttrTypedef(const AttrTypedef &b) : Attr(b) {
      if (b.type)
	 type = b.type->dup();
      else
	 type = NULL;
      name = strdup(b.name);
   }
   virtual ~AttrTypedef() {
      free(name);
      if (type)
	 delete type;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrTypedef(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrTypedef";
   }
#endif // ENABLE_DEBUG
};

class AttrMethod: public ListNode {
public:
   char                    *name;
   bool                     varargs;
   bool                     isOperator;
   List<RPTypeNode>        *args;
public:
   AttrMethod(char *_name, List<RPTypeNode> *_args, 
	      bool _varargs = false, bool _isOperator = false) :
      name(_name), args(_args), varargs(_varargs), isOperator(_isOperator) {}
   AttrMethod(const AttrMethod &b) {
      name = strdup(b.name);
      varargs = b.varargs;
      if (b.args)
	 args = new List<RPTypeNode>(*b.args);
      else
	 args = NULL;
   }
   virtual ~AttrMethod() {
      free(name);
      if (args)
	 delete args;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual AttrMethod *dup() const {
      return new AttrMethod(*this);
   }
   bool validateArgs(ItemList *actualArgs, 
		     int &position, const RPType *&correctType) const;
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrMethod";
   }
#endif // ENABLE_DEBUG
};

inline std::ostream &operator<<(std::ostream &out, const AttrMethod &mtd) {
   return mtd.print(out);
}

class AttrRPAttr: public Attr {
public:
   char             *name;
   List<AttrMethod> *methods;
public:
   AttrRPAttr(char *_name, List<AttrMethod> *_methods) :
      name(_name), methods(_methods) {}
   // AttrRPAttr(const AttrRPAttr &b) {
   AttrRPAttr(const AttrRPAttr &b) : Attr(b) {
      if (b.methods)
	 methods = new List<AttrMethod>(*b.methods);
      else
	 methods = NULL;

      name = strdup(b.name);
   }
   virtual ~AttrRPAttr() {
      free(name);
      if (methods)
	 delete methods;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrRPAttr(*this);
   }
   const AttrMethod *searchMethod(const char *name) const; 
   const AttrMethod *searchNextMethod(const AttrMethod *last) const;
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrRPAttr";
   }
#endif // ENABLE_DEBUG
};

class AttrProtocolOption: public ListNode {
public:
   bool        optional;
   AttrMethod *option;
   bool        found;

public:
   AttrProtocolOption(bool _optional, AttrMethod *opt) : 
      optional(_optional), option(opt), found(false) {}
   AttrProtocolOption(const AttrProtocolOption &b) :
      optional(b.optional), found(b.found) {
      option = b.option->dup();
   }
   virtual ~AttrProtocolOption() {
      if (option)
	 delete option;
   }

   virtual std::ostream& print(std::ostream &out) const;
   virtual AttrProtocolOption *dup() {
      return new AttrProtocolOption(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrProtocolOption";
   }
#endif // ENABLE_DEBUG
};

class AttrProtocol: public Attr  {
public:
   char *name;
   List<AttrProtocolOption> *options;
public:
   AttrProtocol(char *_name, List<AttrProtocolOption> *_options) : 
      name(_name), options(_options) {}
   AttrProtocol(const AttrProtocol &b) : Attr(b) {
      if (b.name)
	 name = strdup(b.name);
      else
	 name = NULL;
      if (b.options)
	 options = new List<AttrProtocolOption>(*b.options);
      else
	 options = NULL;
   }
   virtual ~AttrProtocol() {
      if (name)
	 free(name);
      if (options)
	 delete options;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrProtocol(*this);
   }

   const AttrProtocolOption *searchOption(const char *name) const; 
   const AttrProtocolOption *searchNextOption(const AttrProtocolOption *last) const;

   void startMandatoryCheck();
   const AttrProtocolOption *missingMandatoryOption();
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrProtocol";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "name = \"" << name << "\"" << std::endl;
   }
#endif // ENABLE_DEBUG
};

///////////////////////// aut-num ///////////////////////////////////

/// mp-import/import

class AttrImport: public Attr {
public:
   PolicyExpr *policy;
   const AttrProtocol *fromProt;
   const AttrProtocol *intoProt;
public:
   AttrImport(const AttrProtocol *fprot, 
	      const AttrProtocol *iprot, 
	      PolicyExpr *p) : 
      policy(p), fromProt(fprot), intoProt(iprot)  {}

   AttrImport(const AttrImport &b) : Attr(b) {
      policy = (PolicyExpr *) b.policy->dup();
      fromProt = b.fromProt;
      intoProt = b.intoProt;
   }
   virtual ~AttrImport() {
      delete policy;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrImport(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrImport";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "fromProt" << std::endl;
      if (fromProt) 
	 fromProt->printClass(os, indent + 2);
      else {
	 INDENT(indent); os << "  NULL" << std::endl;
      }
      INDENT(indent); os << "intoProt" << std::endl;
      if (intoProt) 
	 intoProt->printClass(os, indent + 2);
      else {
	 INDENT(indent); os << "  NULL" << std::endl;
      }
      INDENT(indent); os << "policy (" << policy->className() << " *)" << std::endl;
      policy->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

// mp-export/export

class AttrExport: public Attr {
public:
   PolicyExpr *policy;
   const AttrProtocol *fromProt;
   const AttrProtocol *intoProt;
public:
   AttrExport(const AttrProtocol *fprot, 
	      const AttrProtocol *iprot, 
	      PolicyExpr *p) : 
      policy(p), fromProt(fprot), intoProt(iprot)  {}

   AttrExport(const AttrExport &b) : Attr(b) {
      policy = (PolicyExpr *) b.policy->dup();
      fromProt = b.fromProt;
      intoProt = b.intoProt;
   }
   virtual ~AttrExport() {
      delete policy;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrExport(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrExport";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "fromProt" << std::endl;
      if (fromProt) 
	 fromProt->printClass(os, indent + 2);
      else {
	 INDENT(indent); os << "  NULL" << std::endl;
      }
      INDENT(indent); os << "intoProt" << std::endl;
      if (intoProt) 
	 intoProt->printClass(os, indent + 2);
      else {
	 INDENT(indent); os << "  NULL" << std::endl;
      }
      INDENT(indent); os << "policy (" << policy->className() << " *)" << std::endl;
      policy->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

// mp-default/default

class AttrDefault: public Attr {
public: 
   ItemList *afi_list;
   PolicyPeering      *peering;
   PolicyActionList   *action;
   Filter             *filter;
public:
   AttrDefault(ItemList *af, PolicyPeering *prng, PolicyActionList *act, Filter *flt) : 
      afi_list(af), peering(prng), action(act), filter(flt) {
   }
   AttrDefault(PolicyPeering *prng, PolicyActionList *act, Filter *flt):
      afi_list(new ItemList), peering(prng), action(act), filter(flt)
   {
      afi_list->append(new ItemAFI(new AddressFamily("ipv4.unicast")));
   }

   AttrDefault(const AttrDefault &pt) : Attr(pt) {
      afi_list = new ItemList(*pt.afi_list);
      peering = new PolicyPeering(*pt.peering);
      action  = new PolicyActionList(*pt.action);
      filter  = (Filter *)pt.filter->dup();
   }
   virtual ~AttrDefault() {
      delete afi_list;
      delete peering;
      delete action;
      delete filter;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrDefault(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrDefault";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "peering (PolicyPeering *)" << std::endl;
      peering->printClass(os, indent + 2);
      INDENT(indent); os << "action (PolicyActionList *)" << std::endl;
      action->printClass(os, indent + 2);
      INDENT(indent); os << "filter (Filter *)" << std::endl;
      filter->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

class AttrFilter: public Attr {
public: 
   Filter             *filter;
public:
   AttrFilter(Filter *flt) : filter(flt) {
   }
   AttrFilter(const AttrFilter &pt) : Attr(pt) {
      filter  = (Filter *)pt.filter->dup();
   }
   virtual ~AttrFilter() {
      delete filter;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrFilter(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrFilter";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "filter (Filter *)" << std::endl;
      filter->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

class AttrMPPeval: public Attr {
public:
   Filter             *filter;
public:
   AttrMPPeval(Filter *flt) : filter(flt) {
   }
   AttrMPPeval(const AttrMPPeval &pt) : Attr(pt) {
      filter  = (Filter *)pt.filter->dup();
   }
   virtual ~AttrMPPeval() {
      delete filter;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrMPPeval(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrMPPeval";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "mp-peval (Filter *)" << std::endl;
      filter->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

// peering mp-peering

class AttrPeering: public Attr {
public: 
   PolicyPeering    *peering;
public:
   AttrPeering(PolicyPeering *p) : peering(p) {
   }
   AttrPeering(const AttrPeering &pt) : Attr(pt) {
      peering  = (PolicyPeering *)pt.peering->dup();
   }
   virtual ~AttrPeering() {
      delete peering;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrPeering(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrPeering";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "peering (PolicyPeering *)" << std::endl;
      peering->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

///////////////////////// inet-rtr ///////////////////////////////////
/*
class AttrIfAddr: public Attr {
public: 
   Prefix              ifaddr;
   PolicyActionList   *action;
public:
   AttrIfAddr(int ip, int masklen, PolicyActionList *_action) {
      action = _action;
      ifaddr.define(ip, masklen);
   }
   AttrIfAddr(const AttrIfAddr& b) : ifaddr(b.ifaddr) {
      action = (PolicyActionList *) b.action->dup();
   }
   virtual ~AttrIfAddr() {
      if (action)
	 delete action;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrIfAddr(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrIfAddr";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); 
      // ??? This causes a lot of warning mesg right now
      // It's hard to fix since it involves modification to
      // hundred of places across the whole src tree
      //     os << "ifaddr = " << ifaddr << std::endl;
      os << "(*** Need to fix const declaration in the extraction operator ***)"
	 << std::endl;
   }
#endif // ENABLE_DEBUG
};
*/
class Tunnel {
  public:
    MPPrefix *remote_ip;
    ItemWORD *encapsulation;
  
  public:
    Tunnel(MPPrefix *_ip, ItemWORD *_enc) {
      remote_ip = _ip;
      encapsulation = _enc;
    }
    Tunnel *dup() {
      return new Tunnel(new MPPrefix(*remote_ip), new ItemWORD(*encapsulation));
    }
    ~Tunnel() {
      if (remote_ip)
        delete (remote_ip);
      if (encapsulation)
        delete encapsulation;
    }
    friend std::ostream& operator<<(std::ostream& stream, const Tunnel& p);
};

class AttrIfAddr: public Attr {
public:
   MPPrefix           *ifaddr;
   PolicyActionList   *action; // may be NULL
   Tunnel             *tunnel; // may be NULL

public:
   AttrIfAddr(MPPrefix *ip, int masklen, PolicyActionList *_action, Tunnel *_tunnel):  action(NULL), tunnel(NULL) {
      if (_action)
        action = _action;
      if (_tunnel)
        tunnel = _tunnel;
      ifaddr = ip;
      ifaddr->define(masklen);
   }
   AttrIfAddr(const AttrIfAddr& b) : action(NULL), tunnel(NULL) {
      ifaddr = new MPPrefix(*b.ifaddr);
      if (b.action)
        action = (PolicyActionList *) b.action->dup();
      if (b.tunnel)
        tunnel = (Tunnel *) b.tunnel->dup();
   }
   virtual ~AttrIfAddr() {
      if (action)
        delete action;
      if (tunnel)
        delete tunnel;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrIfAddr(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrIfAddr";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent);
      // ??? This causes a lot of warning mesg right now
      // It's hard to fix since it involves modification to
      // hundred of places across the whole src tree
      //     os << "ifaddr = " << ifaddr << std::endl;
      os << "(*** Need to fix const declaration in the extraction operator ***)"
   << std::endl;
   }
#endif // ENABLE_DEBUG
};


class AttrPeerOption: public ListNode {
public: 
   char                     *option;    // option name
   ItemList                 *args;      // arguments to the option
public:
   AttrPeerOption(char *_option, ItemList *_args) : 
      option(_option), args(_args) {}

   AttrPeerOption(const AttrPeerOption &b) {
      option = strdup(b.option);
      args = (ItemList *) b.args->dup();
   }

   virtual ~AttrPeerOption() {
      if (option)
	 free(option);
      if (args) 
	 delete args;
   }

   virtual std::ostream& print(std::ostream &out) const;
   virtual AttrPeerOption* dup() const {
      return new AttrPeerOption(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrPeerOption";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      // For rp_attr
      INDENT(indent); os << "rp_attr" << std::endl;
      INDENT(indent); os << "  _name = \"" << "\"" << std::endl;
      INDENT(indent); os << "  methods (RPMethod *)" << std::endl;
      INDENT(indent); os << "    _name = \"" << "\"" << std::endl;
      // For rp_method
      INDENT(indent); os << "rp_method" << std::endl;
      INDENT(indent); os << "  _name = \"" << "\"" << std::endl;
      // For rp_args
      INDENT(indent); os << "args (ItemList *)" << std::endl;
      args->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

// serves both peer and mp-peer
class AttrPeer: public Attr {
public:
   const AttrProtocol   *protocol;
   MPPrefix             *peer;
   List<AttrPeerOption> *options;
public:
   AttrPeer(const AttrProtocol *_protocol, MPPrefix *_peer,
      List<AttrPeerOption> *_options) :
      protocol(_protocol), peer(_peer), options(_options) {
   }
   AttrPeer(const AttrPeer &b) : Attr(b), protocol(b.protocol) {
      peer = new MPPrefix(*b.peer);
      options = new List<AttrPeerOption>(*b.options);
   }
   virtual ~AttrPeer() {
      if (peer) delete peer;
      if (options) delete options;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Attr *dup() const {
      return new AttrPeer(*this);
   }
   const AttrPeerOption *searchOption(const char *name) const;
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrPeer";
   }
#endif // ENABLE_DEBUG
};



class AttrMntRoutes: public Attr {
public: 
   class MntPrfxPair : public ListNode {
   public:
      char           *mnt;
      FilterMPPRFXList *prefixes;


      MntPrfxPair(char *mntner, FilterMPPRFXList *prfxs) : 
	 mnt(mntner), prefixes(prfxs) {
      }

      MntPrfxPair(const MntPrfxPair &b) {
	 if (b.prefixes)
	    prefixes = new FilterMPPRFXList(*b.prefixes);
	 else
	    prefixes = NULL;
	 mnt = strdup(b.mnt);
      }

      ~MntPrfxPair() {
	 if (prefixes)
	    delete prefixes;
	 free(mnt);
      }
   };

   List<MntPrfxPair> *mntPrfxList;
public:
   AttrMntRoutes(List<MntPrfxPair> *_mntPrfxList): mntPrfxList(_mntPrfxList) {}
   AttrMntRoutes(const AttrMntRoutes &b) : Attr(b) {
      mntPrfxList = new List<MntPrfxPair>(*b.mntPrfxList);
   }
   virtual ~AttrMntRoutes() {
      delete mntPrfxList;
   }

   virtual std::ostream& print(std::ostream &out) const;

   virtual Attr *dup() const {
      return new AttrMntRoutes(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "AttrMntRoutes";
   }
#endif // ENABLE_DEBUG
};

class AttrTRLabel: public Attr {
public:
   ItemWORD      *source;
   ItemINT       *seq;
   ItemTimeStamp *stamp;

public:
   AttrTRLabel(ItemWORD *so, ItemINT *sq, ItemTimeStamp *ts) :
      source(so), seq(sq), stamp(ts) {
   }
   ~AttrTRLabel() {
      delete source;
      delete seq;
      delete stamp;
   }
   AttrTRLabel(const AttrTRLabel &b) {
      source = new ItemWORD(*b.source);
      seq    = new ItemINT(*b.seq);
      stamp  = new ItemTimeStamp(*b.stamp);
   }

   virtual Attr *dup() const {
      return new AttrTRLabel(*this);
   }
   virtual std::ostream& print(std::ostream &out) const;
};

#endif   // PTREE_ATTR_HH
