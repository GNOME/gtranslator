## ------------------------------------------------------
## Process this file with automake to produce Makefile.in
## ------------------------------------------------------
## Put the -DDEBUG , so on CVS a debuggable version 
## should be build , too .
## ------------------------------------------------------

extra_flags = -DDEBUG

INCLUDES = \
	-I$(top_srcdir)/intl \
	$(GNOME_INCLUDEDIR)  \
	$(extra_flags)

bin_PROGRAMS = gtranslator

gtranslator_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	parse.c	parse.h \
	header_stuff.c header_stuff.h 

gtranslator_LDADD = $(GNOME_LIBDIR) $(GNOMEUI_LIBS) $(INTLLIBS)

EXTRA_DIST = ChangeLog
