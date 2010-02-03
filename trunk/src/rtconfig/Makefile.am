bin_PROGRAMS = rtconfig

rtconfig_SOURCES = rtconfig.cc f_cisco.cc f_junos.cc \
                   rtconfig.hh f_cisco.hh f_junos.hh

nodist_rtconfig_SOURCES = command.l.cc command.y.cc command.y.hh

dist_man1_MANS = rtconfig.1

MAINTAINERCLEANFILES = $(nodist_rtconfig_SOURCES)
CLEANFILES = *~ *.bak

LDADD = ../libirrtoolset.la @EXTRA_LIBS@

command.l.cc: command.y.cc
	$(LEX) command.l

command.y.cc:
	$(YACC) -d command.y

