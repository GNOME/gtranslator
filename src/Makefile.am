# --------------------------------------------------------
# Welcome to the gtranslator-automake-autoconf Wonderland!
# --------------------------------------------------------

SUBDIRS = . backends pixmaps

INCLUDES = \
	-I$(top_srcdir) \
	-I$(includedir) \
	$(GNOME_INCLUDEDIR) \
	$(GAL_CFLAGS) \
	$(VFS_CFLAGS) \
	$(GCONF_CFLAGS) \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\"

bin_PROGRAMS = gtranslator

%.include: %.config
	sed \
	-e s,"&GNOMELOCALEDIR","$(datadir)/locale",g \
	-e s,"&BACKENDS_DIR","$(libdir)/backends",g \
	-e s,"&BACKENDS_TMDIR","$(libdir)/backends/translation-memories",g \
	-e s,"&BUILD_STRING","$(VERSION) -- $(shell date +%Y-%m-%d\ %H:%M:%S)",g \
	-e s,"&GNOMELOCALEDIR","$(datadir)/locale",g \
	-e s,"&SCHEMESDIR","$(datadir)/gtranslator/colorschemes",g \
	-e s,"&SCRIPTSDIR","$(datadir)/gtranslator/scripts",g \
	-e s,"&WINDOW_ICON","$(datadir)/pixmaps/gtranslator.png",g \
		< $< > $@

$(gtranslator_SOURCES): defines.include

gtranslator_SOURCES = \
	about.c about.h \
	actions.c actions.h \
	backend.c backend.h \
	color-schemes.c color-schemes.h \
	comment.c comment.h \
	dialogs.c dialogs.h \
	dnd.c dnd.h \
	find.c find.h \
	gnome-regex.c gnome-regex.h \
	gtkspell.c gtkspell.h \
	gui.c gui.h \
	header_stuff.c header_stuff.h \
	history.c history.h \
	htmlizer.c htmlizer.h \
	id.c id.h \
	languages.h \
	learn.c learn.h \
	main.c \
	menus.c menus.h \
	message.c message.h \
	messages.h \
	nautilus-string.c nautilus-string.h \
	open-differently.c open-differently.h \
	parse.c	parse.h \
	parse_core.c \
	preferences.c preferences.h \
	prefs.c prefs_init.c prefs.h \
	query.c query.h \
	replace.c replace.h \
	session.c session.h \
	sidebar.c sidebar.h \
	sighandling.c sighandling.h \
	stylistics.c stylistics.h \
	syntax.c syntax.h \
	syntax-elements.h \
	translation-memory.c translation-memory.h \
	undo-core.c undo-core.h \
	undo.c undo.h \
	update.c \
	utf8.c utf8.h \
	utils.c utils.h \
	vfs-handle.c vfs-handle.h \
	views.c views.h

gtranslator_LDADD = \
	$(GNOME_LIBDIR) \
	$(GNOMEUI_LIBS) \
	$(INTLLIBS) \
	$(GCONF_LIBS) \
	$(VFS_LIBS) \
	$(GAL_LIBS)

clean-am:
	-rm -f defines.include

EXTRA_DIST = ChangeLog \
	defines.config
