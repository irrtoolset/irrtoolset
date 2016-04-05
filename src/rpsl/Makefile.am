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

noinst_LTLIBRARIES = librpsl.la
librpsl_la_SOURCES = \
	Buffer.cc afi.cc object.cc prefix.cc rangelist.cc regexp.cc \
	rpsl_attr.cc rpsl_filter.cc rpsl_item.cc rpsl_policy.cc rptype.cc \
	schema.cc schema_rpsl.cc symbols.cc Allocator.hh Buffer.hh List.hh \
	afi.hh object.hh object_log.hh prefix.hh rangelist.hh regexp.hh \
	rpsl.hh rpsl_asnum.hh rpsl_attr.hh rpsl_filter.hh rpsl_item.hh \
	rpsl_policy.hh rptype.hh schema.hh symbols.hh

nodist_librpsl_la_SOURCES = object_log.l.cc rpsl.l.cc rpsl.y.cc rpsl.y.hh

dist_man1_MANS = irrtoolset-errors.1

MAINTAINERCLEANFILES = $(nodist_librpsl_la_SOURCES)
CLEANFILES = *~ *.bak *.y.cc *.l.cc

schema_rpsl.cc: rpsl.y.cc

# FIXME: dependency on lex / yacc sources
object_log.l.cc:
	$(LEX) object_log.l

rpsl.l.cc: rpsl.y.cc
	$(LEX) rpsl.l
        
rpsl.y.cc:
	$(YACC) -d rpsl.y
