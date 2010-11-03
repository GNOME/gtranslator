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

#include "gtr-application.h"
#include "gtr-dirs.h"

#include <errno.h>
#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>

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
    g_warning ("Could not set PATH for gtranslator");

  g_free (path);
}
#endif

/* The ubiquitous main function... */
gint
main (gint argc, gchar * argv[])
{
  GtrApplication *app;
  gint status;

  /* Init type system and threads as soon as possible */
  g_type_init ();
  g_thread_init (NULL);

  /* Initialize gettext. */
  setlocale (LC_ALL, "");

  bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

#ifdef G_OS_WIN32
  setup_path ();
#endif

  app = _gtr_application_new ();

  status = g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return 0;
}
