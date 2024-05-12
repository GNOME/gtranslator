/*
 * gtr-language.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2003, 2004 - Christian Persch
 * Copyright (C) 2006 - Paolo Maggi
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n.h>

#include "gtr-language.h"
#include "gtr-dirs.h"

#define GTR_PLURAL_FORMS_FILENAME	"gtr-plural-forms.ini"
#define GTR_LANGS_FILENAME	"gtr-languages.ini"

struct _GtrLanguage
{
  gchar *code;
  gchar *name;
  gchar *plural_form;
};

static gboolean initialized = FALSE;
static GKeyFile *plurals_file = NULL;
static GSList *languages = NULL;

G_DEFINE_BOXED_TYPE (GtrLanguage, gtr_language,
                     gtr_language_copy,
                     gtr_language_free)

/* These are to make language bindings happy. Since Languages are
 * const, copy() just returns the same pointer and frees() doesn't
 * do nothing */

GtrLanguage *
gtr_language_copy (const GtrLanguage *lang)
{
  g_return_val_if_fail (lang != NULL, NULL);

  return (GtrLanguage *) lang;
}

void
gtr_language_free (GtrLanguage *lang)
{
  g_return_if_fail (lang != NULL);
}

static void
load_plural_form (GtrLanguage *lang)
{
  gchar *plural_form = NULL;
  gchar **parts = NULL;

  plural_form = g_key_file_get_string (plurals_file, "Plural Forms", lang->code, NULL);

  if (plural_form != NULL && *plural_form != '\0')
    {
      lang->plural_form = plural_form;
      return;
    }

  // Trying with the base language
  parts = g_strsplit (lang->code, "_", 2);

  plural_form = g_key_file_get_string (plurals_file, "Plural Forms", parts[0], NULL);
  if (plural_form != NULL && *plural_form != '\0')
    lang->plural_form = plural_form;
  else
    lang->plural_form = NULL;

  g_strfreev (parts);
}

typedef enum
{
  STATE_START,
  STATE_STOP,
  STATE_ENTRIES
} ParserState;

static void
gtr_language_lazy_init (void)
{
  gchar *filename;
  GKeyFile *lang_file;
  gchar **langs;
  gchar *lang;
  gsize n, i;

  if (initialized)
    return;

  plurals_file = g_key_file_new ();
  filename = gtr_dirs_get_ui_file (GTR_PLURAL_FORMS_FILENAME);
  if (!g_key_file_load_from_file (plurals_file, filename, G_KEY_FILE_NONE, NULL))
    {
      g_warning ("Bad plural form file: '%s'", filename);
      g_free (filename);
      return;
    }

  g_free (filename);

  lang_file = g_key_file_new ();
  filename = gtr_dirs_get_ui_file (GTR_LANGS_FILENAME);
  if (!g_key_file_load_from_file (lang_file, filename, G_KEY_FILE_NONE, NULL))
    {
      g_warning ("Bad languages file: '%s'", filename);
      g_free (filename);
      return;
    }

  g_free (filename);

  langs = g_key_file_get_keys (lang_file, "Languages", &n, NULL);
  for (i=0; i<n; i++)
    {
      GtrLanguage *gtr_lang = g_slice_new (GtrLanguage);
      lang = langs[i];
      gtr_lang->code = g_strdup (lang);
      gtr_lang->name = g_key_file_get_string (lang_file, "Languages", lang, NULL);
      load_plural_form (gtr_lang);
      languages = g_slist_prepend (languages, gtr_lang);
    }
  g_strfreev (langs);

  languages = g_slist_reverse (languages);

  /* free the file, not needed anymore */
  g_key_file_free (plurals_file);
  g_key_file_free (lang_file);

  initialized = TRUE;
}

const GSList *
gtr_language_get_languages (void)
{
  gtr_language_lazy_init ();

  return languages;
}

const gchar *
gtr_language_get_code (const GtrLanguage *lang)
{
  g_return_val_if_fail (lang != NULL, NULL);

  gtr_language_lazy_init ();

  return lang->code;
}

const gchar *
gtr_language_get_name (const GtrLanguage *lang)
{
  g_return_val_if_fail (lang != NULL, NULL);

  gtr_language_lazy_init ();

  return lang->name;
}

const gchar *
gtr_language_get_plural_form (const GtrLanguage *lang)
{
  g_return_val_if_fail (lang != NULL, NULL);

  gtr_language_lazy_init ();

  return lang->plural_form;
}
