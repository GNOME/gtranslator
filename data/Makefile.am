## the ./data/Makefile.am of gtranslator ..

SUBDIRS = \
	desktop \
	pixmaps

@INTLTOOL_XML_RULE@
appdatadir = $(datadir)/appdata
appdata_DATA = $(appdata_in_files:.xml.in=.xml)
appdata_in_files = gtranslator.appdata.xml.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gtranslator.pc

uidir = $(pkgdatadir)/ui
ui_DATA =			\
	gtr-toolbar.xml		\
	gtr-plural-forms.ini	\
	po.lang			\
	layout.xml

gsettings_SCHEMAS = org.gnome.gtranslator.gschema.xml

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@


EXTRA_DIST =					\
	$(ui_DATA)				\
	org.gnome.gtranslator.gschema.xml.in	\
	gtranslator.pc.in

CLEANFILES =			\
	gtranslator.appdata.xml \
	$(appdata_DATA)		\
	$(gsettings_SCHEMAS)	\
	$(pkgconfig_DATA)

DISTCLEANFILES =		\
	$(gsettings_SCHEMAS)	\
	$(pkgconfig_DATA)

-include $(top_srcdir)/git.mk
