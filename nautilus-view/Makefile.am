##
## The nautilus-gtranslator-view component.
##

srcbuild=$(top_builddir)/src

if BUILD_NV
bin_PROGRAMS = nautilus-gtranslator-view
endif

INCLUDES = \
	-DG_LOG_DOMAIN=\""Nautilus-Gtranslator-View"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DICON_DIR=\""$(datadir)/pixmaps/nautilus"\" \
	-I$(top_srcdir)/src \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	$(GNOMEUI_CFLAGS) \
	$(GAL_CFLAGS) \
	$(NAUTILUS_CFLAGS)
	

nautilus_gtranslator_view_SOURCES = \
	main.c \
	nautilus-gtranslator-view.c \
	nautilus-gtranslator-view.h

nautilus_gtranslator_view_DEPENDENCIES = \
	$(srcbuild)/about.o \
	$(srcbuild)/actions.o \
	$(srcbuild)/backend.o \
	$(srcbuild)/color-schemes.o \
	$(srcbuild)/convert.o \
	$(srcbuild)/comment.o \
	$(srcbuild)/dialogs.o \
	$(srcbuild)/dnd.o \
	$(srcbuild)/find.o \
	$(srcbuild)/gnome-regex.o \
	$(srcbuild)/gtkspell.o \
	$(srcbuild)/gui.o \
	$(srcbuild)/header_stuff.o \
	$(srcbuild)/history.o \
	$(srcbuild)/learn.o \
	$(srcbuild)/menus.o \
	$(srcbuild)/message.o \
	$(srcbuild)/messages-table.o \
	$(srcbuild)/nautilus-string.o \
	$(srcbuild)/open-differently.o \
	$(srcbuild)/parse.o \
	$(srcbuild)/parse_core.o \
	$(srcbuild)/prefs.o \
	$(srcbuild)/prefs_init.o \
	$(srcbuild)/preferences.o \
	$(srcbuild)/query.o \
	$(srcbuild)/replace.o \
	$(srcbuild)/save-differently.o \
	$(srcbuild)/sidebar.o \
	$(srcbuild)/stylistics.o \
	$(srcbuild)/syntax.o \
	$(srcbuild)/undo.o \
	$(srcbuild)/update.o \
	$(srcbuild)/utf8.o \
	$(srcbuild)/utils.o \
	$(srcbuild)/utils_gui.o \
	$(srcbuild)/vfs-handle.o \
	$(srcbuild)/views.o

nautilus_gtranslator_view_LDADD = \
	$(GNOMEUI_LIBS) \
	$(VFS_LIBS) \
	$(GAL_LIBS) \
	$(NAUTILUS_LIBS) \
	$(nautilus_gtranslator_view_DEPENDENCIES)

EXTRA_DIST = ChangeLog
