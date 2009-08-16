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
	$(DEPRECATED_FLAGS)                                     \
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

gtranslator_LDADD = libgtranslator.la $(GTRANSLATOR_LIBS) $(GTKSPELL_LIBS) $(INTLLIBS)

if PLATFORM_WIN32
gtranslator_LDFLAGS = -Wl,--export-all-symbols -mwindows -Wl,--out-implib,libgedit-$(GTR_API_VERSION).a
else
gtranslator_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"
endif


libgtranslator_la_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"

BUILT_SOURCES =                         \
        gtranslator-marshal.h  \
        gtranslator-marshal.c	\
	gtranslator-enum-types.h \
	gtranslator-enum-types.c

if PLATFORM_WIN32
gtranslator-res.o: gtranslator.rc
	$(WINDRES) -i gtranslator.rc --input-format=rc -o gtranslator-res.o -O coff

gtranslator_LDADD += gtranslator-res.o
endif

INST_H_FILES =				\
	application.h \
	context.h \
	debug.h \
	header.h \
	io-error-message-area.h \
	message-area.h \
	message-table.h \
	msg.h \
	notebook.h \
	po.h \
	profile.h \
	statusbar.h \
	tab.h \
	utils.h \
	view.h \
	window.h

headerdir = $(prefix)/include/gtranslator-@GTR_API_VERSION@/gtranslator

header_DATA = \
	$(INST_H_FILES)

libgtranslator_la_SOURCES = \
	$(BUILT_SOURCES) \
	application.c \
	message-area.c \
	message-table.c \
	message-table-model.c \
	message-table-model.h \
	notebook.c \
	tab.c \
	view.c \
	window.c \
	po.c \
	profile.c \
	utils.c \
	msg.c \
	actions.h \
	actions-file.c \
	actions-go.c \
	actions-edit.c \
	actions-view.c \
	actions-help.c \
	io-error-message-area.c \
	prefs-manager.c \
	prefs-manager.h \
	prefs-manager-private.h \
	prefs-manager-app.c \
	prefs-manager-app.h \
	history-entry.c \
	history-entry.h \
	actions-search.c \
	header.c \
	statusbar.c \
	context.c \
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

gtranslator-enum-types.h: gtranslator-enum-types.h.template $(INST_H_FILES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gtranslator-enum-types.h.template $(INST_H_FILES)) > $@
	
gtranslator-enum-types.c: gtranslator-enum-types.c.template $(INST_H_FILES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gtranslator-enum-types.c.template $(INST_H_FILES)) > $@

gtranslator-marshal.h: gtranslator-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN) $(GLIB_GENMARSHAL) $< --header --prefix=gtranslator_marshal > $@

gtranslator-marshal.c: gtranslator-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN) $(GLIB_GENMARSHAL) $< --body --header --prefix=gtranslator_marshal > $@

EXTRA_DIST =	\
	gtranslator-marshal.list	\
	gtranslator-enum-types.h.template	\
	gtranslator-enum-types.c.template

CLEANFILES = $(BUILT_SOURCES)

-include $(top_srcdir)/git.mk
