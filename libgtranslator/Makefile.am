# the Makefile.am for libgtranslator

lib_LTLIBRARIES = libgtranslator.la

INCLUDES = -I$(includedir) $(IDL_CFLAGS) \
	$(GLIB_CFLAGS) $(GNOME_CFLAGS) \
	$(OAF_CFLAGS) $(XML_CFLAGS) -I../src \
	-DMESSAGE_DB_DIR=\"@MESSAGE_DB_DIR@/message-db.d\"

libgtranslator_la_SOURCES = $(GTRANSLATORD_SOURCES) \
	parse-db.c \
	handle-db.c \
	team-handle.c

libgtranslator_la_LDFLAGS = -avoid-version

libgtranslator_la_LIBADD = $(IDL_LIBS) $(GLIB_LIBS) \
	$(XML_LIBS) $(OAF_LIBS)

libgtranslator_include_dir = $(includedir)/libgtranslator

libgtranslator_include__DATA = parse-db.h \
	handle-db.h \
	team-handle.h

idl_dir = $(datadir)/idl

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
