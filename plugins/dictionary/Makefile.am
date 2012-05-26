# Dictionary Panel plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir)                 \
	-I$(top_srcdir)/src             \
	$(GTRANSLATOR_CFLAGS)           \
	$(DICTIONARY_CFLAGS)            \
	$(WARN_CFLAGS)                  \
	$(DISABLE_DEPRECATED)

plugin_LTLIBRARIES = libdict.la

libdict_la_SOURCES = \
	gtr-dict-panel.h            \
	gtr-dict-panel.c            \
	gtr-gdict-sidebar.c         \
	gtr-gdict-sidebar.h         \
	gtr-dictionary-plugin.h     \
	gtr-dictionary-plugin.c

libdict_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)     \
	$(GTRANSLATOR_LIBS)         \
	$(DICTIONARY_LIBS)

pixmaps_dir = $(datadir)/gtranslator/pixmaps
pixmaps__DATA = \
	gnome-dictionary.png

# Plugin Info

plugin_in_files = gtr-dict.plugin.desktop.in

%.plugin: %.plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.plugin.desktop.in=.plugin)

gsettings_SCHEMAS = org.gnome.gtranslator.plugins.dictionary.gschema.xml

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

EXTRA_DIST = \
	$(pixmaps__DATA)    \
	$(plugin_in_files)  \
	org.gnome.gtranslator.plugins.dictionary.gschema.xml.in.in

CLEANFILES = $(plugin_DATA) $(gsettings_SCHEMAS)
DISTCLEANFILES = $(plugin_DATA) $(gsettings_SCHEMAS)

MAINTAINERCLEANFILES = $(gsettings_SCHEMAS:.xml=.valid)

-include $(top_srcdir)/git.mk
