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
 * Argv.c --
 *
 *	Targvle contains a procedure that handles table-based
 *	argv-argc parsing.
 *
 * Copyright (c) 1990-1994 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
This software is copyrighted by the Regents of the University of
California, Sun Microsystems, Inc., and other parties.  The following
terms apply to all files associated with the software unless explicitly
disclaimed in individual files.

The authors hereby grant permission to use, copy, modify, distribute,
and license this software and its documentation for any purpose, provided
that existing copyright notices are retained in all copies and that this
notice is included verbatim in any distributions. No written agreement,
license, or royalty fee is required for any of the authorized uses.
Modifications to this software may be copyrighted by their authors
and need not follow the licensing terms described here, provided that
the new terms are clearly indicated on the first page of each file where
they apply.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
MODIFICATIONS.

RESTRICTED RIGHTS: Use, duplication or disclosure by the government
is subject to the restrictions as set forth in subparagraph (c) (1) (ii)
of the Rights in Technical Data and Computer Software Clause as DFARS
252.227-7013 and FAR 52.227-19.
 *
 */

#include "config.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include "Argv.hh"
#include "version.hh"	/* For version control */

using namespace std;

/*
 * Default table of argument descriptors.  These are normally available
 * in every application.
 */

static ArgvInfo defaultTable[] = {
    {"-help",	ARGV_HELP,	(char *) NULL,	(char *) NULL,
	"Print summary of command-line options and abort"},
    {NULL,	ARGV_END,	(char *) NULL,	(char *) NULL,
	(char *) NULL}
};

/*
 * Forward declarations for procedures defined in this file:
 */

static void PrintUsage(ArgvInfo *argTable, int flags);

/*
 *----------------------------------------------------------------------
 *
 * ParseArgv --
 *
 *	Process an argv array according to a table of expected
 *	command-line options.  See the manual page for more details.
 *
 * Results:
 *	Under normal conditions, both *argcPtr and *argv are modified
 *	to return the arguments that couldn't be processed here (they
 *	didn't match the option table, or followed an ARGV_REST
 *	argument).
 *
 * Side effects:
 *	Variables may be modified, or procedures may be called.  It
 *  	all depends on the arguments
 *	and their entries in argTable.  See the user documentation
 *	for details.
 *
 *----------------------------------------------------------------------
 */

int ParseArgv(int *argcPtr, char **argv, ArgvInfo *argTable, int flags)
    /* argcPtr: Number of arguments in argv.  Modified
     * to hold # args left in argv at end. */
    /* argv: Array of arguments.  Modified to hold
     * those that couldn't be processed here. */
    /* argTable: Array of option descriptions */
    /* flags: Or'ed combination of various flag bits,
     * such as ARGV_NO_DEFAULTS. */
{
    register ArgvInfo *infoPtr;
				/* Pointer to the current entry in the
				 * table of argument descriptions. */
    ArgvInfo *matchPtr;	/* Descriptor that matches current argument. */
    char *curArg;		/* Current argument */
    register char c;		/* Second character of current arg (used for
				 * quick check for matching;  use 2nd char.
				 * because first char. will almost always
				 * be '-'). */
    int srcIndex;		/* Location from which to read next argument
				 * from argv. */
    int dstIndex;		/* Index into argv to which next unused
				 * argument should be copied (never greater
				 * than srcIndex). */
    int argc;			/* # arguments in argv still to process. */
    int length;		        /* Number of characters in current argument. */
    int i;

    /* Try to fill the name of current program into ProjectGoal[64] -- wlee@ISI.EDU */
    int len = strlen(argv[0]);
    char *pGoal = argv[0] + len;
    while (*pGoal != '/' && *pGoal != '\\' && --len >= 0) pGoal--;
    if (*pGoal == '/' || *pGoal == '\\')
       pGoal++;
    strncpy(ProjectGoal, pGoal, 63);

    if (flags & ARGV_DONT_SKIP_FIRST_ARG) {
	srcIndex = dstIndex = 0;
	argc = *argcPtr;
    } else {
	srcIndex = dstIndex = 1;
	argc = *argcPtr-1;
    }

    while (argc > 0) {
	curArg = argv[srcIndex];
	srcIndex++;
	argc--;
	length = strlen(curArg);
	if (length > 0) {
	    c = curArg[1];
	} else {
	    c = 0;
	}

	/*
	 * Loop throught the argument descriptors searching for one with
	 * the matching key string.  If found, leave a pointer to it in
	 * matchPtr.
	 */

	matchPtr = NULL;
	for (i = 0; i < 2; i++) {
	    if (i == 0) {
		infoPtr = argTable;
	    } else {
		infoPtr = defaultTable;
	    }
	    for (; (infoPtr != NULL) && (infoPtr->type != ARGV_END);
		    infoPtr++) {
		 if (infoPtr->key == NULL) {
		     continue;
		 }
		 if ((infoPtr->key[1] != c)
			 || (strncmp(infoPtr->key, curArg, length) != 0)) {
		     continue;
		 }
		 if (infoPtr->key[length] == 0) {
		     matchPtr = infoPtr;
		     goto gotMatch;
		 }
		 if (flags & ARGV_NO_ABBREV) {
		     continue;
		 }
		 if (matchPtr != NULL) {
		    cerr << "ambiguous option \"" << curArg << "\"";
		     return ARGV_ERROR;
		 }
		 matchPtr = infoPtr;
	    }
	}
	if (matchPtr == NULL) {

	    /*
	     * Unrecognized argument.  Just copy it down, unless the caller
	     * prefers an error to be registered.
	     */

	    if (flags & ARGV_NO_LEFTOVERS) {
	       cerr << "unrecognized argument \"" << curArg << "\"";
		return ARGV_ERROR;
	    }
	    argv[dstIndex] = curArg;
	    dstIndex++;
	    continue;
	}

	/*
	 * Take the appropriate action based on the option type
	 */

	gotMatch:
	infoPtr = matchPtr;
	switch (infoPtr->type) {
	    case ARGV_BOOL:
		*((bool *) infoPtr->dst) = ! *((bool *) infoPtr->dst);
		break;
	    case ARGV_CONSTANT:
		*((char **) infoPtr->dst) = infoPtr->src;
		break;
	    case ARGV_INT:
		if (argc == 0) {
		    goto missingArg;
		} else {
		    char *endPtr;

		    *((int *) infoPtr->dst) =
			    strtol(argv[srcIndex], &endPtr, 0);
		    if ((endPtr == argv[srcIndex]) || (*endPtr != 0)) {
		       cerr << "expected integer argument for \"" 
			    << infoPtr->key << "\" but got \""
			    << argv[srcIndex] << "\"";
			return ARGV_ERROR;
		    }
		    srcIndex++;
		    argc--;
		}
		break;
	    case ARGV_STRING:
		if (argc == 0) {
		    goto missingArg;
		} else {
		    *((char **)infoPtr->dst) = argv[srcIndex];
		    srcIndex++;
		    argc--;
		}
		break;
	    case ARGV_REST:
		*((int *) infoPtr->dst) = dstIndex;
		goto argsDone;
	    case ARGV_FLOAT:
		if (argc == 0) {
		    goto missingArg;
		} else {
		    char *endPtr;

		    *((double *) infoPtr->dst) =
			    strtod(argv[srcIndex], &endPtr);
		    if ((endPtr == argv[srcIndex]) || (*endPtr != 0)) {
		       cerr << "expected floating-point argument for \""
			    << infoPtr->key << "\" but got \""
			    << argv[srcIndex] <<  "\"";
			return ARGV_ERROR;
		    }
		    srcIndex++;
		    argc--;
		}
		break;
	    case ARGV_FUNC: {
		int (*handlerProc)(...);

		handlerProc = (int (*)(...))infoPtr->src;
		
		if ((*handlerProc)(infoPtr->dst, infoPtr->key,
			argv[srcIndex])) {
		    srcIndex += 1;
		    argc -= 1;
		}
		break;
	    }
	    case ARGV_GENFUNC: {
		int	    (*handlerProc)(...);

		handlerProc = (int (*)(...))infoPtr->src;

		argc = (*handlerProc)(infoPtr->dst, infoPtr->key,
			argc, argv+srcIndex);
		if (argc < 0) {
		    return ARGV_ERROR;
		}
		break;
	    }
	    case ARGV_HELP:
		PrintUsage (argTable, flags);
		return ARGV_ERROR;
	    default:
	       cerr << "bad argument type %d in ArgvInfo" << infoPtr->type;
		return ARGV_ERROR;
	}
    }

    /*
     * If we broke out of the loop because of an OPT_REST argument,
     * copy the remaining arguments down.
     */

    argsDone:
    while (argc) {
	argv[dstIndex] = argv[srcIndex];
	srcIndex++;
	dstIndex++;
	argc--;
    }
    argv[dstIndex] = (char *) NULL;
    *argcPtr = dstIndex;
    return ARGV_OK;

    missingArg:
    cerr << "\"" << curArg << "\" option requires an additional argument";
    return ARGV_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * PrintUsage --
 *
 *	Print (in cerr) a help string describing command-line options.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
PrintUsage(ArgvInfo *argTable, int flags)
   /* argTable: Array of command-specific argument
    * descriptions. */
   /* flags: If the ARGV_NO_DEFAULTS bit is set
    * in this word, then don't generate
    * information for default options. */
{
    register ArgvInfo *infoPtr;
    int width, i, numSpaces;
    static char spaces[] = "                    ";
    unsigned spacesLen;
    
    spacesLen = sizeof(spaces) - 1;

    /*
     * First, compute the width of the widest option key, so that we
     * can make everything line up.
     */

    width = 4;
    for (i = 0; i < 2; i++) {
	for (infoPtr = i ? defaultTable : argTable;
		infoPtr->type != ARGV_END; infoPtr++) {
	    int length;
	    if (infoPtr->key == NULL) {
		continue;
	    }
	    length = strlen(infoPtr->key);
	    if (length > width) {
		width = length;
	    }
	}
    }

    // Added by wlee@isi.edu
    cerr << endl;

    cerr << "Command-specific options:";
    for (i = 0; ; i++) {
	for (infoPtr = i ? defaultTable : argTable;
		infoPtr->type != ARGV_END; infoPtr++) {
	    if ((infoPtr->type == ARGV_HELP) && (infoPtr->key == NULL)) {
	       cerr << "\n" << infoPtr->help;
		continue;
	    }
	    // Modified by wlee@isi.edu to get rid of ':' 
	    cerr << "\n " << infoPtr->key << " ";
//	    cerr << "\n " << infoPtr->key << ":";
	    numSpaces = width + 1 - strlen(infoPtr->key);
	    while (numSpaces > 0) {
		if (numSpaces >= spacesLen) {
		   cerr << spaces;
		} else {
		   cerr << spaces+spacesLen-numSpaces;
		}
		numSpaces -= spacesLen;
	    }
	    cerr << infoPtr->help;
	    switch (infoPtr->type) {
		case ARGV_INT: {
		    cerr << "\n\t\tDefault value: " << *((int *) infoPtr->dst);
		    break;
		}
		case ARGV_FLOAT: {
		    cerr << "\n\t\tDefault value: " << *((double *) infoPtr->dst);
		    break;
		}
		case ARGV_STRING: {
		    char *string;

		    string = *((char **) infoPtr->dst);
		    if (string != NULL) {
		       cerr << "\n\t\tDefault value: \"" << string << "\"";
		    }
		    break;
		}
		default: {
		    break;
		}
	    }
	}

	if ((flags & ARGV_NO_DEFAULTS) || (i > 0)) {
	    break;
	}
	cerr << "\nGeneric options for all commands:";
    }
    // Added by wlee@isi.edu
    cerr << endl;
}



