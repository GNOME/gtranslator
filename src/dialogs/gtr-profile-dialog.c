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
#include "gtr-language.h"

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

  GtkWidget *language;
  GtkWidget *language_code;
  GtkWidget *charset;
  GtkWidget *encoding;
  GtkWidget *team_email;
  GtkWidget *plural_forms;

  GtkListStore *language_store;
  GtkListStore *code_store;
};

static void
gtr_profile_dialog_class_init (GtrProfileDialogClass *klass)
{
  g_type_class_add_private (klass, sizeof (GtrProfileDialogPrivate));
}

static void
append_from_languages (GtrProfileDialog *dlg)
{
  const GSList *languages, *l;
  GHashTable *plurals;

  plurals = g_hash_table_new (g_str_hash, g_int_equal);

  languages = gtr_language_get_languages ();

  for (l = languages; l != NULL; l = (const GSList *)g_list_next (l))
    {
      GtrLanguage *lang = (GtrLanguage *)l->data;
      GtkTreeIter iter1, iter2;
      const gchar *plural_form;

      gtk_list_store_append (dlg->priv->language_store, &iter1);
      gtk_list_store_set (dlg->priv->language_store, &iter1,
                          0, gtr_language_get_name (lang),
                          1, lang,
                          -1);

      gtk_list_store_append (dlg->priv->code_store, &iter2);
      gtk_list_store_set (dlg->priv->code_store, &iter2,
                          0, gtr_language_get_code (lang),
                          1, lang,
                          -1);

      plural_form = gtr_language_get_plural_form (lang);
      if (plural_form != NULL && *plural_form != '\0')
        {
          gint *value;

          value = g_hash_table_lookup (plurals, plural_form);
          if (value == NULL)
            {
              g_hash_table_insert (plurals, (gchar *)plural_form, GINT_TO_POINTER (1));
              gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (dlg->priv->plural_forms),
                                              plural_form);
            }
        }
    }

    g_hash_table_unref (plurals);
}

static void
fill_encoding_and_charset (GtrProfileDialog *dlg)
{
  const gchar *text;

  text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->charset))));

  if (text == NULL || *text == '\0')
    gtk_combo_box_set_active (GTK_COMBO_BOX (dlg->priv->charset), 0);

  text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->encoding))));

  if (text == NULL || *text == '\0')
    gtk_combo_box_set_active (GTK_COMBO_BOX (dlg->priv->encoding), 0);
}

static void
fill_from_language_entry (GtrProfileDialog *dlg,
                          GtrLanguage      *lang)
{
  const gchar *entry_text;

  fill_encoding_and_charset (dlg);

  entry_text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->language_code))));

  if (*entry_text == '\0')
    {
      const gchar *code;

      code = gtr_language_get_code (lang);

      if (code != NULL && *code != '\0')
        gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->language_code))), code);
    }

  entry_text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->plural_forms))));

  if (*entry_text == '\0')
    {
      const gchar *plural_form;

      plural_form = gtr_language_get_plural_form (lang);

      if (plural_form != NULL && *plural_form != '\0')
        gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->plural_forms))), plural_form);
    }
}

static void
fill_from_language_code_entry (GtrProfileDialog *dlg,
                               GtrLanguage      *lang)
{
  const gchar *entry_text;

  fill_encoding_and_charset (dlg);

  entry_text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->language))));

  if (*entry_text == '\0')
    {
      const gchar *name;

      name = gtr_language_get_name (lang);

      if (name != NULL && *name != '\0')
        gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->language))), name);
    }

  entry_text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->plural_forms))));

  if (*entry_text == '\0')
    {
      const gchar *plural_form;

      plural_form = gtr_language_get_plural_form (lang);

      if (plural_form != NULL && *plural_form != '\0')
        gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->plural_forms))), plural_form);
    }
}

typedef void (* fill_method) (GtrProfileDialog *dlg, GtrLanguage *lang);

static void
fill_boxes (GtrProfileDialog *dlg,
            GtkEntry         *entry,
            GtkTreeModel     *store,
            fill_method       fill)
{
  const gchar *text;
  gchar *entry_row;
  GtkTreeIter iter;
  GtrLanguage *lang;
  gboolean found = FALSE;

  text = gtk_entry_get_text (entry);

  if (text == NULL || *text == '\0' ||
      !gtk_tree_model_get_iter_first (store, &iter))
    return;

  do
    {
      gtk_tree_model_get (store, &iter,
                          0, &entry_row,
                          -1);

      if (entry_row != NULL && strcmp (entry_row, text) == 0)
        {
          found = TRUE;
          g_free (entry_row);
          break;
        }

      g_free (entry_row);
    }
  while (gtk_tree_model_iter_next (store, &iter));

  if (found)
    {
      gtk_tree_model_get (store, &iter,
                          1, &lang,
                          -1);

      fill (dlg, lang);
    }
}

static void
on_language_activate (GtkEntry         *entry,
                      GtrProfileDialog *dlg)
{
  fill_boxes (dlg, entry, GTK_TREE_MODEL (dlg->priv->language_store),
              fill_from_language_entry);
}

static void
on_language_focus_out_event (GtkEntry         *entry,
                             GdkEvent         *event,
                             GtrProfileDialog *dlg)
{
  on_language_activate (entry, dlg);
}

static void
on_language_code_activate (GtkEntry         *entry,
                           GtrProfileDialog *dlg)
{
  fill_boxes (dlg, entry, GTK_TREE_MODEL (dlg->priv->code_store),
              fill_from_language_code_entry);
}

static void
on_language_code_focus_out_event (GtkEntry         *entry,
                                  GdkEvent         *event,
                                  GtrProfileDialog *dlg)
{
  on_language_code_activate (entry, dlg);
}

static void
gtr_profile_dialog_init (GtrProfileDialog *dlg)
{
  gboolean ret;
  GtkWidget *error_widget, *action_area;
  GtkBox *content_area;
  gchar *path;
  gchar *root_objects[] = {
    "main_box",
    "language_store",
    "code_store",
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
                                  "language", &dlg->priv->language,
                                  "language_code", &dlg->priv->language_code,
                                  "charset", &dlg->priv->charset,
                                  "encoding", &dlg->priv->encoding,
                                  "team_email", &dlg->priv->team_email,
                                  "plural_forms", &dlg->priv->plural_forms,

                                  "language_store", &dlg->priv->language_store,
                                  "code_store", &dlg->priv->code_store,
                                  NULL);
  g_free (path);

  if (!ret)
    {
      gtk_widget_show (error_widget);
      gtk_box_pack_start (content_area, error_widget, TRUE, TRUE, 0);

      return;
    }

  gtk_box_pack_start (content_area, dlg->priv->main_box, FALSE, FALSE, 0);

  /* add items to comboboxes */
  append_from_languages (dlg);

  g_signal_connect (gtk_bin_get_child (GTK_BIN (dlg->priv->language)),
                    "activate",
                    G_CALLBACK (on_language_activate),
                    dlg);
  g_signal_connect (gtk_bin_get_child (GTK_BIN (dlg->priv->language)),
                    "focus-out-event",
                    G_CALLBACK (on_language_focus_out_event),
                    dlg);
  g_signal_connect (gtk_bin_get_child (GTK_BIN (dlg->priv->language_code)),
                    "activate",
                    G_CALLBACK (on_language_code_activate),
                    dlg);
  g_signal_connect (gtk_bin_get_child (GTK_BIN (dlg->priv->language_code)),
                    "focus-out-event",
                    G_CALLBACK (on_language_code_focus_out_event),
                    dlg);
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
    gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->language))),
                        gtr_profile_get_language_name (profile));

  if (gtr_profile_get_language_code (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->language_code))),
                        gtr_profile_get_language_code (profile));

  if (gtr_profile_get_charset (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->charset))),
                        gtr_profile_get_charset (profile));

  if (gtr_profile_get_encoding (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->encoding))),
                        gtr_profile_get_encoding (profile));

  if (gtr_profile_get_group_email (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (dlg->priv->team_email),
                        gtr_profile_get_group_email (profile));

  if (gtr_profile_get_plural_forms (profile) != NULL)
    gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->plural_forms))),
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
  gchar *text;

  g_return_val_if_fail (GTR_IS_PROFILE_DIALOG (dlg), NULL);

  profile = gtr_profile_new ();

  gtr_profile_set_name (profile,
                        gtk_entry_get_text (GTK_ENTRY (dlg->priv->profile_name)));

  gtr_profile_set_author_name (profile,
                               gtk_entry_get_text (GTK_ENTRY (dlg->priv->author_name)));

  gtr_profile_set_author_email (profile,
                                gtk_entry_get_text (GTK_ENTRY (dlg->priv->author_email)));

  gtr_profile_set_language_name (profile,
                                 gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->language)))));

  gtr_profile_set_language_code (profile,
                                 gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (dlg->priv->language_code)))));

  text = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (dlg->priv->charset));
  gtr_profile_set_charset (profile, text);
  g_free (text);

  text = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (dlg->priv->encoding));
  gtr_profile_set_encoding (profile, text);
  g_free (text);

  gtr_profile_set_group_email (profile,
                               gtk_entry_get_text (GTK_ENTRY (dlg->priv->team_email)));

  text = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (dlg->priv->plural_forms));
  gtr_profile_set_plural_forms (profile, text);
  g_free (text);

  return profile;
}
