## ./data/mime/Makefile.am of gtranslator

if BIND_MIME
Mimeicondir = $(PIXMAP_DIR)
Mimeicon_DATA = application-x-po.png application-x-gmo.png

Mimekeysdir = $(datadir)/mime-info
Mimekeys_predata = gtranslator.keys.in
Mimekeys_DATA = gtranslator.mime $(Mimekeys_predata:.keys.in=.keys)
	
@XML_I18N_MERGE_KEYS_RULE@
endif

EXTRA_DIST = application-x-po.png application-x-gmo.png gtranslator.mime
