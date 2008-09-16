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

#include "db-orig.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <string.h>

#define GTR_DB_ORIG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_DB_ORIG,     \
						 GtranslatorDbOrigPrivate))

G_DEFINE_TYPE(GtranslatorDbOrig, gtranslator_db_orig, GTR_TYPE_DB_BASE)


struct _GtranslatorDbOrigPrivate
{

};

static void
gtranslator_db_orig_init (GtranslatorDbOrig *db_orig)
{
	//db_orig->priv = GTR_DB_ORIG_GET_PRIVATE (db_orig);
	
	gtranslator_db_base_create_dabatase (GTR_DB_BASE (db_orig),
					     _("original.db"),
					     DB_HASH);
}

static void
gtranslator_db_orig_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_db_orig_parent_class)->finalize (object);
}

static void
gtranslator_db_orig_class_init (GtranslatorDbOrigClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	//g_type_class_add_private (klass, sizeof (GtranslatorDbOrigPrivate));

	object_class->finalize = gtranslator_db_orig_finalize;
}

/**
 * gtranslator_db_orig_new:
 * 
 * Creates a new #GtranslatorDbOrig object.
 * 
 * Returns: a newly #GtranslatorDbOrig object
 */
GtranslatorDbOrig *
gtranslator_db_orig_new ()
{
	GtranslatorDbOrig *db_orig;

	db_orig = g_object_new (GTR_TYPE_DB_ORIG, NULL);
	
	return db_orig;
}

/**
 * gtranslator_db_orig_write:
 * @orig: a #GtranslatorDbOrig
 * @string: string to store in the database
 * @value: the foreign key from #GtranslatorDbTrans
 *
 * Stores the @string in the database with the right foreign key @value.
 * It returns TRUE if there was not any error.
 *
 * Returns: TRUE if it was successfully stored.
 */
gboolean
gtranslator_db_orig_write (GtranslatorDbOrig *orig,
			   const gchar *string,
			   db_recno_t value)
{
	DBT key, data;
	gint error;

	memset (&key, 0, sizeof (key));
	memset (&data, 0, sizeof (data));
	key.data = (gpointer) string;
	key.size = strlen (string);
	data.data = &value;
	data.size = sizeof (value);
	
	error = gtranslator_db_base_put (GTR_DB_BASE (orig),
					 &key,
					 &data,
					 0);
	if (error != 0)
	{
		gtranslator_db_base_show_error (GTR_DB_BASE (orig), error);
		return FALSE;
	}
	
	return TRUE;
}

/**
 * gtranslator_db_orig_read:
 * @orig: a #GtranslatorDbOrig
 * @string: the primary key of the #GtranslatorDbOrig
 *
 * Gets the foreign key for #GtranslatorDbTrans for a given #GtranslatorDbOrig
 * primary key.
 *
 * Returns: the foreign key for #GtranslatorDbTrans
 */
db_recno_t
gtranslator_db_orig_read (GtranslatorDbOrig *orig,
			  const gchar *string)
{
	DBT key, data;
	gint error;

	memset (&key, 0, sizeof (key));
	memset (&data, 0, sizeof (data));
	key.data = (gpointer)string;
	key.size = strlen (string);
	
	error = gtranslator_db_base_get (GTR_DB_BASE (orig),
					 &key, &data);
	if (error != 0)
	{
		if (error != DB_NOTFOUND)
			gtranslator_db_base_show_error (GTR_DB_BASE (orig),
							error);
		return 0;
	}
	
	return *((db_recno_t*)data.data);
}
