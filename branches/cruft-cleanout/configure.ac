# $Id$
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.62])
AC_INIT([irrtoolset], [4.9.0-alpha1], [irrtoolset@lists.isc.org])
AC_CONFIG_SRCDIR([RELEASE-NOTES])
AC_CONFIG_HEADERS([src/config.h])

# Fix up Automake and libtool
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
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
AC_ARG_WITH(readline, [  --enable-readline       support fancy command line editing (default: yes)],
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
AC_SUBST(READLINE_LIBS)

dnl enable debugging
AC_ARG_WITH(debug, [  --enable-debug          turn on debugging],
if eval "test x$enable_debug = xyes"; then
  OPTFLAG="-DDEBUG -fno-inline"
else
  OPTFLAG=""
fi
,
OPTFLAG=""
)

case "${host_os}" in
      solaris* )
              EXTRA_LIBS="-lsocket -lnsl"
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

