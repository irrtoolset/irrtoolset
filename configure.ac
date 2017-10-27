# $Id$
#
#  Copyright (c) 2010 Nick Hilliard
#  All rights reserved.
#
#    Permission is hereby granted, free of charge, to any person obtaining a copy
#    of this software and associated documentation files (the "Software"), to deal
#    in the Software without restriction, including without limitation the rights
#    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the Software is
#    furnished to do so, subject to the following conditions:
#
#    The above copyright notice and this permission notice shall be included in
#    all copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#    THE SOFTWARE.
#
#  Author(s): Nick Hilliard <nick@foobar.org>
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([irrtoolset], [5.1.0], [irrtoolset@lists.isc.org])
AC_CONFIG_SRCDIR([src/rpsl/rpsl.l])
AC_CONFIG_HEADERS([src/config.h])

# Fix up Automake and libtool
AM_INIT_AUTOMAKE([foreign tar-ustar])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Some versions of GCC 6.x broke -ftree-vrp, so we need to work around this
if [[ "X`(${CC} --version 2> /dev/null | grep ^gcc | sed -n 's/.*\( 6\.[0-9]*\.\).*/\1/p')`" != "X" ]]; then
  echo detected gcc6: disabling the '"'-ftree-vrp'"' option
  CFLAGS="${CFLAGS} -fno-tree-vrp"
  CXXFLAGS="${CFLAGS} -fno-tree-vrp"

  # re-run compiler checks to configure CFLAGS correctly
  AC_PROG_CXX
  AC_PROG_CC

fi

AC_PROG_INSTALL
AC_PROG_LN_S
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h libintl.h limits.h malloc.h memory.h netdb.h netinet/in.h regex.h stddef.h stdlib.h string.h strings.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([timegm bzero gethostbyaddr gethostbyname gethostname getpagesize gettimeofday inet_ntoa isascii memchr memmove memset regcomp select socket strcasecmp strchr strdup strerror strncasecmp strrchr strstr strtol])

# Libtoolize wants this
AC_CONFIG_MACRO_DIR([m4])

# check for other stuff

dnl Check if accept uses socklen_t is defined
AC_MSG_CHECKING(for whether accept uses type (socklen_t *) as parameter)
AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/socket.h>
], [
	int sock;
    	struct sockaddr socknew;	
	int addrlen = 5;
	int newsock = accept (sock, &socknew, (socklen_t *) &addrlen);
], [AC_DEFINE([ACCEPT_USES_SOCKLEN_T], [], [accept() uses type (socklen_t *) as parameter]) 
AC_MSG_RESULT(yes)], AC_MSG_RESULT(no)) 	

dnl Do the readline dance
READLINE_LIBS=""
AC_ARG_ENABLE([readline], [  --enable-readline       support fancy command line editing (default: yes)],
if eval "test x$enable_readline = xyes"; then
	AC_CHECK_LIB(termcap, main)
	AC_CHECK_LIB(readline, main)
	AC_CHECK_LIB(history, main)
	READLINE_LIBS="$LIBS"
fi
,
AC_CHECK_LIB(termcap, main)
AC_CHECK_LIB(readline, main)
AC_CHECK_LIB(history, main)
READLINE_LIBS="$LIBS"
)

dnl enable debugging
AC_MSG_CHECKING(whether to enable development debugging)
AC_ARG_ENABLE([debug], [  --enable-debug          turn on debugging],
if eval "test x$enable_debug = xyes"; then
	AC_DEFINE([ENABLE_DEBUG], [1], [enable debugging stuff for development purposes])
	CFLAGS=
	CXXLAGS=
	OPTFLAGS="-fno-inline -g -O"
else
	OPTFLAGS=""
fi
,
OPTFLAGS=""
)
AC_MSG_RESULT(${enable_debug})

case "${host_os}" in
	# Solaris needs extra libs for its socket api
	solaris* )
		SOLARIS_LIBS="-lsocket -lnsl"
	;;
	# Mac OS/X has native support for readline but doesn't contain
	# libhistory.  We pretend it does.
	darwin* )
		READLINE_LIBS="-lreadline"
		AC_DEFINE([HAVE_LIBHISTORY], [1])
	;;
esac

dnl check for yylineno -- HAVE_YYLINENO
dnl
ac_have_yylineno=no
AC_MSG_CHECKING([for yylineno declaration])
# some systems have yylineno, others don't...
  echo '
%%
a { ECHO; }
b { REJECT; }
c { yymore (); }
d { yyless (1); }
e { yyless (input () != 0); }
f { unput (yytext[0]); }
. { BEGIN INITIAL; }
%%
int main (void)
{
  return ! yylex () + ! yywrap ();
}
' | ${LEX} -t > conftest.out
egrep yylineno conftest.out >/dev/null 2>&1
if test X"$?" = X0 ; then
  AC_DEFINE([HAVE_YYLINENO], [1], [check for yylineno declaration in lex output])
  ac_have_yylineno=yes
fi
rm -f conftest.out
AC_MSG_RESULT(${ac_have_yylineno})

EXTRA_LIBS="${SOLARIS_LIBS} ${READLINE_LIBS}"

AC_SUBST(EXTRA_LIBS)

CXXFLAGS="${CFLAGS} ${OPTFLAGS}"
CFLAGS="${CFLAGS} ${OPTFLAGS}"

dnl spit out configuration files 
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/rtconfig/Makefile
                 src/dataset/Makefile
                 src/irr/Makefile
                 src/irrutil/Makefile
                 src/normalform/Makefile
                 src/peval/Makefile
                 src/re2dfa/Makefile
                 src/rpsl/Makefile
                 src/rpslcheck/Makefile
])
AC_OUTPUT

