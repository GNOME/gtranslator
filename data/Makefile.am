## the ./data/Makefile.am of gtranslator ..

## the dirs where all the DATA should go ..
Applicationdir = $(datadir)/gnome/apps/Applications
mimeinfofilesdir = $(datadir)/mime-info
Applicationicondir = $(datadir)/pixmaps
Mimeicondir = $(Applicationicondir)/mc

## the DATA ...
mimeinfofiles_DATA = gtranslator.mime gtranslator.keys
Application_DATA = gtranslator.desktop
Applicationicon_DATA = gtranslator.png
Mimeicon_DATA = application-x-po.png

## include this nice pieces into a dist .
EXTRA_DIST=$(Application_DATA) $(Applicationicon_DATA) $(mimeinfofiles_DATA) \
                        $(Mimeicon_DATA) gtranslator.keys.in
