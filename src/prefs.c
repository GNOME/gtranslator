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

// Convenience functions for adding items
static GtkWidget * attach_combo_with_label(GtkWidget *table, gint row,
		const char *label_text, GList *list, const char *value,
		gpointer user_data);
static GtkWidget * attach_toggle_with_label(GtkWidget *table, gint row,
		const char *label_text, gboolean value);
static GtkWidget * attach_entry_with_label(GtkWidget *table, gint row,
		const char *label_text, const char *history, const char *value);
static GtkWidget * attach_text_with_label(GtkWidget *table, gint row,
		const char *label_text, const char *value);
static GtkWidget * append_page_table(gint rows,gint cols,
		const char *label_text);

// The callbacks
static void prefs_box_changed(GtkWidget *widget,gpointer useless);
static void prefs_box_apply(GtkWidget *widget,gint page_num,gpointer useless);
static void prefs_box_help(GtkWidget *widget,gpointer useless);
static gboolean prefs_box_close(GtkWidget *widget,gpointer useless);

/**
* The entries
**/
static GtkWidget *authors_name,*authors_email,*authors_language;
static GtkWidget *mime_type,*encoding,*lcode,*lg_email;
static GtkWidget *additional_comments;

/**
* The *-buttons used in the preferences box
**/
static GtkWidget *use_msg_db,*add_additional_comments;
static GtkWidget *warn_if_no_change,*dont_save_unchanged_files;
static GtkWidget *warn_if_fuzzy,*save_geometry;

/**
* Lists for the combo-boxes ..
**/
static GList
	*languages_list=NULL,
	*encodings_list=NULL,
	*lcodes_list=NULL,
	*group_emails_list=NULL,
	*bits_list=NULL;

// The preferences dialog
static GtkWidget *prefs=NULL;

static GtkWidget * attach_combo_with_label(GtkWidget *table, gint row,
		const char *label_text, GList *list, const char *value,
		gpointer user_data)
{
	GtkWidget *label;
	GtkWidget *combo;
	label=gtk_label_new(label_text);
	combo=gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(combo),list);
	if (value)
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(combo)->entry),value);
	gtk_table_attach_defaults(GTK_TABLE(table),
		label,0,1,row,row+1);
	gtk_table_attach_defaults(GTK_TABLE(table),
		combo,1,2,row,row+1);
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(combo)->entry),
		"changed",GTK_SIGNAL_FUNC(prefs_box_changed),user_data);
	return combo;
}

static GtkWidget * attach_toggle_with_label(GtkWidget *table, gint row,
		const char *label_text, gboolean value)
{
	GtkWidget *toggle;
	toggle=gtk_check_button_new_with_label(label_text);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),value);
	gtk_table_attach_defaults(GTK_TABLE(table),
		toggle,0,1,row,row+1);
	gtk_signal_connect(GTK_OBJECT(toggle),
		"toggled",GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	return toggle;
}

static GtkWidget * attach_entry_with_label(GtkWidget *table, gint row,
		const char *label_text, const char *history, const char *value)
{
	GtkWidget *label;
	GtkWidget *entry;
	label=gtk_label_new(label_text);
	entry=gnome_entry_new(history);
	if (value)
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(entry)->entry),value);
	gtk_table_attach_defaults(GTK_TABLE(table),
		label,0,1,row,row+1);
	gtk_table_attach_defaults(GTK_TABLE(table),
		entry,1,2,row,row+1);
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(entry)->entry),
		"changed",GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	return entry;
}

static GtkWidget * attach_text_with_label(GtkWidget *table, gint row,
		const char *label_text, const char *value)
{
	GtkWidget *label;
	GtkWidget *widget;
	label=gtk_label_new(label_text);
	widget=gtk_text_new(NULL,NULL);
	gtk_text_set_editable(GTK_TEXT(widget),TRUE);
	if (value)
		gtk_text_insert(GTK_TEXT(widget),NULL,NULL,NULL,value,-1);
	gtk_table_attach_defaults(GTK_TABLE(table),
		label,0,1,row,row+1);
	gtk_table_attach_defaults(GTK_TABLE(table),
		widget,1,2,row,row+1);
	gtk_signal_connect(GTK_OBJECT(widget),
		"changed",GTK_SIGNAL_FUNC(prefs_box_changed),NULL);
	return widget;
}

static GtkWidget * append_page_table(gint rows,gint cols,const char *label_text)
{
	GtkWidget *label;
	GtkWidget *page;
	label=gtk_label_new(label_text);
	page=gtk_table_new(rows,cols,FALSE);
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(prefs),page,label);
	return page;
}

void prefs_box(GtkWidget *widget,gpointer useless)
{
	gint c;
	// The notebook page widgets 
	static GtkWidget *first_page,*second_page,*third_page;

	raise_and_return_if_exists(prefs);
	/**
	* Create the prefs-box .. 
	**/
	prefs=gnome_property_box_new();
	gtk_notebook_popup_enable(GTK_NOTEBOOK(GNOME_PROPERTY_BOX(prefs)->notebook));
	/**
	* The tables for holding all the entries below
	**/
	first_page=append_page_table(5,2,_("Personal information"));
	second_page=append_page_table(5,2,_("Language options"));
	third_page=append_page_table(6,1,_("Po file options"));
	/**
	* Set up the lists
	**/
	c=0;
	while (languages[c].name!=NULL)
	{
	 	languages_list=g_list_prepend(languages_list,
			(gpointer)_(languages[c].name));	
		lcodes_list=g_list_prepend(lcodes_list,
			(gpointer)languages[c].lcode);
		if (g_list_find_custom(encodings_list,
			(gpointer)languages[c].enc,
			(GCompareFunc)g_strcasecmp)==NULL)
			encodings_list=g_list_prepend(encodings_list,
				(gpointer)languages[c].enc);
		if (g_list_find_custom(group_emails_list,
			(gpointer)languages[c].group,
			(GCompareFunc)g_strcasecmp)==NULL)
		group_emails_list=g_list_prepend(group_emails_list,
			(gpointer)languages[c].group);
		if (g_list_find_custom(bits_list,
			(gpointer)languages[c].bits,
			(GCompareFunc)g_strcasecmp)==NULL)
			bits_list=g_list_prepend(bits_list,
				(gpointer)languages[c].bits);
		c++;
	}
	// Arrange resulting lists
	languages_list=g_list_reverse(languages_list);
	lcodes_list=g_list_reverse(lcodes_list);
	group_emails_list=g_list_sort(group_emails_list,
		(GCompareFunc)g_strcasecmp);
	encodings_list=g_list_sort(encodings_list,
		(GCompareFunc)g_strcasecmp);
	bits_list=g_list_sort(bits_list,
		(GCompareFunc)g_strcasecmp);
	/**
	* Create, attach, and connect all the combo boxes with labels
	**/
	authors_language=attach_combo_with_label(first_page,2,
		_("Language :"),languages_list,language,GINT_TO_POINTER(1));
	mime_type=attach_combo_with_label(second_page,0,
		_("MIME type :"),encodings_list,mime,NULL);
	lcode=attach_combo_with_label(second_page,1,
		_("Language code :"),lcodes_list,lc,GINT_TO_POINTER(2));
	encoding=attach_combo_with_label(second_page,2,
		_("Encoding :"),bits_list,enc,NULL);
	lg_email=attach_combo_with_label(second_page,3,
		_("Language group's EMail :"),group_emails_list,lg,NULL);
	/**
	* Create, attach, and connect the toggle buttons
	**/
	add_additional_comments=attach_toggle_with_label(third_page,0,
		_("Add the additional comments to the header"),
		if_add_additional_comments);
	save_geometry=attach_toggle_with_label(third_page,1,
		_("Save geometry on exit & restore it on startup"),
		if_save_geometry);
	warn_if_no_change=attach_toggle_with_label(third_page,2,
		_("Warn me if I'm trying to save an unchanged file"),
		if_warn_if_no_change);
	warn_if_fuzzy=attach_toggle_with_label(third_page,3,
		_("Warn if the .po-file contains fuzzy translations"),
		if_warn_if_fuzzy);
	dont_save_unchanged_files=attach_toggle_with_label(third_page,4,
		_("Don't save unchanged .po-files"),
		if_dont_save_unchanged_files);
	use_msg_db=attach_toggle_with_label(third_page,5,
		_("Use the messages db"),
		if_use_msg_db);
	/**
	* Create all the gnome entries
	**/
	authors_name=attach_entry_with_label(first_page,0,
		_("Author's name :"),"AUTHORS_NAME",author);
	authors_email=attach_entry_with_label(first_page,1,
		_("Author's EMail :"),"AUTHORS_EMAIL",email);
	additional_comments=attach_text_with_label(first_page,3,
		_("Comments :"),comments);
	/**
	* The basic signal-handlers 
	**/
	gtk_signal_connect(GTK_OBJECT(prefs),"apply",
		GTK_SIGNAL_FUNC(prefs_box_apply),NULL);
	gtk_signal_connect(GTK_OBJECT(prefs),"help",
		GTK_SIGNAL_FUNC(prefs_box_help),NULL);
	gtk_signal_connect(GTK_OBJECT(prefs),"close",
		GTK_SIGNAL_FUNC(prefs_box_close),NULL);
	show_nice_dialog(&prefs,"gtranslator -- prefs");
}

/**
* If it's an apply then do this nice moves ...
**/
static void prefs_box_apply(GtkWidget *box,gint page_num,gpointer useless)
{
	// We need to apply only once
	if (page_num!=-1) return;
#define update(value,widget) g_free(value);\
	value=gtk_editable_get_chars(GTK_EDITABLE(widget),0,-1);
	update(author,GTK_COMBO(authors_name)->entry);
	update(email,GTK_COMBO(authors_email)->entry);
	update(language,GTK_COMBO(authors_language)->entry);
	update(mime,GTK_COMBO(mime_type)->entry);
	update(enc,GTK_COMBO(encoding)->entry);
	update(lc,GTK_COMBO(lcode)->entry);
	update(lg,GTK_COMBO(lg_email)->entry);
	update(comments,additional_comments);
#define if_active(widget) gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
	if_use_msg_db=if_active(use_msg_db);
	if_add_additional_comments=if_active(add_additional_comments);
	if_save_geometry=if_active(save_geometry);
	if_warn_if_fuzzy=if_active(warn_if_fuzzy);
	if_warn_if_no_change=if_active(warn_if_no_change);
	if_dont_save_unchanged_files=if_active(dont_save_unchanged_files);
	
	gnome_config_push_prefix("/gtranslator/");
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
}

/**
* The preferences box help
**/
static void prefs_box_help(GtkWidget *widget,gpointer useless)
{
	gnome_app_message(GNOME_APP(app1),
		_("With the Preferences box you can define some variables\nwith which you can make gtranslator make more work\nlike YOU want it to work!"));
}

static gboolean prefs_box_close(GtkWidget *widget,gpointer useless)
{
#define destroy_list(list) g_list_free(list);	list=NULL;
	destroy_list(languages_list);
	destroy_list(encodings_list);
	destroy_list(lcodes_list);
	destroy_list(group_emails_list);
	destroy_list(bits_list);
	return FALSE;
}

static void prefs_box_changed(GtkWidget *widget,gpointer flag)
{
	gint c=0;
	gchar *current;
	gnome_property_box_changed(GNOME_PROPERTY_BOX(prefs));
	switch (GPOINTER_TO_INT(flag)) {
#define set_text(widget,field) gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(widget)->entry),languages[c].field)
		case 1:
			current=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(authors_language)->entry));
			while (languages[c].name!=NULL) 
			{
				if (!g_strcasecmp(current,_(languages[c].name)))
				{
					set_text(lcode,lcode);
					set_text(mime_type,enc);	
					set_text(encoding,bits);
					set_text(lg_email,group);	
					break;
				}
				c++;
			}
			break;
		case 2:
			
			current=gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(lcode)->entry));
			while (languages[c].name!=NULL)
			{
				if (!g_strcasecmp(current,languages[c].lcode))
				{
					set_text(mime_type,enc);
					set_text(encoding,bits);
					set_text(lg_email,group);
					break;
				}
				c++;
			}
			break;
		default:
	}
}

void read_prefs(void)
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
        if_use_msg_db=gnome_config_get_bool("Toggles/Use msg_db=true");
        if_add_additional_comments=gnome_config_get_bool("Toggles/Add comments=false");
	if_save_geometry=gnome_config_get_bool("Toggles/Save Geometry=true");
        if_warn_if_fuzzy=gnome_config_get_bool("Toggles/Warn if fuzzy=true");
        if_warn_if_no_change=gnome_config_get_bool("Toggles/Warn if no change=true");
        if_dont_save_unchanged_files=gnome_config_get_bool("Toggles/Don't save unchanged files=false");
        if_match_case=gnome_config_get_bool("Toggles/Case sensitive=false");
        if_fill_header=gnome_config_get_bool("Toggles/Fill header=false");
	gtranslator_geometry_x=gnome_config_get_int("Geometry/X");
	gtranslator_geometry_y=gnome_config_get_int("Geometry/Y");
	gtranslator_geometry_w=gnome_config_get_int("Geometry/Width");
	gtranslator_geometry_h=gnome_config_get_int("Geometry/Height");
	gnome_config_pop_prefix();
}

void free_prefs(void)
{
	g_free(author);
	g_free(email);
	g_free(language);
	g_free(mime);
	g_free(enc);
	g_free(lc);
	g_free(lg);
	g_free(comments);
}
