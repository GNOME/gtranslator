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

if USE_VFS_STUFF
vfs_cflags = $(VFS_CFLAGS) -DUSE_VFS_STUFF=7
vfs_libs = $(VFS_LIBS)
else
vfs_cflags =
vfs_libs =
endif

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	$(vfs_cflags) \
	$(GAL_CFLAGS) \
	$(GTKHTML_CFLAGS) \
	$(GNOME_INCLUDEDIR) \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\" \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	-DSCRIPTSDIR=\""$(datadir)/gtranslator/scripts"\" \
	$(XML_CFLAGS) \
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
	open-differently.c open-differently.h \
	parse.c	parse.h \
	preferences.c preferences.h \
	prefs.c prefs.h \
	session.c session.h \
	shortcutbar.c shortcutbar.h \
	stylistics.c stylistics.h \
	update.c

gtranslator_LDADD = \
	$(top_builddir)/libgtranslator/libgtranslator.a \
	-L/usr/X11R6/lib \
	$(GNOME_LIBDIR) \
	$(GNOMEUI_LIBS) \
	$(INTLLIBS) \
	$(gconf_libs) \
	$(XML_LIBS) \
	$(vfs_libs) \
	$(GAL_LIBS) \
	$(GTKHTML_LIBS)

EXTRA_DIST = ChangeLog
