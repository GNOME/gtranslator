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
	authors_language_team=gnome_entry_new("LANGUAGE TEAM");
	mime_type=gnome_entry_new("MIME TYPE");
	encoding=gnome_entry_new("ENCODING");
	additional_comments=gtk_text_new(NULL,NULL);
	gtk_text_set_editable(GTK_TEXT(additional_comments),TRUE);
	/**
	* The table holding these entries ..
	**/
	first_page=gtk_table_new(7,5,FALSE);
	/**
	* Add this table to the notebook of the Propertybox ..
	**/
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(prefs),
		first_page,first_page_label);
	/**
	* The basic signal-handlers 
	**/
	gtk_signal_connect(GTK_OBJECT(prefs),"close",
		GTK_SIGNAL_FUNC(prefs_box_hide),NULL);
	gtk_signal_connect(GTK_OBJECT(prefs),"apply",
		GTK_SIGNAL_FUNC(prefs_box_apply),NULL);
	gtk_signal_connect(GTK_OBJECT(prefs),"help",
		GTK_SIGNAL_FUNC(prefs_box_help),NULL);
}

/**
* Shows the prefs-box ..
**/
void prefs_box_show(GtkWidget *widget,gpointer useless)
{
	gtk_widget_show_all(GTK_WIDGET(GNOME_PROPERTY_BOX(prefs)));
}

/**
* And this one hides it ..
**/
void prefs_box_hide(GtkWidget *widget,gpointer useless)
{
	gtk_widget_hide(prefs);
}

/**
* If it's an apply then do this nice moves ...
**/
void prefs_box_apply(GtkWidget *widget,gpointer more_useless)
{
	gnome_config_push_prefix("/gtranslator/");
	gnome_config_set_string("Test/Message","Merhaba");
	gnome_config_pop_prefix();
	gnome_config_synx();
	gnome_config_drop_all();	
}
