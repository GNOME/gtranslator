/*
 * Copyright (C) 2000-2007  Fatih Demir <kabalak@kabalak.net>
 *                          Ross Golder <ross@golder.org>
 *                          Gediminas Paulauskas <menesis@kabalak.net>
 *                          Peeter Vois <peeter@kabalak.net>
 *                          Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-application.h"
#include "gtr-dirs.h"

#include <errno.h>
#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>

#ifdef ENABLE_INTROSPECTION
#include <girepository.h>
#endif

#ifdef G_OS_WIN32
#define SAVE_DATADIR DATADIR
#undef DATADIR
#define _WIN32_WINNT 0x0500
#include <windows.h>
#define DATADIR SAVE_DATADIR
#undef SAVE_DATADIR
#endif

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
    g_warning ("Could not set PATH for Translation Editor");

  g_free (path);
}
#endif

/* The ubiquitous main function... */
gint
main (gint argc, gchar * argv[])
{
  GtrApplication *app;
  gint status;

  gtr_dirs_init ();

  /* Initialize gettext. */
  setlocale (LC_ALL, "");

  bindtextdomain (GETTEXT_PACKAGE, gtr_dirs_get_gtr_locale_dir ());
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

#ifdef G_OS_WIN32
  setup_path ();
#endif

#ifdef ENABLE_INTROSPECTION
  GOptionContext *context;
  g_autoptr (GError) error = NULL;

  context = g_option_context_new (_("— Edit PO files"));
  g_option_context_add_group (context, g_irepository_get_option_group ());
  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
      g_print (_("%s\nRun “%s --help” to see a full list of available "
                 "command line options.\n"),
               error->message, argv[0]);
      g_option_context_free (context);
      return 1;
    }
  g_option_context_free (context);
#endif

  app = gtr_application_new ();

  status = g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);
  gtr_dirs_shutdown ();

  /* Make sure settings are saved */
  g_settings_sync ();

  return status;
}
