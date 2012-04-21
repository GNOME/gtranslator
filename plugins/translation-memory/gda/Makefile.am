INCLUDES = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/plugins/translation-memory \
	-I$(headerdir) \
	$(GTRANSLATOR_CFLAGS) \
	$(WARN_CFLAGS) \
	$(DISABLE_DEPRECATED)

noinst_LTLIBRARIES = \
	libgda.la

libgda_la_SOURCES = \
	gda-utils.c \
	gda-utils-h \
	gtr-gda.c \
	gtr-gda.h

libgda_la_CFLAGS = \
	$(GTRANSLATOR_CFLAGS) \
	$(WARN_CFLAGS) \
	$(DISABLE_DEPRECATED) \
	$(AM_CFLAGS)

EXTRA_DIST = $(pixmaps__DATA)


-include $(top_srcdir)/git.mk
