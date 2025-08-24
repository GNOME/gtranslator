/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#include "gtr-translation-memory.h"

G_DEFINE_INTERFACE (GtrTranslationMemory, gtr_translation_memory, G_TYPE_OBJECT)

/**
 * gtr_translation_memory_store:
 * @obj: a #GtrTranslationMemory
 * @msg: message
 *
 * Stores the @msg in the database.
 */
gboolean
gtr_translation_memory_store (GtrTranslationMemory * obj, GtrMsg * msg)
{
  g_return_val_if_fail (GTR_IS_TRANSLATION_MEMORY (obj), FALSE);
  return GTR_TRANSLATION_MEMORY_GET_IFACE (obj)->store (obj, msg);
}

/* Default implementation */
static gboolean
gtr_translation_memory_store_default (GtrTranslationMemory * obj, GtrMsg * msg)
{
  g_return_val_if_reached (FALSE);
}

/**
 * gtr_translation_memory_store_list:
 * @obj: a #GtrTranslationMemory
 * @msgs: list of messages (#GtrMsg)
 *
 * Stores the messages from @msgs in the database.
 */
gboolean
gtr_translation_memory_store_list (GtrTranslationMemory * obj, GList * msgs)
{
  g_return_val_if_fail (GTR_IS_TRANSLATION_MEMORY (obj), FALSE);
  return GTR_TRANSLATION_MEMORY_GET_IFACE (obj)->store_list (obj, msgs);
}

/* Default implementation */
static gboolean
gtr_translation_memory_store_list_default (GtrTranslationMemory * obj,
                                           GList * msgs)
{
  GList * l;

  for (l = msgs; l; l = g_list_next (l))
    {
      GtrMsg *msg = GTR_MSG (l->data);
      gboolean result;

      if (!gtr_msg_is_translated (msg))
        continue;

      result = gtr_translation_memory_store (obj, msg);
      if (!result)
        return FALSE;
    }

  return TRUE;
}

/**
 * gtr_translation_memory_lookup:
 * @obj: a #GtrTranslationMemory
 * @phrase: the unstranslated text to search for translations.
 *
 * Looks for the @phrase in the database and gets a list of the #GtrTranslationMemoryMatch.
 *
 * Returns: a list of #GtrTranslationMemoryMatch.
 */
GList *
gtr_translation_memory_lookup (GtrTranslationMemory * obj,
                               const gchar * phrase)
{
  g_return_val_if_fail (GTR_IS_TRANSLATION_MEMORY (obj), 0);
  return GTR_TRANSLATION_MEMORY_GET_IFACE (obj)->lookup (obj, phrase);
}

/* Default implementation */
static GList *
gtr_translation_memory_lookup_default (GtrTranslationMemory *obj,
                                       const gchar * phrase)
{
  g_return_val_if_reached (0);
}

/**
 * gtr_translation_memory_set_max_omits:
 * @omits: the number of omits
 *
 * Sets the number of omits used in the search.
 */
void
gtr_translation_memory_set_max_omits (GtrTranslationMemory * obj, gsize omits)
{
  g_return_if_fail (GTR_IS_TRANSLATION_MEMORY (obj));
  GTR_TRANSLATION_MEMORY_GET_IFACE (obj)->set_max_omits (obj, omits);
}

/* Default implementation */
static void
gtr_translation_memory_set_max_omits_default (GtrTranslationMemory * obj,
                                              gsize omits)
{
  g_return_if_reached ();
}

/**
 * gtr_translation_memory_set_max_delta:
 * @delta: the difference in the length of strings
 *
 * Sets the difference in the length of string for searching in the database.
 */
void
gtr_translation_memory_set_max_delta (GtrTranslationMemory * obj, gsize delta)
{
  g_return_if_fail (GTR_IS_TRANSLATION_MEMORY (obj));
  GTR_TRANSLATION_MEMORY_GET_IFACE (obj)->set_max_delta (obj, delta);
}

/* Default implementation */
static void
  gtr_translation_memory_set_max_delta_default
  (GtrTranslationMemory * obj, gsize omits)
{
  g_return_if_reached ();
}

/**
 * gtr_translation_memory_set_max_items:
 * @items: the max item to return in lookup
 *
 * Sets the number of item to return in gtr_translation_memory_lookup().
 */
void
gtr_translation_memory_set_max_items (GtrTranslationMemory * obj, gint items)
{
  g_return_if_fail (GTR_IS_TRANSLATION_MEMORY (obj));
  GTR_TRANSLATION_MEMORY_GET_IFACE (obj)->set_max_items (obj, items);
}

/* Default implementation */
static void
gtr_translation_memory_set_max_items_default (GtrTranslationMemory * obj, gint items)
{
  g_return_if_reached ();
}

static void
gtr_translation_memory_default_init (GtrTranslationMemoryInterface *iface)
{
  static gboolean initialized = FALSE;

  iface->store = gtr_translation_memory_store_default;
  iface->store_list = gtr_translation_memory_store_list_default;
  iface->lookup = gtr_translation_memory_lookup_default;
  iface->set_max_omits = gtr_translation_memory_set_max_omits_default;
  iface->set_max_delta = gtr_translation_memory_set_max_delta_default;
  iface->set_max_items = gtr_translation_memory_set_max_items_default;

  if (!initialized)
    initialized = TRUE;
}


