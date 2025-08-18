/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <icq@gnome.org>
 *               2008  Igalia
 *               2022  Daniel Garcia Moreno <danigm@gnome.org>
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
 *   Daniel Garcia Moreno <danigm@gnome.org>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-actions.h"
#include "gtr-actions-app.h"
#include "gtr-application.h"
#include "gtr-dirs.h"
#include "gtr-settings.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "gtr-preferences-dialog.h"
#include "gtr-search-bar.h"
#include "gtr-tab.h"

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#ifdef ENABLE_INTROSPECTION
#include <girepository.h>
#endif

#ifdef GDK_WINDOWING_X11
#include <gdk/gdk.h>
#endif

struct _GtrApplication
{
  AdwApplication base_instance;
};

struct _GtrApplicationClass
{
  AdwApplicationClass parent_class;
};

typedef struct
{
  GSettings *settings;
  GSettings *window_settings;

  gchar *last_dir;

  guint first_run : 1;
} GtrApplicationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrApplication, gtr_application, ADW_TYPE_APPLICATION)

GtrWindow *gtr_application_create_window (GtrApplication *app);

static GtrWindow *
get_active_window (GtrApplication *app)
{
  return GTR_WINDOW (
      gtk_application_get_active_window (GTK_APPLICATION (app)));
}

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

static int
handle_local_options_cb (GApplication *application, GVariantDict *options, gpointer user_data) {
  if (g_variant_dict_contains (options, "new-window"))
    {
      g_application_register (application, NULL, NULL);

      if (g_application_get_is_remote (application))
        {
          g_action_group_activate_action (G_ACTION_GROUP (application),
                                          "new-window", NULL);
          return 0;
        }
    }

  if (g_variant_dict_contains (options, "version")) {
    g_print ("%s - %s\n", PACKAGE, PACKAGE_VERSION);
    return 0;
  }
  return -1;
}

static void
gtr_application_init (GtrApplication *application)
{
  const gchar *gtr_folder;
  gchar *profiles_file;
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (application);

  priv->last_dir = NULL;
  priv->first_run = FALSE;

  GOptionEntry options[] = { { "new-window", 'w', 0, G_OPTION_ARG_NONE, NULL,
                               "Open a new window", NULL },
                             { "version", 'v', 0, G_OPTION_ARG_NONE, NULL,
                               "Print version information and exit", NULL },
                             { NULL } };

  g_application_add_main_option_entries (G_APPLICATION (application), options);

  g_signal_connect (application, "handle-local-options", G_CALLBACK (handle_local_options_cb), NULL);

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
}

static void
gtr_application_dispose (GObject * object)
{
  GtrApplication *app = GTR_APPLICATION (object);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);

  g_debug ("Disposing App");


  g_clear_object (&priv->settings);
  g_clear_object (&priv->window_settings);

  G_OBJECT_CLASS (gtr_application_parent_class)->dispose (object);
}

static void
gtr_application_finalize (GObject *object)
{
  GtrApplication *app = GTR_APPLICATION (object);
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);

  g_free (priv->last_dir);

  gtk_source_finalize ();

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
  gtr_window_show_projects (window);
}

static void
find_toggle_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);

  gtr_window_toggle_search_bar (get_active_window (app));
}

static void
find_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);

  GtrTab *active_tab = gtr_window_get_active_tab (get_active_window (app));
  g_return_if_fail (active_tab != NULL);
  gtr_window_show_focus_search_bar (get_active_window (app), TRUE);
  gtr_tab_find_set_replace (active_tab, FALSE);
}

static void
find_next_activated (GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);

  GtrTab *active_tab = gtr_window_get_active_tab (get_active_window (app));
  g_return_if_fail (active_tab != NULL);
  gtr_window_show_focus_search_bar (get_active_window (app), TRUE);
  gtr_tab_find_next (active_tab);
}

static void
find_prev_activated (GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);

  GtrTab *active_tab = gtr_window_get_active_tab (get_active_window (app));
  g_return_if_fail (active_tab != NULL);
  gtr_window_show_focus_search_bar (get_active_window (app), TRUE);
  gtr_tab_find_prev (active_tab);
}

static void
find_and_replace_activated (GSimpleAction *action,
                            GVariant      *parameter,
                            gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);

  GtrTab *active_tab = gtr_window_get_active_tab (get_active_window (app));
  gtr_window_show_focus_search_bar (get_active_window (app), TRUE);
  gtr_tab_find_set_replace (active_tab, TRUE);
}

static void
copy_text_activated (GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);

  gtr_message_copy_to_translation (get_active_window (app));
}

static void
copy_original_activated (GSimpleAction *action,
                         GVariant      *parameter,
                         gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);

  gtr_message_copy_original (get_active_window (app));
}

static void
preferences_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrWindow *window;

  window = get_active_window (app);
  if (adw_application_window_get_visible_dialog (ADW_APPLICATION_WINDOW (window)) == NULL)
    gtr_show_preferences_dialog (window);
}

static void
edit_header_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_actions_edit_header (get_active_window (app));
}

static void
fix_plurals_activated (GSimpleAction *action, GVariant *parameter,
                       gpointer user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_actions_fix_plurals (get_active_window (app));
  gtr_window_add_toast_msg (get_active_window (app), _("Fixed plurals"));
}

static void
clear_msgstr_activated (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_actions_edit_clear (get_active_window (app));
}

static void
help_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_show_help (GTK_WINDOW (get_active_window (app)));
}

static void
about_activated (GSimpleAction *action,
                 GVariant      *parameter,
                 gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_about_dialog (get_active_window (app));
}

static void
quit_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtkApplication *app = GTK_APPLICATION (user_data);
  GList *windows;

  windows = gtk_application_get_windows (app);
  g_list_foreach (windows, (GFunc)gtr_file_quit, NULL);
}

static void
upload_file_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_upload_file_dialog (get_active_window (app));
}

static void
saveas_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_save_file_as_dialog (get_active_window (app));
}

static void
open_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_open_file_dialog (get_active_window (app));
}

static void
dl_activated (GSimpleAction *action,
              GVariant      *parameter,
              gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);

  GtrTab *active_tab = gtr_window_get_active_tab (get_active_window (app));
  if (active_tab == NULL) {
    gtr_window_show_dlteams (get_active_window (app));
    return;
  }

  GtrPoState state = gtr_po_get_state (gtr_tab_get_po (active_tab));

  if (state == GTR_PO_STATE_MODIFIED)
    gtr_want_to_save_current_dialog (get_active_window (app),
                                     gtr_window_show_dlteams);
  else
    gtr_window_show_dlteams (get_active_window (app));
}

static void
prev_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_message_go_to_previous (get_active_window (app));
}

static void
next_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_message_go_to_next (get_active_window (app));
}

static void
prev_no_activated (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_message_go_to_prev_fuzzy_or_untranslated (get_active_window (app));
}

static void
next_no_activated (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  gtr_message_go_to_next_fuzzy_or_untranslated (get_active_window (app));
}

static void
build_tm_activated (GSimpleAction *action,
                    GVariant      *parameter,
                    gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrWindow *w = GTR_WINDOW (get_active_window (app));
  gtr_window_show_tm_dialog (w);
}

static void
tm_activated (GSimpleAction *action,
              GVariant      *parameter,
              gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrWindow *w = GTR_WINDOW (get_active_window (app));
  gtr_window_tm_keybind (w, action);
}

static void
toggle_fuzzy_activated (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       user_data)
{
  GtrApplication *app = GTR_APPLICATION (user_data);
  GtrWindow *w = GTR_WINDOW (get_active_window (app));
  gtr_message_status_toggle_fuzzy (w);
}

static GActionEntry app_entries[]
    = { { "saveas", saveas_activated, NULL, NULL, NULL },

        { "upload_file", upload_file_activated, NULL, NULL, NULL },

        { "open", open_activated, NULL, NULL, NULL },
        { "dl", dl_activated, NULL, NULL, NULL },

        { "prev", prev_activated, NULL, NULL, NULL },
        { "next", next_activated, NULL, NULL, NULL },

        { "prev_no", prev_no_activated, NULL, NULL, NULL },
        { "next_no", next_no_activated, NULL, NULL, NULL },

        { "fuzzy", toggle_fuzzy_activated, NULL, NULL, NULL },

        { "build_tm", build_tm_activated, NULL, NULL, NULL },
        { "tm_1", tm_activated, NULL, NULL, NULL },
        { "tm_2", tm_activated, NULL, NULL, NULL },
        { "tm_3", tm_activated, NULL, NULL, NULL },
        { "tm_4", tm_activated, NULL, NULL, NULL },
        { "tm_5", tm_activated, NULL, NULL, NULL },
        { "tm_6", tm_activated, NULL, NULL, NULL },
        { "tm_7", tm_activated, NULL, NULL, NULL },
        { "tm_8", tm_activated, NULL, NULL, NULL },
        { "tm_9", tm_activated, NULL, NULL, NULL },

        { "copy_text", copy_text_activated, NULL, NULL, NULL },
        { "copy-original", copy_original_activated, NULL, NULL, NULL },
        { "find_and_replace", find_and_replace_activated, NULL, NULL, NULL },
        { "findtoggle", find_toggle_activated, NULL, NULL, NULL },
        { "find", find_activated, NULL, NULL, NULL },
        { "find-next", find_next_activated, NULL, NULL, NULL },
        { "find-prev", find_prev_activated, NULL, NULL, NULL },
        { "new-window", new_window_activated, NULL, NULL, NULL },
        { "preferences", preferences_activated, NULL, NULL, NULL },
        { "edit_header", edit_header_activated, NULL, NULL, NULL },
        { "fix-plurals", fix_plurals_activated, NULL, NULL, NULL },
        { "clear_msgstr", clear_msgstr_activated, NULL, NULL, NULL },
        { "help", help_activated, NULL, NULL, NULL },
        { "about", about_activated, NULL, NULL, NULL },
        { "quit", quit_activated, NULL, NULL, NULL } };

static void
set_kb (GApplication *app, const gchar *action, const gchar *accel)
{
  const gchar *keys[] = {accel, NULL};
  gtk_application_set_accels_for_action (GTK_APPLICATION (app), action, keys);
}

static void
gtr_application_startup (GApplication *application)
{
  G_APPLICATION_CLASS (gtr_application_parent_class)->startup (application);
  g_set_application_name (_("Translation Editor"));
  gtk_window_set_default_icon_name (PACKAGE_APPID);

  g_debug ("App startup");

  gtk_source_init ();

  g_action_map_add_action_entries (G_ACTION_MAP (application), app_entries,
                                   G_N_ELEMENTS (app_entries), application);

  // keybindings
  set_kb (application, "app.open", "<Ctrl>o");
  set_kb (application, "app.dl", "<Ctrl>d");
  set_kb (application, "win.save", "<Ctrl>s");
  set_kb (application, "app.saveas", "<Ctrl><Shift>s");
  set_kb (application, "app.upload_file", "<Ctrl>b");
  set_kb (application, "app.preferences", "<Ctrl>comma");
  set_kb (application, "app.help", "F1");
  set_kb (application, "app.quit", "<Primary>q");
  set_kb (application, "app.new-window", "<Ctrl>n");

  set_kb (application, "app.clear_msgstr", "<Ctrl>k");

  set_kb (application, "app.prev", "<Alt>Left");
  set_kb (application, "app.next", "<Alt>Right");
  set_kb (application, "app.prev_no", "<Alt>Page_Up");
  set_kb (application, "app.next_no", "<Alt>Page_Down");

  set_kb (application, "app.fuzzy", "<Ctrl>u");
  set_kb (application, "app.find", "<Ctrl>f");
  set_kb (application, "app.find_and_replace", "<Ctrl>h");
  set_kb (application, "app.find-next", "<Ctrl>g");
  set_kb (application, "app.find-prev", "<Ctrl><Shift>g");

  set_kb (application, "app.copy_text", "<Ctrl>space");
  set_kb (application, "app.copy-original", "<Ctrl><Shift>c");

  set_kb (application, "app.build_tm", "<Ctrl>plus");
  set_kb (application, "app.tm_1", "<Ctrl>1");
  set_kb (application, "app.tm_2", "<Ctrl>2");
  set_kb (application, "app.tm_3", "<Ctrl>3");
  set_kb (application, "app.tm_4", "<Ctrl>4");
  set_kb (application, "app.tm_5", "<Ctrl>5");
  set_kb (application, "app.tm_6", "<Ctrl>6");
  set_kb (application, "app.tm_7", "<Ctrl>7");
  set_kb (application, "app.tm_8", "<Ctrl>8");
  set_kb (application, "app.tm_9", "<Ctrl>9");

  set_kb (application, "window.close", "<Ctrl>w");
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
          file_list = g_slist_prepend (file_list, g_object_ref (files[i]));
    }
  window = gtr_application_create_window (GTR_APPLICATION (application));

  /* If it is the first run, the default directory was created in this
   * run, then we show the First run greeter
   */
  if (priv->first_run)
    gtr_window_show_greeter (window);
  else
    gtr_window_show_projects (window);

  if (file_list != NULL)
    {
      file_list = g_slist_reverse (file_list);
      gtr_actions_load_locations (window, file_list);
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
  GtkWindow *window;
  G_APPLICATION_CLASS (gtr_application_parent_class)->activate (application);

  g_debug ("App activate");

  window = gtk_application_get_active_window (GTK_APPLICATION (application));

  if (window != NULL)
    gtk_window_present (window);
  else
    gtr_application_setup_window (application, NULL, 0);
}

static void
gtr_application_shutdown (GApplication *application)
{
  ensure_user_config_dir ();
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
gtr_application_new (void)
{
  return GTR_APPLICATION (g_object_new (GTR_TYPE_APPLICATION,
                                        "application-id", PACKAGE_APPID,
                                        "flags", G_APPLICATION_HANDLES_OPEN,
                                        "resource-base-path", "/org/gnome/translator/",
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
  gint w, h;
  GtrApplicationPrivate *priv = gtr_application_get_instance_private (app);
  GtkWindowGroup *group;
  gboolean is_maximized;

  group = gtk_window_group_new ();
  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  window = g_object_new (GTR_TYPE_WINDOW, "application", app, NULL);
  gtk_window_group_add_window (group, GTK_WINDOW (window));
  g_object_unref (group);

  g_settings_get (priv->window_settings,
                  GTR_SETTINGS_WINDOW_SIZE,
                  "(ii)", &w, &h);

  gtk_window_set_default_size (GTK_WINDOW (window), w, h);

  is_maximized = g_settings_get_boolean (priv->window_settings,
                                         GTR_SETTINGS_WINDOW_MAXIMIZED);
  if (is_maximized)
    gtk_window_maximize (GTK_WINDOW (window));

  gtk_window_present (GTK_WINDOW (window));

  return window;
}

/**
 * gtr_application_get_views:
 * @app: the #GtranslationApplication
 * @original: TRUE if you want original TextViews.
 * @translated: TRUE if you want translated TextViews.
 *
 * Returns all the views currently present in #GtranslationApplication
 *
 * Return value: (transfer container) (element-type Gtranslator.View):
 * a newly allocated list of #GtranslationApplication objects
 */
GList *
gtr_application_get_views (GtrApplication * app,
                           gboolean original, gboolean translated)
{
  GList *res = NULL;

  g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

  res = g_list_concat (
      res, gtr_window_get_all_views (GTR_WINDOW (get_active_window (app)),
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

  return get_active_window (app);
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

  g_clear_pointer (&priv->last_dir, g_free);
  priv->last_dir = g_strdup (last_dir);
}
