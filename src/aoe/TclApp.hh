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
