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


#ifndef AOE_HH
#define AOE_HH

#include "TclApp.hh"
#include "TclList.hh"
#include "TclText.hh"
#include "TclCheckButton.hh"
#include "TclRadioButton.hh"
#include "TclLabel.hh"
#include "TclToggleButton.hh"
#include "TclButton.hh"
#include "TclEntry.hh"
#include "as_peer.hh"
#include "bgp_aspath.hh"
#include "irr/autnum.hh"   // To include the def. of class AutNum
#include "irr/rawhoisc.hh"

#define dFilenameLength 127
#define dBufferLength   127

//***************************
// Command classes start here
class ListPeer : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   private:
      int updateWindowTitle(TclApplication *pcApp, int iCategory, 
			    char *pzcASNo);
      int displayIRRPolicy(int iImport, int iExport, ASt tAS,
			   TclText *pcText, const AutNum *pcAutNum);
      int generatePolicyFromBGP(int iImport, int iExport,
				ASt tMyAS, ASt tPeerAS, TclText *pcText, 
				BgpASPath *pcBgpASPath, int iPeerASType);
      int generatePolicyFromPeer(int iImport, int iExport,
				 ASt tMyAS, 
				 ASt tPeerAS, const AutNum *pcPeerAutNum, 
				 TclText *pcText,
				 int iPeerASType);
      int generateTemplatePolicy(int iCategory, int iImport, int iExport,
				 char *pzcMyAS, char *pzcPeerAS, 
				 TclText *pcText);
      char *substitute(char *pzcString, char *pzcMyAS, char *pzcPeerAS);

   public:
      ListPeer(char *pzcName) : TclCommand(pzcName) {}
};


class LoadBgpDump : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      LoadBgpDump(char *pzcName) : TclCommand(pzcName) {}
};


class UpdatePolicy : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   private:
      int append(void);
      int replace(int iTarget);
      
   public:
      UpdatePolicy(char *pzcName) : TclCommand(pzcName) {}
};

/*
class DeletePolicyText : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      DeletePolicyText(char *pzcName) : TclCommand(pzcName) {} 
};
*/

class EditPolicyText : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      EditPolicyText(char *pzcName) : TclCommand(pzcName) {} 
};

class ShowPolicyText : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      ShowPolicyText(char *pzcName) : TclCommand(pzcName) {}
};

class FileOpen : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      FileOpen(char *pzcName) : TclCommand(pzcName) {} 
};

class FileSave : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      FileSave(char *pzcName) : TclCommand(pzcName) {} 
};

class ListSave : public TclCommand // katie@ripe.net for peers save
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      ListSave(char *pzcName) : TclCommand(pzcName) {} 
};

class FileRevert : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      FileRevert(char *pzcName) : TclCommand(pzcName) {} 
};

class FilePrint : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      FilePrint(char *pzcName) : TclCommand(pzcName) {} 
};


class AddPeer : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      AddPeer(char *pzcName) : TclCommand(pzcName) {} 
};


class DeletePeer: public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      DeletePeer(char *pzcName) : TclCommand(pzcName) {} 
};


class CreatePolicyTemplateMenu : public TclCommand
{
   protected:
      virtual int command(int argc, char *argv[]);

   public:
      CreatePolicyTemplateMenu(char *pzcName) : TclCommand(pzcName) {} 
};


//******************************
// Application class starts here
class AoeApplication : public TclApplication
{
   private:
      char pzcASNo[32];
      ASt tASNo;

   public:
      IRR *pcIrr;
      AutNum *pcAutNum;
      TclList *pcASPeerList;
      TclText *pcPolicyText, *pcDefText, *pcNeighborText;
      TclCheckButton *pcImportButton, *pcExportButton;
      TclRadioButton *pcCategoryButton;
      TclLabel *pcDefLabel, *pcNeighborLabel;
//      TclButton *pcPolicyDeleteButton, *pcCommitButton;
      TclButton *pcCommitButton;
      // katie@ripe.net
      TclButton *pcCheckButton;
      TclToggleButton *pcPolicyEditButton, *pcPolicyShowButton;
      TclRadioButton *pcPolicyTemplateRadioButton;
      TclLabel *pcStatusLine;
      SortedList<ASPeer> cASPeers;
      BgpASPath cBgpASPath;

   public:
      AoeApplication(char *pzcAppName, char *pzcDisplay, 
		     char *pzcDefaultResource, char *pzcResourcefile, 
		     ASt tASNo);
      ~AoeApplication(void);
      virtual int init(void);
      virtual int run(void);
      ASt getASNo(void) const { return tASNo; }
      const char *getASNoInString(void) const { return pzcASNo; }
      ASt getASPeerNo(int i);
      int getASPeerType(int i);
      int bootStrap(void);
      int createPolicyTemplateMenu(void);
      int reparseWorkingAutNumArea(void);
      int getASInfoFromServer(void);
};

#endif
