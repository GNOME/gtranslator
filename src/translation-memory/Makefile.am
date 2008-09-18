SUBDIRS = berkeley

INCLUDES =                                                      \
        -I$(top_srcdir)                                         \
        -I$(top_builddir)                                       \
        -I$(top_srcdir)/src                                     \
        -I$(top_builddir)/src                                   \
	-I$(headerdir)						\
	-DPIXMAPSDIR=\""$(datadir)"/pixmaps/gtranslator\"       \
        $(GTRANSLATOR_CFLAGS)                                   \
        $(WARN_CFLAGS)                                          \
        $(DISABLE_DEPRECATED_CFLAGS)

noinst_LTLIBRARIES = \
	libtranslationmemory.la

INST_H_FILES = \
	translation-memory.h \
	translation-memory-ui.h

headerdir = $(prefix)/include/gtranslator-@GTR_API_VERSION@/gtranslator

header_DATA = \
	$(INST_H_FILES)	

libtranslationmemory_la_SOURCES = \
	$(INST_H_FILES) \
	translation-memory.c \
	translation-memory-ui.c

libtranslationmemory_la_CFLAGS = \
	$(GTRANSLATOR_CFLAGS)			\
	$(WARN_CFLAGS)				\
	$(DISABLE_DEPRECATED)			\
	-DCURSOR_DIR=\"$(pkgdatadir)\"		\
	$(AM_CFLAGS)

libtranslationmemory_la_LDFLAGS = \
	-export-dynamic

libtranslationmemory_la_LIBADD = \
	berkeley/libberkeley.la

EXTRA_DIST = $(pixmaps__DATA)

