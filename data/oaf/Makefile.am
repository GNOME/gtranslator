##
## The OAF file for the nautilus-gtranslator-view
##

OAFdir = $(datadir)/oaf

##
## The OAF data and the XML I18N rules according to this.
## 
OAF_predata = \
	Nautilus_Gtranslator_View.oaf.in
	
OAF_DATA = $(OAF_predata:.oaf.in=.oaf)

@XML_I18N_MERGE_OAF_RULE@

EXTRA_DIST = $(OAF_predata) $(OAF_DATA)
