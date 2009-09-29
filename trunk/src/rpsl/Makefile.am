noinst_LTLIBRARIES = librpsl.la
librpsl_la_SOURCES = \
	Buffer.cc afi.cc object.cc prefix.cc rangelist.cc regexp.cc \
	rpsl_attr.cc rpsl_filter.cc rpsl_item.cc rpsl_policy.cc rptype.cc \
	schema.cc schema_rpsl.cc symbols.cc object_log.l.cc rpsl.l.cc \
	rpsl.y.cc \
	Allocator.hh Buffer.hh List.hh afi.hh object.hh object_log.hh \
	prefix.hh rangelist.hh regexp.hh rpsl.hh rpsl_asnum.hh rpsl_attr.hh \
	rpsl_filter.hh rpsl_item.hh rpsl_policy.hh rptype.hh rpsl.y.hh \
	schema.hh symbols.hh

man1_MANS = irrtoolset-errors.1
dist_man_MANS = $(man1_MANS)

MAINTAINERCLEANFILES = object_log.l.cc rpsl.l.cc rpsl.y.cc rpsl.y.hh
CLEANFILES = *~ *.bak

schema_rpsl.cc: rpsl.y.cc

# FIXME: dependency on lex / yacc sources
object_log.l.cc:
	$(LEX) object_log.l

rpsl.l.cc: rpsl.y.cc
	$(LEX) rpsl.l
        
rpsl.y.cc:
	$(YACC) -d rpsl.y
