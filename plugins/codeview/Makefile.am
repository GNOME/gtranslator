# source-code-view plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	-I$(top_srcdir)/src/plugin-system		\
	$(GTRANSLATOR_CFLAGS) 				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED)				\
	-DDATADIR=\""$(pkgdatadir)"\"                   \
	-DGTR_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

plugin_LTLIBRARIES = \
	libsourcecodeview.la

libsourcecodeview_la_SOURCES = \
	gtr-source-code-view-plugin.c \
	gtr-source-code-view-plugin.h \
	gtr-viewer.c \
	gtr-viewer.h

libsourcecodeview_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)

uidir = $(pkgdatadir)/ui

ui_DATA =  \
        gtr-viewer.ui \
	gtr-source-code-view-dialog.ui

# Plugin Info

plugin_in_files = gtr-source-code-view.gtranslator-plugin.desktop.in

%.gtranslator.plugin: %.gtranslator-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.gtranslator-plugin.desktop.in=.gtranslator.plugin)

gsettings_SCHEMAS = org.gnome.gtranslator.plugins.codeview.gschema.xml

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

EXTRA_DIST =	\
	$(plugin_in_files)	\
	$(ui_DATA)		\
	org.gnome.gtranslator.plugins.codeview.gschema.xml.in.in

CLEANFILES = $(plugin_DATA) $(gsettings_SCHEMAS)
DISTCLEANFILES = $(plugin_DATA) $(gsettings_SCHEMAS)

MAINTAINERCLEANFILES = $(gsettings_SCHEMAS:.xml=.valid)

-include $(top_srcdir)/git.mk
