## the main Makefile.am

if BUILD_NV
NV_DIR = nautilus-view
else
NV_DIR =
endif

SUBDIRS = data help intl macros man po src $(NV_DIR)

bin_SCRIPTS = pozilla.sh

EXTRA_DIST= \
		$(bin_SCRIPTS) \
		kabalak_generate_all_the_files.sh \
		DEPENDS \
		HACKING \
		README.GConf \
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

cvspack: clean
	@-rm -f gtranslator-CVS_$(shell date +%Y-%m-%d).tar.gz
	@echo Creating CVS package of gtranslator...
	@./autogen.sh --help
	@$(TAR) -C .. -chozf gtranslator-CVS_$(shell date +%Y-%m-%d).tar.gz gtranslator

pre-release: cvspack
	@-./pozilla.sh
