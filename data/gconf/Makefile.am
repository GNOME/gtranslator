## ./date/gconf/Makefile.am for gtranslator

##
## The place of the schemas.
##
schemas_dir = $(sysconfdir)/gconf/schemas

##
## And our schema.
##
schemas__DATA = gtranslator.schemas
confsources = "xml::${sysconfdir}/gconf/gconf.xml.defaults"


install-data-local:
	-gconftool --shutdown
	GCONF_CONFIG_SOURCE=$(confsources) gconftool --makefile-install-rule \
		$(top_srcdir)/data/gconf/$(schemas__DATA)

EXTRA_DIST = $(schemas__DATA)
