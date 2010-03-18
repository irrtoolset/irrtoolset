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


#if !defined(IDENT_OFF)
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7) 
static char ident[] = 
   "@(#)$Id$";
#endif
#endif

#include "config.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <strstream.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "aoe.hh"
#include "aoe.xbm"
#include "TclVar.hh"
#include "util/Argv.hh"
#include "util/version.hh"
#include "util/trace.hh"
#include "util/rusage.hh"
#include "util/net.hh"       // for class ipAddr
#include "irr/rawhoisc.hh"
#include "irr/ripewhoisc.hh"
#include "irr/birdwhoisc.hh"

//*******************************
// Some TK variables defined here
char *tkASPeerList                 = ".body.aspeer.listbox.list";
char *tkPolicyText                 = ".body.workingpolicy.text";
char *tkDefLabel                   = ".body.templatearea.deflabel";
char *tkDefText                    = ".body.templatearea.def.text";
char *tkNeighborLabel              = ".body.templatearea.neighborlabel";
char *tkNeighborText               = ".body.templatearea.neighbor.text";
char *tkExportButton               = ".body.templatebutton.export";
char *tkImportButton               = ".body.templatebutton.import";
char *tkCategoryButton             = ".body.templatebutton.category.menu";
char *tkLogoButton		   = ".body.templatebutton.logo";
char *tkCategoryVar                = "categoryVar";
char *tkCommitButton               = ".body.workingpolicybutton.commit";
char *tkPolicyEditButton           = ".body.workingpolicybutton.edit";
char *tkPolicyShowButton           = ".body.workingpolicybutton.show";
char *tkStatusLine                 = ".statusline.content";


//***********************************
// Some global variables defined here
bool opt_rusage  = false;
Rusage ru(clog, &opt_rusage);
char *opt_my_as  = NULL;
char *opt_rcfile = NULL;
char *display    = NULL;
ASt myAS         = 0;


//**********************
// Define the usage here
void usage(void)  
{
   fprintf(stderr, 
	"\nUsage: aoe  [-help] [-T all | whois_query | whois_response | input]"
	"\n            [-D debug_level] [-version]"
	"\n            [-h server_ip] [-p server_port] [-s db_order]"
        "\n            [-protocol [rawhoisd | ripe]]"
	"\n            [-rusage] [-ignore_errors] [-report_errors]"
	"\n            [-display  display]"
	"\n            [-as as_no]"
	"\n\n\a\a"); 
}


//**************************
// Some initialization stuff 
int start_tracing(char *dst, char *key, char *nextArg) {
   if (nextArg)
      trace.enable(nextArg);
   return 1; // return 1 to signify nextArg is used by us
}

int start_debugging(char *dst, char *key, char *nextArg) {
   Debug(if (nextArg) dbg.enable(atoi(nextArg)));
   return 1; // return 1 to signify nextArg is used by us
}

void init_and_set_options (int argc, char **argv, char **envp) {
   ArgvInfo argTable[] = {
      // RAToolSet common arguments
      // key, type, src, dst, help
      {"-T", ARGV_FUNC, (char *) &start_tracing,           (char *)NULL,
       "Start tracing the next argument"},
      {"-D", ARGV_FUNC, (char *) &start_debugging,         (char *)NULL,
       "Start debugging the next argument"},
      {"-version", ARGV_FUNC, (char *) &version,           (char *)NULL,
       "Show version"},
      {"-rusage", ARGV_BOOL, (char *)NULL,                 (char *)&opt_rusage,
       "On termination print resource usage"},

      IRR_COMMAND_LINE_OPTIONS,

      // aoe specific arguments
      {"-as",  ARGV_STRING,     (char *)NULL, (char *) &opt_my_as,
       "AS number of the aut-num object to use"},
      {"-rcfile", ARGV_STRING,  (char *)NULL, (char *)&opt_rcfile,
       "aoe resource file (default: ~/.aoerc)"},
      {"-display", ARGV_STRING, (char *)NULL, (char *)&display,
       "X display"},

      // End of argument
      {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL, (char *) NULL}
   };

   IRR::handleEnvironmentVariables(envp);

   if (ParseArgv(&argc, argv, argTable, 0) != ARGV_OK) {
      usage();
      exit(1);
   }

   switch (argc) {
   case 2: 
      opt_my_as = argv[1];
      break;
   case 1:
      break;
   default:
      usage();
      exit(1);
   }

   if (opt_my_as) 
     myAS = atoi(opt_my_as + 2);
   else 
     {
     IRR *pcIrr;
     if (!(pcIrr = IRR::newClient())) 
       {
       usage();
       exit(1);
       }

     ipAddr myip;
     myip.setToMyHost();
     
     if ((myAS = pcIrr->getOrigin("%s/32", myip.getIpaddr())) == 0)
       {
       cerr << "aoe could not guess your AS number, "
	 "please specify an AS number on the command line." 
	    << endl;
       delete pcIrr;
       exit(1);
       }
     delete pcIrr;
   }
}

/*
static void colorSyntax(TclText &tt)
{
  for (int i = 0; i < tt.getTotalLine(); i++)
    {
    char pzcBuffer[16];
    const char *pzcLine = tt.getLine(i);
    char *p; 
    if (pzcLine)
      if ((p = strchr(pzcLine, ':')) != NULL)
	{
	int iPos = p - pzcLine;
	sprintf(pzcBuffer, "%d", iPos);
	tt.addLineTag(i, "Attribute", "0", pzcBuffer);
	sprintf(pzcBuffer, "%d", iPos + 1);
	tt.addLineTag(i, "Value", pzcBuffer, "end");
	}
      else
	tt.addLineTag(i, "Value", "0", "end");
    }
//  tt.configureTag("Attribute", "-foreground DeepPink -font *-times-*-i-*"); 
  tt.configureTag("Attribute", "-foreground DeepPink"); 
  tt.configureTag("Value", "-foreground blue");
}
*/

static TclText &operator<<(TclText &tt, const AutNum &au)
{
  tt.clear();
  tt.insert(((AutNum &) au).contents, ((AutNum &) au).size);
  tt.removeLastBlankLines();
  TclApplication::evalf("aoe:colorSyntax %s", tt.getName());
  return tt;
}

/*
// Got core dump if strstream class is used with compiler option -frtti
static TclText &operator<<(TclText &tt, const AutNum &au)
{
   strstream ss;
   tt.clear();
   ss << au << '\0';       // Always null-terminated!!!
   tt.insert(ss.str());
   ss.rdbuf()->freeze(0);  // Users have to unfreeze the buffer explicitly
   tt.removeLastBlankLines();
   TclApplication::evalf("aoe:colorSyntax %s", tt.getName());
   return tt;
}
*/

TclText &operator<<(TclText &t1, TclText &t2)
{
   char *p = strdup(t2.getAll());
//   if (*p == '\n') return t1;
   t1.insert(p);
   free(p);
   return t1;
}

TclList &operator<<(TclList &tl, List<ASPeer> &lh)
{
   char pzcBuffer[64], pzcASPeerNo[16];
   for (ASPeer *pcASPeer = lh.head();
	pcASPeer;
	pcASPeer = lh.next(pcASPeer))
      {
      sprintf(pzcASPeerNo, "AS%d", pcASPeer->getNo());
      switch (pcASPeer->getType())
	 {
	 case dASPeerFromIRR:
	    sprintf(pzcBuffer, "{%-8s (IRR)}", pzcASPeerNo);
	    break;
	 case dASPeerFromBGP:
	    sprintf(pzcBuffer, "{%-8s (BGP)}", pzcASPeerNo);
	    break;
	 case dASPeerFromBoth:
	    sprintf(pzcBuffer, "{%-8s (IRR & BGP)}", pzcASPeerNo);
	    break;
	 case dASPeerNew:
	    sprintf(pzcBuffer, "{%-8s (NEW)}", pzcASPeerNo);
	    break;
	 default:
	    sprintf(pzcBuffer, "{Shoudn't happen!}");
	    break;
	 }
      tl.insert(pzcBuffer);
      }
   return tl;
}

// List<ASPeer> to file
FILE * &operator<<(FILE *fd, List<ASPeer> &lh)
{
   strstream ss;
   for (ASPeer *pcASPeer = lh.head();
        pcASPeer;
        pcASPeer = lh.next(pcASPeer))
      {
      ss << "AS" << pcASPeer->getNo();
      switch (pcASPeer->getType())
         {
         case dASPeerFromIRR:
            ss << "(IRR)";
            break;
         case dASPeerFromBGP:
            ss << "(BGP)";
            break;
         case dASPeerFromBoth:
            ss << "(IRR & BGP)";
            break;
         case dASPeerNew:
            ss << "(NEW)";
            break;
         default:
            ss << "{Shoudn't happen!}";
            break;
         }
      ss << endl;
      }
   ss << '\0';
   fputs(ss.str(), fd);
   return(fd);
}

List<ASPeer> &operator<<(List<ASPeer> &lh, BgpASPath &ap)
{
   ASPeer *pcASPeer;
   FirstAndLastAS *pcFirstAndLastAS;
   for (pcFirstAndLastAS = ap.cFirstAndLastASes.head(); 
	pcFirstAndLastAS; 
	pcFirstAndLastAS = ap.cFirstAndLastASes.next(pcFirstAndLastAS))
      {
      for (pcASPeer = lh.head(); pcASPeer; pcASPeer = lh.next(pcASPeer))
	 if (pcASPeer->getNo() == pcFirstAndLastAS->getFirst()) 
	    break;
      if (pcASPeer) 
	 // Found!
	 pcASPeer->setType(pcASPeer->getType() | dASPeerFromBGP);
      else
	 {
	 // Not found!
	 pcASPeer = new ASPeer(pcFirstAndLastAS->getFirst(), dASPeerFromBGP);
	 lh.append(pcASPeer);
	 }
      }
   return lh;
}



//***************************
// Command classes start here

int ListPeer::updateWindowTitle(TclApplication *pcTcl, int iCategory, 
				char *pzcASNo)
{
   char *title, *dummy = "Shouldn't happen!";
   char pzcBuffer[128];
   AoeApplication *pcApp = (AoeApplication *)pcTcl;
   switch (iCategory)
      {
      case 0: title  = "from IRR"; break;
      case 1: title  = "from BGP Dump"; break;
      case 2: title  = "from Peer's aut-num"; break;
      case 3: title  = dummy; break;  // Shouldn't happen!
      default: 
	{
	sprintf(pzcBuffer, "PolicyTemplate(%d,Description)", iCategory - 4);
	title = pcApp->getVar(pzcBuffer);
	break;
	}
      }
   sprintf(pzcBuffer, "%s -- %s", pzcASNo, title);
   // Update label on top of def text widget
   TCL(!pcApp->pcDefLabel->attach(pzcBuffer));
   // Update title of main window
   TCL(!pcApp->evalf("wm title . {%s (%s)}", pcApp->getAppName(), pzcBuffer));
   return TCL_OK;
}

int ListPeer::displayIRRPolicy(int iImport, int iExport, ASt tAS,
			       TclText *pcText, const AutNum *pcAutNum)
{
  // Clear the text widget first no matter if the peer exist or not!
   pcText->clear();
   if (pcAutNum)
     {
     if (iImport)
       for (AutNumImportIterator itr(pcAutNum, tAS); itr; itr++)
	 pcText->insert(itr()->getObject()->contents + itr()->getOffset(), 
			itr()->getLen());
     if (iExport)
       for (AutNumExportIterator itr(pcAutNum, tAS); itr; itr++)
	 pcText->insert(itr()->getObject()->contents + itr()->getOffset(), 
			itr()->getLen());
     pcText->removeLastBlankLines();
     }
   return 1;
}

int ListPeer::generatePolicyFromBGP(int iImport, int iExport,
				    ASt tMyAS, ASt tPeerAS, TclText *pcText, 
				    BgpASPath *pcBgpASPath, int iPeerASType)
{
   strstream ss;

   // Clear the text widget first no matter if the peer exist or not!
   pcText->clear();   

   if (iPeerASType & dASPeerFromBGP)
      {
      if (iImport)
	 {
	 // import
	    ss << "import: from "
	       << "AS" 
	       << tPeerAS
	       << endl
	       << "        action pref = 10;"
	       << endl
	       << "        accept ";

	    FirstAndLastAS *pcFirstAndLastAS = pcBgpASPath->find(tPeerAS);
	    int c = 0;  // for tidy printing 10 ASes per line
	    for (int i = pcFirstAndLastAS->getLast().first(); 
		 i >= 0; 
		 i = pcFirstAndLastAS->getLast().next(i))
	       {
			if (c == 0 && i != pcFirstAndLastAS->getLast().first()) ss << "\t";
	       		ss << "AS" << i << " ";
			c++;
		        if (c >= 9) { ss << endl; c = 0; }
	       }
	    if (c < 10) ss << endl;
	 }
      if (iExport)
	 // export
	 // Temp hack ???
	 ss << "export: to "
	    << "AS"
	    << tPeerAS
	    << endl
	    << "        announce ANY"
	    << endl;
      }

   // Fill the text widget
   ss << '\0';
   pcText->insert(ss.str());
   pcText->removeLastBlankLines();
   ss.rdbuf()->freeze(0);  // This is also a must
   return 1;
}

int ListPeer::generatePolicyFromPeer(int iImport, int iExport,
				     ASt tMyAS, 
				     ASt tPeerAS, const AutNum *pcPeerAutNum, 
				     TclText *pcText, int iPeerASType)
{
   strstream ss;

   // Clear the text widget first no matter if the peer exist or not!
   pcText->clear();   
   
   if (pcPeerAutNum && (iPeerASType & dASPeerFromIRR))
   {
      	if (iImport)
   	{
                // iterate all peerings of AS
		AutNumPeeringIterator peerings(pcPeerAutNum);
	        const Peering *peering = peerings.first();
      		for (peering; peering; peering = peerings.next())
                        // if requested peer
       			if (peering->peerAS == tMyAS)
       			{
                                // gather all the matching policies 				      	      // for requested peer and all router's IPs
				AutNumSelector<AttrExport> itr(pcPeerAutNum, "export", NULL, tMyAS, &peering->peerIP, &peering->localIP);

                                // get filter-action
				const FilterAction *fa = itr.first();
		       		int last = 0;
        			for (; fa && !last; fa = itr.next())	{
              				ss << "import: from "
                 			<< "AS" << tPeerAS << " ";
                                        // if local IP defined for peering
					if (peering->localIP != NullIPAddr)
                 				ss << peering->localIP.get_text();
                                        // if peer IP defined for peering
					if (peering->localIP != NullIPAddr)
                 				ss << " at " << peering->peerIP.get_text();
                 			ss << endl
                 			<< "        action pref = 100;"
                 			<< endl
                 			<< "        accept "
                                        // extract filter for this peering
                 			<< *(fa)->filter
                 			<< endl;
				}
			}
	}
      	if (iExport)	{
		// iterate peerings defined for AS
		AutNumPeeringIterator peerings(pcPeerAutNum);
                const Peering *peering = peerings.first();
                for (peering; peering; peering = peerings.next())
                        // if requested peer
                        if (peering->peerAS == tMyAS)
                        {
				// gather all the matching policies                                 	      // for requested peer and all router's IPs
	                	AutNumSelector<AttrImport> itr(pcPeerAutNum, "import", NULL, tMyAS, &peering->peerIP, &peering->localIP);
                        	const FilterAction *fa = itr.first();
                        	int last = 0;
                        	for (; fa && !last; fa = itr.next())	{
                                	ss << "export: to "
                                	<< "AS" << tPeerAS << " ";
					// if locat IP defined for peering
					if (peering->localIP != NullIPAddr)
                                                ss << peering->localIP.get_text();
					// if peer IP defined for peering
                                        if (peering->localIP != NullIPAddr)
                                                ss << " at " << peering->peerIP.get_text();	
                        	        ss<< endl
                                	<< "        action pref = 100;"
                	                << endl
                        	        << "        announce "
					// extract filter
                                	<< *(fa)->filter
                                	<< endl;
				}
			}
	}
   }

   // Fill the text widget
   ss << '\0';
   pcText->insert(ss.str());
   pcText->removeLastBlankLines();
   ss.rdbuf()->freeze(0);  // This is also a must
   return 1;
}

int ListPeer::generateTemplatePolicy(int iCategory, int iImport, int iExport,
				     char *pzcMyAS, char *pzcPeerAS, 
				     TclText *pcText)
{

   TclApplication *pcApp = (TclApplication *)getExtraArgument();
   strstream ss;
   char pzcBuffer[128];
   // Clear the text widget first no matter if the peer exist or not!
   pcText->clear();   

   if (iImport)
      {
      // import
      sprintf(pzcBuffer, "PolicyTemplate(%d,import)", iCategory - 4);
      ss << "import: "
	 << substitute(pcApp->getVar(pzcBuffer), pzcMyAS, pzcPeerAS)
	 << endl;
      }
   if (iExport)
      {
      // export
      sprintf(pzcBuffer, "PolicyTemplate(%d,export)", iCategory - 4);
      ss << "export: "
	 << substitute(pcApp->getVar(pzcBuffer), pzcMyAS, pzcPeerAS)
	 << endl;
      }
   // Fill the text widget
   ss << '\0';
   pcText->insert(ss.str());
   pcText->removeLastBlankLines();
   ss.rdbuf()->freeze(0);  // This is also a must
   return 1;
}

char *ListPeer::substitute(char *pzcString, char *pzcMyAS, char *pzcPeerAS)
{
   static char pzcBuffer[dBufferLength + 1 + 32];
   char *pzcMyASVar = "$MyAS", *pzcPeerASVar = "$PeerAS";
   char *p1, *p2;
   int iMyASVarLen = strlen(pzcMyASVar), iPeerASVarLen = strlen(pzcPeerASVar);
   int iTotal = 0;
   p1 = p2 = pzcString;
   pzcBuffer[0] = 0;
   while (p2 = strchr(p2, '$'))
      {      
      int iLen = p2 - p1;
      strncpy(pzcBuffer + iTotal, p1, iLen);
      iTotal += iLen;
      if (strncmp(p2, pzcMyASVar, iMyASVarLen) == 0)
	 {
	 p2 += iMyASVarLen;
	 char *p = pzcMyAS;
	 iLen = strlen(p);
	 strncpy(pzcBuffer + iTotal, p, iLen);
	 iTotal += iLen;
	 }
      else
	if (strncmp(p2, pzcPeerASVar, iPeerASVarLen) == 0)
	   {
	   p2 += iPeerASVarLen;
	   char *p = pzcPeerAS;
	   iLen = strlen(p);
	   strncpy(pzcBuffer + iTotal, p, iLen);
	   iTotal += iLen;
	   }
        else
	   strncpy(pzcBuffer + iTotal++, p2++, 1); 
      p1 = p2;
      }
   pzcBuffer[iTotal] = 0;
   // Copy the rest
   if (*p1) strcat(pzcBuffer, p1);
   return pzcBuffer;
}

int ListPeer::command(int argc, char *argv[])
{
   if (argc != 2) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
/*
   printf("\nAS = %s, Name = %s, argc = %d, argv[0] = %s, argv[1] = %s", 
	  pcApp->getASNo(), getName(), argc, argv[0], argv[1]);
*/
   // Check if the list is empty of not,
   // If empty, it is NOT an error but do nothing
   if (!argv[1][0]) 
      {
      if (pcApp->cASPeers.head())
	 {	
	 if (!pcApp->eval("showWarning {Please point the peer in the listbox "
			  "on the left hand side you are going to show!}")) 
	    return TCL_ERROR;
	 }
      else
	 {
	 if (!pcApp->eval("showWarning {Peer list is empty now!}")) 
	    return TCL_ERROR;
	 }
      return TCL_OK;
      }

   int iImport      = pcApp->pcImportButton->getOption();
   int iExport      = pcApp->pcExportButton->getOption();
   int iCategory    = pcApp->pcCategoryButton->getOption();
   int iPeerASIndex = atoi(argv[1]);
   int iPeerASType  = pcApp->getASPeerType(iPeerASIndex);

   ASt tMyAS        = pcApp->getASNo();
   ASt tPeerAS      = pcApp->getASPeerNo(iPeerASIndex);
   
   char pzcMyAS[16], pzcPeerAS[16];
   sprintf(pzcMyAS, "AS%d", tMyAS);
   sprintf(pzcPeerAS, "AS%d", tPeerAS);

   // Setup $PeerAS properly
   if (!pcApp->evalf("set PeerAS %s", pzcPeerAS)) return TCL_ERROR;

   // Make the window title more meaningful
   updateWindowTitle(pcApp, iCategory, pzcMyAS);
   if (!pcApp->pcNeighborLabel->attach(pzcPeerAS)) return TCL_ERROR;
   pcApp->pcPolicyShowButton->labelBothString(pzcPeerAS);
   if (!pcApp->eval("showPolicyText")) return TCL_ERROR;
   

   const AutNum *pcPeerAutNum = pcApp->pcIrr->getAutNum(tPeerAS);
   // Display peer of working AS
   displayIRRPolicy(iExport, iImport, tMyAS, 
		    pcApp->pcNeighborText, pcPeerAutNum);

   switch (iCategory)
      {
      case 0: // from IRR Database
	 {
	 // Display working AS
	 displayIRRPolicy(iImport, iExport, tPeerAS, 
			  pcApp->pcDefText, pcApp->pcAutNum);
	 break;
	 }
      case 1: // from BGP Dump (AS-Path)
//	 pcApp->pcNeighborText->clear();
	 generatePolicyFromBGP(iImport, iExport, 
			       tMyAS, tPeerAS, pcApp->pcDefText, 
			       &pcApp->cBgpASPath, iPeerASType);
	 break;
      case 2: // copy from peer (valid only for AS from IRR)
	 generatePolicyFromPeer(iImport, iExport, 
				tMyAS, 
				tPeerAS,
				pcPeerAutNum,	
				pcApp->pcDefText, iPeerASType);
	 break;
      case 3: // Shouldn't happen here since it's just a separator!
	 break;
      default:
	 {
//	 pcApp->pcNeighborText->clear();
	 generateTemplatePolicy(iCategory, iImport, iExport, 
				pzcMyAS, pzcPeerAS, pcApp->pcDefText);
	 break;
	 }
      }

   // Color syntaxing before leaving
   if (!pcApp->evalf("aoe:colorSyntax %s", pcApp->pcDefText->getName()))
     return TCL_ERROR;
   if (!pcApp->evalf("aoe:colorSyntax %s", pcApp->pcNeighborText->getName()))
     return TCL_ERROR;

   return TCL_OK;
}


int LoadBgpDump::command(int argc, char *argv[])
{
   if (argc != 2) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   // Call lexer 
   extern int loadBgpDump(BgpASPath &);  // Declared in bgp_dump.l
   extern FILE *bgp_dumpin;
   FILE *fpIn = fopen(argv[1], "r");
   if (!fpIn) return TCL_ERROR;
   bgp_dumpin = fpIn;
   loadBgpDump(pcApp->cBgpASPath);
   fclose(fpIn);
   // Duplicate first to last if last is empty
   pcApp->cBgpASPath.verify();
   //   pcApp->cBgpASPath.print();
   // Try to fill the result of BGP dump into AS Peer list
   if (pcApp->pcASPeerList->size()) pcApp->pcASPeerList->clear();
   (*pcApp->pcASPeerList << (pcApp->cASPeers << pcApp->cBgpASPath));
   // Assume the first peer is selected at the first place
   if (pcApp->pcASPeerList->size()) pcApp->pcASPeerList->select(0, 0);
   // Do the bootstrap action
   if (!pcApp->bootStrap()) return TCL_ERROR;
   return TCL_OK;
}

int UpdatePolicy::command(int argc, char *argv[])
{
   if (argc != 3) return TCL_ERROR;
   char *pzcOption = argv[1];
   int iTarget = atoi(argv[2]);
   int iResult = TCL_ERROR;
   if (strcmp(pzcOption, "append") == 0)
      iResult = append();
   else
      if (strcmp(pzcOption, "replace") == 0)
	 iResult = replace(iTarget);
   return iResult;
}

int UpdatePolicy::append(void)
{
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   pcApp->pcPolicyText->insert("\n");
   *pcApp->pcPolicyText << *pcApp->pcDefText;

   // Reparse it
   TCL(!pcApp->reparseWorkingAutNumArea());

   return TCL_OK;
}

int UpdatePolicy::replace(int iTarget)
{
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   ASPeer *pcASPeer;
   int i;
   for (i = 0, pcASPeer = pcApp->cASPeers.head(); 
	i < iTarget && pcASPeer; 
	i++, pcASPeer = pcApp->cASPeers.next(pcASPeer)) ;
   if (pcASPeer)
     {
     ASt tPeerAS = pcASPeer->getNo();   
     pcApp->pcAutNum->removePeer(tPeerAS);
     *pcApp->pcPolicyText << *pcApp->pcAutNum; // Redisplay
     }
   return append();
}

/*
int DeletePolicyText::command(int argc, char *argv[])
{
   if (argc != 1) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   if (!pcApp->pcPolicyText->removeSelectedText()) return TCL_ERROR;
   return TCL_OK;
}
*/

int EditPolicyText::command(int argc, char *argv[])
{
   if (argc != 1) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   if (pcApp->pcPolicyEditButton->getOption())
      {
      if (!pcApp->pcPolicyText->enable()) return TCL_ERROR;
      if (!pcApp->pcPolicyText->focus()) return TCL_ERROR;
      }
   else 
      {
      if (!pcApp->pcPolicyText->disable()) return TCL_ERROR;
      if (!pcApp->pcPolicyText->unfocus()) return TCL_ERROR;
      if (!pcApp->reparseWorkingAutNumArea()) return TCL_ERROR;
      }
   return TCL_OK;
}

int ShowPolicyText::command(int argc, char *argv[])
{
   if (argc != 1) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   if (pcApp->pcPolicyShowButton->getOption())
      {
      if (pcApp->cASPeers.head())
	 {
	 int iActiveItem = pcApp->pcASPeerList->getCurrentItem();
	 if (iActiveItem >= 0)
	    {
	    const AutNum *pcPeerAutNum = 
	      pcApp->pcIrr->getAutNum(pcApp->getASPeerNo(iActiveItem));
	    pcApp->pcPolicyText->clear();
	    // Might be NULL here!
	    if (pcPeerAutNum) *pcApp->pcPolicyText << *pcPeerAutNum;
	    // Disable improper buttons
	    //	    pcApp->pcPolicyDeleteButton->disable();
	    pcApp->pcPolicyEditButton->disable();
	    pcApp->pcCommitButton->disable();
	    }
	 else
	   {
	   pcApp->pcPolicyShowButton->toggle();
	   if (!pcApp->eval("showWarning {Please point the peer " 
			    "you are going to display!}")) 
	      return TCL_ERROR;
	   }
	 }
      else
	 {
	 pcApp->pcPolicyShowButton->toggle();
	 if (!pcApp->eval("showWarning {No peer to be displayed!}"))
	    return TCL_ERROR;
	 }
      }
   else
      {
      pcApp->pcPolicyText->clear();
      *pcApp->pcPolicyText << *pcApp->pcAutNum;
      // Enable some proper buttons
      //      pcApp->pcPolicyDeleteButton->enable();
      pcApp->pcPolicyEditButton->enable();
      pcApp->pcCommitButton->enable();
      }
   return TCL_OK;
}

int FileOpen::command(int argc, char *argv[])
{
   if (argc != 2) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   FILE *fp;
   int iResult = TCL_OK;
   struct stat sFileStat;
   // Get size of the file
   if (stat(argv[1], &sFileStat)) return TCL_ERROR;
   // Open the file for reading
   if ((fp = fopen(argv[1], "r")) == NULL) return TCL_ERROR;
   char *pzcBuffer = new char [sFileStat.st_size + 1];
   // Read the whole thing
   if (fread(pzcBuffer, 1, sFileStat.st_size, fp) != sFileStat.st_size)
     iResult = TCL_ERROR;
   fclose(fp);
   pzcBuffer[sFileStat.st_size] = 0;
   if (iResult == TCL_OK)
     {
     // Delete original one
     if (pcApp->pcAutNum) delete pcApp->pcAutNum;
     Buffer b(pzcBuffer, sFileStat.st_size);
     if ((pcApp->pcAutNum = new AutNum(b)) == NULL)
       iResult = TCL_ERROR;
     else
       // Refill policy text widget
       *pcApp->pcPolicyText << *pcApp->pcAutNum;
     }
   return iResult;
}

int FileSave::command(int argc, char *argv[])
{
   if (argc != 2) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   FILE *fp;
   if ((fp = fopen(argv[1], "w")) == NULL) return TCL_ERROR;
   fputs(pcApp->pcPolicyText->getAll(), fp);
   fclose(fp);
   return TCL_OK; 
}  

// by katie@ripe.net for saving the peer list with status
int ListSave::command(int argc, char *argv[])
{  
   if (argc != 2) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   FILE *fp;
   if ((fp = fopen(argv[1], "w")) == NULL) return TCL_ERROR;

   // include AS number in report
   fputs ("ASNo = ", fp);
   fputs (pcApp->getASNoInString(), fp);
   fputs ("\n", fp);
   // my peer's
   SortedList<ASPeer> peers = pcApp->cASPeers;
   // put peer list to the file
   fp << peers;
   fclose(fp);
   return TCL_OK; 
}  


int FileRevert::command(int argc, char *argv[])
{
   if (argc != 1) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   // Delete original one first
   if (pcApp->pcAutNum) delete pcApp->pcAutNum;
   // Clear the peer list first
   pcApp->cASPeers.clear();       // Clear the list from memory
   pcApp->pcASPeerList->clear();  // Clear the list from Tk
   // Then ask the parser requery the AutNum object and redisplay the peer list
   pcApp->getASInfoFromServer();
   pcApp->bootStrap();
   return TCL_OK;
}

int FilePrint::command(int argc, char *argv[])
{   
   if (argc != 2) return TCL_ERROR;
   char *pzcCommand = argv[1];
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   const char *pzcText;
   int iLen;
   FILE *fp;
   if ((fp = popen(pzcCommand, "w")) == NULL)
      {
      TCL(!pcApp->evalf("showError {Open pipe error with command: %s}", 
			pzcCommand));
      return TCL_ERROR;
      } 
   pzcText = pcApp->pcPolicyText->getAll();
   iLen = strlen(pzcText);
   if (fwrite(pzcText, 1, iLen, fp) != iLen) 
      {
      TCL(!pcApp->evalf("showError {Printing error!}"));
      return TCL_ERROR; 
      }   
   pclose(fp);
   TCL(!pcApp->evalf("showInfo {Printing is done!}"));
   return TCL_OK;
}


int AddPeer::command(int argc, char *argv[])
{
   if (argc != 2) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   // Allocate a new peer
   ASPeer *pcASPeer = new ASPeer(atoi(argv[1] + 2), dASPeerNew);
   if (!pcASPeer) return TCL_ERROR;
   pcApp->cASPeers.append(pcASPeer);
   // Clear the aspeer list
   if (!pcApp->pcASPeerList->clear()) TCL_ERROR;
   // Refill the AS Peer list
   *pcApp->pcASPeerList << pcApp->cASPeers;   
   return TCL_OK;
}

int DeletePeer::command(int argc, char *argv[])
{
   if (argc != 2) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   // Check if the list is empty of not,
   if (!argv[1][0]) 
      {
      if (pcApp->cASPeers.head())
	 {	
	 if (!pcApp->eval("showWarning {Please point the peer " 
			  "you are going to delete!}")) 
	 return TCL_ERROR;
	 }
      else
	 {
	 if (!pcApp->eval("showWarning {Nothing to be deleted!}")) 
	    return TCL_ERROR;
	 }
      return TCL_OK;
      }
   ASPeer *pcASPeer;
   int i, iTarget = atoi(argv[1]);
   for (i = 0, pcASPeer = pcApp->cASPeers.head(); 
	i < iTarget && pcASPeer; 
	i++, pcASPeer = pcApp->cASPeers.next(pcASPeer)) ; //For loop ends here
   if (pcASPeer)
     {
     pcApp->pcAutNum->removePeer(pcASPeer->getNo());
     // Redisplay the contain of AutNum in policy text widget
     *pcApp->pcPolicyText << *pcApp->pcAutNum;
     // Remove the peer AS from the list
     pcApp->cASPeers.remove(pcASPeer);
     delete pcASPeer;
     // Clear the aspeer list
     if (!pcApp->pcASPeerList->clear()) TCL_ERROR;
     // Refill the AS Peer list
     *pcApp->pcASPeerList << pcApp->cASPeers;      
     // Clear the corresponding text widgets
     if (!pcApp->pcDefText->clear()) return TCL_ERROR;
     if (!pcApp->pcNeighborText->clear()) return TCL_ERROR;
     if (!pcApp->pcNeighborLabel->attach("Empty")) return TCL_ERROR;
     pcApp->pcPolicyShowButton->labelBothString("Empty");
     }

   // Take care of show policy button
   if (pcApp->pcPolicyShowButton->getOption())
     {
     pcApp->pcPolicyShowButton->toggle();
     if (!pcApp->eval("showPolicyText")) return TCL_ERROR;
     }
   return TCL_OK;
}


int CreatePolicyTemplateMenu::command(int argc, char *argv[])
{
   if (argc != 1) return TCL_ERROR;
   AoeApplication *pcApp = (AoeApplication *)getExtraArgument();
   // Delete menu first
   for (int i = pcApp->pcCategoryButton->getSize(); i > 4; i--)
      pcApp->pcCategoryButton->del(i);
   // Create menu
   char *pzcTotal = pcApp->getVar("PolicyTemplate(Total)");
   if (pzcTotal)
      {
      for (int i = 0; i < atoi(pzcTotal); i++)
	 {
	 char pzcBuffer[256];
	 sprintf(pzcBuffer, "PolicyTemplate(%d,Description)", i);
	 if (!pcApp->pcCategoryButton->add(pcApp->getVar(pzcBuffer), 
	      "listPeer [.body.aspeer.listbox.list curselection]"))
	 return TCL_ERROR;
	 }
      }
   return TCL_OK;
}


//******************************
// Application class starts here

#include "gnug++/SetOfUInt.hh"

AoeApplication::AoeApplication(char *pzcAppName, 
			       char *pzcDisplay,
			       char *pzcDefaultResource,
			       char *pzcResourceFile,
			       ASt tASNo) :
   TclApplication(pzcAppName, pzcDisplay, pzcDefaultResource, pzcResourceFile),
   tASNo(tASNo),
   pcAutNum(NULL),
   pcASPeerList(NULL),
   pcPolicyText(NULL),
   pcDefText(NULL),
   pcNeighborText(NULL),
   pcImportButton(NULL),
   pcExportButton(NULL),
   pcCategoryButton(NULL),
   pcDefLabel(NULL),
   pcNeighborLabel(NULL),
//   pcPolicyDeleteButton(NULL),
   pcCommitButton(NULL),
   pcPolicyEditButton(NULL),
   pcPolicyShowButton(NULL),
   pcStatusLine(NULL)
{
  sprintf(pzcASNo, "AS%d", tASNo);
  if (!(pcIrr = IRR::newClient()))
    {
    usage();
    exit(1);
    }
//  pcIrr->setFastResponseFlag(false);
}

AoeApplication::~AoeApplication(void)
{
   // Save settings into resource file before program ends
   evalf("aoe:saveResource {%s} {%s}", getResourceFile(), ProjectVersion);
   // Free allocate memory
   if (!cASPeers.isEmpty()) cASPeers.clear();
   if (pcASPeerList) delete pcASPeerList;
   if (pcPolicyText) delete pcPolicyText;
   if (pcDefText) delete pcDefText;
   if (pcNeighborText) delete pcNeighborText;
   if (pcImportButton) delete pcImportButton;
   if (pcExportButton) delete pcExportButton;
   if (pcCategoryButton) delete pcCategoryButton;
   if (pcDefLabel) delete pcDefLabel;
   if (pcNeighborLabel) delete pcNeighborLabel;
//   if (pcPolicyDeleteButton) delete pcPolicyDeleteButton;
   if (pcCommitButton) delete pcCommitButton;
   if (pcPolicyEditButton) delete pcPolicyEditButton;
   if (pcPolicyShowButton) delete pcPolicyShowButton;
   if (pcStatusLine) delete pcStatusLine;
   if (pcAutNum) delete pcAutNum;
   if (pcIrr) delete pcIrr;
}

int AoeApplication::getASInfoFromServer(void)
{
   const AutNum *pcAutNumTemp = pcIrr->getAutNum(tASNo);
   if (!pcAutNumTemp) return 0;

   // Get a fresh copy of aut-num object
   pcAutNum = new AutNum(*pcAutNumTemp);

   // Get Peer AS from import/export/default attributes
   /*for (ASt tPeerAS = pcAutNum->firstPeerAS(); 
	tPeerAS != INVALID_AS;
	tPeerAS = pcAutNum->nextPeerAS())
     cASPeers.append(new ASPeer(tPeerAS, dASPeerFromIRR));
   */
   
   // Get Peer AS from import/export/default attributes - reimp
    irr = pcIrr; // for AutNumPeerinIterator
    AutNumPeeringIterator itr(pcAutNum);
    for (const Peering *peering = itr.first(); peering; peering = itr.next())	
         // no dups to avoid dups in peer list
        cASPeers.insertSortedNoDups(new ASPeer(peering->peerAS, dASPeerFromIRR));
   // Fill out the AS Peer list
   *pcASPeerList << cASPeers;

   // Assume the first peer is selected at the first place
   if (pcASPeerList->size()) pcASPeerList->select(0, 0);

   // Fill out the policy text widget for users reference
   *pcPolicyText << *pcAutNum;

   // Insert peers into the peer list
   //   for (Pix pix = cPeers.first(); pix; cPeers.next(pix))
   //      evalf("%s insert end \"%s\"", tkASPeerList, AS_map(cPeers(pix)));

   return 1;
}

ASt AoeApplication::getASPeerNo(int i)
{
   ASPeer *pcASPeer;
   int iIndex;
   for (pcASPeer = cASPeers.head(), iIndex = 0;
	pcASPeer;
	pcASPeer = cASPeers.next(pcASPeer), iIndex++)
      if (iIndex == i) break;
   if (pcASPeer) return pcASPeer->getNo();
   // Shouldn't come to here
   return 0;
}

int AoeApplication::getASPeerType(int i)
{
   ASPeer *pcASPeer;
   int iIndex;
   for (pcASPeer = cASPeers.head(), iIndex = 0;
	pcASPeer;
	pcASPeer = cASPeers.next(pcASPeer), iIndex++)
      if (iIndex == i) break;
   if (pcASPeer) return pcASPeer->getType();
   // Shouldn't come to here
   return dASPeerUnknown;   
}

int AoeApplication::bootStrap(void)
{
   // Create a menu for policy template
   if (!evalf("createPolicyTemplateMenu")) return False;
   // Setup $MyAS properly
   if (!evalf("set MyAS %s", getASNoInString())) return False;
   // Select the fist one
   if (!evalf("listPeer [%s curselection]", tkASPeerList)) return False;
   return True;
}

int AoeApplication::reparseWorkingAutNumArea(void)
{
   if (pcAutNum) delete pcAutNum;
   char *pzcPolicyText = strdup(pcPolicyText->getAll());
   Buffer b(pzcPolicyText, strlen(pzcPolicyText));
   pcAutNum = new AutNum(b);
   // Refill policy text widget
   *pcPolicyText << *pcAutNum;
   // added by katie@ripe.net to show error window
   strstream ss;
   pcAutNum->reportErrors(ss);
   ss << '\0';
   if (pcAutNum->has_error && !evalf("showError {%s}", ss.str())) return False;
   return 1;
}

int AoeApplication::init(void)
{
   // Need to call init() of base class first, since it does all the
   // Tcl_Init() & Tk_Init() stuff
   if (!TclApplication::init()) return 0;

   // eval() is only working after Tcl_Init()

   // Load the file select module from fileselect.tk
   extern char *fileselect_tk;
   if (!eval(fileselect_tk)) return 0;

   // Load the utils module from utils.tk
   extern char *utils_tk;
   if (!eval(utils_tk)) return 0;

   // Load the layout converted from layout.tk
   // Should be the last one to be loaded
   extern char *layout_tk;
   if (!eval(layout_tk)) return 0;

   // Initiate bitmap
   Tk_DefineBitmap(getInterp(), Tk_GetUid("aoe_logo"), 
		   aoe_bits, aoe_width, aoe_height);
   if (!evalf("%s configure -bitmap aoe_logo", tkLogoButton)) return 0;

   // Display something first
   if (!eval("update")) return 0;

   // Create Tcl commands here
   if (!createCommand(new ListPeer("listPeer"))) return 0;
   if (!createCommand(new LoadBgpDump("loadBgpDump"))) return 0;
   if (!createCommand(new UpdatePolicy("updatePolicy"))) return 0;
//   if (!createCommand(new DeletePolicyText("deletePolicyText"))) return 0;
   if (!createCommand(new EditPolicyText("editPolicyText"))) return 0;
   if (!createCommand(new ShowPolicyText("showPolicyText"))) return 0;
   if (!createCommand(new FileOpen("fileOpen"))) return 0;
   if (!createCommand(new FileSave("fileSave"))) return 0;
   if (!createCommand(new ListSave("listSave"))) return 0; // peers save
   if (!createCommand(new FileRevert("fileRevert"))) return 0;
   if (!createCommand(new FilePrint("filePrint"))) return 0;
   if (!createCommand(new AddPeer("addPeer"))) return 0;
   if (!createCommand(new DeletePeer("deletePeer"))) return 0;
   if (!createCommand(
	   new CreatePolicyTemplateMenu("createPolicyTemplateMenu")))
      return 0;

   // Create Widgets (operations) here
   pcASPeerList = new TclList(tkASPeerList);
   if (!insert(pcASPeerList)) return 0;

   pcPolicyText = new TclText(tkPolicyText);
   if (!insert(pcPolicyText)) return 0;

   pcDefText = new TclText(tkDefText);
   if (!insert(pcDefText)) return 0;

   pcNeighborText = new TclText(tkNeighborText);
   if (!insert(pcNeighborText)) return 0;

   pcImportButton = new TclCheckButton(tkImportButton, cbCheck);
   if (!insert(pcImportButton)) return 0;

   pcExportButton = new TclCheckButton(tkExportButton, cbCheck);
   if (!insert(pcExportButton)) return 0;

   pcCategoryButton = new TclRadioButton(tkCategoryButton, tkCategoryVar, 0);
   if (!insert(pcCategoryButton)) return 0;

   pcDefLabel = new TclLabel(tkDefLabel);
   if (!insert(pcCategoryButton)) return 0;
   if (!pcDefLabel->setColor("red")) return 0;

   pcNeighborLabel = new TclLabel(tkNeighborLabel);
   if (!insert(pcNeighborLabel)) return 0;
/*
   pcPolicyDeleteButton = new TclButton(tkPolicyDeleteButton);
   if (!insert(pcPolicyDeleteButton)) return 0;
*/
   pcCommitButton = new TclButton(tkCommitButton);
   if (!insert(pcCommitButton)) return 0;

   pcPolicyEditButton = new TclToggleButton(tkPolicyEditButton,
					    "Edit", "Edit Done!",
					    cbUnCheck);
   if (!insert(pcPolicyEditButton)) return 0;

   pcPolicyShowButton = new TclToggleButton(tkPolicyShowButton,
//					    getASNoInString(), "Peer AS", 
					    "Peer AS", "Peer AS", 
					    cbUnCheck);
   if (!insert(pcPolicyShowButton)) return 0;

   pcStatusLine = new TclLabel(tkStatusLine);
   if (!insert(pcStatusLine)) return 0;

   getASInfoFromServer();

   // Bootstrap something here
   if (!pcDefLabel->attach(pzcASNo)) return 0;
   if (!pcStatusLine->detach()) return 0;
   if (!bootStrap()) return 0;

   return 1;
}

// The reason I overide run() is to let main() looks better  :-)
int AoeApplication::run(void)
{
   if (!TclApplication::run())
      {
      fprintf(stderr, "%s", getResult());
      exit(1);
      }
   return 1;
}

// Main program
int main(int argc, char *argv[], char *envp[])
{
   schema.initialize();
   init_and_set_options(argc, argv, envp);
   if (!opt_rcfile) opt_rcfile = "~/.aoerc";
   extern char *aoerc_tk;
   AoeApplication cAoeApplication("aoe", display, aoerc_tk, opt_rcfile, myAS);
   cAoeApplication.run();
   return 0;
}
