# source-code-view plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	-I$(top_srcdir)/src/plugin-system		\
	$(GTRANSLATOR_CFLAGS) 				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED_CFLAGS)			\
	-DDATADIR=\""$(pkgdatadir)"\"                   \
	-DGTR_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

plugin_LTLIBRARIES = \
	libsourcecodeview.la

libsourcecodeview_la_SOURCES = \
	source-code-view-plugin.c \
	source-code-view-plugin.h \
	viewer.c \
	viewer.h

libsourcecodeview_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)

gladedir = $(pkgdatadir)

glade_DATA =  \
        viewer.glade \
	source-code-view-dialog.glade

# Plugin Info

plugin_in_files = source-code-view.gtranslator-plugin.desktop.in

%.gtranslator.plugin: %.gtranslator-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.gtranslator-plugin.desktop.in=.gtranslator.plugin)

EXTRA_DIST = $(plugin_in_files)

CLEANFILES = $(plugin_DATA)
DISTCLEANFILES = $(plugin_DATA)

