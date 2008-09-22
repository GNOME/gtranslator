# Dictionary Panel plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	-I$(top_srcdir)/src/plugin-system		\
	-I$(top_srcdir)/src/toolbareditor		\
	$(GTRANSLATOR_CFLAGS) 				\
	$(DICTIONARY_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED_CFLAGS)			\
	-DGTR_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

plugin_LTLIBRARIES = libdict.la

libdict_la_SOURCES = \
	dict-panel.h			\
	dict-panel.c			\
	gdict-sidebar.c			\
	gdict-sidebar.h			\
	dictionary-plugin.h		\
	dictionary-plugin.c
	

libdict_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)					\
	$(DICTIONARY_LIBS)

pixmaps_dir = $(datadir)/pixmaps/gtranslator
pixmaps__DATA = \
        gnome-dictionary.png


# Plugin Info

plugin_in_files = dict.gtranslator-plugin.desktop.in

%.gtranslator.plugin: %.gtranslator-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.gtranslator-plugin.desktop.in=.gtranslator.plugin)

EXTRA_DIST = $(pixmaps__DATA) $(plugin_in_files)

CLEANFILES = $(plugin_DATA)
DISTCLEANFILES = $(plugin_DATA)
