##
## The backends Makefile.am of gtranslator.
##

backendsdir = $(libdir)/gtranslator/backends

backends_DATA = po.xml text.xml
backends_LTLIBRARIES = po.la text.la

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	-DG_LOG_DOMAIN=\""gtranslator"\" \
	$(GLIB_CFLAGS)

po_la_SOURCES = po.c
po_la_LDFLAGS = -avoid-version -module

text_la_SOURCES = text.c
text_la_LDFLAGS = -avoid-version -module

EXTRA_DIST = $(backends_DATA)
