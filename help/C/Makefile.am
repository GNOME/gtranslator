##
## gtranslator's docs Makefile.am.
##

SUBDIRS = figures

##
## Your language locale -- C for the English original manual.
##
lc = C

gtranslator_helpdir = $(datadir)/gnome/help/gtranslator/$(lc)

gtranslator_sgmlfile = gtranslator.sgml

gtranslator_omfdir = $(SC_OMFDIR)/gtranslator

if SCROLLKEEPER_PRESENT
gtranslator_omf_DATA = $(gtranslator_omffile)

gtranslator_omffile = gtranslator-$(lc).omf

$(gtranslator_omffile): $(gtranslator_omffile).presetup
	scrollkeeper-preinstall $(gtranslator_helpdir)/$(gtranslator_sgmlfile) \
		$(gtranslator_omffile).presetup $(gtranslator_omffile)
endif		

gtranslator_htmlfiles = \
	authors.html \
	bugs.html \
	glossary.html \
	index.html \
	license.html \
	prefs.html \
	usage.html \
	$(css_files)

if SCROLLKEEPER_PRESENT
gtranslator_help_DATA=$(gtranslator_htmlfiles) topic.dat $(gtranslator_sgmlfile)
else
gtranslator_help_DATA=$(gtranslator_htmlfiles) topic.dat
endif

if JW_PRESENT
build_command=jw -c /etc/sgml/catalog $(gtranslator_sgmlfile) -o gtranslator
css_files=
else
build_command=db2html $(gtranslator_sgmlfile)
css_files=docbook.css
endif

if SCROLLKEEPER_PRESENT
$(gtranslator_htmlfiles):$(gtranslator_sgmlfile) $(gtranslator_omffile)
else
$(gtranslator_htmlfiles):$(gtranslator_sgmlfile)
endif
	-@if [ ! -f gtranslator/index.html ]; then \
		cd $(srcdir) && $(build_command) ; \
	fi
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
	scrollkeeper-update -p $(SC_STATEDIR)
else	
install-data-local: index.html
endif

if SCROLLKEEPER_PRESENT
uninstall-local:
	scrollkeeper-update -p $(SC_STATEDIR)
endif

clean-docs:
	@-rm -f $(gtranslator_htmlfiles)
	@-rm -rf gtranslator

EXTRA_DIST = $(gtranslator_sgmlfile) \
	$(gtranslator_htmlfiles) \
	$(gtranslator_omffile).presetup \
	topic.dat
