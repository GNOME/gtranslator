/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
 *			Gediminas Paulauskas <menesis@delfi.lt>
 * 
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "prefs.h"
#include "dialogs.h"
#include "languages.h"
#include "find.h"

#include <libgtranslator/preferences.h>
#include <libgtranslator/stylistics.h>

/*
 * The callbacks:
 */
static void prefs_box_changed(GtkWidget  * widget, gpointer useless);
static void prefs_box_apply(GtkWidget  * widget, gint page_num,
			    gpointer useless);
static void prefs_box_help(GtkWidget  * widget, gpointer useless);

static gint list_ref = 0;

/*
 * The notebook page widgets: 
 */
static GtkWidget *first_page, *second_page, *third_page, *fourth_page,
		*fifth_page, *sixth_page;

/*
 * The entries:
 */
static GtkWidget
	*authors_name, *authors_email, *authors_language,
	*mime_type, *encoding, *lcode, *lg_email, *dictionary_file;

/*
 * The toggle buttons used in the preferences box:
 */
static GtkWidget
	*warn_if_no_change, *warn_if_fuzzy, *unmark_fuzzy,
	*dont_save_unchanged_files, *save_geometry_tb, *no_uzis,
	*enable_popup_menu, *use_dot_char, *use_update_function,
	*check_recent_files, *own_specs, *instant_spell_checking,
	*use_own_dict;
	
/*
 * The preferences dialog widget itself.
 */
static GtkWidget *prefs = NULL;

GtkWidget *attach_combo_with_label(GtkWidget  * table, gint row,
				   const char *label_text,
				   GList  * list, const char *value,
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

GtkWidget *attach_toggle_with_label(GtkWidget  * table, gint row,
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

GtkWidget *attach_entry_with_label(GtkWidget  * table, gint row,
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

GtkWidget *attach_text_with_label(GtkWidget  * table, gint row,
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

GtkWidget *append_page_table(GtkWidget  * probox, gint rows, gint cols,
			     const char *label_text)
{
	GtkWidget *label;
	GtkWidget *page;
	label = gtk_label_new(label_text);
	page = gtk_table_new(rows, cols, FALSE);
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(probox), page, label);
	return page;
}

/*
 * Set up the lists to use within the combo boxes.
 */
void create_lists(void)
{
	gint c = 0;
	list_ref++;
	/*
	 * Create only if it's the first call.
	 */
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
		     (GCompareFunc) strcmp) == NULL)
			encodings_list =
			    g_list_prepend(encodings_list,
					   (gpointer) languages[c].enc);
		if (g_list_find_custom
		    (group_emails_list, (gpointer) languages[c].group,
		     (GCompareFunc) strcmp) == NULL)
			group_emails_list =
			    g_list_prepend(group_emails_list,
					   (gpointer) languages[c].group);
		if (g_list_find_custom
		    (bits_list, (gpointer) languages[c].bits,
		     (GCompareFunc) strcmp) == NULL)
			bits_list =
			    g_list_prepend(bits_list,
					   (gpointer) languages[c].bits);
		c++;
	}
	/*
	 * Arrange the resulting lists.
	 */
	languages_list = g_list_sort(languages_list, (GCompareFunc) strcoll);
	lcodes_list = g_list_reverse(lcodes_list);
	group_emails_list =
	    g_list_sort(group_emails_list, (GCompareFunc) strcmp);
	encodings_list =
	    g_list_sort(encodings_list, (GCompareFunc) strcmp);
	bits_list = g_list_sort(bits_list, (GCompareFunc) strcmp);
}

void prefs_box(GtkWidget  * widget, gpointer useless)
{
	/*
	 * The internally used variables.
	 */
	GtkWidget 	*fg_color_label,
			*bg_color_label,
			*font_label;
	
	raise_and_return_if_exists(prefs);
	/*
	 * Create the preferences box... 
	 */
	prefs = gnome_property_box_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(
		GNOME_PROPERTY_BOX(prefs)->notebook), GTK_POS_RIGHT);
	gtk_window_set_title(GTK_WINDOW(prefs), _("gtranslator -- options"));
	
	/*
	 * The tables for holding all the entries below.
	 */
	first_page = append_page_table(prefs, 2, 2, _("Personal information"));
	second_page = append_page_table(prefs, 5, 2, _("Language options"));
	third_page = append_page_table(prefs, 3, 1, _("Po file options"));
	fourth_page = append_page_table(prefs, 5, 1, _("Miscellaneous"));
	fifth_page = append_page_table(prefs, 4, 2, _("Recent files & spell checking"));
	sixth_page = append_page_table(prefs, 4, 2, _("Fonts & Colors"));
	
	/*
	 * Create all the personal entries.
	 */
	authors_name =
	    attach_entry_with_label(first_page, 0, _("Author's name :"),
				    author, prefs_box_changed);
	authors_email =
	    attach_entry_with_label(first_page, 1, _("Author's EMail :"),
				    email, prefs_box_changed);
	
	/*
	 * Create, attach, and connect all the combo boxes with labels. 
	 */
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
	/*
	 * Create, attach, and connect the toggle buttons.
	 */
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

	/*
	 * The fourth page with the popup menu & the dot_char.
	 */
	use_dot_char=attach_toggle_with_label(fourth_page, 0,
		_("Use free space indicating special character"),
		wants.dot_char, prefs_box_changed);
	enable_popup_menu=attach_toggle_with_label(fourth_page, 2,
		_("Enable the popup menu"),
		wants.popup_menu, prefs_box_changed);	
	use_update_function=attach_toggle_with_label(fourth_page, 3,
		_("Enable the update function of gtranslator (you need the sources for this)"),
		wants.update_function, prefs_box_changed);
	save_geometry_tb=attach_toggle_with_label(fourth_page, 4,
		_("Save geometry on exit & restore it on startup"),
		wants.save_geometry, prefs_box_changed);
	no_uzis=attach_toggle_with_label(fourth_page, 5,
		_("Don't show the update information dialogs"),
		wants.uzi_dialogs, prefs_box_changed);
	/*
	 * The fifth page with the Recent files options.
	 */
	check_recent_files=attach_toggle_with_label(fifth_page, 0,
		_("Check every recent file before listing it up"),
		wants.check_recent_file, prefs_box_changed);
	instant_spell_checking=attach_toggle_with_label(fifth_page, 1,
		_("Instant spell checking"),
		wants.instant_spell_check, prefs_box_changed);
	use_own_dict=attach_toggle_with_label(fifth_page, 2,
		_("Use special dictionary"),
		wants.use_own_dict, prefs_box_changed);
	dictionary_file=
	    attach_entry_with_label(fifth_page, 3, _("Dictionary to use:"),
				    wants.dictionary, prefs_box_changed);
	
	/*
	 * The sixth page with the special font/color stuff.
	 */
	own_specs=attach_toggle_with_label(sixth_page, 0,
		_("Apply special font/colors"),
		wants.use_own_specs, prefs_box_changed);
	
	/*
	 * The used labels onn the sixth page.
	 */	
	font_label=gtk_label_new(_("Font:"));
	fg_color_label=gtk_label_new(_("Foreground color:"));
	bg_color_label=gtk_label_new(_("Background color:"));
	font=gnome_font_picker_new();
	gnome_font_picker_set_title(GNOME_FONT_PICKER(font),
		_("gtranslator -- font selection"));
	if (wants.font)
		gnome_font_picker_set_font_name(GNOME_FONT_PICKER(font),
						wants.font);
	foreground=gnome_color_picker_new();
	gnome_color_picker_set_title(GNOME_COLOR_PICKER(foreground),
		_("gtranslator -- foreground color"));
	background=gnome_color_picker_new();
	gnome_color_picker_set_title(GNOME_COLOR_PICKER(background),
		_("gtranslator -- background color"));

	gtranslator_config_init();
	gtranslator_color_values_get(GNOME_COLOR_PICKER(foreground),
		COLOR_VALUE_FG);
	gtranslator_color_values_get(GNOME_COLOR_PICKER(background),
		COLOR_VALUE_BG);
	gtranslator_config_close();
	
	/*
	 * Attach all the widgets to the sixth page.
	 */		
	gtk_table_attach_defaults(GTK_TABLE(sixth_page),
		font_label, 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(sixth_page),
		font, 1, 2, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(sixth_page),
		fg_color_label, 0, 1, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(sixth_page),
		foreground, 1, 2, 2, 3);	
	gtk_table_attach_defaults(GTK_TABLE(sixth_page),
		bg_color_label, 0, 1, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(sixth_page),
		background, 1, 2, 3, 4);	
		
	/*
	 * Connect the signals to the preferences box.
	 */
	gtk_signal_connect(GTK_OBJECT(font), "font_set",
			   GTK_SIGNAL_FUNC(prefs_box_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(foreground), "color_set",
			   GTK_SIGNAL_FUNC(prefs_box_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(background), "color_set",
			   GTK_SIGNAL_FUNC(prefs_box_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(prefs), "apply",
			   GTK_SIGNAL_FUNC(prefs_box_apply), NULL);
	gtk_signal_connect(GTK_OBJECT(prefs), "help",
			   GTK_SIGNAL_FUNC(prefs_box_help), NULL);
	gtk_signal_connect(GTK_OBJECT(prefs), "close",
			   GTK_SIGNAL_FUNC(destroy_lists), NULL);
	show_nice_dialog(&prefs, "gtranslator -- prefs");
}

/*
 * The actions to take when the user presses "Apply".
 */
static void prefs_box_apply(GtkWidget  * box, gint page_num, gpointer useless)
{
	/*
	 * We need to apply only once. 
	 */
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
	update(wants.dictionary, dictionary_file);
#undef update
#define if_active(widget) \
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
	wants.save_geometry = if_active(save_geometry_tb);
	wants.unmark_fuzzy = if_active(unmark_fuzzy);
	wants.warn_if_fuzzy = if_active(warn_if_fuzzy);
	wants.warn_if_no_change = if_active(warn_if_no_change);
	wants.dont_save_unchanged_files = if_active(dont_save_unchanged_files);
	wants.dot_char = if_active(use_dot_char);
	wants.update_function = if_active(use_update_function);
	wants.popup_menu = if_active(enable_popup_menu);
	wants.uzi_dialogs = if_active(no_uzis);
	wants.check_recent_file = if_active(check_recent_files);
	wants.instant_spell_check = if_active(instant_spell_checking);
	wants.use_own_specs = if_active(own_specs);
	wants.use_own_dict = if_active(use_own_dict);
#undef if_active
	
	gtranslator_config_init();
	gtranslator_config_set_string("translator/name", author);
	gtranslator_config_set_string("translator/email", email);
	gtranslator_config_set_string("language/name", language);
	gtranslator_config_set_string("language/mime_type", mime);
	gtranslator_config_set_string("language/encoding", enc);
	gtranslator_config_set_string("language/language_code", lc);
	gtranslator_config_set_string("language/team_email", lg);
	gtranslator_config_set_string("dict/file", wants.dictionary);
	
	g_free(wants.font);
	wants.font=gnome_font_picker_get_font_name(GNOME_FONT_PICKER(font));
	gtranslator_config_set_string("font/name", wants.font);
	
	gtranslator_color_values_set(GNOME_COLOR_PICKER(foreground),
		COLOR_VALUE_FG);
	gtranslator_color_values_set(GNOME_COLOR_PICKER(background),
		COLOR_VALUE_BG);
	
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
	gtranslator_config_set_bool("toggles/use_update_function",
			      wants.update_function);
	gtranslator_config_set_bool("toggles/enable_popup_menu",
			      wants.popup_menu);
	gtranslator_config_set_bool("toggles/uzi_dialogs",
			      wants.uzi_dialogs);
	gtranslator_config_set_bool("toggles/check_recent_files",
			      wants.check_recent_file);
	gtranslator_config_set_bool("toggles/instant_spell_check",
			      wants.instant_spell_check);
	gtranslator_config_set_bool("toggles/use_own_specs",
			      wants.use_own_specs);
	gtranslator_config_set_bool("toggles/use_own_dict",
			      wants.use_own_dict);
	gtranslator_config_close();
}

/*
 * The preferences box's help window.
 */
static void prefs_box_help(GtkWidget  * widget, gpointer useless)
{
	gnome_app_message(GNOME_APP(app1), _("\
With the Preferences box you can define some variables\n\
with which you can make gtranslator make more work\n\
like YOU want it to work!"));
}

gboolean destroy_lists(GtkWidget  * widget, gpointer useless)
{
	list_ref--;
	/*
	 * If something needs them, leave.
	 */
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

static void prefs_box_changed(GtkWidget  * widget, gpointer flag)
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
			if (!strcmp(current, languages[c].lcode)) {
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
	gtranslator_config_init();
	author = gtranslator_config_get_string("translator/name");
	email = gtranslator_config_get_string("translator/email");
	language = gtranslator_config_get_string("language/name");
	lc = gtranslator_config_get_string("language/language_code");
	lg = gtranslator_config_get_string("language/team_email");
	mime = gtranslator_config_get_string("language/mime_type");
	enc = gtranslator_config_get_string("language/encoding");
	wants.font = gtranslator_config_get_string("font/name");
	wants.dictionary = gtranslator_config_get_string("dict/file");
	
	wants.instant_spell_check = 
	    gtranslator_config_get_bool("toggles/instant_spell_check");
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
	wants.update_function =
	    gtranslator_config_get_bool("toggles/use_update_function");    
	wants.dot_char = 
	    gtranslator_config_get_bool("toggles/use_dot_char");
	wants.uzi_dialogs =
	    gtranslator_config_get_bool("toggles/uzi_dialogs");
	wants.check_recent_file = 
	    gtranslator_config_get_bool("toggles/check_recent_files");
	wants.use_own_specs =
	    gtranslator_config_get_bool("toggles/use_own_specs");
	wants.use_own_dict =
	    gtranslator_config_get_bool("toggles/use_own_dict");
	wants.match_case = gtranslator_config_get_bool("find/case_sensitive");
	wants.find_in = gtranslator_config_get_int("find/find_in");
	update_flags();
	wants.fill_header = gtranslator_config_get_bool("toggles/fill_header");
	gtranslator_config_close();
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

void restore_geometry(gchar  * gstr)
{
	gint x, y, width, height;
	/*
	 * Set the main window's geometry from the preferences.
	 */
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
	/*
	 * IF a geometry definition had been defined try to parse it.
	 */
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
