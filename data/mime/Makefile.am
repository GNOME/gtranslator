## ./data/mime/Makefile.am of gtranslator

##
## The mime dirs
## 
icondir = $(datadir)/pixmaps/mc
mimedir = $(datadir)/mime-info
keysdir = $(datadir)/mime-info

##
## And the mime-files
##
icon_DATA = application-x-po.png application-x-gmo.png
keys_predata = gtranslator.keys.in
keys_DATA = gtranslator.keys
mime_DATA = gtranslator.mime
	
##
## HACK -- overread this please..
##
all-am:
	@mv $(srcdir)/gtranslator.keys.in gtranslator.keys.in.bak
	@mv gtranslator.keys gtranslator.keys.in
	xml-i18n-merge -k $(top_srcdir)/po gtranslator.keys.in gtranslator.keys
	@mv gtranslator.keys.in.bak $(srcdir)/gtranslator.keys.in

EXTRA_DIST = $(icon_DATA) $(mime_DATA) $(keys_predata)

