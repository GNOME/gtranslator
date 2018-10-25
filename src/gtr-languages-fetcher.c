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
#include "gtr-lang-button.h"
#include <string.h>

typedef struct
{
  GtkWidget *language;
  GtkWidget *language_code;
  GtkWidget *charset;
  GtkWidget *encoding;
  GtkWidget *team_email;
  GtkWidget *plural_forms;
  GtkWidget *advanced;

  GtkListStore *code_store;
} GtrLanguagesFetcherPrivate;

struct _GtrLanguagesFetcher
{
  GtkBox parent;
};

/* Signals */
enum
{
  CHANGED,
  LAST_SIGNAL
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrLanguagesFetcher, gtr_languages_fetcher, GTK_TYPE_BOX)

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
}

static gint
compare_languages_code (gconstpointer a,
                        gconstpointer b)
{
  GtrLanguage *lang1, *lang2;
  const gchar *name1, *name2;

  lang1 = (GtrLanguage *) a;
  lang2 = (GtrLanguage *) b;

  name1 = gtr_language_get_code (lang1);
  name2 = gtr_language_get_code (lang2);

  return g_utf8_collate (name1, name2);
}

static void
append_from_languages (GtrLanguagesFetcher *fetcher)
{
  const GSList *languages, *l;
  GHashTable *plurals;
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);

  plurals = g_hash_table_new (g_str_hash, g_int_equal);

  languages = gtr_language_get_languages ();
  languages = g_slist_sort ((GSList *)languages, compare_languages_code);

  for (l = languages; l != NULL; l = g_slist_next (l))
    {
      GtrLanguage *lang = (GtrLanguage *)l->data;
      GtkTreeIter iter2;
      const gchar *plural_form;

      gtk_list_store_append (priv->code_store, &iter2);
      gtk_list_store_set (priv->code_store, &iter2,
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
              gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (priv->plural_forms),
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
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);

  text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->charset))));

  if (text == NULL || *text == '\0' || !gtk_widget_is_sensitive (priv->charset))
    gtk_combo_box_set_active (GTK_COMBO_BOX (priv->charset), 0);

  text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->encoding))));

  if (text == NULL || *text == '\0' || !gtk_widget_is_sensitive (priv->encoding))
    gtk_combo_box_set_active (GTK_COMBO_BOX (priv->encoding), 0);
}

static void
fill_from_language_entry (GtrLanguagesFetcher *fetcher,
                          GtrLanguage         *lang)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);

  fill_encoding_and_charset (fetcher);

  const gchar *code;
  code = gtr_language_get_code (lang);
  if (code != NULL && *code != '\0')
    gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->language_code))), code);

  const gchar *plural_form;
  plural_form = gtr_language_get_plural_form (lang);
  if (plural_form != NULL && *plural_form != '\0')
    gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->plural_forms))), plural_form);
}

static void
fill_from_language_code_entry (GtrLanguagesFetcher *fetcher,
                               GtrLanguage         *lang)
{
  const gchar *entry_text;
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);

  fill_encoding_and_charset (fetcher);

  entry_text = gtr_lang_button_get_lang (GTR_LANG_BUTTON (priv->language));

  if (*entry_text == '\0')
    {
      const gchar *name;

      name = gtr_language_get_name (lang);

      if (name != NULL && *name != '\0')
        gtr_lang_button_set_lang (GTR_LANG_BUTTON (priv->language), name);
    }

  entry_text = gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->plural_forms))));

  if (*entry_text == '\0')
    {
      const gchar *plural_form;

      plural_form = gtr_language_get_plural_form (lang);

      if (plural_form != NULL && *plural_form != '\0')
        gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->plural_forms))), plural_form);
    }
}

typedef void (* fill_method) (GtrLanguagesFetcher *fetcher, GtrLanguage *lang);

static void
fill_boxes (GtrLanguagesFetcher *fetcher,
            const gchar         *text,
            GtkTreeModel        *store,
            fill_method          fill)
{
  gchar *entry_row;
  GtkTreeIter iter;
  GtrLanguage *lang;
  gboolean found = FALSE;

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
on_language_activate (GtrLangButton       *btn,
                      GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  const gchar *text = gtr_lang_button_get_lang (GTR_LANG_BUTTON (priv->language));
  GtrLanguage *lang;
  const GSList *l;
  const GSList *languages = gtr_language_get_languages ();

  for (l = languages; l != NULL; l = g_slist_next (l))
    {
      lang = (GtrLanguage*)l->data;
      const gchar *langname = gtr_language_get_name (lang);
      if (text != NULL && strcmp (langname, text) == 0)
        {
          fill_from_language_entry (fetcher, lang);
          break;
        }
    }
}

static void
on_language_code_activate (GtkEntry            *entry,
                           GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  const gchar *text = gtk_entry_get_text (entry);

  fill_boxes (fetcher, text, GTK_TREE_MODEL (priv->code_store),
              fill_from_language_code_entry);
}

static gboolean
on_language_code_focus_out_event (GtkEntry         *entry,
                                  GdkEvent         *event,
                                  GtrLanguagesFetcher *fetcher)
{
  on_language_code_activate (entry, fetcher);

  return FALSE;
}

static void
on_combo_box_changed (GtkWidget           *widget,
                      GtrLanguagesFetcher *fetcher)
{
  g_signal_emit (fetcher, signals[CHANGED], 0, NULL);
}

static void
on_advanced_changed (GtkToggleButton     *widget,
                     GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  gboolean active = gtk_toggle_button_get_active (widget);

  gtk_widget_set_sensitive (priv->language_code, active);
  gtk_widget_set_sensitive (priv->charset, active);
  gtk_widget_set_sensitive (priv->encoding, active);
  gtk_widget_set_sensitive (priv->plural_forms, active);
}

static void
on_lang_changed (GtkWidget           *widget,
                 GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  on_language_activate (GTR_LANG_BUTTON (priv->language), fetcher);
  g_signal_emit (fetcher, signals[CHANGED], 0, NULL);
}

static void
gtr_languages_fetcher_init (GtrLanguagesFetcher *fetcher)
{
  GtkWidget *content;
  GtkBuilder *builder;
  gchar *root_objects[] = {
    "main_box",
    "code_store",
    NULL
  };
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);

  gtk_orientable_set_orientation (GTK_ORIENTABLE (fetcher),
                                  GTK_ORIENTATION_VERTICAL);

  g_type_ensure (gtr_lang_button_get_type ());

  builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (builder, "/org/gnome/translator/gtr-languages-fetcher.ui",
                                         root_objects, NULL);

  content = GTK_WIDGET (gtk_builder_get_object (builder, "main_box"));
  g_object_ref (content);
  priv->language = GTK_WIDGET (gtk_builder_get_object (builder, "language"));
  priv->language_code = GTK_WIDGET (gtk_builder_get_object (builder, "language_code"));
  priv->charset = GTK_WIDGET (gtk_builder_get_object (builder, "charset"));
  priv->encoding = GTK_WIDGET (gtk_builder_get_object (builder, "encoding"));
  priv->plural_forms = GTK_WIDGET (gtk_builder_get_object (builder, "plural_forms"));
  priv->advanced = GTK_WIDGET (gtk_builder_get_object (builder, "advanced_check"));
  priv->code_store = GTK_LIST_STORE (gtk_builder_get_object (builder, "code_store"));

  g_object_unref (builder);

  gtk_box_pack_start (GTK_BOX (fetcher), content, FALSE, FALSE, 0);

  /* add items to comboboxes */
  append_from_languages (fetcher);

  g_signal_connect (G_OBJECT (priv->language),
                    "clicked",
                    G_CALLBACK (on_language_activate),
                    fetcher);
  g_signal_connect (gtk_bin_get_child (GTK_BIN (priv->language_code)),
                    "activate",
                    G_CALLBACK (on_language_code_activate),
                    fetcher);
  g_signal_connect (gtk_bin_get_child (GTK_BIN (priv->language_code)),
                    "focus-out-event",
                    G_CALLBACK (on_language_code_focus_out_event),
                    fetcher);

  /* To emit the changed signal */
  g_signal_connect (priv->language, "changed",
                    G_CALLBACK (on_lang_changed),
                    fetcher);
  g_signal_connect (priv->language_code, "changed",
                    G_CALLBACK (on_combo_box_changed),
                    fetcher);
  g_signal_connect (priv->charset, "changed",
                    G_CALLBACK (on_combo_box_changed),
                    fetcher);
  g_signal_connect (priv->encoding, "changed",
                    G_CALLBACK (on_combo_box_changed),
                    fetcher);
  g_signal_connect (priv->plural_forms, "changed",
                    G_CALLBACK (on_combo_box_changed),
                    fetcher);

  g_signal_connect (priv->advanced, "toggled",
                    G_CALLBACK (on_advanced_changed),
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
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtr_lang_button_get_lang (GTR_LANG_BUTTON (priv->language));
}

void
gtr_languages_fetcher_set_language_name (GtrLanguagesFetcher *fetcher,
                                         const gchar         *name)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (name != NULL);

  gtr_lang_button_set_lang (GTR_LANG_BUTTON (priv->language), name);
}

const gchar *
gtr_languages_fetcher_get_language_code (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->language_code))));
}

void
gtr_languages_fetcher_set_language_code (GtrLanguagesFetcher *fetcher,
                                         const gchar         *code)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (code != NULL);

  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->language_code))), code);
}

const gchar *
gtr_languages_fetcher_get_charset (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->charset))));
}

void
gtr_languages_fetcher_set_charset (GtrLanguagesFetcher *fetcher,
                                   const gchar         *charset)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (charset != NULL);

  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->charset))), charset);
}

const gchar *
gtr_languages_fetcher_get_encoding (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->encoding))));
}

void
gtr_languages_fetcher_set_encoding (GtrLanguagesFetcher *fetcher,
                                    const gchar         *enc)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (enc != NULL);

  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->encoding))), enc);
}

const gchar *
gtr_languages_fetcher_get_plural_form (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  g_return_val_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher), NULL);

  return gtk_entry_get_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->plural_forms))));
}

void
gtr_languages_fetcher_set_plural_form (GtrLanguagesFetcher *fetcher,
                                       const gchar         *plural_form)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  g_return_if_fail (GTR_IS_LANGUAGES_FETCHER (fetcher));
  g_return_if_fail (plural_form != NULL);

  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->plural_forms))), plural_form);
}
