# the Makefile.am for libgtranslator

noinst_LIBRARIES = libgtranslator.a

INCLUDES = -I$(includedir) $(IDL_CFLAGS) \
	$(GLIB_CFLAGS) $(GNOME_CFLAGS) \
	-DMESSAGE_DB_DIR=\"@MESSAGE_DB_DIR@\" \
	$(XML_CFLAGS)

libgtranslator_a_SOURCES = $(GTRANSLATORD_SOURCES) \
	parse-db.c

noinst_HEADERS = parse-db.h

GTRANSLATORD_SOURCES = gtranslatord.h \
	gtranslatord-common.c \
	gtranslatord-skels.c \
	gtranslatord-stubs.c

$(GTRANSLATORD_SOURCES): gtranslatord.idl
	orbit-idl $(IDL_CFLAGS) gtranslatord.idl

clean-am:
	-rm -f $(GTRANSLATORD_SOURCES)

EXTRA_DIST = gtranslatord.idl
