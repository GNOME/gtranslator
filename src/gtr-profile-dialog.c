/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 *               2008  Igalia 
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
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-dirs.h"
#include "gtr-profile-dialog.h"
#include "gtr-profile.h"
#include "gtr-utils.h"
#include "gtr-languages-fetcher.h"

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

typedef struct
{
  GtkWidget *main_box;

  GtkWidget *profile_name;

  GtkWidget *author_name;
  GtkWidget *author_email;
  GtkWidget *team_email;

  GtkWidget *languages_fetcher;
} GtrProfileDialogPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrProfileDialog, gtr_profile_dialog, GTK_TYPE_DIALOG)

static void
gtr_profile_dialog_class_init (GtrProfileDialogClass *klass)
{
}

static void
gtr_profile_dialog_init (GtrProfileDialog *dlg)
{
  GtrProfileDialogPrivate *priv = gtr_profile_dialog_get_instance_private (dlg);
  GtkBox *content_area;
  GtkWidget *fetcher_box;
  GtkBuilder *builder;
  gchar *root_objects[] = {
    "main_box",
    NULL
  };

  gtk_dialog_add_button (GTK_DIALOG (dlg),
                         _("_Cancel"), GTK_RESPONSE_CANCEL);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Translation Editor Profile"));
  gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);
  gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);

  content_area = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg)));

  /* HIG defaults */
  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (content_area, 2); /* 2 * 5 + 2 = 12 */

  builder = gtk_builder_new ();
  GError *error = NULL;
  gtk_builder_add_objects_from_resource (builder, "/org/gnome/translator/gtr-profile-dialog.ui",
                                         root_objects, &error);
  if (error)
  {
    g_message("%s", error->message);
  }
  priv->main_box = GTK_WIDGET (gtk_builder_get_object (builder, "main_box"));
  g_object_ref (priv->main_box);
  priv->profile_name = GTK_WIDGET (gtk_builder_get_object (builder, "profile_name"));
  priv->author_name = GTK_WIDGET (gtk_builder_get_object (builder, "name"));
  priv->author_email = GTK_WIDGET (gtk_builder_get_object (builder, "email"));
  priv->team_email = GTK_WIDGET (gtk_builder_get_object (builder, "team_email"));
  fetcher_box = GTK_WIDGET (gtk_builder_get_object (builder, "fetcher_box"));
  g_object_unref (builder);

  gtk_box_pack_start (content_area, priv->main_box, FALSE, FALSE, 0);

  priv->languages_fetcher = gtr_languages_fetcher_new ();
  gtk_widget_show (priv->languages_fetcher);
  gtk_box_pack_start (GTK_BOX (fetcher_box), priv->languages_fetcher,
                      TRUE, TRUE, 0);
}

static void
fill_entries (GtrProfileDialog *dlg, GtrProfile *profile)
{
  GtrProfileDialogPrivate *priv = gtr_profile_dialog_get_instance_private (dlg);
  if (gtr_profile_get_name (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (priv->profile_name),
                        gtr_profile_get_name (profile));

  if (gtr_profile_get_author_name (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (priv->author_name),
                        gtr_profile_get_author_name (profile));

  if (gtr_profile_get_author_email (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (priv->author_email),
                        gtr_profile_get_author_email (profile));

  if (gtr_profile_get_group_email (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (priv->team_email),
                        gtr_profile_get_group_email (profile));

  if (gtr_profile_get_language_name (profile) != NULL)
    gtr_languages_fetcher_set_language_name (GTR_LANGUAGES_FETCHER (priv->languages_fetcher),
                                             gtr_profile_get_language_name (profile));

  if (gtr_profile_get_language_code (profile) != NULL)
    gtr_languages_fetcher_set_language_code (GTR_LANGUAGES_FETCHER (priv->languages_fetcher),
                                             gtr_profile_get_language_code (profile));

  if (gtr_profile_get_charset (profile) != NULL)
    gtr_languages_fetcher_set_charset (GTR_LANGUAGES_FETCHER (priv->languages_fetcher),
                                       gtr_profile_get_charset (profile));

  if (gtr_profile_get_encoding (profile) != NULL)
    gtr_languages_fetcher_set_encoding (GTR_LANGUAGES_FETCHER (priv->languages_fetcher),
                                        gtr_profile_get_encoding (profile));

  if (gtr_profile_get_plural_forms (profile) != NULL)
    gtr_languages_fetcher_set_plural_form (GTR_LANGUAGES_FETCHER (priv->languages_fetcher),
                                           gtr_profile_get_plural_forms (profile));
}

GtrProfileDialog *
gtr_profile_dialog_new (GtkWidget  *parent,
                        GtrProfile *profile)
{
  GtrProfileDialog *dlg;

  dlg = g_object_new (GTR_TYPE_PROFILE_DIALOG, NULL);

  if (profile != NULL)
    {
      fill_entries (dlg, profile);

      /* We distinguish in the preferences dialog if we are modifying
         or adding a new profile depending on the response */
      gtk_dialog_add_button (GTK_DIALOG (dlg),
                             _("_OK"), GTK_RESPONSE_YES);
    }
  else
    {
      gtk_dialog_add_button (GTK_DIALOG (dlg),
                             _("_OK"), GTK_RESPONSE_ACCEPT);
    }

  if (GTK_WINDOW (parent) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (parent));
    }

  return dlg;
}

GtrProfile *
gtr_profile_dialog_get_profile (GtrProfileDialog *dlg)
{
  GtrProfile *profile;
  GtrProfileDialogPrivate *priv = gtr_profile_dialog_get_instance_private (dlg);

  g_return_val_if_fail (GTR_IS_PROFILE_DIALOG (dlg), NULL);

  profile = gtr_profile_new ();

  gtr_profile_set_name (profile,
                        gtk_entry_get_text (GTK_ENTRY (priv->profile_name)));

  gtr_profile_set_author_name (profile,
                               gtk_entry_get_text (GTK_ENTRY (priv->author_name)));

  gtr_profile_set_author_email (profile,
                                gtk_entry_get_text (GTK_ENTRY (priv->author_email)));

  gtr_profile_set_group_email (profile,
                               gtk_entry_get_text (GTK_ENTRY (priv->team_email)));

  gtr_profile_set_language_name (profile,
                                 gtr_languages_fetcher_get_language_name (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  gtr_profile_set_language_code (profile,
                                 gtr_languages_fetcher_get_language_code (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  gtr_profile_set_charset (profile,
                           gtr_languages_fetcher_get_charset (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  gtr_profile_set_encoding (profile,
                            gtr_languages_fetcher_get_encoding (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  gtr_profile_set_plural_forms (profile,
                                gtr_languages_fetcher_get_plural_form (GTR_LANGUAGES_FETCHER (priv->languages_fetcher)));

  return profile;
}
