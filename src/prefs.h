/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Here has the preferences box got his own
*  home file ...
*
* -- the header
**/

#ifndef GTR_PREFS_H
#define GTR_PREFS_H 1

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "about.h"
#include "dialogs.h"

/**
* These are the variables set by the prefs-box ( globally ones )
**/
gchar *author,*email,*language,*mime,*enc,*lc,*lg,*comments;
gboolean if_use_msg_db,if_add_additional_comments,if_warn_if_fuzzy,
	if_warn_if_no_change,if_dont_save_unchanged_files,
	if_save_geometry;
gint gtranslator_geometry_x,gtranslator_geometry_y,
	gtranslator_geometry_w,gtranslator_geometry_h;

// Options, used NOT in prefs-box.
gboolean if_match_case, if_fill_header;

// Preferences-box creation and callbacks
void prefs_box(GtkWidget *widget,gpointer useless);
void read_prefs(void);
void free_prefs(void);

#endif
