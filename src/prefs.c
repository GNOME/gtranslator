/*
 * (C) 2000-2003 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *                      Ross Golder <ross@golder.org>
 *                      Søren Wedel Nielsen <swn@herlevkollegiet.dk>
 *
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
#include "defines.h"
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

/*
 * The callbacks:
 */
static void gtranslator_preferences_dialog_changed(GtkWidget  * widget, gpointer useless);
static void gtranslator_preferences_dialog_close(GtkWidget  * widget, gint page_num,
			    gpointer useless);
static void toggle_sensitive(GtkWidget *widget, gpointer data);

/*
 * The entries:
 */
static GtkWidget
	*authors_name, *authors_email, *authors_language,
	*mime_type, *encoding, *lcode, *lg_email, *dictionary_file,
	*scheme_file, *autosave_suffix, *hotkey_chars;

/*
 * The toggle buttons/labels used in the preferences box:
 */
static GtkWidget
	*warn_if_fuzzy, *unmark_fuzzy, *save_geometry_tb,
	*use_dot_char, *use_update_function, *show_pf_dialog,
	*check_recent_files, *own_fonts, *own_colors, *use_own_dict,
	*instant_spell_checking, *keep_obsolete, *autosave,
	*autosave_with_suffix, *sweep_compile_file, *use_learn_buffer,
	*show_messages_table, *rambo_function, *use_own_mt_colors,
	*collapse_all_entries, *auto_learn, *fuzzy_matching,
	*show_comment, *highlight;

/*
 * The autosave etc. timeout GtkSpinButtons:
 */
static GtkWidget
	*autosave_timeout, *max_history_entries, *min_match_percentage;

/*
 * Font/color specific widgets used in the preferences box.
 */
static GtkWidget *foreground, *background, *msgid_font, *msgstr_font,
	*mt_untranslated, *mt_fuzzy, *mt_translated;

/*
 * The preferences dialog widget itself, plus a notebook.
 */
static GtkWidget *prefs = NULL, *prefs_notebook = NULL;

/*
 * Flag to set when something changes
 */
static gboolean prefs_changed;

GtkWidget *gtranslator_utils_append_page_to_preferences_dialog(GtkWidget  *notebook, gint rows, gint cols,
			     const char *label_text)
{
	GtkWidget *page;
	page = gtk_table_new(rows, cols, FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(page), 2);
	gtk_table_set_row_spacings(GTK_TABLE(page), 2);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new(label_text));
	return page;
}

/*
 * Helper functions
 */

GtkWidget* gtranslator_preferences_page_new_append(GtkWidget *notebook, const char *caption) {
	GtkWidget *vbox, *label;

	vbox = gtk_vbox_new (FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 12);

	label = gtk_label_new (caption);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);
  
	return vbox;
}


GtkWidget* gtranslator_preferences_category_new_pack_start(GtkWidget *page, const gchar *caption) {
	GtkWidget *vbox;
	GtkWidget *label, *hbox, *content_box;
	gchar *labeltext;

	vbox = gtk_vbox_new (FALSE, 6);

	labeltext = g_strconcat("<span weight=\"bold\">",caption,"</span>",NULL);
	label = gtk_label_new (labeltext);
	g_free(labeltext);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);  
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

	label = gtk_label_new ("    ");
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

	content_box = gtk_vbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (hbox), content_box, TRUE, TRUE, 0);

	gtk_box_pack_start (GTK_BOX (page), vbox, TRUE, TRUE, 0);

	return content_box;
}



void *gtranslator_preferences_pack_start_with_label(GtkWidget *box, 
						    GtkWidget *widget, 
						    GtkSizeGroup *label_size_group, 
						    GtkWidget *depend,
						    const char *caption)
{
	GtkWidget *hbox, *label;

	hbox = gtk_hbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(caption);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
	if (label_size_group != NULL)
		gtk_size_group_add_widget (GTK_SIZE_GROUP (label_size_group), GTK_WIDGET (label));

	gtk_box_pack_start (GTK_BOX (hbox), widget, FALSE, TRUE, 0);
	if (depend != NULL) {
		gtk_widget_set_sensitive(GTK_WIDGET(label), 
					 gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(depend)));
		g_signal_connect(G_OBJECT(depend), "toggled",
				 G_CALLBACK(toggle_sensitive), label); 
		gtk_widget_set_sensitive(GTK_WIDGET(widget), 
					 gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(depend)));
		g_signal_connect(G_OBJECT(depend), "toggled",
				 G_CALLBACK(toggle_sensitive), widget);
	}
	return label;
}



GtkWidget *gtranslator_preferences_combo_new(GList  * list, 
					     const char *value,
					     GtkSizeGroup *size_group, 
					     GCallback callback,
					     gpointer user_data)
{
	GtkWidget *combo;

	combo= gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(combo), list);
	if (value)
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(combo)->entry), value);
	gtk_editable_set_editable(GTK_EDITABLE(GTK_COMBO(combo)->entry), 0);
	gtk_size_group_add_widget (GTK_SIZE_GROUP (size_group), GTK_WIDGET (combo));
	g_signal_connect(G_OBJECT(GTK_COMBO(combo)->entry), "changed",
			 G_CALLBACK(callback), user_data);
	return combo;
}


GtkWidget *gtranslator_preferences_hotkey_char_widget_new(GtkSizeGroup *size_group)
{
	GtkWidget *box, *label, *rb_1, *rb_2;

	box=gtk_hbox_new(FALSE, 2);
	label=gtk_label_new(_("Hotkey indicating character:"));

	rb_1=gtk_radio_button_new_with_label(NULL, "\"_\" (GNOME)");
	rb_2=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(rb_1), "\"&\" (KDE)");

	if(GtrPreferences.hotkey_char=='_')
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_1), TRUE);
	}
	else
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_2), TRUE);
	}
	
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(label), FALSE, TRUE, 2);

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(rb_1), FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(rb_2), FALSE, FALSE, 2);

	gtk_size_group_add_widget(GTK_SIZE_GROUP(size_group), GTK_WIDGET(box));

	g_signal_connect(G_OBJECT(rb_1), "toggled", G_CALLBACK(gtranslator_preferences_dialog_changed), GINT_TO_POINTER(10));
	g_signal_connect(G_OBJECT(rb_2), "toggled", G_CALLBACK(gtranslator_preferences_dialog_changed), GINT_TO_POINTER(11));

	return box;
}

GtkWidget *gtranslator_preferences_entry_new(const char *value,
					     GtkSizeGroup *size_group, 
					     GCallback callback)
{
	GtkWidget *entry;

	entry = gtk_entry_new();
	if (value)
		gtk_entry_set_text(GTK_ENTRY(entry), value);
	if (size_group != NULL)
		gtk_size_group_add_widget (GTK_SIZE_GROUP (size_group), GTK_WIDGET (entry));
	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(callback), NULL);
	return entry;
}


GtkWidget *gtranslator_preferences_toggle_new(const char *label_text,
					      gboolean value,
					      GCallback callback)
{
	GtkWidget *toggle;
	toggle = gtk_check_button_new_with_label(label_text);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), value);
	if(callback)
	{
		g_signal_connect(G_OBJECT(toggle), "toggled",
				 G_CALLBACK(callback), NULL);
	}
  
	return toggle;
}



GtkWidget *gtranslator_preferences_font_picker_new(const gchar *title_text,
						   const gchar *fontspec,
						   GtkSizeGroup *size_group, 
						   GCallback callback)
{
	GtkWidget *font_selector;

	font_selector = gnome_font_picker_new();
	gnome_font_picker_set_title(GNOME_FONT_PICKER(font_selector), title_text);
	if(fontspec)
		gnome_font_picker_set_font_name(GNOME_FONT_PICKER(font_selector), fontspec);
	gnome_font_picker_set_mode(GNOME_FONT_PICKER(font_selector),
				   GNOME_FONT_PICKER_MODE_FONT_INFO);
	gtk_size_group_add_widget (GTK_SIZE_GROUP (size_group), GTK_WIDGET (font_selector));
	g_signal_connect(G_OBJECT(font_selector), "font_set",
			 G_CALLBACK(callback), NULL);

	return font_selector;
}


GtkWidget *gtranslator_preferences_color_picker_new(const gchar *title_text,
						    GtkSizeGroup *size_group, 
						    ColorType color_type,
						    GCallback callback)
{
	GtkWidget *color_selector;

	color_selector = gnome_color_picker_new();
	gnome_color_picker_set_title(GNOME_COLOR_PICKER(color_selector), title_text);

	gtranslator_color_values_get(GNOME_COLOR_PICKER(color_selector), color_type);
	if (size_group != NULL)
		gtk_size_group_add_widget (GTK_SIZE_GROUP (size_group), GTK_WIDGET (color_selector));
	g_signal_connect(G_OBJECT(color_selector), "color_set",
			 G_CALLBACK(callback), NULL);

	return color_selector;
}

void gtranslator_preferences_dialog_create(GtkWidget *widget, gpointer useless)
{
 	gchar	*old_colorscheme=NULL;
 	GtkObject *adjustment;

	GtkWidget *page, *category_box, *hbox;
	GtkSizeGroup *label_size_group, *control_size_group;

	prefs_changed = FALSE;

 	if(prefs != NULL) {
		gtk_window_present(GTK_WINDOW(prefs));
		return;
 	}
 
 	/*
 	 * Create the preferences box... 
 	 */
	prefs = gtk_dialog_new_with_buttons(
		_("gtranslator -- options"),
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
		NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(prefs), GTK_RESPONSE_CLOSE);
 
	/*
	 * The notebook containing the pages
	 */
	prefs_notebook = gtk_notebook_new();
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(prefs)->vbox),
			   prefs_notebook);
 

	/*
	 * Files page
	 */
	page = gtranslator_preferences_page_new_append(prefs_notebook, _("Files")); 
	category_box = gtranslator_preferences_category_new_pack_start(page, _("General"));
	warn_if_fuzzy = gtranslator_preferences_toggle_new(_("Warn if po file contains fuzzy translations"),
							   GtrPreferences.warn_if_fuzzy,
							   G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), warn_if_fuzzy, FALSE, FALSE, 0);
	sweep_compile_file = gtranslator_preferences_toggle_new(_("Delete compiled files (e.g. \"project.gmo\")"),
								GtrPreferences.sweep_compile_file,
								G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), sweep_compile_file, FALSE, FALSE, 0);
	category_box = gtranslator_preferences_category_new_pack_start(page, _("Autosave"));
        autosave = gtranslator_preferences_toggle_new(_("Automatically save at regular intervals"),
						      GtrPreferences.autosave,
						      G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), autosave, FALSE, FALSE, 0);
	adjustment = gtk_adjustment_new (GtrPreferences.autosave_timeout, 1.0, 30.0, 1.0, 1.0, 1.0);
	autosave_timeout = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 1, 0);
 	gtk_widget_set_sensitive(GTK_WIDGET(autosave_timeout), GtrPreferences.autosave);
       	gtranslator_preferences_pack_start_with_label(category_box, autosave_timeout, NULL, 
						      autosave, _("Autosave interval:"));
        autosave_with_suffix = gtranslator_preferences_toggle_new(_("Append a suffix to automatically saved files"),
								  GtrPreferences.autosave_with_suffix,
								  G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), autosave_with_suffix, FALSE, FALSE, 0);
	autosave_suffix = gtranslator_preferences_entry_new(GtrPreferences.autosave_suffix, NULL, 
							    G_CALLBACK(gtranslator_preferences_dialog_changed));
       	gtranslator_preferences_pack_start_with_label(category_box, autosave_suffix, NULL, 
						      autosave_with_suffix, _("Suffix:"));
	category_box =  gtranslator_preferences_category_new_pack_start(page, _("Recent files"));
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (category_box), hbox, FALSE, TRUE, 0);
	adjustment = gtk_adjustment_new (GtrPreferences.max_history_entries, 3, 15, 1, 10, 10);
	max_history_entries = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 1, 0);
	g_signal_connect(G_OBJECT(max_history_entries), "changed",
 			 G_CALLBACK(gtranslator_preferences_dialog_changed), NULL);
	gtranslator_preferences_pack_start_with_label(category_box, max_history_entries, NULL,  NULL,
						      _("Maximum number of entries in the recent files list:"));
	check_recent_files = gtranslator_preferences_toggle_new(_("Check recent files before showing in recent files list"),
								GtrPreferences.check_recent_file,
								G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), check_recent_files, FALSE, FALSE, 0);
 
	/*
	 * Editor page
	 */
 	page = gtranslator_preferences_page_new_append(prefs_notebook, _("Editor")); 
	label_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	control_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
 	category_box = gtranslator_preferences_category_new_pack_start(page, _("Text display"));
        highlight = gtranslator_preferences_toggle_new(_("Highlight syntax of the translation message"),
						       GtrPreferences.highlight,
						       G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), highlight, FALSE, FALSE, 0);

	old_colorscheme=gtranslator_utils_get_raw_file_name(GtrPreferences.scheme);
	scheme_file = gtranslator_preferences_combo_new(colorschemes, old_colorscheme,
							control_size_group, 
							G_CALLBACK(gtranslator_preferences_dialog_changed),
							NULL);
	gtranslator_preferences_pack_start_with_label(category_box, scheme_file, label_size_group, NULL, 
						      _("Syntax color scheme to use:"));
        use_dot_char = gtranslator_preferences_toggle_new(_("Use special character to indicate white space"),
							  GtrPreferences.dot_char,
							  G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), use_dot_char, FALSE, FALSE, 0);

	hotkey_chars = gtranslator_preferences_hotkey_char_widget_new(label_size_group);
	gtk_box_pack_start (GTK_BOX (category_box), hotkey_chars, FALSE, FALSE, 0);
	
        own_fonts = gtranslator_preferences_toggle_new(_("Apply own fonts"),
						       GtrPreferences.use_own_fonts,
						       G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), own_fonts, FALSE, FALSE, 0);
	msgid_font=gtranslator_preferences_font_picker_new(_("gtranslator -- font selection/msgid font"), 
							   GtrPreferences.msgid_font, control_size_group,
							   G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtranslator_preferences_pack_start_with_label(category_box, msgid_font, label_size_group, 
						      own_fonts, _("Original text font:"));
	msgstr_font = gtranslator_preferences_font_picker_new(_("gtranslator -- font selection/msgstr font"), 
							      GtrPreferences.msgstr_font, control_size_group,
							      G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtranslator_preferences_pack_start_with_label(category_box, msgstr_font, label_size_group, 
						      own_fonts, _("Translation font:"));
	own_colors = gtranslator_preferences_toggle_new(_("Apply own colors:"),
							GtrPreferences.use_own_colors,
							G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), own_colors, FALSE, FALSE, 0);
	foreground = gtranslator_preferences_color_picker_new(_("gtranslator -- foreground color"), 
							      control_size_group, COLOR_FG,
							      G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtranslator_preferences_pack_start_with_label(category_box, foreground, label_size_group, 
						      own_colors, _("Foreground color:"));
	background = gtranslator_preferences_color_picker_new(_("gtranslator -- background color"), 
							      control_size_group, COLOR_BG,
							      G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtranslator_preferences_pack_start_with_label(category_box, background, label_size_group, 
						      own_colors, _("Background:"));
	category_box = gtranslator_preferences_category_new_pack_start(page, _("Contents"));
	unmark_fuzzy = gtranslator_preferences_toggle_new(_("Remove fuzzy status if message is changed"),
							  GtrPreferences.unmark_fuzzy,
							  G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), unmark_fuzzy, FALSE, FALSE, 0);
        keep_obsolete = gtranslator_preferences_toggle_new(_("Keep obsolete messages in the po files"),
							   GtrPreferences.keep_obsolete,
							   G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), keep_obsolete, FALSE, FALSE, 0);
	category_box = gtranslator_preferences_category_new_pack_start(page, _("Spell checking"));
	instant_spell_checking = gtranslator_preferences_toggle_new(_("Instant spell checking"),
								    GtrPreferences.instant_spell_check,
								    G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), instant_spell_checking, FALSE, FALSE, 0);
	hbox = gtk_hbox_new (FALSE, 6);
        gtk_box_pack_start (GTK_BOX (category_box), hbox, FALSE, FALSE, 0);
        use_own_dict = gtranslator_preferences_toggle_new(_("Use special dictionary:"),
                                                          GtrPreferences.use_own_dict,
                                                          G_CALLBACK(gtranslator_preferences_dialog_changed));
        gtk_size_group_add_widget (GTK_SIZE_GROUP (label_size_group), GTK_WIDGET (use_own_dict));
        gtk_box_pack_start (GTK_BOX (hbox), use_own_dict, FALSE, FALSE, 0);
                                                                                
        dictionary_file = gtranslator_preferences_entry_new(GtrPreferences.dictionary, control_size_group,
                                                            G_CALLBACK(gtranslator_preferences_dialog_changed));
        gtk_box_pack_start (GTK_BOX (hbox), dictionary_file, FALSE, TRUE, 0);
        gtk_widget_set_sensitive(GTK_WIDGET(dictionary_file), GtrPreferences.use_own_dict);
        g_signal_connect(G_OBJECT(use_own_dict), "toggled",
                         G_CALLBACK(toggle_sensitive), dictionary_file);
        g_signal_connect(G_OBJECT(dictionary_file), "changed",
                         G_CALLBACK(gtranslator_preferences_dialog_changed), NULL);
                                                                                
	/*
	 * PO header page
	 */
	page = gtranslator_preferences_page_new_append(prefs_notebook, _("PO header")); 
	label_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	control_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	category_box = gtranslator_preferences_category_new_pack_start(page, _("Personal Information"));
	authors_name = gtranslator_preferences_entry_new(gtranslator_translator->name, control_size_group, 
							 G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtranslator_preferences_pack_start_with_label(category_box, authors_name, label_size_group, 
						      NULL, _("Author's name:"));
	authors_email = gtranslator_preferences_entry_new(gtranslator_translator->email, control_size_group, 
							  G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtranslator_preferences_pack_start_with_label(category_box, authors_email, label_size_group, 
						      NULL, _("Author's email:"));
	category_box = gtranslator_preferences_category_new_pack_start(page, _("Language settings"));
	gtranslator_utils_language_lists_create();
	authors_language = gtranslator_preferences_combo_new(languages_list, gtranslator_translator->language->name,
							     control_size_group,
							     G_CALLBACK(gtranslator_preferences_dialog_changed),
							     GINT_TO_POINTER(1));
	gtranslator_preferences_pack_start_with_label(category_box, authors_language, label_size_group, 
						      NULL, _("Language:"));
	lcode = gtranslator_preferences_combo_new(lcodes_list, gtranslator_translator->language->locale,
						  control_size_group, 
						  G_CALLBACK(gtranslator_preferences_dialog_changed),
						  GINT_TO_POINTER(2));
	gtranslator_preferences_pack_start_with_label(category_box, lcode, label_size_group, 
						      NULL, _("Language code:"));
	lg_email = gtranslator_preferences_combo_new(group_emails_list, gtranslator_translator->language->group_email,
						     control_size_group, 
						     G_CALLBACK(gtranslator_preferences_dialog_changed),
						     NULL);
	gtranslator_preferences_pack_start_with_label(category_box, lg_email, label_size_group,  
						      NULL,_("Language group's email:"));
	mime_type = gtranslator_preferences_combo_new(encodings_list, gtranslator_translator->language->encoding,
						      control_size_group, 
						      G_CALLBACK(gtranslator_preferences_dialog_changed),
						      NULL);
	gtranslator_preferences_pack_start_with_label(category_box, mime_type, label_size_group, 
						      NULL, _("Charset:"));
	encoding = gtranslator_preferences_combo_new(bits_list, gtranslator_translator->language->bits,
						     control_size_group, 
						     G_CALLBACK(gtranslator_preferences_dialog_changed),
						     NULL);
	gtranslator_preferences_pack_start_with_label(category_box, encoding, label_size_group, NULL, _("Encoding:"));

	/*
	 * Functionality page
	 */
	page = gtranslator_preferences_page_new_append(prefs_notebook, _("Functionality")); 
	label_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	category_box = gtranslator_preferences_category_new_pack_start(page, _("General"));
	use_update_function = gtranslator_preferences_toggle_new(_("Enable the functionality to update a po file from within gtranslator"),
								 GtrPreferences.update_function,
								 G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), use_update_function, FALSE, FALSE, 0);
	rambo_function = gtranslator_preferences_toggle_new(_("Enable the functionality to remove all translations from a po file"),
							    GtrPreferences.rambo_function,
							    G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), rambo_function, FALSE, FALSE, 0);
	show_comment = gtranslator_preferences_toggle_new(_("Show instant comment view in main pane"),
							  GtrPreferences.show_comment,
							  G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), show_comment, FALSE, FALSE, 0);
	show_pf_dialog = gtranslator_preferences_toggle_new(_("Display special dialog for messages with plural forms"),
							  GtrPreferences.show_plural_forms,
							  G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), show_pf_dialog, FALSE, FALSE, 0);
	save_geometry_tb = gtranslator_preferences_toggle_new(_("Save geometry on exit and restore it on startup"),
							      GtrPreferences.save_geometry,
							      G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), save_geometry_tb, FALSE, FALSE, 0);
	category_box = gtranslator_preferences_category_new_pack_start(page, _("Messages table"));
	show_messages_table = gtranslator_preferences_toggle_new(_("Show the messages table (requires restart)"),
								 GtrPreferences.show_messages_table,
								 G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), show_messages_table, FALSE, FALSE, 0);
	collapse_all_entries = gtranslator_preferences_toggle_new(_("Collapse all entries by default"),
									 GtrPreferences.collapse_all,
									 G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), collapse_all_entries, FALSE, FALSE, 0);
	use_own_mt_colors = gtranslator_preferences_toggle_new(_("Use own colors for messages table groups"),
							       GtrPreferences.use_own_mt_colors,
							       G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), use_own_mt_colors, FALSE, FALSE, 0);
	mt_untranslated = gtranslator_preferences_color_picker_new("gtranslator -- untranslated entries' color",
								   NULL,
								   COLOR_MESSAGES_TABLE_UNTRANSLATED,
								   G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtranslator_preferences_pack_start_with_label(category_box, mt_untranslated, label_size_group, 
						      use_own_mt_colors, _("Fuzzy entries color:"));
	mt_fuzzy = gtranslator_preferences_color_picker_new("gtranslator -- fuzzy entries' color",
							    NULL,
							    COLOR_MESSAGES_TABLE_FUZZY,
							    G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtranslator_preferences_pack_start_with_label(category_box, mt_fuzzy, label_size_group, 
						      use_own_mt_colors, _("Untranslated entries color:"));
	mt_translated = gtranslator_preferences_color_picker_new("gtranslator -- translated entries' color",
								 NULL,
								 COLOR_MESSAGES_TABLE_TRANSLATED,
								 G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtranslator_preferences_pack_start_with_label(category_box, mt_translated, label_size_group, 
						      use_own_mt_colors, _("Translated entries color:"));

	/*
	 * Autotranslate page
	 */
	page = gtranslator_preferences_page_new_append(prefs_notebook, _("Autotranslation")); 
	category_box = gtranslator_preferences_category_new_pack_start(page, _("General"));
	use_learn_buffer = gtranslator_preferences_toggle_new(_("Also query the personal learn buffer while autotranslating untranslated messages"),
							      GtrPreferences.use_learn_buffer,
							      G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), use_learn_buffer, FALSE, FALSE, 0);
	auto_learn = gtranslator_preferences_toggle_new(_("Automatically learn a newly translated message"),
							GtrPreferences.auto_learn,
							G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), auto_learn, FALSE, FALSE, 0);
	category_box = gtranslator_preferences_category_new_pack_start(page, _("Fuzzy matching"));
	/*
	 * Translators: With "fuzzy" I mean a more enhanced (and more
	 *  crappy) logic while searching for appropriate translations
	 *   for a original string.
	 */
	fuzzy_matching = gtranslator_preferences_toggle_new(		
		_("Use \"fuzzy\" matching routines for learn buffer queries"),
		GtrPreferences.fuzzy_matching,
		G_CALLBACK(gtranslator_preferences_dialog_changed));
	gtk_box_pack_start (GTK_BOX (category_box), fuzzy_matching, FALSE, FALSE, 0);
	adjustment = gtk_adjustment_new (75, 0, 100, 1, 10, 10);
	min_match_percentage = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 1, 0);
	gtranslator_preferences_pack_start_with_label(category_box, min_match_percentage, NULL, fuzzy_matching,
						      _("Minimal required similarity persentage for fuzzy queries:"));
	/*
	 * Connect the signals to the preferences box.
	 */
	g_signal_connect(G_OBJECT(prefs), "response",
			 G_CALLBACK(gtranslator_preferences_dialog_close), NULL);

	gtranslator_dialog_show(&prefs, "gtranslator -- prefs");
}
 

/*
 * The actions to take when the user presses "Apply".
 */
static void gtranslator_preferences_dialog_close(GtkWidget * widget, gint page_num, gpointer useless)
{
	gchar	*selected_scheme_file=NULL;
	gchar	*translator_str=NULL;
	gchar	*translator_email_str=NULL;
	
	/*
	 * Free the languages list
	 */
	gtranslator_utils_language_lists_free(widget, useless);

	/*
	 * If nothing changed, just return
	 */
	if(!prefs_changed) {
		gtk_widget_destroy(GTK_WIDGET(prefs));
		return;
	}

#define update(value,widget) GTR_FREE(value); \
	value=gtk_editable_get_chars(GTK_EDITABLE(widget),0,-1);

	update(translator_str, authors_name);

	/*
	 * Check if the user did forget to enter his/her name into the prefs
	 *  dialog.
	 */
	if(!translator_str || *translator_str=='\0')
	{
		GtkWidget *dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			_("Please enter your name!"));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

		return;
	}
	
	update(translator_email_str, authors_email);

	/*
	 * Also check if the user did forget to enter his/her EMail address
	 *  into the prefs dialog.
	 */
	if(!translator_email_str || *translator_email_str=='\0')
	{
		GtkWidget *dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			_("Please enter your EMail address!"));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

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
			GtkWidget *dialog = gtk_message_dialog_new(
				GTK_WINDOW(gtranslator_application),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_OK,
				_("Please enter a valid EMail address!"));
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			
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
	GtrPreferences.sweep_compile_file = if_active(sweep_compile_file);
	GtrPreferences.show_comment = if_active(show_comment);
	GtrPreferences.show_plural_forms = if_active(show_pf_dialog);
	GtrPreferences.show_messages_table = if_active(show_messages_table);
	GtrPreferences.collapse_all = if_active(collapse_all_entries);
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
	GtrPreferences.autosave = if_active(autosave);
	GtrPreferences.autosave_with_suffix = if_active(autosave_with_suffix);
#undef if_active

	GtrPreferences.autosave_timeout = 
		gtk_spin_button_get_value(GTK_SPIN_BUTTON(
			autosave_timeout));
		
	GtrPreferences.max_history_entries =
		gtk_spin_button_get_value(GTK_SPIN_BUTTON(
			max_history_entries));
	
	GtrPreferences.min_match_percentage =
		gtk_spin_button_get_value(GTK_SPIN_BUTTON(
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
	 * Assign our attended hotkey character from the prefs dialog.
	 */
	gtranslator_config_set_int("editor/hotkey_char", GtrPreferences.hotkey_char);
	
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
		if(!g_file_test(GtrPreferences.scheme, G_FILE_TEST_EXISTS))
		{
			GtrPreferences.scheme=g_strdup_printf("%s/%s.xml", 
				SCHEMESDIR, selected_scheme_file);
		}
	    
		if(g_file_test(GtrPreferences.scheme, G_FILE_TEST_EXISTS))
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

	gtranslator_set_style(GTK_WIDGET(text_box), 0);
	gtranslator_set_style(GTK_WIDGET(trans_box), 1);

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
	gtranslator_config_set_bool("toggles/show_comment",
			      GtrPreferences.show_comment);
	gtranslator_config_set_bool("toggles/rambo_function",
			      GtrPreferences.rambo_function);
	gtranslator_config_set_bool("toggles/check_recent_files",
			      GtrPreferences.check_recent_file);
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
	gtranslator_config_set_bool("toggles/show_plural_forms",
			      GtrPreferences.show_plural_forms);
	gtranslator_config_set_bool("toggles/show_messages_table",
			      GtrPreferences.show_messages_table);
	gtranslator_config_set_bool("toggles/collapse_all",
			      GtrPreferences.collapse_all);
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

	gtk_widget_destroy(GTK_WIDGET(prefs));
	return;
}

static void gtranslator_preferences_dialog_changed(GtkWidget  * widget, gpointer flag)
{
	gint c = 0;
	G_CONST_RETURN gchar *current;

	prefs_changed = TRUE;

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
	case 10:
		/*
		 * This is the next group where we're handling the hotkey char changes.
		 */
		if(GTK_TOGGLE_BUTTON(widget)->active)
		{
			GtrPreferences.hotkey_char='_';
		}
		break;
	case 11:
		if(GTK_TOGGLE_BUTTON(widget)->active)
		{
			GtrPreferences.hotkey_char='&';
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
	prefs_changed = TRUE;
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

	GtrPreferences.hotkey_char = gtranslator_config_get_int("editor/hotkey_char");

	/*
	 * Beware us of stupid, non-existing hotkey characters please! If so,
	 *  then assign the standard GNOME '_' hotkey mnemonicial for it.
	 */
	if(!GtrPreferences.hotkey_char || GtrPreferences.hotkey_char<=1)
	{
		GtrPreferences.hotkey_char='_';
	}

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
	GtrPreferences.highlight = gtranslator_config_get_bool(
		"toggles/highlight");

	GtrPreferences.match_case = gtranslator_config_get_bool(
		"find/case_sensitive");
	GtrPreferences.ignore_hotkeys = gtranslator_config_get_bool(
		"find/ignore_hotkeys");
	
	GtrPreferences.fi_comments = gtranslator_config_get_bool("find/find_in_comments");
	GtrPreferences.fi_english = gtranslator_config_get_bool("find/find_in_english");
	GtrPreferences.fi_translation = gtranslator_config_get_bool("find/find_in_translation");
	
	GtrPreferences.fi_comments = gtranslator_config_get_bool("replace/replace_in_comments");
	GtrPreferences.fi_english = gtranslator_config_get_bool("replace/replace_in_english");
	GtrPreferences.fi_translation = gtranslator_config_get_bool("replace/replace_in_translation");

	gtranslator_update_regex_flags();

	GtrPreferences.fill_header = gtranslator_config_get_bool(
		"toggles/fill_header");
	GtrPreferences.show_comment = gtranslator_config_get_bool(
		"toggles/show_comment");
	GtrPreferences.show_messages_table = gtranslator_config_get_bool(
		"toggles/show_messages_table");
	GtrPreferences.collapse_all = gtranslator_config_get_bool(
		"toggles/collapse_all");
	GtrPreferences.show_plural_forms = gtranslator_config_get_bool(
		"toggles/show_plural_forms");

	/*
	 * Check if we'd to use special styles.
	 */
	if(GtrPreferences.use_own_fonts || GtrPreferences.use_own_colors)
	{
		/*
		 * Set the own specs for colors and for the font.
		 */
		gtranslator_set_style(GTK_WIDGET(text_box), 0);
		gtranslator_set_style(GTK_WIDGET(trans_box), 1);
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
