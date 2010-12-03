# Charmap Panel plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	$(GTRANSLATOR_CFLAGS) 				\
	$(GUCHARMAP_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED)

plugin_LTLIBRARIES = libcharmap.la

libcharmap_la_SOURCES = \
	gtr-charmap-plugin.h				\
	gtr-charmap-plugin.c				\
	gtr-charmap-panel.h				\
	gtr-charmap-panel.c

libcharmap_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)					\
	$(GUCHARMAP_LIBS)

pixmaps_dir = $(datadir)/pixmaps/gtranslator
pixmaps__DATA = \
        gucharmap.ico

# Plugin Info

plugin_in_files = gtr-charmap.plugin.desktop.in

%.plugin: %.plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.plugin.desktop.in=.plugin)

EXTRA_DIST = $(pixmaps__DATA) $(plugin_in_files)

CLEANFILES = $(plugin_DATA)
DISTCLEANFILES = $(plugin_DATA)

-include $(top_srcdir)/git.mk
