/*
 * gtr-plugins-engine.c
 * This file is part of gtr
 *
 * Copyright (C) 2002-2005 Paolo Maggi 
 * Copyright (C) 2010 Steve Frécinaux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA. 
 */

/*
 * Modified by the gtr Team, 2002-2005. See the AUTHORS file for a 
 * list of people on the gtr Team.  
 * See the ChangeLog files for a list of changes. 
 *
 * $Id$
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <glib/gi18n.h>
#include <girepository.h>

#include "gtr-plugins-engine.h"
//#include "gtr-debug.h"
#include "gtr-application.h"
#include "gtr-dirs.h"
#include "gtr-settings.h"
#include "gtr-utils.h"

G_DEFINE_TYPE (GtrPluginsEngine, gtr_plugins_engine, PEAS_TYPE_ENGINE)

struct _GtrPluginsEnginePrivate
{
  GSettings *plugin_settings;
};

GtrPluginsEngine *default_engine = NULL;

static void
gtr_plugins_engine_init (GtrPluginsEngine * engine)
{
  gchar *typelib_dir;
  GError *error = NULL;

  engine->priv = G_TYPE_INSTANCE_GET_PRIVATE (engine,
                                              GTR_TYPE_PLUGINS_ENGINE,
                                              GtrPluginsEnginePrivate);

  peas_engine_enable_loader (PEAS_ENGINE (engine), "python");

  engine->priv->plugin_settings = g_settings_new ("org.gnome.gtranslator.plugins");

  /* Require gtr's typelib. */
  typelib_dir = g_build_filename (gtr_dirs_get_gtr_lib_dir (),
                                  "girepository-1.0", NULL);

  if (!g_irepository_require_private (g_irepository_get_default (),
                                      typelib_dir, "Gtranslator", "3.0", 0, &error))
    {
      g_warning ("Could not load Gtr repository: %s", error->message);
      g_error_free (error);
      error = NULL;
    }

  g_free (typelib_dir);

  /* This should be moved to libpeas */
  if (!g_irepository_require (g_irepository_get_default (),
                              "Peas", "1.0", 0, &error))
    {
      g_warning ("Could not load Peas repository: %s", error->message);
      g_error_free (error);
      error = NULL;
    }

  if (!g_irepository_require (g_irepository_get_default (),
                              "PeasGtk", "1.0", 0, &error))
    {
      g_warning ("Could not load PeasGtk repository: %s", error->message);
      g_error_free (error);
      error = NULL;
    }

  peas_engine_add_search_path (PEAS_ENGINE (engine),
                               gtr_dirs_get_user_plugins_dir (),
                               gtr_dirs_get_user_plugins_dir ());

  peas_engine_add_search_path (PEAS_ENGINE (engine),
                               gtr_dirs_get_gtr_plugins_dir (),
                               gtr_dirs_get_gtr_plugins_data_dir ());

  g_settings_bind (engine->priv->plugin_settings,
                   GTR_SETTINGS_ACTIVE_PLUGINS,
                   engine, "loaded-plugins", G_SETTINGS_BIND_DEFAULT);
}

static void
gtr_plugins_engine_dispose (GObject * object)
{
  GtrPluginsEngine *engine = GTR_PLUGINS_ENGINE (object);

  if (engine->priv->plugin_settings != NULL)
    {
      g_object_unref (engine->priv->plugin_settings);
      engine->priv->plugin_settings = NULL;
    }

  G_OBJECT_CLASS (gtr_plugins_engine_parent_class)->dispose (object);
}

static void
gtr_plugins_engine_class_init (GtrPluginsEngineClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_plugins_engine_dispose;

  g_type_class_add_private (klass, sizeof (GtrPluginsEnginePrivate));
}

GtrPluginsEngine *
gtr_plugins_engine_get_default (void)
{
  if (default_engine == NULL)
    {
      default_engine =
        GTR_PLUGINS_ENGINE (g_object_new (GTR_TYPE_PLUGINS_ENGINE, NULL));

      g_object_add_weak_pointer (G_OBJECT (default_engine),
                                 (gpointer) & default_engine);
    }

  return default_engine;
}

/* ex:set ts=8 noet: */
