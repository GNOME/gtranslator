# the ./man/Makefile.am for gtranslator manual pages

gtranslator_man_dir = $(mandir)/man1

gtr_man=gtranslator.1.gz

poz_man=pozilla.sh.1.gz

$(gtr_man):gtranslator.1
	@if [ ! -f $(gtr_man) ]; then \
		echo y|gzip -9 < gtranslator.1 > $(gtr_man); \
	fi
	

$(poz_man):pozilla.sh.1
	@if [ ! -f $(poz_man) ]; then \
		echo y|gzip -9 < pozilla.sh.1 > $(poz_man); \
	fi

gtranslator_man__DATA = $(gtr_man) $(poz_man)

clean-generic:
	@-rm -f $(gtranslator_man__DATA)

EXTRA_DIST = \
	gtranslator.1.in \
	pozilla.sh.1
