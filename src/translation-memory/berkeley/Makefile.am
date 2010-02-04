INCLUDES =                                                      \
        -I$(top_srcdir)                                         \
        -I$(top_builddir)                                       \
        -I$(top_srcdir)/src                                     \
        -I$(top_builddir)/src                                   \
        -I$(top_srcdir)/src/translation-memory                  \
	-I$(headerdir)						\
	-DPIXMAPSDIR=\""$(datadir)"/pixmaps/gtranslator\"       \
        $(GTRANSLATOR_CFLAGS)                                   \
        $(WARN_CFLAGS)                                          \
        $(DISABLE_DEPRECATED_CFLAGS)

noinst_LTLIBRARIES = \
	libberkeley.la

libberkeley_la_SOURCES = \
	gtr-berkeley.c \
	gtr-berkeley.h \
	gtr-db-base.h \
	gtr-db-base.c \
	gtr-db-trans.h \
	gtr-db-trans.c \
	gtr-db-orig.h \
	gtr-db-orig.c \
	gtr-db-words.h \
	gtr-db-words.c \
	gtr-db-keys.h \
	gtr-db-keys.c

libberkeley_la_CFLAGS = \
	$(GTRANSLATOR_CFLAGS)			\
	$(WARN_CFLAGS)				\
	$(DISABLE_DEPRECATED)			\
	-DCURSOR_DIR=\"$(pkgdatadir)\"		\
	$(AM_CFLAGS)

EXTRA_DIST = $(pixmaps__DATA)


-include $(top_srcdir)/git.mk
