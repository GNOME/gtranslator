## ./data/mime/Makefile.am of gtranslator

##
## The mime dirs
## 
Mimeicondir = $(datadir)/pixmaps/mc
Mimeinfofilesdir = $(datadir)/mime-info

##
## And the mime-files
##
Mimeicon_DATA = application-x-po.png application-x-gmo.png
Mimeinfofiles_predata = gtranslator.keys.in
Mimeinfofiles_DATA = gtranslator.mime $(Mimeinfofiles_predata:.keys.in=.keys)
	
##
## HACK -- overread this please..
##
all-am:
	@mv $(Mimeinfofiles_predata) $(Mimeinfofiles_predata).backmeup
	@mv $(Mimeinfofiles_predata:.keys.in=.keys) $(Mimeinfofiles_predata)
	xml-i18n-merge -k ../../po $(Mimeinfofiles_predata) $(Mimeinfofiles_predata:.keys.in=.keys)
	@mv $(Mimeinfofiles_predata).backmeup $(Mimeinfofiles_predata)

EXTRA_DIST = $(Mimeicon_DATA) $(Mimeinfofiles_DATA)
