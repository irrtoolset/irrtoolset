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
#include "irrutil/version.hh"
#include "rpsl/object_log.hh"

extern "C" {
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

   if (use_proxy_connection) {
     // Open connection via a Proxy
     char proxybuf[BUFFER_SIZE + 1];
     int length = 0;

     Trace(TR_WHOIS_QUERY) << "Whois: Open " << host << ":" << port
                           << " via proxy " << proxy_host << ":" << proxy_port
			   << " source=" << _sources
			   << " protocol=" << protocolName[dflt_protocol].name
			   << endl;

     hp = gethostbyname(proxy_host);
     if (!hp)
#ifdef DIAG
         die_error("Proxy address translation failed!", 0);
#else
         error.Die("Error: proxy address translation failed.\n");
#endif /* DIAG */

     server_sockaddr.sin_family = AF_INET;
     memcpy((char *) &(server_sockaddr.sin_addr.s_addr), hp->h_addr, hp->h_length);
     server_sockaddr.sin_port = htons((u_short) proxy_port);

     sock = socket(AF_INET, SOCK_STREAM, 0);

     if (sock < 0)
#ifdef DIAG
         die_error("Proxy socket creation failed!", 0);
#else
         error.Die("Error: socket() failed.\n");
#endif /* DIAG */

     if (connect(sock, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr)) < 0) {
#ifdef DIAG
         err_msg = new char[1024];
         sprintf(err_msg, "Connect process to proxy %s on port %d failed!", proxy_host, proxy_port);
         die_error(err_msg, 0);
#else
         error.Die("Error: connect() failed.\n");
#endif /* DIAG */
     }

     in  = fdopen(sock, "r");
#ifdef DIAG
     if (in == NULL)
    	 die_error("Opening proxy socket for reading failed!", 0);
#endif /* DIAG */
     out = fdopen(sock, "w");
#ifdef DIAG
     if (out == NULL)
    	 die_error("Opening proxy socket for writing failed!", 0);
#endif /* DIAG */
     _is_open = 1;

     sprintf(proxybuf, "CONNECT %s:%d HTTP/1.1\nUser-Agent: %s %s\n\n", host, port, ProjectGoal, ProjectVersion);
     length = strlen(proxybuf);

     if (fwrite(proxybuf, 1, length, out) != length) {
#ifdef DIAG
		err_msg = new char[1024];
		sprintf(err_msg, "Connection request to proxy %s on port %d could not be sent!", proxy_host, proxy_port);
		die_error(err_msg, 0);
#else
	    error.Die("Error: Connection request to proxy %s on port %d could not be sent!\n", proxy_host, proxy_port);
#endif /* DIAG */
     }
     fflush(out);

     *proxybuf = '\0';
     fgets(proxybuf, BUFFER_SIZE, in);
     if (ferror(in)) {
#ifdef DIAG
       err_msg = new char[1024];
       sprintf(err_msg, "Connection response from proxy %s on port %d could not be read!", proxy_host, proxy_port);
       die_error(err_msg, 0);
#else
	   error.Die("Error: Connection response from proxy %s on port %d could not be read!\n", proxy_host, proxy_port);
#endif /* DIAG */
     }

     if (0 == memcmp(proxybuf, "HTTP/1.0 ", 9) || 0 == memcmp(proxybuf, "HTTP/1.1 ", 9)) {
       // got an HTTP response!
       char *proxybufptr = proxybuf + 9;
       if (0 != memcmp(proxybufptr, "200 ", 4)) {
         // Proxy response is NOT an "ok"!
         char *lf = strchr(proxybuf, '\n');
         if (lf != NULL) {
           lf++; *lf = '\0';
         }
#ifdef DIAG
         err_msg = new char[BUFFER_SIZE + strlen(proxybuf)];
         sprintf(err_msg, "Connect process via proxy %s on port %d failed! Proxy reported:\n%s", proxy_host, proxy_port, proxybuf);
         die_error(err_msg, 0);
#else
	     error.Die("Error: Connect process via proxy %s on port %d failed! Proxy reported:\n%s\n", proxy_host, proxy_port, proxybuf);
#endif /* DIAG */
       } else {
         // Proxy response is an "ok"! We don't need the rest, so clear "in" stream now.
         fseek(in, 0, SEEK_END);
       }
     } else {
       // unknown response from Proxy
#ifdef DIAG
       err_msg = new char[BUFFER_SIZE + strlen(proxybuf)];
       sprintf(err_msg, "Connect process via proxy %s on port %d failed! Unexpected response:\n%s", proxy_host, proxy_port, proxybuf);
       die_error(err_msg, 0);
#else
	   error.Die("Error: Connect process via proxy %s on port %d failed! Unexpected response:\n%s\n", proxy_host, proxy_port, proxybuf);
#endif /* DIAG */
     }
   } else {
     // Open connection directly
     Trace(TR_WHOIS_QUERY) << "Whois: Open " << _host << ":" << _port
			   << " source=" << _sources
			   << " protocol=" << protocolName[dflt_protocol].name
			   << endl;

     hp = gethostbyname(host);
     if (!hp)
#ifdef DIAG
         die_error("Host address translation failed!", 0);
#else
         error.Die("Error: gethostbyname(%s) failed.\n", host);
#endif /* DIAG */

     server_sockaddr.sin_family = AF_INET;
     memcpy((char *) &(server_sockaddr.sin_addr.s_addr), hp->h_addr, hp->h_length);
     server_sockaddr.sin_port = htons((u_short) port);

     sock = socket(AF_INET, SOCK_STREAM, 0);
     if (sock < 0)
#ifdef DIAG
        die_error("Socket creation failed!", 0);
#else
        error.Die("Error: socket() failed.\n");
#endif /* DIAG */

     if (connect(sock, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr)) < 0)
#ifdef DIAG
        die_error("Connect process failed!", 0);
#else
        error.Die("Error: connect() failed.\n");
#endif /* DIAG */

     in  = fdopen(sock, "r");
#ifdef DIAG
     if (in == NULL)
    	die_error("Opening socket for reading failed!", 0);
#endif /* DIAG */
     out = fdopen(sock, "w");
#ifdef DIAG
     if (out == NULL)
    	die_error("Opening socket for writing failed!", 0);
#endif /* DIAG */
   }

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
#ifdef DIAG
	   die_error(err_msg, err_occ);
#else
	   err = 1;
	   error.error("Error: setting source to %s failed.\n", sources);
#endif /* DIAG */
   }

   if (current_sources) 
      delete [] current_sources;
#ifdef DIAG
   if (QueryResponse(current_sources, "!s-lc") == 0) {
     die_error(err_msg, err_occ);
   }
#else
   QueryResponse(current_sources, "!s-lc");
#endif /* DIAG */
   current_sources[strlen(current_sources) - 1] = 0; // chop \n

   if (err)
      error.error("Error: current source setting is %s.\n", current_sources);
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
#ifdef DIAG
      err_occ = 2;
      err_msg = new char[strlen(last_query) + 128];
      sprintf(err_msg, "Reading response header for query \"%s\" failed!", last_query);
      return 0;
#else
      error.Die("Error: fgets() failed: no answer\n");
      return 0;
#endif /* DIAG */
   }

   if (!is_rpslng()) // one-liners
     Trace(TR_WHOIS_RESPONSE) << "Whois: Response " << buffer << endl;

   if (*buffer == 'D') { // key not found error
#ifdef DIAG
      if (irr_warnings) {
         err_msg = new char[strlen(last_query) + 128];
         sprintf(err_msg, "Key not found for query \"%s\"", last_query);
         warn_error();
      }
      extractRPSLInfo(last_query);
      return 0;
#else
      error.warning("Warning: key not found error for query %s.\n", last_query);
      return 0;
#endif /* DIAG */
   }
   if (*buffer == 'C') { // returned success
      response = new char[1];
      *response = 0;
      return 1;
   }
#ifdef DIAG
   if (*buffer == 'E') { // failure: multiple copies of key in database
      err_occ = 2;
      err_msg = new char[strlen(last_query) + 128];
      sprintf(err_msg, "Multiple copies of key found in database for query \"%s\"", last_query);
      return 0;
   }
#endif /* DIAG */
   if (buffer[0] == 'F') { // some other query error
#ifdef DIAG
      err_occ = 2;
      err_msg = new char[strlen(last_query) + strlen(buffer) + 128];
      sprintf(err_msg, "Server reported error in response for query \"%s\": %s", last_query, buffer);
      return 0;
#else
      return 0;
#endif /* DIAG */
   }
   if (*buffer != 'A' && !is_rpslng()) { // we are expecting a byte-count line
#ifdef DIAG
#else
      error.Die("Warning: no byte count error for query %s.\n", last_query);
      return 0;
#endif /* DIAG */
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
#ifdef DIAG
   {
	   err_occ = 2;
	   err_msg = new char[strlen(last_query) + 128];
	   sprintf(err_msg, "Reading response body for query \"%s\" failed!", last_query);
	   return 0;
   }
#else
      error.Die("Error: fread() failed.\n");
#endif /* DIAG */
   response[count] = 0;

   Trace(TR_WHOIS_RESPONSE) << "Whois: Response\n" << response << endl;

   // Read the return code line
   if (!fgets(buffer, sizeof(buffer), in)) {
#ifdef DIAG
       err_occ = 2;
       err_msg = new char[strlen(last_query) + 128];
       sprintf(err_msg, "Reading response termination line for query \"%s\" failed!", last_query);
       return 0;
#else
      error.Die("Error: fgets() failed.\n");
      return 0;
#endif /* DIAG */
   }

   Trace(TR_WHOIS_RESPONSE) << "Whois: Response " << buffer << endl;

   if (*buffer != 'C') {
#ifdef DIAG
      err_occ = 2;
      err_msg = new char[strlen(last_query) + 128];
      sprintf(err_msg, "Response did not terminate correctly for query \"%s\"", last_query);
      return 0;
#else
      error.Die("Warning: no end of data line error for query %s.\n", 
		    last_query);
#endif /* DIAG */
      delete [] response;
      response = NULL;
      return 0;
   }

   return count;
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
#ifdef DIAG
   {
	  last_query[length - 1] = '\0';
	  err_occ = 1;
	  delete err_msg;
	  err_msg = new char[length + 24];
	  sprintf(err_msg, "Sending query \"%s\" failed!", last_query);
   }
#else
      error.fatal("Error: fwrite() failed.\n");
#endif /* DIAG */

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

bool RAWhoisClient::getSet(SymID sname, const char *clss, char *&text, int &len) {
   len = QueryResponse(text, "!m%s,%s", clss, sname);
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
        cerr << "Warning: irrd/rawhoisd cannot resolve as-set " << asset << "!";
        cerr << "Unknown element found in as-set definition!\n";
      }
      if (set)
        delete [] set;
  } else {
    char *response;
    if (!QueryResponse(response, "!i%s,1", asset))
      return false;
    for (char *word = strtok(response, " \t\n"); 
         word; 
         word = strtok(NULL, " \t\n"))
      result->add(atoi(word+2));
    if (response)
      delete [] response;
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

    if (set)
      delete [] set;
  } else {
    char *response;
    if (!QueryResponse(response, "!i%s,1", rsset))
      return false;
    for (char *word = strtok(response, " \t\n"); 
         word; 
         word = strtok(NULL, " \t\n"))
      result->push_back(MPPrefix(word));
    if (response)
      delete [] response;
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
  if ((len = QueryResponse(result, "!r%s,l", buffer))) {
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
