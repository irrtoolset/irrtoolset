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


#if !defined(IDENT_OFF)
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7) 
static char ident[] = 
   "@(#)$Id$";
#endif
#endif


#include "config.h"
#include <cstring>
#include <cstdlib>
#include "TclVar.hh"
#include "TclApp.hh"


SplitName::SplitName(char *pzcName) :
  pzcName1(NULL),
  pzcName2(NULL)
{
  char pzcBuffer[256], *p;
  strcpy(pzcBuffer, pzcName);
  if (p = strchr(pzcBuffer, '('))
    {
    *p = 0;
    pzcName1 = new char [strlen(pzcBuffer) + 1];
    strcpy(pzcName1, pzcBuffer);
    //    pzcName1 = strdup(pzcBuffer);
    *p = '(';
    pzcName2 = new char [strlen(p + 1) + 1];
    strcpy(pzcName2, p + 1);
    //    pzcName2 = strdup(p + 1);
    pzcName2[strlen(pzcName2) - 1] = 0;
    }
  else
    {
    pzcName1 = new char [strlen(pzcBuffer) + 1];
    strcpy(pzcName1, pzcBuffer);
    //    pzcName1 = strdup(pzcBuffer);
    } 
}

SplitName::~SplitName(void)
{
  if (pzcName1) delete []pzcName1;
  if (pzcName2) delete []pzcName2;
}



TclVariable::TclVariable(char *pzcName) :
  TclObject(pzcName),
  pzcName1(NULL),
  pzcName2(NULL)
{
}

TclVariable::~TclVariable(void)
{
  if (pzcName1) free(pzcName1);
  if (pzcName2) free(pzcName2);
}

int TclVariable::setExtraArgument(void *pvExtraArgument)
{ 
  TclApplication *pcApp = (TclApplication *)pvExtraArgument;
  SplitName cName(getName());
  if (cName.name1()) pzcName1 = strdup(cName.name1());
  if (cName.name2()) pzcName2 = strdup(cName.name2());
  return TclObject::setExtraArgument(pvExtraArgument);
}

void TclVariable::setValue(char *pzcValue)
{
  TclApplication *pcApp = (TclApplication *)getExtraArgument();
  Tcl_SetVar2(pcApp->getInterp(), pzcName1, pzcName2, pzcValue, 0); 
}

char *TclVariable::getValue(char *pzcDummy = NULL)
{
  TclApplication *pcApp = (TclApplication *)getExtraArgument();
  return Tcl_GetVar2(pcApp->getInterp(), pzcName1, pzcName2, 0);
}



void TclInt::setValue(int iValue)
{
  char pzcBuffer[64];
  sprintf(pzcBuffer, "%d", iValue);
  TclVariable::setValue(pzcBuffer);
}

int TclInt::getValue(void)
{
  char *pzcBuffer = TclVariable::getValue();
  return atoi(pzcBuffer);
}
