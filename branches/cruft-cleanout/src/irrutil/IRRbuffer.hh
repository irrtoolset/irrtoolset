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
//  Author(s): WeeSan Lee <wlee@ISI.EDU>

#ifndef IRRBUFFER_HH
#define IRRBUFFER_HH

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cassert>

#define PROBE_BUFFER_SIZE 8191

/*
  This class is created to replace stl string class due to the limited
  size of buffer it can handle.

        |<------------------------ ulCapacity --------------------->|

                       |<-- length() -->|
        +-------------------------------------------------------------+
Buffer: |                               0                            0|
        +-------------------------------------------------------------+ 
         ^             ^                ^
         |             |                |
         |-------------+                |
         |          ulBegin             |
         |                              |
         |------------------------------+
         |
     pzcBuffer                        ulEnd
*/

class StringBuffer 
{
  private:
    char *pzcBuffer;              // Pointer points to a buffer
    unsigned long ulCapacity;     // Length of the buffer
    unsigned long ulBegin, ulEnd; // Offset from the beginning of the buffer

  public:
    StringBuffer(unsigned long ulCapacity = PROBE_BUFFER_SIZE) : 
      ulCapacity(ulCapacity), ulBegin(0), ulEnd(0) {
      pzcBuffer = new char [ulCapacity + 1];
      pzcBuffer[0] = 0;
      assert(pzcBuffer != NULL);
    }
    ~StringBuffer(void) {
      if (pzcBuffer) delete []pzcBuffer;
    }
    // Length of buffer
    unsigned long capacity(void) const {
      return ulCapacity;
    }
    // Length of the string stored
    unsigned long length(void) const {
      return ulEnd - ulBegin;
    }
    bool empty(void) {
      return ulBegin == ulEnd;
    }
    char *c_str(void) const {
      return pzcBuffer + ulBegin;
    }
    unsigned long find(char *pzcText) const {
      char *p = strstr(pzcBuffer + ulBegin, pzcText);
      if (p) return p - pzcBuffer - ulBegin;
      return length() + 1;
    }
    void clear(void) {
      ulBegin = ulEnd = 0;
      pzcBuffer[0] = 0;
    }
    //    char *remove(unsigned long ulIndex, unsigned long ulLength);
    // Remove characters from the right hand side of the string
    StringBuffer &remove(unsigned long ulLength) {
      if (ulLength >= length()) 
	clear();
      else 
	ulBegin += ulLength;
      return *this;
    }
    // Insert text into the buffer
    StringBuffer &append(char *pzcText, unsigned int uiLen);
    StringBuffer &append(char zcOneByte) {
      return append(&zcOneByte, 1);
    }
    StringBuffer &append(char *pzcText) {
      return append(pzcText, strlen(pzcText));
    }
    StringBuffer &append(const char *pzcFormat, ...) {
      char pzcText[1024]; // This will limit the use of the member function
      va_list ap;
      va_start(ap, pzcFormat);
      vsprintf(pzcText, pzcFormat, ap);
      va_end(ap);
      return append(pzcText, strlen(pzcText));
    }
    friend std::ostream &operator<<(std::ostream &os, const StringBuffer &b) {
      os << "[" << b.pzcBuffer + b.ulBegin << "]" << std::endl;
      return os;
    }
};


#endif // IRRBUFFER_HH
