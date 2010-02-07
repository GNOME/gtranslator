/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dialogs/gtr-assistant.h"
#include "gtr-actions.h"
#include "gtr-application.h"
#include "gtr-dirs.h"
#include "gtr-prefs-manager.h"
#include "gtr-prefs-manager-app.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "egg-toolbars-model.h"
#include "./translation-memory/gtr-translation-memory.h"
#include "./translation-memory/berkeley/gtr-berkeley.h"

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_APPLICATION_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_APPLICATION,     \
					 GtrApplicationPrivate))

G_DEFINE_TYPE (GtrApplication, gtr_application, G_TYPE_OBJECT)

struct _GtrApplicationPrivate
{
  GList *windows;
  GtrWindow *active_window;

  GList *profiles;
  GtrProfile *active_profile;

  gchar *toolbars_file;
  EggToolbarsModel *toolbars_model;

  GtkIconFactory *icon_factory;

  gchar *last_dir;

  GtrTranslationMemory *tm;

  gboolean first_run;
};

static gchar *
get_accel_file (void)
{
  gchar *config;

  config = gtr_dirs_get_user_config_dir ();

  if (config != NULL)
    {
      return g_build_filename (config, "gtr-accels", NULL);
      g_free (config);
    }

  return NULL;
}

static void
load_accels (void)
{
  gchar *filename;

  filename = get_accel_file ();
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

  filename = get_accel_file ();
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
on_window_destroy_cb (GtrWindow * window,
		      GtrApplication * app)
{
  save_accels ();
  //if(app->priv->active_window == NULL)
  g_object_unref (app);
}

static void
gtr_application_init (GtrApplication * application)
{
  gchar *gtr_folder;
  gchar *path_default_gtr_toolbar;
  gchar *profiles_file;
  gchar *dir;

  GtrApplicationPrivate *priv;

  application->priv = GTR_APPLICATION_GET_PRIVATE (application);
  priv = application->priv;

  priv->windows = NULL;
  priv->last_dir = NULL;
  priv->first_run = FALSE;
  priv->profiles = NULL;

  /*
   * Creating config folder
   */
  gtr_folder = gtr_dirs_get_user_config_dir ();

  if (!g_file_test (gtr_folder, G_FILE_TEST_IS_DIR))
    {
      GFile *file;
      GError *error = NULL;

      file = g_file_new_for_path (gtr_folder);

      if (g_file_test (gtr_folder, G_FILE_TEST_IS_REGULAR))
	{
	  if (!g_file_delete (file, NULL, &error))
	    {
	      g_warning ("There was an error deleting the "
			 "old gtranslator file: %s", error->message);
	      g_error_free (error);
	      g_object_unref (file);
	      g_free (gtr_folder);
	      gtr_application_shutdown (application);
	    }
	}

      if (!g_file_make_directory (file, NULL, &error))
	{
	  g_warning ("There was an error making the gtranslator config directory: %s",
                     error->message);

	  g_error_free (error);
	  g_object_unref (file);
	  g_free (gtr_folder);
	  gtr_application_shutdown (application);
	}

      priv->first_run = TRUE;
      g_object_unref (file);
    }

  /*
   * If the config folder exists but there is no profile
   */
  profiles_file = g_build_filename (gtr_folder, "profiles.xml", NULL);
  if (!g_file_test (profiles_file, G_FILE_TEST_EXISTS))
    priv->first_run = TRUE;
  g_free (profiles_file);

  priv->toolbars_model = egg_toolbars_model_new ();

  priv->toolbars_file = g_build_filename (gtr_folder,
					  "gtr-toolbar.xml", NULL);

  g_free (gtr_folder);

  dir = gtr_dirs_get_gtr_data_dir ();
  path_default_gtr_toolbar = g_build_filename (dir,
                                               "gtr-toolbar.xml",
                                               NULL);
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
  application->priv->tm =
    GTR_TRANSLATION_MEMORY (gtr_berkeley_new ());
  gtr_translation_memory_set_max_omits (application->priv->tm,
						gtr_prefs_manager_get_max_missing_words
						());
  gtr_translation_memory_set_max_delta (application->priv->tm,
						gtr_prefs_manager_get_max_length_diff
						());
  gtr_translation_memory_set_max_items (application->priv->tm, 10);
}


static void
gtr_application_finalize (GObject * object)
{
  GtrApplication *app = GTR_APPLICATION (object);

  if (app->priv->icon_factory)
    g_object_unref (app->priv->icon_factory);

  g_free (app->priv->last_dir);

  if (app->priv->tm)
    g_object_unref (app->priv->tm);

  G_OBJECT_CLASS (gtr_application_parent_class)->finalize (object);
}

static void
gtr_application_class_init (GtrApplicationClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrApplicationPrivate));

  object_class->finalize = gtr_application_finalize;
}

static void
app_weak_notify (gpointer data, GObject * where_the_app_was)
{
  gtk_main_quit ();
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
  static GtrApplication *instance = NULL;

  if (!instance)
    {
      instance = GTR_APPLICATION (g_object_new (GTR_TYPE_APPLICATION, NULL));

      g_object_weak_ref (G_OBJECT (instance), app_weak_notify, NULL);
    }
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
gtr_application_open_window (GtrApplication * app)
{
  GtrWindow *window;
  GdkWindowState state;
  gint w, h;

  app->priv->active_window = window =
    GTR_WINDOW (g_object_new (GTR_TYPE_WINDOW, NULL));

  state = gtr_prefs_manager_get_window_state ();

  if ((state & GDK_WINDOW_STATE_MAXIMIZED) != 0)
    {
      gtr_prefs_manager_get_default_window_size (&w, &h);
      gtk_window_set_default_size (GTK_WINDOW (window), w, h);
      gtk_window_maximize (GTK_WINDOW (window));
    }
  else
    {
      gtr_prefs_manager_get_window_size (&w, &h);
      gtk_window_set_default_size (GTK_WINDOW (window), w, h);
      gtk_window_unmaximize (GTK_WINDOW (window));
    }

  g_signal_connect (window, "delete-event",
		    G_CALLBACK (on_window_delete_event_cb), app);

  g_signal_connect (window, "destroy",
		    G_CALLBACK (on_window_destroy_cb), app);

  gtk_widget_show (GTK_WIDGET (window));

  /*
   * If it is the first run, the default directory was created in this
   * run, then we show the First run Assistant
   */
  if (app->priv->first_run)
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
_gtr_application_get_toolbars_model (GtrApplication *
					     application)
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
_gtr_application_save_toolbars_model (GtrApplication *
					      application)
{
  egg_toolbars_model_save_toolbars (application->priv->toolbars_model,
				    application->priv->toolbars_file, "1.0");
}

/**
 * gtr_application_shutdown:
 * @app: a #GtrApplication
 * 
 * Shutdowns the application.
 */
void
gtr_application_shutdown (GtrApplication * app)
{
  if (app->priv->toolbars_model)
    {
      g_object_unref (app->priv->toolbars_model);
      g_free (app->priv->toolbars_file);
      app->priv->toolbars_model = NULL;
      app->priv->toolbars_file = NULL;
    }

  g_object_unref (app);
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
							 (app->priv->
							  active_window),
							 original,
							 translated));

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
 * gtr_application_get_active_profile:
 * @app: a #GtrApplication
 * 
 * Return value: the active #GtrProfile
 **/
GtrProfile *
gtr_application_get_active_profile (GtrApplication * app)
{
  return app->priv->active_profile;
}

/**
 * gtr_application_set_profiles:
 * @app: a #GtrApplication
 * @profiles: a #GList
 *
 **/
void
gtr_application_set_active_profile (GtrApplication * app,
					    GtrProfile * profile)
{
  app->priv->active_profile = profile;
}

/**
 * gtr_application_get_profiles:
 * @app: a #GtrApplication
 * 
 * Return value: a list of all profiles.
 **/
GList *
gtr_application_get_profiles (GtrApplication * app)
{
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  return app->priv->profiles;
}

/**
 * gtr_application_set_profiles:
 * @app: a #GtrApplication
 * @profiles: a #GList
 *
 **/
void
gtr_application_set_profiles (GtrApplication * app,
				      GList * profiles)
{
  app->priv->profiles = profiles;
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
gtr_application_register_icon (GtrApplication * app,
				       const gchar * icon,
				       const gchar * stock_id)
{
  GtkIconSet *icon_set;
  GtkIconSource *icon_source = gtk_icon_source_new ();
  gchar *pixmaps_dir;
  gchar *path;
  GdkPixbuf *pixbuf;

  pixmaps_dir = gtr_dirs_get_pixmaps_dir ();
  path = g_build_filename (pixmaps_dir,
                           icon,
                           NULL);
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
_gtr_application_set_last_dir (GtrApplication * app,
				       const gchar * last_dir)
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
