## the ./data/Makefile.am of gtranslator ..

##
## First set the default subdirs.
##
DEFAULT_SUBDIRS = desktop oaf scripts

##
## Extend it with the gconf-files if it's detected.
##
if GCONF_IS_PRESENT
GCONF_SUBDIRS = gconf
else
GCONF_SUBDIRS =
endif

##
## And extend it also with the mime-files if it's wished.
##
if BIND_MIME
MIME_SUBDIRS = mime
else
MIME_SUBDIRS =
endif

##
## And now the complete subdirs
##
SUBDIRS = ${DEFAULT_SUBDIRS} ${GCONF_SUBDIRS} ${MIME_SUBDIRS}
