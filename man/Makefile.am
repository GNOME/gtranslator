# the ./man/Makefile.am for the gtranslator manual page

gtranslator_man_dir = $(mandir)/man1

gtranslator_man__DATA = gtranslator.1.gz

$(gtranslator_man__DATA): gtranslator.1
	@if [ ! -f $(gtranslator_man__DATA) ]; then \
		echo y|gzip -9 gtranslator.1;\
	fi

clean-generic:
	@-rm -f $(gtranslator_man__DATA)

EXTRA_DIST = gtranslator.1.in
