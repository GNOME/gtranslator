/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "color-schemes.h"
#include "defines.include"
#include "dialogs.h"
#include "find.h"
#include "gui.h"
#include "languages.h"
#include "messages-table.h"
#include "prefs.h"
#include "query.h"
#include "sidebar.h"
#include "stylistics.h"
#include "utils.h"
#include "utils_gui.h"

#include <libgnome/gnome-util.h>
#include <libgnomeui/libgnomeui.h>

/*
 * The callbacks:
 */
static void gtranslator_preferences_dialog_changed(GtkWidget  * widget, gpointer useless);
static void gtranslator_preferences_dialog_apply(GtkWidget  * widget, gint page_num,
			    gpointer useless);
static void gtranslator_preferences_dialog_help(GtkWidget  * widget, gpointer useless);

/*
 * The notebook page widgets: 
 */
static GtkWidget 
	*first_page, *second_page, *third_page, *fourth_page,
	*fifth_page, *sixth_page, *seventh_page;

/*
 * The entries:
 */
static GtkWidget
	*authors_name, *authors_email, *authors_language,
	*mime_type, *encoding, *lcode, *lg_email, *dictionary_file,
	*scheme_file, *autosave_suffix;

/*
 * The toggle buttons used in the preferences box:
 */
static GtkWidget
	*warn_if_no_change, *warn_if_fuzzy, *unmark_fuzzy,
	*dont_save_unchanged_files, *save_geometry_tb, *show_sidebar,
	*enable_popup_menu, *use_dot_char, *use_update_function,
	*check_recent_files, *own_fonts, *own_colors, *use_own_dict,
	*instant_spell_checking, *keep_obsolete, *defaultdomain,
	*autosave, *autosave_with_suffix, *sweep_compile_file;

/*
 * The timeout GtkSpinButton:
 */
static GtkWidget
	*autosave_timeout, *max_history_entries;

/*
 * Font/color specific widgets used in the preferences box.
 */
static GtkWidget *foreground, *background, *msgid_font, *msgstr_font;

/*
 * The preferences dialog widget itself.
 */
static GtkWidget *prefs = NULL;

void gtranslator_preferences_dialog_create(GtkWidget  *widget, gpointer useless)
{
	GList 	*colorschemeslist=NULL;
	gchar	*old_colorscheme;
	
	gtranslator_raise_dialog(prefs);
	
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
	first_page = gtranslator_utils_append_page_to_preferences_dialog(prefs, 3, 2, 
		_("Personal informations"));
	second_page = gtranslator_utils_append_page_to_preferences_dialog(prefs, 5, 2, 
		_("Language settings"));
	third_page = gtranslator_utils_append_page_to_preferences_dialog(prefs, 4, 1, 
		_("Po file editing"));
	fourth_page = gtranslator_utils_append_page_to_preferences_dialog(prefs, 7, 1, 
		_("Miscellaneous"));
	fifth_page = gtranslator_utils_append_page_to_preferences_dialog(prefs, 4, 2, 
		_("Recent files & spell checking"));
	sixth_page = gtranslator_utils_append_page_to_preferences_dialog(prefs, 6, 2, 
		_("Fonts, colors and color schemes"));
	seventh_page = gtranslator_utils_append_page_to_preferences_dialog(prefs, 4, 2, 
		_("Autosaving"));
	
	/*
	 * Create all the personal entries.
	 */
	authors_name =
	    gtranslator_utils_attach_entry_with_label(first_page, 0, _("Author's name:"),
				    author, gtranslator_preferences_dialog_changed);
	authors_email =
	    gtranslator_utils_attach_entry_with_label(first_page, 1, _("Author's EMail:"),
				    email, gtranslator_preferences_dialog_changed);

	/*
	 * If no domainslist could be got from the startup routines, build up a foo'sh list.
	 */
	if(!domains)
	{
		domains=g_list_prepend(domains, g_strdup(" "));
		domains=g_list_reverse(domains);
	}

	defaultdomain =
	    gtranslator_utils_attach_combo_with_label(first_page, 2, _("Default query domain:"),
			    	    domains, GtrPreferences.defaultdomain,
				    gtranslator_preferences_dialog_changed, NULL);
	
	/*
	 * Create, attach, and connect all the combo boxes with labels. 
	 */
	gtranslator_utils_language_lists_create();

	authors_language =
	    gtranslator_utils_attach_combo_with_label(second_page, 0, _("Language:"),
				    languages_list, language,
				    gtranslator_preferences_dialog_changed, GINT_TO_POINTER(1));
	lcode =
	    gtranslator_utils_attach_combo_with_label(second_page, 1, _("Language code:"),
				    lcodes_list, lc,
				    gtranslator_preferences_dialog_changed, GINT_TO_POINTER(2));
	lg_email =
	    gtranslator_utils_attach_combo_with_label(second_page, 2,
				    _("Language group's EMail:"),
				    group_emails_list, lg,
				    gtranslator_preferences_dialog_changed, NULL);
	mime_type =
	    gtranslator_utils_attach_combo_with_label(second_page, 3, _("Charset:"),
				    encodings_list, mime,
				    gtranslator_preferences_dialog_changed, NULL);
	encoding =
	    gtranslator_utils_attach_combo_with_label(second_page, 4, _("Encoding:"),
				    bits_list, enc,
				    gtranslator_preferences_dialog_changed, NULL);
	/*
	 * Create, attach, and connect the toggle buttons.
	 */
	unmark_fuzzy =
	    gtranslator_utils_attach_toggle_with_label(third_page, 0,
		_("Set non-fuzzy status, if message was changed"),
		GtrPreferences.unmark_fuzzy, gtranslator_preferences_dialog_changed);
	warn_if_fuzzy =
	    gtranslator_utils_attach_toggle_with_label(third_page, 1,
		_("Warn if the po file contains fuzzy translations"),
		GtrPreferences.warn_if_fuzzy, gtranslator_preferences_dialog_changed);
	dont_save_unchanged_files =
	    gtranslator_utils_attach_toggle_with_label(third_page, 2,
		_("Don't save unchanged po files"),
		GtrPreferences.dont_save_unchanged_files, gtranslator_preferences_dialog_changed);
	warn_if_no_change =
	    gtranslator_utils_attach_toggle_with_label(third_page, 3,
		_("Warn me if I'm trying to save an unchanged file"),
		GtrPreferences.warn_if_no_change, gtranslator_preferences_dialog_changed);
	keep_obsolete =
	    gtranslator_utils_attach_toggle_with_label(third_page, 4,
		_("Keep obsolete message in the po files"),
		GtrPreferences.keep_obsolete, gtranslator_preferences_dialog_changed);

	/*
	 * The fourth page with the popup menu & the dot_char.
	 */
	use_dot_char=gtranslator_utils_attach_toggle_with_label(fourth_page, 0,
		_("Use free space indicating special character"),
		GtrPreferences.dot_char, gtranslator_preferences_dialog_changed);
	enable_popup_menu=gtranslator_utils_attach_toggle_with_label(fourth_page, 2,
		_("Enable the popup menu"),
		GtrPreferences.popup_menu, gtranslator_preferences_dialog_changed);	
	use_update_function=gtranslator_utils_attach_toggle_with_label(fourth_page, 3,
		_("Enable the update function of gtranslator (you need the sources for this)"),
		GtrPreferences.update_function, gtranslator_preferences_dialog_changed);
	sweep_compile_file=gtranslator_utils_attach_toggle_with_label(fourth_page, 4,
		_("Delete the compile result file (named \"project-language_code.po\")"),
		GtrPreferences.sweep_compile_file, gtranslator_preferences_dialog_changed);
	save_geometry_tb=gtranslator_utils_attach_toggle_with_label(fourth_page, 5,
		_("Save geometry on exit & restore it on startup"),
		GtrPreferences.save_geometry, gtranslator_preferences_dialog_changed);
	show_sidebar=gtranslator_utils_attach_toggle_with_label(fourth_page, 6,
		_("Show the views sidebar"),
		GtrPreferences.show_sidebar, gtranslator_preferences_dialog_changed);
	
	/*
	 * The fifth page with the Recent files options.
	 */
	max_history_entries=gtranslator_utils_attach_spin_with_label(fifth_page, 0, 
		_("Maximally showed recent files:"), 3, 15, 
		GtrPreferences.max_history_entries, gtranslator_preferences_dialog_changed);
	
	check_recent_files=gtranslator_utils_attach_toggle_with_label(fifth_page, 1,
		_("Check every recent file before listing it up"),
		GtrPreferences.check_recent_file, gtranslator_preferences_dialog_changed);
	instant_spell_checking=gtranslator_utils_attach_toggle_with_label(fifth_page, 2,
		_("Instant spell checking"),
		GtrPreferences.instant_spell_check, gtranslator_preferences_dialog_changed);
	use_own_dict=gtranslator_utils_attach_toggle_with_label(fifth_page, 3,
		_("Use special dictionary"),
		GtrPreferences.use_own_dict, gtranslator_preferences_dialog_changed);
	dictionary_file=
	    gtranslator_utils_attach_entry_with_label(fifth_page, 4, _("Dictionary to use:"),
				    GtrPreferences.dictionary, gtranslator_preferences_dialog_changed);
	
	/*
	 * The sixth page with the special font/color stuff.
	 */
	colorschemeslist=gtranslator_utils_file_names_from_directory(SCHEMESDIR,
		".xml", TRUE, TRUE, FALSE);
	old_colorscheme=gtranslator_utils_get_raw_file_name(GtrPreferences.scheme);
	
	scheme_file=gtranslator_utils_attach_combo_with_label(sixth_page, 0,
		_("Syntax color scheme to use:"), colorschemeslist, old_colorscheme,
		gtranslator_preferences_dialog_changed, NULL);
	 
	own_fonts=gtranslator_utils_attach_toggle_with_label(sixth_page, 1,
		_("Apply own fonts:"),
		GtrPreferences.use_own_fonts, gtranslator_preferences_dialog_changed);

	msgid_font=gtranslator_utils_attach_font_with_label(sixth_page, 2,
		_("Original text font:"), _("gtranslator -- font selection/msgid font"),
		GtrPreferences.msgid_font, gtranslator_preferences_dialog_changed);
	
	msgstr_font=gtranslator_utils_attach_font_with_label(sixth_page, 3,
		_("Translation font:"), _("gtranslator -- font selection/msgstr font"),
		GtrPreferences.msgstr_font, gtranslator_preferences_dialog_changed);

	own_colors=gtranslator_utils_attach_toggle_with_label(sixth_page, 4,
		_("Apply own colors:"),
		GtrPreferences.use_own_colors, gtranslator_preferences_dialog_changed);

	foreground=gtranslator_utils_attach_color_with_label(sixth_page, 5,
		_("Foreground color:"), _("gtranslator -- foreground color"),
		COLOR_FG, gtranslator_preferences_dialog_changed);

	background=gtranslator_utils_attach_color_with_label(sixth_page, 6,
		_("Background color:"), _("gtranslator -- background color"),
		COLOR_BG, gtranslator_preferences_dialog_changed);

	/*
	 * The seventh page of the prefs-box: autosaving options.
	 */
	autosave=gtranslator_utils_attach_toggle_with_label(seventh_page, 0,
		_("Save the po file automatically after a period of time"),
		GtrPreferences.autosave, gtranslator_preferences_dialog_changed);
	
	autosave_with_suffix=gtranslator_utils_attach_toggle_with_label(seventh_page, 1,
		_("Append a special suffix to the automatically saved files"),
		GtrPreferences.autosave_with_suffix, gtranslator_preferences_dialog_changed);
	
	autosave_suffix=gtranslator_utils_attach_entry_with_label(seventh_page, 2,
		_("Autosave suffix:"),
		GtrPreferences.autosave_suffix, gtranslator_preferences_dialog_changed);

	autosave_timeout=gtranslator_utils_attach_spin_with_label(seventh_page, 3,
		_("Autosave timeout in minutes:"), 1.0, 30.0,
		GtrPreferences.autosave_timeout, gtranslator_preferences_dialog_changed);

	/*
	 * Connect the signals to the preferences box.
	 */
	gtk_signal_connect(GTK_OBJECT(prefs), "apply",
			   GTK_SIGNAL_FUNC(gtranslator_preferences_dialog_apply), NULL);
	gtk_signal_connect(GTK_OBJECT(prefs), "help",
			   GTK_SIGNAL_FUNC(gtranslator_preferences_dialog_help), NULL);
	gtk_signal_connect(GTK_OBJECT(prefs), "close",
			   GTK_SIGNAL_FUNC(gtranslator_utils_language_lists_free), NULL);
	gtranslator_dialog_show(&prefs, "gtranslator -- prefs");
}

/*
 * The actions to take when the user presses "Apply".
 */
static void gtranslator_preferences_dialog_apply(GtkWidget  * box, gint page_num, gpointer useless)
{
	gchar	*selected_scheme_file;
	
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
	update(GtrPreferences.defaultdomain, GTK_COMBO(defaultdomain)->entry);
	update(GtrPreferences.dictionary, dictionary_file);
	update(GtrPreferences.autosave_suffix, autosave_suffix);
#undef update
#define if_active(widget) \
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
	GtrPreferences.save_geometry = if_active(save_geometry_tb);
	GtrPreferences.unmark_fuzzy = if_active(unmark_fuzzy);
	GtrPreferences.warn_if_fuzzy = if_active(warn_if_fuzzy);
	GtrPreferences.warn_if_no_change = if_active(warn_if_no_change);
	GtrPreferences.dont_save_unchanged_files = if_active(dont_save_unchanged_files);
	GtrPreferences.dot_char = if_active(use_dot_char);
	GtrPreferences.update_function = if_active(use_update_function);
	GtrPreferences.popup_menu = if_active(enable_popup_menu);
	GtrPreferences.sweep_compile_file = if_active(sweep_compile_file);
	GtrPreferences.show_sidebar = if_active(show_sidebar);
	GtrPreferences.check_recent_file = if_active(check_recent_files);
	GtrPreferences.instant_spell_check = if_active(instant_spell_checking);
	GtrPreferences.use_own_fonts = if_active(own_fonts);
	GtrPreferences.use_own_colors = if_active(own_colors);
	GtrPreferences.use_own_dict = if_active(use_own_dict);
	GtrPreferences.keep_obsolete = if_active(keep_obsolete);
	GtrPreferences.autosave = if_active(autosave);
	GtrPreferences.autosave_with_suffix = if_active(autosave_with_suffix);
#undef if_active
	
	/*
	 * Read out the SpinButton's.
	 */
	GtrPreferences.autosave_timeout = 
		gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(
			autosave_timeout));
		
	GtrPreferences.max_history_entries =
		gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(
			max_history_entries));
	
	gtranslator_config_init();
	gtranslator_config_set_string("translator/name", author);
	gtranslator_config_set_string("translator/email", email);
	gtranslator_config_set_string("query/defaultdomain", 
		GtrPreferences.defaultdomain);
	gtranslator_config_set_string("language/name", language);
	gtranslator_config_set_string("language/mime_type", mime);
	gtranslator_config_set_string("language/encoding", enc);
	gtranslator_config_set_string("language/language_code", lc);
	gtranslator_config_set_string("language/team_email", lg);
	gtranslator_config_set_string("dict/file", GtrPreferences.dictionary);
	gtranslator_config_set_string("informations/autosave_suffix", 
		GtrPreferences.autosave_suffix);
	
	gtranslator_config_set_float("informations/autosave_timeout", 
		GtrPreferences.autosave_timeout);

	gtranslator_config_set_float("informations/max_history_entries", 
		GtrPreferences.max_history_entries);
	
	g_free(GtrPreferences.msgid_font);
	g_free(GtrPreferences.msgstr_font);
	
	GtrPreferences.msgid_font=g_strdup(
		gnome_font_picker_get_font_name(
			GNOME_FONT_PICKER(msgid_font)));
	
	GtrPreferences.msgstr_font=g_strdup(
		gnome_font_picker_get_font_name(
			GNOME_FONT_PICKER(msgstr_font)));
	
	gtranslator_config_set_string("interface/original_font",
		GtrPreferences.msgid_font);
	gtranslator_config_set_string("interface/translation_font",
		GtrPreferences.msgstr_font);

	/*
	 * Apply the given color scheme.
	 */ 
	g_free(GtrPreferences.scheme);
	
	selected_scheme_file=gtk_editable_get_chars(GTK_EDITABLE(
		GTK_COMBO(scheme_file)->entry), 0, -1);

	GtrPreferences.scheme=g_strdup_printf("%s/%s.xml", SCHEMESDIR,
		selected_scheme_file);

	if(g_file_exists(GtrPreferences.scheme))
	{
		/*
		 * Free the old used colorscheme.
		 */
		gtranslator_color_scheme_free(&theme);
		
		/*
		 * Read in the new colorscheme and initialize the colors.
		 */
		gtranslator_color_scheme_apply(GtrPreferences.scheme);
		theme=gtranslator_color_scheme_load_from_prefs();
		
		gtranslator_colors_initialize();
	}
	
	gtranslator_color_values_set(GNOME_COLOR_PICKER(foreground), COLOR_FG);
	gtranslator_color_values_set(GNOME_COLOR_PICKER(background), COLOR_BG);

	gtranslator_set_style(text_box, 0);
	gtranslator_set_style(trans_box, 1);

	gtranslator_config_set_bool("toggles/save_geometry", GtrPreferences.save_geometry);
	gtranslator_config_set_bool("toggles/warn_if_fuzzy", GtrPreferences.warn_if_fuzzy);
	gtranslator_config_set_bool("toggles/set_non_fuzzy_if_changed", 
			      GtrPreferences.unmark_fuzzy);
	gtranslator_config_set_bool("toggles/warn_if_no_change",
			      GtrPreferences.warn_if_no_change);
	gtranslator_config_set_bool("toggles/do_not_save_unchanged_files",
			      GtrPreferences.dont_save_unchanged_files);
	gtranslator_config_set_bool("toggles/use_dot_char",
			      GtrPreferences.dot_char);
	gtranslator_config_set_bool("toggles/use_update_function",
			      GtrPreferences.update_function);
	gtranslator_config_set_bool("toggles/sweep_compile_file",
			      GtrPreferences.sweep_compile_file);
	gtranslator_config_set_bool("toggles/enable_popup_menu",
			      GtrPreferences.popup_menu);
	gtranslator_config_set_bool("toggles/show_sidebar",
			      GtrPreferences.show_sidebar);
	gtranslator_config_set_bool("toggles/check_recent_files",
			      GtrPreferences.check_recent_file);
	gtranslator_config_set_bool("toggles/instant_spell_check",
			      GtrPreferences.instant_spell_check);
	gtranslator_config_set_bool("toggles/use_own_fonts",
			      GtrPreferences.use_own_fonts);
	gtranslator_config_set_bool("toggles/use_own_colors",
			      GtrPreferences.use_own_colors);
	gtranslator_config_set_bool("toggles/use_own_dict",
			      GtrPreferences.use_own_dict);
	gtranslator_config_set_bool("toggles/keep_obsolete",
			      GtrPreferences.keep_obsolete);
	gtranslator_config_set_bool("toggles/autosave",
			      GtrPreferences.autosave);
	gtranslator_config_set_bool("toggles/autosave_with_suffix",
			      GtrPreferences.autosave_with_suffix);

	gtranslator_config_close();

	/*
	 * Show or hide the views sidebar/content pane according to the preferences.
	 */
	if(GtrPreferences.show_sidebar)
	{
		gtranslator_sidebar_show();
	}
	else
	{
		gtranslator_sidebar_hide();
	}
}

/*
 * The preferences box's help window.
 */
static void gtranslator_preferences_dialog_help(GtkWidget  * widget, gpointer useless)
{
	gnome_app_message(GNOME_APP(gtranslator_application), _("\
With the Preferences box you can define some variables\n\
with which you can make gtranslator make more work\n\
like YOU want it to work!"));
}

static void gtranslator_preferences_dialog_changed(GtkWidget  * widget, gpointer flag)
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

void gtranslator_preferences_read(void)
{
	/*
	 * Initialize the preferences with default values if this is our first
	 *  startup of gtranslator.
	 */  
	gtranslator_preferences_init_default_values();

	gtranslator_config_init();
	author = gtranslator_config_get_string("translator/name");
	email = gtranslator_config_get_string("translator/email");
	GtrPreferences.defaultdomain = gtranslator_config_get_string("query/defaultdomain");
	language = gtranslator_config_get_string("language/name");
	lc = gtranslator_config_get_string("language/language_code");
	lg = gtranslator_config_get_string("language/team_email");
	mime = gtranslator_config_get_string("language/mime_type");
	enc = gtranslator_config_get_string("language/encoding");
	
	GtrPreferences.msgid_font = 
		gtranslator_config_get_string("interface/original_font");
	GtrPreferences.msgstr_font = 
		gtranslator_config_get_string("interface/translation_font");
	
	GtrPreferences.dictionary = gtranslator_config_get_string("dict/file");
	GtrPreferences.scheme =  gtranslator_config_get_string("scheme/filename");
	
	GtrPreferences.autosave =
		gtranslator_config_get_bool("toggles/autosave");
	GtrPreferences.autosave_timeout = 
		gtranslator_config_get_float("informations/autosave_timeout");
	GtrPreferences.autosave_with_suffix =
		gtranslator_config_get_bool("toggles/autosave_with_suffix");
	GtrPreferences.autosave_suffix =
		gtranslator_config_get_string("informations/autosave_suffix");

	GtrPreferences.max_history_entries =
		gtranslator_config_get_float("informations/max_history_entries");

	GtrPreferences.instant_spell_check = 
		gtranslator_config_get_bool("toggles/instant_spell_check");
	GtrPreferences.save_geometry =
		gtranslator_config_get_bool("toggles/save_geometry");
	GtrPreferences.unmark_fuzzy =
		gtranslator_config_get_bool("toggles/set_non_fuzzy_if_changed"); 
	GtrPreferences.warn_if_fuzzy =
		gtranslator_config_get_bool("toggles/warn_if_fuzzy");
	GtrPreferences.warn_if_no_change =
		gtranslator_config_get_bool("toggles/warn_if_no_change");
	GtrPreferences.dont_save_unchanged_files =
		gtranslator_config_get_bool("toggles/do_not_save_unchanged_files");
	GtrPreferences.popup_menu =
		gtranslator_config_get_bool("toggles/enable_popup_menu");
	GtrPreferences.update_function =
		gtranslator_config_get_bool("toggles/use_update_function");
	GtrPreferences.sweep_compile_file =
		gtranslator_config_get_bool("toggles/sweep_compile_file");
	GtrPreferences.dot_char = 
		gtranslator_config_get_bool("toggles/use_dot_char");
	GtrPreferences.check_recent_file = 
		gtranslator_config_get_bool("toggles/check_recent_files");
	GtrPreferences.use_own_fonts =
		gtranslator_config_get_bool("toggles/use_own_fonts");
	GtrPreferences.use_own_colors =
		gtranslator_config_get_bool("toggles/use_own_colors");
	GtrPreferences.use_own_dict =
		gtranslator_config_get_bool("toggles/use_own_dict");
	GtrPreferences.keep_obsolete =
		gtranslator_config_get_bool("toggles/keep_obsolete");

	GtrPreferences.match_case = gtranslator_config_get_bool("find/case_sensitive");
	GtrPreferences.find_in = gtranslator_config_get_int("find/find_in");
	gtranslator_update_regex_flags();
	
	GtrPreferences.fill_header = gtranslator_config_get_bool("toggles/fill_header");
	GtrPreferences.show_sidebar = gtranslator_config_get_bool("toggles/show_sidebar");

	/*
	 * Check if we'd to use special styles.
	 */
	if(GtrPreferences.use_own_fonts || GtrPreferences.use_own_colors)
	{
		/*
		 * Set the own specs for colors and for the font.
		 */
		gtranslator_set_style(text_box, 0);
		gtranslator_set_style(trans_box, 1);
	}

	gtranslator_config_close();
}

void gtranslator_preferences_free(void)
{
	g_free(author);
	g_free(email);
	g_free(language);
	g_free(mime);
	g_free(enc);
	g_free(lg);
	g_free(lc);
}
