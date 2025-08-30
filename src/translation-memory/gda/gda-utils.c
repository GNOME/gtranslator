/*
 * Copyright (C) 2001  Fatih Demir <kabalak@kabalak.net>
 *               2012  Ignacio Casal Quinteiro <icq@gnome.org>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gda-utils.h"

#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>


static const gchar *badwords[] = {
  "a",
  //"all",
  "an",
  //"are",
  //"can",
  //"for",
  //"from",
  "have",
  //"it",
  //"may",
  //"not",
  "of",
  //"that",
  "the",
  //"this",
  //"was",
  "will",
  //"with",
  //"you",
  //"your",
  NULL
};

static gboolean
check_good_word (const gchar * word, gchar ** bad_words)
{
  gboolean check = TRUE;
  g_autofree gchar *lower = NULL;
  gint i = 0;

  lower = g_utf8_strdown (word, -1);

  while (bad_words[i] != NULL)
    {
      gchar *lower_collate = g_utf8_collate_key (lower, -1);

      if (strcmp (lower_collate, bad_words[i]) == 0)
        {
          check = FALSE;
          g_free (lower_collate);
          break;
        }
      i++;
      g_free (lower_collate);
    }
  return check;
}


/**
 * gtr_gda_utils_split_string_in_words:
 * @string: the text to process
 *
 * Process a text and split it in words using pango.
 * 
 * Returns: an array of words of the processed text
 */
GStrv
gtr_gda_utils_split_string_in_words (const gchar * string)
{
  PangoLanguage *lang = pango_language_from_string ("en");
  PangoLogAttr *attrs;
  g_autoptr(GStrvBuilder) builder = g_strv_builder_new ();
  gint char_len;
  gint i = 0;
  gchar *s;
  gchar *start = NULL;
  static gchar **badwords_collate = NULL;

  if (badwords_collate == NULL)
    {
      gint words_size = g_strv_length ((gchar **) badwords);
      gint x = 0;

      badwords_collate = g_new0 (gchar *, words_size + 1);

      while (badwords[x] != NULL)
        {
          badwords_collate[x] = g_utf8_collate_key (badwords[x], -1);
          x++;
        }
      badwords_collate[x] = NULL;
    }

  char_len = g_utf8_strlen (string, -1);
  attrs = g_new (PangoLogAttr, char_len + 1);

  pango_get_log_attrs (string,
                       strlen (string), -1, lang, attrs, char_len + 1);

  s = (gchar *) string;
  while (i <= char_len)
    {
      gchar *end;

      if (attrs[i].is_word_start)
        start = s;
      if (attrs[i].is_word_end)
        {
          g_autofree gchar *word = NULL;

          end = s;
          word = g_strndup (start, end - start);

          if (check_good_word (word, badwords_collate))
            g_strv_builder_take (builder, g_steal_pointer (&word));
        }

      i++;
      s = g_utf8_next_char (s);
    }

  g_free (attrs);

  return g_strv_builder_end (builder);
}
