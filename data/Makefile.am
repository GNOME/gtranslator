## the ./data/Makefile.am of gtranslator ..

##
## First set the default subdirs.
##
DEFAULT_SUBDIRS = \
	colorschemes \
	desktop \
	scripts

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
## The nautilus-gtranslator-view subdirectories.
##
if BUILD_NV
NV_DIRS = oaf ui
else
NV_DIRS =
endif

##
## And now the complete subdirs
##
SUBDIRS = \
	$(DEFAULT_SUBDIRS) \
	$(GCONF_SUBDIRS) \
	$(MIME_SUBDIRS) \
	$(NV_DIRS) 
