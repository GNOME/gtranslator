##
## gtranslator's docs Makefile.am.
##

##
## Your language locale -- should work with "ja" here already...
##
lc = ja

gtranslator_helpdir = $(datadir)/gnome/help/gtranslator/$(lc)

gtranslator_xmlfile = gtranslator.xml

gtranslator_omfdir = $(SC_OMFDIR)/gtranslator

gtranslator_omf_DATA = $(gtranslator_omffile)

gtranslator_omffile = gtranslator-$(lc).omf

$(gtranslator_omffile): $(gtranslator_omffile).presetup
	scrollkeeper-preinstall $(gtranslator_helpdir)/$(gtranslator_xmlfile) \
		$(gtranslator_omffile).presetup $(gtranslator_omffile)

gtranslator_help_DATA=$(gtranslator_xmlfile)

install-data-local: 
	scrollkeeper-update -p $(SC_STATEDIR)

uninstall-local:
	scrollkeeper-update -p $(SC_STATEDIR)

EXTRA_DIST = $(gtranslator_xmlfile) \
	$(gtranslator_omffile).presetup
