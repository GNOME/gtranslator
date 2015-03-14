# source-code-view plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	$(GTRANSLATOR_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED)

BUILT_SOURCES = \
	gtr-codeview-resources.c

plugin_LTLIBRARIES = \
	libsourcecodeview.la

libsourcecodeview_la_SOURCES = \
	gtr-codeview-plugin.c \
	gtr-codeview-plugin.h \
	gtr-viewer.c \
	gtr-viewer.h \
	$(BUILT_SOURCES)

libsourcecodeview_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)


gtr-codeview-resources.c: gtr-codeview.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies gtr-codeview.gresource.xml)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source gtr-codeview.gresource.xml

# Plugin Info

plugin_in_files = gtr-codeview.plugin.desktop.in

%.plugin: %.plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.plugin.desktop.in=.plugin)

gsettings_SCHEMAS = org.gnome.gtranslator.plugins.codeview.gschema.xml
@GSETTINGS_RULES@

EXTRA_DIST =	\
	$(plugin_in_files)	\
	gtr-codeview.gresource.xml	\
	gtr-codeview-dialog.ui	\
	gtr-viewer.ui

CLEANFILES = $(plugin_DATA) $(BUILT_SOURCES)
DISTCLEANFILES = $(plugin_DATA) $(BUILT_SOURCES)

-include $(top_srcdir)/git.mk
