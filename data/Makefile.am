## the ./data/Makefile.am of gtranslator ..

SUBDIRS = \
	desktop \
	mime \
	scripts

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gtranslator-@GTR_API_VERSION@.pc

gtranslator-@GTR_API_VERSION@.pc: gtranslator.pc
	cp gtranslator.pc gtranslator-@GTR_API_VERSION@.pc

uidir = $(pkgdatadir)
ui_DATA = \
		gtranslator-ui.xml \
		gtr-toolbar.xml \
		po.lang \
		layout.xml

icondir = $(datadir)/pixmaps/gtranslator
icon_DATA = gtranslator-plugin.png

EXTRA_DIST = \
		$(ui_DATA) \
		$(icon_DATA)\
		gtranslator.pc.in

CLEANFILES =	 			\
		$(pkgconfig_DATA)
