# -------------------------------------------------------
# This is the base for the Makefile, produced by automake
#   and autoconf. 
# -------------------------------------------------------

myicon = -DWINDOW_ICON=\"${datadir}/pixmaps/gtranslator.png\"

INCLUDES = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libgtranslator \
	-I$(includedir) \
	$(GNOME_INCLUDEDIR) \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\" \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	-DSCRIPTSDIR=\""$(datadir)/gtranslator"\" \
	$(XML_CFLAGS) \
	if GCONF_IS_PRESENT
	$(GCONF_CFLAGS) -DGCONF_IS_PRESENT=7 \
	endif
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
	messages.h \
	languages.h \
	gnome-regex.c gnome-regex.h

gtranslator_LDADD = \
	$(top_builddir)/libgtranslator/libgtranslator.la \
	$(GNOME_LIBDIR) \
	$(GNOMEUI_LIBS) \
	$(INTLLIBS) \
	if GCONF_IS_PRESENT
	$(GCONF_LIBS)
	endif
	$(XML_LIBS)

EXTRA_DIST = ChangeLog TODO.src
