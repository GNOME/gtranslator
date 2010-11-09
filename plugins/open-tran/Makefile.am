# Open-tran Panel plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES =						\
	-I$(top_srcdir)					\
	-I$(top_srcdir)/src				\
	$(GTRANSLATOR_CFLAGS)				\
	$(JSON_GLIB_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED)

plugin_LTLIBRARIES = libopentran.la

libopentran_la_SOURCES = \
	gtr-open-tran-panel.c \
	gtr-open-tran-panel.h\
	gtr-open-tran-plugin.c \
	gtr-open-tran-plugin.h

libopentran_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)				\
	$(JSON_GLIB_LIBS)

uidir = $(pkgdatadir)/ui

ui_DATA =  \
	gtr-open-tran-dialog.ui

pixmaps_dir = $(datadir)/pixmaps/gtranslator
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

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

EXTRA_DIST =	\
	$(ui_DATA)		\
	$(pixmaps__DATA)	\
	$(plugin_in_files)	\
	org.gnome.gtranslator.plugins.open-tran.gschema.xml.in.in

CLEANFILES = $(plugin_DATA) $(gsettings_SCHEMAS)
DISTCLEANFILES = $(plugin_DATA) $(gsettings_SCHEMAS)

MAINTAINERCLEANFILES = $(gsettings_SCHEMAS:.xml=.valid)

-include $(top_srcdir)/git.mk
