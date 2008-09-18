## ./data/mime/Makefile.am of gtranslator

##
## The mime dirs
## 
icondir = $(datadir)/pixmaps
mimedir = $(datadir)/mime-info
keysdir = $(datadir)/mime-info

##
## And the mime-files
##
icon_DATA = gnome-application-x-gmo.png gnome-application-x-po.png
keys_predata = gtranslator.keys_template.in
keys_DATA = gtranslator.keys
mime_DATA = gtranslator.mime

gtranslator.keys: gtranslator.keys_template
	$(INTLTOOL_MERGE) -k $(top_srcdir)/po gtranslator.keys_template \
		gtranslator.keys

##
## HACK -- overread this please..
##
all-am: gtranslator.keys

EXTRA_DIST = $(icon_DATA) $(mime_DATA) $(keys_predata)

CLEANFILES = $(keys_DATA)
