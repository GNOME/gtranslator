## the main Makefile.am

SUBDIRS = data intl po help macros man libgtranslator message-db.d src

EXTRA_DIST=INSTALL.CVS kabalak_generate_all_the_files.sh \
		HACKING \
		po/update.pl \
		po/desk.pl \
		po/README.tools

distpacks: distdir
	@-rm -f $(distdir).tar.bz2
	@-rm -f $(distdir).tar.gz
	@echo Creating tar gzip/bzip2 packages of $(distdir)...
	@$(TAR) choIf $(distdir).tar.bz2 $(distdir)
	@$(TAR) chozf $(distdir).tar.gz $(distdir)
	@echo Cleaning up...
	@-rm -rf $(distdir)
