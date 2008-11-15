## the ./data/Makefile.am of gtranslator ..

SUBDIRS = \
	desktop \
	mime \
	pixmaps

schemasdir	 = $(GCONF_SCHEMA_FILE_DIR)
schemas_in_files = gtranslator.schemas.in
schemas_DATA 	 = $(schemas_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gtranslator.pc

uidir = $(pkgdatadir)
ui_DATA = \
		gtranslator-ui.xml \
		gtr-toolbar.xml \
		po.lang \
		layout.xml

install-data-local:
	        if test -z "$(DESTDIR)" ; then \
	                for p in $(schemas_DATA) ; do \
	                        GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(top_builddir)/data/$$p; \
	                done \
	        fi

EXTRA_DIST =			\
	$(schemas_in_files)	\
	$(ui_DATA)		\
	gtranslator.pc.in

CLEANFILES =	 		\
	$(schemas_DATA)		\
	$(pkgconfig_DATA)
