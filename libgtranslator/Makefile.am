# the Makefile.am for libgtranslator

noinst_LIBRARIES = libgtranslator.a

INCLUDES = -I$(includedir) $(IDL_CFLAGS) \
	$(GLIB_CFLAGS) $(GNOME_CFLAGS) \
	-DMESSAGE_DB_DIR=\"@MESSAGE_DB_DIR@/message-db.d\" \
	$(XML_CFLAGS) -I../src

libgtranslator_a_SOURCES = $(GTRANSLATORD_SOURCES) \
	$(GTRANSLATOR_EXTRA_SOURCES) \
	parse-db.c

noinst_HEADERS = parse-db.h

GTRANSLATORD_SOURCES = gtranslatord.h \
	gtranslatord-common.c \
	gtranslatord-skels.c \
	gtranslatord-stubs.c

GTRANSLATOR_EXTRA_SOURCES = gtranslator.h \
	gtranslator-common.c \
	gtranslator-skels.c \
	gtranslator-stubs.c

$(GTRANSLATORD_SOURCES): gtranslatord.idl
	orbit-idl $(IDL_CFLAGS) gtranslatord.idl

$(GTRANSLATOR_EXTRA_SOURCES): gtranslator.idl
	orbit-idl $(IDL_CFLAGS) gtranslator.idl

clean-am:
	-rm -f $(GTRANSLATORD_SOURCES) $(GTRANSLATOR_EXTRA_SOURCES)

EXTRA_DIST = gtranslatord.idl gtranslator.idl
