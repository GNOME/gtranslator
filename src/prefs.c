/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * (C) 2000 Published under GNU GPL V 2.0+
 *
 * Here has the preferences box got his own
 *  home file ...
 *
 * -- the source
 **/

#include "prefs.h"

void prefs_box(GtkWidget *widget,gpointer useless)
{
	/**
	 * Declare all the Gtk+ lables here
	 **/
	GtkWidget *first_page_label,*second_page_label,
		*third_page_label;
	GtkWidget *authors_name_label,*authors_email_label,
		*authors_language_label,*authors_language_team_label,
	 	*mime_type_label,*encoding_label,
		*additional_comments_label;
	/**
	 * Create all the labels and the prefs-box .. 
	 **/
	prefs=gnome_property_box_new();
	first_page_label=gtk_label_new(_("Personal informations"));
	second_page_label=gtk_label_new(_("Po file options"));
	third_page_label=gtk_label_new(_("Programm options"));
}
