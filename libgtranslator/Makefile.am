# the Makefile.am for libgtranslator

bin_PROGRAMS = gtranslatord

noinst_LIBRARIES = libgtranslator.a

if GCONF_IS_PRESENT
configstuffcflags = $(GCONF_CFLAGS) $(GNOME_CFLAGS) -DGCONF_IS_PRESENT=7
else
configstuffcflags = $(GNOME_CFLAGS)
endif

if USE_VFS_STUFF
vfs_cflags = $(VFS_CFLAGS) -DUSE_VFS_STUFF=7
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
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"

libgtranslator_a_SOURCES = \
	$(GTRANSLATORD_SOURCES) \
	$(vfs_source)


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

libgtranslatordata = \
	gtranslatord.h \
	libgtranslator.h \
	messages.h

gtranslatord_SOURCES = main.c

gtranslatord_LDADD = libgtranslator.a $(IDL_LIBS) \
	-L/usr/X11R6/lib $(OAF_LIBS)  $(XML_LIBS) \
	$(GCONF_LIBS)

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
	$(libgtranslatordata) \
	vfs-handle.h \
	vfs-handle.c
