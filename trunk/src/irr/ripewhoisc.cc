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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include "ripewhoisc.hh"
#include "route.hh"
#include "autnum.hh"
#include "classes.hh"
#include "rpsl/symbols.hh"   // For symbols.symID()
#include "util/version.hh"
#include "gnug++/Regex.h"
#include "util/trace.hh"

#define NO_OF_TRIAL_TRY		3

RipeWhoisClient::RipeWhoisClient(void) :
  Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP),
  pzcSourceOriginReply(NULL)
{
  Open(dflt_host, dflt_port, dflt_sources);
}

RipeWhoisClient::RipeWhoisClient(const char *host, 
                                 const int port,
                                 const char *sources = dflt_sources) :
  Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP),
  pzcSourceOriginReply(NULL)
{
  Open(host, port, sources);
}

void RipeWhoisClient::Open(const char *_host = dflt_host, 
                           const int _port = dflt_port, 
                           const char *_sources = dflt_sources)
{
  Trace(TR_WHOIS_QUERY) << "Whois: Open " 
			<< _host << ":" << _port 
			<< " source=" << _sources 
			<< " protocol=" << protocolName[dflt_protocol].name
			<< endl;

  ipAddr cServer(_host);
  set_dstInaddr(cServer.getInaddr());
  setPort(_port);
  if (connect() < 0) 
    {
    cerr << "Connection to " << _host << ":" << _port << " failed!" << endl;
    exit(1);
    }
  // whois server stuff rather than the socket stuff
  strcpy(host, _host);
  if (_sources && *_sources) {
     strcpy(sources, "-s ");
     strcat(sources, _sources);
  } else
     *sources = 0;
  port = _port;
}

void RipeWhoisClient::Close()
{
  Trace(TR_WHOIS_QUERY) << "Whois: Close" << endl;
  write("\n", 1);
}

static int lastiNoofTry;
static char *lastBuf;
static int lastLen;

void RipeWhoisClient::recoverFromSleepingServerBug()
{
   cerr << "Warning: server disappeared, re-establishing the connection..." 
	<< endl;
   newSock();
   Open();
   write(lastiNoofTry, lastBuf, lastLen);
}


// 1. Have to do the buffer myself 
// 2. Have to look for \n\n\n for the end of each respond 
//    (might have multiple responds)
char *RipeWhoisClient::read(void)
{
  char pzcBuffer[PROBE_BUFFER_SIZE];
  char *pzcEor = "\n\n\n";
  char *pzcReturn = NULL;          // Must be NULL first
  int iByteRead;
  unsigned long ulSearchResult;
  if (!cProbeBuffer.empty()  // if something in the probe buffer
      && (ulSearchResult = cProbeBuffer.find(pzcEor)) <= cProbeBuffer.length())
    ; // No statement here
  else
    {
    int iNoofTry = 0;
    while (1)
      {
      iByteRead = Socket::read(pzcBuffer, PROBE_BUFFER_SIZE);
      if (iByteRead < 0)
	 if (++iNoofTry > NO_OF_TRIAL_TRY)
	    return NULL;
	 else
	    continue;
      else if (iByteRead == 0)
	 if (++iNoofTry > NO_OF_TRIAL_TRY) {
	    recoverFromSleepingServerBug();
	    cProbeBuffer.clear();
	    iNoofTry = 0;
	    continue;
	 } else
	    continue;
      cProbeBuffer.append(pzcBuffer, iByteRead);
      if ((ulSearchResult = cProbeBuffer.find(pzcEor))<= cProbeBuffer.length())
	break;
      }
    }

  // At least found one \n\n\n
  ulSearchResult++;  // the first \n belongs to the data itself
  pzcReturn = (char *)malloc(ulSearchResult + 1);
  strncpy(pzcReturn, cProbeBuffer.c_str(), ulSearchResult);
  pzcReturn[ulSearchResult] = 0;
  cProbeBuffer.remove(ulSearchResult + 2);

  Trace(TR_WHOIS_RESPONSE) << "Whois: Response (" << sock << ") " 
			   << pzcReturn << endl;

  return pzcReturn;
}

// Modified from Rusty's Socket class
int RipeWhoisClient::write(int iNoofTry, char *buf, int len)
{
   lastiNoofTry = iNoofTry;
   lastBuf = buf;
   lastLen = len;

    char *b = (char *) buf;
    int l = len;
    while (l > 0) {
        // Make it non-blocking then
        int iFlags = ::fcntl(sock, F_GETFL, 0);
#if defined(O_NONBLOCK)
	::fcntl(sock, F_SETFL, iFlags | O_NONBLOCK);
#else
	::fcntl(sock, F_SETFL, iFlags | O_NDELAY);
#endif
        // Checking for closed connection from the server
	char pzcOneByte;
        int iRead = ::read(sock, &pzcOneByte, 1);
	// Restore it back to blocking
	::fcntl(sock, F_SETFL, iFlags);
	if (iRead == 1) cProbeBuffer.append(pzcOneByte);
	if (iRead == 0 && errno != EAGAIN)
	  {
	  newSock();
	  Open();
	  }

	// Do a real write here
        int c = Socket::write (b, l);
	if (c < 0) 
	  {
	  if (--iNoofTry <= 0)
	    {
	    cerr << "Error(" 
		 << errno 
		 << "): RipeWhoisClient::write()"
		 << endl;
	    return c;
	    }
	  continue;
	  }
	l -= c;
	b += c; 
    }
    return (len - l);
}

int RipeWhoisClient::write(char *buf, int len)
{
  Trace(TR_WHOIS_QUERY) << "Whois: WriteQuery (" << sock << ") " 
			<< buf << endl;

  return write(NO_OF_TRIAL_TRY, buf, len);  // Try 3 times before giving up
}

bool RipeWhoisClient::getObject(char *pzcToken, char *pzcCommand, 
                                char *&text, int &len)
{
  char *pzcReply;
  bool bResult = false;
  // Initial return value first
  text = NULL;
  len = 0;
  if (write(pzcCommand, strlen(pzcCommand)) < 0) return false;
  if ((pzcReply = read()) != NULL)
    {
    char *p = strstr(pzcReply, pzcToken);
    if (p)
      {
      p++;  // Skip the "\n"
      len = strlen(p);
      text = new char[len + 1];
      strcpy(text, p);
      bResult = true;
      }
    free(pzcReply);  // Corresponding to realloc() from RipeWhoisClient::read()
    }
  return bResult;
}

bool RipeWhoisClient::getLine(char *pzcToken, char *pzcCommand, 
                              char *&text, int &len)
{
  char *pzcReply;
  bool bResult = false;
  // Initial return value first
  text = NULL;
  len = 0;
  if (write(pzcCommand, strlen(pzcCommand)) < 0) return false;
  if ((pzcReply = read()) != NULL)
    {
    char *p = strstr(pzcReply, pzcToken);
    if (p)
      {
      p += strlen(pzcToken);        // Skip the "pzcToken"
      while (*p && *p == ' ') p++;  // Skip heading spaces
      len = strlen(p);
      text = new char[len + 1];
      strcpy(text, p);
//      cout << "***" << text << "***" << endl;
      bResult = true;
      }
    free(pzcReply);  // Corresponding to realloc() from RipeWhoisClient::read()
    }
  return bResult;
}


bool RipeWhoisClient::getAutNum(char *as,          char *&text, int &len)
{
  char pzcCommand[256], *pzcToken = "\naut-num:";
  // Might be buffer overrun here!!!
  sprintf(pzcCommand, 
	  "-V %s --single --silent -k -r %s -u -T aut-num %s\n",
          ProjectVersion2, sources, as);
  return getObject(pzcToken, pzcCommand, text, len);
}

bool RipeWhoisClient::getSet(SymID sname, char *clss, char *&text, int &len) {
   char *pzcReply;
   char pzcCommand[256], pzcToken[256];
   pzcToken[0] = '\n';
   strcpy(pzcToken+1, clss);
   // Might be buffer overrun here!!!
   sprintf(pzcCommand, 
	   "-V %s --single --silent -k -r %s -u -T %s %s\n",
	   ProjectVersion2, sources, clss, sname);
   return getObject(pzcToken, pzcCommand, text, len);
}

bool RipeWhoisClient::getRoute(char *rt, char *as, char *&text, int &len)
{
  char pzcCommand[256], *pzcToken = "\nroute:";
  // Might be buffer overrun here!!!
  sprintf(pzcCommand, 
	  "-V %s --single --silent -k -r %s -u -T route %s\t%s\n",
          ProjectVersion2, sources, rt, as);
  return getObject(pzcToken, pzcCommand, text, len);
}

bool RipeWhoisClient::getInetRtr(SymID inetrtr,    char *&text, int &len)
{
  char pzcCommand[256], *pzcToken = "\ninet-rtr:";
  // Might be buffer overrun here!!!
  sprintf(pzcCommand, 
	  "-V %s --single --silent -k -r %s -u -T inet-rtr %s\n",
          ProjectVersion2, sources, inetrtr);
  return getObject(pzcToken, pzcCommand, text, len);
}

void RipeWhoisClient::parseRoutes(const char *pzcReply, 
				  PrefixRanges *pzcResult)
{
  char *pzcToken = "\nroute:";
  if (pzcReply != NULL)
    {
    char *p = strstr(pzcReply, pzcToken);
    if (p)
      {
      p++;
      // 1: use fastmap
      Regex cRegex("[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+\\/[0-9]+", 1);  
      if (cRegex.OK())
        {
        int iMatchLen = 0;
        int iMatchPos = 0;
        int iLen = strlen(p);
        char zcTempChar;
        while ((iMatchPos = 
		cRegex.search(p, iLen, iMatchLen, iMatchPos + iMatchLen)) >= 0)
          {
//          printf("***Len = %d, Pos = %d***\n", iMatchLen, iMatchPos);
          zcTempChar = p[iMatchPos + iMatchLen];
          p[iMatchPos + iMatchLen] = 0;
	  pzcResult->add_high(PrefixRange(p + iMatchPos));
          p[iMatchPos + iMatchLen] = zcTempChar;
          }
        }
      }
    }
}

bool RipeWhoisClient::expandAS(char *as,           PrefixRanges *result)
{
  char *pzcReply;
  char pzcCommand[256], *pzcToken = "\nroute:";
  // Might be buffer overrun here!!!
  sprintf(pzcCommand, 
	  "-V %s --silent -k -r %s -T route -o route -i origin %s\n",
          ProjectVersion2, sources, as);
  if (write(pzcCommand, strlen(pzcCommand)) < 0) return false;
  if ((pzcReply = read()) != NULL)
    {
    parseRoutes(pzcReply, result);
    free(pzcReply);
    }
  return true;
}

bool RipeWhoisClient::getIndirectMembers(const Object *o, 
					 char *query, char *token,
					 void (*collect)(void *, const Object *), void *collectArg) {
   AttrGenericIterator<ItemWORD> mbrs_by_ref(o, "mbrs-by-ref");
   if (! mbrs_by_ref)
      return false;

   if (write(query, strlen(query)) < 0)
      return false;

   char *response;
   int  len;
   bool isAny = false;
   if (! (response = read()))
      return false;

   // if mbrs_by_ref contains any, then we are done
   for (mbrs_by_ref.first(); mbrs_by_ref; mbrs_by_ref++)
      if (! strcasecmp(mbrs_by_ref()->word, "any")) {
	 isAny = true;
	 break;
      }

   // We need to omit members with wrong maintainers
   char *p1, *p2;
   int tokenLen = strlen(token);
   for (p1 = strstr(response, token); p1; p1 = p2) {
      if (p2 = strstr(p1 + tokenLen, token))
	 len = p2 - p1;
      else
	 len = strlen(p1) - 1;
      Buffer b(strdup(p1+1), len);
      Object *mbr = new Object(b, true);
      if (isAny || isIndirectMember(mbr, mbrs_by_ref))
	 (*collect)(collectArg, mbr);
      delete mbr;
   } 
}

bool RipeWhoisClient::expandRSSet(SymID sname, PrefixRanges *result) {
   const Set *set = IRR::getRSSet(sname);
   AttrGenericIterator<Item> itr(set, "members");
   for (Item *pt = itr.first(); pt; pt = itr.next())
      expandItem(pt, result);

   char buffer[1024];
   sprintf(buffer, 
	   "-V %s --silent -k -r %s -T route -o route,mnt-by "
	   "-i member-of %s\n",
	   ProjectVersion2, sources, sname);

   getIndirectMembers(set, buffer, "\nroute:", collectPrefix, result);
	  
   return true;
}

bool RipeWhoisClient::expandRtrSet(SymID sname, PrefixRanges *result) {
   const Set *set = IRR::getRtrSet(sname);
   AttrGenericIterator<Item> itr(set, "members");
   for (Item *pt = itr.first(); pt; pt = itr.next())
      expandItem(pt, result);

   char buffer[1024];
   sprintf(buffer, 
	   "-V %s --silent -k -r %s -T inet-rtr -o inet-rtr,ifaddr,mnt-by "
	   "-i member-of %s\n",
	   ProjectVersion2, sources, sname);

   getIndirectMembers(set, buffer, "\ninet-rtr:", collectIfAddr, result);
	  
   return true;
}

bool RipeWhoisClient::expandASSet(SymID sname, SetOfUInt *result) {
   const Set *set = IRR::getASSet(sname);
   AttrGenericIterator<Item> itr(set, "members");
   for (Item *pt = itr.first(); pt; pt = itr.next())
      if (typeid(*pt) == typeid(ItemASNAME)) { // ASNAME (aka as-set)
	 const SetOfUInt *tmp = IRR::expandASSet(((ItemASNAME *)pt)->name);
	 if (tmp) 
	    *result |= *(SetOfUInt *) tmp;
      } else if (typeid(*pt) == typeid(ItemASNO))
	 result->add(((ItemASNO *)pt)->asno);
      else
	 assert(false) ; ;

   char buffer[1024];
   sprintf(buffer, 
	   "-V %s --silent -k -r %s -T aut-num -o aut-num,mnt-by "
	    "-i member-of %s\n",
	   ProjectVersion2, sources, sname);

   getIndirectMembers(set, buffer, "\naut-num:", collectASNO, result);
	  
   return true;
}

int RipeWhoisClient::getSourceOrigin(char *&buffer, const char *rt)
{
  char *pzcReply;
  querySourceOrigin(rt);
  return getSourceOrigin(buffer);
}

/* Will parse things like below:

origin:        AS226
source:        RADB

origin:        AS52
source:        CW


into 

RADB  AS226
CW  AS52
*/
// Need to take care of multiple requests separated by \n\n\n
int RipeWhoisClient::getSourceOrigin(char *&buffer) 
{
  static char *pzcTempReply;       // A pointer to keep track the allocated 
                                   // memory from read() and free it later
                                   // after being used.
  StringBuffer cBuffer;
  char pzcOrigin[16], pzcSource[32], *pzcToken = "\norigin:";
  if (!pzcSourceOriginReply)
    {
    if ((pzcSourceOriginReply = read()) == NULL) return 0;
    pzcTempReply = pzcSourceOriginReply;
    }
  // To make sure that it starts with \norigin:
  if ((pzcSourceOriginReply = strstr(pzcSourceOriginReply, pzcToken)) == NULL)
    {
    free(pzcTempReply);
    return 0;
    }
  while (sscanf(pzcSourceOriginReply, 
		"\norigin: %s\nsource: %s\n", 
		pzcOrigin, pzcSource) == 2)
    {
    cBuffer.append("%s %s\n", pzcSource, pzcOrigin);
    if (pzcSourceOriginReply = strstr(pzcSourceOriginReply, "\n\n")) 
      {
      pzcSourceOriginReply++;        // Skip the first \n
      if (*(pzcSourceOriginReply + 1) == '\n') // Look for 3rd \n
	{
        pzcSourceOriginReply += 2;   // Skip the \n\n
	break;                       // That's it, will go on in the next call
	}
      // else still in a single query and go on the while loop
      }
    else
      {
      free(pzcTempReply);
      pzcSourceOriginReply = NULL;   // This is very important for next call
      break;
      }
    }
  if (cBuffer.empty()) 
    {
    buffer = NULL;
    return 0;
    }
  buffer = new char[cBuffer.length() + 1];
  strcpy(buffer, cBuffer.c_str());
  return cBuffer.length();
}

// Ask for Source and Origin of a given route and don't wait for respond yet
void RipeWhoisClient::querySourceOrigin(const char *rt)
{
  char pzcCommand[256];
  // Might be buffer overrun here!!!
  sprintf(pzcCommand, 
	  "-V %s --silent -k -r %s -T route -o origin,source -l %s\n",
	  ProjectVersion2, sources, rt);
//cout << "***" << pzcCommand << endl;
  if (write(pzcCommand, strlen(pzcCommand)) < 0)
    cerr << "Query source & origin failed: " << rt << endl;
}

ASt RipeWhoisClient::getOrigin(char *format, ...)
{
  char buffer[64];
  va_list ap;
  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);

  ASt tResult = 0;
  int iLen;
  char *pzcText;
  char pzcCommand[256], *pzcToken = "\nroute:";
  // Might be buffer overrun here!!!
  sprintf(pzcCommand, 
	  "-V %s --silent -k -r %s -T route %s\n",
	  ProjectVersion2, sources, buffer);
  if (getObject(pzcToken, pzcCommand, pzcText, iLen))
    {
    char *pzcObject = pzcText, *pzcNextObject;
    bool bLastObject = false;
    unsigned int uiMaxLen = 0;
    while (!bLastObject)
      {
      // Search for next object
      if (pzcNextObject = strstr(pzcObject, pzcToken))
	*pzcNextObject = 0;  // Yes, will keep parsing
      else
	bLastObject = true;  // OOPS, will terminate this loop after this parse
      // Parse it
      Buffer b(strdup(pzcObject), strlen(pzcObject));
      Route cRoute(b);
      // Look for the length of the route
      AttrGenericIterator<ItemPRFXV4> cRouteItr(&cRoute, "route");
      ItemPRFXV4 *pcRoute = cRouteItr.first();
      if (pcRoute && pcRoute->prfxv4)
	{
        // Eventually got it here!
	unsigned int uiLen = pcRoute->prfxv4->get_length();
	if (uiMaxLen < uiLen)
	  {
	  uiMaxLen = uiLen;
	  // Look for the AS origin
	  AttrGenericIterator<ItemASNO> cObjItr(&cRoute, "origin");
	  if (cObjItr)
	    tResult = cObjItr()->asno;
	  }
	}
      // Get new object position
      pzcObject = pzcNextObject + 1;  
      }
    delete [] pzcText;
    }
  return tResult;
}



#ifdef DEBUG_RIPEWHOISC

#include <iostream.h>
#include "autnum.hh"
#include "route.hh"

void getAutNum(int argc, char *argv[])
{
  IRR *pcClient = 
    new RipeWhoisClient("brind.isi.edu", 43);
  for (int i = 1; i < argc; i++)
    {
    const AutNum *pcAutNum = pcClient->getAutNum(atoi(argv[i]));
    cout << "\033[32m" << i << ": AS" << argv[i] << "\033[m" << endl;
    if (pcAutNum)
      {
      pcAutNum->printPTree(cout);
      cout << *pcAutNum;

      // Can't do that since AutNum object will be cache!
      // delete pcAutNum;
      }
    else
      cout << "Nothing!" << endl;
    }
  delete pcClient;
}

void getRoute(void)
{
  IRR *pcClient = 
    new RipeWhoisClient("brind.isi.edu", 43, "RADB,CW,RIPE,CANET,ANS");
  Prefix *pcPrefix = new Prefix("128.9.0.0/16");
  Route *pcRoute;
  pcClient->getRoute(pcRoute, pcPrefix, 226);
  if (pcRoute)
    {
    pcRoute->printPTree(cout);
    cout << *pcRoute << endl;
    delete pcRoute;
    }
  else
    cout << "Nothing!" << endl;
  delete pcPrefix;                   
  delete pcClient;
}

void getInetRtr(int argc, char *argv[])
{
  IRR *pcClient = new RipeWhoisClient("whois.6bone.net", 43);
  for (int i = 1; i < argc; i++)
    {
    const InetRtr *pcInetRtr = pcClient->getInetRtr(argv[i]);
    if (pcInetRtr) 
      cout << *(Object *)pcInetRtr;
    else
      cout << "Nothing!" << endl;
    }
  delete pcClient;
}

void expandAS(int argc, char *argv[])
{
  IRR::SetDefaultSources("RADB,CW,RIPE,CANET,ANS");
  IRR *pcClient = 
    new RipeWhoisClient("brind.isi.edu", 43, "RADB,CW,RIPE,CANET,ANS");
  //  const PrefixRanges *pcPrefixRanges = pcClient->expandAS(3582);
  for (int i = 1; i < argc; i++)
    {
    cout << "\n\nexpandAS(" << argv[i] << ")\n======================\n";
    const PrefixRanges *pcPrefixRanges = pcClient->expandAS(atoi(argv[i]));
    if (pcPrefixRanges)
      {
      for (int j = pcPrefixRanges->low(); j < pcPrefixRanges->fence(); j++)
        cout << (*pcPrefixRanges)[j].get_text() << endl;
      cout << endl;
      }
    else
      cout << "Nothing!" << endl;
    }
  delete pcClient;
}

void expandASSet(int argc, char *argv[])
{
  IRR *pcClient = 
    new RipeWhoisClient("whois.6bone.net", 43, "RADB,CW,RIPE,CANET,ANS");
  for (int i = 1; i < argc; i++)
    {
    const SetOfUInt *pcASSet = 
      pcClient->expandASSet(symbols.symID(argv[i]));
    cout << "\n\nexpandASSet() for " << argv[i] << endl
	 << "==================================" << endl;
    if (pcASSet)
      {
      for (Pix x = pcASSet->first(); x; pcASSet->next(x))
	cout << (*pcASSet)(x) << " ";
      }
    else
      cout << "Nothing!";
    cout << endl << endl;
    }
  delete pcClient;  
}

void expandRSSet(int argc, char *argv[])
{
  IRR *pcClient = 
    new RipeWhoisClient("water", 43, "RADB,CW,RIPE,CANET,ANS");
  for (int i = 1; i < argc; i++)
    {
//    char aa[] = "rs-any";
    const PrefixRanges *pcRSSet = 
//      pcClient->expandRSSet(symbols.symID(aa));
      pcClient->expandRSSet(symbols.symID(argv[i]));
    cout << "\n\nexpandRSSet() for " << argv[i] << endl
	 << "==================================" << endl;
    if (pcRSSet)
      for (int j = pcRSSet->low(); j < pcRSSet->fence(); j++)
	{
        PrefixRange prefix((*pcRSSet)[j]); // To avoid warning!
	cout << prefix << " ";
	}
    else
      cout << "Nothing!";
    cout << endl << endl;
    }
  delete pcClient;  
}


void main(int argc, char *argv[])
{
  trace.enable(0);
  schema.initialize();
  //  getAutNum(argc, argv);
  //  getRoute();
  getInetRtr(argc, argv);
  // Example input: 3582 226
  //  expandAS(argc, argv);
  // Example input: AS-ESNETEU
  //  expandASSet(argc, argv);
  // Example input: RS-CANET_PROXY_AGGRS
  //  expandRSSet(argc, argv);
}

#endif // DEBUG_RIPEWHOISC
