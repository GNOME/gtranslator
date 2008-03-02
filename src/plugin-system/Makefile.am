INCLUDES =                                                      \
        -I$(top_srcdir)                                         \
        -I$(top_builddir)                                       \
        -I$(top_srcdir)/src                                     \
        -I$(top_builddir)/src                                   \
	-I$(top_srcdir)/src/toolbareditor			\
	-I$(headerdir)						\
	-DPIXMAPSDIR=\""$(datadir)"/pixmaps/gtranslator\"       \
	-DGTR_PLUGINDIR=\""$(libdir)/gtranslator/plugins"\"	\
        $(GTRANSLATOR_CFLAGS)                                   \
        $(WARN_CFLAGS)                                          \
        $(DISABLE_DEPRECATED_CFLAGS)

noinst_LTLIBRARIES = \
	libpluginsystem.la

INST_H_FILES = \
	plugin.h

headerdir = $(prefix)/include/gtranslator-@GTR_API_VERSION@/gtranslator

header_DATA = \
	$(INST_H_FILES)	

libpluginsystem_la_SOURCES = \
	$(INST_H_FILES) \
	module.h \
	module.c \
	plugin-info-priv.h \
	plugin-info.c \
	plugin-info.h \
	plugin.c \
	plugin-manager.c \
	plugin-manager.h \
	plugins-engine.c \
	plugins-engine.h

libpluginsystem_la_CFLAGS = \
	$(GTRANSLATOR_CFLAGS)			\
	$(WARN_CFLAGS)				\
	$(DISABLE_DEPRECATED)			\
	-DCURSOR_DIR=\"$(pkgdatadir)\"		\
	$(AM_CFLAGS)

libpluginsystem_la_LDFLAGS = \
	-export-dynamic

EXTRA_DIST = $(pixmaps__DATA)

