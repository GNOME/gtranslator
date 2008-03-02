/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "preferences-dialog.h"
#include "prefs-manager.h"
#include "utils_gui.h"
#include "plugin-manager.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>


#define GTR_PREFERENCES_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_PREFERENCES_DIALOG,     \
						 	GtranslatorPreferencesDialogPrivate))


G_DEFINE_TYPE(GtranslatorPreferencesDialog, gtranslator_preferences_dialog, GTK_TYPE_DIALOG)

struct _GtranslatorPreferencesDialogPrivate
{
	GtkWidget *notebook;
	
	/* Files->General */
	GtkWidget *warn_if_fuzzy_checkbutton;
	GtkWidget *delete_compiled_checkbutton;
	
	/* Files->Autosave */
	GtkWidget *autosave_checkbutton;
	GtkWidget *autosave_interval_spinbutton;
	GtkWidget *autosave_hbox;
	GtkWidget *append_suffix_checkbutton;
	GtkWidget *autosave_suffix_entry;
	GtkWidget *autosave_suffix_hbox;
	
	/* Editor->Text display */
	GtkWidget *highlight_checkbutton;
	GtkWidget *visible_whitespace_checkbutton;
	GtkWidget *use_custom_font_checkbutton;
	GtkWidget *editor_font_fontbutton;
	GtkWidget *editor_font_hbox;
	
	/* Editor->Contents */
	GtkWidget *unmark_fuzzy_checkbutton;
	GtkWidget *keep_obsolete_checkbutton;
	GtkWidget *spellcheck_checkbutton;
	
	/*PO header->Personal information*/
	GtkWidget *name_entry;
	GtkWidget *email_entry;
	
	/*PO header->Language settings*/
	GtkWidget *language_comboentry;
	GtkWidget *langcode_comboentry;
	GtkWidget *charset_comboentry;
	GtkWidget *encoding_comboentry;
	GtkWidget *team_email_comboentry;
	GtkWidget *plurals_entry;
	GtkWidget *number_plurals_spinbutton;
	
	/*Inteface*/
	GtkWidget *left_radiobutton;
	GtkWidget *right_radiobutton;
	
	/*Plugins*/
	GtkWidget *plugins_box;
};

/***************Files pages****************/

static void
warn_if_fuzzy_checkbutton_toggled(GtkToggleButton *button,
				  GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->warn_if_fuzzy_checkbutton));
	
	gtranslator_prefs_manager_set_warn_if_fuzzy(gtk_toggle_button_get_active(button));
}

static void
delete_compiled_checkbutton_toggled(GtkToggleButton *button,
				       GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->delete_compiled_checkbutton));
	
	gtranslator_prefs_manager_set_delete_compiled(gtk_toggle_button_get_active(button));
}

static void
setup_files_general_page(GtranslatorPreferencesDialog *dlg)
{	
	/*Set initial value*/
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->warn_if_fuzzy_checkbutton),
				     gtranslator_prefs_manager_get_warn_if_fuzzy());
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->delete_compiled_checkbutton),
				     gtranslator_prefs_manager_get_delete_compiled());
	
	/*Connect signals*/
	g_signal_connect(dlg->priv->warn_if_fuzzy_checkbutton, "toggled",
			 G_CALLBACK(warn_if_fuzzy_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->delete_compiled_checkbutton, "toggled",
			 G_CALLBACK(delete_compiled_checkbutton_toggled),
			 dlg);
}

static void
autosave_checkbutton_toggled(GtkToggleButton *button,
			     GtranslatorPreferencesDialog *dlg)
{
	gboolean autosave;
	
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->autosave_checkbutton));
	
	autosave = gtk_toggle_button_get_active(button);
	
	gtk_widget_set_sensitive(dlg->priv->autosave_hbox, autosave);
	gtranslator_prefs_manager_set_autosave(autosave);
}

static void
append_suffix_checkbutton_toggled(GtkToggleButton *button,
					 GtranslatorPreferencesDialog *dlg)
{
	gboolean append_suffix;
	
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->append_suffix_checkbutton));
	
	append_suffix = gtk_toggle_button_get_active(button);
	
	gtk_widget_set_sensitive(dlg->priv->autosave_suffix_hbox, append_suffix);
	gtranslator_prefs_manager_set_append_suffix(append_suffix);
}

static void
autosave_interval_spinbutton_value_changed(GtkSpinButton *spin_button,
					   GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail (spin_button == GTK_SPIN_BUTTON (dlg->priv->autosave_interval_spinbutton));
	
	gtranslator_prefs_manager_set_autosave_interval(gtk_spin_button_get_value_as_int(spin_button));
}

static void
autosave_suffix_entry_changed(GObject    *gobject,
			      GParamSpec *arg1,
			      GtranslatorPreferencesDialog *dlg)
{
	const gchar *text;
	
	text = gtk_entry_get_text(GTK_ENTRY(gobject));
	
	gtranslator_prefs_manager_set_autosave_suffix(text);
}

static void
setup_files_autosave_page(GtranslatorPreferencesDialog *dlg)
{
	gboolean autosave, suffix;
	gint autosave_interval;
	const gchar *autosave_suffix;

	/*Set initial value*/
	autosave = gtranslator_prefs_manager_get_autosave();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->autosave_checkbutton),
				     autosave);
	suffix = gtranslator_prefs_manager_get_append_suffix();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->append_suffix_checkbutton),
				     suffix);
	
	autosave_interval = gtranslator_prefs_manager_get_autosave_interval();
	
	if(autosave_interval <= 0)
		autosave_interval = GPM_DEFAULT_AUTOSAVE_INTERVAL;
	
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(dlg->priv->autosave_interval_spinbutton),
				  autosave_interval);
	
	autosave_suffix = gtranslator_prefs_manager_get_autosave_suffix();
	if(!autosave_suffix)
		autosave_suffix = GPM_DEFAULT_AUTOSAVE_SUFFIX;
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->autosave_suffix_entry),
			   autosave_suffix);
	
	
	/*Set sensitive*/
	gtk_widget_set_sensitive(dlg->priv->autosave_hbox,
				 autosave);
	gtk_widget_set_sensitive(dlg->priv->autosave_suffix_hbox,
				 suffix);
	
	/*Connect signals*/
	g_signal_connect(dlg->priv->autosave_checkbutton, "toggled",
			 G_CALLBACK(autosave_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->append_suffix_checkbutton, "toggled",
			 G_CALLBACK(append_suffix_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->autosave_interval_spinbutton, "value-changed",
			 G_CALLBACK(autosave_interval_spinbutton_value_changed),
			 dlg);
	g_signal_connect(dlg->priv->autosave_suffix_entry, "notify::text",
			 G_CALLBACK(autosave_suffix_entry_changed),
			 dlg);
}

static void
setup_files_pages(GtranslatorPreferencesDialog *dlg)
{	
	/*Children*/
	setup_files_general_page(dlg);
	setup_files_autosave_page(dlg);
}


/***************Editor pages****************/
static void
highlight_checkbutton_toggled(GtkToggleButton *button,
			      GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->highlight_checkbutton));
	
	gtranslator_prefs_manager_set_highlight(gtk_toggle_button_get_active(button));
}

static void
visible_whitespace_checkbutton_toggled(GtkToggleButton *button,
				 GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->visible_whitespace_checkbutton));
	
	gtranslator_prefs_manager_set_visible_whitespace(gtk_toggle_button_get_active(button));
}

static void
use_custom_font_checkbutton_toggled(GtkToggleButton *button,
				    GtranslatorPreferencesDialog *dlg)
{
	gboolean use_custom_font;
	
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->use_custom_font_checkbutton));

	use_custom_font = gtk_toggle_button_get_active(button);
	
	gtranslator_prefs_manager_set_use_custom_font(use_custom_font);
	
	gtk_widget_set_sensitive(dlg->priv->editor_font_hbox, use_custom_font);
}

static void
editor_font_set(GtkFontButton *button,
		 GtranslatorPreferencesDialog *dlg)
{
	const gchar *editor_font;
	
	g_return_if_fail(button == GTK_FONT_BUTTON(dlg->priv->editor_font_fontbutton));
	
	editor_font = gtk_font_button_get_font_name(GTK_FONT_BUTTON(dlg->priv->editor_font_fontbutton));
	
	if(editor_font)
		gtranslator_prefs_manager_set_editor_font(editor_font);
	//FIXME:else set default font
}

static void
setup_editor_text_display_page(GtranslatorPreferencesDialog *dlg)
{
	gboolean use_custom_font;
	const gchar *editor_font;
	
	/*Set initial value*/
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->highlight_checkbutton),
				     gtranslator_prefs_manager_get_highlight());
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->visible_whitespace_checkbutton),
				     gtranslator_prefs_manager_get_visible_whitespace());
	
	use_custom_font = gtranslator_prefs_manager_get_use_custom_font();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->use_custom_font_checkbutton),
				     use_custom_font);
	
	if(use_custom_font)
	{
		editor_font = gtranslator_prefs_manager_get_editor_font();
		if(editor_font)
			gtk_font_button_set_font_name(GTK_FONT_BUTTON(dlg->priv->editor_font_fontbutton),
						      editor_font);
		//else FIXME: I think i need to set the system font (maybe Sans 12?)
	}
	
	/*Set sensitive*/
	gtk_widget_set_sensitive(dlg->priv->editor_font_hbox, use_custom_font);
	
	/*Connect signals*/
	g_signal_connect(dlg->priv->highlight_checkbutton, "toggled",
			 G_CALLBACK(highlight_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->visible_whitespace_checkbutton, "toggled",
			 G_CALLBACK(visible_whitespace_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->use_custom_font_checkbutton, "toggled",
			 G_CALLBACK(use_custom_font_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->editor_font_fontbutton, "font-set",
			 G_CALLBACK(editor_font_set),
			 dlg);
}

static void
unmark_fuzzy_checkbutton_toggled(GtkToggleButton *button,
				 GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->unmark_fuzzy_checkbutton));
	
	gtranslator_prefs_manager_set_unmark_fuzzy(gtk_toggle_button_get_active(button));
}

static void
keep_obsolete_checkbutton_toggled(GtkToggleButton *button,
				  GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->keep_obsolete_checkbutton));

	gtranslator_prefs_manager_set_keep_obsolete(gtk_toggle_button_get_active(button));
}

static void
spellcheck_checkbutton_toggled(GtkToggleButton *button,
					   GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->spellcheck_checkbutton));
	
	gtranslator_prefs_manager_set_spellcheck(gtk_toggle_button_get_active(button));
}

static void
setup_editor_contents(GtranslatorPreferencesDialog *dlg)
{	
	/*Set initial values*/
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->unmark_fuzzy_checkbutton),
				     gtranslator_prefs_manager_get_unmark_fuzzy());
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->keep_obsolete_checkbutton),
				     gtranslator_prefs_manager_get_keep_obsolete());
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->spellcheck_checkbutton),
				     gtranslator_prefs_manager_get_spellcheck());
	
	/*Connect signals*/
	g_signal_connect(dlg->priv->unmark_fuzzy_checkbutton, "toggled",
			 G_CALLBACK(unmark_fuzzy_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->keep_obsolete_checkbutton, "toggled",
			 G_CALLBACK(keep_obsolete_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->spellcheck_checkbutton, "toggled",
			 G_CALLBACK(spellcheck_checkbutton_toggled),
			 dlg);
}

static void
setup_editor_pages(GtranslatorPreferencesDialog *dlg)
{	
	/*Children*/
	setup_editor_text_display_page(dlg);
	setup_editor_contents(dlg);
}

/***************PO header pages****************/
static void
name_entry_changed(GObject    *gobject,
		   GParamSpec *arg1,
		   GtranslatorPreferencesDialog *dlg)
{
	const gchar *text;
	
	g_return_if_fail(GTK_ENTRY(gobject) == GTK_ENTRY(dlg->priv->name_entry));

	text = gtk_entry_get_text(GTK_ENTRY(gobject));
	
	if(text)
		gtranslator_prefs_manager_set_name(text);
}

static void
email_entry_changed(GObject    *gobject,
		    GParamSpec *arg1,
		    GtranslatorPreferencesDialog *dlg)
{
	const gchar *text;
	
	g_return_if_fail(GTK_ENTRY(gobject) == GTK_ENTRY(dlg->priv->email_entry));

	text = gtk_entry_get_text(GTK_ENTRY(gobject));
	
	if(text)
		gtranslator_prefs_manager_set_email(text);
}

static void
setup_po_header_personal_information_page(GtranslatorPreferencesDialog *dlg)
{
	const gchar *value;
	
	/*Set initial value*/
	value = gtranslator_prefs_manager_get_name();
	if(value)
		gtk_entry_set_text(GTK_ENTRY(dlg->priv->name_entry),
				   value);
	value = gtranslator_prefs_manager_get_email();
	if(value)
		gtk_entry_set_text(GTK_ENTRY(dlg->priv->email_entry),
				   value);
	
	/*Connect signals*/
	g_signal_connect(dlg->priv->name_entry, "notify::text",
			 G_CALLBACK(name_entry_changed),
			 dlg);
	g_signal_connect(dlg->priv->email_entry, "notify::text",
			 G_CALLBACK(email_entry_changed),
			 dlg);
}

static void
setup_po_header_language_settings_page(GtranslatorPreferencesDialog *dlg)
{
}

static void
setup_po_header_pages(GtranslatorPreferencesDialog *dlg)
{
	/*Children*/
	setup_po_header_personal_information_page(dlg);
	setup_po_header_language_settings_page(dlg);
}


/***************Interface pages****************/


static void
setup_interface_pages(GtranslatorPreferencesDialog *dlg)
{
	
}

static void
setup_plugin_pages(GtranslatorPreferencesDialog *dlg)
{
	GtkWidget *page_content;

	page_content = gtranslator_plugin_manager_new ();
	g_return_if_fail (page_content != NULL);

	gtk_box_pack_start (GTK_BOX (dlg->priv->plugins_box),
			    page_content,
			    TRUE,
			    TRUE,
			    0);

	gtk_widget_show_all (page_content);
}


static void
dialog_response_handler (GtkDialog *dlg, 
			 gint       res_id)
{
	switch (res_id)
	{
		default:
			gtk_widget_destroy (GTK_WIDGET(dlg));
	}
}

static void
gtranslator_preferences_dialog_init (GtranslatorPreferencesDialog *dlg)
{
	gboolean ret;
	GtkWidget *error_widget;
	
	dlg->priv = GTR_PREFERENCES_DIALOG_GET_PRIVATE (dlg);
	
	gtk_dialog_add_buttons (GTK_DIALOG (dlg),
				GTK_STOCK_CLOSE,
				GTK_RESPONSE_CLOSE,
				GTK_STOCK_HELP,
				GTK_RESPONSE_HELP,
				NULL);
	
	gtk_window_set_title (GTK_WINDOW (dlg), _("Gtranslator Preferences"));
	gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
	gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);
	
	/* HIG defaults */
	gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->vbox), 2); /* 2 * 5 + 2 = 12 */
	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dlg)->action_area), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->action_area), 4);
	
	g_signal_connect (dlg,
			  "response",
			  G_CALLBACK (dialog_response_handler),
			  NULL);
	
	/*Glade*/
	
	ret = gtranslator_utils_get_glade_widgets(DATADIR "/preferences-dialog.glade",
		"notebook",
		&error_widget,
		
		"notebook", &dlg->priv->notebook,

		"warn_if_fuzzy_checkbutton", &dlg->priv->warn_if_fuzzy_checkbutton,
		"delete_compiled_checkbutton", &dlg->priv->delete_compiled_checkbutton,

		"autosave_checkbutton", &dlg->priv->autosave_checkbutton,
		"autosave_interval_spinbutton", &dlg->priv->autosave_interval_spinbutton,
		"autosave_hbox", &dlg->priv->autosave_hbox,
		"append_suffix_checkbutton", &dlg->priv->append_suffix_checkbutton,
		"autosave_suffix_entry", &dlg->priv->autosave_suffix_entry,
		"autosave_suffix_hbox", &dlg->priv->autosave_suffix_hbox,

		"highlight_checkbutton", &dlg->priv->highlight_checkbutton,
		"visible_whitespace_checkbutton", &dlg->priv->visible_whitespace_checkbutton,
		"use_custom_font_checkbutton", &dlg->priv->use_custom_font_checkbutton,
		"editor_font_fontbutton", &dlg->priv->editor_font_fontbutton,
		"editor_font_hbox", &dlg->priv->editor_font_hbox,

		"unmark_fuzzy_checkbutton", &dlg->priv->unmark_fuzzy_checkbutton,
		"keep_obsolete_checkbutton", &dlg->priv->keep_obsolete_checkbutton,
		"spellcheck_checkbutton", &dlg->priv->spellcheck_checkbutton,

		"name_entry", &dlg->priv->name_entry,
		"email_entry", &dlg->priv->email_entry,

		"language_comboentry", &dlg->priv->language_comboentry,
		"langcode_comboentry", &dlg->priv->langcode_comboentry,
		"charset_comboentry", &dlg->priv->charset_comboentry,
		"encoding_comboentry", &dlg->priv->encoding_comboentry,
		"team_email_comboentry", &dlg->priv->team_email_comboentry,
		"number_plurals_spinbutton", &dlg->priv->number_plurals_spinbutton,
		"plurals_entry", &dlg->priv->plurals_entry,
						  
		"left_radiobutton", &dlg->priv->left_radiobutton,
		"right_radiobutton", &dlg->priv->right_radiobutton,
		
		"plugins_box", &dlg->priv->plugins_box,
		NULL);
	
	if(!ret)
	{
		gtk_widget_show(error_widget);
		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dlg)->vbox),
					     error_widget);
		
		return;
	}
	
	
	
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
			    dlg->priv->notebook, FALSE, FALSE, 0);
	
	gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->notebook), 5);
	
	setup_files_pages(dlg);
	setup_editor_pages(dlg);
	setup_po_header_pages(dlg);
	setup_interface_pages(dlg);
	setup_plugin_pages(dlg);
}

static void
gtranslator_preferences_dialog_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_preferences_dialog_parent_class)->finalize (object);
}

static void
gtranslator_preferences_dialog_class_init (GtranslatorPreferencesDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorPreferencesDialogPrivate));

	object_class->finalize = gtranslator_preferences_dialog_finalize;
}

void
gtranslator_show_preferences_dialog (GtranslatorWindow *window)
{
	static GtkWidget *dlg = NULL;
	
	g_return_if_fail(GTR_IS_WINDOW(window));
	
	if(dlg == NULL)
	{
		dlg = GTK_WIDGET (g_object_new (GTR_TYPE_PREFERENCES_DIALOG, NULL));
		g_signal_connect (dlg,
				  "destroy",
				  G_CALLBACK (gtk_widget_destroyed),
				  &dlg);
		gtk_widget_show_all(dlg);
	}
	
	if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
	{
		gtk_window_set_transient_for (GTK_WINDOW (dlg),
					      GTK_WINDOW (window));
	}

	gtk_window_present (GTK_WINDOW (dlg));
}
