##
## The OpenTag translation memory backend for gtranslator.
##

tm_backends_xmldir = $(libdir)/gtranslator/backends/translation-memories
tm_backendsdir = $(tm_backends_xmldir)/opentag

tm_backends_xml_DATA = opentag.xml
tm_backends_LTLIBRARIES = opentag.la

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	-I../ \
	-DG_LOG_DOMAIN=\""gtranslator"\" \
	$(GLIB_CFLAGS) \
	$(GAL_CFLAGS)

opentag_la_SOURCES = opentag.c
opentag_la_LDFLAGS = -avoid-version -module
opentag_la_DEPENDENCIES = ../translation-memories.h

EXTRA_DIST = $(tm_backends_xml_DATA)
