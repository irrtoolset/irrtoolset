bin_PROGRAMS = rtconfig

rtconfig_SOURCES = rtconfig.cc command.l.cc command.y.cc f_cisco.cc f_junos.cc \
                   rtconfig.hh f_cisco.hh f_junos.hh command.y.hh

man1_MANS = rtconfig.1
dist_man_MANS = $(man1_MANS)

MAINTAINERCLEANFILES = command.y.cc command.y.hh command.l.cc
CLEANFILES = *~ *.bak

LDADD = ../libirrtoolset.la @EXTRA_LIBS@

command.l.cc: command.y.cc
	$(LEX) command.l

command.y.cc:
	$(YACC) -d command.y

