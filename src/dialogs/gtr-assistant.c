/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *                     Igalia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>  
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../translation-memory/gtr-translation-memory.h"
#include "gtr-application.h"
#include "gtr-assistant.h"
#include "gtr-profile.h"
#include "gtr-utils.h"
#include "gtr-window.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include <gio/gio.h>

#define GTR_ASSISTANT_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),	\
						 GTR_TYPE_ASSISTANT,     \
						 GtrAssistantPrivate))


G_DEFINE_TYPE (GtrAssistant, gtranslator_assistant,
	       GTK_TYPE_ASSISTANT)

struct _GtrAssistantPrivate
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
  GtkWidget *po_name;

  /* Confirmation Page */
  GtkWidget *finish_box;
  GtkWidget *confirm_label;
  GtkWidget *add_db_progressbar;
};

typedef struct _IdleData
{
  GSList *list;
  GtkProgressBar *progress;
  GtrTranslationMemory *tm;
  GtkWindow *parent;
} IdleData;

static gboolean
add_to_database (gpointer data_pointer)
{
  IdleData *data = (IdleData *) data_pointer;
  static GSList *l = NULL;
  gdouble percentage;

  if (l == NULL)
    l = data->list;
  else
    l = g_slist_next (l);

  if (l)
    {
      GList *msg_list = NULL;
      GList *l2 = NULL;
      GFile *location;
      GError *error = NULL;
      GtrPo *po;

      po = gtranslator_po_new ();
      location = (GFile *) l->data;

      gtranslator_po_parse (po, location, &error);
      if (error)
	return TRUE;

      msg_list = gtranslator_po_get_messages (po);

      for (l2 = msg_list; l2; l2 = g_list_next (l2))
	{
	  GtrMsg *msg;

	  msg = GTR_MSG (l2->data);
	  if (gtranslator_msg_is_translated (msg))
	    gtranslator_translation_memory_store (data->tm,
						  gtranslator_msg_get_msgid
						  (msg),
						  gtranslator_msg_get_msgstr
						  (msg));
	}

      g_object_unref (po);
    }
  else
    {
      GtkWidget *dialog;

      gtk_progress_bar_set_fraction (data->progress, 1.0);

      dialog = gtk_message_dialog_new (data->parent,
				       GTK_DIALOG_DESTROY_WITH_PARENT,
				       GTK_MESSAGE_INFO,
				       GTK_BUTTONS_CLOSE, NULL);

      gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog),
				     _("<span weight=\"bold\" size=\"large\">Strings added to database</span>"));

      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);

      return FALSE;
    }

  percentage =
    (gdouble) g_slist_position (data->list,
				l) / (gdouble) g_slist_length (data->list);

  /*
   * Set the progress only if the values are reasonable.
   */
  if (percentage > 0.0 || percentage < 1.0)
    {
      /*
       * Set the progressbar status.
       */
      gtk_progress_bar_set_fraction (data->progress, percentage);
    }

  return TRUE;
}

static void
destroy_idle_data (gpointer data)
{
  IdleData *d = (IdleData *) data;

  gtk_widget_hide (GTK_WIDGET (d->progress));

  g_slist_foreach (d->list, (GFunc) g_object_unref, NULL);
  g_slist_free (d->list);

  gtk_widget_destroy (GTK_WIDGET (d->parent));

  g_free (d);
}

static void
on_assistant_close (GtkAssistant * assistant)
{
  gtk_widget_destroy (GTK_WIDGET (assistant));
}

static void
on_assistant_apply (GtkAssistant * assistant)
{
  GFile *dir;
  const gchar *dir_name;
  IdleData *data;
  GtrAssistant *as = GTR_ASSISTANT (assistant);
  const gchar *po_name;
  GtrProfile *profile;
  GList *profiles_list;
  gulong close_signal_id;

  profile = gtranslator_profile_new ();

  gtranslator_profile_set_name (profile,
				gtk_entry_get_text (GTK_ENTRY
						    (as->priv->
						     profile_name)));

  gtranslator_profile_set_author_name (profile,
				       gtk_entry_get_text (GTK_ENTRY
							   (as->priv->name)));

  gtranslator_profile_set_author_email (profile,
					gtk_entry_get_text (GTK_ENTRY
							    (as->priv->
							     email)));

  gtranslator_profile_set_language_name (profile,
					 gtk_entry_get_text (GTK_ENTRY
							     (as->priv->
							      language)));

  gtranslator_profile_set_language_code (profile,
					 gtk_entry_get_text (GTK_ENTRY
							     (as->priv->
							      lang_code)));

  gtranslator_profile_set_group_email (profile,
				       gtk_entry_get_text (GTK_ENTRY
							   (as->priv->
							    team_email)));

  gtranslator_profile_set_charset (profile,
				   gtk_entry_get_text (GTK_ENTRY
						       (as->priv->charset)));

  gtranslator_profile_set_encoding (profile,
				    gtk_entry_get_text (GTK_ENTRY
							(as->priv->
							 trans_enc)));

  gtranslator_profile_set_plurals (profile,
				   gtk_entry_get_text (GTK_ENTRY
						       (as->priv->
							plural_form)));

  gtranslator_application_set_active_profile (GTR_APP, profile);

  profiles_list = gtranslator_application_get_profiles (GTR_APP);

  gtranslator_application_set_profiles (GTR_APP,
					g_list_append (profiles_list,
						       profile));

  close_signal_id = g_signal_connect (as,
				      "close",
				      G_CALLBACK (on_assistant_close), NULL);

  dir_name = gtk_entry_get_text (GTK_ENTRY (as->priv->path));
  if (strcmp (dir_name, "") == 0)
    return;

  g_signal_handler_block (as, close_signal_id);

  data = g_new0 (IdleData, 1);
  data->list = NULL;

  dir = g_file_new_for_path (dir_name);

  po_name = gtk_entry_get_text (GTK_ENTRY (as->priv->po_name));

  gtranslator_utils_scan_dir (dir, &data->list, po_name);

  data->tm =
    GTR_TRANSLATION_MEMORY (gtranslator_application_get_translation_memory
			    (GTR_APP));
  data->progress = GTK_PROGRESS_BAR (as->priv->add_db_progressbar);
  data->parent = GTK_WINDOW (as);

  gtk_widget_show (as->priv->add_db_progressbar);

  g_idle_add_full (G_PRIORITY_HIGH_IDLE + 30,
		   (GSourceFunc) add_to_database,
		   data, (GDestroyNotify) destroy_idle_data);

  g_object_unref (dir);
}

static void
on_assistant_prepare (GtkAssistant * assistant, GtkWidget * page)
{
  GtrAssistant *as = GTR_ASSISTANT (assistant);
  gchar *string;
  const gchar *database_path;

  if (page != as->priv->finish_box)
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
			    gtk_entry_get_text (GTK_ENTRY
						(as->priv->profile_name)),
			    gtk_entry_get_text (GTK_ENTRY (as->priv->name)),
			    gtk_entry_get_text (GTK_ENTRY (as->priv->email)),
			    gtk_entry_get_text (GTK_ENTRY
						(as->priv->language)),
			    gtk_entry_get_text (GTK_ENTRY
						(as->priv->team_email)),
			    gtk_entry_get_text (GTK_ENTRY
						(as->priv->lang_code)),
			    gtk_entry_get_text (GTK_ENTRY
						(as->priv->charset)),
			    gtk_entry_get_text (GTK_ENTRY
						(as->priv->trans_enc)),
			    gtk_entry_get_text (GTK_ENTRY
						(as->priv->plural_form)),
			    (strcmp (database_path, "") !=
			     0) ? database_path : _("None"));

  gtk_label_set_text (GTK_LABEL (as->priv->confirm_label), string);
  g_free (string);
}

static void
on_assistant_cancel (GtkAssistant * assistant)
{
  gtk_widget_destroy (GTK_WIDGET (assistant));
}

static void
create_start_page (GtrAssistant * as)
{
  GtkWidget *box, *label;

  box = gtk_hbox_new (FALSE, 12);
  gtk_widget_show (box);
  gtk_container_set_border_width (GTK_CONTAINER (box), 12);

  label =
    gtk_label_new (_("This assistant will help you to create the main profile\n"
		   "and generate your translation memory database."));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (box), label, TRUE, TRUE, 0);

  gtk_assistant_append_page (GTK_ASSISTANT (as), box);
  gtk_assistant_set_page_title (GTK_ASSISTANT (as), box, _("Assistant"));
  gtk_assistant_set_page_complete (GTK_ASSISTANT (as), box, TRUE);
  gtk_assistant_set_page_type (GTK_ASSISTANT (as), box,
			       GTK_ASSISTANT_PAGE_INTRO);
}

static void
on_profile1_entry_changed (GtkWidget * widget, GtrAssistant * as)
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
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
				       FALSE);
      return;
    }

  /* Translator name */
  text = gtk_entry_get_text (GTK_ENTRY (as->priv->name));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
				       FALSE);
      return;
    }

  /* Translator email */
  text = gtk_entry_get_text (GTK_ENTRY (as->priv->email));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
				       FALSE);
      return;
    }

  /* Language */
  text = gtk_entry_get_text (GTK_ENTRY (as->priv->language));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
				       FALSE);
      return;
    }

  /* Team email */
  text = gtk_entry_get_text (GTK_ENTRY (as->priv->team_email));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
}

static void
create_profiles_page1 (GtrAssistant * as)
{
  GtkWidget *box, *hbox;
  GtkWidget *label;
  GtrAssistantPrivate *priv = as->priv;

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
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Profile name:</b>"));
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
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Translator name:</b>"));
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
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Translator email:</b>"));
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
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Language:</b>"));
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
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Team email:</b>"));
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
on_profile2_entry_changed (GtkWidget * widget, GtrAssistant * as)
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
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
				       FALSE);
      return;
    }

  /* Charset */
  text = gtk_entry_get_text (GTK_ENTRY (as->priv->charset));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
				       FALSE);
      return;
    }

  /* Trans encoding */
  text = gtk_entry_get_text (GTK_ENTRY (as->priv->trans_enc));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
				       FALSE);
      return;
    }

  /* Plural form */
  text = gtk_entry_get_text (GTK_ENTRY (as->priv->plural_form));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
}

static void
create_profiles_page2 (GtrAssistant * as)
{
  GtkWidget *box, *hbox;
  GtkWidget *label;
  GtrAssistantPrivate *priv = as->priv;

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
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Language code:</b>"));
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
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Character set:</b>"));
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
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Transfer encoding:</b>"));
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
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Plural forms:</b>"));
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
on_dir_find_button_clicked (GtkButton * button, GtrAssistant * as)
{
  GtkWidget *dialog;
  gint res;

  dialog = gtk_file_chooser_dialog_new (_("Checkout directory"),
					GTK_WINDOW (as),
					GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL,
					GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
  res = gtk_dialog_run (GTK_DIALOG (dialog));
  switch (res)
    {
    case GTK_RESPONSE_OK:
      {
	gchar *filename;

	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	gtk_entry_set_text (GTK_ENTRY (as->priv->path), filename);
	g_free (filename);
	break;
      }
    default:
      break;
    }
  gtk_widget_destroy (dialog);
}

static void
create_database_page (GtrAssistant * as)
{
  GtkWidget *box, *hbox;
  GtkWidget *label;
  GtrAssistantPrivate *priv = as->priv;

  box = gtk_vbox_new (FALSE, 6);
  gtk_widget_show (box);
  gtk_container_set_border_width (GTK_CONTAINER (box), 5);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label),
			_
			("<b>Select the path to generate the database:</b>"));
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
  gtk_box_pack_start (GTK_BOX (hbox), priv->search_button, FALSE, FALSE, 0);
  g_signal_connect (priv->search_button, "clicked",
		    G_CALLBACK (on_dir_find_button_clicked), as);

  /* Po name label */
  hbox = gtk_hbox_new (FALSE, 12);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label),
			_("<b>Look for a specific PO filename:</b>"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

  /* Po name entry */
  priv->po_name = gtk_entry_new ();
  gtk_widget_show (priv->po_name);
  gtk_box_pack_start (GTK_BOX (hbox), priv->po_name, TRUE, TRUE, 0);
  gtk_widget_set_tooltip_text (priv->po_name, _("E.g.: gl.po"));

  gtk_assistant_append_page (GTK_ASSISTANT (as), box);
  gtk_assistant_set_page_title (GTK_ASSISTANT (as), box,
				_("Generate Database"));
  gtk_assistant_set_page_complete (GTK_ASSISTANT (as), box, TRUE);
}

static void
create_finish_page (GtrAssistant * as)
{
  as->priv->finish_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (as->priv->finish_box);

  as->priv->confirm_label = gtk_label_new (NULL);
  gtk_widget_show (as->priv->confirm_label);
  gtk_box_pack_start (GTK_BOX (as->priv->finish_box), as->priv->confirm_label,
		      TRUE, TRUE, 0);

  as->priv->add_db_progressbar = gtk_progress_bar_new ();
  gtk_widget_show (as->priv->add_db_progressbar);
  gtk_box_pack_start (GTK_BOX (as->priv->finish_box),
		      as->priv->add_db_progressbar, FALSE, FALSE, 0);

  gtk_assistant_append_page (GTK_ASSISTANT (as), as->priv->finish_box);
  gtk_assistant_set_page_type (GTK_ASSISTANT (as), as->priv->finish_box,
			       GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (as), as->priv->finish_box,
				   TRUE);
  gtk_assistant_set_page_title (GTK_ASSISTANT (as), as->priv->finish_box,
				_("Confirmation"));
}

static void
gtranslator_assistant_init (GtrAssistant * as)
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
gtranslator_assistant_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtranslator_assistant_parent_class)->finalize (object);
}

static void
gtranslator_assistant_class_init (GtrAssistantClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkAssistantClass *assistant_class = GTK_ASSISTANT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrAssistantPrivate));

  object_class->finalize = gtranslator_assistant_finalize;
  assistant_class->prepare = on_assistant_prepare;
  assistant_class->apply = on_assistant_apply;
  assistant_class->cancel = on_assistant_cancel;
  //assistant_class->close = on_assistant_close;
}

void
gtranslator_show_assistant (GtrWindow * window)
{
  static GtrAssistant *assist = NULL;

  g_return_if_fail (GTR_IS_WINDOW (window));

  if (assist == NULL)
    {
      assist = g_object_new (GTR_TYPE_ASSISTANT, NULL);
      g_signal_connect (assist,
			"destroy",
			G_CALLBACK (gtk_widget_destroyed), &assist);

      gtk_widget_show (GTK_WIDGET (assist));
    }

  /* Make asssitant modal preventing using application without profile */
  gtk_window_set_modal (GTK_WINDOW (assist), TRUE);

  if (GTK_WINDOW (window) !=
      gtk_window_get_transient_for (GTK_WINDOW (assist)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (assist), GTK_WINDOW (window));
    }

  gtk_window_present (GTK_WINDOW (assist));
}
