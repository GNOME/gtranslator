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

#include "gtr-application.h"
#include "gtr-assistant.h"
#include "gtr-profile.h"
#include "gtr-profile-manager.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "gtr-languages-fetcher.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include <gio/gio.h>

typedef struct
{
  /* Profiles Page 1 */
  GtkWidget *profile_name;
  GtkWidget *name;
  GtkWidget *email;

  /* Profiles Page 2 */
  GtkWidget *languages_fetcher;

  /* Confirmation Page */
  GtkWidget *finish_box;
  GtkWidget *confirm_label;
} GtrAssistantPrivate;

G_DEFINE_TYPE_WITH_CODE (GtrAssistant, gtr_assistant, GTK_TYPE_ASSISTANT,
                         G_ADD_PRIVATE (GtrAssistant))

static void
on_assistant_close (GtkAssistant * assistant)
{
  gtk_widget_destroy (GTK_WIDGET (assistant));
}

static void
on_assistant_apply (GtkAssistant * assistant)
{
  GtrAssistant *as = GTR_ASSISTANT (assistant);
  GtrProfile *profile;
  GtrProfileManager *prof_manager;
  GtrAssistantPrivate *priv = gtr_assistant_get_instance_private (as);

  profile = gtr_profile_new ();
  prof_manager = gtr_profile_manager_get_default ();

  gtr_profile_set_name (profile,
                        gtk_entry_get_text (GTK_ENTRY
                                            (priv->profile_name)));

  gtr_profile_set_author_name (profile,
                               gtk_entry_get_text (GTK_ENTRY
                                                   (priv->name)));

  gtr_profile_set_author_email (profile,
                                gtk_entry_get_text (GTK_ENTRY
                                                    (priv->email)));

  gtr_profile_set_language_name (profile,
                                 gtr_languages_fetcher_get_language_name (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  gtr_profile_set_language_code (profile,
                                 gtr_languages_fetcher_get_language_code (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  gtr_profile_set_charset (profile,
                           gtr_languages_fetcher_get_charset (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  gtr_profile_set_encoding (profile,
                            gtr_languages_fetcher_get_encoding (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  gtr_profile_set_group_email (profile,
                               gtr_languages_fetcher_get_team_email (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  gtr_profile_set_plural_forms (profile,
                                gtr_languages_fetcher_get_plural_form (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  /* Add profile to profile manager and save it */
  gtr_profile_manager_add_profile (prof_manager, profile);

  g_object_unref (prof_manager);

  g_signal_connect (as, "close",
                    G_CALLBACK (on_assistant_close), NULL);
}

static void
on_assistant_prepare (GtkAssistant * assistant, GtkWidget * page)
{
  GtrAssistant *as = GTR_ASSISTANT (assistant);
  GtrAssistantPrivate *priv = gtr_assistant_get_instance_private (as);
  gchar *string;

  if (page != priv->finish_box)
    return;

  string = g_strdup_printf (_("Profile name: %s\n"
                              "Translator name: %s\n"
                              "Translator email: %s\n"
                              "Language name: %s\n"
                              "Team email: %s\n"
                              "Language code: %s\n"
                              "Character set: %s\n"
                              "Transfer encoding: %s\n"
                              "Plural form: %s\n"),
                            gtk_entry_get_text (GTK_ENTRY
                                                (priv->profile_name)),
                            gtk_entry_get_text (GTK_ENTRY (priv->name)),
                            gtk_entry_get_text (GTK_ENTRY (priv->email)),
                            gtr_languages_fetcher_get_language_name (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)),
                            gtr_languages_fetcher_get_team_email (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)),
                            gtr_languages_fetcher_get_language_code (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)),
                            gtr_languages_fetcher_get_charset (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)),
                            gtr_languages_fetcher_get_encoding (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)),
                            gtr_languages_fetcher_get_plural_form (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  gtk_label_set_text (GTK_LABEL (priv->confirm_label), string);
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

  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_show (box);
  gtk_container_set_border_width (GTK_CONTAINER (box), 12);

  label = gtk_label_new (_("This assistant will help you to create the main profile."));
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
  GtrAssistantPrivate *priv = gtr_assistant_get_instance_private (as);

  page_number = gtk_assistant_get_current_page (GTK_ASSISTANT (as));
  current_page = gtk_assistant_get_nth_page (GTK_ASSISTANT (as), page_number);

  /* Profile name */
  text = gtk_entry_get_text (GTK_ENTRY (priv->profile_name));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
                                       FALSE);
      return;
    }

  /* Translator name */
  text = gtk_entry_get_text (GTK_ENTRY (priv->name));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
                                       FALSE);
      return;
    }

  /* Translator email */
  text = gtk_entry_get_text (GTK_ENTRY (priv->email));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
                                       FALSE);
      return;
    }
}

static void
create_profiles_page1 (GtrAssistant * as)
{
  GtkWidget *box, *hbox;
  GtkWidget *label;
  GtrAssistantPrivate *priv = gtr_assistant_get_instance_private (as);
  gchar *markup;

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
  gtk_widget_show (box);
  gtk_container_set_border_width (GTK_CONTAINER (box), 5);

  /*
   * Profile name:
   */
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);

  label = gtk_label_new (NULL);
  markup = g_strdup_printf("<b>%s</b>", _("Profile name:"));
  gtk_label_set_markup (GTK_LABEL (label), markup);
  g_free(markup);
  gtk_label_set_xalign (GTK_LABEL (label), 0.0);
  gtk_label_set_yalign (GTK_LABEL (label), 0.5);
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
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);

  label = gtk_label_new (NULL);
  markup = g_strdup_printf("<b>%s</b>", ("Translator name:"));
  gtk_label_set_markup (GTK_LABEL (label), markup);
  g_free(markup);
  gtk_label_set_xalign (GTK_LABEL (label), 0.0);
  gtk_label_set_yalign (GTK_LABEL (label), 0.5);
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
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);

  label = gtk_label_new (NULL);
  markup = g_strdup_printf("<b>%s</b>", _("Translator email:"));
  gtk_label_set_markup (GTK_LABEL (label), markup);
  g_free(markup);
  gtk_label_set_xalign (GTK_LABEL (label), 0.0);
  gtk_label_set_yalign (GTK_LABEL (label), 0.5);
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

  priv->email = gtk_entry_new ();
  gtk_widget_show (priv->email);
  gtk_box_pack_start (GTK_BOX (hbox), priv->email, FALSE, FALSE, 0);
  g_signal_connect (G_OBJECT (priv->email), "changed",
                    G_CALLBACK (on_profile1_entry_changed), as);

  gtk_assistant_append_page (GTK_ASSISTANT (as), box);
  gtk_assistant_set_page_title (GTK_ASSISTANT (as), box, _("Profile"));
}

static void
on_profile2_entry_changed (GtkWidget * widget, GtrAssistant * as)
{
  GtrAssistantPrivate *priv = gtr_assistant_get_instance_private (as);
  const gchar *text;
  GtkWidget *current_page;
  gint page_number;

  page_number = gtk_assistant_get_current_page (GTK_ASSISTANT (as));
  current_page = gtk_assistant_get_nth_page (GTK_ASSISTANT (as), page_number);

  /* Language */
  text = gtr_languages_fetcher_get_language_name (GTR_LANGUAGES_FETCHER (priv->languages_fetcher));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
                                       FALSE);
      return;
    }

  /* Lang code */
  text = gtr_languages_fetcher_get_language_code (GTR_LANGUAGES_FETCHER (priv->languages_fetcher));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
                                       FALSE);
      return;
    }

  /* Charset */
  text = gtr_languages_fetcher_get_charset (GTR_LANGUAGES_FETCHER (priv->languages_fetcher));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
                                       FALSE);
      return;
    }

  /* Trans encoding */
  text = gtr_languages_fetcher_get_encoding (GTR_LANGUAGES_FETCHER (priv->languages_fetcher));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    {
      gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page,
                                       FALSE);
      return;
    }

  /* Team email */
  text = gtr_languages_fetcher_get_team_email (GTR_LANGUAGES_FETCHER (priv->languages_fetcher));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);

  /* Plural form */
  text = gtr_languages_fetcher_get_plural_form (GTR_LANGUAGES_FETCHER (priv->languages_fetcher));

  if (text && *text)
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, TRUE);
  else
    gtk_assistant_set_page_complete (GTK_ASSISTANT (as), current_page, FALSE);
}

static void
create_profiles_page2 (GtrAssistant * as)
{
  GtrAssistantPrivate *priv = gtr_assistant_get_instance_private (as);

  priv->languages_fetcher = gtr_languages_fetcher_new ();
  gtk_widget_show (priv->languages_fetcher);
  gtk_container_set_border_width (GTK_CONTAINER (priv->languages_fetcher), 5);
  g_signal_connect (priv->languages_fetcher, "changed",
                    G_CALLBACK (on_profile2_entry_changed),
                    as);

  gtk_assistant_append_page (GTK_ASSISTANT (as), priv->languages_fetcher);
  gtk_assistant_set_page_title (GTK_ASSISTANT (as), priv->languages_fetcher, _("Language"));
}

static void
create_finish_page (GtrAssistant * as)
{
  GtrAssistantPrivate *priv = gtr_assistant_get_instance_private (as);
  priv->finish_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (priv->finish_box);

  priv->confirm_label = gtk_label_new (NULL);
  gtk_widget_show (priv->confirm_label);
  gtk_box_pack_start (GTK_BOX (priv->finish_box), priv->confirm_label,
                      TRUE, TRUE, 0);

  gtk_assistant_append_page (GTK_ASSISTANT (as), priv->finish_box);
  gtk_assistant_set_page_type (GTK_ASSISTANT (as), priv->finish_box,
                               GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (as), priv->finish_box,
                                   TRUE);
  gtk_assistant_set_page_title (GTK_ASSISTANT (as), priv->finish_box,
                                _("Confirmation"));
}

static void
gtr_assistant_init (GtrAssistant * as)
{
  gtk_window_set_resizable (GTK_WINDOW (as), TRUE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (as), TRUE);

  create_start_page (as);
  create_profiles_page1 (as);
  create_profiles_page2 (as);
  create_finish_page (as);
}

static void
gtr_assistant_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_assistant_parent_class)->finalize (object);
}

static void
gtr_assistant_class_init (GtrAssistantClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkAssistantClass *assistant_class = GTK_ASSISTANT_CLASS (klass);

  object_class->finalize = gtr_assistant_finalize;
  assistant_class->prepare = on_assistant_prepare;
  assistant_class->apply = on_assistant_apply;
  assistant_class->cancel = on_assistant_cancel;
  //assistant_class->close = on_assistant_close;
}

void
gtr_show_assistant (GtrWindow * window)
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

  gtk_window_set_transient_for (GTK_WINDOW (assist), GTK_WINDOW (window));
  gtk_window_set_attached_to (GTK_WINDOW (assist), GTK_WIDGET (window));
  gtk_window_set_type_hint (GTK_WINDOW (assist), GDK_WINDOW_TYPE_HINT_DIALOG);
  /* Make asssitant modal preventing using application without profile */
  gtk_window_set_modal (GTK_WINDOW (assist), TRUE);

  gtk_window_present (GTK_WINDOW (assist));
}
