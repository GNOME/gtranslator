## the main Makefile.am

if BUILD_NV
NV_DIR = nautilus-view
else
NV_DIR =
endif

SUBDIRS = data intl macros src $(NV_DIR) po help man

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

cvspack:
	@-rm -f gtranslator-CVS_$(shell date +%Y-%m-%d).tar.gz
	@echo Creating CVS distpackage of gtranslator...
	@cp -pR po __po
	@cp configure.in __configure.in
	@sed -e "s,AM_INIT_AUTOMAKE.*,AM_INIT_AUTOMAKE\(gtranslator\, CVS_$(shell date +%Y-%m-%d)\),g" < configure.in \
		> configure.in.new && mv configure.in.new configure.in
	@./autogen.sh --help
	@make dist
	@mv __po po
	@mv __configure.in configure.in

pre-release: cvspack
	@-./pozilla.sh
