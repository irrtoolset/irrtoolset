/*
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
//  $Id$
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
//  Author(s): eddy@isi.edu
*/


//
// The purpose of this class is to handle errors as they occur
// down a call stack, allowing the called functions to provide diag.
// info to be used by a calling function.  syserrs are handled as
// well.
//
// This error class is intended to work as follows:  initaly error level
// set to 0 indicating NO error.  When an error occurs a printf style message
// is added to a message list and the error level is set to a non-zero
// int (the higher the level, the more severe. Existing error levels are:
//
//   None          =  0 
//   System error  = -1
//   Library error = -2
//   Info          = -3
//   Warning       = -4
//   Fatal         = -5
//
// As the error returns
// back up the call stack a chain of messages may exist.  a calling
// function (typically an application), can then decide how to handle
// message, for example, print the messages and exit the program (fatal error)
// or print the messages and reset the error, proceeding on (warning).
//
// Note: all parameters specified as (msg ...) imply printf style arguments
//
// error.info (msg ...)
// error.warning (msg ...) 
// error.fatal (msg ...)
// 
// 	set the level accordingly, and add the printf sytle msg
// 	to the message list. 
//
// error(msg ...)
// error.error(msg ...)
//
// 	adds the optional msg to the message list and
//	returns the error level.  
//
// error.print()
// error.print(msg ...)
//
// 	prints the messages according to the order flag, defaulting
// 	to lifo. with optional msg.  there is an optional "separator"
//	that may be printed between each message, the default seperator
//	is a single space.  
//
// error.reset()
//
// 	clears errorlevel and cleans the message list
//
// error.Die(msg ...)
//
//	causes the error to print() and exit().  'dontExit' can be
//	set causing Die() _not_ to call exit().  this is mainly for
//	backward compatablility.
//
// error.setOutput (char *filename)
// error.setOutput (FILE *stream)
//
//	set the output stream for error messages.  default to stderr.
//	example:
//
//		error.setOutput("/var/log/logfile");
//	or
//		error.setOutput(stdout);
//
//
// error.separator(char *)
//
//      provide a separator to be printed between messages.  the separator
//	is applied _before_ the current message.  example: one may
//	find setting the separator to ": " to be convienient providing
//	the following functionality.
//
//		error.separator(": "); 
//		int f() {
//		  ...
//		  return (error.warning("error in f()"));
//		}
//
//	        if (f() == Warning) {
//		   error.print (progname);
//		   error.reset();
//		}
//
//	will produce the following output:
//
//	    progname: error in f()
//
//
// error = f();
//
//	The '=' operator will set the level and return that level.
//
//
// A semantic alternative usage of the Error, class.  Error can also be
// used as follows (with no impact upon the previous semantics).  
//
//
// error.warn(msg ...)
//
//	iff level is set, print() and reset().
//
// error.die(msg ...)
//
//	iff level is set, print() and exit().
//
// NOTE: these produce semantics similar to those found in perl (hence)
// the names.  e.g in perl on can do:
//
//	func() || die "problem in func()";
//
// with these member funcs and the '= (int)' operator one can do the
// following:
//
//	error = func();
//	error.die("problem in func()");
//
// Flags:
//
//  syserr_:		handle syserr: yes
//  immediate_:		handle errs immediatly: no
//  ignore_:		ignore these calls: no
//  order_;		print order: reverse
//  dontExit_;		for the Die() function: false
//
// each of the above flags have a corresponding members functions
// to set the flag.
//
// error.dontExit (boolean)
//
//	sets the corresponding flag.
//
// error.immediate(boolean)
//
//	sets a flag that causes the error to print() and reset() when
//	immediatly when encountered.  this will result in no error level
//	or message list upon return.  immediate defaults to off.
//
// error.order(order)
//
//      sets the order flag. can be: Reverse or InOrder.
//
// error.syserr(boolean);
// error.ignore(boolean);
//



#ifndef __ERROR_HH__
#define __ERROR_HH__

#include "config.h"

#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

extern "C" {
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif

#include <assert.h>   
}
#include <cerrno>
#include <cstring>
#include <iostream>
#include <iomanip>

static const int BufferSize = 8192;

static const int No  = 0;
static const int Yes = 1;

static const int InOrder = 0;
static const int Reverse = 1;

const int ErrorNone    =  0;
const int ErrorSystem  = -1;
const int ErrorLibrary = -2;
const int ErrorInfo    = -3;
const int ErrorWarning = -4;
const int ErrorFatal   = -5;

static FILE *errorlog = stderr;

//
// This is a message within a dll, a single string with pointers to
// the previous and next neighbors.  perhaps i should really us class
// with templates...
// 

struct Message {
    char *string_;		// the string message itself
    const char *separator_;	// an optional separator
    int len_;			// len of string_
    Message *next_;		// make this a dbl llist because we may
    Message *prev_;		// print in either order (default reverse).

    Message () {
	string_ = NULL;
	len_ = 0;
	next_ = prev_ = NULL;
	separator_ = " ";
    };
    Message (const char *str, const char* sp = " ") {
	next_ = prev_ = NULL;
	createString(str);
	separator_ = sp;
    }
    ~Message () {
	if (string_)
	    delete []string_;
    }
    void createString (const char *str) { 
	len_ = strlen (str);
	string_ = new char[len_+1];
	strncpy (string_, str, len_);
	string_[len_] = '\0';
    }
    
    void print() {
	fprintf (errorlog, "%s", string_);
	if (prev_ != NULL && separator_)
	    fprintf (errorlog, "%s", separator_);
    }
    
  //    void operator() (char &str) { createString ((char *) &str); } 
  //    void operator() (char *str) { createString ((char *) &str); }
};

class Error;
class Messages {
    Message *head_;
    Message *tail_;
    char buffer_[BufferSize];
    char *separator_;

  public:
    Messages () {
	head_ = tail_ = NULL;
	buffer_[0] = '\0';
	setSeparator(" ");
    }

    ~Messages () {
	clear();
    }

    void setSeparator (const char *sep) {
	separator_ = new char [strlen (sep) + 1];
	strcpy (separator_, sep); 
    }

    void clear () {
	while (head_) {
	    Message *msg = head_->next_;
	    delete head_;
	    head_ = msg;
	}
	head_ = tail_ = NULL;
	buffer_[0] = '\0';
    }

    void operator () (const char *str) {
	Message *msg = new Message(str, separator_);
	if (head_ == NULL) {
	    head_ = tail_ = msg;
	} else {
	    assert (tail_);
	    tail_->next_ = msg;
	    msg->prev_ = tail_;
	    tail_ = msg;
	}
    }
    
    void operator () (const char *fmt, va_list ap) {
	vsprintf (buffer_, fmt, ap);
	
	Message *msg = new Message(buffer_, separator_);
	msg->separator_ = separator_;
	if (head_ == NULL) {
	    head_ = tail_ = msg;
	} else {
	    assert (tail_);
	    tail_->next_ = msg;
	    msg->prev_ = tail_;
	    tail_ = msg;
	}
    }

    void print (int order = Reverse) {
	Message *msg ;
	if (order == Reverse) {
	    msg = tail_;
	    while (msg) {
		msg->print();
		msg = msg->prev_;
	    }
	} else {
	    msg = head_;
	    while (msg) {
		msg->print();
		msg = msg->next_;
	    }
	}
    }
};

class Error {
    int level_;			// message level
    Messages messages_;		// the message stack

				// flags: default
    int syserr_;		// handle syserr:  no
    int immediate_;		// handle errs immediatly: no
    int ignore_;		// ignore these calls: no
    int order_;			// print order: reverse
    int dontExit_;		// for the Die() function.

    void addmsg (const char *fmt, va_list ap) {
	if (ignore_)
	    return;
	if (errno && syserr_) {
	    messages_(strerror(errno));
	    errno = 0;
	}
	messages_(fmt, ap);
	if (immediate_) {
	    print();
	    reset();
	}
    }

  public:
    Error() {
	syserr_ = No;
	immediate_ = No;
	ignore_ = No;
	order_ = Reverse;
	dontExit_ = 0;
	reset();
    }

    void reset() {
	errno = 0;
	level_ = ErrorNone;
	messages_.clear();
    }

    void syserr (int val)	{ syserr_ = val; }
    void immediate  (int val)	{ immediate_ = val; }
    void ignore (int val)	{ ignore_ = val; }			
    void order (int val)	{ order_ = val; }
    void dontExit (int val)     { dontExit_ = val; }

    int info (const char *fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	if (level_ > ErrorInfo)
	    level_ = ErrorInfo;
	addmsg (fmt, ap);
	va_end (ap);
	return level_;
    }

    int warning (const char *fmt,  ...) {
	va_list ap;
	va_start (ap, fmt);
	if (level_ > ErrorWarning)
	    level_ = ErrorWarning;
	addmsg(fmt, ap);
	va_end (ap);
	return level_;
    }

    int fatal (const char *fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	if (level_ > ErrorFatal)
	    level_ = ErrorFatal;
	addmsg (fmt, ap);
	va_end (ap);
	return level_;
    }

    int Die (const char *fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	addmsg (fmt, ap);
	va_end (ap);
	print ();
	if (!dontExit_)
	    exit (1);
	reset();
	return level_;
    }


    // these following two classes die() and warn() offer a
    // slightly different semantic, both of these only add msgs
    // iff level_ is set.  die() will also exit if level_ is
    // set.  this provides conditional error processing for
    // example, assume function f() returns 0 on success
    // anything else upon error.
    int warn (const char *fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	if (level_ == 0) {
	   va_end (ap);
	   return level_;
	}
	level_ = ErrorFatal;
	addmsg (fmt, ap);
	va_end (ap);
	print ();
	reset();
	return level_;
    }
    
    int die (const char *fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	if (level_ == 0) {
	   va_end (ap);
	   return level_;
	}
	addmsg (fmt, ap);
	va_end (ap);
	print ();
	exit (1);
    }

    
    int error(const char *fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	if (level_ > ErrorInfo)
	    level_ = ErrorInfo;
	addmsg(fmt, ap);
	va_end (ap);
	return level_;
    }

    void print() {
	fflush(stdout);
	messages_.print (order_);
	fprintf(errorlog, "\n");
	fflush (errorlog);
    }

    void print(const char *fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	addmsg(fmt, ap);
	va_end (ap);
	print();
    }

    // The following two functions allow the errorlog stream
    // to be set.  stderr is used by default.  return 0 on
    // success, non-zero otherwise
    int setErrorlog (const char *file) {
	errorlog = fopen (file, "w");
	if (errorlog == NULL) {
	    warning("could not open: %s for writing, using stderr",  file);
	    errorlog = stderr;
	}
	return level_;
    }

    int setErrorlog (FILE *stream) {
	if (stream == NULL) {
	    return (warning("empty stream"));
	}
	errorlog = stream;
	return level_;
    }

    // Operators
    int operator() () { return level_; }
    int operator() (const char *fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	if (level_ > ErrorInfo)
	    level_ = ErrorInfo;
	addmsg(fmt, ap);
	va_end (ap);
	return level_;
    }

    int operator= (int i) { return level_ = i; }
};

#endif // __ERROR_HH__
