/**
* Fatih Demir <kabalak@gmx.net>
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Here has the preferences box got his own
*  home file ...
*
* -- the source
**/

#include "prefs.h"
#include "dialogs.h"
#include "languages.h"
#include "find.h"

#include <libgtranslator/preferences.h>

/* The callbacks */
static void prefs_box_changed(GtkWidget * widget, gpointer useless);
static void prefs_box_apply(GtkWidget * widget, gint page_num,
			    gpointer useless);
static void prefs_box_help(GtkWidget * widget, gpointer useless);

static gint list_ref = 0;

/* The notebook page widgets  */
static GtkWidget *first_page, *second_page, *third_page, *fourth_page;

/**
* The entries
**/
static GtkWidget
	*authors_name, *authors_email, *authors_language,
	*mime_type, *encoding, *lcode, *lg_email;

/**
* The toggle buttons used in the preferences box
**/
static GtkWidget
	*warn_if_no_change, *warn_if_fuzzy, *unmark_fuzzy,
	*dont_save_unchanged_files, *save_geometry_tb,
	*enable_popup_menu, *use_dot_char;

/* The preferences dialog */
static GtkWidget *prefs = NULL;

GtkWidget *attach_combo_with_label(GtkWidget * table, gint row,
				   const char *label_text,
				   GList * list, const char *value,
				   GtkSignalFunc callback,
				   gpointer user_data)
{
	GtkWidget *label;
	GtkWidget *combo;
	label = gtk_label_new(label_text);
	combo = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(combo), list);
	if (value)
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(combo)->entry), value);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), combo, 1, 2, row, row + 1);
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(combo)->entry), "changed",
			   GTK_SIGNAL_FUNC(callback), user_data);
	return combo;
}

GtkWidget *attach_toggle_with_label(GtkWidget * table, gint row,
				    const char *label_text,
				    gboolean value,
				    GtkSignalFunc callback)
{
	GtkWidget *toggle;
	toggle = gtk_check_button_new_with_label(label_text);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), value);
	gtk_table_attach_defaults(GTK_TABLE(table), toggle, 0, 1, row, row + 1);
	gtk_signal_connect(GTK_OBJECT(toggle), "toggled",
			   GTK_SIGNAL_FUNC(callback), NULL);
	return toggle;
}

GtkWidget *attach_entry_with_label(GtkWidget * table, gint row,
				   const char *label_text,
				   const char *value,
				   GtkSignalFunc callback)
{
	GtkWidget *label;
	GtkWidget *entry;
	label = gtk_label_new(label_text);
	entry = gtk_entry_new();
	if (value)
		gtk_entry_set_text(GTK_ENTRY(entry), value);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), entry, 1, 2, row, row + 1);
	gtk_signal_connect(GTK_OBJECT(entry), "changed",
			   GTK_SIGNAL_FUNC(callback), NULL);
	return entry;
}

GtkWidget *attach_text_with_label(GtkWidget * table, gint row,
				  const char *label_text,
				  const char *value,
				  GtkSignalFunc callback)
{
	GtkWidget *label;
	GtkWidget *widget;
	GtkWidget *scroll;
	label = gtk_label_new(label_text);
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	widget = gtk_text_new(NULL, NULL);
	gtk_text_set_editable(GTK_TEXT(widget), TRUE);
	if (value)
		gtk_text_insert(GTK_TEXT(widget), NULL, NULL, NULL, value, -1);
	gtk_container_add(GTK_CONTAINER(scroll), widget);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), scroll, 1, 2, row, row + 1);
	gtk_signal_connect(GTK_OBJECT(widget), "changed",
			   GTK_SIGNAL_FUNC(callback), NULL);
	return widget;
}

GtkWidget *append_page_table(GtkWidget * probox, gint rows, gint cols,
			     const char *label_text)
{
	GtkWidget *label;
	GtkWidget *page;
	label = gtk_label_new(label_text);
	page = gtk_table_new(rows, cols, FALSE);
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(probox), page, label);
	return page;
}

/* Set up the lists */
void create_lists(void)
{
	gint c = 0;
	list_ref++;
	/* Create only if it's the first call */
	if (list_ref > 1) 
		return;
	languages_list = encodings_list = lcodes_list = group_emails_list = 
		bits_list = NULL;
	while (languages[c].name != NULL) {
		languages_list =
		    g_list_prepend(languages_list,
				   (gpointer) _(languages[c].name));
		lcodes_list =
		    g_list_prepend(lcodes_list,
				   (gpointer) languages[c].lcode);
		if (g_list_find_custom
		    (encodings_list, (gpointer) languages[c].enc,
		     (GCompareFunc) g_strcasecmp) == NULL)
			encodings_list =
			    g_list_prepend(encodings_list,
					   (gpointer) languages[c].enc);
		if (g_list_find_custom
		    (group_emails_list, (gpointer) languages[c].group,
		     (GCompareFunc) g_strcasecmp) == NULL)
			group_emails_list =
			    g_list_prepend(group_emails_list,
					   (gpointer) languages[c].group);
		if (g_list_find_custom
		    (bits_list, (gpointer) languages[c].bits,
		     (GCompareFunc) g_strcasecmp) == NULL)
			bits_list =
			    g_list_prepend(bits_list,
					   (gpointer) languages[c].bits);
		c++;
	}
	/* Arrange resulting lists */
	languages_list = g_list_reverse(languages_list);
	lcodes_list = g_list_reverse(lcodes_list);
	group_emails_list =
	    g_list_sort(group_emails_list, (GCompareFunc) g_strcasecmp);
	encodings_list =
	    g_list_sort(encodings_list, (GCompareFunc) g_strcasecmp);
	bits_list = g_list_sort(bits_list, (GCompareFunc) g_strcasecmp);
}

void prefs_box(GtkWidget * widget, gpointer useless)
{
	raise_and_return_if_exists(prefs);
	/**
	* Create the prefs-box .. 
	**/
	prefs = gnome_property_box_new();
	gtk_window_set_title(GTK_WINDOW(prefs), _("gtranslator -- options"));
	/**
	* The tables for holding all the entries below
	**/
	first_page = append_page_table(prefs, 2, 2, _("Personal information"));
	second_page = append_page_table(prefs, 5, 2, _("Language options"));
	third_page = append_page_table(prefs, 4, 1, _("Po file options"));
	fourth_page = append_page_table(prefs, 3, 1, _("Miscellaneous"));
	/**
	* Create all the personal entries
	**/
	authors_name =
	    attach_entry_with_label(first_page, 0, _("Author's name :"),
				    author, prefs_box_changed);
	authors_email =
	    attach_entry_with_label(first_page, 1, _("Author's EMail :"),
				    email, prefs_box_changed);
	/**
	* Create, attach, and connect all the combo boxes with labels 
	**/
	create_lists();

	authors_language =
	    attach_combo_with_label(second_page, 0, _("Language :"),
				    languages_list, language,
				    prefs_box_changed, GINT_TO_POINTER(1));
	lcode =
	    attach_combo_with_label(second_page, 1, _("Language code :"),
				    lcodes_list, lc, 
				    prefs_box_changed, GINT_TO_POINTER(2));
	lg_email =
	    attach_combo_with_label(second_page, 2,
				    _("Language group's EMail :"),
				    group_emails_list, lg,
				    prefs_box_changed, NULL);
	mime_type =
	    attach_combo_with_label(second_page, 3, _("Charset :"),
				    encodings_list, mime,
				    prefs_box_changed, NULL);
	encoding =
	    attach_combo_with_label(second_page, 4, _("Encoding :"),
				    bits_list, enc,
				    prefs_box_changed, NULL);
	/**
	* Create, attach, and connect the toggle buttons
	**/
	unmark_fuzzy =
	    attach_toggle_with_label(third_page, 0,
		_("Set non-fuzzy status, if message was changed"),
		wants.unmark_fuzzy, prefs_box_changed);
	warn_if_fuzzy =
	    attach_toggle_with_label(third_page, 1,
		_("Warn if the .po-file contains fuzzy translations"),
		wants.warn_if_fuzzy, prefs_box_changed);
	dont_save_unchanged_files =
	    attach_toggle_with_label(third_page, 2,
		_("Don't save unchanged .po-files"),
		wants.dont_save_unchanged_files, prefs_box_changed);
	warn_if_no_change =
	    attach_toggle_with_label(third_page, 3,
		_("Warn me if I'm trying to save an unchanged file"),
		wants.warn_if_no_change, prefs_box_changed);

	/**
	* The fourth page with the popup menu & the dot_char.
	**/
	use_dot_char=attach_toggle_with_label(fourth_page, 0,
		_("Use the dot char \'·\' instead of free spaces"),
		wants.dot_char, prefs_box_changed);
	enable_popup_menu=attach_toggle_with_label(fourth_page, 1,
		_("Enable popup menu(s)"),
		wants.popup_menu, prefs_box_changed);	
	save_geometry_tb=attach_toggle_with_label(fourth_page, 2,
		_("Save geometry on exit & restore it on startup"),
		wants.save_geometry, prefs_box_changed);	
	/**
	* The basic signal-handlers 
	**/
	gtk_signal_connect(GTK_OBJECT(prefs), "apply",
			   GTK_SIGNAL_FUNC(prefs_box_apply), NULL);
	gtk_signal_connect(GTK_OBJECT(prefs), "help",
			   GTK_SIGNAL_FUNC(prefs_box_help), NULL);
	gtk_signal_connect(GTK_OBJECT(prefs), "close",
			   GTK_SIGNAL_FUNC(destroy_lists), NULL);
	show_nice_dialog(&prefs, "gtranslator -- prefs");
}

/**
* If it's an apply then do this nice moves ...
**/
static void prefs_box_apply(GtkWidget * box, gint page_num, gpointer useless)
{
	/* We need to apply only once */
	if (page_num != -1)
		return;
#define update(value,widget) g_free(value);\
	value=gtk_editable_get_chars(GTK_EDITABLE(widget),0,-1);
	update(author, authors_name);
	update(email, authors_email);
	update(language, GTK_COMBO(authors_language)->entry);
	update(lc, GTK_COMBO(lcode)->entry);
	update(lg, GTK_COMBO(lg_email)->entry);
	update(mime, GTK_COMBO(mime_type)->entry);
	update(enc, GTK_COMBO(encoding)->entry);
#undef update
#define if_active(widget) \
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
	wants.save_geometry = if_active(save_geometry_tb);
	wants.unmark_fuzzy = if_active(unmark_fuzzy);
	wants.warn_if_fuzzy = if_active(warn_if_fuzzy);
	wants.warn_if_no_change = if_active(warn_if_no_change);
	wants.dont_save_unchanged_files = if_active(dont_save_unchanged_files);
	wants.dot_char = if_active(use_dot_char);
	wants.popup_menu = if_active(enable_popup_menu);
#undef if_active

	gtranslator_config_set_string("translator/name", author);
	gtranslator_config_set_string("translator/email", email);
	gtranslator_config_set_string("language/name", language);
	gtranslator_config_set_string("language/mime_type", mime);
	gtranslator_config_set_string("language/encoding", enc);
	gtranslator_config_set_string("language/language_code", lc);
	gtranslator_config_set_string("language/team_email", lg);
	gtranslator_config_set_bool("toggles/save_geometry", wants.save_geometry);
	gtranslator_config_set_bool("toggles/warn_if_fuzzy", wants.warn_if_fuzzy);
	gtranslator_config_set_bool("toggles/set_non_fuzzy_if_changed", 
			      wants.unmark_fuzzy);
	gtranslator_config_set_bool("toggles/warn_if_no_change",
			      wants.warn_if_no_change);
	gtranslator_config_set_bool("toggles/do_not_save_unchanged_files",
			      wants.dont_save_unchanged_files);
	gtranslator_config_set_bool("toggles/use_dot_char",
			      wants.dot_char);
	gtranslator_config_set_bool("toggles/enable_popup_menu",
			      wants.popup_menu);		      
}

/**
* The preferences box help
**/
static void prefs_box_help(GtkWidget * widget, gpointer useless)
{
	gnome_app_message(GNOME_APP(app1), _("\
With the Preferences box you can define some variables\n\
with which you can make gtranslator make more work\n\
like YOU want it to work!"));
}

gboolean destroy_lists(GtkWidget * widget, gpointer useless)
{
	list_ref--;
	/* If something needs them, leave */
	if (list_ref != 0) return FALSE;
#define free_a_list(list) g_list_free(list); list=NULL;
	free_a_list(languages_list);
	free_a_list(lcodes_list);
	free_a_list(group_emails_list);
	free_a_list(encodings_list);
	free_a_list(bits_list);
#undef free_a_list
	return FALSE;
}

static void prefs_box_changed(GtkWidget * widget, gpointer flag)
{
	gint c = 0;
	gchar *current;
	gnome_property_box_changed(GNOME_PROPERTY_BOX(prefs));
#define set_text(widget,field) \
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(widget)->entry),\
			   languages[c].field)
	switch (GPOINTER_TO_INT(flag)) {
	case 1:
		current =
		    gtk_entry_get_text(GTK_ENTRY
				       (GTK_COMBO(authors_language)->entry));
		while (languages[c].name != NULL) {
			if (!g_strcasecmp(current, _(languages[c].name))) {
				set_text(lcode, lcode);
				set_text(mime_type, enc);
				set_text(encoding, bits);
				set_text(lg_email, group);
				break;
			}
			c++;
		}
		break;
	case 2:
		current =
		    gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(lcode)->entry));
		while (languages[c].name != NULL) {
			if (!g_strcasecmp(current, languages[c].lcode)) {
				set_text(mime_type, enc);
				set_text(encoding, bits);
				set_text(lg_email, group);
				break;
			}
			c++;
		}
		break;
	default:
		break;
	}
#undef set_text
}

void read_prefs(void)
{
	author = gtranslator_config_get_string("translator/name");
	email = gtranslator_config_get_string("translator/email");
	language = gtranslator_config_get_string("language/name");
	lc = gtranslator_config_get_string("language/language_code");
	lg = gtranslator_config_get_string("language/team_email");
	mime = gtranslator_config_get_string("language/mime_type");
	enc = gtranslator_config_get_string("language/encoding");
	wants.save_geometry =
	    gtranslator_config_get_bool("toggles/save_geometry");
	wants.unmark_fuzzy =
	    gtranslator_config_get_bool("toggles/set_non_fuzzy_if_changed"); 
	wants.warn_if_fuzzy =
	    gtranslator_config_get_bool("toggles/warn_if_fuzzy");
	wants.warn_if_no_change =
	    gtranslator_config_get_bool("toggles/warn_if_no_change");
	wants.dont_save_unchanged_files =
	    gtranslator_config_get_bool("toggles/do_not_save_unchanged_files");
	wants.popup_menu =
	    gtranslator_config_get_bool("toggles/enable_popup_menu");
	wants.dot_char = 
	    gtranslator_config_get_bool("toggles/use_dot_char");    
	wants.match_case = gtranslator_config_get_bool("find/case_sensitive");
	wants.find_in = gtranslator_config_get_int("find/find_in");
	update_flags();
	wants.fill_header = gtranslator_config_get_bool("toggles/fill_header");
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
}

void save_geometry(void)
{
	if (wants.save_geometry == TRUE) {
		gchar *gstr;
		gint x, y, w, h;
		gstr = gnome_geometry_string(app1->window);
		gnome_parse_geometry(gstr, &x, &y, &w, &h);
		g_free(gstr);
		gtranslator_config_init();
		gtranslator_config_set_int("geometry/x", x);
		gtranslator_config_set_int("geometry/y", y);
		gtranslator_config_set_int("geometry/width", w);
		gtranslator_config_set_int("geometry/height", h);
		gtranslator_config_close();
		
	}
}

void restore_geometry(gchar * gstr)
{
	gint x, y, width, height;
	/* Set the main window's geometry from prefs. */
	if (gstr == NULL) {
		if (wants.save_geometry == TRUE) {
			gtranslator_config_init();
			x = gtranslator_config_get_int("geometry/x");
			y = gtranslator_config_get_int("geometry/y");
			width = gtranslator_config_get_int("geometry/width");
			height = gtranslator_config_get_int("geometry/height");
			gtranslator_config_close();
		}
		else return;
	}
	/* And if a geometry-definition has been given .. parse it. */
	else {
		if (!gnome_parse_geometry(gstr, &x, &y, &width, &height)) {
			g_warning(
			    _("The geometry string \"%s\" couldn't be parsed!"),
			    gstr);
			return;
		}
	}
	if (x != -1)
		gtk_widget_set_uposition(app1, x, y);
	if ((width > 0) && (height > 0))
		gtk_window_set_default_size(GTK_WINDOW(app1), width, height);
}

