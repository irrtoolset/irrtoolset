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
//             John Mehringer <mehringe@isi.edu>

#include "bgp/actiondictionary.hh"

//------------------------------------------------------
//
//  ActionDictionary::ActionDictionary()
//    Sets up the a dictionary for actions to do 
//    comparisons to determine the type of action in 
//    parsing the action
//
//-------------------------------------------------------
  
  ActionDictionary::ActionDictionary() {
   dctn_rp_pref = (AttrRPAttr *) NULL;
   dctn_rp_pref_set = (AttrMethod *) NULL;
   dctn_rp_dpa = (AttrRPAttr *) NULL;
   dctn_rp_dpa_set = (AttrMethod *) NULL;
   dctn_rp_med = (AttrRPAttr *) NULL;
   dctn_rp_med_set = (AttrMethod *) NULL;
   dctn_rp_community = (AttrRPAttr *) NULL;
   dctn_rp_community_setop= (AttrMethod *) NULL;
   dctn_rp_community_appendop=(AttrMethod*)NULL;
   dctn_rp_community_append= (AttrMethod*) NULL;
   dctn_rp_aspath = (AttrRPAttr *) NULL;
   dctn_rp_aspath_prepend = (AttrMethod *) NULL;


   dctn_rp_pref      = schema.searchRPAttr("pref");
   dctn_rp_dpa       = schema.searchRPAttr("dpa");
   dctn_rp_med       = schema.searchRPAttr("med");
   dctn_rp_community = schema.searchRPAttr("community");
   dctn_rp_aspath    = schema.searchRPAttr("aspath");
   
   if (dctn_rp_pref)
      dctn_rp_pref_set         = dctn_rp_pref->searchMethod("=");
   if (dctn_rp_dpa)
      dctn_rp_dpa_set          = dctn_rp_dpa->searchMethod("=");
   if (dctn_rp_med)
      dctn_rp_med_set          = dctn_rp_med->searchMethod("=");
   if (dctn_rp_community) {
      dctn_rp_community_setop    = dctn_rp_community->searchMethod("=");
      dctn_rp_community_appendop = dctn_rp_community->searchMethod(".=");
      dctn_rp_community_append   = dctn_rp_community->searchMethod("append");
   }
   if (dctn_rp_aspath)
      dctn_rp_aspath_prepend   = dctn_rp_aspath->searchMethod("prepend");
}


  
