##
## gtranslator's docs Makefile.am.
##

gtranslator_helpdir = $(datadir)/gnome/help/gtranslator/C

gtranslator_sgmlfile = gtranslator.sgml

gtranslator_htmlfiles = \
	authors.html \
	commandlinearguments.html \
	features.html \
	index.html \
	thanks.html \
	use-of-gtranslator.html \
	docbook.css

gtranslator_help_DATA=$(gtranslator_htmlfiles) topic.dat

$(gtranslator_htmlfiles):$(gtranslator_sgmlfile)
	@if [ ! -f gtranslator/index.html ]; then cd $(srcdir) && db2html gtranslator.sgml; fi
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
	@-rm -rf gtranslator
	@-rm -f Makefile

install-data-local: index.html

clean-docs:
	@-rm -f $(gtranslator_htmlfiles)
	@-rm -rf gtranslator

EXTRA_DIST = $(gtranslator_sgmlfile) \
	$(gtranslator_htmlfiles) \
	topic.dat
