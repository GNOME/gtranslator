# -------------------------------------------------------
# This is the base for the Makefile, produced by automake
#   and autoconf. 
# -------------------------------------------------------

SUBDIRS = . pixmaps

myicon = -DWINDOW_ICON=\"${datadir}/pixmaps/gtranslator.png\"

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
	$(myicon)

bin_PROGRAMS = gtranslator

gtranslator_SOURCES = \
	about.c about.h \
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
	open-differently.c open-differently.h \
	parse.c	parse.h \
	preferences.c preferences.h \
	prefs.c prefs.h \
	session.c session.h \
	shortcutbar.c shortcutbar.h \
	stylistics.c stylistics.h \
	update.c \
	vfs-handle.c vfs-handle.h

gtranslator_LDADD = \
	-L/usr/X11R6/lib \
	$(GNOME_LIBDIR) \
	$(GNOMEUI_LIBS) \
	$(INTLLIBS) \
	$(gconf_libs) \
	$(VFS_LIBS) \
	$(GAL_LIBS)

EXTRA_DIST = ChangeLog
