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
#include <cctype>
#include <cstdarg>
#include <cstring>
#include "TclApp.hh"

// Static data members 
Tcl_Interp *TclApplication::ptInterp = NULL;


// Constructor
TclApplication::TclApplication(char *pzcAppName, char *pzcDisplay, 
			       char *pzcDefaultResource = NULL, 
			       char *pzcResourceFile = NULL) 
   : ptMainWindow(NULL),
     pzcDisplay(pzcDisplay),
     pzcDefaultResource(pzcDefaultResource),
     pzcResourceFile(pzcResourceFile)
{
   strcpy(this->pzcAppName, pzcAppName);
   strcpy(pzcClassName, pzcAppName);
   pzcClassName[0] = toupper(pzcClassName[0]);
}

// Destructor
TclApplication::~TclApplication(void)
{
   if (ptInterp) Tcl_DeleteInterp(ptInterp);
}

// Since init() is a protected virtual function, users could append more stuff
// instead of default one
int TclApplication::init(void)
{
   if ((ptInterp = Tcl_CreateInterp()) == NULL) return 0;

#ifdef HAVE_TK_CREATEMAINWINDOW
   if ((ptMainWindow = Tk_CreateMainWindow(ptInterp, pzcDisplay, 
				      pzcAppName, pzcClassName)) == NULL)
      return 0;
#else
   if (pzcDisplay)
      Tcl_SetVar2(ptInterp, "env", "DISPLAY", pzcDisplay, TCL_GLOBAL_ONLY);
#endif  // HAVE_TK_CREATEMAINWINDOW

   if (Tcl_Init(ptInterp) != TCL_OK) return 0;
   if (Tk_Init(ptInterp) != TCL_OK) return 0;

#ifndef HAVE_TK_CREATEMAINWINDOW
   if (!evalf("wm title . %s", pzcAppName)) return 0;
#endif  // HAVE_TK_CREATEMAINWIND

   // Use default resource first
   if (pzcDefaultResource)
     if (!eval(pzcDefaultResource)) return 0;

   // Then use user-defined resource
   // In this case, user-defined resource take precedence of default resource
   // at the same time, new version of resource can complement old 
   // user-defined resource file
   if (pzcResourceFile) 
     if (!evalf("catch {source %s}", pzcResourceFile)) return 0;

   return 1;
}

// Although run() is a public virtual function, basically users do not
// have to overide it.
// Be aware that, run() does all the initiations (Tcl_Init & Tk_Init)
// and call Tk_MainLoop(), there is no way you could add things after this.
// If you would like to do more things before 
// or after initiations, you should go to overide init() instead of run()
int TclApplication::run(void)
{
   if (!init()) return 0;
   Tk_MainLoop();

   // Code below is really copied from the source of Tk_MainLoop()
   // and modified
   //   while (Tk_GetNumMainWindows() > 0) 
   //      Tk_DoOneEvent(TK_ALL_EVENTS | TK_DONT_WAIT);

   return 1;
}

int TclApplication::eval(char *pzcCommand)
{
   if (Tcl_Eval(ptInterp, pzcCommand) != TCL_OK) 
      {
#ifdef DEBUG_TCLAPP
      fprintf(stderr, "TclApplication::eval(\"%s\") - %s\n", 
	      pzcCommand, getResult());
#endif // DEBUG_TCLAPP
      return 0;
      }
   return 1;
}

int TclApplication::evalf(char *pzcFormat, ...)
{
   char pzcBuffer[dBufferSize + 1];
   va_list ap;
   va_start(ap, pzcFormat);
   vsprintf(pzcBuffer, pzcFormat, ap);
   va_end(ap);

   if (Tcl_Eval(ptInterp, pzcBuffer) != TCL_OK) 
      {
#ifdef DEBUG_TCLAPP
      fprintf(stderr, "TclApplication::evalf(\"%s\") - %s\n", 
	      pzcBuffer, getResult());
#endif // DEBUG_TCLAPP
      return 0;
      }
   return 1;
}

char *TclApplication::setVar(char *pzcName, char *pzcValue, 
			   int iFlags = TCL_GLOBAL_ONLY)
{
   SplitName cName(pzcName);
   return Tcl_SetVar2(ptInterp, cName.name1(), cName.name2(), 
		      pzcValue, iFlags);
}

char *TclApplication::setVar(char *pzcName, int iValue, 
			     int iFlags = TCL_GLOBAL_ONLY)
{
   SplitName cName(pzcName);
   char pzcBuffer[64];
   sprintf(pzcBuffer, "%d", iValue);
   return Tcl_SetVar2(ptInterp, cName.name1(), cName.name2(), 
		      pzcBuffer, iFlags);
}

char *TclApplication::getVar(char *pzcName, int iFlags = TCL_GLOBAL_ONLY)
{
   SplitName cName(pzcName);
   return Tcl_GetVar2(ptInterp, cName.name1(), cName.name2(), iFlags);
}

int TclApplication::createCommand(TclCommand *pcCommand)
{
   if (!ptInterp) return 0;

   // Pass the pointer of Tcl application as an arguement 
   pcCommand->setExtraArgument(this);

   // Call Tcl function to create a Tcl command
   if (Tcl_CreateCommand(ptInterp, 
			 pcCommand->getName(), 
			 TclCommand::command,
			 (ClientData)pcCommand,
			 TclCommand::cleanUp) == NULL) return 0;
   return 1;
}

// So far, just setup the extra argument for each object
// In the future, this function should do more stuff than now 
int TclApplication::insert(TclObject *pcObject)
{
   if (!ptInterp) return 0;
   return (pcObject->setExtraArgument(this));
}
