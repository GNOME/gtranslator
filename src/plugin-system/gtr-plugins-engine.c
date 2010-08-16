/*
 * plugins-engine.c
 * This file is part of gtr
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
 * Modified by the gtr Team, 2002-2005. See the AUTHORS file for a 
 * list of people on the gtr Team.  
 * See the ChangeLog files for a list of changes. 
 *
 * $Id: plugins-engine.c 6376 2008-08-10 14:01:38Z pborelli $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <glib/gi18n.h>

#include "gtr-plugins-engine.h"
#include "gtr-plugin-info-priv.h"
#include "gtr-plugin.h"
#include "gtr-debug.h"
#include "gtr-application.h"
#include "gtr-utils.h"
#include "gtr-settings.h"

#include "gtr-module.h"
#ifdef ENABLE_PYTHON
#include "gtr-python-module.h"
#endif

#define USER_GTR_PLUGINS_LOCATION ".config/gtranslator/plugins/"

#define PLUGIN_EXT	".plugin"

/* Signals */
enum
{
  ACTIVATE_PLUGIN,
  DEACTIVATE_PLUGIN,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

G_DEFINE_TYPE (GtrPluginsEngine, gtr_plugins_engine, G_TYPE_OBJECT)

struct _GtrPluginsEnginePrivate
{
  GList *plugin_list;
  GSettings *settings;
};

GtrPluginsEngine *default_engine = NULL;

     static void
       gtr_plugins_engine_activate_plugin_real
       (GtrPluginsEngine * engine, GtrPluginInfo * info);
     static void
       gtr_plugins_engine_deactivate_plugin_real
       (GtrPluginsEngine * engine, GtrPluginInfo * info);

static void
gtr_plugins_engine_load_dir (GtrPluginsEngine * engine,
                             const gchar * dir,
                             GSList * active_plugins)
{
  GError *error = NULL;
  GDir *d;
  const gchar *dirent;

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
          GtrPluginInfo *info;

          plugin_file = g_build_filename (dir, dirent, NULL);
          info = _gtr_plugin_info_new (plugin_file);
          g_free (plugin_file);

          if (info == NULL)
            continue;

          /* If a plugin with this name has already been loaded
           * drop this one (user plugins override system plugins) */
          if (gtr_plugins_engine_get_plugin_info
              (engine, info->module_name) != NULL)
            {
              g_warning ("Two or more plugins named '%s'. "
                         "Only the first will be considered.\n",
                         info->module_name);

              _gtr_plugin_info_unref (info);

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
gtr_plugins_engine_load_all (GtrPluginsEngine * engine)
{
  GSList *active_plugins = NULL;
  const gchar *home;
  const gchar *pdirs_env;
  gchar **pdirs;
  int i;

  active_plugins = gtr_settings_get_list (engine->priv->settings,
                                          GTR_SETTINGS_ACTIVE_PLUGINS);

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
        gtr_plugins_engine_load_dir (engine, pdir, active_plugins);

      g_free (pdir);
    }

  pdirs_env = g_getenv ("GTR_PLUGINS_PATH");
  /* What if no env var is set? We use the default location(s)! */
  if (pdirs_env == NULL)
    pdirs_env = GTR_PLUGINDIR;

  /* FIXME, workaround to avoid enviroment variables on windows */
#ifdef G_OS_WIN32
  pdirs_env = gtr_utils_get_win32_plugindir ();
#endif

  DEBUG_PRINT ("GTR_PLUGINS_PATH=%s", pdirs_env);
  pdirs = g_strsplit (pdirs_env, G_SEARCHPATH_SEPARATOR_S, 0);

  for (i = 0; pdirs[i] != NULL; i++)
    gtr_plugins_engine_load_dir (engine, pdirs[i], active_plugins);

  g_strfreev (pdirs);
  g_slist_foreach (active_plugins, (GFunc) g_free, NULL);
  g_slist_free (active_plugins);
}

static void
on_active_plugins_changed (GSettings        *settings,
                           const gchar      *key,
                           GtrPluginsEngine *engine)
{
  GList *pl;
  gboolean to_activate;
  GSList *active_plugins;

  //gtr_debug (DEBUG_PLUGINS);

  active_plugins = gtr_settings_get_list (settings, key);

  for (pl = engine->priv->plugin_list; pl; pl = pl->next)
    {
      GtrPluginInfo *info = (GtrPluginInfo *) pl->data;

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

static void
gtr_plugins_engine_init (GtrPluginsEngine * engine)
{
  //gtr_debug (DEBUG_PLUGINS);

  if (!g_module_supported ())
    {
      g_warning ("gtr is not able to initialize the plugins engine.");
      return;
    }

  engine->priv = G_TYPE_INSTANCE_GET_PRIVATE (engine,
                                              GTR_TYPE_PLUGINS_ENGINE,
                                              GtrPluginsEnginePrivate);

  engine->priv->settings = g_settings_new ("org.gnome.gtranslator.plugins");

  g_signal_connect (engine->priv->settings,
                    "changed::active-plugins",
                    G_CALLBACK (on_active_plugins_changed),
                    engine);

  gtr_plugins_engine_load_all (engine);
}

void
gtr_plugins_engine_garbage_collect (GtrPluginsEngine * engine)
{
  GType *module_types = g_type_children (GTR_TYPE_MODULE, NULL);
  unsigned i;
  for (i = 0; module_types[i] != 0; i++)
    {
      gpointer klass = g_type_class_peek (module_types[i]);
      if (klass != NULL)
        gtr_module_class_garbage_collect (klass);
    }
  g_free (module_types);
}

static void
gtr_plugins_engine_finalize (GObject * object)
{
  GtrPluginsEngine *engine = GTR_PLUGINS_ENGINE (object);

  //gtr_debug (DEBUG_PLUGINS);

#ifdef ENABLE_PYTHON
  /* Note: that this may cause finalization of objects (typically
   * the GtrWindow) by running the garbage collector. Since some
   * of the plugin may have installed callbacks upon object
   * finalization (typically they need to free the WindowData)
   * it must run before we get rid of the plugins.
   */
  gtr_python_shutdown ();
#endif

  g_list_foreach (engine->priv->plugin_list,
                  (GFunc) _gtr_plugin_info_unref, NULL);
  g_list_free (engine->priv->plugin_list);

  if (engine->priv->settings)
    g_object_unref (engine->priv->settings);
}

static void
gtr_plugins_engine_class_init (GtrPluginsEngineClass * klass)
{
  GType the_type = G_TYPE_FROM_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtr_plugins_engine_finalize;
  klass->activate_plugin = gtr_plugins_engine_activate_plugin_real;
  klass->deactivate_plugin = gtr_plugins_engine_deactivate_plugin_real;

  signals[ACTIVATE_PLUGIN] =
    g_signal_new ("activate-plugin",
                  the_type,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrPluginsEngineClass,
                                   activate_plugin), NULL, NULL,
                  g_cclosure_marshal_VOID__BOXED, G_TYPE_NONE, 1,
                  GTR_TYPE_PLUGIN_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

  signals[DEACTIVATE_PLUGIN] =
    g_signal_new ("deactivate-plugin",
                  the_type,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrPluginsEngineClass,
                                   deactivate_plugin), NULL, NULL,
                  g_cclosure_marshal_VOID__BOXED, G_TYPE_NONE, 1,
                  GTR_TYPE_PLUGIN_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

  g_type_class_add_private (klass, sizeof (GtrPluginsEnginePrivate));
}

GtrPluginsEngine *
gtr_plugins_engine_get_default (void)
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
gtr_plugins_engine_get_plugin_list (GtrPluginsEngine * engine)
{
  //gtr_debug (DEBUG_PLUGINS);

  return engine->priv->plugin_list;
}

static gint
compare_plugin_info_and_name (GtrPluginInfo * info, const gchar * module_name)
{
  return strcmp (info->module_name, module_name);
}

GtrPluginInfo *
gtr_plugins_engine_get_plugin_info (GtrPluginsEngine * engine,
                                    const gchar * name)
{
  GList *l = g_list_find_custom (engine->priv->plugin_list,
                                 name,
                                 (GCompareFunc) compare_plugin_info_and_name);
  return l == NULL ? NULL : (GtrPluginInfo *) l->data;
}

static gboolean
load_plugin_module (GtrPluginInfo * info)
{
  gchar *dirname;

  //gtr_debug (DEBUG_PLUGINS);

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
      if (!gtr_python_init ())
        {
          /* Mark plugin as unavailable and fail */
          info->available = FALSE;
          g_warning ("Cannot load Python plugin '%s' since gtr "
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
                 gtr_module_get_module_name (info->module),
                 gtr_module_get_path (info->module));

      g_object_unref (G_OBJECT (info->module));
      info->module = NULL;

      /* Mark plugin as unavailable and fail. */
      info->available = FALSE;

      return FALSE;
    }

  info->plugin = GTR_PLUGIN (gtr_module_new_object (info->module));

  g_type_module_unuse (G_TYPE_MODULE (info->module));

  DEBUG_PRINT ("End");

  return TRUE;
}

static void
save_active_plugin_list (GtrPluginsEngine * engine)
{
  GSList *active_plugins = NULL;
  GList *l;

  for (l = engine->priv->plugin_list; l != NULL; l = l->next)
    {
      const GtrPluginInfo *info = (const GtrPluginInfo *) l->data;
      if (info->active)
        {
          active_plugins = g_slist_prepend (active_plugins,
                                            info->module_name);
        }
    }

  gtr_settings_set_list (engine->priv->settings,
                         GTR_SETTINGS_ACTIVE_PLUGINS,
                         active_plugins);

  g_slist_free (active_plugins);
}

static void
gtr_plugins_engine_activate_plugin_real (GtrPluginsEngine *
                                         engine, GtrPluginInfo * info)
{
  gboolean res = TRUE;

  if (info->active || !info->available)
    return;

  if (info->plugin == NULL)
    res = load_plugin_module (info);

  if (res)
    {
      const GList *wins =
        gtr_application_get_windows (gtr_application_get_default ());
      for (; wins != NULL; wins = wins->next)
        gtr_plugin_activate (info->plugin, GTR_WINDOW (wins->data));

      info->active = TRUE;
    }
  else
    g_warning ("Error activating plugin '%s'", info->name);
}

gboolean
gtr_plugins_engine_activate_plugin (GtrPluginsEngine * engine,
                                    GtrPluginInfo * info)
{
  //gtr_debug (DEBUG_PLUGINS);

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
gtr_plugins_engine_deactivate_plugin_real (GtrPluginsEngine *
                                           engine, GtrPluginInfo * info)
{
  const GList *wins;

  if (!info->active || !info->available)
    return;

  wins = gtr_application_get_windows (gtr_application_get_default ());
  for (; wins != NULL; wins = wins->next)
    gtr_plugin_deactivate (info->plugin, GTR_WINDOW (wins->data));

  info->active = FALSE;
}

gboolean
gtr_plugins_engine_deactivate_plugin (GtrPluginsEngine *
                                      engine, GtrPluginInfo * info)
{
  //gtr_debug (DEBUG_PLUGINS);

  g_return_val_if_fail (info != NULL, FALSE);

  if (!info->active || !info->available)
    return TRUE;

  g_signal_emit (engine, signals[DEACTIVATE_PLUGIN], 0, info);
  if (!info->active)
    save_active_plugin_list (engine);

  return !info->active;
}

static void
reactivate_all (GtrPluginsEngine * engine, GtrWindow * window)
{
  GList *pl;

  //gtr_debug (DEBUG_PLUGINS);

  for (pl = engine->priv->plugin_list; pl; pl = pl->next)
    {
      gboolean res = TRUE;

      GtrPluginInfo *info = (GtrPluginInfo *) pl->data;

      /* If plugin is not available, don't try to activate/load it */
      if (info->available && info->active)
        {
          if (info->plugin == NULL)
            res = load_plugin_module (info);

          if (res)
            gtr_plugin_activate (info->plugin, window);
        }
    }

  DEBUG_PRINT ("End");
}

void
gtr_plugins_engine_update_plugins_ui (GtrPluginsEngine *
                                      engine,
                                      GtrWindow * window, gboolean new_window)
{
  GList *pl;

  //gtr_debug (DEBUG_PLUGINS);

  g_return_if_fail (GTR_IS_WINDOW (window));

  if (new_window)
    reactivate_all (engine, window);

  /* updated ui of all the plugins that implement update_ui */
  for (pl = engine->priv->plugin_list; pl; pl = pl->next)
    {
      GtrPluginInfo *info = (GtrPluginInfo *) pl->data;

      if (!info->available || !info->active)
        continue;

      DEBUG_PRINT ("Updating UI of %s", info->name);

      gtr_plugin_update_ui (info->plugin, window);
    }
}

void
gtr_plugins_engine_configure_plugin (GtrPluginsEngine *
                                     engine,
                                     GtrPluginInfo * info, GtkWindow * parent)
{
  GtkWidget *conf_dlg;

  GtkWindowGroup *wg;

  //gtr_debug (DEBUG_PLUGINS);

  g_return_if_fail (info != NULL);

  conf_dlg = gtr_plugin_create_configure_dialog (info->plugin);
  g_return_if_fail (conf_dlg != NULL);
  gtk_window_set_transient_for (GTK_WINDOW (conf_dlg), parent);

  wg = gtk_window_get_group (parent);
  gtk_window_group_add_window (wg, parent);
  gtk_window_group_add_window (wg, GTK_WINDOW (conf_dlg));

  gtk_window_set_modal (GTK_WINDOW (conf_dlg), TRUE);
  gtk_widget_show (conf_dlg);
}
