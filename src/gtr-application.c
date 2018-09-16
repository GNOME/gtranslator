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
#include "gtr-actions-app.h"
#include "gtr-application.h"
#include "gtr-debug.h"
#include "gtr-dirs.h"
#include "gtr-settings.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "gtr-preferences-dialog.h"

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

typedef struct
{
  GSettings *settings;
  GSettings *window_settings;
  GtkCssProvider *provider;

  GtrWindow *active_window;

  gchar *last_dir;

  guint first_run : 1;
} GtrApplicationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrApplication, gtr_application, GTK_TYPE_APPLICATION)

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
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  priv->active_window = window;
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
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);

  windows = gtk_application_get_windows (GTK_APPLICATION (app));

  if (window == priv->active_window)
    set_active_window (app, windows != NULL ? windows->data : NULL);
}

static void
gtr_application_init (GtrApplication *application)
{
  const gchar *gtr_folder;
  gchar *profiles_file;
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (application);

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

  /* Custom css */
  priv->provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_resource (priv->provider, "/org/gnome/translator/styles.css");

  load_accels ();
}

static void
gtr_application_dispose (GObject * object)
{
  GtrApplication *app = GTR_APPLICATION (object);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);

  g_clear_object (&priv->settings);
  g_clear_object (&priv->window_settings);
  g_clear_object (&priv->provider);

  G_OBJECT_CLASS (gtr_application_parent_class)->dispose (object);
}

static void
gtr_application_finalize (GObject *object)
{
  GtrApplication *app = GTR_APPLICATION (object);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);

  g_free (priv->last_dir);

  G_OBJECT_CLASS (gtr_application_parent_class)->finalize (object);
}

static void
new_window_activated (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrWindow *window;

  window = gtr_application_create_window (app);
  gtk_application_add_window (GTK_APPLICATION (app), GTK_WINDOW (window));
}

static void
preferences_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_show_preferences_dialog (priv->active_window);
}

static void
help_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_show_help (priv->active_window);
}

static void
about_activated (GSimpleAction *action,
                 GVariant      *parameter,
                 gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_about_dialog (priv->active_window);
}

static void
quit_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtkApplication *app = GTK_APPLICATION (user_data);
  GList *windows, *l;

  // FIXME: this sucks, we need a way to deal with this in a better way
  windows = gtk_application_get_windows (app);

  for (l = windows; l != NULL; l = g_list_next (l))
    {
      gtr_file_quit (NULL, l->data);
    }

  // FIXME: we may want to continue editing
  g_application_quit (G_APPLICATION (app));
}

static void
saveas_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_save_file_as_dialog (NULL, priv->active_window);
}

static void
save_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_save_current_file_dialog (NULL, priv->active_window);
}

static void
open_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_open_file_dialog (NULL, priv->active_window);
}

static void
undo_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_actions_edit_undo (NULL, priv->active_window);
}

static void
redo_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_actions_edit_redo (NULL, priv->active_window);
}

static void
prev_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_message_go_to_previous (NULL, priv->active_window);
}

static void
next_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_message_go_to_next (NULL, priv->active_window);
}

static void
prev_no_activated (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_message_go_to_prev_fuzzy_or_untranslated (NULL, priv->active_window);
}

static void
next_no_activated (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  gtr_message_go_to_next_fuzzy_or_untranslated (NULL, priv->active_window);
}

static void
build_tm_activated (GSimpleAction *action,
                    GVariant      *parameter,
                    gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  GtrWindow *w = GTR_WINDOW (priv->active_window);
  gtr_window_show_tm_dialog (w);
}

static void
toggle_fuzzy_activated (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  GtrWindow *w = GTR_WINDOW (priv->active_window);
  gtr_message_status_toggle_fuzzy (NULL, w);
}

static GActionEntry app_entries[] = {
  { "save", save_activated, NULL, NULL, NULL },
  { "saveas", saveas_activated, NULL, NULL, NULL },
  { "open", open_activated, NULL, NULL, NULL },

  { "undo", undo_activated, NULL, NULL, NULL },
  { "redo", redo_activated, NULL, NULL, NULL },

  { "prev", prev_activated, NULL, NULL, NULL },
  { "next", next_activated, NULL, NULL, NULL },

  { "prev_no", prev_no_activated, NULL, NULL, NULL },
  { "next_no", next_no_activated, NULL, NULL, NULL },

  { "fuzzy", toggle_fuzzy_activated, NULL, NULL, NULL },

  { "build_tm", build_tm_activated, NULL, NULL, NULL },
  { "new_window", new_window_activated, NULL, NULL, NULL },
  { "preferences", preferences_activated, NULL, NULL, NULL },
  { "help", help_activated, NULL, NULL, NULL },
  { "about", about_activated, NULL, NULL, NULL },
  { "quit", quit_activated, NULL, NULL, NULL }
};

static void
set_kb (GApplication *app, gchar *action, gchar *accel)
{
  const gchar *keys[] = {accel, NULL};
  gtk_application_set_accels_for_action(GTK_APPLICATION (app), action, keys);
}

static void
gtr_application_startup (GApplication *application)
{
  GtkBuilder *builder;
  GtrApplication *app = GTR_APPLICATION (application);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);

  G_APPLICATION_CLASS (gtr_application_parent_class)->startup (application);

  g_set_application_name (_("Gtranslator"));
  gtk_window_set_default_icon_name ("gtranslator");

  /* We set the default icon dir */
  gtk_icon_theme_append_search_path (gtk_icon_theme_get_default (),
                                     gtr_dirs_get_gtr_pixmaps_dir ());

  g_action_map_add_action_entries (G_ACTION_MAP (application), app_entries,
                                   G_N_ELEMENTS (app_entries), application);

  // keybindings
  set_kb (application, "app.open", "<Ctrl>o");
  set_kb (application, "app.save", "<Ctrl>s");
  set_kb (application, "app.saveas", "<Ctrl><Shift>s");
  set_kb (application, "app.preferences", "<Ctrl>p");

  set_kb (application, "app.undo", "<Ctrl>z");
  set_kb (application, "app.redo", "<Ctrl><Shift>z");

  set_kb (application, "app.prev", "<Alt>Left");
  set_kb (application, "app.next", "<Alt>Right");
  set_kb (application, "app.prev_no", "<Alt>Page_Up");
  set_kb (application, "app.next_no", "<Alt>Page_Down");

  set_kb (application, "app.fuzzy", "<Alt>f");

  builder = gtk_builder_new ();
  gtk_builder_add_from_resource (builder, "/org/gnome/translator/gtranslator-menu.ui", NULL);
  gtk_application_set_app_menu (GTK_APPLICATION (application),
                                G_MENU_MODEL (gtk_builder_get_object (builder, "appmenu")));
  g_object_unref (builder);

  gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
                                             GTK_STYLE_PROVIDER (priv->provider), 600);
}

static void
gtr_application_setup_window (GApplication *application,
                              GFile       **files,
                              gint          n_files)
{
  GtrApplication *app = GTR_APPLICATION (application);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
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

  /** loading custom styles **/
  if (g_strrstr (PACKAGE_APPID, "Devel") != NULL)
    {
      GtkStyleContext *ctx = gtk_widget_get_style_context (GTK_WIDGET (window));
      gtk_style_context_add_class (ctx, "devel");
    }

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
gtr_application_shutdown (GApplication *application)
{
  ensure_user_config_dir ();
  save_accels ();

  G_APPLICATION_CLASS (gtr_application_parent_class)->shutdown (application);
}

static void
gtr_application_class_init (GtrApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  object_class->dispose = gtr_application_dispose;
  object_class->finalize = gtr_application_finalize;

  application_class->startup = gtr_application_startup;
  application_class->open = gtr_application_open;
  application_class->activate = gtr_application_activate;
  application_class->shutdown = gtr_application_shutdown;
}

GtrApplication *
_gtr_application_new ()
{
  return GTR_APPLICATION (g_object_new (GTR_TYPE_APPLICATION,
                                        "application-id", "org.gnome.Gtranslator",
                                        "flags", G_APPLICATION_HANDLES_OPEN,
                                        NULL));
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
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);

  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  window = g_object_new (GTR_TYPE_WINDOW, "application", app, NULL);
  set_active_window (app, window);

  state = g_settings_get_int (priv->window_settings,
                              GTR_SETTINGS_WINDOW_STATE);

  g_settings_get (priv->window_settings,
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
 * gtr_application_get_views:
 * @app: the #GtranslationApplication
 * @original: TRUE if you want original TextViews.
 * @translated: TRUE if you want translated TextViews.
 *
 * Returns all the views currently present in #GtranslationApplication.
 *
 * Return value: (transfer container) (element-type Gtranslator.View):
 * a newly allocated list of #GtranslationApplication objects
 */
GList *
gtr_application_get_views (GtrApplication * app,
                           gboolean original, gboolean translated)
{
  GList *res = NULL;
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);

  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  res = g_list_concat (res,
                       gtr_window_get_all_views (GTR_WINDOW (priv->active_window),
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
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  return GTR_WINDOW (priv->active_window);
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
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  return priv->last_dir;
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
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  g_return_if_fail (GTR_IS_APPLICATION (app));

  priv->last_dir = g_strdup (last_dir);
}

GSettings *
_gtr_application_get_settings (GtrApplication *app)
{
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  return priv->settings;
}
