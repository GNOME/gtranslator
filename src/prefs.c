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
#include "languages.h"

void prefs_box()
{
	gint c=0;
	/**
	* Declare all the Gtk+ lables here
	**/
	GtkWidget *first_page_label,*second_page_label,
		*third_page_label;
	GtkWidget *authors_name_label,*authors_email_label,
		*authors_language_label,
	 	*mime_type_label,*encoding_label,
		*additional_comments_label,*lcode_label,*lg_email_label;
	/**
	* Create the prefs-box .. 
	**/
	prefs=gnome_property_box_new();
	/**
	* Create the labels ..
	**/
	first_page_label=gtk_label_new(_("Personal informations"));
	second_page_label=gtk_label_new(_("Language options"));
	third_page_label=gtk_label_new(_("Po file options"));
	authors_name_label=gtk_label_new(_("Author's name :  "));
	authors_email_label=gtk_label_new(_("Author's EMail : "));
	authors_language_label=gtk_label_new(_("Language : "));
	mime_type_label=gtk_label_new(_("MIME type : "));
	encoding_label=gtk_label_new(_("Encoding : "));
	lcode_label=gtk_label_new(_("Language code : "));
	lg_email_label=gtk_label_new(_("Language group's EMail : "));
	additional_comments_label=gtk_label_new(_("Comments : "));
	/**
	* The entries for that .
	**/
	authors_name=gnome_entry_new("AUTHOR");
	authors_email=gnome_entry_new("EMAIL");
	authors_language=gnome_entry_new("LANGUAGE");
	mime_type=gnome_entry_new("MIME TYPE");
	encoding=gnome_entry_new("ENCODING");
	lcode=gnome_entry_new("LANGUAGE CODE");
	lg_email=gnome_entry_new("LANGUAGE TEAM'S EMAIL");
	additional_comments=gtk_text_new(NULL,NULL);
	gtk_text_set_editable(GTK_TEXT(additional_comments),TRUE);
	add_additional_comments=gtk_check_button_new_with_label(_("Add the additional comments to the header"));
	warn_if_no_change=gtk_check_button_new_with_label(_("Wanr if trying to save an unchanged file"));
	warn_if_fuzzy=gtk_check_button_new_with_label(_("Warn if the .po-file contains fuzzy translations"));
	dont_save_unchanged_files=gtk_check_button_new_with_label(_("Don't save unchanged .po-files"));
	use_msg_db=gtk_check_button_new_with_label(_("Use the msg_db . ( FUTURE_WORK ! )"));
	/**
	* The table holding these entries ..
	**/
	first_page=gtk_table_new(5,2,FALSE);
	/**
	* The second page ..
	**/
	second_page=gtk_table_new(5,2,FALSE);
	/**
	* The third page
	**/
	third_page=gtk_table_new(5,1,FALSE);
	/**
	* Sets up the languages list
	**/
	while(languages[c].name!=NULL)
	{
	 	languages_list=g_list_append(languages_list,(gpointer)languages[c].name);	
		lcodes_list=g_list_append(lcodes_list,(gpointer)languages[c].lcode);
		encodings_list=g_list_append(encodings_list,(gpointer)languages[c].enc);
		group_emails_list=g_list_append(group_emails_list,(gpointer)languages[c].group);
		
		c++;
	}
	/**
	* Connect the languages_list with the entries ..
	**/
	gtk_combo_set_popdown_strings(GTK_COMBO(authors_language),languages_list);
	gtk_combo_set_use_arrows(GTK_COMBO(authors_language),10);
	/****************************************************************/
	gtk_combo_set_popdown_strings(GTK_COMBO(mime_type),encodings_list);
	gtk_combo_set_use_arrows(GTK_COMBO(mime_type),10);
	/****************************************************************/
	gtk_combo_set_popdown_strings(GTK_COMBO(lcode),lcodes_list);
	gtk_combo_set_use_arrows(GTK_COMBO(lcode),10);
	/****************************************************************/
	gtk_combo_set_popdown_strings(GTK_COMBO(lg_email),group_emails_list);
	gtk_combo_set_use_arrows(GTK_COMBO(lg_email),10);
	/**
	* Add the items to the tables
	**/
	gtk_table_attach_defaults(GTK_TABLE(first_page),
		authors_name_label,0,1,0,1);
	gtk_table_attach_defaults(GTK_TABLE(first_page),
		authors_name,1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(first_page),
		authors_email_label,0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(first_page),
		authors_email,1,2,1,2);
	gtk_table_attach_defaults(GTK_TABLE(first_page),
		authors_language_label,0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(first_page),
		authors_language,1,2,2,3);
	gtk_table_attach_defaults(GTK_TABLE(first_page),
		additional_comments_label,0,1,3,4);
	gtk_table_attach_defaults(GTK_TABLE(first_page),
		additional_comments,1,2,3,4);
	/****************************************************************/
	gtk_table_attach_defaults(GTK_TABLE(second_page),
		mime_type_label,0,1,0,1);
	gtk_table_attach_defaults(GTK_TABLE(second_page),
		mime_type,1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(second_page),
		lcode_label,0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(second_page),
		lcode,1,2,1,2);			
	gtk_table_attach_defaults(GTK_TABLE(second_page),
		encoding_label,0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(second_page),
		encoding,1,2,2,3);
	/****************************************************************/
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		add_additional_comments,0,1,0,1);
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		warn_if_no_change,0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		dont_save_unchanged_files,0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		warn_if_fuzzy,0,1,3,4);
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		use_msg_db,0,1,4,5);				
	/**
	* Add this table to the notebook of the Propertybox ..
	**/
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(prefs),
		first_page,first_page_label);
	/*****************************************************************/
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(prefs),
		second_page,second_page_label);
	/*****************************************************************/
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(prefs),
		third_page,third_page_label);
	/*****************************************************************/
	gtk_notebook_popup_enable(GTK_NOTEBOOK(GNOME_PROPERTY_BOX(prefs)->notebook));
	/**
	* The basic signal-handlers 
	**/
	gtk_signal_connect(GTK_OBJECT(additional_comments),"changed",
		GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	gtk_signal_connect(GTK_OBJECT(dont_save_unchanged_files),"toggled",
		GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	gtk_signal_connect(GTK_OBJECT(warn_if_no_change),"toggled",
		GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	gtk_signal_connect(GTK_OBJECT(warn_if_fuzzy),"toggled",
		GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	gtk_signal_connect(GTK_OBJECT(use_msg_db),"toggled",
		GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	/******************************************************************/
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
	prefs_box();
	gtk_widget_show_all(prefs);
	gnome_config_set_string("/gtranslator/ZUH","Merhaba");
	gnome_config_sync();
}

/**
* And this one hides it ..
**/
void prefs_box_hide(GtkWidget *widget,gpointer useless)
{
	gtk_widget_hide(prefs);
}

/**
* Sets the apply button of the prefs-box
**/
void prefs_box_changed(GtkWidget *widget,gpointer useless)
{
	gnome_property_box_changed(GNOME_PROPERTY_BOX(prefs));	
}

/**
* If it's an apply then do this nice moves ...
**/
void prefs_box_apply(GtkWidget *widget,gpointer more_useless)
{

}
