## ./data/desktop/Makefile.am of gtranslator

##
## the dirs where all the DATA should go ..
##
Applicationdir = $(datadir)/gnome/apps/Development
Applicationicondir = $(datadir)/pixmaps

##
## and the DATA
##
Application_predata = gtranslator.desktop.in
Application_DATA = $(Application_predata:.desktop.in=.desktop)
Applicationicon_DATA = gtranslator.png

##
## xml-i18n tools rule
##
@XML_I18N_MERGE_DESKTOP_RULE@

EXTRA_DIST = $(Application_DATA) \
	$(Applicationicon_DATA) \
	$(Application_predata)
