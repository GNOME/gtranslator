##
## The text backend for gtranslator.
##

backendsdir = $(libdir)/gtranslator/backends
backendslibdir = $(backendsdir)/text

backends_DATA = text.xml
backendslib_LTLIBRARIES = text.la

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	-DG_LOG_DOMAIN=\""gtranslator"\" \
	$(GLIB_CFLAGS)		\
	$(GNOME_INCLUDEDIR)	\
	$(GAL_CFLAGS)

text_la_SOURCES = text.c
text_la_LDFLAGS = -avoid-version -module

EXTRA_DIST = $(backends_DATA)
