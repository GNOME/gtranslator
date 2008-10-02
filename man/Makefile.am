# the ./man/Makefile.am for gtranslator manual pages

gtranslator_man_dir = $(mandir)/man1

gtranslator.1.gz: gtranslator.1
	gzip -9 < gtranslator.1 > gtranslator.1.gz

gtranslator_man__DATA = \
	gtranslator.1.gz 

CLEANFILES = $(gtranslator_man__DATA)

EXTRA_DIST = \
	gtranslator.1.in 
