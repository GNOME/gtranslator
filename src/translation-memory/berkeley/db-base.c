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

#include "utils.h"
#include "db-base.h"
#include DB_HEADER

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#define GTR_DB_BASE_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_DB_BASE,     \
						 GtranslatorDbBasePrivate))

G_DEFINE_TYPE (GtranslatorDbBase, gtranslator_db_base, G_TYPE_OBJECT)
     struct _GtranslatorDbBasePrivate
     {
       DB *db;
       gchar *path;
     };

     static gchar *get_db_base_directory ()
{
  gchar *config;
  gchar *db_dir;

  config = gtranslator_utils_get_user_config_dir ();

  db_dir = g_build_filename (config, "berkeley", NULL);
  g_free (config);

  if (!g_file_test (db_dir, G_FILE_TEST_IS_DIR))
    {
      GFile *file;
      GError *error = NULL;

      file = g_file_new_for_path (db_dir);

      if (!g_file_make_directory (file, NULL, &error))
	{
	  g_warning
	    ("There was an error making the gtranslator berkeley directory: %s",
	     error->message);

	  g_error_free (error);
	  g_object_unref (file);
	  g_free (db_dir);
	  return NULL;
	}

      g_object_unref (file);
    }

  return db_dir;
}

static void
gtranslator_db_base_init (GtranslatorDbBase * base)
{
  base->priv = GTR_DB_BASE_GET_PRIVATE (base);

  base->priv->path = NULL;
}

static void
gtranslator_db_base_finalize (GObject * object)
{
  GtranslatorDbBase *base = GTR_DB_BASE (object);
  int err;

  if ((err = base->priv->db->close (base->priv->db, 0)) != 0)
    gtranslator_db_base_show_error (base, err);

  g_free (base->priv->path);

  G_OBJECT_CLASS (gtranslator_db_base_parent_class)->finalize (object);
}

static void
gtranslator_db_base_class_init (GtranslatorDbBaseClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtranslatorDbBasePrivate));

  object_class->finalize = gtranslator_db_base_finalize;
}

void
gtranslator_db_base_create_dabatase (GtranslatorDbBase * base,
				     const gchar * filename, DBTYPE type)
{
  gint error;
  gchar *db_dir;

  g_return_if_fail (GTR_IS_DB_BASE (base));

  error = db_create (&base->priv->db, NULL, 0);
  if (error != 0)
    {
      gtranslator_db_base_show_error (base, error);
      return;
    }

  db_dir = get_db_base_directory ();
  base->priv->path = g_build_filename (db_dir, filename, NULL);
  g_free (db_dir);

  error = base->priv->db->open (base->priv->db,
				NULL,
				base->priv->path, NULL, type, DB_CREATE, 0);

  if (error != 0)
    {
      gtranslator_db_base_show_error (base, error);
      return;
    }
}

void
gtranslator_db_base_show_error (GtranslatorDbBase * base, int error)
{
  gchar *err = NULL;
  DB_ENV *env;
  gint e;

  switch (error)
    {
    case DB_NOTFOUND:
      break;
    case DB_RUNRECOVERY:
      g_warning (_("Running recovery..."));

      env = base->priv->db->get_env (base->priv->db);
      e = env->open (env, base->priv->path, DB_RECOVER_FATAL, 0);

      if (e != 0)
	{
	  err =
	    g_strdup_printf (_
			     ("There was an error recovering the database: %s"),
			     db_strerror (e));

	  g_warning ("%s", err);
	  g_free (err);
	}
      break;
    default:
      err = g_strdup_printf (_("There was an error in database: %s"),
			     db_strerror (error));

      g_warning ("%s", err);
      g_free (err);
      break;
    }
}

gint
gtranslator_db_base_put (GtranslatorDbBase * base,
			 DBT * key, DBT * data, u_int32_t flags)
{
  return base->priv->db->put (base->priv->db, NULL, key, data, flags);
}

gint
gtranslator_db_base_get (GtranslatorDbBase * base, DBT * key, DBT * data)
{
  return base->priv->db->get (base->priv->db, NULL, key, data, 0);
}
