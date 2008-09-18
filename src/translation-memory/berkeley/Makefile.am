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
	berkeley.c \
	berkeley.h \
	db-base.h \
	db-base.c \
	db-trans.h \
	db-trans.c \
	db-orig.h \
	db-orig.c \
	db-words.h \
	db-words.c \
	db-keys.h \
	db-keys.c

libberkeley_la_CFLAGS = \
	$(GTRANSLATOR_CFLAGS)			\
	$(WARN_CFLAGS)				\
	$(DISABLE_DEPRECATED)			\
	-DCURSOR_DIR=\"$(pkgdatadir)\"		\
	$(AM_CFLAGS)

EXTRA_DIST = $(pixmaps__DATA)

