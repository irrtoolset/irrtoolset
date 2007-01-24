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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
//#include "dbase.hh"
//#include "Node.h"
//#include "debug.hh"
//#include "trace.hh"
//#include "rusage.hh"
//#include "aut-num.hh"
//#include "Route.hh"
#include "CIDRAdvisor.hh"
#include "radix.hh"
#include "bgp/bgproute.hh"

extern void append(addr ** list, addr * element);
//*extern addr * component_match(addr **,Filter_Action **);

addr * allowed_proxy_aggregates(ASt aggregator, ASt neighbor, addr * aggregates);
addr * check_components(addr *);
//*void Originate(BGPRoute &r, ASt as);
void Announce(BGPRoute &r, ASt as);
//void Import(BGPRoute &r, Pix as, Pix laddr, Pix ras, Pix raddr);

#define PREFIX_DEFAULT_NOT_ALLOWED 0  // default initial state of the addr prefix
#define PREFIX_ALLOWED 1         // addr prefix is allowed
#define PREFIX_NOT_ALLOWED 2


static ASt agg_as;
static ASt neighbor_as;
static int allowed = PREFIX_DEFAULT_NOT_ALLOWED;
static IRR * whois;
static ActionDictionary * actiondictionary;




//----------------------------------------------------------------------------
// addr *
// allowed_proxy_aggregates(ASt aggregator, ASt neighbor, addr * aggregates)
//
//  Checks a list of aggregates to see which can be aggregated 
//    In order to be aggregated the aggregator AS must be the center AS of
//    the path taken for a route to travel one side of the internet to the 
//    opposite.  For Example, AS3 can aggregate routes destined for AS1 or AS2
//    coming from AS4 or AS5
//
//              AS1 --       -- AS4
//                    \    /
//              AS2 -- AS3 --- AS5
//                        
// Not Finished Yet
//----------------------------------------------------------------------------
addr *
allowed_proxy_aggregates(ASt aggregator, ASt neighbor, addr * aggregates, IRR * irr) {
  addr * components = NULL;
  addr * proxy_aggs = NULL;

  agg_as = aggregator;
  neighbor_as = neighbor;
  whois = irr;
  
  actiondictionary = new ActionDictionary;
    
  if(!aggregates)
    return(NULL);

  while(aggregates) {
    allowed = PREFIX_DEFAULT_NOT_ALLOWED;
    //*if((aggregates->rt).origin.size() == 1) 
    if (aggregates->bgproute->origin->getSize() == 1) {
    
    //*  if (((aggregates->rt).origin.head())->pix == agg_as)
      if (aggregates->bgproute->origin->contains(agg_as)) //- aggregate originates from aggregator AS
        allowed = PREFIX_ALLOWED;
      else { //- We have to search for all paths between ASes on either side of the aggregator AS
           //- and make sure that the Aggregator is always in the middle somewhere
       //* Originate(aggregates->bgproute,((aggregates->bgproute->.origin.head())->pix);
        ItemASNO * origin_asln = aggregates->bgproute->origin->list.head();
        Announce(*(aggregates->bgproute), origin_asln->asno);
      } 
    }
    
    if (allowed == PREFIX_ALLOWED)
      append(&proxy_aggs, aggregates);
    else {
      components = NULL;
      components = check_components(aggregates->components);
      for (; components ; components = components->next)
        append(&proxy_aggs, components);
    }
    
    aggregates = aggregates->next;
  }

  return(proxy_aggs);
}




//----------------------------------------------------------------------------
// addr *
// check_components(addr *components)
//
//----------------------------------------------------------------------------
addr *
check_components(addr *components) {
cerr << "---- check_components not implemented" << endl;
  addr *component_list = NULL;
  addr *tmp = NULL;
  radix_node *root = NULL;

  while(components) {
    allowed = PREFIX_DEFAULT_NOT_ALLOWED;
    //*if(((components->rt).origin.head())->pix == agg_as) {
    ItemASNO * origin_asln = component_list->bgproute->origin->list.head();
    if(origin_asln->asno == agg_as) {
      allowed = PREFIX_ALLOWED;
    } else {
      //*Originate(components->rt,((components->rt).origin.head())->pix);
      Announce(*(components->bgproute), origin_asln->asno);
    }

    if(allowed == PREFIX_ALLOWED)
      append(&component_list,components);
    components = components->next;
  }
    
  if (component_list) {
    root = radix_create_node(NULL,0,0,INVALID_AS,EMPTY);
  } else {
    return(NULL);
  }
  
  for(; component_list ; component_list = component_list->next) {
    //*ListNodePix *p = (component_list->rt).origin.head();
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



//-------------------------------------------------------------
// void Import(BGPRoute &bgproute, ASt export_as, ASt import_as,
//             IPAddr *import_ip, IPAddr *export_ip)
//
//  export_as was the current as import_as is the peer_as 
//  from IterateExports and 
//
//  Now we have to check to see if the peer as will import the
//  current as since BGP works from destination to source
//
//  import_ip & export_ip are the ip addresses of the routers
//      
//     also look at prpath/prpath.cc for the original code
//     upon which this is based
//-------------------------------------------------------------
void Import(BGPRoute &bgproute, ASt export_as, ASt import_as,
            const IPAddr *import_ip, const IPAddr *export_ip) {
  const AutNum * import_autnum = NULL;
  // Check to see if loop is created by adding this AS 
  if (! bgproute.loopCheck(import_as)) {  
    // Get Imports from IRR 
    import_autnum = whois->getAutNum(import_as);
    if (import_autnum) {
      AutNumSelector<AttrImport> import_itr(import_autnum, "import", NULL,
					    export_as, export_ip, import_ip);
      if (const FilterAction * fa = import_itr.first()) {
              for (; fa; fa = import_itr.next()) {
        // if not at max nodes in route 
        // then continue depth first search
          if (!bgproute.atMaxRouteLength()) {
            if (bgproute.isMatching(import_as, fa->filter, whois)) {
              if (import_as == neighbor_as) { // Have we found the source
                if(bgproute.aspath->contains(agg_as)) {
                  allowed = PREFIX_ALLOWED; 
                } else { 
                  allowed = PREFIX_NOT_ALLOWED; 
                  return;
                }
              } else {
                bgproute.executeAction(fa->action, actiondictionary);
                Announce(bgproute, import_as);
              }
            } // end if (bgproute.isMatching())
          } else {
            //cerr << "  Max Path length reached." << endl;
          }
        } // end for
        
      } else {  //No Imports then Check defaults
      // cerr << "   No Imports! Checking for default..." << endl;
        AttrIterator<AttrDefault> itr(import_autnum, "default");
        for (const AttrDefault *  ad = itr.first(); ad; ad = itr.next()) {
          if (! bgproute.atMaxRouteLength()) {
            if (bgproute.isMatching(import_as,ad->filter, whois)) {
              if (import_as == bgproute.source) { // Check to see if we found the source
                bgproute.print();
              } else {
                bgproute.executeAction(ad->action, actiondictionary);
                Announce(bgproute, import_as);
              }
            } // end if (bgproute.isMatching
          } else {
            //cerr << "Max Path length reached." << endl;
          }
        }  // end for
      }
    }  // end if (import_autnum)
  }  // end if (! bgproute.loopCheck())
}

//--------------------------------------------------------------
//  void IterateExports(BGPRoute &bgproute, ASt as, AutNum * as_autnum,
//                      ASt peer_as, IPAddr * peer_ip, IPAddr * local_ip)
//
//    Find export filter which contain peer_as,
//    Loop over list of exports,
//    Execute the actions for that export filter
//
//    peer_ip & local_ip are ip addresses for the routers
//-------------------------------------------------------------
void IterateExports(BGPRoute &bgproute, ASt as, const AutNum * as_autnum,
                    ASt peer_as, const IPAddr * peer_ip, 
		    const IPAddr * local_ip) {
  // Get matching Export Filter-Actions
  AutNumSelector<AttrExport> export_itr(as_autnum, "export", NULL, peer_as, peer_ip, local_ip);
  const FilterAction * fa;
  BGPRoute * import_route;
  // Check each Export filter for possible correct route
  for (fa = export_itr.first(); fa; fa = export_itr.next()) {
    import_route = new BGPRoute(bgproute); // Duplicate the currrent BGPRoute 
                                           // to check this possible path

    // For each export check if they allow the current AS's  
    // traffic on their network by looking at the peer AS's imports
    // Import(bgproute, current as, ip of current as, peer AS, peer IP address)
    import_route->executeAction(fa->action, actiondictionary);
    Import(*import_route, as, peer_as, peer_ip, local_ip);
    delete import_route;  // We are done with it
    
    if(allowed == PREFIX_NOT_ALLOWED) {
      return;
    }
  }  
}  

//--------------------------------------------------------------
//  void Announce(BGPRoute &r, ASt as) 
//    Analogous to a bgp AS announcement
//      Check exports of current AS to see where to go next
//      doing a recursive depth first search on each export  
//      it terminates when either the max route length is reached
//      or the BGP's source AS is found
//
//   Announce()-->IterateExports()-->Imports()-->Announce()...until path is found 
//-------------------------------------------------------------
void Announce(BGPRoute &bgproute, ASt as) {
   const AutNum * as_autnum = whois->getAutNum(as);
   ASt peer_as;
   bgproute.add(as);  // Add the current AS as to the BGPRoute path list

   if (as_autnum) {  // Do we have an autnum record for this as?
      // Find all peers of current AS
      AutNumPeeringIterator itr(as_autnum);
      for (const Peering *peering = itr.first(); itr; peering = itr.next()){
	 // Get exports containing peerAS and check the imports of the peer
        IterateExports(bgproute, as, as_autnum, peering->peerAS,
                       &peering->peerIP, &peering->localIP);
        if(allowed == PREFIX_NOT_ALLOWED) {
          return;
        }
      } // End for
   } else 
      cerr << "Warning: No autnum record found for AS" << as << endl;
}


//----------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------

//void Originate(Route &r, Pix as) {
//void Originate(BGPRoute & r, ASt as) {
 /*  if (!AS_map.define(as))
//      return; */
//   Announce( r, as);
//}




//----------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------

/*
//void Announce(Route &r, Pix as) {
  ASPolicy *p;
  Filter_Action *fap;
  Filter_Action *ifap;
  InterASPolicy *ip;
  int interas_match;

  if (!AS_map.define(as))
    return;

  AutNum &autnum = *(AutNum *) AS_map.contents(as).definition;

  r.aspath.append((new ListNodePix(as))->l);

  // for each peer
  for (p = autnum.peers.head(); p; p = autnum.peers.next(p->peers)) {
    // for each as-out expression
    for (fap = p->out.head(); fap; fap = p->out.next(fap->falist)) {
      if (fap->filter->match(r)) {
        fap->action->Execute(r);
        interas_match = 0;

        // for each laddr raddr pair
        for (ip = p->interas.head(); ip; ip = p->interas.next(ip->interas))
          // for each interas-out expression
          for (ifap = ip->out.head(); 
               ifap; 
               ifap = ip->out.next(ifap->falist))
            if (ifap->filter->match(r)) {
              interas_match = 1;
              ifap->action->Execute(r);
              Import(r, p->peeras, ip->raddress, as, ip->laddress);
       //---       if(allowed == PREFIX_NOT_ALLOWED) { return; }
            }

        // left over route
        if (!interas_match) { 
          Import(r, p->peeras, NULL, as, NULL);
          if(allowed == PREFIX_NOT_ALLOWED) { return; }
        }
      }
    }
  }

  ListNodePix *lnp = r.aspath.tail();
  r.aspath.unlink(lnp->l);
  delete lnp;
} */




/*
inline int aspath_loop_check(Route &r, Pix as) {
   for (ListNodePix *p = r.aspath.head(); p; p = r.aspath.next(p->l))
      if (p->pix == as)
         return TRUE;

   return FALSE;
}

*/

/*

inline void check_source_if_not_announce(Route &r, Pix as) {
  int tmp_allowed = FALSE;
  if (as == neighbor_as) {
    for (ListNodePix *p = r.aspath.head(); p; p = r.aspath.next(p->l))
      if(p->pix == agg_as) tmp_allowed = TRUE;
    if(tmp_allowed)
      allowed = PREFIX_ALLOWED; 
     else 
      allowed = PREFIX_NOT_ALLOWED; 
  } else
    Announce(r, as);
}





void Import(Route &r, Pix as, Pix laddr, Pix ras, Pix raddr) {
   ASPolicy *p;
   Filter_Action *fap;
   Filter_Action *ifap;
   InterASPolicy *ip;

   if (!AS_map.define(as))
      return;

   AutNum &autnum = *(AutNum *) AS_map.contents(as).definition;
   int match;

   if (aspath_loop_check(r, as)) // route has a loop
      return;

   for (p = autnum.peers.head(); p; p = autnum.peers.next(p->peers))
      if (p->peeras == ras)
         break;

   if (!p) // this peer does not exist
      return;

   match = 0;
   for (fap = p->in.head(); fap; fap = p->in.next(fap->falist))
      if (fap->filter->match(r)) {
         match = 1;
         fap->action->Execute(r);
         for (ip = p->interas.head(); ip; ip = p->interas.next(ip->interas))
            for (ifap = ip->in.head(); ifap; ifap = ip->in.next(ifap->falist))
               if (ifap->filter->match(r)) {
                  ifap->action->Execute(r);
                  match = 2;
                  check_source_if_not_announce(r, as);
		  if(allowed == PREFIX_NOT_ALLOWED) { return; }
               }
         if (match != 2) { // left over route
            check_source_if_not_announce(r, as);
	    if(allowed == PREFIX_NOT_ALLOWED) { return; }
         }
      }

   if (!match) { // check to see if we default
      for (fap = p->dflt.head(); fap; fap = p->dflt.next(fap->falist))
         if (fap->filter->match(r)) {
            fap->action->Execute(r);
            check_source_if_not_announce(r, as);
	    if(allowed == PREFIX_NOT_ALLOWED) { return; }
         }
   }
}

*/
