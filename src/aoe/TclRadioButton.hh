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


#ifndef TCLRADIOBUTTON_HH
#define TCLRADIOBUTTON_HH

#include "TclObj.hh"

class TclRadioButton : public TclObject
{
   private:
      char *pzcVariableName;
      int iOption;        // Should start from 0 and 
                          // explicitly specify in tk script

   public:
      TclRadioButton(char *pzcName, char *pzcVariableName, int iOption = 0) 
	 : TclObject(pzcName), pzcVariableName(pzcVariableName),
           iOption(iOption) {}
      virtual int setExtraArgument(void *pvExtraArgument)
         { 
	 // Setup extra argument first
	 TclObject::setExtraArgument(pvExtraArgument);
	 return setOption(iOption);
	 }
      int setOption(int iOption);
      int getOption(void);
      int add(char *pzcLabel, char *pzcCommand);
      int del(int iWhichOne);
      int getSize(void);
};

#endif // TCLRADIOBUTTON_HH
