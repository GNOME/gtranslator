## Process this file with automake to produce Makefile.in

SUBDIRS = intl po help macros prep-scripts src

Appldir = $(datadir)/gnome/apps/Applications

mimedir = $(datadir)/mime-info

mime_DATA = gtranslator.mime gtranslator.keys

Appl_DATA = gtranslator.desktop

Applicondir = $(datadir)/pixmaps

Applicon_DATA = gtranslator.png

EXTRA_DIST=$(Appl_DATA) $(Applicon_DATA) $(mime_DATA) gtranslator.tem 

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

