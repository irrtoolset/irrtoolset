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
//  Author(s): Hagen Boehm <hboehm@brutus.nic.dtag.de>

#include "irreval.hh"
#include "f_ios.hh"
#include "f_iosxr.hh"
#include "f_junos.hh"
#include "f_rpsl.hh"
#include "config.h"
#include "irr/irr.hh"
#include "irr/classes.hh"
#include "irr/rawhoisc.hh"
// #include "irr/ripewhoisc.hh" ??????
#include "irrutil/debug.hh"
#include "irrutil/trace.hh"
#include "irrutil/rusage.hh"
#include "irrutil/Argv.hh"
#include "irrutil/version.hh"
#include "re2dfa/regexp_nf.hh"
#include "rpsl/schema.hh"
#include "normalform/NE.hh"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <cstdio>

using namespace std;

#ifdef DTAG
bool opt_asn16 = false;
#endif /* DTAG */
bool opt_asdot = false;
const int SIZE = 8 * 1024;
char *filter = (char *)calloc(SIZE, 1);
char *opt_prompt = (char *)"irreval> ";

const AttrRPAttr *IRReval::dctn_rp_pref = (AttrRPAttr *) NULL;
const AttrMethod *IRReval::dctn_rp_pref_set = (AttrMethod *) NULL;
const AttrRPAttr *IRReval::dctn_rp_nhop = (AttrRPAttr *) NULL;
const AttrMethod *IRReval::dctn_rp_nhop_set = (AttrMethod *) NULL;
const AttrRPAttr *IRReval::dctn_rp_dpa = (AttrRPAttr *) NULL;
const AttrMethod *IRReval::dctn_rp_dpa_set = (AttrMethod *) NULL;
const AttrRPAttr *IRReval::dctn_rp_med = (AttrRPAttr *) NULL;
const AttrMethod *IRReval::dctn_rp_med_set = (AttrMethod *) NULL;
const AttrRPAttr *IRReval::dctn_rp_community = (AttrRPAttr *) NULL;
const AttrMethod *IRReval::dctn_rp_community_setop= (AttrMethod *) NULL;
const AttrMethod *IRReval::dctn_rp_community_appendop=(AttrMethod*)NULL;
const AttrMethod *IRReval::dctn_rp_community_append= (AttrMethod*) NULL;
const AttrMethod *IRReval::dctn_rp_community_delete= (AttrMethod*) NULL;
const AttrRPAttr *IRReval::dctn_rp_aspath = (AttrRPAttr *) NULL;
const AttrMethod *IRReval::dctn_rp_aspath_prepend = (AttrMethod *) NULL;

BaseConfig *IRReval::configuration = (BaseConfig *) NULL;
char *IRReval::configFormat = (char *)"\0";
char *IRReval::configCacheDir = (char *)"\0";
int   IRReval::configCompression = COMP_NORMAL;
bool  IRReval::printEmptyLists = false;
bool  IRReval::configASN16 = false;
bool  IRReval::configASDot = false;
bool  IRReval::initialized = false;
char *IRReval::executableName = (char *)"irreval";
IRReval::FNameList *IRReval::rpkiFilenameList = (IRReval::FNameList *)NULL;
IRReval::FNameList *IRReval::rpslFilenameList = (IRReval::FNameList *)NULL;


IRReval::IRReval() {
   IRReval::configuration = new RPSLConfig();
   configFormat = (char *)"rpsl";
}

int start_tracing(char *dst, char *key, char *nextArg) {
   if (nextArg) {
      trace.enable(nextArg);
      return 1; // return 1 to signify nextArg is used by us
   }
   return 0; 
}

int start_debugging(char *dst, char *key, char *nextArg) {
   if (nextArg) {
      Debug(dbg.enable(atoi(nextArg)));
      return 1; // return 1 to signify nextArg is used by us
   }
   return 0;
}

int IRReval::select_config_format(char *dst, char *key, char *nextArg) {
   if (!nextArg) return 0;

   if (strcmp(nextArg, "rpsl") == 0) {
      return 1;// return 1 to signify nextArg is used by us
   }
   delete configuration;
   if (strcmp(nextArg, "ios") == 0) {
      configuration = new IOSConfig();
      configFormat = (char *)"ios";
      return 1;// return 1 to signify nextArg is used by us
   }
   if (strcmp(nextArg, "iosxr") == 0) {
      configuration = new IOSXRConfig();
      configFormat = (char *)"iosxr";
      return 1;
   }
   if (strcmp(nextArg, "junos") == 0) {
      configuration = new JUNOSConfig();
      configFormat = (char *)"junos";
      return 1;
   }

   cerr << "Error: configuration format \"" << nextArg << "\" is not supported" << endl;
   exit(1);

}

int IRReval::set_config_cache_dir(char *dst, char *key, char *nextArg) {
   if (!nextArg) return 0;

   configCacheDir = nextArg;
   return 1;// return 1 to signify nextArg is used by us
}

int IRReval::set_compression_format(char *dst, char *key, char *nextArg) {
   if (!nextArg) return 0;

   int i;
   for (i=0; compTypes[i].type != COMP_UNDEF; i++) {
     if (strcmp(nextArg, compTypes[i].name) == 0) {
       configCompression = compTypes[i].type;
       break;
     }
   }
   if (compTypes[i].type == COMP_UNDEF)
     cerr << "Warning: unknown level of prefix compression! Using COMP_NORMAL" << endl;

   return 1;
}

#ifdef DTAG
int IRReval::use_16bit_ASN(char *dst, char *key, char *nextArg) {
  configASN16 = true;
  configASDot = false;
  return 0; //signal that we don't use the next argument
}
#endif /* DTAG */

int IRReval::use_opt_ASDOT(char *dst, char *key, char *nextArg) {
  configASN16 = false;
  configASDot = true;
  return 0; //signal that we don't use the next argument
}

int IRReval::use_32bit_ASN(char *dst, char *key, char *nextArg) {
  configASN16 = false;
  configASDot = false;
  return 0; //signal that we don't use the next argument
}

#ifdef RPKI
int IRReval::add_rpki_filename(char *dst, char *key, char *nextArg) {
  if (!nextArg) return 0;

  FNameList *element = new FNameList;
  element->name = nextArg;
  element->next = rpkiFilenameList;
  rpkiFilenameList = element;
  return 1;
}
#endif /* RPKI */

int IRReval::add_rpsl_filename(char *dst, char *key, char *nextArg) {
  if (!nextArg) return 0;

  FNameList *element = new FNameList;
  element->name = nextArg;
  element->next = rpslFilenameList;
  rpslFilenameList = element;
  return 1;
}


void IRReval::setCompression() {
  configuration->setCompression(configCompression);
}

void IRReval::extractNameAndExpr(char *command, char *ctype, char *&name, char *&expr) {
  int length = strlen(ctype);
  char *startPtr = command+length;
  char *endPtr;

  if (isspace(*(startPtr)) || *startPtr == '\0') {
    // correct command, find a name
    while (isspace(*startPtr) && *startPtr != '\0') startPtr++;
    if (*startPtr == '\0') {
      cerr << "Error: no name/number and RPSL expression defined in \"" << command << "\"\n";
      exit(1);
    }
    endPtr = startPtr;
    while (!isspace(*endPtr) && *endPtr != '\0') endPtr++;
    if (*endPtr == '\0') {
      cerr << "Error: no RPSL expression defined in \"" << command << "\"\n";
      exit(1);
    }
    // found a name
    length = endPtr-startPtr;
    name = (char *)calloc(length+1,1);
    strncpy(name, startPtr, length);
    name[length] = '\0';

    startPtr = endPtr;
    while (isspace(*startPtr) && *startPtr != '\0') startPtr++;
    if (*startPtr == '\0') {
      cerr << "Error: no RPSL expression defined in \"" << command << "\"\n";
      exit(1);
    }
    // found an expression
    length = strlen(startPtr);
    expr = (char *)calloc(length+1,1);
    strncpy(expr, startPtr, length);
    expr[length] = '\0';

  } else {
    cerr << "Error: badly formed command \"" << command << "\"\n";
    exit(1);
  }

}

void IRReval::evaluate(char *command) {
  char *name;
  char *expr;
  char *ctype;

  if (! initialized) {
	  this->initialize();
	  initialized = true;
  }

  if (strcmp(configFormat, "ios") == 0 ||
      strcmp(configFormat, "junos") == 0 ||
      strcmp(configFormat, "iosxr") == 0) {

    // save command
    char *cmdptr = command;
    // jump over leading "@IRReval"
    cmdptr += 8;
    // jump over leading blanks
    while(isspace(*cmdptr))
        cmdptr++;

    ctype = (char *)"prefix-list";
    if (cmdptr == strstr(cmdptr, ctype)) {
      extractNameAndExpr(cmdptr, ctype, name, expr);
      IRReval::configuration->prefixListName = name;
      accessList(expr);
      return;
    }

    ctype = (char *)"as-path";
    if (cmdptr == strstr(cmdptr, ctype)) {
      extractNameAndExpr(cmdptr, ctype, name, expr);
      if (strcmp(configFormat, "iosxr") == 0) {
        IRReval::configuration->asPathName = name;
      } else {
        int asPathNum = atoi(name);
        if (!asPathNum) {
          cerr << "Error: badly formed as-path number in \"" << command << "\"\n";
          exit(1);
        }
        IRReval::configuration->asPathNumber = asPathNum;
      }
      aspathAccessList(expr);
      return;
    }

    cerr << "Error: unknown syntax used in \"" << command << "\" for vendor "
         << configFormat << "\npossible syntax:\n@IRReval prefix-list <name> <expression>\n@IRReval as-path <name> <expression>\n\n";
    exit(1);
  }
  if (strcmp(configFormat, "rpsl") == 0) {
    if (*command) {
      ctype = (char *)"route-set";
      if (command == strstr(command, ctype)) {
        extractNameAndExpr(command, ctype, name, expr);
        IRReval::configuration->prefixListName = name;
        ((RPSLConfig *)IRReval::configuration)->setRPSLExpr(expr);
        accessList(expr);
        ((RPSLConfig *)IRReval::configuration)->rsWrapUp();
        return;
      }

      ctype = (char *)"as-set";
      if (command == strstr(command, ctype)) {
        extractNameAndExpr(command, ctype, name, expr);
        IRReval::configuration->asPathName = name;
        ((RPSLConfig *)IRReval::configuration)->setRPSLExpr(expr);
        rpslASSet(expr);
        return;
      }

      cerr << "Error: unknown syntax used in \"" << command << "\" for format "
           << configFormat << "\npossible syntax:\nroute-set <name> <expression>\nas-set <name> <expression>\n\n";
      exit(1);
    } else {
      // command is empty. Nothing to do means return
      return;
    }
  }

}

void IRReval::evaluate_rpsl(char *command) {
  char *name;
  char *expr;
  char *ctype;

  if (strcmp(configFormat, "rpsl") == 0) {
    if (! initialized) {
      this->initialize();
	  initialized = true;
	}

    if (*command) {
      ctype = (char *)"route-set";
      if (command == strstr(command, ctype)) {
        extractNameAndExpr(command, ctype, name, expr);
        IRReval::configuration->prefixListName = name;
        ((RPSLConfig *)IRReval::configuration)->setRPSLExpr(expr);
        accessList(expr);
        ((RPSLConfig *)IRReval::configuration)->rsWrapUp();
        return;
      }

      ctype = (char *)"as-set";
      if (command == strstr(command, ctype)) {
        extractNameAndExpr(command, ctype, name, expr);
        IRReval::configuration->asPathName = name;
        ((RPSLConfig *)IRReval::configuration)->setRPSLExpr(expr);
        rpslASSet(expr);
        return;
      }

      cerr << "Error: unknown syntax used in \"" << command << "\" for format "
           << configFormat << "\npossible syntax:\nroute-set <name> <expression>\nas-set <name> <expression>\n\n";
      exit(1);
    } else {
      // command is empty. Nothing to do means return
      return;
    }
  }
}

void IRReval::evaluate_vendor(char *command) {
  char *name;
  char *expr;
  char *ctype;

  // save command
  char *cmdptr = command;
  // jump over leading "@IRReval"
  cmdptr += 8;
  // jump over leading blanks
  while(isspace(*cmdptr))
      cmdptr++;

  if (! initialized) {
	  if (*cmdptr == '-') {
		int argn = 1;
		char *argc;
		char *args[128];

		args[0] = (char *)"reconfigure";
		argc = strtok(cmdptr, " \t");
		while ( argc ) {
	      args[argn++] = strdup(argc);
		  if (argn < 128) {
			argc = strtok(NULL, " \t");
		  } else {
		    cerr << "Error: too many configuration arguments" << endl;
		    exit(-1);
		  }
		}
		this->reconfigure(argn, args);
		return;
	  }

	  this->initialize();
  }

  if (strcmp(configFormat, "ios") == 0 ||
		  	 strcmp(configFormat, "junos") == 0 ||
             strcmp(configFormat, "iosxr") == 0) {
	if (*cmdptr) {
      ctype = (char *)"prefix-list";
      if (cmdptr == strstr(cmdptr, ctype)) {
        extractNameAndExpr(cmdptr, ctype, name, expr);
        IRReval::configuration->prefixListName = name;
        accessList(expr);
        return;
      }

      ctype = (char *)"as-path";
      if (cmdptr == strstr(cmdptr, ctype)) {
        extractNameAndExpr(cmdptr, ctype, name, expr);
        if (strcmp(configFormat, "iosxr") == 0) {
          IRReval::configuration->asPathName = name;
        } else {
          int asPathNum = atoi(name);
          if (!asPathNum) {
            cerr << "Error: badly formed as-path number in \"" << command << "\"\n";
            exit(1);
          }
          IRReval::configuration->asPathNumber = asPathNum;
        }
        aspathAccessList(expr);
        return;
      }

      cerr << "Error: unknown syntax used in \"" << command << "\" for vendor "
           << configFormat << "\npossible syntax:\n@IRReval prefix-list <name> <expression>\n@IRReval as-path <name> <expression>\n\n";
      exit(1);
    } else {
      // command is empty. Nothing to do means return
      return;
    }
  }
}

bool IRReval::configFormatIs (char *format) {
  if (strcmp(configFormat, format) == 0)
    return true;
  else
    return false;
}


void IRReval::initialize () {
  FNameList *node;
  char filename[512];

#ifdef DTAG
  opt_asn16 = configASN16;
#endif /* DTAG */
  opt_asdot = configASDot;

  // because option '-asn32' influences the way cache files are processed and because we
  // cannot predict the order in which options are specified, the cache file processing
  // is postponed until all options are parsed.
  while (rpslFilenameList) {
	node = rpslFilenameList;

	filename[0] = '\0';
	if ((strlen(configCacheDir) > 0) && (strstr(node->name, "/") == NULL)) {
	   strcat(filename, configCacheDir);
	   strcat(filename, "/");
	}
	strcat(filename, node->name);
	IRR::initCache(filename);

	rpslFilenameList = node->next;
    delete node;
  }

  // rpki dependend initialization
#ifdef RPKI
  while (rpkiFilenameList) {
	node = rpkiFilenameList;

	filename[0] = '\0';
	if ((strlen(configCacheDir) > 0) && (strstr(node->name, "/") == NULL)) {
	   strcat(filename, configCacheDir);
	   strcat(filename, "/");
	}
	strcat(filename, node->name);
    IRR::initRPKICache(filename);

    rpkiFilenameList = node->next;
    delete node;
  }
#endif /* RPKI */

  if (irr != NULL) {
	  delete irr;
  }
  if (! (irr = IRR::newClient())) {
     cerr << "Unknown protocol!" << endl;
     exit(1);
  }
  this->setCompression();
  this->initialized = true;
}

void IRReval::configure (int argc, char **argv, char **envp) {
   ArgvInfo argTable[] = {
     // RAToolSet common arguments
     // key, type, src, dst, help
     {"-T", ARGV_FUNC, (char *) &start_tracing, (char *) NULL, 
      "Start tracing the next argument"},
     {"-D", ARGV_FUNC, (char *) &start_debugging, (char *) NULL, 
      "Start debugging the next argument"},
     {"-version", ARGV_FUNC, (char *) &version, (char *) NULL,
      "Show version"},
     {"-prompt", ARGV_STRING,  (char *) NULL, (char *) &opt_prompt,
      "Prompt"},

     IRREVAL_COMMAND_LINE_OPTIONS,

     {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL, (char *) NULL}
   };

   argv[0] = executableName;
   IRR::handleEnvironmentVariables(envp);
   if (ParseArgv(&argc, argv, argTable, ARGV_NO_LEFTOVERS) != ARGV_OK) {
      cerr << endl;
      exit(1);
   }

   // if there are remaining arguments
   // the first one is the expression
   switch (argc) {
   case 2:
      strcpy(filter, argv[1]);
      evaluate(filter);
      exit(0);
   case 1:
      break;
   default:
      cerr << "Wrong number of arguments..." << endl;
      exit(-1);
   }

   // have a prompt only if the input is coming from a tty
   if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
      opt_prompt = (char *) NULL;
}

void IRReval::reconfigure (int argc, char **argv) {
   ArgvInfo argTable[] = {
     // RAToolSet common arguments
     // key, type, src, dst, help
	 IRREVAL_COMMAND_LINE_OPTIONS,

     {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL, (char *) NULL}
   };

   argv[0] = executableName;
   if (ParseArgv(&argc, argv, argTable, ARGV_NO_LEFTOVERS) != ARGV_OK) {
      cerr << endl;
      exit(1);
   }
}

int main(int argc, char **argv, char **envp) {
   schema.initialize();
   regexp_nf::expandASSets();
   IRReval::loadDictionary();

   IRReval *irreval = new IRReval();
   irreval->configure(argc, argv, envp);

   char *fptr;

   while (1) {
     filter[0] = '\0';
     if (opt_prompt)
	 cout << opt_prompt;

     if (cin.eof())
         break;
     // get one line of input
     cin.getline(filter, SIZE);
     fptr = filter;
     // clear leading blanks
     while(*fptr == ' ' || *fptr == '\t')
         fptr++;

     if (strncmp(fptr, "@IRReval", 8) == 0) {
       // line starting with "@IRReval" means line by line evaluation
	   irreval->evaluate_vendor(fptr);
     } else if (irreval->configFormatIs((char *)"rpsl")) {
       // rpsl config format
       irreval->evaluate_rpsl(fptr);
     } else {
       // other config formats without starting with "@IRReval" mean just print line as found
       cout << filter << endl;
     }
   }

   cout.flush();
   cerr.flush();
}

void IRReval::loadDictionary() {
   dctn_rp_pref      = schema.searchRPAttr("pref");
   dctn_rp_nhop      = schema.searchRPAttr("next-hop");
   dctn_rp_dpa       = schema.searchRPAttr("dpa");
   dctn_rp_med       = schema.searchRPAttr("med");
   dctn_rp_community = schema.searchRPAttr("community");
   dctn_rp_aspath    = schema.searchRPAttr("aspath");
   
   if (dctn_rp_pref)
      dctn_rp_pref_set         = dctn_rp_pref->searchMethod("=");
   if (dctn_rp_nhop)
      dctn_rp_nhop_set         = dctn_rp_nhop->searchMethod("=");
   if (dctn_rp_dpa)
      dctn_rp_dpa_set          = dctn_rp_dpa->searchMethod("=");
   if (dctn_rp_med)
      dctn_rp_med_set          = dctn_rp_med->searchMethod("=");
   if (dctn_rp_community) {
      dctn_rp_community_setop    = dctn_rp_community->searchMethod("=");
      dctn_rp_community_appendop = dctn_rp_community->searchMethod(".=");
      dctn_rp_community_append   = dctn_rp_community->searchMethod("append");
      dctn_rp_community_delete   = dctn_rp_community->searchMethod("delete");
   }
   if (dctn_rp_aspath)
      dctn_rp_aspath_prepend   = dctn_rp_aspath->searchMethod("prepend");
}

// reimplemented to handle mp-filters
void IRReval::accessList(char *filter) {
   Buffer peval(strlen(filter)+16);

   if (strstr(filter, "afi ")) {
     peval.append("mp-");
   }
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {
     NormalExpression *ne;
     if (strcmp(o->type->name,"mp-peval") == 0) {
       AttrIterator<AttrMPPeval> itr(o, "mp-peval");
       ne = NormalExpression::evaluate(itr()->filter, ~0);
     }
     else {
       AttrIterator<AttrFilter> itr(o, "peval");
       ne = NormalExpression::evaluate(itr()->filter, ~0);
     }

     if (ne) {
       if (ne->is_any() != NEITHER) {
           cerr << "Warning: filter \"" << filter << "\" matches ANY/NOT ANY" << endl;
           char *ptr = strstr(filter, "afi ");
           if (ptr != NULL) {
             ptr += 4;
             while (isspace(*ptr)) {
               ptr++;
             }
             if (strncmp(ptr, "ipv4", 4) == 0) {
               // ipv4 prefix list
               SetOfPrefix *nets = new SetOfPrefix;
               if (ne->is_any() == ANY)
                 nets->not_ = true;
               IRReval::configuration->printAccessList(*nets);
             } else if (strncmp(ptr, "ipv6", 4) == 0) {
               // ipv6 prefix list
               SetOfIPv6Prefix *nets = new SetOfIPv6Prefix;
               if (ne->is_any() == ANY)
                 nets->not_ = true;
               IRReval::configuration->printAccessList(*nets);
             } else if (strncmp(ptr, "any", 3) == 0) {   // && strcmp(configFormat, "rpsl") == 0) {
               // ipv4 and ipv6 prefix list
               SetOfPrefix *nets_ipv4 = new SetOfPrefix;
               SetOfIPv6Prefix *nets_ipv6 = new SetOfIPv6Prefix;
               if (ne->is_any() == ANY) {
                 nets_ipv4->not_ = true;
                 nets_ipv6->not_ = true;
               }
               IRReval::configuration->printAccessList(*nets_ipv4);
               IRReval::configuration->printAccessList(*nets_ipv6);
             } else {
               cerr << "Error: unknown afi value! Can't print configuration for filter \"" << filter << "\"\n";
               exit(1);
             }
           } else {
             // ipv4 prefix list
             SetOfPrefix *nets = new SetOfPrefix;
             if (ne->is_any() == ANY)
                nets->not_ = true;
             IRReval::configuration->printAccessList(*nets);
	   }
	   // }
       } else {
         for (NormalTerm *nt = ne->first(); nt; nt = ne->next())
           if (nt->prfx_set.is_universal() && nt->ipv6_prfx_set.is_universal()) {
             cerr << "Error: badly formed prefix filter \"" << filter << "\"\n";
             exit(1);
           }
         for (NormalTerm *nt = ne->first(); nt != NULL; nt = ne->next()) {
           if (!nt->prfx_set.universal())
              IRReval::configuration->printAccessList(nt->prfx_set);
           if (!nt->ipv6_prfx_set.universal())
              IRReval::configuration->printAccessList(nt->ipv6_prfx_set);
         }
       }
       delete ne;
     }
   } else {
     cerr << "Error: creation of filter object for \"" << filter << "\" revealed an error!\n";
     exit(1);
   }
   delete o;
}

void IRReval::aspathAccessList(char *filter) {
   Buffer peval(strlen(filter)+16);
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {
      AttrIterator<AttrFilter> itr(o, "peval");
      
      NormalExpression *ne = 
	 NormalExpression::evaluate(itr()->filter, ~0);

      if (ne && ne->is_any() != NEITHER)
        cerr << "Warning: filter \"" << filter << "\" matches ANY/NOT ANY" << endl;

      if (ne && ne->singleton_flag == NormalTerm::AS_PATH)
	 IRReval::configuration->printAspathAccessList(ne->first()->as_path);
      else {
	cerr << "Error: badly formed as-path filter \"" << filter << "\"\n";
        exit(1);
      }

      delete ne;
   }
   
   delete o;
}

void IRReval::rpslASSet(char *filter) {
   Buffer peval(strlen(filter)+16);
   peval.append("peval: ");
   peval.append(filter);
   peval.append("\n\n");

   Object *o = new Object(peval);
   if (! o->has_error) {
      AttrIterator<AttrFilter> itr(o, "peval");
      
      NormalExpression *ne = NormalExpression::evaluate(itr()->filter, ~0, ~0 & ~0x000004);

      if (ne && ne->is_any() != NEITHER)
        cerr << "Warning: filter \"" << filter << "\" matches ANY/NOT ANY" << endl;

      if (ne && ne->singleton_flag == NormalTerm::SYMBOLS)
	 ((RPSLConfig *)IRReval::configuration)->printASSet(ne->first()->symbols);
      else {
	cerr << "Error: badly formed as-set filter \"" << filter << "\"\n";
        exit(1);
      }

      delete ne;
   }
   
   delete o;
}
