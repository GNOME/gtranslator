## the main Makefile.am

SUBDIRS = data debian help intl macros man po src

bin_SCRIPTS = pozilla.sh

EXTRA_DIST= \
		$(bin_SCRIPTS) \
		DEPENDS \
		HACKING \
		kabalak_generate_all_the_files.sh \
		gtranslator.spec.in \
		intltool-extract.in \
		intltool-merge.in \
		intltool-update.in

distpacks: distdir
	@-rm -f $(distdir).tar.bz2
	@-rm -f $(distdir).tar.gz
	@echo Creating tar gzip/bzip2 packages of $(distdir)...
	@$(TAR) -choIf $(distdir).tar.bz2 $(distdir)
	@$(TAR) -chozf $(distdir).tar.gz $(distdir)
	@echo Cleaning up...
	@-rm -rf $(distdir)
