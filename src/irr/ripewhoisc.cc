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

#ifdef RIPE
#include "ripewhoisc.hh"
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

RipeWhoisClient::RipeWhoisClient(void) :
	Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
{
	last_query = new char[0];
	*response_buffer = '\0';
	Open(dflt_host, dflt_port, dflt_sources);
}

RipeWhoisClient::RipeWhoisClient(const char *host, const int port, const char *sources) : Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) {
	last_query = new char[0];
	*response_buffer = '\0';
	Open(host, port, sources);
}

void RipeWhoisClient::Open(const char *_host, const int _port, const char *_sources) {
	if (IRR::use_proxy_connection) {
		// Open connection via a Proxy
		Trace(TR_WHOIS_QUERY) << "RIPE: Open " << _host << ":" << _port << " via Proxy " << proxy_host << ":" << proxy_port << " source=" << _sources << " protocol="
				<< protocolName[dflt_protocol].name << endl;

		char proxybuf[RIPE_BUFFER_SIZE + 1];
		ipAddr cServer(IRR::proxy_host);
		set_dstInaddr(cServer.getInaddr());
		setPort(IRR::proxy_port);

		if (connect() < 0) {
#ifdef DIAG
			err_msg = new char[1024];
			sprintf(err_msg, "Connection to proxy %s on port %d failed!", proxy_host, proxy_port);
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
		    cerr << "Connection request to proxy " << proxy_host << " on port " << proxy_port << " could not be sent!" << endl;
		    exit(1);
#endif /* DIAG */
		}
		if (Socket::wait_for_reply(response_buffer, RIPE_BUFFER_SIZE, responseTimeout) < 0) {
#ifdef DIAG
			err_msg = new char[1024];
			sprintf(err_msg, "Connection response from proxy %s on port %d could not be read!", proxy_host, proxy_port);
			die_error(err_msg, 0);
#else
		    cerr << "Connection response from proxy " << proxy_host << " on port " << proxy_port << " could not be read!" << endl;
		    exit(1);
#endif /* DIAG */
		}

		if (0 == memcmp(response_buffer, "HTTP/1.0 ", 9) || 0 == memcmp(response_buffer, "HTTP/1.1 ", 9)) {
			// got an HTTP response!
			char *respbufptr = response_buffer + 9;
			if (0 != memcmp(respbufptr, "200 ", 4)) {
				// Proxy response is NOT an "ok"!
				char *lf = strchr(response_buffer, '\n');
				if (lf != NULL) {
					lf++;
					*lf = '\0';
				}
#ifdef DIAG
				err_msg = new char[RIPE_BUFFER_SIZE + strlen(response_buffer)];
				sprintf(err_msg, "Connect process via proxy %s on port %d failed! Proxy reported:\n%s", proxy_host, proxy_port, response_buffer);
				die_error(err_msg, 0);
#else
				cerr << "Connect process via proxy " << proxy_host << " on port " << proxy_port << " failed! Proxy reported:" << endl;
				cerr << response_buffer << endl;
				exit(1);
#endif /* DIAG */
			} else {
				// Proxy response is an "ok"! We don't need the rest, so clear response_buffer now.
				*response_buffer = '\0';
			}
		} else {
			// unknown response from Proxy
#ifdef DIAG
			err_msg = new char[RIPE_BUFFER_SIZE + strlen(response_buffer)];
			sprintf(err_msg, "Connect process via proxy %s on port %d failed! Unexpected response:\n%s", proxy_host, proxy_port, response_buffer);
			die_error(err_msg, 0);
#else
			cerr << "Connect process via proxy " << proxy_host << " on port " << proxy_port << " failed! Unexpected response:" << endl;
			cerr << response_buffer << endl;
			exit(1);
#endif /* DIAG */
		}
	} else {
		// Open connection directly
		Trace(TR_WHOIS_QUERY) << "RIPE: Open " << _host << ":" << _port << " source=" << _sources << " protocol=" << protocolName[dflt_protocol].name << endl;

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

void RipeWhoisClient::Close() {
	Trace(TR_WHOIS_QUERY) << "RIPE: Close " << getsock() << endl;
}

bool RipeWhoisClient::sendQuery(const char *pzcQuery, ...) {
	char pzcBuffer[RIPE_BUFFER_SIZE + 1];
	char pzcFormat[512];
	if (sources && *sources)
		sprintf(pzcFormat, "-k -V %s -r -s %s %s\n", ProjectVersion, sources, pzcQuery);
	else
		sprintf(pzcFormat, "-k -V %s -r %s\n", ProjectVersion, pzcQuery);

	va_list ap;
	va_start(ap, pzcQuery);
	vsprintf(pzcBuffer, pzcFormat, ap);
	va_end(ap);

	int iLen = strlen(pzcBuffer);

#ifdef DIAG
	delete last_query;
	last_query = new char[iLen + 1];
	*last_query = '\0';
	strcat(last_query, pzcBuffer);
	last_query[iLen - 1] = '\0';
#endif /* DIAG */

	Trace(TR_WHOIS_QUERY) << "RIPE: Send query [" << pzcBuffer << "] length= " << iLen << endl;

	// Send the query out of wire
	if (write(pzcBuffer, iLen) < 0) {
#ifdef DIAG
		err_occ = 1;
		delete err_msg;
		err_msg = new char[iLen + 128];
		sprintf(err_msg, "Sending query \"%s\" failed!", last_query);
#endif /* DIAG */
		return false;
	}
	return true;
}

bool RipeWhoisClient::getResponse(char *&text, int &len) {
	char *response = (char *) malloc(1);
	char *resp_ptr = NULL;
	char *term_ptr = NULL;
	char *fail_ptr = NULL;
	bool found = false; // complete response (all parts) read
	int bytes = -1; // number of bytes read from socket
	int count = 0; // number of response parts
	int retry = 0; // number of retries until MAX_NO_OF_RETRIES

	*response = '\0';
	// if there is a rest left from a former response stuff it in this response
	if (strlen(response_buffer) > 0) {
		response = (char*) realloc(response, strlen(response_buffer) + 1);
		strcat(response, response_buffer);
	}
	Trace(TR_WHOIS_RESPONSE) << "RIPE: old response [" << response << "] length= " << strlen(response) << endl;

	// read the complete response
	while (!found) {
		// read in the new response from socket
		count++;
		response_buffer[0] = '\0';
		bytes = Socket::wait_for_reply(response_buffer, RIPE_BUFFER_SIZE, responseTimeout);
		Trace(TR_WHOIS_RESPONSE) << "RIPE: wait_for_reply: bytes= " << bytes << ", count= " << count << endl;

		if (bytes > 0) {
			retry = 0;

			// copy received response completely
			response_buffer[bytes] = '\0';
			response = (char*) realloc(response, strlen(response) + bytes + 1);
			strncat(response, response_buffer, bytes);
			response_buffer[0] = '\0';

			// check for correct response termination
			resp_ptr = strstr(response, RIPE_RESPONSE);
			if (resp_ptr != NULL) {
				term_ptr = strstr(resp_ptr, RIPE_TERMINATION);
				if (term_ptr != NULL) {
					found = true;
					while (*term_ptr == '\n') {
						term_ptr++;
					}
				}
			}
		} else if (bytes < 0) {
			// an error occurred while reading the socket
#ifdef DIAG
			err_occ = 2;
			err_msg = new char[strlen(last_query) + 128];
			sprintf(err_msg, "Reading response for query \"%s\" failed!", last_query);
#endif /* DIAG */
			free(response);
			response = NULL;
			return false;
		} else {
			if (retry < responseRetries) {
				// no response at this time, retrying...
				retry++;
				cerr << "RIPE: No response for query \"" << last_query << "\" after " << responseTimeout << " seconds; retry " << retry << "..." << endl;
			} else {
				// handle responses that terminate incorrectly
#ifdef DIAG
				err_occ = 2;
				err_msg = new char[strlen(last_query) + 128];
				sprintf(err_msg, "Response did not terminate correctly for query \"%s\"", last_query);
#endif /* DIAG */
				free(response);
				response = NULL;
				return false;
			}
		}
	}
	Trace(TR_WHOIS_RESPONSE) << "RIPE: Response [" << response << "] retries= " << retry << ", found= " << found << endl;

	// save the rest of response_buffer for the next query
	if (strlen(term_ptr) > 0) {
		strcat(response_buffer, term_ptr);
		*term_ptr = '\0';
	}
	Trace(TR_WHOIS_RESPONSE) << "RIPE: completely response [" << response << "] length= " << strlen(response) << endl;
	Trace(TR_WHOIS_RESPONSE) << "RIPE: next response buffer [" << response_buffer << "] length= " << strlen(response_buffer) << endl;

	// handle responses carrying error messages
	// keep connected:
	//    101: no entries found
	//    106: no search key specified
	//    114: unsupported query
	// disconnected:
	//    305: connection has been closed
	//    306: connections exceeded
	//
	fail_ptr = strstr(response, RIPE_FAILURE);
	if ((fail_ptr != NULL) && (strstr(fail_ptr, RIPE_TERMINATION) != NULL)) {
		fail_ptr += strlen(RIPE_FAILURE);
		int fail_num = atoi(fail_ptr);
		switch (fail_num) {
			case 101:
#ifdef DIAG
				if (irr_warnings) {
					// No key found error. Give a warning.
					err_msg = new char[strlen(last_query) + 128];
					sprintf(err_msg, "No entries found for query \"%s\"!", last_query);
					warn_error();
				}
				extractRPSLInfo( last_query);
#endif /* DIAG */
				break;
			default:
				err_occ = 2;
				resp_ptr = strstr(fail_ptr, " ");
				while (*resp_ptr == ' ') {
					resp_ptr++;
				}
				term_ptr = strstr(fail_ptr, "\n");
				if ((resp_ptr != NULL) && (term_ptr != NULL)) {
					*term_ptr = '\0';
#ifdef DIAG
					err_msg = new char[strlen(last_query) + strlen(term_ptr) + 128];
					sprintf(err_msg, "%i (%s) for query \"%s\"!", fail_num, resp_ptr, last_query);
#else
					cerr << "RIPE: " << fail_num << " (" << resp_ptr << ") for query \"" << last_query << "\"!" << endl;
#endif /* DIAG */
				} else {
#ifdef DIAG
					err_msg = new char[strlen(last_query) + 128];
					sprintf(err_msg, "%i (no description) for query \"%s\"!", fail_num, last_query);
#else
					cerr << "RIPE: " << fail_num << " (no description) for query \"" << last_query << "\"!" << endl;
#endif /* DIAG */
				}
		}

		free(response);
		response = NULL;
		return false;
	}

	// remove comments from response
	unsigned long rest_size = strlen(response);
	unsigned long jump = 0;
	resp_ptr = response;

	while ((*resp_ptr == '%' || *resp_ptr == '\n') && rest_size > 0) {
		// skip one line
		if (*resp_ptr == '\n') {
			jump = 1;
		} else {
			char *eol = (char *) memchr(resp_ptr, '\n', rest_size);
			if (eol == NULL) {
				jump = rest_size;
			} else {
				jump = eol - resp_ptr + 1;
			}
		}
		resp_ptr += jump;
		rest_size -= jump;
	}

	if (rest_size == 0) {
#ifdef DIAG
		if (irr_warnings) {
			err_msg = new char[strlen(last_query) + 128];
			sprintf(err_msg, "No data in response for query \"%s\"!", last_query);
			warn_error();
		}
		extractRPSLInfo( last_query);
#else
#endif /* DIAG */
		free(response);
		response = NULL;
		return false;
	}
	if (resp_ptr != response) {
		response = (char *) memmove(response, resp_ptr, rest_size);
		response[rest_size] = '\0';
	}

	Trace(TR_WHOIS_RESPONSE) << "RIPE: Response[" << string(response, 0, rest_size) << "]" << endl;

	text = response;
	len = rest_size;
	return true;
}

bool RipeWhoisClient::getAutNum(char *as, char *&text, int &len) {
	if (!sendQuery("-T aut-num %s", as))
		return false;
	if (!getResponse(text, len))
		return false;
	return true;
}

bool RipeWhoisClient::getSet(SymID sname, const char *clss, char *&text, int &len) {
	if (!sendQuery("-T %s %s", clss, sname))
		return false;
	if (!getResponse(text, len))
		return false;
	return true;
}

bool RipeWhoisClient::getRoute(char *rt, char *as, char *&text, int &len) {
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
	unsigned int asn = atoi(as + 2);

	for (ListIterator<Object> o(objs); o; ++o) {
		AttrGenericIterator<ItemASNO> i(o, "origin");
		if (i && i->asno == asn) {
			text = (char *) malloc(o->size);
			strncpy(text, o->contents, o->size);
			len = o->size;
			// we may have a leak here
			free(response);
			return true;
		}
	}

	return false;
}

bool RipeWhoisClient::getInetRtr(SymID inetrtr, char *&text, int &len) {
	if (!sendQuery("-T inet-rtr %s", inetrtr))
		return false;
	if (!getResponse(text, len))
		return false;
	return true;
}

// REIMP
bool RipeWhoisClient::expandAS(char *as, MPPrefixRanges *result) {
	if (!sendQuery("-K -i origin %s", as))
		return false;
	char *pzcKeys;
	int iKeyLen;
	if (getResponse(pzcKeys, iKeyLen)) {
		assert(*(pzcKeys + iKeyLen - 1) == '\n');
		*(pzcKeys + iKeyLen) = 0;
		char *p = pzcKeys;
		char *prefix;
		char *begin;
		while (p && p < pzcKeys + iKeyLen) {
			begin = strstr(p, "route:");
			if (!begin) {
				begin = strstr(p, "route6:");
			}
			if (!begin)
				break;
			p = begin;
			// locate 1st ":"
			p = strstr(p, ":") + 1; // we skip one extra character for ':' or '@' depending on the protocol
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

bool RipeWhoisClient::getIndirectMembers(char *setName, const Object *o, char *query, void(*collect)(void *, const Object *), void *collectArg) {
	char finalQuery[1024];
	bool isAny = false;
	if (!strcasecmp(setName, "rs-any") || !strcasecmp(setName, "as-any"))
		isAny = true;

	AttrGenericIterator<ItemWORD> mbrs_by_ref(o, "mbrs-by-ref");
	if (!isAny && !mbrs_by_ref)
		return false;

	// if mbrs_by_ref contains any, then we only needs keys
	for (mbrs_by_ref.first(); mbrs_by_ref && !isAny; mbrs_by_ref++)
		isAny = !strcasecmp(mbrs_by_ref()->word, "any");

	if (isAny)
		strcpy(finalQuery, "-K ");
	else
		finalQuery[0] = 0;
	strcat(finalQuery, query);

	if (!sendQuery(finalQuery))
		return false;

	char *response;
	int len;
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

bool RipeWhoisClient::expandRSSet(SymID sname, MPPrefixRanges *result) {

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

bool RipeWhoisClient::expandASSet(SymID sname, SetOfUInt *result) {
	const Set *set = IRR::getASSet(sname);
	AttrGenericIterator<Item> itr(set, "members");
	for (Item *pt = itr.first(); pt; pt = itr.next())
		if (typeid(*pt) == typeid(ItemASNAME)) { // ASNAME (aka as-set)
			const SetOfUInt *tmp = IRR::expandASSet(((ItemASNAME *) pt)->name);
			if (tmp)
				*result |= *(SetOfUInt *) tmp;
		} else if (typeid(*pt) == typeid(ItemASNO))
			result->add(((ItemASNO *) pt)->asno);
		else
			assert(false);
	;

	char buffer[1024];
	sprintf(buffer, "-T aut-num -i member-of %s", sname);
	getIndirectMembers(sname, set, buffer, collectASNO, result);

	return true;
}

bool RipeWhoisClient::expandRtrSet(SymID sname, MPPrefixRanges *result) {
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

void RipeWhoisClient::querySourceOrigin(const char *rt) {
	// ripev3 support
	if (!strcasecmp(protocol, "ripe"))
		sendQuery("-a -x -T route %s", rt);
	else
		sendQuery("-a -T route -i route %s", rt);
}

ASt RipeWhoisClient::getOrigin(char *format, ...) {
	char buffer[64];
	va_list ap;
	va_start(ap, format);
	vsprintf(buffer, format, ap);
	va_end(ap);

	// ripev3 support
	if (!strcasecmp(protocol, "ripe")) {
		if (!sendQuery("-L -T route %s", buffer))
			return 0;
	} else if (!sendQuery("-L -T route -i route %s", buffer))
		return 0;

	ASt tResult = 0;
	char *pzcResult;
	int iResultLen;
	if (getResponse(pzcResult, iResultLen)) {
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
#endif /* RIPE */
