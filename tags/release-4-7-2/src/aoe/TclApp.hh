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


#ifndef TCLAPP_HH
#define TCLAPP_HH

#include <tk.h>
#include "TclObj.hh"
#include "TclCmd.hh"
#include "TclVar.hh"

#define dBufferSize  (1024 * 16)   // 16K

// Some convenient macros defined here
#define TCL(x)  {if (x) return TCL_ERROR;}
#define EVAL(x)  {if (x) return 0;}


class TclApplication
{
   private:
      static Tcl_Interp *ptInterp;
      Tk_Window ptMainWindow;
      char pzcAppName[64], pzcClassName[64], *pzcDisplay;
      char *pzcDefaultResource, *pzcResourceFile;

   protected:
      virtual int init(void);

   public: 
      TclApplication(char *pzcAppName, char *pzcDisplay, 
		     char *pzcDefaultResouce = NULL, 
		     char *pzcResourceFile = NULL);
      virtual ~TclApplication(void);
      virtual int run(void);
      static int eval(char *pzcCommand);
      static int evalf(char *pzcFormat, ...);
      char *getAppName(void) { return pzcAppName; }
      Tcl_Interp *getInterp(void) { return ptInterp; }
      char *getResourceFile(void) { return pzcResourceFile; }
      void setResult(char *pzcResult) { ptInterp->result = pzcResult; }
      const char *getResult(void) { return ptInterp->result; }
      char *setVar(char *pzcName, char *pzcValue, 
		   int iFlags = TCL_GLOBAL_ONLY);
      char *setVar(char *pzcName, int iValue, int iFlags = TCL_GLOBAL_ONLY);
      char *getVar(char *pzcName, int iFlags = TCL_GLOBAL_ONLY);
      int createCommand(TclCommand *pcCommand);
      int insert(TclObject *pcObject);
};

#endif  // TCLAPP_HH
