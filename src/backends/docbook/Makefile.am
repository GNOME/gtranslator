##
## The docbook sgml backend for gtranslator.
##

backendsdir = $(libdir)/gtranslator/backends
backendslibdir = $(backendsdir)/docbook

backends_DATA = docbook.xml
backendslib_LTLIBRARIES = docbook.la

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	-DG_LOG_DOMAIN=\""gtranslator"\" \
        -DPREFIX=\""$(prefix)"\"                                \
        -DSYSCONFDIR=\""$(sysconfdir)"\"                        \
        -DDATADIR=\""$(datadir)"\"                              \
        -DLIBDIR=\""$(libdir)"\"                                \
        $(GTRANSLATOR_CFLAGS)

docbook_la_SOURCES = docbook.c
docbook_la_LDFLAGS = -avoid-version -module

EXTRA_DIST = $(backends_DATA)
