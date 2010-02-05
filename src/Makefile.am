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
	-DSCRIPTSDIR=\""$(pkgdatadir)"/scripts\"	\
	-DWINDOW_ICON=\""$(datadir)"/pixmaps/gtranslator.png\"  \
	-DPIXMAPSDIR=\""$(datadir)"/pixmaps/gtranslator\"

bin_PROGRAMS = \
	gtranslator
#	gtranslator-auto-translate \
#	gtranslator-auto-learn \
#	gtranslator-export-learn-buffer

gtranslator_SOURCES = main.c

gtranslator_LDADD = libgtranslator.la $(GTRANSLATOR_LIBS) $(GTKSPELL_LIBS) $(INTLLIBS) -lgettextpo

if PLATFORM_WIN32
gtranslator_LDFLAGS = -Wl,--export-all-symbols -mwindows -Wl,--out-implib,libgedit-$(GTR_API_VERSION).a
else
gtranslator_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"
endif


libgtranslator_la_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"

BUILT_SOURCES =                         \
	gtr-marshal.h  \
	gtr-marshal.c	\
	gtr-enum-types.h \
	gtr-enum-types.c

if PLATFORM_WIN32
gtranslator-res.o: gtranslator.rc
	$(WINDRES) -i gtranslator.rc --input-format=rc -o gtranslator-res.o -O coff

gtranslator_LDADD += gtranslator-res.o
endif

INST_H_FILES =				\
	gtr-application.h \
	gtr-context.h \
	gtr-debug.h \
	gtr-header.h \
	gtr-io-error-info-bar.h \
	gtr-message-table.h \
	gtr-msg.h \
	gtr-notebook.h \
	gtr-po.h \
	gtr-profile.h \
	gtr-statusbar.h \
	gtr-tab.h \
	gtr-utils.h \
	gtr-view.h \
	gtr-window.h

headerdir = $(prefix)/include/gtranslator-@GTR_API_VERSION@/gtranslator

header_DATA = \
	$(INST_H_FILES)

libgtranslator_la_SOURCES = \
	$(BUILT_SOURCES) \
	gtr-application.c \
	gtr-dirs.h \
	gtr-dirs.c \
	gtr-message-table.c \
	gtr-message-table-model.c \
	gtr-message-table-model.h \
	gtr-notebook.c \
	gtr-tab.c \
	gtr-view.c \
	gtr-window.c \
	gtr-po.c \
	gtr-profile.c \
	gtr-utils.c \
	gtr-msg.c \
	gtr-actions.h \
	gtr-actions-documents.c \
	gtr-actions-file.c \
	gtr-actions-go.c \
	gtr-actions-edit.c \
	gtr-actions-view.c \
	gtr-actions-help.c \
	gtr-io-error-info-bar.c \
	gtr-prefs-manager.c \
	gtr-prefs-manager.h \
	gtr-prefs-manager-private.h \
	gtr-prefs-manager-app.c \
	gtr-prefs-manager-app.h \
	gtr-history-entry.c \
	gtr-history-entry.h \
	gtr-actions-search.c \
	gtr-header.c \
	gtr-statusbar.c \
	gtr-context.c \
	$(INST_H_FILES) 

libgtranslator_la_LIBADD = \
	$(GTRANSLATOR_LIBS) \
	dialogs/libdialogs.la \
	toolbareditor/libtoolbareditor.la \
	plugin-system/libpluginsystem.la \
	translation-memory/libtranslationmemory.la
	
#gtranslator_auto_learn_SOURCES = \
#	auto-learn.c
#	
#gtranslator_auto_translate_SOURCES = \
#	auto-translate.c
#	
#gtranslator_export_learn_buffer_SOURCES = \
#	export-learn-buffer.c
#	

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

-include $(top_srcdir)/git.mk
