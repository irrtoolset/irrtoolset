//  $Id$
// copyright (c) 2001,2002                        RIPE NCC
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
//             WeeSan Lee <wlee@isi.edu>
//             Ramesh Govindan
//             Yuri Pryadkin
//             John Mehringer <mehringe@isi.edu>

#ifndef LIST_H
#define LIST_H

#include <cstdio>      // For NULL definition
#ifdef _STDDEF_H_      // for fbsd: to workaround problem with _G_config.h
#undef _STDDEF_H_
#endif
#include <assert.h>

// These list manipulation routines assume that each object that
// might be on a list is derived from a ListNode.

template <class T> class List;

class ListNode {
public:
   ListNode *forw;
   ListNode *back;

public:
   ListNode() {
      forw = back = this;
   }
   ListNode(const ListNode& ) {
      forw = back = this;
   }
   ~ListNode() {
       assert(forw==this && back==this);
   };
   
//     ListNode& operator=(const ListNode& b) {
//        forw = back = this;
//        return 
//     }

   // Remove us from any list we're on
   void __unlink__() {
      forw->back = back;
      back->forw = forw;
      forw = back = this;
   }

   // Insert us between before and after
   void __link__(ListNode *before, ListNode *after) {
      forw = after;
      back = before;
      before->forw = after->back = this;
   }
};

typedef ListNode DListNode;

template <class T> class List;
//template <class T, class Key=unsigned long> class ListIterator;
//template <class T, class Key=unsigned long> class const_ListSearchIterator;
//template <class T, class Key=unsigned long> class ListSearchIterator;

template <class T> class const_ListIterator {
    //	friend ListIterator<T,Key>;
    //	friend const_ListSearchIterator<T,Key>;
    //	friend ListSearchIterator<T,Key>;
public:
	const_ListIterator () : position(0) {};
	const_ListIterator (const List<T> &l, ListNode *p= 0) : list_(&l), ptr_(p?p:l.forw), position(0) {}

	operator bool () const { return ptr_!=(ListNode*)list_; }
	operator const T* () const { return (ptr_==(ListNode*)list_) ? 0 : (T*)ptr_; }
	const T* operator->() const { 
		assert(ptr_!=(ListNode*)list_); 
		return (T*)ptr_; 
	}
	const_ListIterator& operator++ () { // prefix ++
		ptr_= ptr_->forw;  //list_->next(ptr_);
		position++;
		return *this;
	}
	const_ListIterator& operator-- () { // prefix --
		ptr_= ptr_->back;  //list_->prev(ptr_);
		position--;
		return *this;
	}
//  	bool operator== (const T* t) const {
//  		return (ptr_ == t);
//  	}
//  	bool operator!= (const T* t) const {
//  		return (ptr_ != t);
//  	}
	

   int sizeBefore() const {
      return position;
   }
   int sizeAfter() const {
      return list_->size() - position;
   }
protected:
    const List<T> *list_;
    ListNode* ptr_;
   int position;
};

template<class T> class ListIterator : public const_ListIterator<T> {
public:
	ListIterator () {}
	ListIterator (const const_ListIterator<T>& cit) : const_ListIterator<T>(cit.list_, cit.ptr_) {}
	ListIterator (const List<T> &l, ListNode *p=0) : const_ListIterator<T>(l, p?p:l.forw) {}

	operator T* () const { return (this->ptr_==(ListNode*)this->list_) ? 0 : (T*)this->ptr_; }
	T* operator-> () const { return (T*)const_ListIterator<T>::operator->(); }
	ListIterator& operator++ () { // prefix ++
		const_ListIterator<T>::operator++();
		return *this;
	}
	ListIterator& operator-- () { // prefix --
		const_ListIterator<T>::operator--(); 
		return *this;
	}
};
template <class T, class Key> class const_ListSearchIterator 
	: public const_ListIterator<T> {
    //friend ListSearchIterator<T,Key>;
public:
	const_ListSearchIterator () {};
	const_ListSearchIterator (const Key& k, const List<T>& l, ListNode *p=0) 
		: const_ListIterator<T>(l, p?p:(ListNode*)&l), key_(k) {}

	const_ListSearchIterator& operator++ () { // prefix ++
		for (const_ListIterator<T>::operator++();
		     this->ptr_!=(ListNode*)this->list_;
		     const_ListIterator<T>::operator++()) {
			if (((T*)this->ptr_)->isKey(key_)) break;
		}
		return *this;
	}
	const_ListSearchIterator& operator-- () { // prefix --
		for (const_ListIterator<T>::operator--();
		     this->ptr_!=(ListNode*)this->list_;
		     const_ListIterator<T>::operator--()) {
			if (((T*)this->ptr_)->isKey(key_)) break;
		}
		return *this;
	}
protected:
	Key key_;
};
template <class T, class Key> class ListSearchIterator 
	: public const_ListSearchIterator<T,Key> {
public:
	ListSearchIterator() {};
	ListSearchIterator (const const_ListSearchIterator<T,Key>& csit) 
		: const_ListSearchIterator<T,Key>(csit.key_, csit.list_, csit.ptr_) { }
	ListSearchIterator (const Key& k, const List<T> &l, ListNode *p= 0) 
		: const_ListSearchIterator<T, Key>(k, l, p?p:(ListNode*)&l) {}
	

	operator T* () const { return (this->ptr_==(ListNode*)this->list_) ? 0 : (T*)this->ptr_; }
	T* operator-> () const { return (T*)const_ListIterator<T>::operator->(); }
	ListSearchIterator& operator++ () { // prefix ++
		const_ListSearchIterator<T,Key>::operator++();
		return *this;
	}
	ListSearchIterator& operator-- ()  { // prefix --
		const_ListSearchIterator<T,Key>::operator--();
		return *this;
	}
};

// doubly linked list implemented as a circular list 
// the listhead is also a listnode, except its data pointer points to NULL
// this assumes no non-listhead listnode's data pointer can point to NULL
template <class T>
class List : public ListNode {
protected:
   int       length;

public:
   List() : ListNode() {
      length = 0;
   }
	
   List(const List& b) : ListNode() { 
      // copies the list b, including the data pointed to
      // the data pointed to is copied using its copy constructor
      // and the bits about this link list is changed explicitly.
      // make sure that the data can be correctly copied using a cconst.
      // Can be safely used if T contains only one ListNode in it

      length = 0;
      for (ListNode *e = b.forw; e != (ListNode*)&b; e = e->forw) {
	 append(new T(*(T*)e));
      }
   }

   ~List(void) { 
      clear(); 
   }

   List& operator=(const List& b) {
      // This routine blindly free's memory of old elements if any
      // This routine assumes that the elements in this are not accessible
      // via other means.
      // If this assumption does not hold for your system, do not use the = op.
      clear();

      for (ListNode *e = b.forw; e != (ListNode*)&b; e = e->forw) {
	 append(new T(* (T *) e));
      }

      return *this;
   }

   bool isEmpty() const {
      return (forw == (const ListNode *) this);
   }
   bool isSingleton() const {
      return !isEmpty() && forw == back;
   }

   int size() const {
#ifdef LISTNODES_CAN_MODIFY_LIST
      int	sz = 0;
      for (ListNode* e = forw; e != (ListNode *) this; e = e->forw)
	 sz++;
      return sz;
#else
      return length;
#endif
   }

   void remove(T *t) {
      t->__unlink__();
      length--;
   }

   void unlink() { // destroys the list, but keeps the elements
      for (ListNode *e = forw; e != (ListNode *) this; e = forw)
	 e->__unlink__();
      length = 0;
   }

   void clear() { // destroys the list, and destroys the elements
      for (ListNode *e = forw; e != (ListNode *) this; e = forw) {
	 e->__unlink__();
	 delete (T*)e;
      }
      length = 0;
   }

   // Remove and return the head of the list
   T*
   dequeue() {
      T* node;

      if (forw == (ListNode *) this && back == (ListNode *) this) {
	 return (T*)NULL;
      }
      //length--;
      node = (T*)forw;
      remove(node);
      return node;
   }
   T* betail() { // Remove and return the tail of the list (cf behead)
      T* node;
      if (forw == (ListNode *) this && back == (ListNode *) this) {
	 return 0;
      }
      node = (T*)back;
      remove(node);
      return node;
   }

   void append(T *t) {
      t->__link__(back, this);
      length++;
   }

   void insert(T *t) {
      t->__link__(back, this);
      length++;
   }

   void prepend(T *t) {
      t->__link__(this, forw);
      length++;
   }

   void insertBefore(T *e, T *t) {
      t->__link__(e->back, e);
      length++;
   }

   void insertAfter(T *e, T *t) {
      t->__link__(e, e->forw);
      length++;
   }

/*
 * Splices two lists together, whose list heads are given by this and l.
 * The resulting head is this, and l is returned as the empty list.
 */
   void splice(List& l) {
      if (!l.isEmpty()) {
	 l.forw->back = back;
	 back->forw = l.forw;
	 l.back->forw = this;
	 back = l.back;
	
	 length += l.length;

	 // destroy l
	 l.forw = l.back = &l;
	 l.length = 0;
      }
   }

   // traversal routines
   T *head() const {
      return (forw == (const ListNode *) this) ? (T *) NULL : (T *) forw;
   }

   T *tail() const {
      return (back == (const ListNode *) this) ? (T *) NULL : (T *) back;
   }

   T *next(const T *n) const {
      return (n->forw == (const ListNode *) this) ? (T *) NULL : (T *) n->forw;
   }

   T *prev(const T *n) const {
      return (n->back == (const ListNode *) this) ? (T *) NULL : (T *) n->back;
   }
};

template <class T>
class DList : public List<T> {
};

// T must have the following methods
// T.operator ==
template <class T>
class EQList : public List<T> {
public:
   bool operator==(const EQList<T>& b) {
	/* 
	 *  check if two lists have all the same elements, perhaps in a different 
	 *  order - so nested loops
	 *  PROBLEM: if having equal elements on the list is allowed then these lists:
	 *      (A1 A2 A3) and (A4 B C), where A1==A2==A3==A4,
	 *  will be equal. 
	 */
      if (this->length != b.length)
	 return false;

      ListNode *e, *e2;
      for (e = this->forw; e != (ListNode*)this; e = e->forw) {
	 for (e2 = b.forw; e2 != (const ListNode*) &b; e2 = e2->forw)
	    if (*(T*)e == *(T*)e2)
	       break;
	 if (e2 == (const ListNode *) &b)
	    return false;
      }
      return true;
   }
};

// sorted doubly linked list
// T must have the following methods
// T.operator <
// T.operator ==
template <class T>
class SortedList : public EQList<T> {
public:
   void insert(T *t) {
      insertSorted(t);
   }

   void append(T *t) {
      insertSorted(t);
   }

   void prepend(T *t) {
      insertSorted(t);
   }

   void insertBefore(T *, T *t) {
      insertSorted(t);
   }

   void insertAfter(T *, T *t) {
      insertSorted(t);
   }
     
   void insertSorted(T *t) {
      ListNode *e;
      for (e = this->forw; e != (ListNode*)this && (*(T*)e < *t); e = e->forw)
	 ;

      if (e != (ListNode*)this)
	 t->__link__(e->back, e);
      else
	 t->__link__(this->back, this);

      this->length++;
   }

   bool insertSortedNoDups(T *t) {
      ListNode *e;
      for (e = this->forw; e != (ListNode*)this && (*(T*)e < *t); e = e->forw)
	 ;
      
      if (e != (ListNode*)this) {
	 if (!(*(T*)e == *t)) {
	    t->__link__(e->back, e);
	    this->length++;
	    return true;
	 }
      } else {
	 t->__link__(this->back, this);
	 this->length++;
	    return true;
      }
      return false;
   }

   // do a sorted merge
   void splice(SortedList& l) {
      ListNode *t;
      ListNode *e = this->forw;
      while (!l.isEmpty()) {
	 t = l.forw;
	 t->__unlink__();
	 for (; 
	      e != (ListNode*)this && (*(T*)e < *(T*)t);
	      e = e->forw)
	    ;
	 if (e != (ListNode*)this)
	    t->__link__(e->back, e);
	 else
	    t->__link__(this->back, this);
      }

      this->length += l.length;
   }

   // do a sorted merge
   void splice(List<T>& l) {
      ListNode *t;
      ListNode *e = this->forw;
      while (!l.isEmpty()) {
	 t = l.forw;
	 t->__unlink__();
	 insertSorted((T*)t);
      }
   }
   // sorted merge no dups
   void spliceNoDups(SortedList& l) {
      ListNode *t;
      ListNode *e = this->forw;
      while (!l.isEmpty()) {
   t = l.forw;
   t->__unlink__();
   for (;
        e != (ListNode*)this && (*(T*)e < *(T*)t);
        e = e->forw)
      ;
   if (e != (ListNode*)this) {
      if (!(*(T*)e == *(T*)t))
        t->__link__(e->back, e);
   }
   else
      t->__link__(this->back, this);
      }

      this->length += l.length;
   }
  
  

   T* find(const T &t) const {
      for (ListNode *e = this->forw; e != (const ListNode*)this; e = e->forw)
	 if (*(T*)e == t)
		 return (T*)e;
	 else if (!(*(T*)e < t))
		 return NULL;
      return NULL;
   }
   T *find(const T *t) const {
      return find(*t);
   }
};

#endif /* LIST_H */
