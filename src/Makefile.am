# --------------------------------------------------------
# Welcome to the gtranslator-automake-autoconf Wonderland!
# --------------------------------------------------------

INCLUDES = \
	-I$(top_srcdir)						\
	-I$(includedir)						\
	-DGNOMELOCALEDIR=\""$(prefix)/${DATADIRNAME}/locale"\"	\
        -DPREFIX=\""$(prefix)"\"                                \
        -DSYSCONFDIR=\""$(sysconfdir)"\"                        \
        -DDATADIR=\""$(datadir)"\"                              \
        -DLIBDIR=\""$(libdir)"\"                                \
	$(GTRANSLATOR_CFLAGS)                                   \
	$(GTKSPELL_CFLAGS)                                      \
	$(DEPRECATED_FLAGS)                                     \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\"                         \
	-DSCHEMESDIR=\""$(datadir)"/gtranslator/colorschemes\"	\
	-DSCRIPTSDIR=\""$(datadir)"/gtranslator/scripts\"	\
	-DWINDOW_ICON=\""$(datadir)"/pixmaps/gtranslator.png\"

bin_PROGRAMS = \
	gtranslator
#	gtranslator-auto-translate \
#	gtranslator-auto-learn \
#	gtranslator-export-learn-buffer

gtranslator_SOURCES = \
	about.c about.h \
	actions.c actions.h \
	bookmark.c bookmark.h \
	comment.c comment.h \
	compile.c compile.h \
	dialogs.c dialogs.h \
	dnd.c dnd.h \
	find.c find.h \
	gui.c gui.h \
	header_stuff.c header_stuff.h \
	history.c history.h \
	languages.c languages.h \
	learn.c learn.h \
	main.c \
	menus.c menus.h \
	message.c message.h \
	message-area.c message-area.h \
	messages-table.c messages-table.h \
	nautilus-string.c nautilus-string.h \
	parse.c parse.h \
	page.c page.h \
	preferences.c preferences.h \
	prefs.c prefs_init.c prefs.h \
	replace.c replace.h \
	runtime-config.c runtime-config.h \
	session.c session.h \
	sighandling.c sighandling.h \
	translator.c translator.h \
	undo.c undo.h \
	utils.c utils.h \
	utils_gui.c utils_gui.h \
	vfs-handle.c vfs-handle.h

gtranslator_LDADD = \
	$(INTLLIBS) \
	$(GTRANSLATOR_LIBS) \
	$(GTKSPELL_LIBS)
	
gtranslator_LDFLAGS = \
	-Wall

#gtranslator_auto_learn_SOURCES = \
#	auto-learn.c
#	
#gtranslator_auto_translate_SOURCES = \
#	auto-translate.c
#	
#gtranslator_export_learn_buffer_SOURCES = \
#	export-learn-buffer.c
#	
EXTRA_DIST = ChangeLog
