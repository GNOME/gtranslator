## ------------------------------------------------------
## Process this file with automake to produce Makefile.in
## ------------------------------------------------------

#ifdef EMACS_LOVER
extra_flags = -DEMACS_LOVER=2000
#else
extra_flags = 
#endif

myicon = -DWINDOW_ICON=\"${datadir}/pixmaps/gtranslator.png\"

INCLUDES = $(extra_flags) $(myicon)\
	-I$(top_srcdir)/intl \
	$(GNOME_INCLUDEDIR) $(GLIB_CFLAGS)

bin_PROGRAMS = gtranslator

gtranslator_SOURCES = \
	main.c \
	gui.c gui.h \
	parse.c	parse.h \
	header_stuff.c header_stuff.h \
	msg_db.c msg_db.h \
	gtr_dialogs.c gtr_dialogs.h \
	about.c about.h \
	prefs.c prefs.h \
	session.c session.h \
	dnd.c dnd.h \
	messages.h \
	languages.h 

gtranslator_LDADD = $(GNOME_LIBDIR) $(GNOMEUI_LIBS) $(INTLLIBS) $(GLIB_LIBS)

EXTRA_DIST = ChangeLog
