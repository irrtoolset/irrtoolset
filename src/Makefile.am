# $Id: $
#
# Makefile.am for IRRToolSet
#

SUBDIRS= 	dataset irr normalform re2dfa rpsl irrutil . \
		peval rpslcheck rtconfig

INCLUDES=	-I@top_srcdir@/

noinst_LTLIBRARIES = libirrtoolset.la
libirrtoolset_la_SOURCES =
libirrtoolset_la_LIBADD= \
	normalform/libnormalform.la \
	irr/libirr.la \
	re2dfa/libre2dfa.la \
	rpsl/librpsl.la \
	dataset/libdataset.la \
	irrutil/libirrutil.la
