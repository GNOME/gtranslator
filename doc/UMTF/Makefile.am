# --------------------------------------------------------------------
# A small Makefile  for the UMTF spec packaging and inclusion in the
#  gtranslator dist packs.
# --------------------------------------------------------------------

EXTRA_DIST = $(UMTF_SPEC_FILES)

dtddir = $(datadir)/gtranslator/dtd/

dtd_DATA = umtf.dtd

UMTF_VERSION = 1.0

UMTF_SPEC_FILES = \
	COMMENTS \
	ChangeLog \
	README.UMTF \
	example.xml \
	$(dtd_DATA)

specpack: $(UMTF_SPEC_FILES)
	@-echo Creating a small specifications pack of UMTF $(UMTF_VERSION)...
	@-mkdir UMTF-$(UMTF_VERSION)
	@-cp -f $(UMTF_SPEC_FILES) UMTF-$(UMTF_VERSION)
	@-tar -czf UMTF-$(UMTF_VERSION).tar.gz UMTF-$(UMTF_VERSION)
	@-rm -rf UMTF-$(UMTF_VERSION)

clean:
	rm -rf UMTF-$(UMTF_VERSION)

-include $(top_srcdir)/git.mk
