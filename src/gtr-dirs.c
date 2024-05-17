/*
 * gtr-dirs.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2008 Ignacio Casal Quinteiro
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

#include "gtr-dirs.h"

#ifdef OS_OSX
#include <ige-mac-bundle.h>
#endif

static gchar *user_config_dir = NULL;
static gchar *gtr_data_dir = NULL;
static gchar *gtr_locale_dir = NULL;
static gchar *gtr_sourceview_dir = NULL;

void
gtr_dirs_init (void)
{
#ifdef G_OS_WIN32
  gchar *win32_dir;

  win32_dir = g_win32_get_package_installation_directory_of_module (NULL);

  gtr_data_dir = g_build_filename (win32_dir, "share", "gtranslator", NULL);
  gtr_locale_dir = g_build_filename (win32_dir, "share", "locale", NULL);

  g_free (win32_dir);
#else /* !G_OS_WIN32 */
#ifdef OS_OSX
  IgeMacBundle *bundle = ige_mac_bundle_get_default ();

  if (ige_mac_bundle_get_is_app_bundle (bundle))
    {
      const gchar *bundle_data_dir = ige_mac_bundle_get_datadir (bundle);
      const gchar *bundle_resource_dir =
        ige_mac_bundle_get_resourcesdir (bundle);

      gtr_data_dir = g_build_filename (bundle_data_dir, "gtranslator", NULL);
      gtr_locale_dir = g_strdup (ige_mac_bundle_get_localedir (bundle));
    }
#endif /* !OS_OSX */
  if (gtr_data_dir == NULL)
    {
      gtr_data_dir = g_build_filename (PACKAGE_DATADIR, "gtranslator", NULL);
      gtr_locale_dir = g_strdup (PACKAGE_LOCALEDIR);
    }
#endif /* !G_OS_WIN32 */

  user_config_dir = g_build_filename (g_get_user_config_dir (),
                                      "gtranslator", NULL);
  gtr_sourceview_dir = g_build_filename (gtr_data_dir, "sourceview", NULL);
}

void
gtr_dirs_shutdown (void)
{
  g_free (user_config_dir);
  g_free (gtr_data_dir);
  g_free (gtr_locale_dir);
  g_free (gtr_sourceview_dir);
}

const gchar *
gtr_dirs_get_user_config_dir (void)
{
  return user_config_dir;
}

const gchar *
gtr_dirs_get_gtr_data_dir (void)
{
  return gtr_data_dir;
}

const gchar *
gtr_dirs_get_gtr_locale_dir (void)
{
  return gtr_locale_dir;
}

const gchar *
gtr_dirs_get_gtr_sourceview_dir (void)
{
  return gtr_sourceview_dir;
}

gchar *
gtr_dirs_get_ui_file (const gchar * file)
{
  gchar *ui_file;

  g_return_val_if_fail (file != NULL, NULL);

  ui_file = g_build_filename (gtr_dirs_get_gtr_data_dir (),
                              "ui", file, NULL);

  return ui_file;
}

/* ex:ts=8:noet: */
