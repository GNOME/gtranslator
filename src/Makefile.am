# -------------------------------------------------------
# This is the base for the Makefile, produced by automake
#   and autoconf. 
# -------------------------------------------------------

SUBDIRS = . pixmaps

if USE_WINDOW_ICON
myicon = \
	-DWINDOW_ICON=\""${datadir}/pixmaps/gtranslator.png"\"
else
myicon =
endif

if GCONF_IS_PRESENT
gconf_cflags = $(GCONF_CFLAGS) -DGCONF_IS_PRESENT=7
gconf_libs = $(GCONF_LIBS)
else
gconf_cflags =
gconf_libs =
endif

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	$(GAL_CFLAGS) \
	$(VFS_CFLAGS) \
	$(GNOME_INCLUDEDIR) \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\" \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	-DSCRIPTSDIR=\""$(datadir)/gtranslator/scripts"\" \
	$(gconf_cflags) \
	$(myicon) \
	-DSCHEMESDIR=\""$(datadir)/gtranslator/colorschemes"\"

bin_PROGRAMS = gtranslator

gtranslator_SOURCES = \
	about.c about.h \
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
	update.c \
	vfs-handle.c vfs-handle.h \
	views.c views.h

gtranslator_LDADD = \
	-L/usr/X11R6/lib \
	$(GNOME_LIBDIR) \
	$(GNOMEUI_LIBS) \
	$(INTLLIBS) \
	$(gconf_libs) \
	$(VFS_LIBS) \
	$(GAL_LIBS)

EXTRA_DIST = ChangeLog
