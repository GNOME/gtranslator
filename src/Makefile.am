## ------------------------------------------------------
## Process this file with automake to produce Makefile.in
## ------------------------------------------------------
## Removed the DEBUG define as it's really useless ..
## ------------------------------------------------------

#ifdef EMACS_LOVER
extra_flags = -DEMACS_LOVER=2000
#else
extra_flags = 
#endif

INCLUDES = $(extra_flags) \
	-I$(top_srcdir)/intl \
	$(GNOME_INCLUDEDIR) $(GLIB_CFLAGS)

bin_PROGRAMS = gtranslator

gtranslator_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
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
