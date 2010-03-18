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
