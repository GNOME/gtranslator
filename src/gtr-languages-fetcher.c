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
#include "gtr-lang-combo-row.h"
#include <string.h>

typedef struct
{
  GtkWidget *language;

  GtkWidget *language_code;
  GtkWidget *charset;
  GtkWidget *encoding;
  GtkWidget *plural_forms;

  GtkWidget *language_code_dropdown;
  GtkWidget *charset_dropdown;
  GtkWidget *encoding_dropdown;
  GtkWidget *plural_forms_dropdown;

  GtkStringList *code_store;
  GtkStringList *charset_store;
  GtkStringList *encoding_store;
  GtkStringList *plural_forms_store;

} GtrLanguagesFetcherPrivate;

struct _GtrLanguagesFetcher
{
  AdwPreferencesGroup parent;
};

/* Signals */
enum
{
  CHANGED,
  LAST_SIGNAL
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrLanguagesFetcher, gtr_languages_fetcher, ADW_TYPE_PREFERENCES_GROUP)

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
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_languages_fetcher_finalize;

  signals[CHANGED] =
    g_signal_newv ("changed",
                   G_OBJECT_CLASS_TYPE (object_class),
                   G_SIGNAL_RUN_LAST,
                   NULL, NULL, NULL, NULL,
                   G_TYPE_NONE, 0, NULL);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-languages-fetcher.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, language);

  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, language_code);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, language_code_dropdown);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, code_store);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, charset);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, charset_dropdown);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, charset_store);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, encoding);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, encoding_dropdown);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, encoding_store);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, plural_forms);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, plural_forms_dropdown);
  gtk_widget_class_bind_template_child_private (widget_class, GtrLanguagesFetcher, plural_forms_store);
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
  g_autoptr (GHashTable) plurals = NULL;
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);

  plurals = g_hash_table_new (g_str_hash, g_int_equal);

  languages = gtr_language_get_languages ();
  languages = g_slist_sort ((GSList *)languages, compare_languages_code);

  for (l = languages; l != NULL; l = g_slist_next (l))
    {
      GtrLanguage *lang = (GtrLanguage *)l->data;
      const gchar *plural_form;

      gtk_string_list_append (GTK_STRING_LIST (priv->code_store),
                              gtr_language_get_code (lang));

      plural_form = gtr_language_get_plural_form (lang);
      if (plural_form != NULL && *plural_form != '\0')
        {
          gint *value;

          value = g_hash_table_lookup (plurals, plural_form);
          if (value == NULL)
            {
              g_hash_table_insert (plurals, (gchar *)plural_form, GINT_TO_POINTER (1));
              gtk_string_list_append (GTK_STRING_LIST (priv->plural_forms_store),
                                      plural_form);
            }
        }
    }
}

static void
fill_encoding_and_charset (GtrLanguagesFetcher *fetcher)
{
  const gchar *text;
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);

  text = gtk_editable_get_text (GTK_EDITABLE (priv->charset));
  if (text == NULL || *text == '\0')
    gtk_drop_down_set_selected (GTK_DROP_DOWN (priv->charset_dropdown), 0);

  text = gtk_editable_get_text (GTK_EDITABLE (priv->encoding));
  if (text == NULL || *text == '\0')
    gtk_drop_down_set_selected (GTK_DROP_DOWN (priv->encoding_dropdown), 0);
}

static void
fill_from_language_entry (GtrLanguagesFetcher *fetcher,
                          GtrLanguage         *lang)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  const gchar *code;
  const gchar *plural_form;

  fill_encoding_and_charset (fetcher);

  code = gtr_language_get_code (lang);
  if (code != NULL && *code != '\0')
    gtk_editable_set_text (GTK_EDITABLE (priv->language_code), code);

  plural_form = gtr_language_get_plural_form (lang);
  if (plural_form != NULL && *plural_form != '\0')
    gtk_editable_set_text (GTK_EDITABLE (priv->plural_forms), plural_form);
}

static void
update_language (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  const gchar *text = gtr_lang_combo_row_get_lang (GTR_LANG_COMBO_ROW (priv->language));
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
on_combo_box_changed (GtkWidget           *widget,
                      GtrLanguagesFetcher *fetcher)
{
  g_signal_emit (fetcher, signals[CHANGED], 0, NULL);
}

static void
on_charset_changed (GtkWidget *widget, GParamSpec *spec, GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  int selected = gtk_drop_down_get_selected (GTK_DROP_DOWN (priv->charset_dropdown));
  const char *charset = gtk_string_list_get_string (priv->charset_store, selected);
  gtr_languages_fetcher_set_charset (fetcher, charset);
}

static void
on_encoding_changed (GtkWidget *widget, GParamSpec *spec, GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  int selected = gtk_drop_down_get_selected (GTK_DROP_DOWN (priv->encoding_dropdown));
  const char *encoding = gtk_string_list_get_string (priv->encoding_store, selected);
  gtr_languages_fetcher_set_encoding (fetcher, encoding);
}

static void
on_plural_forms_changed (GtkWidget *widget, GParamSpec *spec, GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  int selected = gtk_drop_down_get_selected (GTK_DROP_DOWN (priv->plural_forms_dropdown));
  const char *plural_forms = gtk_string_list_get_string (priv->plural_forms_store, selected);
  gtr_languages_fetcher_set_plural_form (fetcher, plural_forms);
}

static void
on_language_code_changed (GtkWidget *widget, GParamSpec *spec, GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  int selected = gtk_drop_down_get_selected (GTK_DROP_DOWN (priv->language_code_dropdown));
  const char *lang = gtk_string_list_get_string (priv->code_store, selected);
  gtr_languages_fetcher_set_language_code (fetcher, lang);
}

static void
on_lang_changed (GtkWidget           *widget,
                 GParamSpec          *spec,
                 GtrLanguagesFetcher *fetcher)
{
  update_language (fetcher);
  g_signal_emit (fetcher, signals[CHANGED], 0, NULL);
}

static void
gtr_languages_fetcher_init (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);

  g_type_ensure (GTR_TYPE_LANG_COMBO_ROW);

  gtk_widget_init_template (GTK_WIDGET (fetcher));

  // add items to comboboxes
  append_from_languages (fetcher);

  // Populate other settings based on default language.
  update_language (fetcher);

  g_signal_connect (priv->language, "notify::selected",
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

  g_signal_connect (priv->charset_dropdown,
                    "notify::selected",
                    G_CALLBACK (on_charset_changed),
                    fetcher);
  g_signal_connect (priv->encoding_dropdown,
                    "notify::selected",
                    G_CALLBACK (on_encoding_changed),
                    fetcher);
  g_signal_connect (priv->plural_forms_dropdown,
                    "notify::selected",
                    G_CALLBACK (on_plural_forms_changed),
                    fetcher);
  g_signal_connect (priv->language_code_dropdown,
                    "notify::selected",
                    G_CALLBACK (on_language_code_changed),
                    fetcher);

  // Initial values
  on_charset_changed (NULL, NULL, fetcher);
  on_encoding_changed (NULL, NULL, fetcher);
}

GtkWidget *
gtr_languages_fetcher_new (void)
{
  return g_object_new (GTR_TYPE_LANGUAGES_FETCHER, NULL);
}

const gchar *
gtr_languages_fetcher_get_language_name (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  return gtr_lang_combo_row_get_lang (GTR_LANG_COMBO_ROW (priv->language));
}

void
gtr_languages_fetcher_set_language_name (GtrLanguagesFetcher *fetcher,
                                         const gchar         *name)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  gtr_lang_combo_row_set_lang (GTR_LANG_COMBO_ROW (priv->language), name);
}

const gchar *
gtr_languages_fetcher_get_language_code (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  return gtk_editable_get_text (GTK_EDITABLE (priv->language_code));
}

void
gtr_languages_fetcher_set_language_code (GtrLanguagesFetcher *fetcher,
                                         const gchar         *code)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  gtk_editable_set_text (GTK_EDITABLE (priv->language_code), code);
}

const gchar *
gtr_languages_fetcher_get_charset (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  return gtk_editable_get_text (GTK_EDITABLE (priv->charset));
}

void
gtr_languages_fetcher_set_charset (GtrLanguagesFetcher *fetcher,
                                   const gchar         *charset)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  gtk_editable_set_text (GTK_EDITABLE (priv->charset), charset);
}

const gchar *
gtr_languages_fetcher_get_encoding (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  return gtk_editable_get_text (GTK_EDITABLE (priv->encoding));
}

void
gtr_languages_fetcher_set_encoding (GtrLanguagesFetcher *fetcher,
                                    const gchar         *enc)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  gtk_editable_set_text (GTK_EDITABLE (priv->encoding), enc);
}

const gchar *
gtr_languages_fetcher_get_plural_form (GtrLanguagesFetcher *fetcher)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  return gtk_editable_get_text (GTK_EDITABLE (priv->plural_forms));
}

void
gtr_languages_fetcher_set_plural_form (GtrLanguagesFetcher *fetcher,
                                       const gchar         *plural_form)
{
  GtrLanguagesFetcherPrivate *priv = gtr_languages_fetcher_get_instance_private (fetcher);
  gtk_editable_set_text (GTK_EDITABLE (priv->plural_forms), plural_form);
}
