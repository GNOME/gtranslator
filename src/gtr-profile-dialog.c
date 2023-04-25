/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 *               2008  Igalia
 * Copyright (C) 2022 Daniel Garcia <dani@danigm.net>
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
 *   Daniel Garcia <dani@danigm.net>
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
  GtkWidget *profile_name;
  GtkWidget *auth_token;

  GtkWidget *author_name;
  GtkWidget *author_email;
  GtkWidget *team_email;

  GtkWidget *languages_fetcher;
  gboolean   editing;
} GtrProfileDialogPrivate;

struct _GtrProfileDialog
{
  GtkDialog parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrProfileDialog, gtr_profile_dialog, GTK_TYPE_DIALOG)

static void
gtr_profile_dialog_class_init (GtrProfileDialogClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-profile-dialog.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrProfileDialog, languages_fetcher);
  gtk_widget_class_bind_template_child_private (widget_class, GtrProfileDialog, profile_name);
  gtk_widget_class_bind_template_child_private (widget_class, GtrProfileDialog, auth_token);
  gtk_widget_class_bind_template_child_private (widget_class, GtrProfileDialog, author_name);
  gtk_widget_class_bind_template_child_private (widget_class, GtrProfileDialog, author_email);
  gtk_widget_class_bind_template_child_private (widget_class, GtrProfileDialog, team_email);
}

static void
gtr_profile_dialog_init (GtrProfileDialog *dlg)
{
  GtrProfileDialogPrivate *priv = gtr_profile_dialog_get_instance_private (dlg);

  priv->editing = FALSE;

  gtk_window_set_title (GTK_WINDOW (dlg), _("Translation Editor Profile"));
  gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);
  gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);

  gtk_widget_init_template (GTK_WIDGET (dlg));
}

static void
fill_entries (GtrProfileDialog *dlg, GtrProfile *profile)
{
  GtrProfileDialogPrivate *priv = gtr_profile_dialog_get_instance_private (dlg);

  priv->editing = TRUE;
  if (gtr_profile_get_name (profile) != NULL)
    gtk_editable_set_text (GTK_EDITABLE (priv->profile_name), gtr_profile_get_name (profile));

  if (gtr_profile_get_auth_token (profile) != NULL)
    gtk_editable_set_text (GTK_EDITABLE (priv->auth_token), gtr_profile_get_auth_token (profile));

  if (gtr_profile_get_author_name (profile) != NULL)
    gtk_editable_set_text (GTK_EDITABLE (priv->author_name), gtr_profile_get_author_name (profile));

  if (gtr_profile_get_author_email (profile) != NULL)
    gtk_editable_set_text (GTK_EDITABLE (priv->author_email), gtr_profile_get_author_email (profile));

  if (gtr_profile_get_group_email (profile) != NULL)
    gtk_editable_set_text (GTK_EDITABLE (priv->team_email), gtr_profile_get_group_email (profile));

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

  dlg = g_object_new (GTR_TYPE_PROFILE_DIALOG,"use-header-bar",TRUE, NULL);

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
                        gtk_editable_get_text (GTK_EDITABLE (priv->profile_name)));

  gtr_profile_set_auth_token (profile,
                        gtk_editable_get_text (GTK_EDITABLE (priv->auth_token)));

  gtr_profile_set_author_name (profile,
                               gtk_editable_get_text (GTK_EDITABLE (priv->author_name)));

  gtr_profile_set_author_email (profile,
                                gtk_editable_get_text (GTK_EDITABLE (priv->author_email)));

  gtr_profile_set_group_email (profile,
                               gtk_editable_get_text (GTK_EDITABLE (priv->team_email)));

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
