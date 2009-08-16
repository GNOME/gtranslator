## ./data/desktop/Makefile.am of gtranslator

##
## the dirs where all the DATA should go ..
##
Applicationdir = $(datadir)/applications
Applicationicondir = $(datadir)/pixmaps

##
## and the DATA
##
Application_predata = gtranslator.desktop.in
Application_DATA = $(Application_predata:.desktop.in=.desktop)
Applicationicon_DATA = gtranslator.png

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = $(Application_DATA) \
	$(Applicationicon_DATA) \
	$(Application_predata)

CLEANFILES = $(Application_DATA)

UPDATE_DESKTOP = \
	if [ -f $(Applicationdir)/defaults.list ] ; then \
		if which update-desktop-database>/dev/null 2>&1 ; then \
			update-desktop-database $(Applicationdir); \
		fi \
	fi

install-data-hook:
	$(UPDATE_DESKTOP)

uninstall-hook:
	$(UPDATE_DESKTOP) 
-include $(top_srcdir)/git.mk
