/*
 * gtr-languages-fetcher.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2011 - Ignacio Casal Quinteiro
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
 */

#include "gtr-dirs.h"
#include "gtr-languages-fetcher.h"
#include "gtr-language.h"
#include "gtr-utils.h"
#include <string.h>

#define GTR_LANGUAGES_FETCHER_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GTR_TYPE_LANGUAGES_FETCHER, GtrLanguagesFetcherPrivate))

struct _GtrLanguagesFetcherPrivate
{
  GtkWidget *language;
  GtkWidget *language_code;
  GtkWidget *charset;
  GtkWidget *encoding;
  GtkWidget *team_email;
  GtkWidget *plural_forms;

  GtkListStore *language_store;
  GtkListStore *code_store;
};

/* Signals */
enum
{
  CHANGED,
  LAST_SIGNAL
};

G_DEFINE_TYPE (GtrLanguagesFetcher, gtr_languages_fetcher, GTK_TYPE_VBOX)

static guint signals[LAST_SIGNAL] = { 0 };

static void
gtr_languages_fetcher_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtr_languages_fetcher_parent_class)->finalize (object);
}

static void
gtr_languages_fetcher_class_init (GtrLanguagesFetcherClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtr_languages_fetcher_finalize;

  signals[CHANGED] =
    g_signal_new ("changed",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrLanguagesFetcherClass, changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);

  g_type_class_add_private (object_class, sizeof (GtrLanguagesFetcherPrivate));
}

static void
append_from_languages (GtrLanguagesFetcher *fetcher)
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

      gtk_list_store_append (fetcher->priv->language_store, &iter1);
      gtk_list_store_set (fetcher->priv->language_store, &iter1,
                          0, gtr_language_get_name (lang),
                          1, lang,
                          -1);

      gtk_list_store_append (fetcher->priv->code_store, &iter2);
      gtk_list_store_set (fetcher->priv->code_store, &iter2,
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
              gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (fetcher->priv->plural_forms),
                                              plural_form);
            }
        }
    }

    g_hash_table_unref (plurals);
}

static void
fill_encoding_and_charset (GtrLanguagesFetcher *fetcher)
{
  const gchar *text;

  text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->charset))));

  if (text == NULL || *text == '\0')
    gtk_combo_box_set_active (GTK_COMBO_BOX (fetcher->priv->charset), 0);

  text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->encoding))));

  if (text == NULL || *text == '\0')
    gtk_combo_box_set_active (GTK_COMBO_BOX (fetcher->priv->encoding), 0);
}

static void
fill_from_language_entry (GtrLanguagesFetcher *fetcher,
                          GtrLanguage         *lang)
{
  const gchar *entry_text;

  fill_encoding_and_charset (fetcher);

  entry_text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->language_code))));

  if (*entry_text == '\0')
    {
      const gchar *code;

      code = gtr_language_get_code (lang);

      if (code != NULL && *code != '\0')
        gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->language_code))), code);
    }

  entry_text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->plural_forms))));

  if (*entry_text == '\0')
    {
      const gchar *plural_form;

      plural_form = gtr_language_get_plural_form (lang);

      if (plural_form != NULL && *plural_form != '\0')
        gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->plural_forms))), plural_form);
    }
}

static void
fill_from_language_code_entry (GtrLanguagesFetcher *fetcher,
                               GtrLanguage         *lang)
{
  const gchar *entry_text;

  fill_encoding_and_charset (fetcher);

  entry_text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->language))));

  if (*entry_text == '\0')
    {
      const gchar *name;

      name = gtr_language_get_name (lang);

      if (name != NULL && *name != '\0')
        gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->language))), name);
    }

  entry_text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->plural_forms))));

  if (*entry_text == '\0')
    {
      const gchar *plural_form;

      plural_form = gtr_language_get_plural_form (lang);

      if (plural_form != NULL && *plural_form != '\0')
        gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->plural_forms))), plural_form);
    }
}

typedef void (* fill_method) (GtrLanguagesFetcher *fetcher, GtrLanguage *lang);

static void
fill_boxes (GtrLanguagesFetcher *fetcher,
            GtkEntry            *entry,
            GtkTreeModel        *store,
            fill_method          fill)
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

      fill (fetcher, lang);
    }
}

static void
on_language_activate (GtkEntry         *entry,
                      GtrLanguagesFetcher *fetcher)
{
  fill_boxes (fetcher, entry, GTK_TREE_MODEL (fetcher->priv->language_store),
              fill_from_language_entry);
}

static void
on_language_focus_out_event (GtkEntry         *entry,
                             GdkEvent         *event,
                             GtrLanguagesFetcher *fetcher)
{
  on_language_activate (entry, fetcher);
}

static void
on_language_code_activate (GtkEntry         *entry,
                           GtrLanguagesFetcher *fetcher)
{
  fill_boxes (fetcher, entry, GTK_TREE_MODEL (fetcher->priv->code_store),
              fill_from_language_code_entry);
}

static void
on_language_code_focus_out_event (GtkEntry         *entry,
                                  GdkEvent         *event,
                                  GtrLanguagesFetcher *fetcher)
{
  on_language_code_activate (entry, fetcher);
}

static void
on_combo_box_changed (GtkWidget           *widget,
                      GtrLanguagesFetcher *fetcher)
{
  g_signal_emit (fetcher, signals[CHANGED], 0, NULL);
}

static void
gtr_languages_fetcher_init (GtrLanguagesFetcher *fetcher)
{
  GtkWidget *error_widget;
  GtkWidget *main_box;
  gboolean ret;
  gchar *path;
  gchar *root_objects[] = {
    "main_box",
    "language_store",
    "code_store",
    NULL
  };

  fetcher->priv = GTR_LANGUAGES_FETCHER_GET_PRIVATE (fetcher);

  path = gtr_dirs_get_ui_file ("gtr-languages-fetcher.ui");
  ret = gtr_utils_get_ui_objects (path,
                                  root_objects,
                                  &error_widget,
                                  "main_box", &main_box,
                                  "language", &fetcher->priv->language,
                                  "language_code", &fetcher->priv->language_code,
                                  "charset", &fetcher->priv->charset,
                                  "encoding", &fetcher->priv->encoding,
                                  "team_email", &fetcher->priv->team_email,
                                  "plural_forms", &fetcher->priv->plural_forms,

                                  "language_store", &fetcher->priv->language_store,
                                  "code_store", &fetcher->priv->code_store,
                                  NULL);
  g_free (path);

  if (!ret)
    {
      gtk_widget_show (error_widget);
      gtk_box_pack_start (GTK_BOX (fetcher), error_widget, TRUE, TRUE, 0);

      return;
    }

  gtk_box_pack_start (GTK_BOX (fetcher), main_box, FALSE, FALSE, 0);

  /* add items to comboboxes */
  append_from_languages (fetcher);

  g_signal_connect (gtk_bin_get_child (GTK_BIN (fetcher->priv->language)),
                    "activate",
                    G_CALLBACK (on_language_activate),
                    fetcher);
  g_signal_connect (gtk_bin_get_child (GTK_BIN (fetcher->priv->language)),
                    "focus-out-event",
                    G_CALLBACK (on_language_focus_out_event),
                    fetcher);
  g_signal_connect (gtk_bin_get_child (GTK_BIN (fetcher->priv->language_code)),
                    "activate",
                    G_CALLBACK (on_language_code_activate),
                    fetcher);
  g_signal_connect (gtk_bin_get_child (GTK_BIN (fetcher->priv->language_code)),
                    "focus-out-event",
                    G_CALLBACK (on_language_code_focus_out_event),
                    fetcher);

  /* To emit the changed signal */
  g_signal_connect (fetcher->priv->language, "changed",
                    G_CALLBACK (on_combo_box_changed),
                    fetcher);
  g_signal_connect (fetcher->priv->language_code, "changed",
                    G_CALLBACK (on_combo_box_changed),
                    fetcher);
  g_signal_connect (fetcher->priv->charset, "changed",
                    G_CALLBACK (on_combo_box_changed),
                    fetcher);
  g_signal_connect (fetcher->priv->encoding, "changed",
                    G_CALLBACK (on_combo_box_changed),
                    fetcher);
  g_signal_connect (fetcher->priv->team_email, "changed",
                    G_CALLBACK (on_combo_box_changed),
                    fetcher);
  g_signal_connect (fetcher->priv->plural_forms, "changed",
                    G_CALLBACK (on_combo_box_changed),
                    fetcher);
}

GtkWidget *
gtr_languages_fetcher_new ()
{
  return g_object_new (GTR_TYPE_LANGUAGES_FETCHER, NULL);
}

const gchar *
gtr_languages_fetcher_get_language_name (GtrLanguagesFetcher *fetcher)
{
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->language))));
}

void
gtr_languages_fetcher_set_language_name (GtrLanguagesFetcher *fetcher,
                                         const gchar         *name)
{
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (name != NULL);

  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->language))), name);
}

const gchar *
gtr_languages_fetcher_get_language_code (GtrLanguagesFetcher *fetcher)
{
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->language_code))));
}

void
gtr_languages_fetcher_set_language_code (GtrLanguagesFetcher *fetcher,
                                         const gchar         *code)
{
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (code != NULL);

  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->language_code))), code);
}

const gchar *
gtr_languages_fetcher_get_charset (GtrLanguagesFetcher *fetcher)
{
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->charset))));
}

void
gtr_languages_fetcher_set_charset (GtrLanguagesFetcher *fetcher,
                                   const gchar         *charset)
{
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (charset != NULL);

  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->charset))), charset);
}

const gchar *
gtr_languages_fetcher_get_encoding (GtrLanguagesFetcher *fetcher)
{
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->encoding))));
}

void
gtr_languages_fetcher_set_encoding (GtrLanguagesFetcher *fetcher,
                                    const gchar         *enc)
{
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (enc != NULL);

  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->encoding))), enc);
}

const gchar *
gtr_languages_fetcher_get_team_email (GtrLanguagesFetcher *fetcher)
{
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtk_entry_get_text (GTK_ENTRY (fetcher->priv->team_email));
}

void
gtr_languages_fetcher_set_team_email (GtrLanguagesFetcher *fetcher,
                                      const gchar         *email)
{
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (email != NULL);

  gtk_entry_set_text (GTK_ENTRY (fetcher->priv->team_email), email);
}

const gchar *
gtr_languages_fetcher_get_plural_form (GtrLanguagesFetcher *fetcher)
{
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->plural_forms))));
}

void
gtr_languages_fetcher_set_plural_form (GtrLanguagesFetcher *fetcher,
                                       const gchar         *plural_form)
{
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (plural_form != NULL);

  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (fetcher->priv->plural_forms))), plural_form);
}
