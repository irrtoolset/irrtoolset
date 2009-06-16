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

#ifndef OBJECT_HH
#define OBJECT_HH

#include "config.h"
#include <cstring>
#include <string>
#include <fstream>  // For class ifstream
#include <sstream>  // For class ostream, class istream
#include "List.hh"
#include "Buffer.hh"
#include "irrutil/debug.hh"
#include "rpsl_attr.hh"

#define RPSL_CLASS_UNKONWN 0
#define RPSL_CLASS_AUTNUM 1

class Buffer;

class Object : public Buffer {
public:
   List<Attr> attrs;	        // object's attributes

public:
   AttrClass *type;		// object's type
   bool  isDeleted;             // object is being deleted
   bool  has_error;             // the object's definition has error
   bool  has_warning;           // the object's definition has error
   std::string errors;          // errors about this object
   // note that individual errors are stored with each attribute
   // these are the object wide errors such as mandatory attribute missing

public:
   Object() : Buffer(), type(NULL),
      has_error(false), has_warning(false), isDeleted(false) {}
   Object(Buffer &buf, bool silent = false) 
      : Buffer(), type(NULL),
	 has_error(false), has_warning(false), isDeleted(false)  {
      this->destructiveCopy(buf);
      if (silent)
	 parse();
      else
	 scan();
   }
   // Modified by wlee to fix a fatal bug while duplicating objects
   //   Object(const Object& obj) : attrs(obj.attrs), errors(obj.errors) {
   Object(const Object& obj) : Buffer(obj), errors(obj.errors) {
     has_error   = obj.has_error;
     has_warning = obj.has_warning;
     isDeleted   = obj.isDeleted;
     type        = obj.type;
     // Added by wlee
     for (Attr *attr = obj.attrs.head(); attr; attr = obj.attrs.next(attr))
        attrs.append(attr->dup());
     for (Attr *attr = attrs.head(); attr; attr = attrs.next(attr))
	attr->object = this;
   }
   ~Object();

   Object &operator +=(Attr *attr) {
      if (attr->type && attr->type->isDeleted())
	 isDeleted = true;
      attrs.append(attr);
      attr->object = this;
      return *this;
   }

   bool read(std::istream &in = std::cin);
   static bool read(Buffer &buf, std::istream &in = std::cin);

   void parse();
   bool scan(const char *_text, const int sz, std::ostream &err = std::cerr);
   bool scan(std::ostream &err = std::cerr); 

   void reportErrors(std::ostream &ostrm = std::cerr);

   int getLen(void) const {
      return size;
   }

   char *getTextAt(int offset) const {
      return contents ? contents + offset : NULL;
   }
   char *getText__() const {
      return contents;
   }

   // if class cls exist, set class type to that, return true on success
   bool setClass(char *cls);
   // if attribute attr exist, add it to this class with value item
   // return true on success
   // assumes generic attribute
   bool addAttr(char *attr, Item *item);
   // like addAttr, but delete any other occurance of this attribute
   bool setAttr(char *attr, Item *item);
   bool setAttr(char *attrName, Attr *attr);

   friend std::istream& operator>>(std::istream &is, const Object &o);
   friend std::ostream& operator<<(std::ostream &os, const Object &o);
   
   bool hasAttr(const char *name);
   void validate();

   // Added by wlee@isi.edu
#ifdef ENABLE_DEBUG
   void printPTree(std::ostream &os) const;
#endif // #ifdef ENABLE_DEBUG
};

template<class AttrType = Attr>
class AttrIterator {
private:
   const Object  *object;
   AttrAttr      *type;
   Attr          *currentAttr;

// Made it protected by wlee
protected:
   virtual bool   isMatching(Attr *attr) {
      return true;
   }

public:
   AttrIterator(const Object *o, const char *attrib) : 
      object(o), type(NULL), currentAttr(NULL) {
      if (object && object->type)
	 type = object->type->searchAttr(attrib);
      if (!type)
	 object = NULL;
      first();
   }
   AttrIterator(const Object *o, AttrAttr *_type = NULL) :
      object(o), type(_type), currentAttr(NULL) {
      first();
   }
   const AttrType *first() {
      if (! object)
	 return (AttrType *) NULL;

      for (currentAttr = object->attrs.head(); 
	   currentAttr; 
	   currentAttr = object->attrs.next(currentAttr))
	 if (currentAttr->errors.empty()
	     && (!type 
		 || (currentAttr->type == type && isMatching(currentAttr))))
	    return (AttrType *) currentAttr;

      return (AttrType *) NULL;
   }
   const AttrType *next() {
      if (! (object && currentAttr))
	 return (AttrType *) NULL;
      for (currentAttr = object->attrs.next(currentAttr); 
	   currentAttr; 
	   currentAttr = object->attrs.next(currentAttr))
	 if (currentAttr->errors.empty()
	     && (!type 
		 || (currentAttr->type == type && isMatching(currentAttr))))
	    return (AttrType *) currentAttr;
      return (AttrType *) NULL;
   }
   // Added by wlee@isi.edu
   const AttrIterator &operator++() {
      next();
      return *this;
   }
   const AttrIterator &operator++(int) {
      next();
      return *this;
   }
   operator AttrType*() const { return (AttrType *) currentAttr; }
   AttrType* operator-> () const { return (AttrType *) currentAttr; }
   AttrType* operator()(void) const { return (AttrType *) currentAttr; }
};

template<class ItemType = Item>
class AttrGenericIterator {
   Item              *currentItem;
   const AttrGeneric *currentAttr;
   AttrIterator<AttrGeneric> itr;
public:
   AttrGenericIterator(const Object *o, const char *attrib) : 
      itr(o, attrib), currentItem(NULL), currentAttr(NULL) {
      first();
   }
   AttrGenericIterator(const Object *o, AttrAttr *_type = NULL) :
      itr(o, _type), currentItem(NULL), currentAttr(NULL) {
      first();
   }

   ItemType *first() {
      currentAttr = itr.first();
      if (! currentAttr || !currentAttr->items)
	 return (ItemType *) NULL;
      currentItem = currentAttr->items->head();
      while (!currentItem) {
	 currentAttr = itr.next();
	 if (! currentAttr || !currentAttr->items)
	    return (ItemType *) NULL;
	 currentItem = currentAttr->items->head();
      }
      return (ItemType *) currentItem;
   }
   ItemType *next() {
      currentItem = currentAttr->items->next(currentItem);
      while (!currentItem) {
	 currentAttr = itr.next();
	 if (! currentAttr || !currentAttr->items)
	    return (ItemType *) NULL;
	 currentItem = currentAttr->items->head();
      }
      return (ItemType *) currentItem;
   }

   // Added by wlee@isi.edu
   const AttrGenericIterator &operator++(int) {
      next();
      return *this;
   }
   const AttrGenericIterator &operator++() {
      next();
      return *this;
   }
   operator ItemType*() const { return (ItemType *) currentItem; }
   ItemType* operator-> () const { return  (ItemType *) currentItem; }
   ItemType* operator()(void) const { return (ItemType *) currentItem; }
};


#endif // OBJECT_HH
