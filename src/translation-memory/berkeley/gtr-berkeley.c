/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 *     Based in poedit transmem.cpp file:
 *     Copyright (C) 2001-2007 Vaclav Slavik
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include DB_HEADER              //This can be something like <db.h>
#include "gtr-berkeley.h"
#include "gtr-translation-memory.h"
#include "gtr-db-trans.h"
#include "gtr-db-orig.h"
#include "gtr-db-words.h"
#include "gtr-utils.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <string.h>

#define GTR_BERKELEY_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_BERKELEY,            \
						 GtrBerkeleyPrivate))

static void
gtr_translation_memory_iface_init (GtrTranslationMemoryIface * iface);

G_DEFINE_TYPE_WITH_CODE (GtrBerkeley,
                         gtr_berkeley,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GTR_TYPE_TRANSLATION_MEMORY,
                                                gtr_translation_memory_iface_init))
     struct _GtrBerkeleyPrivate
     {
       GtrDbOrig *orig;
       GtrDbTrans *trans;
       GtrDbWords *words;

       gsize max_omits;
       gsize max_delta;
       gint max_items;
     };

     static gboolean
       gtr_berkeley_store (GtrTranslationMemory * tm,
                           const gchar * original, const gchar * translation)
{
  GtrBerkeley *ber = GTR_BERKELEY (tm);
  gboolean ok;
  db_recno_t key;

  g_return_val_if_fail (GTR_IS_BERKELEY (ber), FALSE);

  key = gtr_db_orig_read (ber->priv->orig, original);
  if (key == 0)
    {
      key = gtr_db_trans_write_string (ber->priv->trans, translation, 0);

      ok = (key != 0) && gtr_db_orig_write (ber->priv->orig, original, key);
      if (ok)
        {
          gchar **words = NULL;
          gsize i;

          words = gtr_utils_split_string_in_words (original);
          gsize sz = g_strv_length (words);

          for (i = 0; i < sz; i++)
            gtr_db_words_append (ber->priv->words, words[i], sz, key);
          g_strfreev (words);
        }
      return ok;
    }
  else
    {
      gboolean found = FALSE;
      gint i = 0;
      gchar *translation_collate;
      GPtrArray *t = gtr_db_trans_read (ber->priv->trans,
                                        key);
      if (!t)
        return FALSE;

      translation_collate = g_utf8_collate_key (translation, -1);
      // -1 because we know that last element is NULL
      while (i < t->len - 1)
        {
          gchar *array_word =
            g_utf8_collate_key (g_ptr_array_index (t, i), -1);

          if (strcmp (array_word, translation_collate) == 0)
            {
              found = TRUE;
              g_free (array_word);
              break;
            }

          g_free (array_word);
          i++;
        }
      g_free (translation_collate);

      if (!found)
        {
          gchar **translations = NULL;

          /*
           * We remove the previous NULL data and then we add the new data
           */
          g_ptr_array_remove_index (t, t->len - 1);
          g_ptr_array_add (t, g_strdup (translation));
          g_ptr_array_add (t, NULL);

          translations = (gchar **) g_ptr_array_free (t, FALSE);

          db_recno_t key2 = gtr_db_trans_write (ber->priv->trans,
                                                translations,
                                                key);
          g_strfreev (translations);
          ok = (key2 != 0);
        }
      else
        {
          ok = TRUE;
          g_ptr_array_free (t, TRUE);
        }

      return ok;
    }
}

static GtrDbKeys *
union_of_db_keys (GtrBerkeley * ber,
                  gsize cnt, GtrDbKeys ** keys, gboolean * mask)
{
  gsize i, minSize;
  db_recno_t **heads = g_new (db_recno_t *, cnt);
  gsize *counters = g_new (gsize, cnt);
  GtrDbKeys *result;
  db_recno_t *res_list;

  // initialize heads and counters _and_ find size of smallest keys list
  // (union can't be larger than that)
  for (minSize = 0, i = 0; i < cnt; i++)
    {
      if (mask[i])
        {
          gsize count = gtr_db_keys_get_count (keys[i]);
          db_recno_t *list = gtr_db_keys_get_list (keys[i]);

          counters[i] = count;
          heads[i] = list;
          if (minSize == 0 || minSize > count)
            minSize = count;
        }
      else
        {
          counters[i] = 0;
          heads[i] = NULL;
        }
    }
  if (minSize == 0)
    {
      g_free (counters);
      g_free (heads);

      return NULL;
    }

  result = gtr_db_keys_new_with_size (minSize);
  res_list = gtr_db_keys_get_list (result);
  gsize res_count = 0;

  // Do union of 'cnt' sorted arrays. Algorithm: treat arrays as lists,
  // remember pointer to first unprocessed item. In each iteration, do:
  // if all heads have same value, add that value to output list, otherwise
  // move the head with smallest value one item forward. (This way we won't
  // miss any value that could possibly be in the output list, because 
  // if the smallest value of all heads is not at the beginning of other
  // lists, it cannot appear later in these lists due to their sorted nature.
  // We end processing when any head reaches end of (its) list
  db_recno_t smallestValue;
  gsize smallestIndex = 0;
  gboolean allSame;
  for (;;)
    {
      allSame = TRUE;
      smallestValue = 0;

      for (i = 0; i < cnt; i++)
        {
          if (counters[i] == 0)
            continue;
          if (smallestValue == 0)
            {
              smallestValue = *heads[i];
              smallestIndex = i;
            }
          else if (smallestValue != *heads[i])
            {
              allSame = FALSE;
              if (smallestValue > *heads[i])
                {
                  smallestValue = *heads[i];
                  smallestIndex = i;
                }
            }
        }

      if (smallestValue == 0)
        break;

      if (allSame)
        {
          gboolean breakMe = FALSE;
          res_list[res_count++] = smallestValue;
          for (i = 0; i < cnt; i++)
            {
              if (counters[i] == 0)
                continue;
              if (--counters[i] == 0)
                {
                  breakMe = TRUE;
                  break;
                }
              heads[i]++;
            }
          if (breakMe)
            break;
        }
      else
        {
          if (--counters[smallestIndex] == 0)
            break;
          heads[smallestIndex]++;
        }
    }

  g_free (counters);
  g_free (heads);
  if (res_count == 0)
    {
      g_object_unref (result);
      result = NULL;
    }
  else
    gtr_db_keys_set_count (result, res_count);

  return result;
}

static gboolean
advance_cycle (gsize omitted[], gsize depth, gsize cnt)
{
  if (++omitted[depth] == cnt)
    {
      if (depth == 0)
        return FALSE;
      depth--;
      if (!advance_cycle (omitted, depth, cnt))
        return FALSE;
      depth++;
      omitted[depth] = omitted[depth - 1] + 1;
      if (omitted[depth] >= cnt)
        return FALSE;
      return TRUE;
    }
  else
    return TRUE;
}

static gboolean
look_fuzzy (GtrBerkeley * ber,
            gchar ** words, GHashTable ** hash, gsize omits, gsize delta)
{
#define RETURN_WITH_CLEANUP(val) \
	for (i = 0; i < cnt_orig; i++) \
		if (keys[i]) \
			g_object_unref (keys[i]); \
	g_free (keys); \
	g_free (mask); \
	g_free (omitted); \
	return (val);

  gsize i, slot;
  gsize missing;
  GtrDbKeys **keys;
  gboolean *mask;
  gsize *omitted;
  gsize cnt = g_strv_length (words);
  gsize cnt_orig = cnt;
  GtrDbKeys *result;

  if (omits >= cnt)
    return FALSE;
  if (cnt + delta <= 0)
    return FALSE;

  keys = g_new (GtrDbKeys *, cnt);
  mask = g_new (gboolean, cnt);
  omitted = g_new (gsize, omits);

  for (missing = 0, slot = 0, i = 0; i < cnt; i++)
    {
      keys[i] = NULL;           // so that unused entries are NULL
      keys[slot] = gtr_db_words_read (ber->priv->words,
                                      words[i], cnt + delta);
      if (keys[slot])
        slot++;
      else
        missing++;
    }

  if (missing >= cnt || missing > omits)
    {
    RETURN_WITH_CLEANUP (FALSE)}
  cnt -= missing;
  omits -= missing;

  if (omits == 0)
    {
      for (i = 0; i < cnt; i++)
        mask[i] = TRUE;

      result = union_of_db_keys (ber, cnt, keys, mask);
      if (result != NULL)
        {
          GPtrArray *array;

          db_recno_t *list = gtr_db_keys_get_list (result);
          for (i = 0; i < gtr_db_keys_get_count (result); i++)
            {
              array = gtr_db_trans_read (ber->priv->trans, list[i]);
              if (array)
                {
                  gint j = 0;
                  gint score = 0;

                  score = (ber->priv->max_omits - omits) * 100 /
                    (ber->priv->max_omits + 1) +
                    (ber->priv->max_delta - delta) * 100 /
                    ((ber->priv->max_delta + 1) * (ber->priv->max_delta + 1));

                  if (score == 0)
                    score = 1;

                  while (j < array->len - 1)
                    {
                      gchar *string;

                      string = (gchar *) g_ptr_array_index (array, j);

                      /* The first adding is always better */
                      if (!g_hash_table_lookup (*hash, string))
                        g_hash_table_insert (*hash, string,
                                             GINT_TO_POINTER (score));

                      j++;
                    }
                  g_ptr_array_free (array, TRUE);
                }
            }
          g_object_unref (result);
        RETURN_WITH_CLEANUP (TRUE)}
    }
  else
    {
      gsize depth = omits - 1;

      for (i = 0; i < omits; i++)
        omitted[i] = i;
      for (;;)
        {
          for (i = 0; i < cnt; i++)
            mask[i] = TRUE;
          for (i = 0; i < omits; i++)
            mask[omitted[i]] = FALSE;

          result = union_of_db_keys (ber, cnt, keys, mask);
          if (result != NULL)
            {
              GPtrArray *array;

              db_recno_t *list = gtr_db_keys_get_list (result);
              for (i = 0; i < gtr_db_keys_get_count (result); i++)
                {
                  array = gtr_db_trans_read (ber->priv->trans, list[i]);
                  if (array)
                    {
                      gint j = 0;
                      gint score = 0;

                      score = (ber->priv->max_omits - omits) * 100 /
                        (ber->priv->max_omits + 1) +
                        (ber->priv->max_delta - delta) * 100 /
                        ((ber->priv->max_delta + 1) *
                         (ber->priv->max_delta + 1));

                      if (score == 0)
                        score = 1;

                      while (j < array->len - 1)
                        {
                          gchar *string;

                          string = (gchar *) g_ptr_array_index (array, j);

                          if (!g_hash_table_lookup (*hash, string))
                            g_hash_table_insert (*hash, string,
                                                 GINT_TO_POINTER (score));

                          j++;
                        }
                      g_ptr_array_free (array, TRUE);
                    }
                }
              g_object_unref (result);
            RETURN_WITH_CLEANUP (TRUE)}

          if (!advance_cycle (omitted, depth, cnt))
            break;
        }
    }
RETURN_WITH_CLEANUP (FALSE)}

static gint
insert_match_sorted (gconstpointer a, gconstpointer b)
{
  GtrTranslationMemoryMatch *match1 = (GtrTranslationMemoryMatch *) a;
  GtrTranslationMemoryMatch *match2 = (GtrTranslationMemoryMatch *) b;

  if (match1->level < match2->level)
    return 1;
  else if (match1->level > match2->level)
    return -1;
  else
    return 0;
}

static GList *
gtr_berkeley_lookup (GtrTranslationMemory * tm, const gchar * phrase)
{
  GPtrArray *array = NULL;
  gchar **words;
  gsize omits;
  gsize delta;
  GtrBerkeley *ber = GTR_BERKELEY (tm);
  GHashTable *hash;
  GHashTableIter iter;
  gpointer hkey, value;
  GList *matches = NULL;

  g_return_val_if_fail (GTR_IS_BERKELEY (ber), NULL);

  hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

  // First of all, try exact match:
  db_recno_t key = gtr_db_orig_read (ber->priv->orig, phrase);
  if (key != 0)
    {
      gint i = 0;

      array = gtr_db_trans_read (ber->priv->trans, key);

      if (array != NULL)
        {
          while (i < array->len - 1 && i < ber->priv->max_items)
            {
              g_hash_table_insert (hash,
                                   g_strdup (g_ptr_array_index (array, i)),
                                   GINT_TO_POINTER (100));
              i++;
            }

          g_ptr_array_free (array, TRUE);

          goto list;
        }
    }

  // Then, try to find inexact one within defined limits
  // (MAX_OMITS is max permitted number of unmatched words,
  // MAX_DELTA is max difference in sentences lengths).
  // Start with best matches first, continue to worse ones.
  words = gtr_utils_split_string_in_words (phrase);
  for (omits = 0;
       omits <= ber->priv->max_omits
       && g_hash_table_size (hash) < ber->priv->max_items; omits++)
    {
      for (delta = 0;
           delta <= ber->priv->max_delta
           && g_hash_table_size (hash) < ber->priv->max_items; delta++)
        {
          look_fuzzy (ber, words, &hash, omits, delta);
        }
    }

list:g_hash_table_iter_init (&iter, hash);
  while (g_hash_table_iter_next (&iter, &hkey, &value))
    {
      GtrTranslationMemoryMatch *match = g_new (GtrTranslationMemoryMatch, 1);
      match->match = g_strdup (hkey);
      match->level = GPOINTER_TO_INT (value);

      matches = g_list_prepend (matches, match);
    }
  matches = g_list_sort (matches, (GCompareFunc) insert_match_sorted);

  g_hash_table_unref (hash);

  return matches;
}

static void
gtr_berkeley_set_max_omits (GtrTranslationMemory * tm, gsize omits)
{
  GtrBerkeley *ber = GTR_BERKELEY (tm);

  ber->priv->max_omits = omits;
}

static void
gtr_berkeley_set_max_delta (GtrTranslationMemory * tm, gsize delta)
{
  GtrBerkeley *ber = GTR_BERKELEY (tm);

  ber->priv->max_delta = delta;
}

static void
gtr_berkeley_set_max_items (GtrTranslationMemory * tm, gint items)
{
  GtrBerkeley *ber = GTR_BERKELEY (tm);

  ber->priv->max_items = items;
}

static void
gtr_translation_memory_iface_init (GtrTranslationMemoryIface * iface)
{
  iface->store = gtr_berkeley_store;
  iface->lookup = gtr_berkeley_lookup;
  iface->set_max_omits = gtr_berkeley_set_max_omits;
  iface->set_max_delta = gtr_berkeley_set_max_delta;
  iface->set_max_items = gtr_berkeley_set_max_items;
}

static void
gtr_berkeley_init (GtrBerkeley * pf)
{
  pf->priv = GTR_BERKELEY_GET_PRIVATE (pf);

  pf->priv->orig = gtr_db_orig_new ();
  pf->priv->trans = gtr_db_trans_new ();
  pf->priv->words = gtr_db_words_new ();
  pf->priv->max_omits = 0;
  pf->priv->max_delta = 0;
  pf->priv->max_items = 0;
}

static void
gtr_berkeley_finalize (GObject * object)
{
  GtrBerkeley *ber = GTR_BERKELEY (object);

  g_object_unref (ber->priv->orig);
  g_object_unref (ber->priv->trans);
  g_object_unref (ber->priv->words);

  G_OBJECT_CLASS (gtr_berkeley_parent_class)->finalize (object);
}

static void
gtr_berkeley_class_init (GtrBerkeleyClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrBerkeleyPrivate));

  object_class->finalize = gtr_berkeley_finalize;
}

/**
 * gtr_berkeley_new:
 * 
 * Creates a new #GtrBerkeley object.
 *
 * Returns: a new #GtrBerkeley object
 */
GtrBerkeley *
gtr_berkeley_new ()
{
  GtrBerkeley *berkeley;

  berkeley = g_object_new (GTR_TYPE_BERKELEY, NULL);

  return berkeley;
}
