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

#include "gtr-assistant.h"
#include "gtr-actions.h"
#include "gtr-application.h"
#include "gtr-debug.h"
#include "gtr-dirs.h"
#include "gtr-settings.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "egg-toolbars-model.h"

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#ifdef ENABLE_INTROSPECTION
#include <girepository.h>
#endif

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#define GTR_APPLICATION_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_APPLICATION,     \
					 GtrApplicationPrivate))

G_DEFINE_TYPE (GtrApplication, gtr_application, GTK_TYPE_APPLICATION)

struct _GtrApplicationPrivate
{
  GSettings *settings;
  GSettings *window_settings;

  GtrWindow *active_window;

  gchar *toolbars_file;
  EggToolbarsModel *toolbars_model;

  GtkIconFactory *icon_factory;

  gchar *last_dir;

  guint first_run : 1;
};

static GtrApplication *instance = NULL;

static gboolean
ensure_user_config_dir (void)
{
  const gchar *config_dir;
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

  return ret;
}

static void
load_accels (void)
{
  gchar *filename;

  filename = g_build_filename (gtr_dirs_get_user_config_dir (),
                               "accels",
                               NULL);
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

  filename = g_build_filename (gtr_dirs_get_user_config_dir (),
                               "accels",
                               NULL);
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
  GList *windows;

  windows = gtk_application_get_windows (GTK_APPLICATION (app));

  if (window == app->priv->active_window)
    set_active_window (app, windows != NULL ? windows->data : NULL);
}

static void
gtr_application_init (GtrApplication *application)
{
  GtrApplicationPrivate *priv;
  const gchar *gtr_folder;
  gchar *path_default_gtr_toolbar;
  gchar *profiles_file;

  application->priv = GTR_APPLICATION_GET_PRIVATE (application);
  priv = application->priv;

  priv->active_window = NULL;
  priv->last_dir = NULL;
  priv->first_run = FALSE;

  /* Creating config folder */
  ensure_user_config_dir (); /* FIXME: is this really needed ? */

  /* Load settings */
  priv->settings = gtr_settings_new ();
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

  path_default_gtr_toolbar = gtr_dirs_get_ui_file ("gtr-toolbar.xml");

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
}

static void
gtr_application_dispose (GObject * object)
{
  GtrApplicationPrivate *priv = GTR_APPLICATION (object)->priv;

  DEBUG_PRINT ("Disposing app");

  g_clear_object (&priv->settings);
  g_clear_object (&priv->window_settings);
  g_clear_object (&priv->icon_factory);
  g_clear_object (&priv->toolbars_model);

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
gtr_application_startup (GApplication *application)
{
  G_APPLICATION_CLASS (gtr_application_parent_class)->startup (application);

  g_set_application_name (_("Gtranslator"));
  gtk_window_set_default_icon_name ("gtranslator");

  /* We set the default icon dir */
  gtk_icon_theme_append_search_path (gtk_icon_theme_get_default (),
                                     gtr_dirs_get_gtr_pixmaps_dir ());
}

static void
gtr_application_setup_window (GApplication *application,
                              GFile       **files,
                              gint          n_files)
{
  GtrApplicationPrivate *priv = GTR_APPLICATION (application)->priv;
  GtrWindow *window;
  GSList *file_list = NULL;

  if (files != NULL)
    {
      gint i;
      for (i = 0; i < n_files; ++i)
        /* I don't know whether GApplication gets rid of
         * malformed files passed on the command-line.
         */
        if (files[i] != NULL)
          file_list = g_slist_prepend (file_list, files[i]);
    }
  window = gtr_application_create_window (GTR_APPLICATION (application));
  gtk_application_add_window (GTK_APPLICATION (application), GTK_WINDOW (window));

  /* If it is the first run, the default directory was created in this
   * run, then we show the First run Assistant
   */
  if (priv->first_run)
    gtr_show_assistant (window);

  if (file_list != NULL)
    {
      file_list = g_slist_reverse (file_list);
      gtr_actions_load_locations (window, file_list);
      g_slist_free_full (file_list, g_object_unref);
    }
}

static void
gtr_application_open (GApplication *application,
                      GFile       **files,
                      gint          n_files,
                      const gchar  *hint)
{
  gtr_application_setup_window (application, files, n_files);
}

static void
gtr_application_activate (GApplication *application)
{
  gtr_application_setup_window (application, NULL, 0);
}

static void
gtr_application_quit_mainloop (GApplication *application)
{
  ensure_user_config_dir ();
  save_accels ();

  G_APPLICATION_CLASS (gtr_application_parent_class)->quit_mainloop (application);
}

static void
gtr_application_class_init (GtrApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrApplicationPrivate));

  object_class->dispose = gtr_application_dispose;
  object_class->finalize = gtr_application_finalize;

  application_class->startup = gtr_application_startup;
  application_class->open = gtr_application_open;
  application_class->activate = gtr_application_activate;
  application_class->quit_mainloop = gtr_application_quit_mainloop;
}

GtrApplication *
_gtr_application_new ()
{
  instance = GTR_APPLICATION (g_object_new (GTR_TYPE_APPLICATION,
                                            "application-id", "org.gnome.Gtranslator",
                                            "flags", G_APPLICATION_HANDLES_OPEN,
                                            NULL));

  return instance;
}

/**
 * gtr_application_get_default:
 * 
 * Returns the default instance of the application.
 * 
 * Returns: (transfer none): the default instance of the application.
 */
GtrApplication *
gtr_application_get_default (void)
{
  return instance;
}

/**
 * gtr_application_create_window:
 * @app: a #GtrApplication
 *
 * Creates a new #GtrWindow and shows it.
 * 
 * Returns: (transfer none):  the #GtrWindow to be opened
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

  return window;
}

/**
 * _gtr_application_get_toolbars_model:
 * @application: a #GtrApplication
 * 
 * Returns the toolbar model.
 * 
 * Returns: the toolbar model.
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
 * Return value: (transfer container): a newly allocated list of #GtranslationApplication objects
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
 * Return value: (transfer none): the active #GtrWindow
 **/
GtrWindow *
gtr_application_get_active_window (GtrApplication * app)
{
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  return GTR_WINDOW (app->priv->active_window);
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
  const gchar *pixmaps_dir;
  gchar *path;
  GdkPixbuf *pixbuf;

  g_return_if_fail (GTR_IS_APPLICATION (app));
  g_return_if_fail (icon != NULL && stock_id != NULL);

  icon_source = gtk_icon_source_new ();
  pixmaps_dir = gtr_dirs_get_gtr_pixmaps_dir ();
  path = g_build_filename (pixmaps_dir, icon, NULL);

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

GSettings *
_gtr_application_get_settings (GtrApplication *app)
{
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  return app->priv->settings;
}
