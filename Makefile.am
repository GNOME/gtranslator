## Process this file with automake to produce Makefile.in

SUBDIRS = intl po help macros src

Applicationdir = $(datadir)/gnome/apps/Applications

mimeinfofilesdir = $(datadir)/mime-info

mimeinfofiles_DATA = gtranslator.mime gtranslator.keys

Application_DATA = gtranslator.desktop

Applicationicondir = $(datadir)/pixmaps

Applicationicon_DATA = gtranslator.png

EXTRA_DIST=$(Application_DATA) $(Applicationicon_DATA) $(mimeinfofiles_DATA) \
			kabalak_generate_all_the_files.sh INSTALL.CVS

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)@PACKAGE_PIXMAPS_DIR@; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)@PACKAGE_PIXMAPS_DIR@; \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

