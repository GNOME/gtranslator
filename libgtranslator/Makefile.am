# the Makefile.am for libgtranslator

bin_PROGRAMS = gtranslatord

noinst_PROGRAMS = test-libgtranslator

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
	parse-db.c \
	handle-db.c \
	preferences.c \
	team-handle.c

libgtranslator_la_LDFLAGS = -version-info @MAINVERSION@:@SUBVERSION@


if GCONF_IS_PRESENT
configstufflibs = $(GCONF_LIBS) $(GNOME_LIBS)
else
configstufflibs = $(GNOME_LIBS)
endif

libgtranslator_la_LIBADD = $(IDL_LIBS) $(GLIB_LIBS) \
	$(XML_LIBS) $(OAF_LIBS) $(configstufflibs)

libgtranslator_include_dir = $(includedir)/libgtranslator

libgtranslator_include__DATA = parse-db.h \
	handle-db.h \
	preferences.h \
	team-handle.h

gtranslatord_SOURCES = main.c

gtranslatord_LDADD = libgtranslator.la $(IDL_LIBS) $(OAF_LIBS) \
	$(XML_LIBS)

test_libgtranslator_SOURCES = test-lg.c

test_libgtranslator_LDADD = libgtranslator.la $(GLIB_LIBS) $(XML_LIBS)

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

EXTRA_DIST = $(idl__DATA) $(libgtranslator_include__DATA) foo.xml
