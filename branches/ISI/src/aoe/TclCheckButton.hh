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


#ifndef TCLCHECKBUTTON_HH
#define TCLCHECKBUTTON_HH

#include "TclObj.hh"

typedef enum { cbUnCheck = 0, cbCheck = 1 } tdCheckButtonOption;

class TclCheckButton : public TclObject
{
   private:
      char *pzcVariableName;
      tdCheckButtonOption eOption;

   public:
      TclCheckButton(char *pzcName, tdCheckButtonOption eOption = cbUnCheck) 
	 : TclObject(pzcName), eOption(eOption), pzcVariableName(0) {}
      ~TclCheckButton(void);
      virtual int setExtraArgument(void *pvExtraArgument)
         { 
	 // Setup extra argument first
	 TclObject::setExtraArgument(pvExtraArgument);
	 // Then setup check button specific option
	 return setOption(eOption); 
	 }
      char *getVariableName(void) { return pzcVariableName; }
      int setOption(tdCheckButtonOption eOption);
      int getOption(void);
//      int getOption(void) { return eOption; }
      int enable(void);
      int disable(void);
};

#endif // TCLCHECKBUTTON_HH
