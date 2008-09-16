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
