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
	args-tags.c args-tags.h \
	bookmark.c bookmark.h \
	charmap-panel.c charmap-panel.h \
	color-schemes.c color-schemes.h \
	comment.c comment.h \
	compile.c compile.h \
	dialogs.c dialogs.h \
	dictionary.c dictionary.h \
	dict-panel.c dict-panel.h \
	dnd.c dnd.h \
	draw-spaces.c draw-spaces.h \
	find.c find.h \
	gdict-sidebar.c gdict-sidebar.h \
	glossary-panel.c glossary-panel.h \
	gucharmap.c gucharmap.h \
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
	stylistics.c stylistics.h \
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

gladedir = $(pkgdatadir)

glade_DATA =  \
       header_dialog.glade \
       main_window.glade \
       preferences.glade

EXTRA_DIST = ChangeLog $(glade_DATA)


