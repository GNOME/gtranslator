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
        $(DISABLE_DEPRECATED)

noinst_LTLIBRARIES = \
	libdialogs.la

libdialogs_la_SOURCES = \
	gtr-close-confirmation-dialog.c \
	gtr-close-confirmation-dialog.h \
	gtr-file-dialogs.c \
	gtr-file-dialogs.h \
	gtr-preferences-dialog.c \
	gtr-preferences-dialog.h \
	gtr-search-dialog.c \
	gtr-search-dialog.h \
	gtr-header-dialog.c \
	gtr-header-dialog.h \
	gtr-profile-dialog.c \
	gtr-profile-dialog.h \
	gtr-assistant.c \
	gtr-assistant.h \
	gtr-jump-dialog.c \
	gtr-jump-dialog.h

uidir = $(pkgdatadir)/ui
ui_DATA =  \
		gtr-preferences-dialog.ui \
		gtr-search-dialog.ui \
		gtr-header-dialog.ui \
		gtr-profile-dialog.ui \
		gtr-jump-dialog.ui

EXTRA_DIST = \
		$(ui_DATA)

-include $(top_srcdir)/git.mk
