# the Makefile.am for libgtranslator

bin_PROGRAMS = gtranslatord

lib_LTLIBRARIES = libgtranslator.la

if GCONF_IS_PRESENT
configstuffcflags = $(GCONF_CFLAGS) $(GNOME_CFLAGS) -DGCONF_IS_PRESENT=7
else
configstuffcflags = $(GNOME_CFLAGS)
endif

if USE_VFS_STUFF
vfs_cflags = $(VFS_CFLAGS) -DUSE_VFS_STUFF=2000
vfs_source = vfs-handle.c
else
vfs_cflags =
vfs_source =
endif

INCLUDES = \
	-I$(includedir) $(IDL_CFLAGS) \
	$(GLIB_CFLAGS) $(configstuffcflags) \
	$(OAF_CFLAGS) $(XML_CFLAGS) -I../src \
	$(vfs_cflags) -DG_LOG_DOMAIN=\"libgtranslator\" \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	-DMESSAGE_DB_DIR=\"@MESSAGE_DB_DIR@/message-db.d\"

libgtranslator_la_SOURCES = \
	$(GTRANSLATORD_SOURCES) \
	gtkled.c \
	handle-db.c \
	parse-db.c \
	preferences.c \
	stylistics.c \
	team-handle.c \
	$(vfs_source)

libgtranslator_la_LDFLAGS = -version-info @MAINVERSION@:@SUBVERSION@


if GCONF_IS_PRESENT
configstufflibs = $(GCONF_LIBS) $(GNOME_LIBS)
else
configstufflibs = $(GNOME_LIBS)
endif

if USE_VFS_STUFF
vfs_libs = $(VFS_LIBS)
else
vfs_libs =
endif

libgtranslator_la_LIBADD = $(IDL_LIBS) $(GLIB_LIBS) \
	$(XML_LIBS) $(OAF_LIBS) -L/usr/X11R6/lib \
	$(GNOMEUI_LIBS) $(configstufflibs) \
	$(vfs_libs)

libgtranslator_include_dir = $(includedir)/libgtranslator

libgtranslator_include__DATA = \
	gtkled.h \
	gtranslatord.h \
	handle-db.h \
	libgtranslator.h \
	messages.h \
	parse-db.h \
	preferences.h \
	stylistics.h \
	team-database.h \
	team-handle.h \
	translation-database.h

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

EXTRA_DIST = $(idl__DATA) \
	$(libgtranslator_include__DATA) \
	vfs-handle.h
