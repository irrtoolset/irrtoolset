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
//  Author(s): WeeSan Lee <wlee@ISI.EDU>


#if !defined(IDENT_OFF)
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7) 
static char ident[] = 
   "@(#)$Id$";
#endif
#endif


#include "config.h"
#include <tk.h>
#include "TclToggleButton.hh"
#include "TclApp.hh"

TclToggleButton::TclToggleButton(char *pzcName, char *pzcUnCheckString, 
				 char *pzcCheckString, 
				 tdCheckButtonOption eOption = cbUnCheck) :
  TclCheckButton(pzcName, eOption)
{
  this->pzcUnCheckString = strdup(pzcUnCheckString);
  this->pzcCheckString = strdup(pzcCheckString);
}

TclToggleButton::~TclToggleButton(void)
{  
  TclApplication *pcApp = (TclApplication *)getExtraArgument();
  Tcl_UntraceVar(pcApp->getInterp(), getVariableName(),
		 TCL_GLOBAL_ONLY | TCL_TRACE_WRITES,
		 traceProc, (ClientData)this);
  free(pzcUnCheckString);
  free(pzcCheckString);
}

int TclToggleButton::setExtraArgument(void *pvExtraArgument)
{
  if (!TclCheckButton::setExtraArgument(pvExtraArgument)) return 0;
  traceProc(); // Bootstrap
  // Install a proc to keep track of its variable 
  TclApplication *pcApp = (TclApplication *)pvExtraArgument;
  if (Tcl_TraceVar(pcApp->getInterp(), getVariableName(), 
		   TCL_GLOBAL_ONLY | TCL_TRACE_WRITES,
		   traceProc, (ClientData)this) != TCL_OK) return 0;
  return 1;
}

int TclToggleButton::label(char *pzcLabel)
{
  TclApplication *pcApp = (TclApplication *)getExtraArgument();
  return pcApp->evalf("%s configure -text {%s}", getName(), pzcLabel);
}

// Static member function
char *TclToggleButton::traceProc(ClientData pvData, Tcl_Interp *pcInterp, 
				 char *pzcName1, char *pzcName2, int iFlags)
{
  TclToggleButton *pcToggleButton = (TclToggleButton *)pvData;
  return pcToggleButton->traceProc();
}

char *TclToggleButton::traceProc(void)
{
  int iResult;
  if (getOption() == cbUnCheck)
    iResult = label(pzcUnCheckString);
  else
    iResult = label(pzcCheckString);
  if (iResult) return NULL;
  return "Error occurred when updating toggle button's label!";
}

void TclToggleButton::toggle(void)
{
   if (getOption())
      setOption(cbUnCheck);
   else
      setOption(cbCheck);
}
