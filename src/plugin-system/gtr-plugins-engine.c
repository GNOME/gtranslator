/*
 * plugins-engine.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2002-2005 Paolo Maggi 
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
 * Modified by the gtranslator Team, 2002-2005. See the AUTHORS file for a 
 * list of people on the gtranslator Team.  
 * See the ChangeLog files for a list of changes. 
 *
 * $Id: plugins-engine.c 6376 2008-08-10 14:01:38Z pborelli $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <glib/gi18n.h>
#include <gconf/gconf-client.h>

#include "gtr-plugins-engine.h"
#include "gtr-plugin-info-priv.h"
#include "gtr-plugin.h"
#include "gtr-debug.h"
#include "gtr-application.h"
#include "gtr-utils.h"

#include "gtr-module.h"
#ifdef ENABLE_PYTHON
#include "gtranslator-python-module.h"
#endif

#define USER_GTR_PLUGINS_LOCATION ".gtranslator/plugins/"

#define GTR_PLUGINS_ENGINE_BASE_KEY "/apps/gtranslator/plugins"
#define GTR_PLUGINS_ENGINE_KEY GTR_PLUGINS_ENGINE_BASE_KEY "/active-plugins"

#define PLUGIN_EXT	".plugin"

/* Signals */
enum
{
  ACTIVATE_PLUGIN,
  DEACTIVATE_PLUGIN,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

G_DEFINE_TYPE (GtranslatorPluginsEngine, gtranslator_plugins_engine,
	       G_TYPE_OBJECT)
     struct _GtranslatorPluginsEnginePrivate
     {
       GList *plugin_list;
       GConfClient *gconf_client;
     };

     GtranslatorPluginsEngine *default_engine = NULL;

     static void
       gtranslator_plugins_engine_active_plugins_changed (GConfClient *
							  client,
							  guint cnxn_id,
							  GConfEntry * entry,
							  gpointer user_data);
     static void
       gtranslator_plugins_engine_activate_plugin_real
       (GtranslatorPluginsEngine * engine, GtranslatorPluginInfo * info);
     static void
       gtranslator_plugins_engine_deactivate_plugin_real
       (GtranslatorPluginsEngine * engine, GtranslatorPluginInfo * info);

     static void
       gtranslator_plugins_engine_load_dir (GtranslatorPluginsEngine * engine,
					    const gchar * dir,
					    GSList * active_plugins)
{
  GError *error = NULL;
  GDir *d;
  const gchar *dirent;

  g_return_if_fail (engine->priv->gconf_client != NULL);
  g_return_if_fail (dir != NULL);

  DEBUG_PRINT ("DIR: %s", dir);

  d = g_dir_open (dir, 0, &error);
  if (!d)
    {
      g_warning ("%s", error->message);
      g_error_free (error);
      return;
    }

  while ((dirent = g_dir_read_name (d)))
    {
      if (g_str_has_suffix (dirent, PLUGIN_EXT))
	{
	  gchar *plugin_file;
	  GtranslatorPluginInfo *info;

	  plugin_file = g_build_filename (dir, dirent, NULL);
	  info = _gtranslator_plugin_info_new (plugin_file);
	  g_free (plugin_file);

	  if (info == NULL)
	    continue;

	  /* If a plugin with this name has already been loaded
	   * drop this one (user plugins override system plugins) */
	  if (gtranslator_plugins_engine_get_plugin_info
	      (engine, info->module_name) != NULL)
	    {
	      g_warning ("Two or more plugins named '%s'. "
			 "Only the first will be considered.\n",
			 info->module_name);

	      _gtranslator_plugin_info_unref (info);

	      continue;
	    }

	  /* Actually, the plugin will be activated when reactivate_all
	   * will be called for the first time. */
	  info->active = g_slist_find_custom (active_plugins,
					      info->module_name,
					      (GCompareFunc) strcmp) != NULL;

	  engine->priv->plugin_list =
	    g_list_prepend (engine->priv->plugin_list, info);

	  DEBUG_PRINT ("Plugin %s loaded", info->name);
	}
    }

  g_dir_close (d);
}

static void
gtranslator_plugins_engine_load_all (GtranslatorPluginsEngine * engine)
{
  GSList *active_plugins = NULL;
  const gchar *home;
  const gchar *pdirs_env;
  gchar **pdirs;
  int i;

  active_plugins = gconf_client_get_list (engine->priv->gconf_client,
					  GTR_PLUGINS_ENGINE_KEY,
					  GCONF_VALUE_STRING, NULL);

  /* load user's plugins */
  home = g_get_home_dir ();
  if (home == NULL)
    {
      g_warning ("Could not get HOME directory\n");
    }
  else
    {
      gchar *pdir;

      pdir = g_build_filename (home, USER_GTR_PLUGINS_LOCATION, NULL);

      if (g_file_test (pdir, G_FILE_TEST_IS_DIR))
	gtranslator_plugins_engine_load_dir (engine, pdir, active_plugins);

      g_free (pdir);
    }

  pdirs_env = g_getenv ("GTR_PLUGINS_PATH");
  /* What if no env var is set? We use the default location(s)! */
  if (pdirs_env == NULL)
    pdirs_env = GTR_PLUGINDIR;

  /* FIXME, workaround to avoid enviroment variables on windows */
#ifdef G_OS_WIN32
  pdirs_env = gtranslator_utils_get_win32_plugindir ();
#endif

  DEBUG_PRINT ("GTR_PLUGINS_PATH=%s", pdirs_env);
  pdirs = g_strsplit (pdirs_env, G_SEARCHPATH_SEPARATOR_S, 0);

  for (i = 0; pdirs[i] != NULL; i++)
    gtranslator_plugins_engine_load_dir (engine, pdirs[i], active_plugins);

  g_strfreev (pdirs);
  g_slist_foreach (active_plugins, (GFunc) g_free, NULL);
  g_slist_free (active_plugins);
}

static void
gtranslator_plugins_engine_init (GtranslatorPluginsEngine * engine)
{
  //gtranslator_debug (DEBUG_PLUGINS);

  if (!g_module_supported ())
    {
      g_warning ("gtranslator is not able to initialize the plugins engine.");
      return;
    }

  engine->priv = G_TYPE_INSTANCE_GET_PRIVATE (engine,
					      GTR_TYPE_PLUGINS_ENGINE,
					      GtranslatorPluginsEnginePrivate);

  engine->priv->gconf_client = gconf_client_get_default ();
  g_return_if_fail (engine->priv->gconf_client != NULL);

  gconf_client_add_dir (engine->priv->gconf_client,
			GTR_PLUGINS_ENGINE_BASE_KEY,
			GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);

  gconf_client_notify_add (engine->priv->gconf_client,
			   GTR_PLUGINS_ENGINE_KEY,
			   gtranslator_plugins_engine_active_plugins_changed,
			   engine, NULL, NULL);

  gtranslator_plugins_engine_load_all (engine);
}

void
gtranslator_plugins_engine_garbage_collect (GtranslatorPluginsEngine * engine)
{
  GType *module_types = g_type_children (GTR_TYPE_MODULE, NULL);
  unsigned i;
  for (i = 0; module_types[i] != 0; i++)
    {
      gpointer klass = g_type_class_peek (module_types[i]);
      if (klass != NULL)
	gtranslator_module_class_garbage_collect (klass);
    }
  g_free (module_types);
}

static void
gtranslator_plugins_engine_finalize (GObject * object)
{
  GtranslatorPluginsEngine *engine = GTR_PLUGINS_ENGINE (object);

  //gtranslator_debug (DEBUG_PLUGINS);

#ifdef ENABLE_PYTHON
  /* Note: that this may cause finalization of objects (typically
   * the GtranslatorWindow) by running the garbage collector. Since some
   * of the plugin may have installed callbacks upon object
   * finalization (typically they need to free the WindowData)
   * it must run before we get rid of the plugins.
   */
  gtranslator_python_shutdown ();
#endif

  g_return_if_fail (engine->priv->gconf_client != NULL);

  g_list_foreach (engine->priv->plugin_list,
		  (GFunc) _gtranslator_plugin_info_unref, NULL);
  g_list_free (engine->priv->plugin_list);

  g_object_unref (engine->priv->gconf_client);
}

static void
gtranslator_plugins_engine_class_init (GtranslatorPluginsEngineClass * klass)
{
  GType the_type = G_TYPE_FROM_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtranslator_plugins_engine_finalize;
  klass->activate_plugin = gtranslator_plugins_engine_activate_plugin_real;
  klass->deactivate_plugin =
    gtranslator_plugins_engine_deactivate_plugin_real;

  signals[ACTIVATE_PLUGIN] =
    g_signal_new ("activate-plugin",
		  the_type,
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (GtranslatorPluginsEngineClass,
				   activate_plugin), NULL, NULL,
		  g_cclosure_marshal_VOID__BOXED, G_TYPE_NONE, 1,
		  GTR_TYPE_PLUGIN_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

  signals[DEACTIVATE_PLUGIN] =
    g_signal_new ("deactivate-plugin",
		  the_type,
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (GtranslatorPluginsEngineClass,
				   deactivate_plugin), NULL, NULL,
		  g_cclosure_marshal_VOID__BOXED, G_TYPE_NONE, 1,
		  GTR_TYPE_PLUGIN_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

  g_type_class_add_private (klass, sizeof (GtranslatorPluginsEnginePrivate));
}

GtranslatorPluginsEngine *
gtranslator_plugins_engine_get_default (void)
{
  if (default_engine != NULL)
    return default_engine;

  default_engine =
    GTR_PLUGINS_ENGINE (g_object_new (GTR_TYPE_PLUGINS_ENGINE, NULL));
  g_object_add_weak_pointer (G_OBJECT (default_engine),
			     (gpointer) & default_engine);
  return default_engine;
}

const GList *
gtranslator_plugins_engine_get_plugin_list (GtranslatorPluginsEngine * engine)
{
  //gtranslator_debug (DEBUG_PLUGINS);

  return engine->priv->plugin_list;
}

static gint
compare_plugin_info_and_name (GtranslatorPluginInfo * info,
			      const gchar * module_name)
{
  return strcmp (info->module_name, module_name);
}

GtranslatorPluginInfo *
gtranslator_plugins_engine_get_plugin_info (GtranslatorPluginsEngine * engine,
					    const gchar * name)
{
  GList *l = g_list_find_custom (engine->priv->plugin_list,
				 name,
				 (GCompareFunc) compare_plugin_info_and_name);
  return l == NULL ? NULL : (GtranslatorPluginInfo *) l->data;
}

static gboolean
load_plugin_module (GtranslatorPluginInfo * info)
{
  gchar *dirname;

  //gtranslator_debug (DEBUG_PLUGINS);

  g_return_val_if_fail (info != NULL, FALSE);
  g_return_val_if_fail (info->file != NULL, FALSE);
  g_return_val_if_fail (info->module_name != NULL, FALSE);
  g_return_val_if_fail (info->plugin == NULL, FALSE);
  g_return_val_if_fail (info->available, FALSE);

  dirname = g_path_get_dirname (info->file);
  g_return_val_if_fail (dirname != NULL, FALSE);

#ifdef ENABLE_PYTHON
  if (info->module_type == GTR_TYPE_PYTHON_MODULE)
    {
      if (!gtranslator_python_init ())
	{
	  /* Mark plugin as unavailable and fail */
	  info->available = FALSE;
	  g_warning ("Cannot load Python plugin '%s' since gtranslator "
		     "was not able to initialize the Python interpreter.",
		     info->name);
	}
    }
#endif

  info->module = GTR_MODULE (g_object_new (info->module_type,
					   "path", dirname,
					   "module-name", info->module_name,
					   NULL));

  g_free (dirname);

  if (!g_type_module_use (G_TYPE_MODULE (info->module)))
    {
      g_warning ("Cannot load plugin '%s' since file '%s' cannot be read.",
		 gtranslator_module_get_module_name (info->module),
		 gtranslator_module_get_path (info->module));

      g_object_unref (G_OBJECT (info->module));
      info->module = NULL;

      /* Mark plugin as unavailable and fail. */
      info->available = FALSE;

      return FALSE;
    }

  info->plugin = GTR_PLUGIN (gtranslator_module_new_object (info->module));

  g_type_module_unuse (G_TYPE_MODULE (info->module));

  DEBUG_PRINT ("End");

  return TRUE;
}

static void
save_active_plugin_list (GtranslatorPluginsEngine * engine)
{
  GSList *active_plugins = NULL;
  GList *l;
  gboolean res;

  for (l = engine->priv->plugin_list; l != NULL; l = l->next)
    {
      const GtranslatorPluginInfo *info =
	(const GtranslatorPluginInfo *) l->data;
      if (info->active)
	{
	  active_plugins = g_slist_prepend (active_plugins,
					    info->module_name);
	}
    }

  res = gconf_client_set_list (engine->priv->gconf_client,
			       GTR_PLUGINS_ENGINE_KEY,
			       GCONF_VALUE_STRING, active_plugins, NULL);

  if (!res)
    g_warning ("Error saving the list of active plugins.");

  g_slist_free (active_plugins);
}

static void
gtranslator_plugins_engine_activate_plugin_real (GtranslatorPluginsEngine *
						 engine,
						 GtranslatorPluginInfo * info)
{
  gboolean res = TRUE;

  if (info->active || !info->available)
    return;

  if (info->plugin == NULL)
    res = load_plugin_module (info);

  if (res)
    {
      const GList *wins =
	gtranslator_application_get_windows
	(gtranslator_application_get_default ());
      for (; wins != NULL; wins = wins->next)
	gtranslator_plugin_activate (info->plugin, GTR_WINDOW (wins->data));

      info->active = TRUE;
    }
  else
    g_warning ("Error activating plugin '%s'", info->name);
}

gboolean
gtranslator_plugins_engine_activate_plugin (GtranslatorPluginsEngine * engine,
					    GtranslatorPluginInfo * info)
{
  //gtranslator_debug (DEBUG_PLUGINS);

  g_return_val_if_fail (info != NULL, FALSE);

  if (!info->available)
    return FALSE;

  if (info->active)
    return TRUE;

  g_signal_emit (engine, signals[ACTIVATE_PLUGIN], 0, info);
  if (info->active)
    save_active_plugin_list (engine);

  return info->active;
}

static void
gtranslator_plugins_engine_deactivate_plugin_real (GtranslatorPluginsEngine *
						   engine,
						   GtranslatorPluginInfo *
						   info)
{
  const GList *wins;

  if (!info->active || !info->available)
    return;

  wins =
    gtranslator_application_get_windows (gtranslator_application_get_default
					 ());
  for (; wins != NULL; wins = wins->next)
    gtranslator_plugin_deactivate (info->plugin, GTR_WINDOW (wins->data));

  info->active = FALSE;
}

gboolean
gtranslator_plugins_engine_deactivate_plugin (GtranslatorPluginsEngine *
					      engine,
					      GtranslatorPluginInfo * info)
{
  //gtranslator_debug (DEBUG_PLUGINS);

  g_return_val_if_fail (info != NULL, FALSE);

  if (!info->active || !info->available)
    return TRUE;

  g_signal_emit (engine, signals[DEACTIVATE_PLUGIN], 0, info);
  if (!info->active)
    save_active_plugin_list (engine);

  return !info->active;
}

static void
reactivate_all (GtranslatorPluginsEngine * engine, GtranslatorWindow * window)
{
  GList *pl;

  //gtranslator_debug (DEBUG_PLUGINS);

  for (pl = engine->priv->plugin_list; pl; pl = pl->next)
    {
      gboolean res = TRUE;

      GtranslatorPluginInfo *info = (GtranslatorPluginInfo *) pl->data;

      /* If plugin is not available, don't try to activate/load it */
      if (info->available && info->active)
	{
	  if (info->plugin == NULL)
	    res = load_plugin_module (info);

	  if (res)
	    gtranslator_plugin_activate (info->plugin, window);
	}
    }

  DEBUG_PRINT ("End");
}

void
gtranslator_plugins_engine_update_plugins_ui (GtranslatorPluginsEngine *
					      engine,
					      GtranslatorWindow * window,
					      gboolean new_window)
{
  GList *pl;

  //gtranslator_debug (DEBUG_PLUGINS);

  g_return_if_fail (GTR_IS_WINDOW (window));

  if (new_window)
    reactivate_all (engine, window);

  /* updated ui of all the plugins that implement update_ui */
  for (pl = engine->priv->plugin_list; pl; pl = pl->next)
    {
      GtranslatorPluginInfo *info = (GtranslatorPluginInfo *) pl->data;

      if (!info->available || !info->active)
	continue;

      DEBUG_PRINT ("Updating UI of %s", info->name);

      gtranslator_plugin_update_ui (info->plugin, window);
    }
}

void
gtranslator_plugins_engine_configure_plugin (GtranslatorPluginsEngine *
					     engine,
					     GtranslatorPluginInfo * info,
					     GtkWindow * parent)
{
  GtkWidget *conf_dlg;

  GtkWindowGroup *wg;

  //gtranslator_debug (DEBUG_PLUGINS);

  g_return_if_fail (info != NULL);

  conf_dlg = gtranslator_plugin_create_configure_dialog (info->plugin);
  g_return_if_fail (conf_dlg != NULL);
  gtk_window_set_transient_for (GTK_WINDOW (conf_dlg), parent);

  wg = parent->group;
  if (wg == NULL)
    {
      wg = gtk_window_group_new ();
      gtk_window_group_add_window (wg, parent);
    }

  gtk_window_group_add_window (wg, GTK_WINDOW (conf_dlg));

  gtk_window_set_modal (GTK_WINDOW (conf_dlg), TRUE);
  gtk_widget_show (conf_dlg);
}

static void
gtranslator_plugins_engine_active_plugins_changed (GConfClient * client,
						   guint cnxn_id,
						   GConfEntry * entry,
						   gpointer user_data)
{
  GtranslatorPluginsEngine *engine;
  GList *pl;
  gboolean to_activate;
  GSList *active_plugins;

  //gtranslator_debug (DEBUG_PLUGINS);

  g_return_if_fail (entry->key != NULL);
  g_return_if_fail (entry->value != NULL);

  engine = GTR_PLUGINS_ENGINE (user_data);

  if (!((entry->value->type == GCONF_VALUE_LIST) &&
	(gconf_value_get_list_type (entry->value) == GCONF_VALUE_STRING)))
    {
      g_warning ("The gconf key '%s' may be corrupted.",
		 GTR_PLUGINS_ENGINE_KEY);
      return;
    }

  active_plugins = gconf_client_get_list (engine->priv->gconf_client,
					  GTR_PLUGINS_ENGINE_KEY,
					  GCONF_VALUE_STRING, NULL);

  for (pl = engine->priv->plugin_list; pl; pl = pl->next)
    {
      GtranslatorPluginInfo *info = (GtranslatorPluginInfo *) pl->data;

      if (!info->available)
	continue;

      to_activate = (g_slist_find_custom (active_plugins,
					  info->module_name,
					  (GCompareFunc) strcmp) != NULL);

      if (!info->active && to_activate)
	g_signal_emit (engine, signals[ACTIVATE_PLUGIN], 0, info);
      else if (info->active && !to_activate)
	g_signal_emit (engine, signals[DEACTIVATE_PLUGIN], 0, info);
    }

  g_slist_foreach (active_plugins, (GFunc) g_free, NULL);
  g_slist_free (active_plugins);
}
