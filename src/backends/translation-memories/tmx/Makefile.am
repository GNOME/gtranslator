##
## The TMX translation memory backend for gtranslator.
##

tm_backends_xmldir = $(libdir)/gtranslator/backends/translation-memories
tm_backendsdir = $(tm_backends_xmldir)/tmx

tm_backends_xml_DATA = tmx.xml
tm_backends_LTLIBRARIES = tmx.la

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	-I../ \
	-DG_LOG_DOMAIN=\""gtranslator"\" \
	$(GLIB_CFLAGS) \
	$(GAL_CFLAGS)

tmx_la_SOURCES = tmx.c
tmx_la_LDFLAGS = -avoid-version -module
tmx_la_DEPENDENCIES = ../translation-memories.h

EXTRA_DIST = $(tm_backends_xml_DATA)
