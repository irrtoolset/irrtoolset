//  Author(s): WeeSan Lee <wlee@ISI.EDU>
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include "birdwhoisc.hh"
#include "classes.hh"
#include "autnum.hh"
#include "route.hh"
#include "util/trace.hh"
#include "rpsl/object_log.hh"
#include <memory.h>
#include <malloc.h>
#include <cstring>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE            2047
#endif 

#define QUERY_FIELD_SEPARATOR  '@'
#define DATA_SEPARATOR         '\n'
#define NO_OF_TRIAL_TRY		3

BirdWhoisClient::BirdWhoisClient(void) : response(NULL),
  Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
{
  Open(dflt_host, dflt_port, dflt_sources);
}

BirdWhoisClient::BirdWhoisClient(const char *host, 
				 const int port,
				 const char *sources = dflt_sources) :
  response(NULL), Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
{
  Open(host, port, sources);
}

void BirdWhoisClient::Open(const char *_host = dflt_host, 
		  const int _port = dflt_port, 
		  const char *_sources = dflt_sources)
{
  //Trace(TR_WHOIS_QUERY) << getsock() 
  //  			  << " - Whois: Open " << _host << ":" << _port 
  Trace(TR_WHOIS_QUERY) << "Whois: Open " << _host << ":" << _port 
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
     sprintf(pzcFormat, "-k -V IRRToolSet -r -s %s %s\r\n", sources, pzcQuery);
  else
     sprintf(pzcFormat, "-k -V IRRToolSet -r %s\r\n", pzcQuery);

  va_list ap;
  va_start(ap, pzcQuery);
  vsprintf(pzcBuffer, pzcFormat, ap);
  va_end(ap);

  int iLen = strlen(pzcBuffer);

  Trace(TR_WHOIS_QUERY) << "Whois: WriteQuery " 
			<< pzcBuffer
			<< " ("
			<< iLen
			<< ")"
			<< endl;

  // Send out the query out of wire
  if (write(pzcBuffer, iLen) < 0) return false;
  return true;
}

// remove RIPE-style comments
static void stripRipeComments(Buffer &buf) {
   bool removedLines = false;
   char *cont = buf.contents;
   size_t size = buf.size;

   while (size >= 2 && *cont == '%') {
      while (size >= 2 && *cont == '%') {
	 // cengiz changed below to size - 1 
	 // to make sure we received enough to check the stripped comment
	 // is followed by  either another comment or by a blank line
	 // if not we are in trouble...
	 char *eol = (char *)memchr(cont, '\n', size-1);
	 if (eol == NULL)
	    // no EOL, no comment to strip (yet)
	    goto out;
	 assert(*(eol+1) == '\n' || *(eol+1) == '%');
	 size -= (eol-cont)+1;
	 cont = eol + 1;
	 removedLines = true;
      }
      if (*cont == '\n') {
	 size--;
	 cont++;
      }
   }

 out:
   if (removedLines)
      buf.flush(buf.size - size);
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
	 delete response;
	 response=NULL;
	 return false;
      }
   } while (!found);

   Buffer *result = response;
   if (endOfQuery+2 == response->size)
      response = NULL;
   else { // there is stuff after the response
      int fullSize = response->size;
      result->size = endOfQuery+2;
      response = new Buffer(fullSize - result->size);
      response->append(result->contents+result->size, response->capacity);
   }

   stripRipeComments(*result);
   if (result->size == 2) { // we got 2 \n's
      assert(*result->contents == '\n');
      assert(*(result->contents+1) == '\n');
      delete response;
      response=NULL;
      return false;
   }
   if (result->size == 1) { // we got 1 \n's, 
      // the other \n is removed by stripRIPEComments
      assert(*result->contents == '\n');
      delete response;
      response=NULL;
      return false;
   }

   Trace(TR_WHOIS_RESPONSE).form("WhoisResponse[%.*s]\n",
				 result->size, result->contents);
   text = result->contents;
   len  = result->size;
   return true;
}

bool BirdWhoisClient::getAutNum(char *as,          char *&text, int &len)
{
  if (!sendQuery("-T aut-num %s", as)) return false;
  if (!getResponse(text, len)) return false;
  return true;
}

bool BirdWhoisClient::getSet(SymID sname, char *clss, char *&text, int &len) {
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

bool BirdWhoisClient::expandAS(char *as, PrefixRanges *result) {
  if (!sendQuery("-K -T route -i origin %s", as)) 
    return false;
  char *pzcKeys;
  int iKeyLen;
  if (getResponse(pzcKeys, iKeyLen))
    {
       assert(*(pzcKeys+iKeyLen-1) == '\n');
       *(pzcKeys+iKeyLen) = 0;
       char *p = pzcKeys;
       char *prefix;
       while (p && p < pzcKeys + iKeyLen) {
	  p = strstr(p, "route");
	  if (!p)
	     break;
	  p += 6; // we skip one extra character for ':' or '@' depending on the protocol
	  while (*p && isspace(*p))
	     p++;
	  prefix = p;
	  while (*p && (!isspace(*p) && *p != '@'))
	     p++;
	  *p = 0;
	  p++;
	  result->add_high(prefix);
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
}

bool BirdWhoisClient::expandRSSet(SymID sname, PrefixRanges *result) {
  const Set *set = IRR::getRSSet(sname);
  AttrGenericIterator<Item> itr(set, "members");
  for (Item *pt = itr.first(); pt; pt = itr.next())
     expandItem(pt, result);

  char buffer[1024];
  // ripev3 support
  if (!strcasecmp(protocol, "ripe") && !strcasecmp(sname, "rs-any")) 
     strcpy(buffer, "-T route -M 0.0.0.0/0");
  else
     sprintf(buffer, "-T route -i member-of %s", sname);
  getIndirectMembers(sname, set, buffer, collectPrefix, result);

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

bool BirdWhoisClient::expandRtrSet(SymID sname, PrefixRanges *result) {
  const Set *set = IRR::getRtrSet(sname);
  AttrGenericIterator<Item> itr(set, "members");
  for (Item *pt = itr.first(); pt; pt = itr.next())
     expandItem(pt, result);

  char buffer[1024];
  sprintf(buffer, "-T inet-rtr -i member-of %s", sname);
  getIndirectMembers(sname, set, buffer, collectIfAddr, result);

  return true;
}

int BirdWhoisClient::getSourceOrigin(char *&buffer, const char *rt)
{
  querySourceOrigin(rt);
  return getSourceOrigin(buffer); 
}

int BirdWhoisClient::getSourceOrigin(char *&buffer)
{
  StringBuffer cBuffer;
  char *pzcResult, *pzcResult2;
  int iResultLen, iResultLen2;
  if (getResponse(pzcResult, iResultLen)) 
    {
    // More and complete route objects are here
    Buffer b(pzcResult, iResultLen);  // pass ownership to class Buffer
    List<Object> cObjects;
    objectLog(b, cObjects);
    for (ListIterator<Object> itrObject(cObjects); itrObject; ++itrObject)
      if (itrObject->type && strcmp(itrObject->type->name, "route") == 0)
	{
	AttrGenericIterator<ItemASNO> itrOrigin(itrObject, "origin");
	AttrGenericIterator<ItemWORD> itrSource(itrObject, "source");
	if (itrOrigin && itrSource)
	  cBuffer.append("%s AS%d\n", itrSource()->word, itrOrigin()->asno);
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


#ifdef DEBUG_BIRDWHOISC

#include <iostream.h>
#include "autnum.hh"
#include "route.hh"

void getAutNum(int argc, char *argv[])
{
  IRR *pcClient = new BirdWhoisClient("water", 3500);
  for (int i = 1; i < argc; i++)
    {
    const AutNum *pcAutNum = pcClient->getAutNum(atoi(argv[i]));
    cout << "\033[32m" << i << ": AS" << argv[i] << "\033[m" << endl;
    if (pcAutNum)
      {
	;
//      for (AttrIterator<AttrExport> itr(pcAutNum, "export"); itr; itr++)
//	cout << itr()->policy << endl;

//      pcAutNum->printPTree(cout);

//      cout << *pcAutNum;
/*
for (ASt asno = pcAutNum->firstPeerAS();
     asno != INVALID_AS;
     asno = pcAutNum->nextPeerAS())
  cout << asno << endl;
*/
/*
List<Peering> peeringList = pcAutNum->getPeeringList();
for (ListIterator<Peering> itr(peeringList); itr; itr++)
  cout << *itr() << endl;
*/
      // Can't do that since AutNum object will be cache!
      //      delete pcAutNum;
      }
    else
      cout << "Nothing!" << endl;
    }
  delete pcClient;
}

void getRoute(void)
{
  IRR *pcClient = new BirdWhoisClient("localhost", 3500);
  Prefix *pcPrefix = new Prefix("128.97.0.0/16");
  Route *pcRoute;
  pcClient->getRoute(pcRoute, pcPrefix, 226);
  if (pcRoute)
    {
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
  IRR *pcClient = new BirdWhoisClient("water", 3500);
  for (int i = 1; i < argc; i++)
    {
    const InetRtr *pcInetRtr = pcClient->getInetRtr(argv[i]);
    if (pcInetRtr) 
      //      cout << *(Object *)pcInetRtr;
      ;
    else
      cout << "Nothing!" << endl;
    }
  delete pcClient;
}

void expandAS(int argc, char *argv[])
{
  IRR *pcClient = new BirdWhoisClient("localhost", 3500);
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
    new BirdWhoisClient("localhost", 3500);
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
  IRR *pcClient = new BirdWhoisClient("wee", 3500);
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
  //  getInetRtr(argc, argv);
  //  expandAS(argc, argv);
  expandASSet(argc, argv);
  //  expandRSSet(argc, argv);
}

#endif // DEBUG_BIRDWHOISC
