# -------------------------------------------------------
# This is the base for the Makefile, produced by automake
#   and autoconf. 
# -------------------------------------------------------

SUBDIRS = . backends pixmaps

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	$(GNOME_INCLUDEDIR) \
	$(GAL_CFLAGS) \
	$(VFS_CFLAGS) \
	$(GCONF_CFLAGS) \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\" \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	-DSCRIPTSDIR=\""$(datadir)/gtranslator/scripts"\" \
	-DWINDOW_ICON=\""${datadir}/pixmaps/gtranslator.png"\" \
	-DSCHEMESDIR=\""$(datadir)/gtranslator/colorschemes"\"

bin_PROGRAMS = gtranslator

gtranslator_SOURCES = \
	about.c about.h \
	actions.c actions.h \
	color-schemes.c color-schemes.h \
	dialogs.c dialogs.h \
	dnd.c dnd.h \
	find.c find.h \
	gnome-regex.c gnome-regex.h \
	gtkspell.c gtkspell.h \
	gui.c gui.h \
	header_stuff.c header_stuff.h \
	history.c history.h \
	htmlizer.c htmlizer.h \
	languages.h \
	main.c \
	menus.h menus_main.h menus_message_status.h menus_msic.h \
	message.c message.h \
	messages.h \
	nautilus-string.c nautilus-string.h \
	open-differently.c open-differently.h \
	parse.c	parse.h \
	parse_core.c \
	preferences.c preferences.h \
	prefs.c prefs_init.c prefs.h \
	query.c query.h \
	replace.c replace.h \
	session.c session.h \
	sidebar.c sidebar.h \
	sighandling.c sighandling.h \
	stylistics.c stylistics.h \
	syntax.c syntax.h \
	syntax-elements.h \
	undo-core.c undo-core.h \
	undo.c undo.h \
	update.c \
	utils.c utils.h \
	vfs-handle.c vfs-handle.h \
	views.c views.h

gtranslator_LDADD = \
	$(GNOME_LIBDIR) \
	$(GNOMEUI_LIBS) \
	$(INTLLIBS) \
	$(GCONF_LIBS) \
	$(VFS_LIBS) \
	$(GAL_LIBS)

EXTRA_DIST = ChangeLog
