# --------------------------------------------------------
# Welcome to the gtranslator-automake-autoconf Wonderland!
# --------------------------------------------------------

SUBDIRS = backends pixmaps

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	$(GAL_CFLAGS) \
	$(VFS_CFLAGS) \
	$(GCONF_CFLAGS) \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\"

bin_PROGRAMS = gtranslator

GTRANSLATOR_BUILD_VERSIONS = "$(shell glib-config --version) / $(shell gtk-config --version) / $(shell gnome-config --modversion gnome|sed -e s,gnome-libs-,,)"

%.include: %.config
	sed \
	-e s,"&GNOMELOCALEDIR","$(datadir)/locale",g \
	-e s,"&BACKENDS_DIR","$(libdir)/gtranslator/backends",g \
	-e s,"&BUILD_STRING","$(VERSION) -- $(shell date +%Y-%m-%d\ %H:%M:%S)",g \
	-e s,"&BUILD_VERSIONS",$(GTRANSLATOR_BUILD_VERSIONS),g \
	-e s,"&ETSPECS_DIR","$(datadir)/gtranslator/etspecs",g \
	-e s,"&GNOMELOCALEDIR","$(datadir)/locale",g \
	-e s,"&SCHEMESDIR","$(datadir)/gtranslator/colorschemes",g \
	-e s,"&SCRIPTSDIR","$(datadir)/gtranslator/scripts",g \
	-e s,"&WINDOW_ICON","$(datadir)/pixmaps/gtranslator.png",g \
		< $< > $@

translation_memory_sources = \
	translation-memory.c translation-memory.h

$(gtranslator_SOURCES): defines.include

gtranslator_SOURCES = \
	about.c about.h \
	actions.c actions.h \
	backend.c backend.h \
	bookmark.c bookmark.h \
	color-schemes.c color-schemes.h \
	comment.c comment.h \
	convert.c convert.h \
	dialogs.c dialogs.h \
	dnd.c dnd.h \
	find.c find.h \
	formats.c formats.h \
	gtkspell.c gtkspell.h \
	gui.c gui.h \
	header_stuff.c header_stuff.h \
	history.c history.h \
	htmlizer.c htmlizer.h \
	languages.c languages.h \
	learn.c learn.h \
	main.c \
	menus.c menus.h \
	message.c message.h \
	messages.h \
	messages-table.c messages-table.h \
	nautilus-string.c nautilus-string.h \
	open.c open.h \
	parse.c	parse.h \
	preferences.c preferences.h \
	prefs.c prefs_init.c prefs.h \
	prefs-dialog.c prefs-dialog.h \
	query.c query.h \
	replace.c replace.h \
	runtime-config.c runtime-config.h \
	save.c save.h \
	session.c session.h \
	sighandling.c sighandling.h \
	stylistics.c stylistics.h \
	syntax.c syntax.h \
	syntax-elements.h \
	$(translation_memory_sources) \
	translator.c translator.h \
	undo.c undo.h \
	update.c \
	utf8.c utf8.h \
	utils.c utils.h \
	utils_gui.c utils_gui.h \
	vfs-handle.c vfs-handle.h

gtranslator_LDADD = \
	$(INTLLIBS) \
	$(GCONF_LIBS) \
	$(VFS_LIBS) \
	$(GAL_LIBS)

CLEANFILES = defines.include

EXTRA_DIST = ChangeLog \
	defines.config
