## the ./data/Makefile.am of gtranslator ..

##
## First set the default subdirs.
##
SUBDIRS = desktop oaf

##
## Extend it with the gconf-files if it's detected.
##
if GCONF_IS_PRESENT
SUBDIRS = ${SUBDIRS} gconf
endif

##
## And extend it also with the mime-files if it's wished.
##
if ENABLE_MIME_BIND
SUBDIRS = ${SUBDIRS} mime
endif
