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
GtkWidget *mime_type,*encoding,*lcode,*lg_email;
GtkWidget *additional_comments;

/**
* Three lists for the combo-boxes ..
**/
GList *languages_list,*encodings_list,*lcodes_list,*group_emails_list,*bits_list;

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
* These are the variables set by the prefs-box ( globally ones )
**/
gchar *author,*email,*language,*mime,*enc,*lc,*lg,*comments;
gboolean if_use_msg_db,if_add_additional_comments,if_warn_if_fuzzy,
	if_warn_if_no_change,if_dont_save_unchanged_files,
	if_warn_if_untranslated;

/**
* The first one creates the preferences-box and the others show/hide it
**/
void prefs_box();
void prefs_box_show(GtkWidget *widget,gpointer useless);
void prefs_box_hide(GtkWidget *widget,gpointer useless);
void prefs_box_changed(GtkWidget *widget,gpointer chitem);
void prefs_box_apply(GtkWidget *widget,gpointer more_useless);
void read_prefs();

#endif // GTR_PREFS_H
