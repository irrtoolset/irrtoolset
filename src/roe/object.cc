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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include "config.h"
#include "object.hh"
#include "roe_tcl.hh"
#include "roe.hh"
#include "roe_config.hh"
#include "route-list.hh"
#include "irr/irr.hh"
#include "irr/route.hh"  // for class Route

// Added by wlee@isi.edu
extern IRR *whois;

void RoeObject::clear() {
   tcl_Eval(".object.text configure -state normal");
   tcl_Eval(".object.text delete 1.0 end");
   tcl_Eval(".object.text configure -state disabled");
}

void RoeObject::display(char *dbase, ASt as, char *rt) {
   Route *route = NULL;

   tcl_Eval(".object.text configure -state normal");
   tcl_Eval(".object.text delete 1.0 end");
   //   tcl_Eval(".object.buttons.add configure -state normal");
   tcl_Eval(".object.buttons.delete configure -state normal");
   tcl_Eval(".object.buttons.update configure -state normal");
   tcl_Eval(".object.buttons.schedule configure -state normal");

   // Modified by wlee@isi.edu
   //   whois->QueryKillResponse("!ufo=1");
   //   whois->QueryKillResponse("!uF=0");
   whois->setFullObjectFlag(true);
   whois->setFastResponseFlag(false);

   whois->SetSources(dbase);
   // Modified by wlee@isi.edu
   //   int code = whois->QueryResponse(buffer, "!mrt,%s-AS%d", rt, as);
   whois->getRoute(route, rt, as);

   whois->SetSources();

   // Modified by wlee@isi.edu
   //   if (code)
   if (route) {
      tcl_Eval(".object.text insert 1.0 {%.*s}", route->size, route->contents);
      delete route;
   }

   tcl_Eval(".object.text configure -state disabled");
}

void RoeObject::del(int index) {
   char buffer[64];
   RouteList::Route *vr = routelist(index);

   // Modified by wlee@isi.edu
   tcl_Eval("set AS AS%d", routelist.as);
   //   tcl_Eval("set AS %s", routelist.as);
   tcl_Eval("set ROUTE %s", vr->route.get_text(buffer));
   tcl_Eval("set DATE %s", configure.date);

   tcl_Eval(".object.text configure -state normal");
   tcl_delete_matching_lines(".object.text", "^delete:");
   tcl_Eval(".object.text insert end \"%s\"", configure.del_template);
   tcl_Eval("focus .object.text");
}

void RoeObject::add(int index) {
   char buffer[64];
   RouteList::Route *vr = routelist(index);

   // Modified by wlee@isi.edu
   tcl_Eval("set AS AS%d", routelist.as);
   //   tcl_Eval("set AS %s", routelist.as);
   tcl_Eval("set ROUTE %s", vr->route.get_text(buffer));
   tcl_Eval("set DATE %s", configure.date);

   tcl_Eval(".object.text configure -state normal");
   tcl_Eval(".object.text delete 1.0 end");
   tcl_Eval(".object.text insert end \"%s\"", configure.add_template);
   tcl_Eval("focus .object.text");
   tcl_Eval(".object.buttons.schedule configure -state normal");
}

void RoeObject::upd(int index) {
   char buffer[64];
   RouteList::Route *vr = routelist(index);

   // Modified by wlee@isi.edu
   tcl_Eval("set AS AS%d", routelist.as);
   //   tcl_Eval("set AS %s", routelist.as);
   tcl_Eval("set ROUTE %s", vr->route.get_text(buffer));
   tcl_Eval("set DATE %s", configure.date);

   tcl_Eval(".object.text configure -state normal");
   tcl_delete_matching_lines(".object.text", "^changed:");
   tcl_Eval(".object.text insert end \"%s\"", configure.upd_template);
   tcl_Eval("focus .object.text");
}
