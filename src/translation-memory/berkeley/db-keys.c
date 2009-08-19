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

#include "db-keys.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include DB_HEADER
#include <string.h>

#define GTR_DB_KEYS_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_DB_KEYS,     \
						 GtranslatorDbKeysPrivate))

G_DEFINE_TYPE (GtranslatorDbKeys, gtranslator_db_keys, G_TYPE_OBJECT)
     struct _GtranslatorDbKeysPrivate
     {
       db_recno_t *list;
       gsize count;
     };

     static void gtranslator_db_keys_init (GtranslatorDbKeys * db_keys)
{
  db_keys->priv = GTR_DB_KEYS_GET_PRIVATE (db_keys);
}

static void
gtranslator_db_keys_finalize (GObject * object)
{
  GtranslatorDbKeys *keys = GTR_DB_KEYS (object);

  g_free (keys->priv->list);

  G_OBJECT_CLASS (gtranslator_db_keys_parent_class)->finalize (object);
}

static void
gtranslator_db_keys_class_init (GtranslatorDbKeysClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtranslatorDbKeysPrivate));

  object_class->finalize = gtranslator_db_keys_finalize;
}

/**
 * gtranslator_db_keys_new:
 * @data: the db_recno_t with the list of keys stored into it
 *
 * Creates a new #GtranslatorDbKeys object.
 *
 * Returns: a new #GtranslatorDbKeys object
 */
GtranslatorDbKeys *
gtranslator_db_keys_new (DBT * data)
{
  GtranslatorDbKeys *db_keys;

  db_keys = g_object_new (GTR_TYPE_DB_KEYS, NULL);

  db_keys->priv->count = data->size / sizeof (db_recno_t);
  db_keys->priv->list = g_new (db_recno_t, db_keys->priv->count);
  memcpy (db_keys->priv->list, data->data, data->size);

  return db_keys;
}

/**
 * gtranslator_db_keys_new_with_size:
 * @cnt: the number of element for the list
 *
 * Creates a new #GtranslatorDbKeys object with #cnt elements.
 *
 * Returns: a new #GtranslatorDbKeys object
 */
GtranslatorDbKeys *
gtranslator_db_keys_new_with_size (gsize cnt)
{
  GtranslatorDbKeys *db_keys;

  db_keys = g_object_new (GTR_TYPE_DB_KEYS, NULL);

  db_keys->priv->list = g_new (db_recno_t, cnt);
  db_keys->priv->count = cnt;

  return db_keys;
}

/**
 * gtranslator_db_keys_get_list:
 * @db_keys: a #GtranslatorDbKeys
 * 
 * Gets the list of keys.
 * 
 * Returns: the list of keys
 */
db_recno_t *
gtranslator_db_keys_get_list (GtranslatorDbKeys * db_keys)
{
  g_return_val_if_fail (GTR_IS_DB_KEYS (db_keys), NULL);

  return db_keys->priv->list;
}

/**
 * gtranslator_db_keys_get_count:
 * @db_keys: a #GtranslatorDbKeys
 *
 * Gets the number of elements in the list.
 *
 * Returns: the number of elements in the list
 */
gsize
gtranslator_db_keys_get_count (GtranslatorDbKeys * db_keys)
{
  g_return_val_if_fail (GTR_IS_DB_KEYS (db_keys), 0);

  return db_keys->priv->count;
}

void
gtranslator_db_keys_set_count (GtranslatorDbKeys * db_keys, gsize count)
{
  g_return_if_fail (GTR_IS_DB_KEYS (db_keys));

  db_keys->priv->count = count;
}
