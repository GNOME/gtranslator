##
## The nautilus-gtranslator-view component.
##

sourcedir=./../src

bin_PROGRAMS = nautilus-gtranslator-view

INCLUDES = \
	-DG_LOG_DOMAIN=\""Nautilus-Gtranslator-View"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DICON_DIR=\""$(datadir)/pixmaps/nautilus"\" \
	-I$(sourcedir) \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	$(GNOMEUI_CFLAGS) \
	$(GAL_CFLAGS) \
	$(NAUTILUS_CFLAGS)
	

nautilus_gtranslator_view_SOURCES = \
	main.c \
	nautilus-gtranslator-view.c

nautilus_gtranslator_view_DEPENDENCIES = \
	$(sourcedir)/about.o \
	$(sourcedir)/dialogs.o \
	$(sourcedir)/dnd.o \
	$(sourcedir)/find.o \
	$(sourcedir)/gnome-regex.o \
	$(sourcedir)/gtkspell.o \
	$(sourcedir)/gui.o \
	$(sourcedir)/header_stuff.o \
	$(sourcedir)/history.o \
	$(sourcedir)/open-differently.o \
	$(sourcedir)/parse.o \
	$(sourcedir)/parse_core.o \
	$(sourcedir)/prefs.o \
	$(sourcedir)/prefs_init.o \
	$(sourcedir)/preferences.o \
	$(sourcedir)/sidebar.o \
	$(sourcedir)/stylistics.o \
	$(sourcedir)/syntax.o \
	$(sourcedir)/update.o \
	$(sourcedir)/vfs-handle.o

nautilus_gtranslator_view_LDADD = \
	$(GNOMEUI_LIBS) \
	$(VFS_LIBS) \
	$(GAL_LIBS) \
	$(NAUTILUS_LIBS) \
	$(nautilus_gtranslator_view_DEPENDENCIES)

EXTRA_DIST = ChangeLog
