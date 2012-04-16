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

#define GTR_PROFILE_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
                                                 (object), \
                                                 GTR_TYPE_PROFILE_DIALOG, \
                                                 GtrProfileDialogPrivate))

G_DEFINE_TYPE (GtrProfileDialog, gtr_profile_dialog, GTK_TYPE_DIALOG)

struct _GtrProfileDialogPrivate
{
  GtkWidget *main_box;

  GtkWidget *profile_name;

  GtkWidget *author_name;
  GtkWidget *author_email;

  GtkWidget *languages_fetcher;
};

static void
gtr_profile_dialog_class_init (GtrProfileDialogClass *klass)
{
  g_type_class_add_private (klass, sizeof (GtrProfileDialogPrivate));
}

static void
gtr_profile_dialog_init (GtrProfileDialog *dlg)
{
  gboolean ret;
  GtkWidget *error_widget, *action_area;
  GtkBox *content_area;
  GtkWidget *fetcher_box;
  gchar *path;
  gchar *root_objects[] = {
    "main_box",
    NULL
  };

  dlg->priv = GTR_PROFILE_DIALOG_GET_PRIVATE (dlg);

  gtk_dialog_add_button (GTK_DIALOG (dlg),
                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Gtranslator Profile"));
  gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);
  gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);

  action_area = gtk_dialog_get_action_area (GTK_DIALOG (dlg));
  content_area = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg)));

  /* HIG defaults */
  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (content_area, 2); /* 2 * 5 + 2 = 12 */
  gtk_container_set_border_width (GTK_CONTAINER (action_area), 5);
  gtk_box_set_spacing (GTK_BOX (action_area), 4);

  path = gtr_dirs_get_ui_file ("gtr-profile-dialog.ui");
  ret = gtr_utils_get_ui_objects (path,
                                  root_objects,
                                  &error_widget,
                                  "main_box", &dlg->priv->main_box,
                                  "profile_name", &dlg->priv->profile_name,
                                  "name", &dlg->priv->author_name,
                                  "email", &dlg->priv->author_email,
                                  "fetcher_box", &fetcher_box,
                                  NULL);
  g_free (path);

  if (!ret)
    {
      gtk_widget_show (error_widget);
      gtk_box_pack_start (content_area, error_widget, TRUE, TRUE, 0);

      return;
    }

  gtk_box_pack_start (content_area, dlg->priv->main_box, FALSE, FALSE, 0);

  dlg->priv->languages_fetcher = gtr_languages_fetcher_new ();
  gtk_widget_show (dlg->priv->languages_fetcher);
  gtk_box_pack_start (GTK_BOX (fetcher_box), dlg->priv->languages_fetcher,
                      TRUE, TRUE, 0);
}

static void
fill_entries (GtrProfileDialog *dlg, GtrProfile *profile)
{
  if (gtr_profile_get_name (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (dlg->priv->profile_name),
                        gtr_profile_get_name (profile));

  if (gtr_profile_get_author_name (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (dlg->priv->author_name),
                        gtr_profile_get_author_name (profile));

  if (gtr_profile_get_author_email (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (dlg->priv->author_email),
                        gtr_profile_get_author_email (profile));

  if (gtr_profile_get_language_name (profile) != NULL)
    gtr_languages_fetcher_set_language_name (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher),
                                             gtr_profile_get_language_name (profile));

  if (gtr_profile_get_language_code (profile) != NULL)
    gtr_languages_fetcher_set_language_code (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher),
                                             gtr_profile_get_language_code (profile));

  if (gtr_profile_get_charset (profile) != NULL)
    gtr_languages_fetcher_set_charset (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher),
                                       gtr_profile_get_charset (profile));

  if (gtr_profile_get_encoding (profile) != NULL)
    gtr_languages_fetcher_set_encoding (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher),
                                        gtr_profile_get_encoding (profile));

  if (gtr_profile_get_group_email (profile) != NULL)
    gtr_languages_fetcher_set_team_email (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher),
                                          gtr_profile_get_group_email (profile));

  if (gtr_profile_get_plural_forms (profile) != NULL)
    gtr_languages_fetcher_set_plural_form (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher),
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
                             GTK_STOCK_OK, GTK_RESPONSE_YES);
    }
  else
    {
      gtk_dialog_add_button (GTK_DIALOG (dlg),
                             GTK_STOCK_OK, GTK_RESPONSE_ACCEPT);
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

  g_return_val_if_fail (GTR_IS_PROFILE_DIALOG (dlg), NULL);

  profile = gtr_profile_new ();

  gtr_profile_set_name (profile,
                        gtk_entry_get_text (GTK_ENTRY (dlg->priv->profile_name)));

  gtr_profile_set_author_name (profile,
                               gtk_entry_get_text (GTK_ENTRY (dlg->priv->author_name)));

  gtr_profile_set_author_email (profile,
                                gtk_entry_get_text (GTK_ENTRY (dlg->priv->author_email)));

  gtr_profile_set_language_name (profile,
                                 gtr_languages_fetcher_get_language_name (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher)));

  gtr_profile_set_language_code (profile,
                                 gtr_languages_fetcher_get_language_code (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher)));

  gtr_profile_set_charset (profile,
                           gtr_languages_fetcher_get_charset (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher)));

  gtr_profile_set_encoding (profile,
                            gtr_languages_fetcher_get_encoding (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher)));

  gtr_profile_set_group_email (profile,
                               gtr_languages_fetcher_get_team_email (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher)));

  gtr_profile_set_plural_forms (profile,
                                gtr_languages_fetcher_get_plural_form (GTR_LANGUAGES_FETCHER (dlg->priv->languages_fetcher)));

  return profile;
}
