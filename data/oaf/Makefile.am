## ./data/oaf/Makefile.am of gtranslator

oafdir = $(datadir)/oaf

oaf_in_files = Gtranslator_Gtranslatord.oaf.in

oaf_DATA = $(oaf_in_files:.oaf.in=.oaf)
	 
@XML_I18N_MERGE_OAF_RULE@

EXTRA_DIST = $(oaf_DATA) $(oaf_in_files)
