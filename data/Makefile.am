## the ./data/Makefile.am of gtranslator ..

## the dirs where all the DATA should go ..
Applicationdir = $(datadir)/gnome/apps/Development
if BIND_MIME
mimeinfofilesdir = $(datadir)/mime-info
else
mimeinfofilesdir =
endif
Applicationicondir = $(datadir)/pixmaps
Mimeicondir = $(Applicationicondir)/mc
OAFdir = $(datadir)/oaf

## the DATA ...
if BIND_MIME
mimeinfofiles_DATA = gtranslator.mime gtranslator.keys 
else
mimeinfofiles_DATA = 
endif
Application_DATA = gtranslator.desktop
Applicationicon_DATA = gtranslator.png
Mimeicon_DATA = application-x-po.png
OAF_DATA = gtranslatord.oafinfo

## include these nice pieces into a dist .
EXTRA_DIST=$(Application_DATA) $(Applicationicon_DATA) \
                        $(Mimeicon_DATA) gtranslator.keys.in \
			gtranslator.mime gtranslator.keys
