## the main Makefile.am

SUBDIRS = macros intl po data src help man

EXTRA_DIST= \
		kabalak_generate_all_the_files.sh \
		DEPENDS \
		HACKING \
		po/update.sh \
		xml-i18n-extract.in \
		xml-i18n-merge.in \
		xml-i18n-update.in

distpacks: distdir
	@-rm -f $(distdir).tar.bz2
	@-rm -f $(distdir).tar.gz
	@echo Creating tar gzip/bzip2 packages of $(distdir)...
	@$(TAR) -choIf $(distdir).tar.bz2 $(distdir)
	@$(TAR) -chozf $(distdir).tar.gz $(distdir)
	@echo Cleaning up...
	@-rm -rf $(distdir)

cvspack: distclean
	@-rm -f gtranslator-CVS_$(shell date +%Y-%m-%d).tar.gz
	@echo Creating CVS package of gtranslator...
	@$(TAR) -C .. -chozf gtranslator-CVS_$(shell date +%Y-%m-%d).tar.gz gtranslator

pre-release: cvspack
	@-./pozilla.sh
