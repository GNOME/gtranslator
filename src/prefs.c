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
	authors_name_label=gtk_label_new(_("Author's name :"));
	authors_email_label=gtk_label_new(_("Author's EMail :"));
	authors_language_label=gtk_label_new(_("Language :"));
	mime_type_label=gtk_label_new(_("MIME type :"));
	encoding_label=gtk_label_new(_("Encoding :"));
	lcode_label=gtk_label_new(_("Language code :"));
	lg_email_label=gtk_label_new(_("Language group's EMail :"));
	additional_comments_label=gtk_label_new(_("Comments :"));
	/**
	* The entries for that .
	**/
	authors_name=gtk_combo_new();
	authors_email=gtk_combo_new();
	authors_language=gtk_combo_new();
	mime_type=gtk_combo_new();
	encoding=gtk_combo_new();
	lcode=gtk_combo_new();
	lg_email=gtk_combo_new();
	additional_comments=gtk_text_new(NULL,NULL);
	gtk_text_set_editable(GTK_TEXT(additional_comments),TRUE);
	add_additional_comments=gtk_check_button_new_with_label(_("Add the additional comments to the header"));
	save_geometry=gtk_check_button_new_with_label(_("Save gtranslator's geometry on exit & restore it on startup"));
	warn_if_no_change=gtk_check_button_new_with_label(_("Warn me if I'm trying to save an unchanged file"));
	warn_if_fuzzy=gtk_check_button_new_with_label(_("Warn if the .po-file contains fuzzy translations"));
	dont_save_unchanged_files=gtk_check_button_new_with_label(_("Don't save unchanged .po-files"));
	use_msg_db=gtk_check_button_new_with_label(_("Use the msg_db"));
	/**
	* If previous settings were found they'll be inserted automatically
	**/
	if(author)
        {
                gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(authors_name)->entry),author);
        }
	if(email)
	{
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(authors_email)->entry),email);	
	}
	if(language)
	{
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(authors_language)->entry),language);
		languages[c].name=language;
	}
	if(mime)
	{
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(mime_type)->entry),mime);
	}
	if(enc)
	{
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(encoding)->entry),enc);
	}
	if(lc)
	{
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(lcode)->entry),lc);
	}	
	if(lg)
	{
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(lg_email)->entry),lg);
	}
	if(comments)
	{
		gtk_text_insert(GTK_TEXT(additional_comments),NULL,NULL,NULL,comments,-1);
	}
	if(if_use_msg_db==TRUE)	
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(use_msg_db),TRUE);
	}
	if(if_add_additional_comments==TRUE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(add_additional_comments),TRUE);
	}
	if(if_save_geometry==TRUE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(save_geometry),TRUE);
	}
	if(if_warn_if_fuzzy==TRUE)
        {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(warn_if_fuzzy),TRUE);
	}
	if(if_warn_if_no_change==TRUE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(warn_if_no_change),TRUE);
	}
	if(if_dont_save_unchanged_files==TRUE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dont_save_unchanged_files),TRUE);
	}
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
	third_page=gtk_table_new(6,1,FALSE);
	/**
	* Sets up the languages list
	**/
	while(languages[c].name!=NULL)
	{
	 	languages_list=g_list_append(languages_list,(gpointer)_(languages[c].name));	
		lcodes_list=g_list_append(lcodes_list,(gpointer)languages[c].lcode);
		encodings_list=g_list_append(encodings_list,(gpointer)languages[c].enc);
		group_emails_list=g_list_append(group_emails_list,(gpointer)languages[c].group);
		bits_list=g_list_append(bits_list,(gpointer)languages[c].bits);
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
	/****************************************************************/
	gtk_combo_set_popdown_strings(GTK_COMBO(encoding),bits_list);
	gtk_combo_set_use_arrows(GTK_COMBO(encoding),10);
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
	gtk_table_attach_defaults(GTK_TABLE(second_page),
		lg_email_label,0,1,3,4);
	gtk_table_attach_defaults(GTK_TABLE(second_page),
                lg_email,1,2,3,4);
	/****************************************************************/
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		add_additional_comments,0,1,0,1);
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		save_geometry,0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		warn_if_no_change,0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		dont_save_unchanged_files,0,1,3,4);
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		warn_if_fuzzy,0,1,4,5);
	gtk_table_attach_defaults(GTK_TABLE(third_page),
		use_msg_db,0,1,5,6);				
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
	/*** FIXME How can I set the first element without this hack ? FIXME ***/
	gtk_signal_connect(GTK_OBJECT(prefs),"show",
		GTK_SIGNAL_FUNC(prefs_box_changed),(gpointer)1);
	/******************************************************************/
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(GTK_COMBO(authors_language)->entry)),"changed",
		GTK_SIGNAL_FUNC(prefs_box_changed),(gpointer)1);
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(GTK_COMBO(encoding)->entry)),"changed",
                GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(GTK_COMBO(authors_name)->entry)),"changed",
                GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(GTK_COMBO(authors_email)->entry)),"changed",
                GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(GTK_COMBO(lcode)->entry)),"changed",
                GTK_SIGNAL_FUNC(prefs_box_changed),(gpointer)2);
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(GTK_COMBO(mime_type)->entry)),"changed",
                GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(GTK_COMBO(lg_email)->entry)),"changed",
                GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	gtk_signal_connect(GTK_OBJECT(additional_comments),"changed",
		GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	gtk_signal_connect(GTK_OBJECT(save_geometry),"toggled",
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
void prefs_box_changed(GtkWidget *widget,gpointer chitem)
{
	gint c=0;
	gint ch;
	gchar *current;
	gnome_property_box_changed(GNOME_PROPERTY_BOX(prefs));	
	ch=(gint)chitem;
	switch(ch)
	{
		case 1 :
			current=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(authors_language)->entry));
			while(languages[c].name!=NULL)
			{
				if(!g_strcasecmp(current,_(languages[c].name)))
				{
					gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(lg_email)->entry),languages[c].group);	
					gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(lcode)->entry),languages[c].lcode);
					gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(mime_type)->entry),languages[c].enc);	
					gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(encoding)->entry),languages[c].bits);
				}
				c++;
			}
			break;
		case 2 :
			current=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(lcode)->entry));
			c=0;
			while(languages[c].name!=NULL)
			{
				if(!g_strcasecmp(current,languages[c].lcode))
				{
				 	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(lg_email)->entry),languages[c].group);
					gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(mime_type)->entry),languages[c].enc);
					gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(encoding)->entry),languages[c].bits);
				}
				c++;
			}
			break;
		default :
			break;
	}
}

/**
* If it's an apply then do this nice moves ...
**/
void prefs_box_apply(GtkWidget *widget,gpointer more_useless)
{
	gnome_config_push_prefix("/gtranslator/");
	author=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(authors_name)->entry));
	email=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(authors_email)->entry));
	language=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(authors_language)->entry));
	mime=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(mime_type)->entry));
	enc=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(encoding)->entry));
	lc=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(lcode)->entry));
	lg=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(lg_email)->entry));
	comments=gtk_editable_get_chars(GTK_EDITABLE(additional_comments),0,gtk_text_get_length(GTK_TEXT(additional_comments)));
	if_use_msg_db=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(use_msg_db));
	if_add_additional_comments=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(add_additional_comments));
	if_save_geometry=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(save_geometry));
	if_warn_if_fuzzy=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(warn_if_fuzzy));
	if_warn_if_no_change=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(warn_if_no_change));
	if_dont_save_unchanged_files=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dont_save_unchanged_files));
	
	gnome_config_set_string("Translator/Name",author);
	gnome_config_set_string("Translator/Email",email);
	gnome_config_set_string("Language/Name",language);
	gnome_config_set_string("Language/Mime-type",mime);
	gnome_config_set_string("Language/Encoding",enc);
	gnome_config_set_string("Language/Language-code",lc);
	gnome_config_set_string("Language/Team's EMail address",lg);
	gnome_config_set_string("Extra/Comments",comments);
	gnome_config_set_bool("Toggles/Use msg_db",if_use_msg_db);
	gnome_config_set_bool("Toggles/Add comments",if_add_additional_comments);
	gnome_config_set_bool("Toggles/Save Geometry",if_save_geometry);
	gnome_config_set_bool("Toggles/Warn if fuzzy",if_warn_if_fuzzy);
	gnome_config_set_bool("Toggles/Warn if no change",if_warn_if_no_change);
	gnome_config_set_bool("Toggles/Don't save unchanged files",if_dont_save_unchanged_files);
	gnome_config_pop_prefix();
	gnome_config_sync();
	gnome_config_drop_all();
}

void read_prefs()
{
	gnome_config_push_prefix("/gtranslator/");
	author=gnome_config_get_string("Translator/Name");
        email=gnome_config_get_string("Translator/Email");
        language=gnome_config_get_string("Language/Name");
        mime=gnome_config_get_string("Language/Mime-type");
        enc=gnome_config_get_string("Language/Encoding");
        lc=gnome_config_get_string("Language/Language-code");
        lg=gnome_config_get_string("Language/Team's EMail address");
        comments=gnome_config_get_string("Extra/Comments");
        if_use_msg_db=gnome_config_get_bool("Toggles/Use msg_db");
        if_add_additional_comments=gnome_config_get_bool("Toggles/Add comments");
	if_save_geometry=gnome_config_get_bool("Toggles/Save Geometry");
        if_warn_if_fuzzy=gnome_config_get_bool("Toggles/Warn if fuzzy");
        if_warn_if_no_change=gnome_config_get_bool("Toggles/Warn if no change");
        if_dont_save_unchanged_files=gnome_config_get_bool("Toggles/Don't save unchanged files");
	gnome_config_pop_prefix();
}
