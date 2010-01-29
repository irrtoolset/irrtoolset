# $Id: $
#
# Makefile.am for IRRToolSet
#
SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = RELEASE-NOTES \
	src/rpsl/object_log.l src/rpsl/rpsl.l src/rpsl/rpsl.y \
	src/rtconfig/command.l src/rtconfig/command.y \
	contrib tests

dist-hook:
	rm -rf `find $(distdir)/contrib -name .svn`
	rm -rf `find $(distdir)/tests -name .svn`

CLEANFILES = *~ *.bak
