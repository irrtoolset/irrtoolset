//  Author(s): WeeSan Lee <wlee@ISI.EDU>
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include "birdwhoisc.hh"
#include "classes.hh"
#include "autnum.hh"
#include "route.hh"
#include "irrutil/trace.hh"
#include "irrutil/version.hh"
#include "rpsl/object_log.hh"
#include <memory.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE            2047
#endif 

#define QUERY_FIELD_SEPARATOR  '@'
#define DATA_SEPARATOR         '\n'
#define NO_OF_TRIAL_TRY		3

BirdWhoisClient::BirdWhoisClient(void) : response(NULL),
  Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
{
#ifdef DIAG
  last_query = new char[0];
#endif /* DIAG */
  Open(dflt_host, dflt_port, dflt_sources);
}

BirdWhoisClient::BirdWhoisClient(const char *host, 
				 const int port,
				 const char *sources) :
  response(NULL), Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
{
#ifdef DIAG
  last_query = new char[0];
#endif /* DIAG */
  Open(host, port, sources);
}

void BirdWhoisClient::Open(const char *_host,
                           const int _port,
                           const char *_sources)
{
	if (use_proxy_connection) {
		// Open connection via a Proxy

		Trace(TR_WHOIS_QUERY) << "Whois: Open " << _host << ":" << _port
				<< " via proxy " << proxy_host << ":" << proxy_port
				<< " source=" << _sources << " protocol="
				<< protocolName[dflt_protocol].name << endl;

		char proxybuf[BUFFER_SIZE + 1];
		char proxyrsp[BUFFER_SIZE + 1];
		ipAddr cServer(proxy_host);
		set_dstInaddr(cServer.getInaddr());
		setPort(proxy_port);

		if (connect() < 0) {
#ifdef DIAG
			err_msg = new char[1024];
			sprintf(err_msg, "Connect process to proxy %s on port %d failed!",
					proxy_host, proxy_port);
			die_error(err_msg, 0);
#else
		    cerr << "Connection to proxy " << proxy_host << " on port " << proxy_port << " failed!" << endl;
		    exit(1);
#endif /* DIAG */
		}

		sprintf(proxybuf, "CONNECT %s:%d HTTP/1.1\nUser-Agent: %s %s\n\n", _host, _port, ProjectGoal, ProjectVersion);

		if (Socket::write(proxybuf, strlen(proxybuf)) < 0) {
#ifdef DIAG
			err_msg = new char[1024];
			sprintf(err_msg, "Connection request to proxy %s on port %d could not be sent!", proxy_host, proxy_port);
			die_error(err_msg, 0);
#else
		    cerr << "Connection request to proxy " << proxy_host
		    	 << " on port " << proxy_port << " could not be sent!" << endl;
		    exit(1);
#endif /* DIAG */
		}
		if (Socket::read(proxyrsp, BUFFER_SIZE) < 0) {
#ifdef DIAG
			err_msg = new char[1024];
			sprintf(err_msg, "Connection response from proxy %s on port %d could not be read!", proxy_host, proxy_port);
			die_error(err_msg, 0);
#else
		    cerr << "Connection response from proxy " << proxy_host
		    	 << " on port " << proxy_port << " could not be read!" << endl;
		    exit(1);
#endif /* DIAG */
		}

		if (0 == memcmp(proxyrsp, "HTTP/1.0 ", 9)
				|| 0 == memcmp(proxyrsp, "HTTP/1.1 ", 9)) {
			// got an HTTP response!
			char *respbufptr = proxyrsp + 9;
			if (0 != memcmp(respbufptr, "200 ", 4)) {
				// Proxy response is NOT an "ok"!
				char *lf = strchr(proxyrsp, '\n');
				if (lf != NULL) {
					lf++;
					*lf = '\0';
				}
#ifdef DIAG
				err_msg = new char[BUFFER_SIZE + strlen(proxyrsp)];
				sprintf(err_msg,
						"Connect process via proxy %s on port %d failed! Proxy reported:\n%s",
						proxy_host, proxy_port, proxyrsp);
				die_error(err_msg, 0);
#else
				cerr << "Connect process via proxy " << proxy_host
					 << " on port " << proxy_port << " failed! Proxy reported:" << endl;
				cerr << proxyrsp << endl;
				exit(1);
#endif /* DIAG */
			} else {
				// Proxy response is an "ok"! We don't need the rest, so clear response buffer now.
				*proxyrsp = '\0';
			}
		} else {
#ifdef DIAG
			// unknown response from Proxy
			err_msg = new char[BUFFER_SIZE + strlen(proxyrsp)];
			sprintf(err_msg,
					"Connect process via proxy %s on port %d failed! Unexpected response:\n%s",
					proxy_host, proxy_port, proxyrsp);
			die_error(err_msg, 0);
#else
			cerr << "Connect process via proxy " << proxy_host
				 << " on port " << proxy_port << " failed! Unexpected response:" << endl;
			cerr << proxyrsp << endl;
			exit(1);
#endif /* DIAG */
		}
	} else {
		// Open connection directly

		Trace(TR_WHOIS_QUERY) << "Whois: Open " << _host << ":" << _port
				<< " source=" << _sources << " protocol="
				<< protocolName[dflt_protocol].name << endl;

		ipAddr cServer(_host);
		set_dstInaddr(cServer.getInaddr());
		setPort(_port);
		if (connect() < 0) {
#ifdef DIAG
			die_error("Connect process failed!", 0);
#else
		    cerr << "Connection to " << _host << ":" << _port << " failed!" << endl;
		    exit(1);
#endif /* DIAG */
		}
	}
	// whois server stuff rather than the socket stuff
	strcpy(host, _host);
	strcpy(sources, _sources);
	port = _port;
}

void BirdWhoisClient::Close()
{
  Trace(TR_WHOIS_QUERY) << getsock() << " - Whois: Close" << endl;
}

bool BirdWhoisClient::sendQuery(const char *pzcQuery, ...)
{
  char pzcBuffer[BUFFER_SIZE + 1];
  char pzcFormat[512];
  if (sources && *sources) 
     sprintf(pzcFormat, "-k -V %s -r -s %s %s\r\n", ProjectVersion, sources, pzcQuery);
  else
     sprintf(pzcFormat, "-k -V %s -r %s\r\n", ProjectVersion, pzcQuery);

  va_list ap;
  va_start(ap, pzcQuery);
  vsprintf(pzcBuffer, pzcFormat, ap);
  va_end(ap);

  int iLen = strlen(pzcBuffer);

#ifdef DIAG
  delete last_query;
  last_query = new char[iLen+1];
  *last_query = '\0';
  strcat(last_query, pzcBuffer);
  last_query[iLen-1] = '\0';
#endif /* DIAG */

  Trace(TR_WHOIS_QUERY) << "Whois: WriteQuery " 
			<< pzcBuffer
			<< " ("
			<< iLen
			<< ")"
			<< endl;

  // Send out the query out of wire
#ifdef DIAG
  if (write(pzcBuffer, iLen) < 0) {
     err_occ = 1;
     delete err_msg;
     err_msg = new char[iLen + 128];
     sprintf(err_msg, "Sending query \"%s\" failed!", last_query);
     return false;
  }
#else
  if (write(pzcBuffer, iLen) < 0) return false;
#endif /* DIAG */
  return true;
}

// remove RIPE-style comments
static void stripRipeComments(Buffer &buf) {
/* method rewritten by Hagen Boehm */
/* as long as we find lines starting with % or \n we clear them */ 
   char *cont = buf.contents;
   size_t rest_size = buf.size;
   size_t jump = 0;

   if (*cont == '%' || *cont == '\n') {

     while ((*cont == '%' || *cont == '\n') && rest_size > 0) {
       // skip one line
       if (*cont == '\n') {
         jump = 1;
       } else {
         char *eol = (char *)memchr(cont, '\n', rest_size);
         if (eol == NULL) {
           jump = rest_size;
         } else {
           jump = eol - cont +1;
         }
       }
       rest_size -= jump;
       if (rest_size != 0) {
         cont += jump;
       }
     }

     buf.flush(buf.size - rest_size);
   }
}

bool BirdWhoisClient::getResponse(char *&text, int &len) {
   int bytesRead;
   bool found = false;
   unsigned int endOfQuery;

   if (!response) 
      // there may be some left over data after the previous query's response
      response = new Buffer;

   endOfQuery = 0;
   do {
      if (endOfQuery >= 3)
	 endOfQuery -= 3;
      for (; 
	   endOfQuery + 2 < response->size && !found;
	   endOfQuery++)
	 if (response->contents[endOfQuery] == '\n' 
	     && response->contents[endOfQuery+1] == '\n' 
	     && response->contents[endOfQuery+2] == '\n')
	    found = true;

      if (found)
	 break;

      if (response->capacity - response->size <= 0)
	 response->extend();
      bytesRead = Socket::read(response->contents + response->size, 
			       response->capacity - response->size);
      response->size += bytesRead;

      if (bytesRead <= 0) {
#ifdef DIAG
          err_occ = 2;
          err_msg = new char[strlen(last_query) + 128];
          sprintf(err_msg, "Reading response for query \"%s\" failed!", last_query);
#endif /* DIAG */
	 delete response;
	 response=NULL;
	 return false;
      }
   } while (!found);

   Buffer *result = response;
   if (endOfQuery+2 >= response->size) {
      response = NULL;
   } else { // there is stuff after the response
      int fullSize = response->size;
      result->size = endOfQuery+2;
      response = new Buffer(fullSize - result->size);
      response->append(result->contents+result->size, response->capacity);
   }
   stripRipeComments(*result);
   if (result->size == 0) {
#ifdef DIAG
	  if (irr_warnings) {
		  err_msg = new char[strlen(last_query) + 35];
		  sprintf(err_msg, "No data in response for query \"%s\"!", last_query);
		  warn_error();
	  }
	  extractRPSLInfo(last_query);
#endif /* DIAG */
     delete response;
     response=NULL;
     Trace(TR_WHOIS_RESPONSE) << "TROPOS WARNING: query response was empty!\n" << flush;
     return false;
   }

   Trace(TR_WHOIS_RESPONSE)
      << "WhoisResponse["
      << string(result->contents, 0, result->size)
      << "]"
      << endl;


   text = result->contents;
   len = result->size;
   return true;
}

bool BirdWhoisClient::getAutNum(char *as,          char *&text, int &len)
{
  if (!sendQuery("-T aut-num %s", as)) return false;
  if (!getResponse(text, len)) return false;
  return true;
}

bool BirdWhoisClient::getSet(SymID sname, const char *clss, char *&text, int &len) {
   if (!sendQuery("-T %s %s", clss, sname)) 
      return false;
   if (!getResponse(text, len)) 
      return false;
   return true;
}

bool BirdWhoisClient::getRoute(char *rt, char *as, char *&text, int &len)
{
   if (strcasecmp(protocol, "ripe")) {
      if (!sendQuery("-T route %s@%s", rt, as)) 
	 return false;
      if (!getResponse(text, len)) 
	 return false;
      return true;
   }
   // ripev3 support
   // ripe does not have a query to get a specific route object given its key
   // here is a work around...

   char *response;
   int l;
   // ripev3 support
   if (!sendQuery("-x -T route %s", rt)) 
      return false;

   if (!getResponse(response, l)) 
      return false;
   
   List<Object> objs;
   Buffer respBuf(response, l);
   objectLog(respBuf, objs);
   int asn = atoi(as+2);

   for (ListIterator<Object> o(objs); o; ++o) {
      AttrGenericIterator<ItemASNO> i(o, "origin"); 
      if (i && i->asno == asn) {
	 text = (char *) malloc(o->size);
	 strncpy(text, o->contents, o->size);
	 len  = o->size;
	 // we may have a leak here
	 free(response);
	 return true;
      }
   }

   return false;
}

bool BirdWhoisClient::getInetRtr(SymID inetrtr,    char *&text, int &len)
{
  if (!sendQuery("-T inet-rtr %s", inetrtr))
    return false;
  if (!getResponse(text, len)) return false;
  return true;
}

// REIMP
bool BirdWhoisClient::expandAS(char *as, MPPrefixRanges *result) {
  if (!sendQuery("-K -i origin %s", as)) 
    return false;
  char *pzcKeys;
  int iKeyLen;
  if (getResponse(pzcKeys, iKeyLen))
    {
       assert(*(pzcKeys+iKeyLen-1) == '\n');
       *(pzcKeys+iKeyLen) = 0;
       char *p = pzcKeys;
       char *prefix;
       char *begin;
       while (p && p < pzcKeys + iKeyLen) {
          begin = strstr(p,"route:");
          if (!begin) {
            begin = strstr(p, "route6:"); 
          }
          if (!begin)
            break;
          p = begin;
          // locate 1st ":"
      	  p = strstr(p,":") + 1; // we skip one extra character for ':' or '@' depending on the protocol
      	  while (*p && isspace(*p))
      	     p++;
       	  prefix = p;
      	  while (*p && (!isspace(*p) && *p != '@'))
      	     p++;
      	  *p = 0;
      	  p++;
      	  result->push_back(MPPrefix(prefix));
       }
    free(pzcKeys);
    return true;
    }
  return false;  
}

bool BirdWhoisClient::getIndirectMembers(char *setName,
					  const Object *o, char *query, 
					  void (*collect)(void *, const Object *), void *collectArg) {
   char finalQuery[1024];
   bool isAny = false;
   if (!strcasecmp(setName, "rs-any") || !strcasecmp(setName, "as-any"))
      isAny = true;

   AttrGenericIterator<ItemWORD> mbrs_by_ref(o, "mbrs-by-ref");
   if (!isAny && ! mbrs_by_ref)
      return false;

   // if mbrs_by_ref contains any, then we only needs keys
   for (mbrs_by_ref.first(); mbrs_by_ref && !isAny; mbrs_by_ref++)
      isAny = ! strcasecmp(mbrs_by_ref()->word, "any");

   if (isAny) 
      strcpy(finalQuery, "-K ");
   else
      finalQuery[0] = 0;
   strcat(finalQuery, query);
      
   if (!sendQuery(finalQuery))
      return false;

   char *response;
   int  len;
   if (!getResponse(response, len))
      return false;

   Buffer respBuf(response, len);
   List<Object> objs;
   objectLog(respBuf, objs);

   for (ListIterator<Object> o(objs); o; ++o)
      if (isAny || isIndirectMember(o, mbrs_by_ref))
	 (*collect)(collectArg, o);

   free(response);

   return true;
}

bool BirdWhoisClient::expandRSSet(SymID sname, MPPrefixRanges *result) {
  
  const Set *set = IRR::getRSSet(sname);
  AttrGenericIterator<Item> itr(set, "members");
  for (Item *pt = itr.first(); pt; pt = itr.next()) {
     expandItem(pt, (MPPrefixRanges *) result);
  }
  AttrGenericIterator<Item> itr1(set, "mp-members");
  for (Item *pt = itr1.first(); pt; pt = itr1.next()) {
     expandItem(pt, (MPPrefixRanges *) result);
  }
 
  char buffer[1024];

  // ripev3 support
  if (!strcasecmp(protocol, "ripe") && !strcasecmp(sname, "rs-any")) 
     strcpy(buffer, "-Troute -M 0.0.0.0/0");
  else
     sprintf(buffer, "-i member-of %s", sname);
  getIndirectMembers(sname, set, buffer, collectPrefix, result);
  

  bzero(buffer, sizeof(buffer));
  if (!strcasecmp(protocol, "ripe") && !strcasecmp(sname, "rs-any")) {
     strcpy(buffer, "-Troute6 -M ::/0");
     getIndirectMembers(sname, set, buffer, collectPrefix, result);
  }

  return true;

}

bool BirdWhoisClient::expandASSet(SymID sname, SetOfUInt *result) {
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
  sprintf(buffer, "-T aut-num -i member-of %s", sname);
  getIndirectMembers(sname, set, buffer, collectASNO, result);

  return true;
}

bool BirdWhoisClient::expandRtrSet(SymID sname, MPPrefixRanges *result) {
  const Set *set = IRR::getRtrSet(sname);
  AttrGenericIterator<Item> itr(set, "members");
  for (Item *pt = itr.first(); pt; pt = itr.next())
     expandItem(pt, (MPPrefixRanges *) result);
  AttrGenericIterator<Item> itr1(set, "mp-members");
  for (Item *pt = itr1.first(); pt; pt = itr1.next())
     expandItem(pt, (MPPrefixRanges *) result);

  char buffer[1024];
  sprintf(buffer, "-T inet-rtr -i member-of %s", sname);
  getIndirectMembers(sname, set, buffer, collectIfAddr, result);

  return true;
}

void BirdWhoisClient::querySourceOrigin(const char *rt)
{
   // ripev3 support
   if (!strcasecmp(protocol, "ripe"))
      sendQuery("-a -x -T route %s", rt);
   else
      sendQuery("-a -T route -i route %s", rt);
}

ASt BirdWhoisClient::getOrigin(char *format, ...)
{
  char buffer[64];
  va_list ap;
  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);

  // ripev3 support
  if (!strcasecmp(protocol, "ripe")) {
     if (!sendQuery("-L -T route %s", buffer))
	return 0;
  } else
     if (!sendQuery("-L -T route -i route %s", buffer))
	return 0;

  ASt tResult = 0;
  char *pzcResult;
  int iResultLen;
  if (getResponse(pzcResult, iResultLen)) 
    {
    // Parse the first one only!
    // There might be more route object after NULL character, but we need only
    // one route object
    Buffer b(strdup(pzcResult), strlen(pzcResult));
    Route cRoute(b);
    AttrGenericIterator<ItemASNO> cObjItr(&cRoute, "origin");
    tResult = cObjItr()->asno;
    free(pzcResult);
    }
  return tResult;
}
