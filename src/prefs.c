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
	* Create the prefs-box .. 
	**/
	prefs=gnome_property_box_new();
	/**
	* Create the labels ..
	**/
	first_page_label=gtk_label_new(_("Personal informations"));
	second_page_label=gtk_label_new(_("Po file options"));
	third_page_label=gtk_label_new(_("Programm options"));
	authors_name_label=gtk_label_new(_("Author's name :  "));
	authors_email_label=gtk_label_new(_("Author's EMail : "));
	authors_language_label=gtk_label_new(_("Language : "));
	authors_language_team_label=gtk_label_new(_("Language group : "));
	mime_type_label=gtk_label_new(_("MIME type : "));
	encoding_label=gtk_label_new(_("Encoding : "));
	additional_comments_label=gtk_label_new(_("Comments : "));
	/**
	* The entries for that .
	**/
	authors_name=gnome_entry_new("AUTHOR");
	authors_email=gnome_entry_new("EMAIL");
	authors_language=gnome_entry_new("LANGUAGE");
	/**
	* Set the languages list
 	**/	
	gtk_combo_set_popdowm_strings(GTK_COMBO(GNOME_ENTRY(authors_language)->combo),languages_list);
}
