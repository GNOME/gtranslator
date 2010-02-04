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

#include "gtr-db-trans.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <string.h>

#define GTR_DB_TRANS_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_DB_TRANS,     \
						 GtrDbTransPrivate))

G_DEFINE_TYPE (GtrDbTrans, gtranslator_db_trans, GTR_TYPE_DB_BASE)
     struct _GtrDbTransPrivate
     {

     };

     static void gtranslator_db_trans_init (GtrDbTrans * db_trans)
{
  //db_trans->priv = GTR_DB_TRANS_GET_PRIVATE (db_trans);

  gtranslator_db_base_create_dabatase (GTR_DB_BASE (db_trans),
				       "translations.db", DB_RECNO);
}

static void
gtranslator_db_trans_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtranslator_db_trans_parent_class)->finalize (object);
}

static void
gtranslator_db_trans_class_init (GtrDbTransClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  //g_type_class_add_private (klass, sizeof (GtrDbTransPrivate));

  object_class->finalize = gtranslator_db_trans_finalize;
}

/**
 * gtranslator_db_trans_new:
 * 
 * Creates a new #GtrDbTrans object.
 * 
 * Returns: a newly #GtrDbTrans object
 */
GtrDbTrans *
gtranslator_db_trans_new ()
{
  GtrDbTrans *db_trans;

  db_trans = g_object_new (GTR_TYPE_DB_TRANS, NULL);

  return db_trans;
}

/**
 * gtranslator_db_trans_write_string:
 * @db_trans: a #GtrDbTrans
 * @translation: string to be stored in the database
 * @key: the index record in the database to be modified
 * 
 * Writes @translation in the database and returns the new key index.
 *
 * Returns: if @index is 0 then returns the new index, else returns @index
 */
db_recno_t
gtranslator_db_trans_write_string (GtrDbTrans * db_trans,
				   const gchar * translation, db_recno_t key)
{
  gchar *array[2];
  db_recno_t toret;

  array[0] = g_strdup (translation);
  array[1] = NULL;

  toret = gtranslator_db_trans_write (db_trans, array, key);

  g_free (array[0]);

  return toret;
}

/**
 * gtranslator_db_trans_write:
 * @db_trans: a #GtrDbTrans
 * @translations: array of translations
 * @index: the index record in the database to be modified
 *
 * Writes @translations in the database and returns the new key index.
 *
 * Returns: if @index is 0 then returns the new index, else returns @index
 */
db_recno_t
gtranslator_db_trans_write (GtrDbTrans * db_trans,
			    gchar ** translations, db_recno_t index)
{
  DBT key, data;
  gsize bufLen;
  gsize i;
  gint error = 0;
  gsize len;
  gint trans_len;

  trans_len = g_strv_length (translations);

  /*
   * Firstly we get buffer length to store:
   * - In the first position the number of strings
   * - then we store for each string: the length of the string
   *   and the string.
   * Graphic: (trans_len)(string_len)----->(string_len)----->.....
   * ----> = string data
   */
  for (bufLen = 0, i = 0; i < trans_len; i++)
    {
      len = strlen (translations[i]);
      bufLen += sizeof (len) + len;
    }
  bufLen += sizeof (trans_len);

  /*
   * Now we create the buffer with the length I've got in the previous
   * iteration and I store in that buffer what I said before.
   */
  u_int8_t *p, buf[bufLen];

  p = &buf[0];
  memcpy (p, &trans_len, sizeof (trans_len));
  p += sizeof (trans_len);

  for (i = 0; i < g_strv_length (translations); i++)
    {
      len = strlen (translations[i]);
      memcpy (p, &len, sizeof (len));
      p += sizeof (len);
      memcpy (p, translations[i], len);
      p += len;
    }

  /*
   * Storing the buffer with the length in the that and after that
   * we try to store it in the database.
   * If there is a problem we show it.
   */
  memset (&key, 0, sizeof (key));
  memset (&data, 0, sizeof (data));
  data.data = buf;
  data.size = bufLen;

  if (index == 0)
    {
      error = gtranslator_db_base_put (GTR_DB_BASE (db_trans),
				       &key, &data, DB_APPEND);
    }
  else
    {
      key.data = &index;
      key.size = sizeof (index);

      error = gtranslator_db_base_put (GTR_DB_BASE (db_trans),
				       &key, &data, 0);
    }

  if (error != 0)
    {
      gtranslator_db_base_show_error (GTR_DB_BASE (db_trans), error);
      return 0;
    }
  return (index == 0) ? *((db_recno_t *) key.data) : index;
}

/**
 * gtranslator_db_trans_read:
 * @db_trans: a #GtrDbTrans
 * @index: the index record in the database
 *
 * Retrieves translations stored under given @index. Returns an #GPtrArray of
 * translations or NULL if the key is absent.
 * The caller must free the #GPtrArray.
 */
GPtrArray *
gtranslator_db_trans_read (GtrDbTrans * db_trans, db_recno_t index)
{
  DBT key, data;
  gint error;
  GPtrArray *gparray;
  gint bufLen;
  u_int8_t *p;
  u_int8_t *buf;
  gint i = 0;

  memset (&key, 0, sizeof (key));
  memset (&data, 0, sizeof (data));
  key.data = &index;
  key.size = sizeof (index);

  /*
   * We get the data from the key
   */
  error = gtranslator_db_base_get (GTR_DB_BASE (db_trans), &key, &data);
  if (error != 0)
    {
      gtranslator_db_base_show_error (GTR_DB_BASE (db_trans), error);
      return NULL;
    }

  /*
   * Once we have the data we have to parse the byte array
   * and store it in our GPtrArray
   */
  gparray = g_ptr_array_new ();

  buf = data.data;

  p = &buf[0];
  memcpy (&bufLen, p, sizeof (bufLen));
  p += sizeof (bufLen);

  while (i < bufLen)
    {
      gsize len;
      gchar *data;

      memcpy (&len, p, sizeof (len));
      p += sizeof (len);

      data = g_malloc (len + 1);
      memcpy (data, p, len);
      p += len;
      data[len] = '\0';

      g_ptr_array_add (gparray, (gpointer) data);

      i++;
    }

  g_ptr_array_add (gparray, NULL);

  return gparray;
}
