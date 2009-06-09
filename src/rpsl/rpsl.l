/*
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
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>
//             Katie Petrusha <katie@ripe.net>
*/

/* definitions */
%x STRING
%x EXPECT_OPERATOR
%x FREETEXT
%x BLOBMODE
%x GENERICMODE
%x SKIP_OBJECT
%x POLICYMODE

/*%option nodefault /* */
/*%option debug /* */
%option case-insensitive

ENDOBJECT      ^[ \t\r]*\n
WS             [ \t\r]
NWS            [^ \t\r\n]
INT            [[:digit:]]+
SINT           [+-]?{INT}
REAL           [+-]?{INT}?\.{INT}({WS}*E{WS}*[+-]?{INT})?
NAME           [[:alpha:]]([[:alnum:]_-]*[[:alnum:]])?
ASNO           [Aa][Ss]({INT}|{INT}\.{INT})
ASNAME         AS-[[:alnum:]_-]*[[:alnum:]]
RSNAME         RS-[[:alnum:]_-]*[[:alnum:]]
RTRSNAME       RTRS-[[:alnum:]_-]*[[:alnum:]]
PRNGNAME       PRNG-[[:alnum:]_-]*[[:alnum:]]
FLTRNAME       FLTR-[[:alnum:]_-]*[[:alnum:]]
IPV4           [0-9]+(\.[0-9]+){3,3}
PRFXV4         {IPV4}\/[0-9]+
PRFXV4RNG      {PRFXV4}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
QUAD           [0-9A-F]{1,4}
IPV6           {QUAD}(:{QUAD}){7}
IPV6DC         (({QUAD}:){0,6}{QUAD})?::({QUAD}(:{QUAD}){0,6})?
PRFXV6         {IPV6}\/{INT}
PRFXV6DC       {IPV6DC}\/{INT}
PRFXV6RNG      {PRFXV6}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
PRFXV6DCRNG    {PRFXV6DC}("^+"|"^-"|"^"{INT}|"^"{INT}-{INT})
ENAMECHAR      [^()<>,;:\\\"\.[\] \t\r]
ENAME          ({ENAMECHAR}+(\.{ENAMECHAR}+)*\.?)|(\"[^\"@\\\r\n]+\")
DNAME          [[:alnum:]_-]+
EMAIL          {ENAME}@({DNAME}("."{DNAME})+|{IPV4})
YYYYMMDD       [0-9]{8,8}
HHMMSS         [0-9][0-9]:[0-9][0-9]:[0-9][0-9]
HHMM           [0-9][0-9]:[0-9][0-9]

%{
#include "config.h"
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <time.h>
#include <limits.h>
extern "C" {
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
}

#include "object.hh"
#include "rpsl.y.hh"
#include "schema.hh"

#ifndef HAVE_TIMEGM
extern long int timezone;
#endif

static int errorLine = 0;
static int errorColon = 0;
#define errorNewLine { errorLine++;   errorColon = 0; }

static int scan_position = 0;
static bool start_of_object = true;
static bool genericMode = true;
static int oldStart = 0;

Object *current_object;
Attr   *current_attribute;
YY_BUFFER_STATE rpsl_buffer = (YY_BUFFER_STATE) NULL;

#define MAX_STR_CONST 1024
#define LEXER_RETURN return
#define YY_USER_ACTION { scan_position += yyleng; errorColon += yyleng; }
#define YYLESS(n) {scan_position -= yyleng - n; errorColon -= yyleng - n; yyless(n); }
#define yylval rpsllval
#define yy_scan_object rpsl_scan_object
#define yyschema schema

extern void handle_error(const char *, ...);
extern void handle_warning(const char *, ...);
%}

%%
%{ 
   /* this code is copied to beginning of yylex */
   static char string_buf[MAX_STR_CONST];
   char *string_buf_ptr;
%}

^{NAME}: { //
   errorLine = 0; 
   errorColon = yyleng;

   *(yytext + yyleng - 1) = 0; // remove ':'

   yylval.attr = new Attr;
   yylval.attr->offset = scan_position - yyleng;
   current_attribute = yylval.attr;

   if (start_of_object) {
      start_of_object = false;
      current_object->type = yyschema.searchClass(yytext);
      if (!current_object->type) {
	 handle_error("Error: unknown class \"%s\"\n", yytext);
	 yylval.attr->type   = NULL;
//(*current_object) += yylval.attr;
	 BEGIN(SKIP_OBJECT);
	 LEXER_RETURN(TKN_UNKNOWN_CLASS);
      }
   }

   yylval.attr->type   = current_object->type->searchAttr(yytext);
   yylval.attr->object = current_object;

//   (*current_object) += yylval.attr;

   if (! yylval.attr->type) {
      if (! schema.isForgiving())
	 handle_error("Error: unknown attribute %s of class %s\n",
		      yytext, current_object->type->name);
      BEGIN(FREETEXT);
      LEXER_RETURN(ATTR_FREETEXT);
   }

   if (yylval.attr->type->isObsolete())
      handle_warning("Warning: attribute %s of class %s is now obsolete\n", 
	      yytext, current_object->type->name);

   if (!yylval.attr->type->shouldParse()) { // may be free text?
      BEGIN(FREETEXT);
      LEXER_RETURN(ATTR_FREETEXT);
   } else {
      if (yylval.attr->type->syntax() == ATTR_GENERIC) {
	 genericMode = true;
	 BEGIN(GENERICMODE);
      } else if (yylval.attr->type->syntax() == ATTR_BLOBS) {
	 genericMode = true;
	 BEGIN(BLOBMODE);
      } else if (yylval.attr->type->syntax() == ATTR_REGEXP) {
	 genericMode = true;
	 BEGIN(FREETEXT);
      } else {
	 genericMode = false;
	 BEGIN(POLICYMODE);
      }
      LEXER_RETURN(yylval.attr->type->syntax());
   }
}

{ENDOBJECT}  {
   if (current_attribute)
      current_attribute->len = scan_position - current_attribute->offset;
   LEXER_RETURN(TKN_EOO);
}

<<EOF>> {
   yy_delete_buffer(rpsl_buffer);
   rpsl_buffer = (YY_BUFFER_STATE) NULL;
   if (current_attribute)
      current_attribute->len = scan_position - current_attribute->offset;
   BEGIN(INITIAL);
   yyterminate();
}

#.*

. { // not a valid line
   current_attribute = NULL;
   handle_error("Error: valid lines start with attribute names, spaces or #\n");
   BEGIN(FREETEXT);
   LEXER_RETURN(TKN_ERROR);
}

<SKIP_OBJECT>{
  {ENDOBJECT} {
     BEGIN(INITIAL);
     if (current_attribute)
        current_attribute->len = scan_position - current_attribute->offset;
     LEXER_RETURN(TKN_EOO);
  }
  \n          errorNewLine;
  .+          ;
}

<FREETEXT,BLOBMODE,GENERICMODE,POLICYMODE,EXPECT_OPERATOR>{
   \n\+             errorNewLine; errorColon++; // line continuation with +
   \n/[ \t\r#]      errorNewLine; // line continuation with space or comment
   \n/([ \t\r]+\n)? { // end of attribute and may be end of object too
      BEGIN(INITIAL);
      if (current_attribute)
	 current_attribute->len = scan_position - current_attribute->offset;
      LEXER_RETURN(TKN_EOA);
   }
   {WS}+            ; // skip white space
   #.*              ; // skip comment 
}

<FREETEXT>{     // dont parse anything, keep going till EOA
  .+          ; // this rule needs to be after \n rules
}

<BLOBMODE>{
  {NWS}+      {
     yylval.string = strdup(yytext);
     LEXER_RETURN(TKN_BLOB);
  }
}

<GENERICMODE>{
  {YYYYMMDD}{WS}+{HHMMSS}{WS}+[-+]{HHMM} {
     tm ts;
     bzero((char *) &ts, sizeof(ts));

     int date, zone_hours, zone_mins;
     sscanf(yytext, "%d %d:%d:%d %d:%d", 
		    &date, 
		    &ts.tm_hour, &ts.tm_min, &ts.tm_sec,
		    &zone_hours, &zone_mins);

     ts.tm_mday = date % 100; date /= 100;
     ts.tm_mon  = date % 100 - 1; date /= 100;
     ts.tm_year = date - 1900;
   
     if (ts.tm_mday > 31) {
	handle_error("Error: Invalid day %d\n", ts.tm_mday);
	LEXER_RETURN(TKN_ERROR);
     }
     if (ts.tm_mon > 11) {
	handle_error("Error: Invalid month %d\n", ts.tm_mon);
	LEXER_RETURN(TKN_ERROR);
     }
     if (ts.tm_year < 70) {
	handle_error("Error: Invalid year %d\n", ts.tm_year);
	LEXER_RETURN(TKN_ERROR);
     }
     if (ts.tm_sec > 59) {
	handle_error("Error: Invalid sec %d\n", ts.tm_sec);
	LEXER_RETURN(TKN_ERROR);
     }
     if (ts.tm_min > 59) {
	handle_error("Error: Invalid min %d\n", ts.tm_min);
	LEXER_RETURN(TKN_ERROR);
     }
     if (ts.tm_hour > 23) {
	handle_error("Error: Invalid hour %d\n", ts.tm_hour);
	LEXER_RETURN(TKN_ERROR);
     }
     if (zone_mins > 59) {
	handle_error("Error: Invalid zone min %d\n", zone_mins);
	LEXER_RETURN(TKN_ERROR);
     }
     if (zone_hours > 23) {
	handle_error("Error: Invalid zone hour %d\n", zone_hours);
	LEXER_RETURN(TKN_ERROR);
     }

     // this fiddling with TZ is necessary for mktime to use utc as timezone
     // it sucks, there's got to be a better way
     time_t result;
#ifdef HAVE_TIMEGM
     result = timegm(&ts);
#else
     result = mktime(&ts);
     result -= timezone;
#endif
/*
     char *TZ=getenv("TZ");
     if (TZ)
	TZ = strdup(TZ);
     setenv("TZ", "", 1);
     time_t result = mktime(&ts);
     if (TZ) {
	setenv("TZ", TZ, 1);
	free(TZ);
     } else
	unsetenv("TZ");
*/
     if (zone_hours < 0)
	zone_mins *= -1;

     result += zone_mins * 60 + zone_hours * 60 * 60;
   
     yylval.time = result;
     LEXER_RETURN(TKN_TIMESTAMP);
  }
}

<GENERICMODE,POLICYMODE>{  
  {SINT} {
     yylval.i = strtoll(yytext, NULL, 10);
     LEXER_RETURN(TKN_INT);
  }

  {PRFXV6DCRNG}  {
     yylval.prfxv6rng = new IPv6PrefixRange(yytext);
     if (yylval.prfxv6rng->valid()) {
       LEXER_RETURN(TKN_PRFXV6RNG);
     }
     else 
       LEXER_RETURN(TKN_ERROR);
  }

  {PRFXV6RNG}  {
     yylval.prfxv6rng = new IPv6PrefixRange(yytext);
     if (yylval.prfxv6rng->valid()) {
       LEXER_RETURN(TKN_PRFXV6RNG);
     }
     else 
       LEXER_RETURN(TKN_ERROR);
  }

  {PRFXV6DC}  {
     yylval.prfxv6 = new IPv6Prefix(yytext);
     if (yylval.prfxv6->valid()) {
       LEXER_RETURN(TKN_PRFXV6);
     }
     else
       LEXER_RETURN(TKN_ERROR);
  }

  {PRFXV6}  {
     yylval.prfxv6 = new IPv6Prefix(yytext);
     if (yylval.prfxv6->valid()) {
       LEXER_RETURN(TKN_PRFXV6);
     }
     else
       LEXER_RETURN(TKN_ERROR);
  }

  {IPV6DC}  {
     yylval.ipv6 = new IPv6Addr(yytext);
     if (yylval.ipv6->valid()) {
       LEXER_RETURN(TKN_IPV6);
     }
     else
       LEXER_RETURN(TKN_ERROR);
  }

  {IPV6}  {
     yylval.ipv6 = new IPv6Addr(yytext);
     if (yylval.ipv6->valid()) {
       LEXER_RETURN(TKN_IPV6);
     }
     else
       LEXER_RETURN(TKN_ERROR);
  }

  {INT}:{INT} {
     unsigned int ms, ls;
     char *colon = strchr(yytext, ':');
     *colon = 0;
     ms = atoi(yytext);
     ls = atoi(colon+1);
     if (ms > 65535 || ls > 65535)
	LEXER_RETURN(TKN_ERROR);

     yylval.i = (ms << 16) + ls;
     LEXER_RETURN(TKN_INT);
  }

  {REAL} {
     yylval.real = atof(yytext);
     LEXER_RETURN(TKN_REAL);
  }

  \"  {
     string_buf_ptr = string_buf; 
     oldStart = YY_START;
     BEGIN(STRING);
  }

  {IPV4}    {
     yylval.ip = new IPAddr(yytext);
     LEXER_RETURN(TKN_IPV4);
  }

  {PRFXV4}  {
     yylval.prfx = new Prefix(yytext);
     LEXER_RETURN(TKN_PRFXV4);
  }

  {PRFXV4RNG}  {
     yylval.prfxrng = new PrefixRange(yytext);
     LEXER_RETURN(TKN_PRFXV4RNG);
  }

  /* The order of these rules are important */
  {ASNO} {
     const char *dot = strchr(yytext,'.');
	 if (dot)
	   yylval.i = atoi(yytext+2)<<16 | atoi(dot+1);
	 else
	   yylval.i = atoi(yytext+2);
     LEXER_RETURN(TKN_ASNO);
  }

  (({ASNO}|peeras|{ASNAME}):)*{ASNAME}(:({ASNO}|peeras|{ASNAME}))* {
     yylval.sid = symbols.symID(yytext);
     LEXER_RETURN(TKN_ASNAME);
  }

  (({ASNO}|peeras|{RSNAME}):)*{RSNAME}(:({ASNO}|peeras|{RSNAME}))*  {
     yylval.sid = symbols.symID(yytext);
     LEXER_RETURN(TKN_RSNAME);
  }

  (({ASNO}|peeras|{RTRSNAME}):)*{RTRSNAME}(:({ASNO}|peeras|{RTRSNAME}))*  {
     yylval.sid = symbols.symID(yytext);
     LEXER_RETURN(TKN_RTRSNAME);
  }

  (({ASNO}|peeras|{PRNGNAME}):)*{PRNGNAME}(:({ASNO}|peeras|{PRNGNAME}))*  {
     yylval.sid = symbols.symID(yytext);
     LEXER_RETURN(TKN_PRNGNAME);
  }

  (({ASNO}|peeras|{FLTRNAME}):)*{FLTRNAME}(:({ASNO}|peeras|{FLTRNAME}))*  {
     yylval.sid = symbols.symID(yytext);
     LEXER_RETURN(TKN_FLTRNAME);
  }

  true {
     yylval.i = 1;
     LEXER_RETURN(TKN_BOOLEAN);
  }

  false {
     yylval.i = 0;
     LEXER_RETURN(TKN_BOOLEAN);
  }

  {NAME} { /* reserved word or rp-attribute, or arbitrary name */
     if (!genericMode) {
	oldStart = YY_START;

	int index;
	if ((index  = yyschema.searchKeyword(yytext)) >= 0) {
	   if (index == KEYW_OPERATOR)
	      BEGIN(EXPECT_OPERATOR);
	   LEXER_RETURN(index);
	}
     
	if ((yylval.rp_attr = yyschema.searchRPAttr(yytext)) != NULL) {
	   BEGIN(EXPECT_OPERATOR);
	   LEXER_RETURN(TKN_RP_ATTR);
	}
     }

     yylval.string = strdup(yytext);

     // in case of afi
     RPType *type1 = schema.searchTypedef("address_family");
     
     if (   (type1 != NULL) 
         && (type1->validate(new ItemWORD(yytext))) ) {
          yylval.afi = new AddressFamily(yytext);
          if (strcasecmp(yytext, "any") == 0)
             LEXER_RETURN(KEYW_ANY);
          else
             LEXER_RETURN(TKN_AFI);
     }  

     LEXER_RETURN(TKN_WORD);
  }

  {NAME}"."{NAME}  { /* rp-attribute.method or DNS name */
     if (!genericMode) {
	char *p = strchr(yytext, '.');
	*p = 0;

	if ((yylval.rp_attr = yyschema.searchRPAttr(yytext)) != NULL) {
	   *p = '.';
	   YYLESS((p - yytext));
	   LEXER_RETURN(TKN_RP_ATTR);
	}

	*p = '.';
     }

     // in case of afi containing "."
     char *p = strchr(yytext, '.');
     if (p != NULL) {
       int a_len = p - yytext + 1;
       if (a_len > 1) {
         char *a = (char *) malloc(a_len);
         memcpy(a, yytext, (a_len - 1));
         a[a_len-1] = '\0';

         RPType *type1 = schema.searchTypedef("address_family");
         RPType *type2 = schema.searchTypedef("address_sub_family");
         if ((type1 != NULL) && (type2 != NULL) && 
             (type1->validate(new ItemWORD(a))) && 
             (type2->validate(new ItemWORD(p+1))) ) {
           free (a);
           yylval.afi = new AddressFamily(yytext);
           LEXER_RETURN(TKN_AFI);
         }
         free(a);
       }
     }

     yylval.sid = symbols.symID(yytext);
     LEXER_RETURN(TKN_DNS);
  }

  {DNAME}("."{DNAME})+ { /* DNS name */
     yylval.sid = symbols.symID(yytext);
     LEXER_RETURN(TKN_DNS);
  }

  {EMAIL} { /* email */  
     yylval.string = strdup(yytext);
     LEXER_RETURN(TKN_EMAIL);
  }

  . {
     if (*yytext <= 0) {
	handle_error("Error: Non-ASCII character encountered\n");
	LEXER_RETURN(TKN_ERROR);
     } else
	LEXER_RETURN(*yytext);
  }
}

<POLICYMODE>{
   \^- {
      yylval.moreSpecOp = new FilterMS(0);
      LEXER_RETURN(OP_MS);
   }

   \^\+ {
      yylval.moreSpecOp = new FilterMS(1);
      LEXER_RETURN(OP_MS);
   }

   \^{INT} {
      int i;
      sscanf(yytext, "^%d", &i);
      yylval.moreSpecOp = new FilterMS(2, i, i);
      LEXER_RETURN(OP_MS);
   }

   \^{INT}-{INT} {
      int i, j;
      sscanf(yytext, "^%d-%d", &i, &j);
      yylval.moreSpecOp = new FilterMS(2, i, j);
      LEXER_RETURN(OP_MS);
   }

   "..." LEXER_RETURN(TKN_3DOTS);
}

<EXPECT_OPERATOR>{
   (("<<"|">>`"|"+"|"-"|"*"|"/"|"."|"="|"!"|"<"|">")?=|"<"|">"|"()"|"[]") {
      yylval.string = strdup(yytext);
      BEGIN(oldStart);
      LEXER_RETURN(TKN_OPERATOR);
   }
   
   . {
      YYLESS(0); 
      BEGIN(oldStart);
   }
}

<STRING>{
   \" { /* saw closing quote - all done */
      BEGIN(oldStart);
      *string_buf_ptr = '\0';
      yylval.string = strdup(string_buf);
      LEXER_RETURN(TKN_STRING);
   }
   
   \n {
      errorNewLine;
      handle_error("Error: unterminated string encountered\n");
      BEGIN(oldStart);
      if (current_attribute)
	 current_attribute->len = scan_position - current_attribute->offset;
      LEXER_RETURN(TKN_EOA);
   }

   \\[0-7]{1,3} {
      /* octal escape sequence */
      int result;
      
      (void) sscanf( yytext + 1, "%o", &result );

      if ( result > 0xff ) {
	 handle_error("Error: octal escape sequence is out of bounds\n");
	 BEGIN(oldStart);
	 LEXER_RETURN(TKN_ERROR);
      }

      *string_buf_ptr++ = result;
   }

   \\[0-9]+ {			/* like '\48' or '\0777777' */
      handle_error("Error: bad octal escape sequence\n");
      BEGIN(oldStart);
      LEXER_RETURN(TKN_ERROR);
   }

   \\n  *string_buf_ptr++ = '\n';
   \\t  *string_buf_ptr++ = '\t';
   \\r  *string_buf_ptr++ = '\r';
   \\b  *string_buf_ptr++ = '\b';
   \\f  *string_buf_ptr++ = '\f';

   \\(.|\n)  *string_buf_ptr++ = yytext[1];
   
   [^\\\n\"]+        {
      char *yptr = yytext;
      while ( *yptr )
	 *string_buf_ptr++ = *yptr++;
   }

   .  *string_buf_ptr++ = yytext[0];
}

%%
extern "C" {
int yywrap () {
   return 1;
}
}

void handle_error(const char *s, ...) {
   current_object->has_error = true;

   char buffer[1024] = "***";
   va_list ap;
   va_start(ap, s);
   vsprintf(buffer+3, s, ap);
   va_end(ap);

   if (current_attribute)
      current_attribute->errors += buffer;
   else 
      current_object->errors += buffer;
}

void handle_object_error(const char *s, ...) {
   current_object->has_error = true;

   char buffer[1024] = "***";
   va_list ap;
   va_start(ap, s);
   vsprintf(buffer+3, s, ap);
   va_end(ap);

   current_object->errors += buffer;
}

void rpslerror(const char *, ...) {
   if (current_attribute) {
      current_attribute->errorLine = errorLine;
      current_attribute->errorColon = errorColon - yyleng;
      current_attribute->errorLeng  = yyleng;
   }
}

void handle_warning(const char *s, ...) {
   current_object->has_warning = true;

   char buffer[1024];
   va_list ap;
   va_start(ap, s);
   vsprintf(buffer, s, ap);
   va_end(ap);

   if (current_attribute) 
      current_attribute->errors += buffer;
   else 
      current_object->errors += buffer;
}

void yy_scan_object(Object *o) {
   errorLine = 0; 
   errorColon = 0;
   current_object = o;
   current_attribute = NULL;
   start_of_object = true;
   scan_position = 0;
   genericMode = true;

   if (rpsl_buffer)
      yy_delete_buffer(rpsl_buffer);
   rpsl_buffer = yy_scan_bytes(o->contents, (yy_size_t)o->size);
   BEGIN(INITIAL);
}

Attr *changeCurrentAttr(Attr *b) {
   if (current_attribute) {
      b->object = current_attribute->object;
      b->type   = current_attribute->type;
      b->offset = current_attribute->offset;;
      b->len    = current_attribute->len;
      b->errors = current_attribute->errors;
      b->errorLine  = current_attribute->errorLine;
      b->errorColon = current_attribute->errorColon;
      b->errorLeng  = current_attribute->errorLeng;
      delete current_attribute;
   }
   current_attribute = b;
   return b;
}
