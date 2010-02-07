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

#include "gtr-db-orig.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <string.h>

#define GTR_DB_ORIG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_DB_ORIG,     \
						 GtrDbOrigPrivate))

G_DEFINE_TYPE (GtrDbOrig, gtr_db_orig, GTR_TYPE_DB_BASE)
     struct _GtrDbOrigPrivate
     {

     };

     static void gtr_db_orig_init (GtrDbOrig * db_orig)
{
  //db_orig->priv = GTR_DB_ORIG_GET_PRIVATE (db_orig);

  gtr_db_base_create_dabatase (GTR_DB_BASE (db_orig), "original.db", DB_HASH);
}

static void
gtr_db_orig_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_db_orig_parent_class)->finalize (object);
}

static void
gtr_db_orig_class_init (GtrDbOrigClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  //g_type_class_add_private (klass, sizeof (GtrDbOrigPrivate));

  object_class->finalize = gtr_db_orig_finalize;
}

/**
 * gtr_db_orig_new:
 * 
 * Creates a new #GtrDbOrig object.
 * 
 * Returns: a newly #GtrDbOrig object
 */
GtrDbOrig *
gtr_db_orig_new ()
{
  GtrDbOrig *db_orig;

  db_orig = g_object_new (GTR_TYPE_DB_ORIG, NULL);

  return db_orig;
}

/**
 * gtr_db_orig_write:
 * @orig: a #GtrDbOrig
 * @string: string to store in the database
 * @value: the foreign key from #GtrDbTrans
 *
 * Stores the @string in the database with the right foreign key @value.
 * It returns TRUE if there was not any error.
 *
 * Returns: TRUE if it was successfully stored.
 */
gboolean
gtr_db_orig_write (GtrDbOrig * orig, const gchar * string, db_recno_t value)
{
  DBT key, data;
  gint error;

  memset (&key, 0, sizeof (key));
  memset (&data, 0, sizeof (data));
  key.data = (gpointer) string;
  key.size = strlen (string);
  data.data = &value;
  data.size = sizeof (value);

  error = gtr_db_base_put (GTR_DB_BASE (orig), &key, &data, 0);
  if (error != 0)
    {
      gtr_db_base_show_error (GTR_DB_BASE (orig), error);
      return FALSE;
    }

  return TRUE;
}

/**
 * gtr_db_orig_read:
 * @orig: a #GtrDbOrig
 * @string: the primary key of the #GtrDbOrig
 *
 * Gets the foreign key for #GtrDbTrans for a given #GtrDbOrig
 * primary key.
 *
 * Returns: the foreign key for #GtrDbTrans
 */
db_recno_t
gtr_db_orig_read (GtrDbOrig * orig, const gchar * string)
{
  DBT key, data;
  gint error;

  memset (&key, 0, sizeof (key));
  memset (&data, 0, sizeof (data));
  key.data = (gpointer) string;
  key.size = strlen (string);

  error = gtr_db_base_get (GTR_DB_BASE (orig), &key, &data);
  if (error != 0)
    {
      if (error != DB_NOTFOUND)
        gtr_db_base_show_error (GTR_DB_BASE (orig), error);
      return 0;
    }

  return *((db_recno_t *) data.data);
}
