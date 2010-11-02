# --------------------------------------------------------
# Welcome to the gtranslator-automake-autoconf Wonderland!
# --------------------------------------------------------

SUBDIRS = toolbareditor \
	  dialogs \
	  plugin-system \
	  translation-memory

noinst_LTLIBRARIES = libgtranslator.la

INCLUDES = \
	-I$(top_srcdir)						\
	-I$(includedir)						\
	-I$(top_srcdir)/src/dialogs				\
	-I$(top_srcdir)/src/toolbareditor			\
	-I$(top_srcdir)/src/plugin-system			\
	-I$(top_srcdir)/src/translation-memory			\
	-I$(top_srcdir)/src/translation-memory/berkeley		\
	-DGNOMELOCALEDIR=\""$(prefix)/${DATADIRNAME}/locale"\"	\
	-DPREFIX=\""$(prefix)"\"                                \
	-DSYSCONFDIR=\""$(sysconfdir)"\"                        \
	-DPKGDATADIR=\""$(pkgdatadir)"\"                        \
	-DDATADIR=\""$(datadir)"\"                              \
	-DLIBDIR=\""$(libdir)"\"                                \
	$(GTRANSLATOR_CFLAGS)                                   \
	$(GTKSPELL_CFLAGS)                                      \
	$(SPELLCHECK_CFLAGS)					\
	$(WARN_CFLAGS)						\
	$(DISABLE_DEPRECATED)                                   \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\"                         \
	-DPIXMAPSDIR=\""$(datadir)"/pixmaps/gtranslator\"

bin_PROGRAMS = \
	gtranslator

gtranslator_SOURCES = main.c

gtranslator_LDADD = libgtranslator.la $(GTRANSLATOR_LIBS) $(GTKSPELL_LIBS) $(INTLLIBS) -lgettextpo

if PLATFORM_WIN32
gtranslator_LDFLAGS = -Wl,--export-all-symbols -mwindows -Wl,--out-implib,libgedit-$(GTR_API_VERSION).a
else
gtranslator_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"
endif


libgtranslator_la_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"

BUILT_SOURCES = \
	gtr-marshal.h \
	gtr-marshal.c \
	gtr-enum-types.h \
	gtr-enum-types.c

if PLATFORM_WIN32
gtranslator-res.o: gtranslator.rc
	$(WINDRES) -i gtranslator.rc --input-format=rc -o gtranslator-res.o -O coff

gtranslator_LDADD += gtranslator-res.o
endif

INST_H_FILES = \
	gtr-application.h \
	gtr-context.h \
	gtr-debug.h \
	gtr-header.h \
	gtr-message-table.h \
	gtr-msg.h \
	gtr-notebook.h \
	gtr-po.h \
	gtr-profile.h \
	gtr-statusbar.h \
	gtr-tab.h \
	gtr-view.h \
	gtr-window.h

NOINST_H_FILES = \
	gtr-actions.h \
	gtr-close-button.h \
	gtr-dirs.h \
	gtr-history-entry.h \
	gtr-io-error-info-bar.h \
	gtr-message-table-model.h \
	gtr-profile-manager.h \
	gtr-settings.h \
	gtr-status-combo-box.h \
	gtr-tab-label.h \
	gtr-utils.h

headerdir = $(prefix)/include/gtranslator-@GTR_API_VERSION@/gtranslator

header_DATA = \
	$(INST_H_FILES)

libgtranslator_c_files = \
	gtr-actions-documents.c \
	gtr-actions-edit.c \
	gtr-actions-file.c \
	gtr-actions-go.c \
	gtr-actions-help.c \
	gtr-actions-search.c \
	gtr-actions-view.c \
	gtr-application.c \
	gtr-close-button.c \
	gtr-context.c \
	gtr-dirs.c \
	gtr-header.c \
	gtr-history-entry.c \
	gtr-io-error-info-bar.c \
	gtr-message-table.c \
	gtr-message-table-model.c \
	gtr-msg.c \
	gtr-notebook.c \
	gtr-po.c \
	gtr-profile.c \
	gtr-profile-manager.c \
	gtr-settings.c \
	gtr-statusbar.c \
	gtr-status-combo-box.c \
	gtr-tab.c \
	gtr-tab-label.c \
	gtr-utils.c \
	gtr-view.c \
	gtr-window.c

libgtranslator_la_SOURCES = \
	$(BUILT_SOURCES) \
	$(libgtranslator_c_files) \
	$(NOINST_H_FILES) \
	$(INST_H_FILES)

libgtranslator_la_LIBADD = \
	$(GTRANSLATOR_LIBS) \
	dialogs/libdialogs.la \
	toolbareditor/libtoolbareditor.la \
	plugin-system/libpluginsystem.la \
	translation-memory/libtranslationmemory.la

gtr-enum-types.h: gtr-enum-types.h.template $(INST_H_FILES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gtr-enum-types.h.template $(INST_H_FILES)) > $@
	
gtr-enum-types.c: gtr-enum-types.c.template $(INST_H_FILES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gtr-enum-types.c.template $(INST_H_FILES)) > $@

gtr-marshal.h: gtr-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN) $(GLIB_GENMARSHAL) $< --header --prefix=gtr_marshal > $@

gtr-marshal.c: gtr-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN) $(GLIB_GENMARSHAL) $< --body --header --prefix=gtr_marshal > $@

EXTRA_DIST =	\
	gtr-marshal.list	\
	gtr-enum-types.h.template	\
	gtr-enum-types.c.template

CLEANFILES = $(BUILT_SOURCES)

if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS = Gtranslator-2.0.gir

Gtranslator-2.0.gir: gtranslator
INTROSPECTION_SCANNER_ARGS = -I$(top_srcdir) --warn-all
Gtranslator_2_0_gir_NAMESPACE = Gtr
Gtranslator_2_0_gir_VERSION = 2.0
Gtranslator_2_0_gir_PROGRAM = $(builddir)/src
Gtranslator_2_0_gir_FILES = $(INST_H_FILES) $(libgtranslator_c_files)
Gtranslator_2_0_gir_INCLUDES = Gtk-3.0 GtkSource-3.0

girdir = $(datadir)/gtranslator/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/gtranslator/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES +=			\
	$(gir_DATA)	\
	$(typelib_DATA)
endif

-include $(top_srcdir)/git.mk
