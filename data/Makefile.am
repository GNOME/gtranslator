## the ./data/Makefile.am of gtranslator ..

## the dirs where all the DATA should go ..
Applicationdir = $(datadir)/gnome/apps/Applications
#ifdef BIND_MIME
mimeinfofilesdir = $(datadir)/mime-info
#else
mimeinfofilesdir =
#endif
Applicationicondir = $(datadir)/pixmaps
Mimeicondir = $(Applicationicondir)/mc

## the DATA ...
#ifdef BIND_MIME
mimeinfofiles_DATA = gtranslator.mime gtranslator.keys
#else
mimeinfofiles_DATA = 
#endif
Application_DATA = gtranslator.desktop
Applicationicon_DATA = gtranslator.png
Mimeicon_DATA = application-x-po.png

## include thess nice pieces into a dist .
EXTRA_DIST=$(Application_DATA) $(Applicationicon_DATA) $(mimeinfofiles_DATA) \
                        $(Mimeicon_DATA) gtranslator.keys.in gtranslator.mime \
			gtranslator.keys
