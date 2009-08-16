# Open-tran Panel plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES =                                                      \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	-I$(top_srcdir)/src/plugin-system		\
	-I$(top_srcdir)/src/toolbareditor		\
	-DPIXMAPSDIR=\""$(datadir)"/pixmaps/gtranslator\"       \
	-DDATADIR=\""$(pkgdatadir)"\"                           \
	$(GTRANSLATOR_CFLAGS) 				\
	$(SOUP_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED_CFLAGS)			\
	-DGTR_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

plugin_LTLIBRARIES = libopentran.la

libopentran_la_SOURCES = \
	open-tran-panel.c \
	open-tran-panel.h\
	open-tran-plugin.c \
	open-tran-plugin.h

libopentran_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)					\
	$(SOUP_LIBS)

uidir = $(pkgdatadir)

ui_DATA =  \
	open-tran-dialog.ui

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
	fedora.png

# Plugin Info

plugin_in_files = open-tran.gtranslator-plugin.desktop.in

%.gtranslator.plugin: %.gtranslator-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.gtranslator-plugin.desktop.in=.gtranslator.plugin)

EXTRA_DIST = $(ui_DATA) $(pixmaps__DATA) $(plugin_in_files)

CLEANFILES = $(plugin_DATA)
DISTCLEANFILES = $(plugin_DATA)


-include $(top_srcdir)/git.mk
