##
## gtranslator's docs Makefile.am.
##

##
## Your language locale -- C for the English original manual.
##
lc = C

gtranslator_helpdir = $(datadir)/gnome/help/gtranslator/$(lc)

gtranslator_sgmlfile = gtranslator.sgml

gtranslator_omfdir = $(DESTDIR)$(SC_OMFDIR)/gtranslator

gtranslator_omf_DATA = $(gtranslator_omffile)

gtranslator_omffile = gtranslator-$(lc).omf

$(gtranslator_omffile): $(gtranslator_omffile).presetup
	scrollkeeper-preinstall $(gtranslator_helpdir)/$(gtranslator_sgmlfile) \
		$(gtranslator_omffile).presetup $(gtranslator_omffile)

gtranslator_htmlfiles = \
	aa.html \
	authors.html \
	colorschemes.html \
	commandlinearguments.html \
	features.html \
	index.html \
	resources.html \
	thanks.html \
	docbook.css

if SCROLLKEEPER_PRESENT
gtranslator_help_DATA=$(gtranslator_htmlfiles) topic.dat $(gtranslator_sgmlfile)
else
gtranslator_help_DATA=$(gtranslator_htmlfiles) topic.dat
endif

if SCROLLKEEPER_PRESENT
$(gtranslator_htmlfiles):$(gtranslator_sgmlfile) $(gtranslator_omffile)
else
$(gtranslator_htmlfiles):$(gtranslator_sgmlfile)
endif
	-@if [ ! -f gtranslator/index.html ]; then cd $(srcdir) && db2html gtranslator.sgml; fi
	-cd $(srcdir) && cp gtranslator/$@ .
	
disthook:
	$(mkinstalldirs) $(distdir)/gtranslator
	$(mkinstalldirs) $(distdir)/gtranslator/stylesheets-images
	-cp $(srcdir)/gtranslator/*.html $(srcdir)/gtranslator/*.css \
		$(distdir)/gtranslator
	-cp $(srcdir)/gtranslator/stylesheet-images/*.* \
		$(distdir)/gtranslator/stylesheet-images
	-cp $(srcdir)/*.html $(distdir)
	
distclean-generic:
	@-rm -f $(gtranslator_htmlfiles)
	@-rm -rf gtranslator Makefile $(gtranslator_omffile)


if SCROLLKEEPER_PRESENT
install-data-local: index.html
	scrollkeeper-update
else	
install-data-local: index.html
endif

if SCROLLKEEPER_PRESENT
uninstall-local:
	scrollkeeper-update
endif

clean-docs:
	@-rm -f $(gtranslator_htmlfiles)
	@-rm -rf gtranslator

EXTRA_DIST = $(gtranslator_sgmlfile) \
	$(gtranslator_htmlfiles) \
	$(gtranslator_omffile).presetup \
	topic.dat
