## the main Makefile.am

SUBDIRS = data intl po help macros man libgtranslator message-db.d src

EXTRA_DIST=INSTALL.CVS kabalak_generate_all_the_files.sh \
		po/update.sh po/update.pl po/desk.pl \
		po/README.tools

distpacks: distdir
	-chmod -R a+r $(distdir)
	[ -f $(distdir).tar.gz ] && rm -f $(distdir).tar.gz
	[ -f $(distdir).tar.bz2 ] && rm -f $(distdir).tar.bz2
	$(TAR) choIf $(distdir).tar.bz2 $(distdir)
	$(TAR) chozf $(distdir).tar.gz $(distdir)
	-rm -rf $(distdir)
