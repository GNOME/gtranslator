##
## gtranslator's docs Makefile.am.
##

gtranslator_helpdir = $(datadir)/gnome/help/gtranslator/ja

gtranslator_sgmlfile = gtranslator.sgml

gtranslator_htmlfiles = \
	authors.html \
	commandlinearguments.html \
	features.html \
	index.html \
	thanks.html \
	use-of-gtranslator.html

gtranslator_help_DATA=$(gtranslator_htmlfiles) topic.dat

if JW_PRESENT
build_command=jw -c /etc/sgml/catalog $(gtranslator_sgmlfile) -o gtranslator
else
build_command=db2html $(gtranslator_sgmlfile)
endif
	
$(gtranslator_htmlfiles):$(gtranslator_sgmlfile)
	-@if [ ! -f gtranslator/index.html ]; then cd $(srcdir) && $(build_command); fi
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
	@-rm -f $(gtranslator_htmlfiles) Makefile
	@-rm -rf gtranslator

install-data-local: index.html

EXTRA_DIST = $(gtranslator_sgmlfile) \
	$(gtranslator_htmlfiles) \
	topic.dat
