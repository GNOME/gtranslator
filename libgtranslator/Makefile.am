# the Makefile.am for libgtranslator

bin_PROGRAMS = gtranslatord

lib_LTLIBRARIES = libgtranslator.la

if GCONF_IS_PRESENT
configstuffcflags = $(GCONF_CFLAGS) $(GNOME_CFLAGS) -DGCONF_IS_PRESENT=7
else
configstuffcflags = $(GNOME_CFLAGS)
endif

INCLUDES = \
	-I$(includedir) $(IDL_CFLAGS) \
	$(GLIB_CFLAGS) $(configstuffcflags) \
	$(OAF_CFLAGS) $(XML_CFLAGS) -I../src \
	-DMESSAGE_DB_DIR=\"@MESSAGE_DB_DIR@/message-db.d\"

libgtranslator_la_SOURCES = $(GTRANSLATORD_SOURCES) \
	gtranslator-preferences-dialog.c \
	parse-db.c \
	handle-db.c \
	preferences.c \
	stylistics.c \
	team-handle.c

libgtranslator_la_LDFLAGS = -version-info @MAINVERSION@:@SUBVERSION@


if GCONF_IS_PRESENT
configstufflibs = $(GCONF_LIBS) $(GNOME_LIBS)
else
configstufflibs = $(GNOME_LIBS)
endif

libgtranslator_la_LIBADD = $(IDL_LIBS) $(GLIB_LIBS) \
	$(XML_LIBS) $(OAF_LIBS) -L/usr/X11R6/lib \
	$(GNOMEUI_LIBS) $(configstufflibs)

libgtranslator_include_dir = $(includedir)/libgtranslator

libgtranslator_include__DATA = parse-db.h \
	gtranslator-preferences-dialog.h \
	handle-db.h \
	messages.h \
	preferences.h \
	stylistics.h \
	team-handle.h

gtranslatord_SOURCES = main.c

gtranslatord_LDADD = libgtranslator.la $(IDL_LIBS) \
	-L/usr/X11R6/lib $(OAF_LIBS)  $(XML_LIBS)

idl_dir = $(datadir)/idl/gtranslator

idl__DATA = gtranslatord.idl

GTRANSLATORD_SOURCES = gtranslatord.h \
	gtranslatord-common.c \
	gtranslatord-skels.c \
	gtranslatord-stubs.c

$(GTRANSLATORD_SOURCES): gtranslatord.idl
	orbit-idl $(IDL_CFLAGS) gtranslatord.idl

clean-am:
	-rm -f $(GTRANSLATORD_SOURCES)

EXTRA_DIST = $(idl__DATA) $(libgtranslator_include__DATA)
