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


#ifndef TCLTOGGLEBUTTON_HH
#define TCLTOGGLEBUTTON_HH

#include "TclCheckButton.hh"
#include <string.h>
#include <stdlib.h>

class TclToggleButton : public TclCheckButton
{
  private:
    char *pzcUnCheckString, *pzcCheckString;

  private:
    int label(char *pzcLabel);
    static char *traceProc(ClientData pvData, Tcl_Interp *pcInterp, 
			   char *pzcName1, char *pzcName2, int iFlags);

  protected:
    char *traceProc(void);

  public:
    TclToggleButton(char *pzcName, char *pzcUnCheckString, 
		    char *pzcCheckString, 
		    tdCheckButtonOption eOption = cbUnCheck);
    ~TclToggleButton(void);
    virtual int setExtraArgument(void *pvExtraArgument);
    void labelUnCheckString(char *pzcString) 
       { 
       free(pzcUnCheckString);
       pzcUnCheckString = strdup(pzcString); 
       traceProc();
       }
    void labelCheckString(char *pzcString) 
       { 
       free(pzcCheckString);
       pzcCheckString = strdup(pzcString); 
       traceProc();
       }
    void labelBothString(char *pzcString)
       {
       free(pzcUnCheckString);
       free(pzcCheckString);
       pzcUnCheckString = strdup(pzcString);
       pzcCheckString = strdup(pzcString);
       traceProc();
       }
    void toggle(void);
};

#endif // TCLTOGGLEBUTTON_HH
