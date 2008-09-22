INCLUDES =                                                      \
        -I$(top_srcdir)                                         \
        -I$(top_builddir)                                       \
        -I$(top_srcdir)/src                                     \
        -I$(top_builddir)/src                                   \
	-I$(top_srcdir)/src/plugin-system			\
	-DPKGDATADIR=\""$(pkgdatadir)"\"			\
	-DPIXMAPSDIR=\""$(datadir)"/pixmaps/gtranslator\"	\
        $(GTRANSLATOR_CFLAGS)                                   \
        $(WARN_CFLAGS)                                          \
        $(DISABLE_DEPRECATED_CFLAGS)

noinst_LTLIBRARIES = \
	libdialogs.la

libdialogs_la_SOURCES = \
	close-confirmation-dialog.c \
	close-confirmation-dialog.h \
	file-dialogs.c \
	file-dialogs.h \
	preferences-dialog.c \
	preferences-dialog.h \
	search-dialog.c \
	search-dialog.h \
	header-dialog.c \
	header-dialog.h \
	comment-dialog.c \
	comment-dialog.h \
	profile-dialog.c \
	profile-dialog.h \
	assistant.c \
	assistant.h

gladedir = $(pkgdatadir)

glade_DATA =  \
		preferences-dialog.glade \
		search-dialog.glade \
		header-dialog.glade \
		comment-dialog.glade \
		profile-dialog.glade

EXTRA_DIST = \
		$(glade_DATA)
