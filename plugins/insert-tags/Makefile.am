# Insert Tags plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	$(GTRANSLATOR_CFLAGS) 				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED)

plugin_LTLIBRARIES = \
	libinserttags.la

libinserttags_la_SOURCES = \
	gtr-insert-tags-plugin.c \
	gtr-insert-tags-plugin.h

libinserttags_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)

# Plugin Info

plugin_in_files = gtr-insert-tags.plugin.desktop.in

%.plugin: %.plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.plugin.desktop.in=.plugin)

EXTRA_DIST = $(plugin_in_files)
CLEANFILES = $(plugin_DATA)
DISTCLEANFILES = $(plugin_DATA)

-include $(top_srcdir)/git.mk
