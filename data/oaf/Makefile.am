##
## The OAF file for the nautilus-gtranslator-view
##

OAF_predata = Nautilus_Gtranslator_View.oaf.in

if BUILD_NV
OAFdir = $(datadir)/oaf
OAF_DATA = $(OAF_predata:.oaf.in=.oaf)
@XML_I18N_MERGE_OAF_RULE@
endif

EXTRA_DIST = $(OAF_predata)
