# -------------------------------------------------------
# This is the base for the Makefile, produced by automake
#   and autoconf. 
# -------------------------------------------------------

myicon = -DWINDOW_ICON=\"${datadir}/pixmaps/gtranslator.png\"

if GCONF_IS_PRESENT
gconf_cflags = $(GCONF_CFLAGS)
gconf_libs = $(GCONF_LIBS)
else
gconf_cflags =
gconf_libs =
endif



INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	$(GNOME_INCLUDEDIR) \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\" \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	-DSCRIPTSDIR=\""$(datadir)/gtranslator"\" \
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
	spell.c spell.h \
	about.c about.h \
	prefs.c prefs.h \
	session.c session.h \
	dnd.c dnd.h \
	languages.h \
	gnome-regex.c gnome-regex.h

gtranslator_LDADD = \
	$(top_builddir)/libgtranslator/libgtranslator.la \
	-L/usr/X11R6/lib \
	$(GNOME_LIBDIR) \
	$(GNOMEUI_LIBS) \
	$(INTLLIBS) \
	$(gconf_libs) \
	$(XML_LIBS)

EXTRA_DIST = ChangeLog
