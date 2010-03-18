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
//  Author(s): WeeSan Lee <wlee@ISI.EDU>

#include "buffer.hh"

StringBuffer &StringBuffer::append(char *pzcText, unsigned int uiLen)
{
  // Check if the buffer is big enuf to hold the whole thing
  if (ulEnd + uiLen > capacity())
    {
    // Calculate the new length
    unsigned long ulNewLen = length() + uiLen;
    // Check if the new text and old text would fit in the half of 
    // the same buffer after being reformed
    if ((length() + ulNewLen) <= (capacity() >> 1))
      {
      // Yes, reform the same old buffer
      strcpy(pzcBuffer, pzcBuffer + ulBegin);
      // Update some pointers
      ulEnd = length();  // Have to do this first before assigning ulBegin
      ulBegin = 0;
      }
    else  // No, allocate a new large buffer to replace the old one
      {
      // Calculate the new capacity
      unsigned long ulNewCapacity = capacity() + PROBE_BUFFER_SIZE;
      // If the new capacity is still too small, 
      // make use of the new length + PROBE_BUFFER_SIZE
      if (ulNewCapacity < ulNewLen) 
	ulNewCapacity = ulNewLen + PROBE_BUFFER_SIZE;
      // Update the old capacity
      ulCapacity = ulNewCapacity;
      // Allocate a new buffer
      char *pzcTempBuffer = new char [ulNewCapacity + 1];
      assert(pzcTempBuffer != NULL);
      // Very expensive copy
      // Could do strcpy because pzcBuffer + ulEnd is always NULL
      strcpy(pzcTempBuffer, pzcBuffer + ulBegin);
      // Update some pointers
      ulEnd = length();  // Have to do this first before assigning ulBegin
      ulBegin = 0;
      // Switch buffers
      delete []pzcBuffer;
      pzcBuffer = pzcTempBuffer;
      }
    }
  // Whew!  The buffer is big enuf 
  strncpy(pzcBuffer + ulEnd, pzcText, uiLen);
  ulEnd += uiLen;
  pzcBuffer[ulEnd] = 0;
  return *this;
}
