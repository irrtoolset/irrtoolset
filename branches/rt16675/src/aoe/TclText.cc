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


#if !defined(IDENT_OFF)
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7) 
static char ident[] = 
   "@(#)$Id$";
#endif
#endif


#include "config.h"
#include <cstring>
#include <cstdlib>
#include "TclText.hh"
#include "TclApp.hh"

int TclText::pushState(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pcApp->evalf("%s cget -state", getName())) return 0;
   // Keep the current state
   strcpy(pzcState, pcApp->getResult());
   return 1;
}

// Note, no error checking here, so, should use it very carefully --
//   must pushState() fisrt before popState()
int TclText::popState(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   return (pcApp->evalf("%s configure -state %s", getName(), pzcState));
}

int TclText::clear(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pushState()) return 0;
   // Enable the text widget
   if (!enable()) return 0;
   // Clear it
   if(!pcApp->evalf("%s delete 1.0 end", getName())) return 0;
   return popState();
}

// Some hacks to deal with huge data
int TclText::insert(const char *pzcLine)
{
  TclApplication *pcApp = (TclApplication *)getExtraArgument();
  int iResult = 0;
  int iLineLen, iNameLen;

  // If the strlen is 0, done!
  if ((iLineLen = strlen(pzcLine)) == 0) return 1;
  iNameLen = strlen(getName());
  if (!pushState()) return 0;
  if (!enable()) return 0;
  if (iLineLen < dBufferSize - iNameLen - 16)
    iResult = pcApp->evalf("%s insert end {%s}", getName(), pzcLine);
  else 
    {
    char *pzcBuffer = new char [iLineLen + iNameLen + 16];
    if (pzcBuffer)
      {
      strcpy(pzcBuffer, getName());
      strcat(pzcBuffer, " insert end {");
      strcat(pzcBuffer, pzcLine);
      strcat(pzcBuffer, "}");
      iResult = pcApp->eval(pzcBuffer);
      delete []pzcBuffer;
      }
    }
  if (!popState()) return 0;
  return iResult;
}

int TclText::insert(const char *pzcLine, int iLen)
{
  TclApplication *pcApp = (TclApplication *)getExtraArgument();
  int iResult = 0;
  int iNameLen = strlen(getName());
  char *pzcBuffer = new char [iLen + iNameLen + 16];
  if (!pushState()) return 0;
  if (!enable()) return 0;
  if (pzcBuffer)
    {
    strcpy(pzcBuffer, getName());
    strcat(pzcBuffer, " insert end {");
    int iBufferLen = strlen(pzcBuffer);
    strncpy(pzcBuffer + iBufferLen, pzcLine, iLen);
    pzcBuffer[iBufferLen + iLen] = 0;
    strcat(pzcBuffer, "}");
    iResult = pcApp->eval(pzcBuffer);
    delete []pzcBuffer;
    }
  if (!popState()) return 0;
  return iResult; 
}

int TclText::removeLastLine(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pushState()) return 0;
   if (!enable()) return 0;   
   int iResult = pcApp->evalf(
      "%s delete \"insert linestart\" \"insert lineend +1 char\"", getName()); 
   if (!popState()) return 0;
   return iResult;  
}

int TclText::removeLastBlankLines(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pushState()) return 0;
   if (!enable()) return 0;   
   while (pcApp->evalf("%s search -back -regexp ^$ end", getName()))
     {
     const char *pzcResult = pcApp->getResult();
     if (!*pzcResult || !strcmp(pzcResult, "1.0")) break;
     if (!pcApp->evalf("%s delete %s", getName(), pzcResult)) break;
     }
   if (!popState()) return 0;
   return 1;  
}

int TclText::removeSelectedText(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pushState()) return 0;
   if (!enable()) return 0;
   // No matter success or not!
   pcApp->evalf("%s delete sel.first sel.last", getName());
   return popState();
}

int TclText::enable(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   return pcApp->evalf("%s configure -state normal", getName()); 
}

int TclText::disable(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   return pcApp->evalf("%s configure -state disabled", getName()); 
}

int TclText::addLineTag(char *pzcTagName, char *pzcFrom, char *pzcTo)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   for (int i = 0; i < getTotalLine(); i++)
     if (!pcApp->evalf("%s tag add %s %d.%s %d.%s", 
		       getName(), pzcTagName, i + 1, pzcFrom, i + 1, pzcTo)) 
       return 0;
   return 1;
}

int TclText::addLineTag(int iLine, 
			char *pzcTagName, char *pzcFrom, char *pzcTo)
{
  TclApplication *pcApp = (TclApplication *)getExtraArgument();
  iLine++;
  if (!pcApp->evalf("%s tag add %s %d.%s %d.%s", 
		    getName(), pzcTagName, iLine, pzcFrom, iLine, pzcTo)) 
    return 0;
  return 1;  
}

int TclText::configureTag(char *pzcTagName, char *pzcConfigure)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pcApp->evalf("%s tag configure %s %s", 
		     getName(), pzcTagName, pzcConfigure)) 
     return 0;
   return 1;
}

const char *TclText::getAll(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pcApp->evalf("%s get 1.0 end", getName())) return 0;
   return pcApp->getResult();
}

int TclText::focus(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pcApp->evalf("focus %s", getName())) return 0;
   return 1;
}

int TclText::unfocus(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pcApp->eval("focus .")) return 0;
   return 1;
}

int TclText::getTotalLine(void)
{
  TclApplication *pcApp = (TclApplication *)getExtraArgument();
  if (!pcApp->evalf("%s index end", getName())) return 0;
  int iLen = atoi(pcApp->getResult()) - 1; 
  if (iLen <= 0) return 0;
  return iLen;
}

const char *TclText::getLine(int iLine)
{
  TclApplication *pcApp = (TclApplication *)getExtraArgument();
  iLine++;
  if (!pcApp->evalf("%s get %d.0 %d.end", getName(), iLine, iLine)) 
    return NULL;  
  //  printf("**%s**\n", pcApp->getResult());
  return pcApp->getResult();
}
