/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#include "assistant.h"
#include "window.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>

#define GTR_ASSISTANT_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),	\
						 GTR_TYPE_ASSISTANT,     \
						 GtranslatorAssistantPrivate))


G_DEFINE_TYPE(GtranslatorAssistant, gtranslator_assistant, GTK_TYPE_ASSISTANT)

struct _GtranslatorAssistantPrivate
{
	/* Profiles Page 1 */
	GtkWidget *profile_name;
	GtkWidget *name;
	GtkWidget *email;
	GtkWidget *language;
	GtkWidget *team_email;
	
	/* Profiles Page 2 */
	GtkWidget *lang_code;
	GtkWidget *charset;
	GtkWidget *trans_enc;
	GtkWidget *plural_form;
	
	/* Database Page */
	GtkWidget *path;
	GtkWidget *search_button;
	
	/* Confirmation Page */
	GtkWidget *confirm_label;
};

static void
on_assistant_prepare (GtkAssistant *assistant,
		      GtkWidget *page)
{
	GtranslatorAssistant *as = GTR_ASSISTANT (assistant);
	gchar *string;
	const gchar *database_path;
	
	if (page != as->priv->confirm_label)
		return;
	
	database_path = gtk_entry_get_text (GTK_ENTRY (as->priv->path));
	
	string = g_strdup_printf (_("Profile name: %s\n"
				    "Translator name: %s\n"
				    "Translator email: %s\n"
				    "Language name: %s\n"
				    "Team email: %s\n"
				    "Language code: %s\n"
				    "Character set: %s\n"
				    "Transfer encoding: %s\n"
				    "Plural form: %s\n"
				    "Database path: %s"),
				  gtk_entry_get_text (GTK_ENTRY (as->priv->profile_name)),
				  gtk_entry_get_text (GTK_ENTRY (as->priv->name)),
				  gtk_entry_get_text (GTK_ENTRY (as->priv->email)),
				  gtk_entry_get_text (GTK_ENTRY (as->priv->language)),
				  gtk_entry_get_text (GTK_ENTRY (as->priv->team_email)),
				  gtk_entry_get_text (GTK_ENTRY (as->priv->lang_code)),
				  gtk_entry_get_text (GTK_ENTRY (as->priv->charset)),
				  gtk_entry_get_text (GTK_ENTRY (as->priv->trans_enc)),
				  gtk_entry_get_text (GTK_ENTRY (as->priv->plural_form)),
				  (strcmp (database_path, "") != 0) ? database_path : _("None"));
	
	gtk_label_set_text (GTK_LABEL (as->priv->confirm_label), string);
	g_free (string);
}

static void
on_assistant_cancel (GtkAssistant* assistant)
{
	gtk_widget_destroy (GTK_WIDGET (assistant));
}

static void
create_start_page (GtranslatorAssistant *as)
{
	GtkWidget *box, *label;

	box = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (box);
	gtk_container_set_border_width (GTK_CONTAINER (box), 12);

	label = gtk_label_new (_("This assistant will help you to create the main profile\n"
			       "and generate you translation memory database."));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (box), label, TRUE, TRUE, 0);

	gtk_assistant_append_page (GTK_ASSISTANT (as), box);
	gtk_assistant_set_page_title (GTK_ASSISTANT (as), box, _("Assistant"));
	gtk_assistant_set_page_complete (GTK_ASSISTANT (as), box, TRUE);
	gtk_assistant_set_page_type (GTK_ASSISTANT (as), box, GTK_ASSISTANT_PAGE_INTRO);
}

static void
on_profile1_entry_changed (GtkWidget *widget,
			   GtranslatorAssistant *as)
{
	const gchar *text;
	GtkWidget *current_page;
	gint page_number;
	
	page_number = gtk_assistant_get_current_page (GTK_ASSISTANT (as));
	current_page = gtk_assistant_get_nth_page (GTK_ASSISTANT (as), page_number);
	
	/* Profile name */
	text = gtk_entry_get_text (GTK_ENTRY (as->priv->profile_name));
	
	if (text && *text)
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
	else
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
	
	/* Translator name */
	text = gtk_entry_get_text (GTK_ENTRY (as->priv->name));
	
	if (text && *text)
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
	else
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
	
	/* Translator email */
	text = gtk_entry_get_text (GTK_ENTRY (as->priv->email));
	
	if (text && *text)
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
	else
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
	
	/* Language */
	text = gtk_entry_get_text (GTK_ENTRY (as->priv->language));
	
	if (text && *text)
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
	else
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
	
	/* Team email */
	text = gtk_entry_get_text (GTK_ENTRY (as->priv->team_email));
	
	if (text && *text)
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
	else
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
}

static void
create_profiles_page1 (GtranslatorAssistant *as)
{
	GtkWidget *box, *hbox;
	GtkWidget *label;
	GtranslatorAssistantPrivate *priv = as->priv;

	box = gtk_vbox_new (FALSE, 6);
	gtk_widget_show (box);
	gtk_container_set_border_width (GTK_CONTAINER (box), 5);

	/*
	 * Profile name:
	 */
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
	
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label),
			      _("<b>Profile name:</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	
	priv->profile_name = gtk_entry_new ();
	gtk_widget_show (priv->profile_name);
	gtk_box_pack_start (GTK_BOX (hbox), priv->profile_name, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (priv->profile_name), "changed",
			  G_CALLBACK (on_profile1_entry_changed), as);
	
	/*
	 * Translator name:
	 */
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
	
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label),
			      _("<b>Translator name:</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	
	priv->name = gtk_entry_new ();
	gtk_widget_show (priv->name);
	gtk_box_pack_start (GTK_BOX (hbox), priv->name, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (priv->name), "changed",
			  G_CALLBACK (on_profile1_entry_changed), as);
	
	/*
	 * Translator email:
	 */
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
	
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label),
			      _("<b>Translator email:</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	
	priv->email = gtk_entry_new ();
	gtk_widget_show (priv->email);
	gtk_box_pack_start (GTK_BOX (hbox), priv->email, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (priv->email), "changed",
			  G_CALLBACK (on_profile1_entry_changed), as);
	
	/*
	 * Translator language:
	 */
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
	
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label),
			      _("<b>Language:</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	
	priv->language = gtk_entry_new ();
	gtk_widget_show (priv->language);
	gtk_box_pack_start (GTK_BOX (hbox), priv->language, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (priv->language), "changed",
			  G_CALLBACK (on_profile1_entry_changed), as);
	
	/*
	 * Translator team email:
	 */
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
	
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label),
			      _("<b>Team email:</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	
	priv->team_email = gtk_entry_new ();
	gtk_widget_show (priv->team_email);
	gtk_box_pack_start (GTK_BOX (hbox), priv->team_email, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (priv->team_email), "changed",
			  G_CALLBACK (on_profile1_entry_changed), as);

	gtk_assistant_append_page (GTK_ASSISTANT (as), box);
	gtk_assistant_set_page_title (GTK_ASSISTANT (as), box, _("Profile"));
}

static void
on_profile2_entry_changed (GtkWidget *widget,
			   GtranslatorAssistant *as)
{
	const gchar *text;
	GtkWidget *current_page;
	gint page_number;
	
	page_number = gtk_assistant_get_current_page (GTK_ASSISTANT (as));
	current_page = gtk_assistant_get_nth_page (GTK_ASSISTANT (as), page_number);
	
	/* Lang code */
	text = gtk_entry_get_text (GTK_ENTRY (as->priv->lang_code));
	
	if (text && *text)
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
	else
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
	
	/* Charset */
	text = gtk_entry_get_text (GTK_ENTRY (as->priv->charset));
	
	if (text && *text)
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
	else
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
	
	/* Trans encoding */
	text = gtk_entry_get_text (GTK_ENTRY (as->priv->trans_enc));
	
	if (text && *text)
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
	else
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
	
	
	/* Plural form */
	text = gtk_entry_get_text (GTK_ENTRY (as->priv->plural_form));
	
	if (text && *text)
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
	else
		gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
}

static void
create_profiles_page2 (GtranslatorAssistant *as)
{
	GtkWidget *box, *hbox;
	GtkWidget *label;
	GtranslatorAssistantPrivate *priv = as->priv;

	box = gtk_vbox_new (FALSE, 6);
	gtk_widget_show (box);
	gtk_container_set_border_width (GTK_CONTAINER (box), 5);

	/*
	 * Language code:
	 */
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
	
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label),
			      _("<b>Language code:</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	
	priv->lang_code = gtk_entry_new ();
	gtk_widget_show (priv->lang_code);
	gtk_box_pack_start (GTK_BOX (hbox), priv->lang_code, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (priv->lang_code), "changed",
			  G_CALLBACK (on_profile2_entry_changed), as);
	
	/*
	 * Charset:
	 */
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
	
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label),
			      _("<b>Character set:</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	
	priv->charset = gtk_entry_new ();
	gtk_widget_show (priv->charset);
	gtk_box_pack_start (GTK_BOX (hbox), priv->charset, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (priv->charset), "changed",
			  G_CALLBACK (on_profile2_entry_changed), as);
	
	/*
	 * Transfer enconding:
	 */
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
	
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label),
			      _("<b>Transfer encoding:</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	
	priv->trans_enc = gtk_entry_new ();
	gtk_widget_show (priv->trans_enc);
	gtk_box_pack_start (GTK_BOX (hbox), priv->trans_enc, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (priv->trans_enc), "changed",
			  G_CALLBACK (on_profile2_entry_changed), as);
	
	/*
	 * Plural form:
	 */
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
	
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label),
			      _("<b>Plural forms:</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	
	priv->plural_form = gtk_entry_new ();
	gtk_widget_show (priv->plural_form);
	gtk_box_pack_start (GTK_BOX (hbox), priv->plural_form, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (priv->plural_form), "changed",
			  G_CALLBACK (on_profile2_entry_changed), as);

	gtk_assistant_append_page (GTK_ASSISTANT (as), box);
	gtk_assistant_set_page_title (GTK_ASSISTANT (as), box, _("Profile"));
}

static void
on_dir_find_button_clicked (GtkButton *button,
			    GtranslatorAssistant *as)
{
	GtkWidget *dialog;
	gint res;
	
	dialog = gtk_file_chooser_dialog_new (_("Checkout directory"),
					      GTK_WINDOW (as),
					      GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
					      GTK_STOCK_CANCEL,
					      GTK_RESPONSE_CANCEL,
					      GTK_STOCK_OK,
					      GTK_RESPONSE_OK,
					      NULL);
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (res)
	{
		case GTK_RESPONSE_OK: 
		{
			gchar *filename;
			
			filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			gtk_entry_set_text (GTK_ENTRY (as->priv->path),
					    filename);
			g_free (filename);
			break;
		}
		default:
			break;
	}
	gtk_widget_destroy (dialog);
}

static void
create_database_page (GtranslatorAssistant *as)
{
	GtkWidget *box, *hbox;
	GtkWidget *label;
	GtranslatorAssistantPrivate *priv = as->priv;

	box = gtk_vbox_new (FALSE, 6);
	gtk_widget_show (box);
	gtk_container_set_border_width (GTK_CONTAINER (box), 5);
	
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label),
			      _("<b>Select the path to generate the database:</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 0);
	
	/* hbox */
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);
	
	/* Path entry */
	priv->path = gtk_entry_new ();
	gtk_widget_show (priv->path);
	gtk_box_pack_start (GTK_BOX (hbox), priv->path, TRUE, TRUE, 0);
	
	/* Search button */
	priv->search_button = gtk_button_new_from_stock (GTK_STOCK_FIND);
	gtk_widget_show (priv->search_button);
	gtk_box_pack_start (GTK_BOX (hbox), priv->search_button,
			    FALSE, FALSE, 0);
	g_signal_connect (priv->search_button, "clicked",
			  G_CALLBACK (on_dir_find_button_clicked), as);
	
	gtk_assistant_append_page (GTK_ASSISTANT (as), box);
	gtk_assistant_set_page_title (GTK_ASSISTANT (as), box, _("Generate Database"));
	gtk_assistant_set_page_complete (GTK_ASSISTANT (as), box, TRUE);
}

static void
create_finish_page (GtranslatorAssistant *as)
{
	as->priv->confirm_label = gtk_label_new (NULL);
	gtk_widget_show (as->priv->confirm_label);
	
	gtk_assistant_append_page (GTK_ASSISTANT (as), as->priv->confirm_label);
	gtk_assistant_set_page_type (GTK_ASSISTANT (as), as->priv->confirm_label,
				     GTK_ASSISTANT_PAGE_CONFIRM);
	gtk_assistant_set_page_complete (GTK_ASSISTANT (as), as->priv->confirm_label, TRUE);
	gtk_assistant_set_page_title (GTK_ASSISTANT (as), as->priv->confirm_label, _("Confirmation"));
}

static void
gtranslator_assistant_init (GtranslatorAssistant *as)
{	
	as->priv = GTR_ASSISTANT_GET_PRIVATE (as);
	
	//gtk_window_set_default_size (GTK_WINDOW (as), 300, 200);
	gtk_window_set_resizable (GTK_WINDOW (as), TRUE);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (as), TRUE);
	
	create_start_page (as);
	create_profiles_page1 (as);
	create_profiles_page2 (as);
	create_database_page (as);
	create_finish_page (as);
}

static void
gtranslator_assistant_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_assistant_parent_class)->finalize (object);
}

static void
gtranslator_assistant_class_init (GtranslatorAssistantClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkAssistantClass *assistant_class = GTK_ASSISTANT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorAssistantPrivate));

	object_class->finalize = gtranslator_assistant_finalize;
	assistant_class->prepare = on_assistant_prepare;
	/*assistant_class->apply = on_assistant_apply;*/
	assistant_class->cancel = on_assistant_cancel;
	assistant_class->close = on_assistant_cancel;
}

void
gtranslator_show_assistant (GtranslatorWindow *window)
{
	static GtranslatorAssistant *assist = NULL;
	
	g_return_if_fail(GTR_IS_WINDOW(window));
	
	if(assist == NULL)
	{
		assist = g_object_new (GTR_TYPE_ASSISTANT, NULL);
		g_signal_connect (assist,
				  "destroy",
				  G_CALLBACK (gtk_widget_destroyed),
				  &assist);

		gtk_widget_show (GTK_WIDGET(assist));
	}
	
	if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (assist)))
	{
		gtk_window_set_transient_for (GTK_WINDOW (assist),
					      GTK_WINDOW (window));
	}

	gtk_window_present (GTK_WINDOW (assist));
}
