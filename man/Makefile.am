# the ./man/Makefile.am for gtranslator manual pages

gtranslator_man_dir = $(mandir)/man1

gtranslator_man=gtranslator.1.gz

pozilla_man=pozilla.sh.1.gz

nv_man=nautilus-gtranslator-view.1.gz

$(gtranslator_man):gtranslator.1
	@if [ ! -f $(gtranslator_man) ]; then \
		echo y|gzip -9 < gtranslator.1 > $(gtranslator_man); \
	fi
	

$(pozilla_man):$(srcdir)/pozilla.sh.1
	@if [ ! -f $(pozilla_man) ]; then \
		echo y|gzip -9 < $(srcdir)/pozilla.sh.1 > $(pozilla_man); \
	fi

$(nv_man):nautilus-gtranslator-view.1
	@if [ ! -f $(nv_man) ]; then \
		echo y|gzip -9 < nautilus-gtranslator-view.1 > $(nv_man); \
	fi

gtranslator_man__DATA = \
	$(gtranslator_man) \
	$(nv_man) \
	$(pozilla_man)

clean-am:
	@-rm -f $(gtranslator_man__DATA)

EXTRA_DIST = \
	gtranslator.1.in \
	nautilus-gtranslator-view.1.in \
	pozilla.sh.1
