# -------------------------------------------------------
# This is the base for the Makefile, produced by automake
#   and autoconf. 
# -------------------------------------------------------

myicon = -DWINDOW_ICON=\"${datadir}/pixmaps/gtranslator.png\"

INCLUDES = $(myicon) -I../libgtranslator \
	-I$(top_srcdir)/intl \
	-DG_LOG_DOMAIN=\"$(PACKAGE)\" \
	$(GNOME_INCLUDEDIR) $(GLIB_CFLAGS) \
	$(XML_CFLAGS)

bin_PROGRAMS = gtranslator

gtranslator_SOURCES = \
	gui.c gui.h \
	parse.c	parse.h \
	header_stuff.c header_stuff.h \
	msg_db.c msg_db.h \
	dialogs.c dialogs.h \
	about.c about.h \
	prefs.c prefs.h \
	session.c session.h \
	dnd.c dnd.h \
	find.c find.h \
	messages.h \
	languages.h \
	main.c 

gtranslator_LDADD = ../libgtranslator/libgtranslator.la $(GNOME_LIBDIR) \
		$(GNOMEUI_LIBS) $(INTLLIBS) $(GLIB_LIBS) \
		$(XML_LIBS)

EXTRA_DIST = ChangeLog TODO.src
