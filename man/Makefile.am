# the ./man/Makefile.am for gtranslator manual pages

gtranslator_man_dir = $(mandir)/man1

if BUILD_NV
nv_man = nautilus-gtranslator-view.1.gz
else
nv_man =
endif

##
## The manual->gzip'ed manual generation rule.
##
%.1.gz:%.1
	@if [ ! -f $@ -o $< -nt $@ ]; then \
		echo y|gzip -9 < $< > $@; \
	fi

gtranslator_man__DATA = \
	gtranslator.1.gz \
	pozilla.sh.1.gz \
	$(nv_man)

clean-am:
	@-rm -f $(gtranslator_man__DATA)

EXTRA_DIST = \
	gtranslator.1.in \
	nautilus-gtranslator-view.1.in \
	pozilla.sh.1
