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


  
