## ------------------------------------------------------
## Process this file with automake to produce Makefile.in
## ------------------------------------------------------
## Put the -DDEBUGGY , so on CVS a debuggable version 
## should be build , too .
## ------------------------------------------------------

scriptsdir = $(datadir)/$(PACKAGE)/scripts/

extra_flags = -DDEBUGGY -g1

INCLUDES = \
	-I$(top_srcdir)/intl \
	$(GNOME_INCLUDEDIR)  \
	-DSCRIPTS_DIR=\""$(scriptsdir)"\" \
	$(extra_flags)

bin_PROGRAMS = gtranslator

gtranslator_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	parse.c	parse.h 

gtranslator_LDADD = $(GNOME_LIBDIR) $(GNOMEUI_LIBS) $(INTLLIBS)

EXTRA_DIST = ChangeLog
