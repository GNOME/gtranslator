##
## The po file backend for gtranslator.
##

backendsdir = $(libdir)/gtranslator/backends
backendslibdir = $(backendsdir)/po

backends_DATA = po.xml
backendslib_LTLIBRARIES = po.la

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	-DG_LOG_DOMAIN=\""gtranslator"\" \
	$(GLIB_CFLAGS)

po_la_SOURCES = po.c
po_la_LDFLAGS = -avoid-version -module

EXTRA_DIST = $(backends_DATA)
