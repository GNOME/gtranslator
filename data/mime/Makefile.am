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
## The xml-i18n tools rule
##
@XML_I18N_MERGE_KEYS_RULE@

EXTRA_DIST = $(Mimeicon_DATA) $(Mimeinfofiles_DATA)
