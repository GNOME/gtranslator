## ./date/gconf/Makefile.am for gtranslator

##
## The place of the schemas.
##
schemas_dir = $(sysconfdir)/gconf/schemas

##
## And our schema.
##
schemas__DATA = gtranslator.schemas

install-data-local:
	gconftool --shutdown
	GCONF_CONFIG_SOURCE="xml::${sysconfdir}/gconf/gconf.xml.defaults" \
	gconftool --makefile-install-rule \
	$(top_builddir)/data/gconf/$(schemas__DATA)

EXTRA_DIST = $(schemas__DATA)
