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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif // HAVE_CONFIG_H

#include "about.h"
#include "gtr_dialogs.h"

/**
* The widgets 
**/
GtkWidget *prefs,*first_page,*second_page,*third_page;

/**
* The entries
**/
GtkWidget *authors_name,*authors_email,*authors_language;
GtkWidget *authors_language_team,*mime_type,*encoding;
GtkWidget *additional_comments;

/**
* Three lists for the combo-boxes ..
**/
GList *languages_list,*encodings_list,*mime_types_list;

/**
* The *-buttons used in the preferences box
**/
GtkWidget *use_msg_db,*add_additional_comments,*show_output;
GtkWidget *warn_if_no_change,*dont_save_unchanged_files,*print_info;
GtkWidget *warn_if_fuzzy,*warn_if_untranslated;

/**
* The labels are all declared in the sources as they're'nt needed
*  to be here
**/

/**
* The first one creates the preferences-box and the others show/hide it
**/
void prefs_box(GtkWidget *widget,gpointer useless);
void prefs_box_show(GtkWidget *widget,gpointer useless);
void prefs_box_hide(GtkWidget *widget,gpointer useless);
void prefs_box_apply(GtkWidget *widget,gpointer more_useless);
