## ------------------------------------------------------
## Process this file with automake to produce Makefile.in
## ------------------------------------------------------
## Put the -DDEBUG , so on CVS a debuggable version 
## should be build , too .
## ------------------------------------------------------

msg_db_dir = $(datadir)/gtranslator/

extra_flags = -DDEBUG 

INCLUDES = \
	-I$(top_srcdir)/intl \
	$(GNOME_INCLUDEDIR)  \
	$(extra_flags) $(GLIB_CFLAGS) -DMSG_DB_DIR=\"$(msg_db_dir)\"

bin_PROGRAMS = gtranslator

gtranslator_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	parse.c	parse.h \
	header_stuff.c header_stuff.h \
	msg_db.h msg_db.c \
	messages.h 

gtranslator_LDADD = $(GNOME_LIBDIR) $(GNOMEUI_LIBS) $(INTLLIBS) $(GLIB_LIBS)

EXTRA_DIST = ChangeLog
