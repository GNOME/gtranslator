## the main Makefile.am

SUBDIRS = data debian doc help man src po

EXTRA_DIST= \
		$(bin_SCRIPTS) \
		DEPENDS \
		HACKING \
		kabalak_generate_all_the_files.sh \
		gtranslator.spec.in \
		gtranslator.spec \
		intltool-extract.in \
		intltool-merge.in \
		intltool-update.in

cvspack: distdir
	@-rm -f $(distdir).tar.gz
	@echo Creating CVS snapshot "$(shell date +%Y-%m-%d)" package...
	@$(TAR) -chozf $(distdir).tar.gz $(distdir)
	@mv -f $(distdir).tar.gz "$(distdir).$(shell date +%Y%m%d).tar.gz"
	@-rm -rf $(distdir)

cvsdeb: debian/rules
	@echo Creating debian package...
	@-rm -f build-stamp configure-stamp install-stamp
	@./debian/rules configure
	@./debian/rules binary
