/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 * 			Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-actions.h"
#include "gtr-application.h"
#include "gtr-plugins-engine.h"
#include "gtr-utils.h"
#include "gtr-dirs.h"

#include <errno.h>
#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#ifdef G_OS_WIN32
#define SAVE_DATADIR DATADIR
#undef DATADIR
#define _WIN32_WINNT 0x0500
#include <windows.h>
#define DATADIR SAVE_DATADIR
#undef SAVE_DATADIR
#endif

static gchar **file_arguments = NULL;
static gboolean option_new_window = FALSE;

static const GOptionEntry options[] = {
  { G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY, &file_arguments,
    NULL, N_("[FILE...]")},      /* collects file arguments */

  { "new-window", 'n',  0, G_OPTION_ARG_NONE, &option_new_window,
    NULL, N_("Create a new toplevel window in an existing instance of Gtranslator")},

  {NULL}
};

static gchar **
get_command_line_data ()
{
  GPtrArray *array;

  array = g_ptr_array_new ();

  if (file_arguments)
    {
      gint i;

      for (i = 0; file_arguments[i]; i++)
        {
          GFile *file;

          file = g_file_new_for_commandline_arg (file_arguments[i]);

          if (file != NULL)
            {
              g_ptr_array_add (array, g_file_get_uri (file));
              g_object_unref (file);
            }
          else
            g_print (_("%s: malformed file name or URI.\n"),
                     file_arguments[i]);
        }
    }

  g_ptr_array_add (array, NULL);

  return (gchar **)g_ptr_array_free (array, FALSE);
}

static GSList *
get_files_from_command_line_data (const gchar **data)
{
  const gchar **ptr;
  GSList *l = NULL;

  for (ptr = data; ptr != NULL && *ptr != NULL; ptr++)
    {
      l = g_slist_prepend (l, g_file_new_for_uri (*ptr));
    }

  l = g_slist_reverse (l);

  return l;
}

static UniqueResponse
unique_app_message_cb (UniqueApp *unique_app,
                       gint command,
                       UniqueMessageData *data,
                       guint timestamp,
                       gpointer user_data)
{
  GtrWindow *window;

  if (command == UNIQUE_NEW)
    window = gtr_application_create_window (GTR_APPLICATION (unique_app));
  else
    window = gtr_application_get_active_window (GTR_APPLICATION (unique_app));

  if (command == UNIQUE_OPEN)
    {
      gchar **uris;
      GSList *files;

      uris = unique_message_data_get_uris (data);
      files = get_files_from_command_line_data ((const gchar **)uris);
      g_strfreev (uris);

      if (files != NULL)
        {
          gtr_actions_load_locations (window, files);
          g_slist_foreach (files, (GFunc) g_object_unref, NULL);
          g_slist_free (files);
        }
    }

  /* set the proper interaction time on the window.
   * Fall back to roundtripping to the X server when we
   * don't have the timestamp, e.g. when launched from
   * terminal. We also need to make sure that the window
   * has been realized otherwise it will not work. lame.
   */
  if (!gtk_widget_get_realized (GTK_WIDGET (window)))
    gtk_widget_realize (GTK_WIDGET (window));

#ifdef GDK_WINDOWING_X11
  timestamp = gdk_x11_get_server_time (gtk_widget_get_window (GTK_WIDGET (window)));
#else
  timestamp = GDK_CURRENT_TIME;
#endif

  gtk_window_present_with_time (GTK_WINDOW (window), timestamp);

  return UNIQUE_RESPONSE_OK;
}

static void
send_unique_data (GtrApplication *app)
{
  UniqueMessageData *message_data = NULL;

  if (option_new_window)
    unique_app_send_message (UNIQUE_APP (app), UNIQUE_NEW, NULL);

  if (file_arguments != NULL)
    {
      gchar **uris;

      uris = get_command_line_data ();
      message_data = unique_message_data_new ();
      unique_message_data_set_uris (message_data, uris);
      g_strfreev (uris);
      unique_app_send_message (UNIQUE_APP (app), UNIQUE_OPEN, message_data);
      unique_message_data_free (message_data);
    }
  else
    unique_app_send_message (UNIQUE_APP (app), UNIQUE_ACTIVATE, NULL);
}

#ifdef G_OS_WIN32
static void
setup_path (void)
{
  gchar *path;
  gchar *installdir;
  gchar *bin;

  installdir = g_win32_get_package_installation_directory_of_module (NULL);

  bin = g_build_filename (installdir, "bin", NULL);
  g_free (installdir);

  /* Set PATH to include the gedit executable's folder */
  path = g_build_path (";", bin, g_getenv ("PATH"), NULL);
  g_free (bin);

  if (!g_setenv ("PATH", path, TRUE))
    g_warning ("Could not set PATH for gtranslator");

  g_free (path);
}
#endif

/* The ubiquitous main function... */
gint
main (gint argc, gchar * argv[])
{
  GtrApplication *app;
  GError *error = NULL;
  GtrPluginsEngine *engine;
  GtrWindow *window;
  GSList *file_list = NULL;
  GOptionContext *context;
  gchar *pixmaps_dir;
  gchar **uris;

  /* Init type system and threads as soon as possible */
  g_type_init ();
  g_thread_init (NULL);

  /* Initialize gettext. */
  setlocale (LC_ALL, "");

  bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  g_set_application_name (_("Gtranslator"));
  gtk_window_set_default_icon_name ("gtranslator");

  /* Setup command line options */
  context = g_option_context_new (_("- Edit PO files"));
  g_option_context_add_main_entries (context, options, GETTEXT_PACKAGE);

#ifdef G_OS_WIN32
  setup_path ();
#endif

  gtk_init (&argc, &argv);

  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
       g_print(_("%s\nRun '%s --help' to see a full list of available command line options.\n"),
               error->message, argv[0]);
       g_error_free (error);
       return 1;
    }

  g_option_context_free (context);

  app = _gtr_application_new ();

  if (unique_app_is_running (UNIQUE_APP (app)))
    {
      send_unique_data (app);

      /* we never popup a window... tell startup-notification
       * that we are done. */
      gdk_notify_startup_complete ();

      g_object_unref (app);
      exit (0);
    }
  else
    {
      g_signal_connect (app, "message-received",
                        G_CALLBACK (unique_app_message_cb), NULL);
    }

  /* We set the default icon dir */
  pixmaps_dir = gtr_dirs_get_pixmaps_dir ();
  gtk_icon_theme_append_search_path (gtk_icon_theme_get_default (),
                                     pixmaps_dir);
  g_free (pixmaps_dir);

  /* Init plugin engine */
  engine = gtr_plugins_engine_get_default ();

  /* Create the main app-window. */
  window = gtr_application_create_window (app);

  /* Now we open the files passed as arguments */
  uris = get_command_line_data ();
  file_list = get_files_from_command_line_data ((const gchar **)uris);
  g_strfreev (uris);
  if (file_list)
    {
      gtr_actions_load_locations (window, (const GSList *) file_list);
      g_slist_foreach (file_list, (GFunc) g_object_unref, NULL);
      g_slist_free (file_list);
    }

  /* Enter main GTK loop */
  gtk_main ();

  g_object_unref (engine);
  g_object_unref (app);

  return 0;
}
