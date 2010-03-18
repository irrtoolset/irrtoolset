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
//             John Mehringer <mehringe@isi.edu> updated for RAToolSet 4.x

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <cstdlib>
#include <iostream.h>
#include <iomanip.h>

#include "irr/irr.hh"
#include "irr/autnum.hh"
//#include "Error.hh"
//#include "aut-num.hh"
//#include "Route.hh"
#include "radix.hh"

extern radix_node *test_root; // defined in cidr.cc used for the expts

//*Pix aggregator_as = NULL;
//ASt aggregator_as = INVALID_AS;

static BGPRoute r; // aspath used to check for looping is an attribute of r
static addr * aggs_final = NULL;

// do not use rt.define() 'cos it checks for the exact match in the database
// and if nothing found sets origin to NULL.


void unlink(addr **,addr **);
addr * copy_list(addr *);
addr * component_match(addr * components, Filter * filter, ASt peer_as, IRR * whois);
void import_match(ASt peeras, ASt as, addr ** aggregates, int radius, IRR * whois);
void append_aggregates(addr**,addr**);
addr * refine_aggs(addr *);
void export_match(ASt as, addr ** aggregates, int radius, IRR * whois);
void examine_policy(ASt source_as, int proxies, addr *aggregates, int radius, IRR * whois);
void print_aggregates(addr *,int);
void append(addr ** list, addr * element);
void delete_list(addr **);
addr* chk_policy(ASt as1, ASt as2, addr *aggs, IRR * whois);

extern addr * allowed_proxy_aggregates(ASt aggregator, ASt neighbour, addr * aggregates, IRR * irr);




//--------------------------------------------------------------
// void 
// unlink(addr **list, addr **element)
//  
//  Removes an elemement from a list
//--------------------------------------------------------------
void unlink(addr **list, addr **element) {
  addr *tmp;

  if(*list == *element) {
	  *list = (*list)->next;
    delete_list(&((*element)->components));
    delete *element;
    return;
  }
    
  tmp = *list;
  while(tmp->next != *element) 
  tmp = tmp->next;
    
  tmp->next = (*element)->next;
  delete_list(&((*element)->components));
  delete *element;
}
	



//--------------------------------------------------------------
// void 
// append(addr ** list, addr * element) {
//  
//  Makes a Copy of element and Appends it to the to the addr list
//--------------------------------------------------------------
void append(addr ** list, addr * element) {
  addr * new_elmt, * tmp;
  addr * new_elmt_comps, * original_comp, * copy_comp;

  //- Copy element to new_elmt
  new_elmt = new addr(*element);
  
  //* replaced by above
  /*new_elmt = new addr;
  *new_elmt = *element;
  new_elmt->next = NULL;
  new_elmt->rt.nlri.pix = element->rt.nlri.pix;

  for(ListNodePix *p = (element->rt).origin.head();
                   p;
                   p =  (element->rt).origin.next(p->l)) {
    ListNodePix *origin_list = new ListNodePix(p->pix);
    (new_elmt->rt).origin.append(origin_list->l);
  }*/
  
  
  
  //- Copy over Components to new element 
  original_comp = element->components;
  if(!original_comp)
    new_elmt->components = NULL;
  else {
    //*new_elmt->components = new addr;
    new_elmt->components = new addr(*original_comp);
    copy_comp = new_elmt->components;
  }
  while (original_comp) {
 // for(; original_comp ; original_comp = original_comp->next , copy_comp = copy_comp->next) {
    //*tmp_comps->prefix = comps->prefix;
    //*tmp_comps->length = comps->length;
    //*tmp_comps->rt.nlri.pix = comps->rt.nlri.pix;
	
    //* Copy over origin information
   /* for(ListNodePix *p = (comps->rt).origin.head() ; p ;
        p = (comps->rt).origin.next(p->l)) {
      ListNodePix *list =  new ListNodePix(p->pix);
      (tmp_comps->rt).origin.append(list->l);
    }*/
	
    copy_comp->components =  NULL;
    
    if(original_comp->next) {
      //*tmp_comps->next = new addr;
      copy_comp->next = new addr(* (original_comp->next));
    } else
      copy_comp->next = NULL;
      
    original_comp = original_comp->next;
    copy_comp = copy_comp->next;
  }


  //- Add new_element to list
  
  
  if(!*list) {   //- If no list create one
    *list = new_elmt; 
  } else {       //- Add to end of list
    tmp = *list;
    while(tmp->next != NULL)
      tmp = tmp->next;
    tmp->next = new_elmt;
  }
//    cerr << "aggcheck::append is empty" << endl;
}
    
	


//--------------------------------------------------------------
// void 
// delete_list(addr **list)
//  
//--------------------------------------------------------------
void delete_list(addr **list) {
  addr *tmp;

  if(!(*list)) return;
  while(*list) {
    tmp = *list;
    *list = (*list)->next;
    delete_list(&(tmp->components));
    delete tmp;
  }
}




//--------------------------------------------------------------
// addr * 
// copy_list(addr *aggregates)
//  
//  Copies all elements from aggregates to new list
//--------------------------------------------------------------
addr * 
copy_list(addr * aggregates) {
  //- just cycle through aggregates and 
  //- use append to add them to new list
  addr * new_list = NULL;
  
  if(aggregates) {
    for(; aggregates; aggregates = aggregates->next)
      append(&new_list, aggregates);
    return new_list;
  } else {
    return NULL;
  }
}

    

//---------------------------------------------------------------------------
// addr *
// component_match(addr * components, Filter * filter, ASt PeerAS, IRR * whois)
//  
//   Tests components of an aggregate to see if they pass the filter
//---------------------------------------------------------------------------
addr *
component_match(addr * components, Filter * filter, ASt peer_as, IRR * whois) {
  addr * tmp, * old_tmp, * component_list = NULL;
  radix_node * root = NULL;

  tmp = components;
    
  while (tmp) {
    old_tmp = tmp;
    tmp = tmp->next;
   //* if(fap->filter->match(old_tmp->rt)) {
   if(old_tmp->bgproute->isMatching(peer_as, filter, whois)) {
      append(&component_list,old_tmp);
      unlink(&components,&old_tmp);
    }
  }
    
  if(component_list)
    root = radix_create_node(NULL,0,0,INVALID_AS,EMPTY);
  else
    return(NULL);

  for(; component_list ; component_list = component_list->next) {
    //*ListNodePix * p = (component_list->rt).origin.head();
    //*radix_insert(component_list->prefix,component_list->length,p->pix,root);
    ItemASNO * asln = component_list->bgproute->origin->list.head();
    radix_insert(component_list->prefix, component_list->length, asln->asno, root);
  }
	
  radix_aggregate(root);
  component_list = NULL;
  component_list = radix_get_aggs(root);
  tmp = radix_get_nonaggs(root);
  for(; tmp; tmp = tmp->next)
    append(&component_list,tmp);
  radix_delete_tree(root);
  return(component_list);
}




//---------------------------------------------------------------------------
// void 
// examine_policy(ASt source_as,int proxies,addr *aggregates, int radius)
//  
// not finished
//---------------------------------------------------------------------------
void
examine_policy(ASt source_as, int proxies, addr * aggregates, int radius, IRR * whois) {
  //*ASPolicy *p;
 //* Filter_Action *fap;
  const FilterAction * fa;

  addr * tmp, * old_tmp, * agg_list = NULL, * proxy_aggs = NULL;
  addr * components = NULL, * comps_head = NULL, * aggs = NULL;
  //*Pix originator_as = NULL;
  
  const AutNum * source_autnum = whois->getAutNum(source_as);

  //- Cycle through peers of source AS
//*  for(p = as->peers.head();p;p = as->peers.next(p->peers)) {
  if (source_autnum) {  // Do we have an autnum record for this as?
   
    //*AutNum * as = AS_map.define(source_as);
    //*r.aspath.append((new ListNodePix(source_as))->l);
    r.add(source_as);
  
    // Find all peers of current AS
    AutNumPeeringIterator itr(source_autnum);
    for (const Peering *peering = itr.first(); itr; peering = itr.next()) {
      if (proxies) { //- if we are to check proxy aggregation
        //- Find the allowed proxy aggregates based on routing policy
        proxy_aggs = allowed_proxy_aggregates(source_as, peering->peerAS, aggregates, whois);
      } else {
        proxy_aggs = copy_list(aggregates);
      }
   
      //*cout << AS_map.key(source_as) << "->"<< AS_map.key(p->peeras)<<"\n";
      cout << "AS" << source_as << "->" << "AS" << peering->peerAS << endl;
     
      //- Save proxy_aggs head for the future
      tmp = proxy_aggs;
      
      //- Append peeras to as path ? hmmm
      //-r.aspath.append((new ListNodePix(p->peeras))->l);
      r.add(peering->peerAS);

      //- Cycle through export policies of each peer
     //* for (fap = p->out.head(); fap; fap = p->out.next(fap->falist)) {
      AutNumSelector<AttrExport> export_itr(source_autnum, "export", NULL,
                                            peering->peerAS, &peering->peerIP, &peering->localIP);
      for (fa = export_itr.first(); fa; fa = export_itr.next()) {

        //- Check each aggregate in tmp against the peer's export policy
        while(tmp) {
          //*if(((tmp->rt).origin.size() == 1) && fap->filter->match(tmp->rt)) {
          if((tmp->bgproute->origin->getSize() == 1) && 
              tmp->bgproute->isMatching(peering->peerAS, fa->filter, whois)) {
            append(&agg_list, tmp);
            old_tmp = tmp;
            tmp = tmp->next;
            unlink(&proxy_aggs,&old_tmp);
          } else {
            if(tmp->components) {
              components = component_match(tmp->components, fa->filter, peering->peerAS, whois);
              comps_head = components;
              for(; components ; components = components->next)
                append(&agg_list,components);
              delete_list(&comps_head);
            }
            tmp = tmp->next;
          }
        }
        tmp = proxy_aggs;
      }
    
      delete_list(&proxy_aggs);
      proxy_aggs = NULL;
 
      if(radius == -1) //- only the aggregator's policies are examined
        print_aggregates(agg_list,NO_COMPONENTS);
      else {
        //- source_as and peer AS are swtiched in this function call
        //*import_match(aggregator_as,p->peeras,&agg_list,radius);
        import_match(source_as, peering->peerAS, &agg_list, radius, whois);
        aggs = refine_aggs(agg_list);
        print_aggregates(aggs,NO_COMPONENTS);
        delete_list(&aggs);
        aggs = NULL;
      }
      delete_list(&agg_list);       

    //*  ListNodePix *lnp = r.aspath.tail();
    //*  r.aspath.unlink(lnp->l);
    //*   delete lnp;
      r.remove(peering->peerAS);
    }
  //*  delete as;
  } else {
    cout << "No Autnum record found for AS" << source_as << endl;
  } //end if (source_autnum)
}
       

//--------------------------------------------------------------
// void 
// import_match(ASt peeras, ASt as, addr **aggregates,
//              int radius, IRR * whois)
//  
//---------------------------------------------------------------
void
import_match(ASt peeras, ASt as, addr **aggregates, int radius, IRR * whois) {
  //-cerr << "import_match called" << endl;
  //*ASPolicy * p;
  //*Filter_Action * fap;
  FilterAction * fa;
  addr * tmp, * old_tmp, * import_aggs = NULL, * components = NULL;
  addr * comps_head = NULL, * tmp_aggs = NULL;
  const Peering * peering = NULL;

  //* if(!AS_map.define(as)) 
  //*   return;

  //* AutNum * autnum = (AutNum *) AS_map.contents(as).definition;
  const AutNum * autnum = whois->getAutNum(as);
  
  if(!*aggregates || !autnum)
    return;


   //- Check to make sure as has peeras listed as a peer
  //*for(p = autnum->peers.head(); p; p = autnum->peers.next(p->peers))
  //*  if(p->peeras == peeras)
  //*    break;
  AutNumPeeringIterator itr(autnum);
  for (peering = itr.first(); itr; peering = itr.next())
    if(peering->peerAS == peeras)
      break;
      
  //*if(!p) 
  if(!peering) // peer does not exist
    return;

  tmp_aggs = copy_list(*aggregates);
  tmp = tmp_aggs;

  //* for(fap = p->in.head(); fap; fap=p->in.next(fap->falist)) {
  AutNumSelector<AttrImport> import_itr(autnum, "import", NULL,
                                        peering->peerAS, &peering->peerIP, &peering->localIP);
  for (fa = import_itr.first(); fa; fa = import_itr.next()) {
    while(tmp) {
       //- Copy over global aspath
       //**********************
      //*(tmp->rt).aspath.clear();
      //* for (ListNodePix *q = r.aspath.head() ; q && q->pix != as; q = r.aspath.next(q->l)) {
      //*  ListNodePix *list = new ListNodePix(q->pix);
      //*  (tmp->rt).aspath.append(list->l);
      //* }
      delete tmp->bgproute->aspath;
      delete tmp->bgproute->action_aspath;
      tmp->bgproute->aspath = new ASPath(*(r.aspath));
      tmp->bgproute->action_aspath = new ASPath(*(r.action_aspath));


      //*if(((tmp->rt).origin.size() == 1) && fap->filter->match(tmp->rt)) {
      if((tmp->bgproute->origin->getSize() == 1) && 
          tmp->bgproute->isMatching(peering->peerAS, fa->filter, whois)) {
        append(&import_aggs,tmp);
        old_tmp = tmp;
        tmp = tmp->next;
        unlink(&tmp_aggs,&old_tmp);
      } else {
        if(tmp->components) {
          //*components = component_match(&(tmp->components),&fap);
          components = component_match(tmp->components, fa->filter, peering->peerAS, whois);
          comps_head = components;
          for(; components; components = components->next) 
            append(&import_aggs, components);
          if(comps_head)
            if(comps_head->next) {
             //* cout << "import policy" << AS_map.key(as) << endl; 
             cout << "import policy AS" << as << endl; 
            }
          delete_list(&comps_head);
        }
        tmp = tmp->next;
      }
    }
    tmp = tmp_aggs;
  }

  delete_list(&tmp_aggs);
  tmp_aggs = NULL;
    
  //* ListNodePix *q = r.aspath.head();
  ItemASNO * asln = r.aspath->path_list.head();
  //* if(q->pix != peeras) {
  if(asln->asno != peeras) {
    append_aggregates(aggregates,&import_aggs);
    delete_list(&import_aggs);
      //      tmp_aggs = refine_aggs(*aggregates);
      //      delete_list(aggregates);
      //      aggregates = &tmp_aggs;
  } else {
      // this is the case when peeras = aggregator AS in which case we have to
      // do level 2 computation
    delete_list(aggregates);
    *aggregates = import_aggs;
  }

  //*if(radius != -0.5)
  //*  export_match(as, aggregates, radius);
    export_match(as, aggregates, radius, whois);
}
		    

	       

//--------------------------------------------------------------
// void
// append_aggregates(addr **list,addr **append_list)
//   Copies over aggregate addr from append_list to
//   list only if they don't already exist on list
//  
//---------------------------------------------------------------
void append_aggregates(addr ** list,addr ** append_list) {
  addr * old_elmt, * append_elmt,* check_elmt;
  int duplicate_flag = FALSE;
	
  if(!(*append_list)) return;
  
  append_elmt = * append_list;

  while(append_elmt) {
    check_elmt = * list;
    duplicate_flag = FALSE;
    
    //- Copy over an aggregate from "append_list" only 
    //- if it is not already on "list"
    for(; check_elmt; check_elmt = check_elmt->next) 
    //*  if(elmt->rt.nlri.pix == tmp->rt.nlri.pix) 
      if((append_elmt->prefix == check_elmt->prefix) &&
         (append_elmt->length == check_elmt->length)) {
        duplicate_flag = TRUE;
        break;
      }
	    
    if(!duplicate_flag) {
      append(list,append_elmt);
    }
    
    append_elmt = append_elmt->next;
  }
}




//--------------------------------------------------------------
// addr *
// refine_aggs(addr *aggs)
//  
//--------------------------------------------------------------
addr *
refine_aggs(addr *aggs) {
  addr * tmp1, * tmp_list = NULL;
  addr * aggregates_list = NULL, * component_list = NULL;
  addr * components = NULL, * final_list = NULL;
  radix_node * root1 = NULL, * root2 = NULL;
  int recurse = FALSE; // need to recurse if there are more specifics which
                       // contain even more specifics
       
  if(aggs) {
    root1 = radix_create_node(NULL,0,0,INVALID_AS,EMPTY);
    root2 = radix_create_node(NULL,0,0,INVALID_AS,EMPTY);
  } else
    return(NULL);

  for(tmp1 = aggs; tmp1; tmp1 = tmp1->next)  {
    radix_insert(tmp1->prefix, tmp1->length, INVALID_AS, root1);
    if(!(tmp1->components)) {
     //* ListNodePix *p = (tmp1->rt).origin.head();
     //* radix_insert(tmp1->prefix, tmp1->length, p->pix, root2);
     ItemASNO * asln = tmp1->bgproute->origin->list.head();
     radix_insert(tmp1->prefix, tmp1->length, asln->asno, root2);
    } else
      for(components = tmp1->components; components; components = components->next) {
      //*  ListNodePix *p = (tmp1->rt).origin.head();
      //*  radix_insert(components->prefix,components->length,p->pix,root2);
        ItemASNO * asln = tmp1->bgproute->origin->list.head();
        radix_insert(components->prefix,components->length, asln->asno, root2);
      }
  }

  for(tmp1 = aggs; tmp1; tmp1 = tmp1->next) 
    if(radix_search(tmp1->prefix, tmp1->length, root1) == MORE_SPECIFIC) {
      append(&tmp_list,tmp1);
      if(!(tmp1->components)) 
        radix_delete(tmp1->prefix,tmp1->length,root2);
      else {
        recurse = TRUE;
        for(components = tmp1->components; components; components = components->next) 
          radix_delete(components->prefix,components->length,root2);
      }
    }

  if(recurse) {
    final_list = refine_aggs(tmp_list);
    delete_list(&tmp_list);
  } else
    final_list = tmp_list;

  radix_aggregate(root2);
  aggregates_list = radix_get_aggs(root2);
  for(; aggregates_list; aggregates_list = aggregates_list->next)
    append(&final_list,aggregates_list);
    
  component_list = radix_get_nonaggs(root2);
  for(; component_list; component_list = component_list->next)
    append(&final_list,component_list);  

  radix_delete_tree(root1);
  radix_delete_tree(root2);
  return(final_list);
  
  
//  cerr << "refine_aggs not done yet !!" << endl;
//  return aggs;
}




//--------------------------------------------------------------
// inline int
// aspath_loop_check(BGPRoute & bgp_rt, ASt as)
//  
//---------------------------------------------------------------
inline int
aspath_loop_check(BGPRoute & bgp_rt, ASt as) {
  //*for (ListNodePix *p = r.aspath.head(); p; p = r.aspath.next(p->l))
  for (ItemASNO * asln = bgp_rt.aspath->path_list.head(); asln;
                  asln = bgp_rt.aspath->path_list.next(asln)) {
      //*if (p->pix == as)
      if (asln->asno == as)
         return TRUE;
  }
   return FALSE;
}




//--------------------------------------------------------------
// void 
// export_match(ASt as, addr **aggregates, int radius, IRR * whois)
//  
//---------------------------------------------------------------
void
export_match(ASt as, addr ** aggregates, int radius, IRR * whois) {
 //* ASPolicy *p;
//*  Filter_Action *fap;
  FilterAction * fa;
//*  AutNum *autnum = AS_map.define(as);
  const AutNum * autnum = whois->getAutNum(as);
  addr * tmp, * old_tmp, * agg_list = NULL, * tmp_aggs = NULL;
  addr * final_aggs, * components = NULL, * comps_head = NULL, * aggs = NULL;
  const Peering * peering = NULL;

  if(!*aggregates || !autnum) 
    return;
    
  //*for(p = autnum->peers.head(); p; p = autnum->peers.next(p->peers)) {
  AutNumPeeringIterator itr(autnum);
  for (peering = itr.first(); itr; peering = itr.next()) {
  //*  if(aspath_loop_check(r,p->peeras)) {
    if(aspath_loop_check(r,peering->peerAS)) {
      tmp_aggs = NULL;
    //*  ListNodePix *q1 = r.aspath.head();  // aggregator AS
    //*  ListNodePix *q2 = r.aspath.next(q1->l); // neighbour of agg for which
                                              // level > 0 is being computed

      ItemASNO * aggregator_asln = r.aspath->path_list.head();  // aggregator AS
      ItemASNO * neighbor_asln   = r.aspath->path_list.next(aggregator_asln); // neighbour of agg for which
                                                                              // level > 0 is being computed
      // Checking the aggregator's export policy to the current as
      //*if(p->peeras == q1->pix && as != q2->pix) [
      if((peering->peerAS == aggregator_asln->asno) &&
         (as != neighbor_asln->asno)) {
      //*tmp_aggs = chk_policy(p->peeras,as,*aggregates);
        tmp_aggs = chk_policy(peering->peerAS, as, *aggregates, whois);

      // Checking the policies towards the aggregator's neighbour that is
      // being considered . Since the neigbour is always marked some of its 
      // policies will not be checked otherwise.
      } else {
        //*if(p->peeras == q2->pix)
        //*  tmp_aggs = chk_policy(as,p->peeras,*aggregates);
        if(peering->peerAS == neighbor_asln->asno)
          tmp_aggs = chk_policy(as, peering->peerAS, *aggregates, whois);
      }
      
      if(tmp_aggs) {
        append_aggregates(aggregates, &tmp_aggs);
        delete_list(&tmp_aggs);
        //	  tmp_aggs = refine_aggs(*aggregates);
        //	  delete_list(aggregates);
        //	  aggregates = &tmp_aggs;
      }
      continue;
    }

    //*r.aspath.append((new ListNodePix(p->peeras))->l);      
    r.add(peering->peerAS);      

    // cout << "export_match: "<< AS_map.key(as) << "->"<< AS_map.key(p->peeras)<<"\n";
	    
    tmp_aggs = copy_list(*aggregates);
    tmp = tmp_aggs;
	
    //*for (fap = p->out.head(); fap; fap = p->out.next(fap->falist)) {
    AutNumSelector<AttrExport> export_itr(autnum, "export", NULL,
                                          peering->peerAS, &peering->peerIP, &peering->localIP);
    for (fa = export_itr.first(); fa; fa = export_itr.next()) {
      while(tmp) {
        //- Copy over global aspath
        //*(tmp->rt).aspath.clear();
        //*for(ListNodePix *q = r.aspath.head() ; q && q->pix != as; q = r.aspath.next(q->l)) {
        //*  ListNodePix *list =  new ListNodePix(q->pix);
        //*  (tmp->rt).aspath.append(list->l);
        //*}
        delete tmp->bgproute->aspath;
        delete tmp->bgproute->action_aspath;
        tmp->bgproute->aspath = new ASPath(*(r.aspath));
        tmp->bgproute->action_aspath = new ASPath(*(r.action_aspath));


        //*if (((tmp->rt).origin.size() == 1) && fap->filter->match(tmp->rt)) [
        if((tmp->bgproute->origin->getSize() == 1) && 
          tmp->bgproute->isMatching(peering->peerAS, fa->filter, whois)) {
          // first check if the aggregate matches the filter
          append(&agg_list,tmp);
          old_tmp = tmp;
          tmp = tmp->next;
          unlink(&tmp_aggs,&old_tmp);
        } else {
          // if the aggregate does not match the filter then check the
          // components
          if(tmp->components) {
            //*components = component_match(&(tmp->components),&fap);
            components = component_match(tmp->components, fa->filter, peering->peerAS, whois);
            comps_head = components;
            for(; components ; components = components->next)
              append(&agg_list, components);
            if(comps_head)
              if(comps_head->next) {
                //*cout << "Export policy " << AS_map.key(as) << endl;
                cout << "Export policy AS" << as << endl;
              }
            delete_list(&comps_head);
          }
          tmp = tmp->next;
        }
      }
      tmp = tmp_aggs;
    }
      
    delete_list(&tmp_aggs);
    tmp_aggs = NULL;

    // If nothing can be exported to this peer then consider the next peer
    if(!agg_list) {
      //*ListNodePix *lnp = r.aspath.tail();
      //*r.aspath.unlink(lnp->l);
      //*delete lnp;
      r.remove(peering->peerAS);
      continue;
    }

    // Appending the possibly new aggregates we got to the old aggregates
    append_aggregates(aggregates, &agg_list);
    delete_list(&agg_list);       

    // refining the list of aggregates
    //      tmp_aggs = refine_aggs(*aggregates);
    //      delete_list(aggregates);
    //      aggregates = &tmp_aggs;

    //*int aspath_length = r.aspath.size();
    int aspath_length = r.aspath->getPathLength();

    // subtract 2 from aspath length because agg and neighbour already 
    // included. This is because radius is distance from neighbour
    if(radius > (aspath_length - 2) || radius == -3) {
      //*import_match(as,p->peeras,aggregates,radius);
      import_match(as, peering->peerAS, aggregates, radius, whois);
      
    }  
    //*ListNodePix *lnp = r.aspath.tail();
    //*r.aspath.unlink(lnp->l);
    //*delete lnp;
    r.remove(peering->peerAS);

  }
}

    


//---------------------------------------------------------------------------
// void 
// print_aggregates(addr *aggregates, int print_components)
//    Prints out an aggregated set and its components
//---------------------------------------------------------------------------
void print_aggregates(addr *aggregates, int print_components) {
  addr *tmp1,*tmp2;
  char prefix_quad[16];
  int num_aggregates = 0;
  int num_components = 0;

  tmp1 = aggregates;
  while(tmp1) {
    int2dd(prefix_quad,tmp1->prefix);
    printf("%s/%d ",prefix_quad,tmp1->length);
      
    if(print_components) {
      printf("\nComponents:\n");
      tmp2 = tmp1->components;
	
      while(tmp2) {
        ++num_components;
        int2dd(prefix_quad,tmp2->prefix);
        printf("%s/%d ( ",prefix_quad,tmp2->length);
        
        //-Print out Origins here
        for (ItemASNO * asln = tmp2->bgproute->origin->list.head(); asln;
                        asln = tmp2->bgproute->origin->list.next(asln))
          cout << "AS" << asln->asno << " ";
          
        cout << ")\n";
          
        tmp2 = tmp2->next;
      }
      cout << endl;
    }
    tmp1 = tmp1->next;
  }
  cout << endl;
}

       


//---------------------------------------------------------------------------
// addr *
// chk_policy(ASt as1, ASt as2, addr *aggs, IRR * whois)
// 
//---------------------------------------------------------------------------
// Export policy of as1 and import policy of as2 are examined
addr *
chk_policy(ASt as1, ASt as2, addr *aggs, IRR * whois) {
  //*ASPolicy *p;
  //*Filter_Action *fap;
  FilterAction * fa;
  
  //*AutNum &autnum1 = *(AutNum *)(AS_map.define(as1));
  //*AutNum &autnum2 = *(AutNum *)(AS_map.define(as2));
  const AutNum * autnum1 = whois->getAutNum(as1);
  const AutNum * autnum2 = whois->getAutNum(as2);
  addr *tmp, *old_tmp, *agg_list = NULL, *tmp_aggs = NULL;
  addr *components = NULL, *comps_head = NULL;
  const Peering * peering = NULL;

//*  for(p = autnum1.peers.head(); p; p = autnum1.peers.next(p->peers))
  AutNumPeeringIterator itr(autnum1);
  for (peering = itr.first(); itr; peering = itr.next()) {
    //*if(p->peeras == as2)
    if(peering->peerAS == as2)
      break;
  }
  
  //*if(!p)
  if (!peering)
    return(NULL);

  tmp_aggs = copy_list(aggs);
  if(!tmp_aggs) 
    return(NULL);

  tmp = tmp_aggs;
  //*for (fap = p->out.head(); fap; fap = p->out.next(fap->falist)) {
  AutNumSelector<AttrExport> export_itr(autnum1, "export", NULL,
                                        peering->peerAS, &peering->peerIP, &peering->localIP);
  for (fa = export_itr.first(); fa; fa = export_itr.next()) {
    while(tmp) {
      //*if (((tmp->rt).origin.size() == 1) && fap->filter->match(tmp->rt)) {
      if((tmp->bgproute->origin->getSize() == 1) && 
          tmp->bgproute->isMatching(peering->peerAS, fa->filter, whois)) {
        append(&agg_list,tmp);
        old_tmp = tmp;
        tmp = tmp->next;
        unlink(&tmp_aggs,&old_tmp);
      } else {
        if(tmp->components) {
          //*components = component_match(&(tmp->components),&fap);
          components = component_match(tmp->components, fa->filter, peering->peerAS, whois);
          comps_head = components;
          for(; components ; components = components->next)
            append(&agg_list,components);
          delete_list(&comps_head);
        }
        tmp = tmp->next;
      }
    }
    tmp = tmp_aggs;
  }
  delete_list(&tmp_aggs); 
       
  if(!agg_list)
    return(NULL);
    
  tmp_aggs = agg_list;
  agg_list = NULL;

  //*for(p = autnum2.peers.head(); p; p = autnum2.peers.next(p->peers))
  AutNumPeeringIterator itr2(autnum2);
  for (peering = itr2.first(); itr2; peering = itr2.next()) {
    //*if(p->peeras == as1)
    if(peering->peerAS == as1)
      break;
  }

  //*if(!p)
  if (!peering)
    return(NULL);

  tmp = tmp_aggs;
  //*for (fap = p->in.head(); fap; fap = p->in.next(fap->falist)) {
  AutNumSelector<AttrImport> import_itr(autnum2, "import", NULL,
                                        peering->peerAS, &peering->peerIP, &peering->localIP);
  for (fa = import_itr.first(); fa; fa = import_itr.next()) {
    while(tmp) {
      //*if (((tmp->rt).origin.size() == 1) && fap->filter->match(tmp->rt)) {
      if((tmp->bgproute->origin->getSize() == 1) && 
          tmp->bgproute->isMatching(peering->peerAS, fa->filter, whois)) {
        append(&agg_list,tmp);
        old_tmp = tmp;
        tmp = tmp->next;
        unlink(&tmp_aggs,&old_tmp);
      } else {
        if(tmp->components) {
          //*components = component_match(&(tmp->components),&fap);
          components = component_match(tmp->components, fa->filter, peering->peerAS, whois);
          comps_head = components;
          for(; components ; components = components->next)
            append(&agg_list,components);
          delete_list(&comps_head);
        }
        tmp = tmp->next;
      }
    }
    tmp = tmp_aggs;
  }

  delete_list(&tmp_aggs);
  return(agg_list);
}
