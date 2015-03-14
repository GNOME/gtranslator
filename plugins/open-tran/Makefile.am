# Open-tran Panel plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES =						\
	-I$(top_srcdir)					\
	-I$(top_srcdir)/src				\
	$(GTRANSLATOR_CFLAGS)				\
	$(JSON_GLIB_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED)

BUILT_SOURCES = \
	gtr-open-tran-resources.c

plugin_LTLIBRARIES = libopentran.la

libopentran_la_SOURCES = \
	gtr-open-tran-panel.c \
	gtr-open-tran-panel.h\
	gtr-open-tran-plugin.c \
	gtr-open-tran-plugin.h \
	$(BUILT_SOURCES)

libopentran_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)				\
	$(JSON_GLIB_LIBS)

gtr-open-tran-resources.c: gtr-open-tran.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies gtr-open-tran.gresource.xml)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source gtr-open-tran.gresource.xml


pixmaps_dir = $(datadir)/gtranslator/pixmaps
pixmaps__DATA = \
	gnome.png \
	kde.ico \
	mozilla.png \
	debian.png \
	open-tran.png \
	suse.png \
	xfce.png \
	inkscape.png \
	oo-logo.png \
	fedora.png \
	mandriva.png

# Plugin Info

plugin_in_files = gtr-open-tran.plugin.desktop.in

%.plugin: %.plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.plugin.desktop.in=.plugin)

gsettings_SCHEMAS = org.gnome.gtranslator.plugins.open-tran.gschema.xml
@GSETTINGS_RULES@

EXTRA_DIST =	\
	$(pixmaps__DATA)	\
	$(plugin_in_files)	\
	gtr-open-tran.gresource.xml	\
	gtr-open-tran-dialog.ui

CLEANFILES = $(plugin_DATA) $(BUILT_SOURCES)
DISTCLEANFILES = $(plugin_DATA) $(BUILT_SOURCES)

-include $(top_srcdir)/git.mk
