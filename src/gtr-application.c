/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <icq@gnome.org>
 *               2008  Igalia 
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
 *
 * Authors:
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dialogs/gtr-assistant.h"
#include "gtr-actions.h"
#include "gtr-application.h"
#include "gtr-debug.h"
#include "gtr-dirs.h"
#include "gtr-settings.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "egg-toolbars-model.h"
#include "./translation-memory/gtr-translation-memory.h"
#include "./translation-memory/gda/gtr-gda.h"

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_APPLICATION_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_APPLICATION,     \
					 GtrApplicationPrivate))

G_DEFINE_TYPE (GtrApplication, gtr_application, UNIQUE_TYPE_APP)

struct _GtrApplicationPrivate
{
  GSettings *settings;
  GSettings *tm_settings;
  GSettings *window_settings;

  GList *windows;
  GtrWindow *active_window;

  gchar *toolbars_file;
  EggToolbarsModel *toolbars_model;

  GtkIconFactory *icon_factory;

  gchar *last_dir;

  GtrTranslationMemory *tm;

  guint first_run : 1;
};

static GtrApplication *instance = NULL;

static gboolean
ensure_user_config_dir (void)
{
  gchar *config_dir;
  gboolean ret = TRUE;
  gint res;

  config_dir = gtr_dirs_get_user_config_dir ();
  if (config_dir == NULL)
    {
      g_warning ("Could not get config directory\n");
      return FALSE;
    }

  res = g_mkdir_with_parents (config_dir, 0755);
  if (res < 0)
    {
      g_warning ("Could not create config directory\n");
      ret = FALSE;
    }

  g_free (config_dir);

  return ret;
}

static void
load_accels (void)
{
  gchar *filename;

  filename = gtr_dirs_get_user_accels_file ();
  if (filename != NULL)
    {
      gtk_accel_map_load (filename);
      g_free (filename);
    }
}

static void
save_accels (void)
{
  gchar *filename;

  filename = gtr_dirs_get_user_accels_file ();
  if (filename != NULL)
    {
      gtk_accel_map_save (filename);
      g_free (filename);
    }
}

static gboolean
on_window_delete_event_cb (GtrWindow * window,
                           GdkEvent * event, GtrApplication * app)
{
  gtr_file_quit (NULL, window);
  return TRUE;
}

static void
set_active_window (GtrApplication *app,
                   GtrWindow      *window)
{
  app->priv->active_window = window;
}

static gboolean
window_focus_in_event (GtrWindow      *window,
		       GdkEventFocus  *event,
		       GtrApplication *app)
{
  /* updates active_view and active_child when a new toplevel receives focus */
  g_return_val_if_fail (GTR_IS_WINDOW (window), FALSE);

  set_active_window (app, window);

  return FALSE;
}

static void
on_window_destroy_cb (GtrWindow *window, GtrApplication *app)
{
  app->priv->windows = g_list_remove (app->priv->windows,
                                      window);

  if (window == app->priv->active_window)
    set_active_window (app, app->priv->windows != NULL ? app->priv->windows->data : NULL);

  if(app->priv->active_window == NULL)
    {
      ensure_user_config_dir ();
      save_accels ();
      gtk_main_quit ();
    }
}

static void
gtr_application_init (GtrApplication *application)
{
  gchar *gtr_folder;
  gchar *path_default_gtr_toolbar;
  gchar *profiles_file;
  gchar *dir;
  GtrApplicationPrivate *priv;

  application->priv = GTR_APPLICATION_GET_PRIVATE (application);
  priv = application->priv;

  priv->active_window = NULL;
  priv->windows = NULL;
  priv->last_dir = NULL;
  priv->first_run = FALSE;

  /* Creating config folder */
  ensure_user_config_dir (); /* FIXME: is this really needed ? */

  /* Load settings */
  priv->settings = gtr_settings_new ();
  priv->tm_settings = g_settings_new ("org.gnome.gtranslator.preferences.tm");
  priv->window_settings = g_settings_new ("org.gnome.gtranslator.state.window");

  /* If the config folder exists but there is no profile */
  gtr_folder = gtr_dirs_get_user_config_dir ();
  profiles_file = g_build_filename (gtr_folder, "profiles.xml", NULL);
  if (!g_file_test (profiles_file, G_FILE_TEST_EXISTS))
    priv->first_run = TRUE;
  g_free (profiles_file);

  priv->toolbars_model = egg_toolbars_model_new ();

  priv->toolbars_file = g_build_filename (gtr_folder,
                                          "gtr-toolbar.xml", NULL);

  g_free (gtr_folder);

  dir = gtr_dirs_get_gtr_data_dir ();
  path_default_gtr_toolbar = g_build_filename (dir, "gtr-toolbar.xml", NULL);
  g_free (dir);

  egg_toolbars_model_load_names (priv->toolbars_model,
                                 path_default_gtr_toolbar);

  if (!egg_toolbars_model_load_toolbars (priv->toolbars_model,
                                         priv->toolbars_file))
    {
      egg_toolbars_model_load_toolbars (priv->toolbars_model,
                                        path_default_gtr_toolbar);
    }

  g_free (path_default_gtr_toolbar);

  egg_toolbars_model_set_flags (priv->toolbars_model, 0,
                                EGG_TB_MODEL_NOT_REMOVABLE);

  load_accels ();

  /* Create Icon factory */
  application->priv->icon_factory = gtk_icon_factory_new ();
  gtk_icon_factory_add_default (application->priv->icon_factory);

  /* Creating translation memory */
  application->priv->tm = GTR_TRANSLATION_MEMORY (gtr_gda_new ());
  gtr_translation_memory_set_max_omits (application->priv->tm,
                                        g_settings_get_int (priv->tm_settings,
                                                            GTR_SETTINGS_MAX_MISSING_WORDS));
  gtr_translation_memory_set_max_delta (application->priv->tm,
                                        g_settings_get_int (priv->tm_settings,
                                                            GTR_SETTINGS_MAX_LENGTH_DIFF));
  gtr_translation_memory_set_max_items (application->priv->tm, 10);
}

static void
gtr_application_dispose (GObject * object)
{
  GtrApplication *app = GTR_APPLICATION (object);

  DEBUG_PRINT ("Disposing app");

  if (app->priv->settings != NULL)
    {
      g_object_unref (app->priv->settings);
      app->priv->settings = NULL;
    }

  if (app->priv->tm_settings != NULL)
    {
      g_object_unref (app->priv->tm_settings);
      app->priv->tm_settings = NULL;
    }

  if (app->priv->window_settings != NULL)
    {
      g_object_unref (app->priv->window_settings);
      app->priv->window_settings = NULL;
    }

  if (app->priv->icon_factory != NULL)
    {
      g_object_unref (app->priv->icon_factory);
      app->priv->icon_factory = NULL;
    }

  if (app->priv->tm)
    {
      g_object_unref (app->priv->tm);
      app->priv->tm = NULL;
    }

  if (app->priv->toolbars_model)
    {
      g_object_unref (app->priv->toolbars_model);
      app->priv->toolbars_model = NULL;
    }

  G_OBJECT_CLASS (gtr_application_parent_class)->dispose (object);
}

static void
gtr_application_finalize (GObject *object)
{
  GtrApplication *app = GTR_APPLICATION (object);

  g_free (app->priv->last_dir);
  g_free (app->priv->toolbars_file);

  G_OBJECT_CLASS (gtr_application_parent_class)->finalize (object);
}

static void
gtr_application_class_init (GtrApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrApplicationPrivate));

  object_class->dispose = gtr_application_dispose;
  object_class->finalize = gtr_application_finalize;
}

GtrApplication *
_gtr_application_new ()
{
  instance = GTR_APPLICATION (g_object_new (GTR_TYPE_APPLICATION,
                                            "name", "org.gnome.Gtranslator",
                                            "startup-id", NULL, NULL));

  return instance;
}

/**
 * gtr_application_get_default:
 * 
 * Returns the default instance of the application.
 * 
 * Returns: the default instance of the application.
 */
GtrApplication *
gtr_application_get_default (void)
{
  return instance;
}

/**
 * gtr_application_open_window:
 * @app: a #GtrApplication
 *
 * Creates a new #GtrWindow and shows it.
 * 
 * Returns: the #GtrWindow to be opened
 */
GtrWindow *
gtr_application_create_window (GtrApplication *app)
{
  GtrWindow *window;
  GdkWindowState state;
  gint w, h;

  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  window = g_object_new (GTR_TYPE_WINDOW, NULL);
  set_active_window (app, window);

  app->priv->windows = g_list_prepend (app->priv->windows,
                                       window);

  state = g_settings_get_int (app->priv->window_settings,
                              GTR_SETTINGS_WINDOW_STATE);

  g_settings_get (app->priv->window_settings,
                  GTR_SETTINGS_WINDOW_SIZE,
                  "(ii)", &w, &h);

  gtk_window_set_default_size (GTK_WINDOW (window), w, h);

  if ((state & GDK_WINDOW_STATE_MAXIMIZED) != 0)
    gtk_window_maximize (GTK_WINDOW (window));
  else
    gtk_window_unmaximize (GTK_WINDOW (window));

  if ((state & GDK_WINDOW_STATE_STICKY ) != 0)
    gtk_window_stick (GTK_WINDOW (window));
  else
    gtk_window_unstick (GTK_WINDOW (window));

  g_signal_connect (window, "focus_in_event",
                    G_CALLBACK (window_focus_in_event), app);

  g_signal_connect (window, "delete-event",
                    G_CALLBACK (on_window_delete_event_cb), app);

  g_signal_connect (window, "destroy",
                    G_CALLBACK (on_window_destroy_cb), app);

  gtk_widget_show (GTK_WIDGET (window));

  /*
   * If it is the first run, the default directory was created in this
   * run, then we show the First run Assistant
   */
  if (app->priv->first_run && app->priv->windows->next == NULL)
    gtr_show_assistant (window);

  return window;
}

/**
 * _gtr_application_get_toolbars_model:
 * @application: a #GtrApplication
 * 
 * Returns the toolbar model.
 * 
 * Retuns: the toolbar model.
 */
GObject *
_gtr_application_get_toolbars_model (GtrApplication * application)
{
  return G_OBJECT (application->priv->toolbars_model);
}

/**
 * _gtr_application_save_toolbars_model:
 * @application: a #GtrApplication
 * 
 * Saves the toolbar model.
 */
void
_gtr_application_save_toolbars_model (GtrApplication * application)
{
  egg_toolbars_model_save_toolbars (application->priv->toolbars_model,
                                    application->priv->toolbars_file, "1.0");
}

/**
 * gtr_application_get_views:
 * @app: the #GtranslationApplication
 * @original: TRUE if you want original TextViews.
 * @translated: TRUE if you want translated TextViews.
 *
 * Returns all the views currently present in #GtranslationApplication.
 *
 * Return value: a newly allocated list of #GtranslationApplication objects
 */
GList *
gtr_application_get_views (GtrApplication * app,
                           gboolean original, gboolean translated)
{
  GList *res = NULL;

  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  res = g_list_concat (res,
                       gtr_window_get_all_views (GTR_WINDOW
                                                 (app->priv->active_window),
                                                 original, translated));

  return res;
}

/**
 * gtr_application_get_active_window:
 * @app: a #GtrApplication
 * 
 * Return value: the active #GtrWindow
 **/
GtrWindow *
gtr_application_get_active_window (GtrApplication * app)
{
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  return GTR_WINDOW (app->priv->active_window);
}

/**
 * gtr_application_get_windows:
 * @app: a #GtrApplication
 * 
 * Return value: a list of all opened windows.
 **/
const GList *
gtr_application_get_windows (GtrApplication * app)
{
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  if (!app->priv->windows)
    app->priv->windows =
      g_list_prepend (app->priv->windows, app->priv->active_window);

  return app->priv->windows;
}

/**
 * gtr_application_register_icon:
 * @app: a #GtrApplication
 * @icon: the name of the icon
 * @stock_id: the stock id for the new icon
 * 
 * Registers a new @icon with the @stock_id.
 */
void
gtr_application_register_icon (GtrApplication *app,
                               const gchar *icon, const gchar *stock_id)
{
  GtkIconSet *icon_set;
  GtkIconSource *icon_source;
  gchar *pixmaps_dir;
  gchar *path;
  GdkPixbuf *pixbuf;

  g_return_if_fail (GTR_IS_APPLICATION (app));
  g_return_if_fail (icon != NULL && stock_id != NULL);

  icon_source = gtk_icon_source_new ();
  pixmaps_dir = gtr_dirs_get_pixmaps_dir ();
  path = g_build_filename (pixmaps_dir, icon, NULL);
  g_free (pixmaps_dir);

  pixbuf = gdk_pixbuf_new_from_file (path, NULL);
  if (pixbuf)
    {
      icon_set = gtk_icon_set_new_from_pixbuf (pixbuf);
      gtk_icon_factory_add (app->priv->icon_factory, stock_id, icon_set);
      g_object_unref (pixbuf);
    }

  g_free (path);
  gtk_icon_source_free (icon_source);
}

/**
 * gtr_application_get_last_dir:
 * @app: a #GtrApplication
 *
 * Return value: the last dir where a file was opened in the GtkFileChooser
 */
const gchar *
_gtr_application_get_last_dir (GtrApplication * app)
{
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  return app->priv->last_dir;
}

/**
 * gtr_application_set_last_dir:
 * @app: a #GtrApplication
 * @last_dir: the path of the last directory where a file was opened in the
 * GtkFileChooser.
 */
void
_gtr_application_set_last_dir (GtrApplication * app, const gchar * last_dir)
{
  g_return_if_fail (GTR_IS_APPLICATION (app));

  app->priv->last_dir = g_strdup (last_dir);
}

GObject *
gtr_application_get_translation_memory (GtrApplication * app)
{
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  return G_OBJECT (app->priv->tm);
}

GSettings *
_gtr_application_get_settings (GtrApplication *app)
{
	g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

	return app->priv->settings;
}
