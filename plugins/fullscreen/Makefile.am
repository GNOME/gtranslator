# FullScreen plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	-I$(top_srcdir)/src/plugin-system		\
	-I$(top_srcdir)/src/dialogs			\
	$(GTRANSLATOR_CFLAGS) 				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED)				\
	-DGTR_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

plugin_LTLIBRARIES = \
	libfullscreen.la

libfullscreen_la_SOURCES = \
	gtr-fullscreen-plugin.c \
	gtr-fullscreen-plugin.h


libfullscreen_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)

# Plugin Info

plugin_in_files = gtr-fullscreen.gtranslator-plugin.desktop.in

%.gtranslator.plugin: %.gtranslator-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.gtranslator-plugin.desktop.in=.gtranslator.plugin)

EXTRA_DIST = $(plugin_in_files)

CLEANFILES = $(plugin_DATA)
DISTCLEANFILES = $(plugin_DATA)


-include $(top_srcdir)/git.mk
