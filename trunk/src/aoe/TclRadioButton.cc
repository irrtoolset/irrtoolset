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
#include <cstring>
#include <cstdlib>
#include "TclRadioButton.hh"
#include "TclApp.hh"

int TclRadioButton::setOption(int iOption)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pcApp) return 0;
/*
   if (!pzcVariableName)
      if ((pzcVariableName = new char [strlen(getName()) + 4]) == NULL) 
	 return 0;
   strcpy(pzcVariableName, getName());
   char *p = pzcVariableName;
   while ((p = strchr(p, '.')) != NULL) strcpy(p, p + 1);
   strcat(pzcVariableName, "Var");
*/
   // Create a Tcl global variable whose name is stored in 'pzcVariableName' 
   // with value 'iOption'.
   if (!pcApp->setVar(pzcVariableName, iOption)) return 0;
/*
   // Unlike TclCheckButton, no configure command here!!!
   if (!pcApp->evalf("%s configure -variable %s", getName(), pzcVariableName)) 
      return 0;
*/
   return 1;   
}

// This function is really a special case
// return of -1 indicates of error
int TclRadioButton::getOption(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pcApp) return -1;
   char *pzcBuffer;
   if ((pzcBuffer = pcApp->getVar(pzcVariableName)) == NULL) return -1;
   return atoi(pzcBuffer);   
}

int TclRadioButton::add(char *pzcLabel, char *pzcCommand)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pcApp->evalf("%s index last", getName())) return 0;
   int iValue = atoi(pcApp->getResult());
   return pcApp->evalf("%s add radio -label {%s} -variable %s "
		       "-command {%s} -value %d",
		       getName(), pzcLabel, pzcVariableName, pzcCommand, 
		       iValue);
}

// Start from 0
int TclRadioButton::del(int iWhichOne)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   return pcApp->evalf("%s delete %d", getName(), iWhichOne);   
}

int TclRadioButton::getSize(void)
{
   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   if (!pcApp->evalf("%s index last", getName())) return -1;
   return atoi(pcApp->getResult());
}
