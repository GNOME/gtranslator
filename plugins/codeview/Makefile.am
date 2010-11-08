# source-code-view plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	$(GTRANSLATOR_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED)

plugin_LTLIBRARIES = \
	libsourcecodeview.la

libsourcecodeview_la_SOURCES = \
	gtr-codeview-plugin.c \
	gtr-codeview-plugin.h \
	gtr-viewer.c \
	gtr-viewer.h

libsourcecodeview_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)

uidir = $(pkgdatadir)/ui

ui_DATA =  \
	gtr-viewer.ui \
	gtr-codeview-dialog.ui

# Plugin Info

plugin_in_files = gtr-codeview.plugin.desktop.in

%.plugin: %.plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.plugin.desktop.in=.plugin)

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
