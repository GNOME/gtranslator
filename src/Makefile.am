# --------------------------------------------------------
# Welcome to the gtranslator-automake-autoconf Wonderland!
# --------------------------------------------------------

SUBDIRS = toolbareditor

noinst_LTLIBRARIES = libgtranslator.la

pkglib_LTLIBRARIES = libgtranslator-private.la

INCLUDES = \
	-I$(top_srcdir)						\
	-I$(top_srcdir)/src/toolbareditor			\
	-I$(top_builddir)/src					\
	$(GTRANSLATOR_CFLAGS)					\
	$(GTKSPELL_CFLAGS)					\
	$(WARN_CFLAGS)						\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(INTROSPECTION_CFLAGS)					\
	-DDATADIR=\""$(datadir)"\"				\
	-DLIBDIR=\""$(libdir)"\"

bin_PROGRAMS = \
	gtranslator

gtranslator_SOURCES = main.c

gtranslator_LDADD =			\
	libgtranslator.la		\
	libgtranslator-private.la	\
	$(GTRANSLATOR_LIBS)		\
	$(GTKSPELL_LIBS)		\
	$(INTLLIBS)			\
	$(INTROSPECTION_LIBS)		\
	$(GETTEXT_LIBS)

if PLATFORM_WIN32
gtranslator_LDFLAGS = -Wl,--export-all-symbols -mwindows -Wl,--out-implib,libgtranslator-$(GTR_API_VERSION).a
else
gtranslator_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"
endif

libgtranslator_la_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"

BUILT_SOURCES = \
	gtr-marshal.h \
	gtr-marshal.c \
	gtr-enum-types.h \
	gtr-enum-types.c

BUILT_SOURCES_PRIVATE = \
	gtr-resources.c

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
	gtr-tab-activatable.h \
	gtr-tab.h \
	gtr-view.h \
	gtr-window-activatable.h \
	gtr-window.h

NOINST_H_FILES = \
	gtr-actions.h \
	gtr-actions-app.h \
	gtr-assistant.h \
	gtr-close-button.h \
	gtr-close-confirmation-dialog.h \
	gtr-dirs.h \
	gtr-file-dialogs.h \
	gtr-header-dialog.h \
	gtr-history-entry.h \
	gtr-io-error-info-bar.h \
	gtr-jump-dialog.h \
	gtr-language.h \
	gtr-languages-fetcher.h \
	gtr-message-container.h \
	gtr-message-table-model.h \
	gtr-plugins-engine.h \
	gtr-preferences-dialog.h \
	gtr-profile-dialog.h \
	gtr-profile-manager.h \
	gtr-search-dialog.h \
	gtr-settings.h \
	gtr-status-combo-box.h \
	gtr-tab-label.h \
	gtr-utils.h

headerdir = $(prefix)/include/gtranslator-@GTR_API_VERSION@/gtranslator

header_DATA = \
	$(INST_H_FILES)

libgtranslator_private_la_SOURCES = \
	gtr-tab-activatable.c \
	gtr-window-activatable.c \
	$(BUILT_SOURCES_PRIVATE)

libgtranslator_c_files = \
	gtr-actions-app.c \
	gtr-actions-documents.c \
	gtr-actions-edit.c \
	gtr-actions-file.c \
	gtr-actions-go.c \
	gtr-actions-search.c \
	gtr-actions-view.c \
	gtr-application.c \
	gtr-assistant.c \
	gtr-close-button.c \
	gtr-close-confirmation-dialog.c \
	gtr-context.c \
	gtr-dirs.c \
	gtr-file-dialogs.c \
	gtr-header.c \
	gtr-header-dialog.c \
	gtr-history-entry.c \
	gtr-io-error-info-bar.c \
	gtr-jump-dialog.c \
	gtr-language.c \
	gtr-languages-fetcher.c \
	gtr-message-container.c \
	gtr-message-table.c \
	gtr-message-table-model.c \
	gtr-msg.c \
	gtr-notebook.c \
	gtr-plugins-engine.c \
	gtr-po.c \
	gtr-preferences-dialog.c \
	gtr-profile.c \
	gtr-profile-dialog.c \
	gtr-profile-manager.c \
	gtr-search-dialog.c \
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
	toolbareditor/libtoolbareditor.la \
	$(GTRANSLATOR_LIBS)

libgtranslator_private_la_LIBADD = libgtranslator.la $(GTRANSLATOR_LIBS) $(GETTEXT_LIBS)

libgtranslator_private_la_LDFLAGS = -avoid-version

gtr-enum-types.h: gtr-enum-types.h.template $(INST_H_FILES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gtr-enum-types.h.template $(INST_H_FILES)) > $@

gtr-enum-types.c: gtr-enum-types.c.template $(INST_H_FILES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gtr-enum-types.c.template $(INST_H_FILES)) > $@

gtr-marshal.h: gtr-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN) $(GLIB_GENMARSHAL) $< --header --prefix=gtr_marshal > $@

gtr-marshal.c: gtr-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN) $(GLIB_GENMARSHAL) $< --body --header --prefix=gtr_marshal > $@

gtr-resources.c: gtranslator.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/gtranslator.gresource.xml)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source $(srcdir)/gtranslator.gresource.xml

EXTRA_DIST = \
	gtr-marshal.list \
	gtr-enum-types.h.template \
	gtr-enum-types.c.template \
	gtranslator.gresource.xml \
	gtranslator-ui.xml \
	gtranslator-menu.ui \
	gtr-header-dialog.ui \
	gtr-jump-dialog.ui \
	gtr-languages-fetcher.ui \
	gtr-preferences-dialog.ui \
	gtr-profile-dialog.ui \
	gtr-search-dialog.ui


if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS = Gtranslator-3.0.gir

Gtranslator-3.0.gir: gtranslator
INTROSPECTION_SCANNER_ARGS = -I$(top_srcdir) --warn-all --identifier-prefix=Gtr
Gtranslator_3_0_gir_NAMESPACE = Gtranslator
Gtranslator_3_0_gir_VERSION = 3.0
Gtranslator_3_0_gir_PROGRAM = $(builddir)/gtranslator
Gtranslator_3_0_gir_FILES = $(INST_H_FILES) $(libgtranslator_c_files)
Gtranslator_3_0_gir_INCLUDES = Gtk-3.0 GtkSource-3.0

girdir = $(datadir)/gtranslator/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/gtranslator/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES =	\
	$(gir_DATA)	\
	$(typelib_DATA)	\
	$(BUILT_SOURCES) \
	$(BUILT_SOURCES_PRIVATE)
endif

-include $(top_srcdir)/git.mk
