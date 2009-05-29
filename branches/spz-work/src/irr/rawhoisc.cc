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

#include "config.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <cassert>

using namespace std;


extern "C" {
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
}

#include "rawhoisc.hh"
#include "route.hh"
#include "irrutil/trace.hh"
#include "rpsl/object_log.hh"

extern "C" {
#ifndef HAVE_DECL_CONNECT
extern int connect(...);
#endif // HAVE_DECL_CONNECT
#ifndef HAVE_DECL_SOCKET
extern int socket(...);
#endif // HAVE_DECL_SOCKET
#ifndef HAVE_DECL_GETHOSTBYNAME
extern struct hostent *gethostbyname(...);
#endif // HAVE_DECL_GETHOSTBYNAME

#ifdef DEFINE_HTONS_WITH_ELLIPSIS
extern u_short htons(...);
#else  // DEFINE_HTONS_WITH_ELLIPSIS
#ifdef DEFINE_HTONS_WITH_USHORT
extern u_short htons(unsigned short);
#endif // DEFINE_HTONS_WITH_USHORT
#endif // DEFINE_HTONS_WITH_ELLIPSIS

#ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
#endif
}

#ifndef BUFFER_SIZE
#define BUFFER_SIZE            2047
#endif

void RAWhoisClient::Open(const char *_host, const int _port, const char *_sources) {
   if (_is_open)
      Close();

   // see if we should ignore error messages from IRR
   error.ignore(ignore_errors);

   struct sockaddr_in server_sockaddr;
   struct hostent *hp;
   int sock;

   Trace(TR_WHOIS_QUERY) << "Whois: Open " 
			 << _host << ":" << _port 
			 << " source=" << _sources 
			 << " protocol=" << protocolName[dflt_protocol].name
			 << endl;

   strcpy(host, _host);
   strcpy(sources, _sources);
   port = _port;

   hp = gethostbyname(host);
   if (!hp)
      error.Die("Error: gethostbyname(%s) failed.\n", host);

   server_sockaddr.sin_family = AF_INET;
   memcpy((char *) &(server_sockaddr.sin_addr.s_addr), hp->h_addr, hp->h_length);
   server_sockaddr.sin_port = htons((u_short) port);
   
   sock = socket(AF_INET, SOCK_STREAM, 0);
/*
   // Set keep alive option for the socket -- wlee@isi.edu
   int on = 1;
   if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(on)) < 0)
     error.warning("Warning: setsockopt SO_KEEPALIVE failed!\n");
*/
   if (sock < 0)
      error.Die("Error: socket() failed.\n");

   if (connect(sock, (struct sockaddr *) &server_sockaddr, 
	       sizeof(server_sockaddr)) < 0)
      error.Die("Error: connect() failed.\n");

   in  = fdopen(sock, "r");
   out = fdopen(sock, "w");
   _is_open = 1;

   fwrite("!!\n", 1, 3, out);  // keep the connection open
   fflush(out);

   SetSources(sources);
}

void RAWhoisClient::Close() {
   if (!_is_open)
      return;

   Trace(TR_WHOIS_QUERY) << "Whois: Close" << endl;

   fwrite("q\n", 1, 2, out);  // quit
   fflush(out);

   fclose(in);
   fclose(out);
   _is_open = 0;
}

void RAWhoisClient::CheckConnection() {
   if (_is_open && feof(in)) {
      Trace(TR_WHOIS_QUERY) << "Whois: Closed" << endl;
      _is_open = 0;
      fclose(in);
      fclose(out);
      // Don't know who we are talking to now.
      version = 0;
   }
}

void RAWhoisClient::GetVersion() {
   char *buffer;
   char *start;

   if (! _is_open)
      Open();

   Trace(TR_WHOIS_QUERY) << "Whois: Version " << "!v" << endl;
   QueryResponse(buffer, "!v");
   start = strstr(buffer, "version");
   start = start + 8; //jump
   version = atoi(start)*10 + atoi(start+2); // x.x... format  
   Trace(TR_WHOIS_RESPONSE) << "Whois: Response " << buffer << endl;
   delete [] buffer;
}

void RAWhoisClient::SetSources(const char *_sources) {
   int err = 0;

   if (! _is_open)
      Open();

   if (*_sources)
      strcpy(sources, _sources);
   else
      strcpy(sources, "-*");

   Trace(TR_WHOIS_QUERY) << "Whois: SetSources " << sources << endl;

   if (QueryKillResponse("!s%s", sources) == 0) {
      err = 1;
      error.error("Error: setting source to %s failed.\n", sources);
   }

   if (current_sources) 
      delete [] current_sources;
   QueryResponse(current_sources, "!s-lc");
   current_sources[strlen(current_sources) - 1] = 0; // chop \n

   if (err)
      error.error("Error: current source setting is %s.\n", current_sources);
}

const char *RAWhoisClient::GetSources(void) {
   if (! _is_open)
      Open();
   return current_sources;
}

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

int RAWhoisClient::PendingData() {
   int status;
   int answer;

   if ((status = fcntl(fileno(in), F_GETFL, 0)) < 0)
      return TRUE;

   if (fcntl(fileno(in), F_SETFL, O_NDELAY) < 0)
      return TRUE;

   int c = fgetc(in);
   if (c == EOF) {
      clearerr(in);
      answer = FALSE;
   } else {
      ungetc(c, in);
      answer = TRUE;
   }

   fcntl(fileno(in), F_SETFL, status);

   Trace(TR_WHOIS_QUERY) << "Whois: PendingData " << answer << endl;

   return answer;
}

int RAWhoisClient::Response(char *&response) { 
   if (!_is_open)
      Open();

   char buffer[BUFFER_SIZE]; 

   // Read the "A<byte-count>" line
   if (! fgets(buffer, sizeof(buffer), in)) {
      error.Die("Error: fgets() failed: no answer\n");
      return 0;
   }

   if (!is_rpslng()) // one-liners
     Trace(TR_WHOIS_RESPONSE) << "Whois: Response " << buffer << endl;

   if (*buffer == 'D') { // key not found error
      error.warning("Warning: key not found error for query %s.\n", last_query);
      return 0;
   }
   if (*buffer == 'C') { // returned success
      response = new char[1];
      *response = 0;
      return 1;
   }
   if (buffer[0] == 'F') { // some other query error
      return 0;
   }
   if (*buffer != 'A' && !is_rpslng()) { // we are expecting a byte-count line
      error.Die("Warning: no byte count error for query %s.\n", last_query);
      return 0;
   }
   /*
    * If we sent an RIPE-style query (as we do for *some* RPSLng queries), 
    * then we want to parse it using RIPE-style handling.
    *
    * But we will attempt to detect use of IRRd queries. We assume that 
    * RIPE-style queries start with either an error (meaning the first 
    * character is '%') or they start with an attribute name (meaning 
    * there is a ':' somewhere in the buffer).
    */
   if (is_rpslng() && 
       ((buffer[0] == '%') || (strchr(buffer, ':') != NULL))) {
      int wasnl = 0;
      response = new char[1];
      response[0] = '\0';

      do {
        wasnl = (*buffer == '\n');
        Trace(TR_WHOIS_RESPONSE) << "Whois: Response <<\n" << buffer
                                 << ">>" << endl;
        if (strstr (buffer, "route") || strstr(buffer, "route6")) {
          char *p = strstr(buffer, ":");
          do {
            p++;
          } while (isspace (*p));

          // strip trailing newline
          int len = strlen (p);
          if(len > 0) {
            char *q = p + len - 1;
            if(*q == '\n')
                *q = '\0';
            len--;
          }

          // save current response
          len += strlen (response) + 2;
          char *copy = strdup (response);

          // reallocate response with more space
          delete [] response;
          response = new char [len];

          // append new repsonse onto old one
          snprintf (response, len, "%s %s", copy, p);
          free (copy);
        }
      } while (fgets(buffer, sizeof(buffer), in) &&
      // this condition should work with irrd version >= 2.2b19
      // until then, ripe-style queries won't work with persistent
      // connections
               !(wasnl && (*buffer == '\n')));

      // The WHOIS protocol and RPSL give no indication of
      // end of a protocol data unit, so we need to keep
      // connections open.  Of course, then long-running
      // programs can have connections time out, so we
      // need to restore those.
      CheckConnection();

      return (strlen(response) + 1);
   }

   int count = atoi(buffer + 1);
   response = new char[count + 1];

   if (fread(response, 1, count, in) != count)
      error.Die("Error: fread() failed.\n");
   response[count] = 0;

   Trace(TR_WHOIS_RESPONSE) << "Whois: Response\n" << response << endl;

   // Read the return code line
   if (!fgets(buffer, sizeof(buffer), in)) {
      error.Die("Error: fgets() failed.\n");
      return 0;
   }

   Trace(TR_WHOIS_RESPONSE) << "Whois: Response " << buffer << endl;

   if (*buffer != 'C') {
      error.Die("Warning: no end of data line error for query %s.\n", 
		    last_query);
      delete [] response;
      response = NULL;
      return 0;
   }

   return count;
}

// rusty - this function was added to read an irrserver response, 
// including the initial 'A' and following 'C' lines.  used by relayd
// to cache responses from the the server.
int RAWhoisClient::TotalResponse(char *&response) { 
   if (!_is_open)
      Open();
   if (error())
       return 0;

   char buffer[1024]; 

   // Read the "A<byte-count>" line
   if (!fgets(buffer, sizeof(buffer), in)) {
       error ("fgets() failed.");
       return 0;
   }

   if (*buffer == 'D') { // key not found error
       response = new char[strlen(buffer)];
       strcpy (response, buffer);
   } else if (*buffer == 'C') { // returned success
       response = new char[strlen (buffer) + 1];
       strcpy (response, buffer);
   } else if (*buffer != 'A') { // we are expecting a byte-count line
       response = new char[strlen (buffer) + 1];
       strcpy (response, buffer);
   }

   if (*buffer != 'A')
       return (strlen (response));

   // AXXX + body + return_code & extra, if any
   int len = strlen (buffer);
   int count = atoi(buffer + 1);
   response = new char[len + count + 80];

   strcpy (response, buffer);
   if (fread((char *) &response[len], 1, count, in) != count) {
       error ("fread() failed.");
       return 0;
   }

   len += count;
   response[len] = '\0';

   // Read the return code line
   if (!fgets(buffer, sizeof(buffer), in)) {
      error ("fgets() failed.");
      return 0; 
   }

   strcpy ((char *) &response[len], buffer);
   len += strlen (buffer);
   response[len] = '\0';

   return (error()) ? 0 : len;	// rusty: 0 or count
}

void RAWhoisClient::WriteQuery(const char *format, va_list ap) {
   vsprintf(last_query, format, ap);

   int length = strlen(last_query);

   if (last_query[length - 1] != '\n') {
      last_query[length] = '\n';
      length++;
      last_query[length] = 0;
   }

   //   if (! fwrite(last_query, 1, length, out))
   if (fwrite(last_query, 1, length, out) != length)
      error.fatal("Error: fwrite() failed.\n");

   fflush(out);

   last_query[length - 1] = 0;
   Trace(TR_WHOIS_QUERY) << "Whois: WriteQuery " << last_query << endl;
}

void RAWhoisClient::Query(const char *format, ...) {
   if (!_is_open)
      Open();

   va_list ap;
   va_start(ap, format);

   WriteQuery(format, ap);

   va_end(ap);
}

int RAWhoisClient::QueryResponse(char *&response, const char *format, ...) { 
   if (!_is_open)
      Open();
   
   va_list ap;
   va_start(ap, format);

   WriteQuery(format, ap);

   va_end(ap);

   return Response(response);
}

int RAWhoisClient::QueryKillResponse(const char *format, ...) { 
   char *response = NULL;
   
   if (!_is_open)
      Open();

   va_list ap;
   va_start(ap, format);

   WriteQuery(format, ap);

   va_end(ap);

   int code = Response(response);

   if (response)
      delete [] response;
   return code;
}

bool RAWhoisClient::getAutNum(char *as,   char *&text, int &len) {
   len = QueryResponse(text, "!man,%s", as);
   return len;
}

bool RAWhoisClient::getSet(SymID sname, char *clss, char *&text, int &len) {
   len = QueryResponse(text, "!i%s,1", sname);
   return len;
}

bool RAWhoisClient::getRoute(char *rt, char *as, char *&text, int &len) {
   len = QueryResponse(text, "!mrt,%s-%s", rt, as);
   return len;
}

bool RAWhoisClient::getInetRtr(SymID inetrtr,    char *&text, int &len) {
   len = QueryResponse(text, "!mir,%s", inetrtr);
   return len;
}

bool RAWhoisClient::expandAS(char *as,       MPPrefixRanges *result) {
  char *response;

  if (!version)
    GetVersion();
  
  if (! is_rpslng()) {   
    if (!QueryResponse(response, "!g%s", as)) return false;
    for (char *word = strtok(response, " \t\n");
       word; 
       word = strtok(NULL, " \t\n")) 
       result->push_back(MPPrefix(word));
  } else {
    if (!QueryResponse(response, "-K -r -i origin %s", as)) return false;
    for (char *word = strtok(response, " \t\n");
       word; 
       word = strtok(NULL, " \t\n"))  {
       result->push_back(MPPrefix(word));
       }
  }
  if (response)
   delete [] response;
  return true;
}

bool RAWhoisClient::expandASSet(SymID asset, SetOfUInt *result) {
  Set *set = NULL;

  if (queryCache(asset, set)) {
    AttrGenericIterator<Item> itr(set, "members");
    for (Item *pt = itr.first(); pt; pt = itr.next())
      if (typeid(*pt) == typeid(ItemASNAME)) { // ASNAME (aka as-set)
         const SetOfUInt *tmp = IRR::expandASSet(((ItemASNAME *)pt)->name);
         if (tmp) 
           *result |= *(SetOfUInt *) tmp;
      } else if (typeid(*pt) == typeid(ItemASNO)) {
         result->add(((ItemASNO *)pt)->asno);
      } else {
       cerr << "WARNING: irrd/rawhoisd cannot resolve as-set " << asset << "!";
        cerr << "Unknown element found in as-set definition!\n";
      }
    if (set != NULL) free (set);
  } else {
    char *text;
    int  len;
    if (getSet(asset, "as-set", text, len)) {
      for (char *word = strtok(text, " \t\n"); word; word = strtok(NULL, " \t\n"))
        result->add(atoi(word+2));
    }
    if (text != NULL) free(text);
  }

  return true;
}

bool RAWhoisClient::expandRSSet(SymID rsset, MPPrefixRanges *result) {
  Set *set = NULL;

  if (queryCache(rsset, set)) {
    AttrGenericIterator<Item> itr(set, "members");
    for (Item *pt = itr.first(); pt; pt = itr.next()) {
      expandItem(pt, result);
    }
    AttrGenericIterator<Item> itr1(set, "mp-members");
    for (Item *pt = itr1.first(); pt; pt = itr1.next()) {
      expandItem(pt, result);
    }
    if (set != NULL) free (set);
  } else {
    char *text;
    int  len;
    if (getSet(rsset, "route-set", text, len)) {
      for (char *word = strtok(text, " \t\n"); word; word = strtok(NULL, " \t\n"))
        result->push_back(MPPrefix(word));
    }
    if (text != NULL) free(text);
  }

  return true;  
}

bool RAWhoisClient::expandRtrSet(SymID sname, MPPrefixRanges *result) {
   const Set *set = IRR::getRtrSet(sname);
   AttrGenericIterator<Item> itr(set, "members");
   for (Item *pt = itr.first(); pt; pt = itr.next())
      expandItem(pt, result);

   cerr << "Warning: irrd/rawhoisd does not yet support rtr-set expansion." 
	<< endl;

   return true;
}

ASt RAWhoisClient::getOrigin(char *format, ...)
{
  char buffer[64];
  va_list ap;
  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);

  char *result = NULL;
  int len;
  setFastResponseFlag(false);
  if (len = QueryResponse(result, "!r%s,l", buffer)) {
     Buffer b(result, len);
     List<Object> log;
     extern void objectLog(const Buffer &buf, List<Object> &log);
     objectLog(b, log);

     for (ListIterator<Object> route(log); route; ++route) {
	AttrGenericIterator<ItemPRFXV4> prefix(route, "route");
	if (prefix->prfxv4->get_length() >= 6) {
	   AttrGenericIterator<ItemASNO> objItr(route, "origin");
	   const ItemASNO *asn = objItr.first();
	   if (asn) {
	      if (result)
		 delete [] result;
	      return asn->asno;
	   }
	   // else just go to the bottom and return AS number 0
	}
     }
  }
  if (result)
     delete [] result;
  return 0;
}
