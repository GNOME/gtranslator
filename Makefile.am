## the main Makefile.am

SUBDIRS = data intl po help macros man libgtranslator message-db.d src

##
## The team database get's installed into gtranslator's main datadir.
##

TeamdatabaseDir = $(datadir)/gtranslator

Teamdatabase_DATA = teams.xml

EXTRA_DIST=\
		INSTALL.CVS \
		kabalak_generate_all_the_files.sh \
		DEPENDS \
		HACKING \
		po/update.pl \
		po/desk.pl \
		po/README.tools \
		$(Teamdatabase_DATA)

distpacks: distdir
	@-rm -f $(distdir).tar.bz2
	@-rm -f $(distdir).tar.gz
	@echo Creating tar gzip/bzip2 packages of $(distdir)...
	@$(TAR) choIf $(distdir).tar.bz2 $(distdir)
	@$(TAR) chozf $(distdir).tar.gz $(distdir)
	@echo Cleaning up...
	@-rm -rf $(distdir)
