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


#ifndef TCLCMD_HH
#define TCLCMD_HH

#include <tk.h>
#include "TclObj.hh"

class TclCommand : public TclObject 
{
   private:
      static int command(ClientData ptData, Tcl_Interp *ptInterp,
			 int argc, char *argv[]);
      static void cleanUp(ClientData ptData);

   protected:
      virtual int command(int argc, char *argv[]) = 0;
      virtual void cleanUp(void) { delete this; }

   public:
      TclCommand(char *pzcName) : TclObject(pzcName) {}
      TclCommand(char *pzcName, void *pvExtraArgument) 
	 : TclObject(pzcName, pvExtraArgument) {}
      // Never keep track of this object's pointer,
      // so, there is no way to destroy it,
      // hence, destructor has never been called.
      // Instead, cleanUp() function is used to destroy this object itself.
      // ~TclCommand(void) { delete this; }
};

#endif // TCLCMD_HH