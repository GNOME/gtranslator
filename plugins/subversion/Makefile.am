# FullScreen plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	-I$(top_srcdir)/src/plugin-system		\
	-I$(top_srcdir)/src/dialogs			\
	-DDATADIR=\""$(pkgdatadir)"\"                   \
	$(GTRANSLATOR_CFLAGS) 				\
	$(SVN_CFLAGS)					\
        $(SVN_INCLUDE)					\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED_CFLAGS)			\
	-DGTR_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

plugin_LTLIBRARIES = \
	libsubversion.la

BUILT_SOURCES = \
	subversion-enum-types.h \
	subversion-enum-types.c

NOINST_H_FILES = \
	subversion-plugin.h \
	command.h \
	async-command.h \
	svn-command.h \
	svn-commit-command.h \
	svn-update-command.h \
	svn-add-command.h \
	svn-status-command.h \
	svn-status.h \
	commit-dialog.h \
	vcs-status-tree-view.h \
	subversion-utils.h \
	svn-diff-command.h \
	svn-resolve-command.h \
	svn-cat-command.h \
	svn-checkout-command.h \
	update-dialog.h \
	diff-dialog.h \
	checkout-dialog.h

libsubversion_la_SOURCES = \
	$(BUILT_SOURCES) \
	subversion-plugin.c \
	command.c \
	async-command.c \
	svn-command.c \
	svn-commit-command.c \
	svn-update-command.c \
	svn-add-command.c \
	svn-status-command.c \
	svn-status.c \
	commit-dialog.c \
	vcs-status-tree-view.c \
	subversion-utils.c \
	svn-diff-command.c \
	svn-resolve-command.c \
	update-dialog.c \
	svn-cat-command.c \
	svn-checkout-command.c \
	diff-dialog.c \
	checkout-dialog.c \
	$(NOINST_H_FILES)

libsubversion_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)				\
	$(SVN_LIB)

gladedir = $(pkgdatadir)

glade_DATA =  \
	subversion.glade

subversion-enum-types.h: subversion-enum-types.h.template $(NOINST_H_FILES) $(GLIB_MKENUMS)
	(cd $(srcdir) && $(GLIB_MKENUMS) --template subversion-enum-types.h.template $(NOINST_H_FILES)) > $@

subversion-enum-types.c: subversion-enum-types.c.template $(NOINST_H_FILES) $(GLIB_MKENUMS)
	(cd $(srcdir) && $(GLIB_MKENUMS) --template subversion-enum-types.c.template $(NOINST_H_FILES)) > $@

# Plugin Info

plugin_in_files = subversion.gtranslator-plugin.desktop.in

%.gtranslator.plugin: %.gtranslator-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.gtranslator-plugin.desktop.in=.gtranslator.plugin)

EXTRA_DIST = $(plugin_in_files)

CLEANFILES = $(plugin_DATA) $(BUILT_SOURCES)
DISTCLEANFILES = $(plugin_DATA)

