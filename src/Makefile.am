# -------------------------------------------------------
# This is the base for the Makefile, produced by automake
#   and autoconf. 
# -------------------------------------------------------

myicon = -DWINDOW_ICON=\"${datadir}/pixmaps/gtranslator.png\"

if GCONF_IS_PRESENT
gconf_cflags = $(GCONF_CFLAGS) -DGCONF_IS_PRESENT=7
gconf_libs = $(GCONF_LIBS)
else
gconf_cflags =
gconf_libs =
endif

if USE_VFS_STUFF
vfs_cflags = $(VFS_CFLAGS) -DUSE_VFS_STUFF=2000
vfs_libs = $(VFS_LIBS)
else
vfs_cflags =
cfs_libs =
endif

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	$(vfs_cflags) \
	$(GNOME_INCLUDEDIR) \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\" \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	-DSCRIPTSDIR=\""$(datadir)/gtranslator/scripts"\" \
	$(XML_CFLAGS) \
	$(gconf_cflags) \
	$(myicon)

bin_PROGRAMS = gtranslator

gtranslator_SOURCES = \
	main.c \
	gui.c gui.h \
	parse.c	parse.h \
	header_stuff.c header_stuff.h \
	dialogs.c dialogs.h \
	find.c find.h \
	gtkspell.c gtkspell.h \
	about.c about.h \
	prefs.c prefs.h \
	session.c session.h \
	dnd.c dnd.h \
	languages.h \
	gnome-regex.c gnome-regex.h \
	open-po-file-dialog-functions.c \
	open-po-file-dialog-functions.h \
	open-po-file-dialog-interface.c \
	open-po-file-dialog-interface.h \
	open-differently.c \
	open-differently.h \
	update.c

gtranslator_LDADD = \
	$(top_builddir)/libgtranslator/libgtranslator.la \
	-L/usr/X11R6/lib \
	$(GNOME_LIBDIR) \
	$(GNOMEUI_LIBS) \
	$(INTLLIBS) \
	$(gconf_libs) \
	$(XML_LIBS) \
	$(vfs_libs)

EXTRA_DIST = ChangeLog pixmaps/untrans.xpm
