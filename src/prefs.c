/*
 * (C) 2000-2002 	Fatih Demir <kabalak@gtranslator.org>
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
#include "nautilus-string.h"
#include "prefs.h"
#include "stylistics.h"
#include "translator.h"
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
static void toggle_sensitive(GtkWidget *widget, gpointer data);

/*
 * The notebook page widgets: 
 */
static GtkWidget 
	*first_page, *second_page, *third_page, *fourth_page,
	*fifth_page, *sixth_page, *seventh_page, *eighth_page,
	*ninth_page;

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
	*warn_if_fuzzy, *unmark_fuzzy, *save_geometry_tb,
	*enable_popup_menu, *use_dot_char, *use_update_function,
	*check_recent_files, *own_fonts, *own_colors, *use_own_dict,
	*instant_spell_checking, *keep_obsolete, *autosave,
	*autosave_with_suffix, *sweep_compile_file, *use_learn_buffer,
	*show_messages_table, *rambo_function, *use_own_mt_colors,
	*collapse_translated_entries, *auto_learn, *fuzzy_matching,
	*show_comment, *highlight, *check_formats;

/*
 * The timeout GtkSpinButton:
 */
static GtkWidget
	*autosave_timeout, *max_history_entries, *min_match_percentage;

/*
 * Font/color specific widgets used in the preferences box.
 */
static GtkWidget *foreground, *background, *msgid_font, *msgstr_font,
	*mt_untranslated, *mt_fuzzy, *mt_translated;

/*
 * The preferences dialog widget itself.
 */
static GtkWidget *prefs = NULL;

void gtranslator_preferences_dialog_create(GtkWidget  *widget, gpointer useless)
{
	gchar	*old_colorscheme=NULL;
	GtkObject *adjustment;
	
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
	first_page = gtranslator_utils_append_page_to_preferences_dialog(prefs, 
		2, 2, _("Personal information")); 
	second_page = gtranslator_utils_append_page_to_preferences_dialog(prefs,
		5, 2, _("Language settings"));
	third_page = gtranslator_utils_append_page_to_preferences_dialog(prefs,
		5, 1, _("Po file editing"));
	fourth_page = gtranslator_utils_append_page_to_preferences_dialog(prefs,
		9, 1, _("Miscellaneous"));
	fifth_page = gtranslator_utils_append_page_to_preferences_dialog(prefs,
		3, 2, _("Recent files & spell checking"));
	sixth_page = gtranslator_utils_append_page_to_preferences_dialog(prefs,
		6, 2, _("Fonts, colors and color schemes"));
	seventh_page = gtranslator_utils_append_page_to_preferences_dialog(prefs,
		2, 2, _("Autosaving"));
	eighth_page = gtranslator_utils_append_page_to_preferences_dialog(prefs,
		6, 2, _("Messages table"));
	ninth_page = gtranslator_utils_append_page_to_preferences_dialog(prefs,
		4, 2, _("Learn buffer & autotranslation"));
	
	/*
	 * Create all the personal entries.
	 */
	authors_name =
	    gtranslator_utils_attach_entry_with_label(first_page, 0, _("Author's name:"),
		gtranslator_translator->name, gtranslator_preferences_dialog_changed);
	authors_email =
	    gtranslator_utils_attach_entry_with_label(first_page, 1, _("Author's EMail:"),
		gtranslator_translator->email, gtranslator_preferences_dialog_changed);

	/*
	 * Create, attach, and connect all the combo boxes with labels. 
	 */
	gtranslator_utils_language_lists_create();

	authors_language =
	    gtranslator_utils_attach_combo_with_label(second_page, 0, _("Language:"),
				    languages_list, gtranslator_translator->language->name,
				    FALSE,
				    gtranslator_preferences_dialog_changed, GINT_TO_POINTER(1));
	lcode =
	    gtranslator_utils_attach_combo_with_label(second_page, 1, _("Language code:"),
				    lcodes_list, gtranslator_translator->language->locale,
				    FALSE,
				    gtranslator_preferences_dialog_changed, GINT_TO_POINTER(2));
	lg_email =
	    gtranslator_utils_attach_combo_with_label(second_page, 2,
				    _("Language group's EMail:"),
				    group_emails_list, gtranslator_translator->language->group_email,
				    TRUE,
				    gtranslator_preferences_dialog_changed, NULL);
	mime_type =
	    gtranslator_utils_attach_combo_with_label(second_page, 3, _("Charset:"),
				    encodings_list, gtranslator_translator->language->encoding,
				    FALSE,
				    gtranslator_preferences_dialog_changed, NULL);
	encoding =
	    gtranslator_utils_attach_combo_with_label(second_page, 4, _("Encoding:"),
				    bits_list, gtranslator_translator->language->bits,
				    FALSE,
				    gtranslator_preferences_dialog_changed, NULL);
	/*
	 * Create, attach, and connect the toggle buttons.
	 */
	unmark_fuzzy =
	    gtranslator_utils_attach_toggle_with_label(third_page, 0,
		_("Remove fuzzy status if message changed"),
		GtrPreferences.unmark_fuzzy, gtranslator_preferences_dialog_changed);
	warn_if_fuzzy =
	    gtranslator_utils_attach_toggle_with_label(third_page, 1,
		_("Warn if the po file contains fuzzy translations"),
		GtrPreferences.warn_if_fuzzy, gtranslator_preferences_dialog_changed);
	keep_obsolete =
	    gtranslator_utils_attach_toggle_with_label(third_page, 2,
		_("Keep obsolete messages in the po files"),
		GtrPreferences.keep_obsolete, gtranslator_preferences_dialog_changed);
	highlight =
	    gtranslator_utils_attach_toggle_with_label(third_page, 3,
	    	_("Highlight syntax of the translation messages"),
		GtrPreferences.highlight, gtranslator_preferences_dialog_changed);

	check_formats =
	    gtranslator_utils_attach_toggle_with_label(third_page, 4,
	    	_("Check messages for syntactical correctness"),
		GtrPreferences.check_formats, gtranslator_preferences_dialog_changed);

	/*
	 * The fourth page with the popup menu & the dot_char.
	 */
	use_dot_char=gtranslator_utils_attach_toggle_with_label(fourth_page, 0,
		_("Use special character to indicate free space"),
		GtrPreferences.dot_char, gtranslator_preferences_dialog_changed);
	enable_popup_menu=gtranslator_utils_attach_toggle_with_label(fourth_page, 2,
		_("Enable the popup menu"),
		GtrPreferences.popup_menu, gtranslator_preferences_dialog_changed);	
	use_update_function=gtranslator_utils_attach_toggle_with_label(fourth_page, 3,
		_("Enable the functionality to update a po file from within gtranslator"),
		GtrPreferences.update_function, gtranslator_preferences_dialog_changed);
	rambo_function=gtranslator_utils_attach_toggle_with_label(fourth_page, 4,
		_("Enable the functionality to remove all translations from a po file"),
		GtrPreferences.rambo_function, gtranslator_preferences_dialog_changed);
	sweep_compile_file=gtranslator_utils_attach_toggle_with_label(fourth_page,
		5, _("Delete compiled files (e.g. \"project.gmo\")"),
		GtrPreferences.sweep_compile_file, gtranslator_preferences_dialog_changed);
	save_geometry_tb=gtranslator_utils_attach_toggle_with_label(fourth_page,
		6, _("Save geometry on exit & restore it on startup"),
		GtrPreferences.save_geometry, gtranslator_preferences_dialog_changed);
	show_comment=gtranslator_utils_attach_toggle_with_label(fourth_page,
		7, _("Show instant comment view in the main pane"),
		GtrPreferences.show_comment, gtranslator_preferences_dialog_changed);
	
	/*
	 * The fifth page with the Recent files options.
	 */
	max_history_entries=gtranslator_utils_attach_spin_with_label(fifth_page, 0, 
		_("Maximum number of entries in the recent files' list:"), 3, 15, 
		GtrPreferences.max_history_entries, gtranslator_preferences_dialog_changed);
	
	check_recent_files=gtranslator_utils_attach_toggle_with_label(fifth_page, 1,
		_("Check recent files before showing in recent files' list"),
		GtrPreferences.check_recent_file, gtranslator_preferences_dialog_changed);
	instant_spell_checking=gtranslator_utils_attach_toggle_with_label(fifth_page, 2,
		_("Instant spell checking"),
		GtrPreferences.instant_spell_check, gtranslator_preferences_dialog_changed);
	
	use_own_dict = gtk_check_button_new_with_label(_("Use special dictionary:"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(use_own_dict),
	                             GtrPreferences.use_own_dict);
	gtk_table_attach_defaults(GTK_TABLE(fifth_page), use_own_dict, 0, 1, 3, 4);
	
	dictionary_file = gtk_entry_new();
	if (GtrPreferences.dictionary)
		gtk_entry_set_text(GTK_ENTRY(dictionary_file), GtrPreferences.dictionary);
	gtk_widget_set_sensitive(GTK_WIDGET(dictionary_file), GtrPreferences.use_own_dict);
	gtk_table_attach_defaults(GTK_TABLE(fifth_page), dictionary_file, 1, 2, 3, 4);
	
	gtk_signal_connect(GTK_OBJECT(use_own_dict), "toggled",
			   GTK_SIGNAL_FUNC(toggle_sensitive), dictionary_file);
	gtk_signal_connect(GTK_OBJECT(dictionary_file), "changed",
			   GTK_SIGNAL_FUNC(gtranslator_preferences_dialog_changed), NULL);
	
	/*
	 * The sixth page with the special font/color stuff.
	 */
	old_colorscheme=gtranslator_utils_get_raw_file_name(GtrPreferences.scheme);
	
	scheme_file=gtranslator_utils_attach_combo_with_label(sixth_page, 0,
		_("Syntax color scheme to use:"), colorschemes, old_colorscheme,
		FALSE,
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
	autosave = gtk_check_button_new_with_label(_("Automatically save po files at regular intervals (in minutes):"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(autosave),
	                             GtrPreferences.autosave);
	gtk_table_attach_defaults(GTK_TABLE(seventh_page), autosave, 0, 1, 0, 1);

	adjustment=gtk_adjustment_new(GtrPreferences.autosave_timeout, 1.0, 30.0, 1.0, 1.0, 1.0);	
	autosave_timeout=gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_widget_set_sensitive(GTK_WIDGET(autosave_timeout), GtrPreferences.autosave);
	gtk_table_attach_defaults(GTK_TABLE(seventh_page), autosave_timeout, 1, 2, 0, 1);
	
	gtk_signal_connect(GTK_OBJECT(autosave), "toggled",
			   GTK_SIGNAL_FUNC(toggle_sensitive), autosave_timeout);
	gtk_signal_connect(GTK_OBJECT(autosave_timeout), "changed",
			   GTK_SIGNAL_FUNC(gtranslator_preferences_dialog_changed), NULL);

	autosave_with_suffix = gtk_check_button_new_with_label(_("Append a suffix to automatically saved files:"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(autosave_with_suffix),
	                             GtrPreferences.autosave_with_suffix);
	gtk_table_attach_defaults(GTK_TABLE(seventh_page), autosave_with_suffix, 0, 1, 1, 2);

	autosave_suffix = gtk_entry_new();
	if (GtrPreferences.autosave_suffix)
		gtk_entry_set_text(GTK_ENTRY(autosave_suffix), GtrPreferences.autosave_suffix);
	gtk_widget_set_sensitive(GTK_WIDGET(autosave_suffix), GtrPreferences.autosave_with_suffix);
	gtk_table_attach_defaults(GTK_TABLE(seventh_page), autosave_suffix, 1, 2, 1, 2);
	
	gtk_signal_connect(GTK_OBJECT(autosave_with_suffix), "toggled",
			   GTK_SIGNAL_FUNC(toggle_sensitive), autosave_suffix);
	gtk_signal_connect(GTK_OBJECT(autosave_suffix), "changed",
			   GTK_SIGNAL_FUNC(gtranslator_preferences_dialog_changed), NULL);

	/*
	 * The eighth page with the messages table concerning settings.
	 */
	show_messages_table=gtranslator_utils_attach_toggle_with_label(eighth_page, 0,
		_("Show the messages table (requires gtranslator restart)"),
		GtrPreferences.show_messages_table, gtranslator_preferences_dialog_changed);

	collapse_translated_entries=gtranslator_utils_attach_toggle_with_label(eighth_page, 1,
		_("Collapse translated entries by default"),
		GtrPreferences.collapse_translated, gtranslator_preferences_dialog_changed);

	use_own_mt_colors=gtranslator_utils_attach_toggle_with_label(eighth_page, 2,
		_("Use own colors for the messages table groups"),
		GtrPreferences.use_own_mt_colors, gtranslator_preferences_dialog_changed);

	mt_untranslated=gtranslator_utils_attach_color_with_label(eighth_page, 3,
		_("Untranslated entries color:"), _("gtranslator -- untranslated entries' color"),
		COLOR_MESSAGES_TABLE_UNTRANSLATED, gtranslator_preferences_dialog_changed);

	mt_fuzzy=gtranslator_utils_attach_color_with_label(eighth_page, 4,
		_("Fuzzy entries color:"), _("gtranslator -- fuzzy entries' color"),
		COLOR_MESSAGES_TABLE_FUZZY, gtranslator_preferences_dialog_changed);

	mt_translated=gtranslator_utils_attach_color_with_label(eighth_page, 5,
		_("Translated entries color:"), _("gtranslator -- translated entries' color"),
		COLOR_MESSAGES_TABLE_TRANSLATED, gtranslator_preferences_dialog_changed);

	/*
	 * Learn buffer/auto translation options go now into their own prefs page ,-)
	 */
	use_learn_buffer =
	    gtranslator_utils_attach_toggle_with_label(ninth_page, 0,
	    	_("Also query the personal learn buffer while autotranslating untranslated messages"),
		GtrPreferences.use_learn_buffer, gtranslator_preferences_dialog_changed);
    	auto_learn =
	    gtranslator_utils_attach_toggle_with_label(ninth_page, 1,
	    	_("Automatically learn a newly translated message"),
		GtrPreferences.auto_learn, gtranslator_preferences_dialog_changed);
	fuzzy_matching=gtranslator_utils_attach_toggle_with_label(ninth_page,
		/*
		 * Translators: With "fuzzy" I mean a more enhanced (and more
		 *  crappy) logic while searching for appropriate translations
		 *   for a original string.
		 */
		2, _("Use \"fuzzy\" matching routines for the learn buffer queries"),
		GtrPreferences.fuzzy_matching, gtranslator_preferences_dialog_changed);
	min_match_percentage=gtranslator_utils_attach_spin_with_label(ninth_page,
		3, _("Minimal required similarity percentage for fuzzy queries"),
		25, 100, GtrPreferences.min_match_percentage, gtranslator_preferences_dialog_changed);

	/*
	 * Connect the signals to the preferences box.
	 */
	gtk_signal_connect(GTK_OBJECT(prefs), "apply",
			   GTK_SIGNAL_FUNC(gtranslator_preferences_dialog_apply), NULL);
	gtk_signal_connect(GTK_OBJECT(prefs), "help",
			   GTK_SIGNAL_FUNC(gtranslator_preferences_dialog_help), NULL);
	gtk_signal_connect(GTK_OBJECT(prefs), "close",
			   GTK_SIGNAL_FUNC(gtranslator_utils_language_lists_free), NULL);

	gtk_window_set_wmclass(GTK_WINDOW(prefs), "gtranslator -- prefs",
		"gtranslator");
	gtk_window_set_position(GTK_WINDOW(prefs), GTK_WIN_POS_CENTER);
	gtk_widget_show_all(prefs);
}

/*
 * The actions to take when the user presses "Apply".
 */
static void gtranslator_preferences_dialog_apply(GtkWidget  * box, gint page_num, gpointer useless)
{
	gchar	*selected_scheme_file=NULL;
	gchar	*translator_str=NULL;
	gchar	*translator_email_str=NULL;
	
	/*
	 * We need to apply only once. 
	 */
	if (page_num != -1)
		return;
#define update(value,widget) GTR_FREE(value); \
	value=gtk_editable_get_chars(GTK_EDITABLE(widget),0,-1);

	update(translator_str, authors_name);

	/*
	 * Check if the user did forget to enter his/her name into the prefs
	 *  dialog.
	 */
	if(!translator_str || *translator_str=='\0')
	{
		gnome_app_error(GNOME_APP(gtranslator_application),
			_("Please enter your name!"));

		return;
	}
	
	update(translator_email_str, authors_email);

	/*
	 * Also check if the user did forget to enter his/her EMail address
	 *  into the prefs dialog.
	 */
	if(!translator_email_str || *translator_email_str=='\0')
	{
		gnome_app_error(GNOME_APP(gtranslator_application),
			_("Please enter your EMail address!"));
		return;
	}
	else
	{
		/*
		 * Also check an eventually given EMail address for brevity.
		 */
		if(!strchr(translator_email_str, '@') || 
			!strchr(translator_email_str, '.') || 
			(strlen(translator_email_str) <= 6))
		{
			gnome_app_error(GNOME_APP(gtranslator_application),
				_("Please enter a valid EMail address!"));
			return;
		}
	}

	/*
	 * Now set the translator informations for the "gtranslator_translator".
	 */
	gtranslator_translator_set_translator(gtranslator_translator,
		translator_str, translator_email_str);

	GTR_FREE(translator_str);
	GTR_FREE(translator_email_str);
	
	update(gtranslator_translator->language->name, GTK_COMBO(authors_language)->entry);
	update(gtranslator_translator->language->locale, GTK_COMBO(lcode)->entry);
	update(gtranslator_translator->language->group_email, GTK_COMBO(lg_email)->entry);
	update(gtranslator_translator->language->encoding, GTK_COMBO(mime_type)->entry);
	update(gtranslator_translator->language->bits, GTK_COMBO(encoding)->entry);
	update(GtrPreferences.dictionary, dictionary_file);
	update(GtrPreferences.autosave_suffix, autosave_suffix);
#undef update
#define if_active(widget) \
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
	GtrPreferences.save_geometry = if_active(save_geometry_tb);
	GtrPreferences.unmark_fuzzy = if_active(unmark_fuzzy);
	GtrPreferences.warn_if_fuzzy = if_active(warn_if_fuzzy);
	GtrPreferences.highlight = if_active(highlight);
	GtrPreferences.dot_char = if_active(use_dot_char);
	GtrPreferences.update_function = if_active(use_update_function);
	GtrPreferences.rambo_function = if_active(rambo_function);
	GtrPreferences.popup_menu = if_active(enable_popup_menu);
	GtrPreferences.sweep_compile_file = if_active(sweep_compile_file);
	GtrPreferences.show_comment = if_active(show_comment);
	GtrPreferences.show_messages_table = if_active(show_messages_table);
	GtrPreferences.collapse_translated = if_active(collapse_translated_entries);
	GtrPreferences.check_recent_file = if_active(check_recent_files);
	GtrPreferences.instant_spell_check = if_active(instant_spell_checking);
	GtrPreferences.use_own_fonts = if_active(own_fonts);
	GtrPreferences.use_own_colors = if_active(own_colors);
	GtrPreferences.use_own_mt_colors = if_active(use_own_mt_colors);
	GtrPreferences.use_own_dict = if_active(use_own_dict);
	GtrPreferences.use_learn_buffer = if_active(use_learn_buffer);
	GtrPreferences.fuzzy_matching = if_active(fuzzy_matching);
	GtrPreferences.auto_learn = if_active(auto_learn);
	GtrPreferences.keep_obsolete = if_active(keep_obsolete);
	GtrPreferences.check_formats = if_active(check_formats);
	GtrPreferences.autosave = if_active(autosave);
	GtrPreferences.autosave_with_suffix = if_active(autosave_with_suffix);
#undef if_active

	GtrPreferences.autosave_timeout = 
		gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(
			autosave_timeout));
		
	GtrPreferences.max_history_entries =
		gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(
			max_history_entries));
	
	GtrPreferences.min_match_percentage =
		gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(
			min_match_percentage));
	
	gtranslator_config_set_string("dict/file", GtrPreferences.dictionary);
	gtranslator_config_set_string("informations/autosave_suffix", 
		GtrPreferences.autosave_suffix);
	
	gtranslator_config_set_float("informations/autosave_timeout", 
		GtrPreferences.autosave_timeout);

	gtranslator_config_set_float("informations/max_history_entries", 
		GtrPreferences.max_history_entries);
	
	gtranslator_config_set_float("informations/min_match_percentage",
		GtrPreferences.min_match_percentage);
	
	GTR_FREE(GtrPreferences.msgid_font);
	GTR_FREE(GtrPreferences.msgstr_font);
	
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
	GTR_FREE(GtrPreferences.scheme);
	
	selected_scheme_file=gtk_editable_get_chars(GTK_EDITABLE(
		GTK_COMBO(scheme_file)->entry), 0, -1);

	if(selected_scheme_file)
	{
		/*
		 * First check if there's such a colorscheme in 
		 *  ~/.gtranslator/colorschemes before checking the global 
		 *   colorschemes directory.
		 */
		GtrPreferences.scheme=g_strdup_printf(
			"%s/.gtranslator/colorschemes/%s.xml",
				g_get_home_dir(), 
				selected_scheme_file);

		/*
		 * If there's no such colorscheme in the 
		 *  ~/.gtranslator/colorschemes directory, try the global 
		 *   colorschemes directory.
		 */
		if(!g_file_exists(GtrPreferences.scheme))
		{
			GtrPreferences.scheme=g_strdup_printf("%s/%s.xml", 
				SCHEMESDIR, selected_scheme_file);
		}
	    
		if(g_file_exists(GtrPreferences.scheme))
		{
			/*
			 * Free the old used colorscheme.
			 */
			gtranslator_color_scheme_free(&theme);
			
			/*
			 * Read in the new colorscheme, initialize the colors.
			 */
			gtranslator_color_scheme_apply(GtrPreferences.scheme);
			theme=gtranslator_color_scheme_load_from_prefs();
			
			gtranslator_colors_initialize();
		}

		GTR_FREE(selected_scheme_file);
	}

	gtranslator_color_values_set(GNOME_COLOR_PICKER(foreground), COLOR_FG);
	gtranslator_color_values_set(GNOME_COLOR_PICKER(background), COLOR_BG);

	/*
	 * The messages table colors are set up here.
	 */
	gtranslator_color_values_set(GNOME_COLOR_PICKER(mt_untranslated), 
		COLOR_MESSAGES_TABLE_UNTRANSLATED);
	gtranslator_color_values_set(GNOME_COLOR_PICKER(mt_fuzzy), 
		COLOR_MESSAGES_TABLE_FUZZY);
	gtranslator_color_values_set(GNOME_COLOR_PICKER(mt_translated), 
		COLOR_MESSAGES_TABLE_TRANSLATED);

	gtranslator_set_style(text_box, 0);
	gtranslator_set_style(trans_box, 1);

	gtranslator_config_set_bool("toggles/save_geometry", GtrPreferences.save_geometry);
	gtranslator_config_set_bool("toggles/warn_if_fuzzy", GtrPreferences.warn_if_fuzzy);
	gtranslator_config_set_bool("toggles/set_non_fuzzy_if_changed", 
			      GtrPreferences.unmark_fuzzy);
	gtranslator_config_set_bool("toggles/use_dot_char",
			      GtrPreferences.dot_char);
	gtranslator_config_set_bool("toggles/use_update_function",
			      GtrPreferences.update_function);
	gtranslator_config_set_bool("toggles/sweep_compile_file",
			      GtrPreferences.sweep_compile_file);
	gtranslator_config_set_bool("toggles/enable_popup_menu",
			      GtrPreferences.popup_menu);
	gtranslator_config_set_bool("toggles/show_comment",
			      GtrPreferences.show_comment);
	gtranslator_config_set_bool("toggles/rambo_function",
			      GtrPreferences.rambo_function);
	gtranslator_config_set_bool("toggles/check_recent_files",
			      GtrPreferences.check_recent_file);
	gtranslator_config_set_bool("toggles/check_formats",
			      GtrPreferences.check_formats);
	gtranslator_config_set_bool("toggles/highlight",
			      GtrPreferences.highlight);
	gtranslator_config_set_bool("toggles/instant_spell_check",
			      GtrPreferences.instant_spell_check);
	gtranslator_config_set_bool("toggles/use_own_fonts",
			      GtrPreferences.use_own_fonts);
	gtranslator_config_set_bool("toggles/use_own_colors",
			      GtrPreferences.use_own_colors);
	gtranslator_config_set_bool("toggles/use_own_mt_colors",
			      GtrPreferences.use_own_mt_colors);
	gtranslator_config_set_bool("toggles/use_own_dict",
			      GtrPreferences.use_own_dict);
	gtranslator_config_set_bool("toggles/use_learn_buffer",
			      GtrPreferences.use_learn_buffer);
	gtranslator_config_set_bool("toggles/fuzzy_matching",
			      GtrPreferences.fuzzy_matching);
	gtranslator_config_set_bool("toggles/auto_learn",
			      GtrPreferences.auto_learn);
	gtranslator_config_set_bool("toggles/show_messages_table",
			      GtrPreferences.show_messages_table);
	gtranslator_config_set_bool("toggles/collapse_translated_entries",
			      GtrPreferences.collapse_translated);
	gtranslator_config_set_bool("toggles/keep_obsolete",
			      GtrPreferences.keep_obsolete);
	gtranslator_config_set_bool("toggles/autosave",
			      GtrPreferences.autosave);
	gtranslator_config_set_bool("toggles/autosave_with_suffix",
			      GtrPreferences.autosave_with_suffix);

	/*
	 * Save our translator data.
	 */
	gtranslator_translator_save(gtranslator_translator);

	/*
	 * Hide the comment viewing (area) via our new util function.
	 */
	if(!GtrPreferences.show_comment)
	{
		gtranslator_comment_hide();
	}
}

/*
 * The preferences box's help window.
 */
static void gtranslator_preferences_dialog_help(GtkWidget  * widget, gpointer useless)
{
	gnome_app_message(GNOME_APP(gtranslator_application), _("\
The Preferences box allows you to customise gtranslator\n\
to work in ways you find comfortable and productive."));
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
			if (!nautilus_strcasecmp(current, _(languages[c].name))) {
				set_text(lcode, locale);
				set_text(mime_type, encoding);
				set_text(encoding, bits);
				set_text(lg_email, group_email);
				break;
			}
			c++;
		}
		break;
	case 2:
		current =
		    gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(lcode)->entry));
		while (languages[c].name != NULL) {
			if (!nautilus_strcmp(current, languages[c].locale)) {
				set_text(mime_type, encoding);
				set_text(encoding, bits);
				set_text(lg_email, group_email);
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

void toggle_sensitive(GtkWidget *widget, gpointer data)
{
	gboolean active;
	active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	gtk_widget_set_sensitive(GTK_WIDGET(data), active);
	gnome_property_box_changed(GNOME_PROPERTY_BOX(prefs));
}

void gtranslator_preferences_read(void)
{
	/*
	 * Initialize the preferences with default values if this is our first
	 *  startup of gtranslator.
	 */  
	gtranslator_preferences_init_default_values();

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
	GtrPreferences.min_match_percentage =
		gtranslator_config_get_float("informations/min_match_percentage");

	/*
	 * Check some prefs values for sanity and set sane values if no really
	 *  good prefs values have been detected.
	 */
	if(GtrPreferences.min_match_percentage < 25.0)
	{
		GtrPreferences.min_match_percentage=25.0;
	}

	if(GtrPreferences.autosave_timeout < 1.0)
	{
		GtrPreferences.autosave_timeout=1.0;
	}

	GtrPreferences.instant_spell_check = 
		gtranslator_config_get_bool("toggles/instant_spell_check");
	GtrPreferences.save_geometry =
		gtranslator_config_get_bool("toggles/save_geometry");
	GtrPreferences.unmark_fuzzy =
		gtranslator_config_get_bool("toggles/set_non_fuzzy_if_changed"); 
	GtrPreferences.warn_if_fuzzy =
		gtranslator_config_get_bool("toggles/warn_if_fuzzy");
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
	GtrPreferences.use_own_mt_colors =
		gtranslator_config_get_bool("toggles/use_own_mt_colors");
	GtrPreferences.use_own_dict =
		gtranslator_config_get_bool("toggles/use_own_dict");
	GtrPreferences.keep_obsolete =
		gtranslator_config_get_bool("toggles/keep_obsolete");
	GtrPreferences.rambo_function =
		gtranslator_config_get_bool("toggles/rambo_function");

	GtrPreferences.use_learn_buffer = gtranslator_config_get_bool(
		"toggles/use_learn_buffer");
	GtrPreferences.fuzzy_matching = gtranslator_config_get_bool(
		"toggles/fuzzy_matching");
	GtrPreferences.auto_learn = gtranslator_config_get_bool(
		"toggles/auto_learn");
	GtrPreferences.check_formats = gtranslator_config_get_bool(
		"toggles/check_formats");
	GtrPreferences.highlight = gtranslator_config_get_bool(
		"toggles/highlight");
	GtrPreferences.match_case = gtranslator_config_get_bool(
		"find/case_sensitive");
	GtrPreferences.find_in = gtranslator_config_get_int("find/find_in");
	gtranslator_update_regex_flags();
	
	GtrPreferences.fill_header = gtranslator_config_get_bool(
		"toggles/fill_header");
	GtrPreferences.show_comment = gtranslator_config_get_bool(
		"toggles/show_comment");
	GtrPreferences.show_messages_table = gtranslator_config_get_bool(
		"toggles/show_messages_table");
	GtrPreferences.collapse_translated = gtranslator_config_get_bool(
		"toggles/collapse_translated_entries");

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
}

/*
 * Free the resting pieces of the configuration.
 */
void gtranslator_preferences_free()
{
	GTR_FREE(GtrPreferences.autosave_suffix);
	GTR_FREE(GtrPreferences.spell_command);
	GTR_FREE(GtrPreferences.dictionary);
	GTR_FREE(GtrPreferences.msgid_font);
	GTR_FREE(GtrPreferences.msgstr_font);
	GTR_FREE(GtrPreferences.scheme);
}
