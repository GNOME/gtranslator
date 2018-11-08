/*
 * gtr-dirs.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2008 Ignacio Casal Quinteiro
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
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
static gchar *user_cache_dir = NULL;
static gchar *user_plugins_dir = NULL;
static gchar *gtr_data_dir = NULL;
static gchar *gtr_help_dir = NULL;
static gchar *gtr_locale_dir = NULL;
static gchar *gtr_lib_dir = NULL;
static gchar *gtr_plugins_dir = NULL;
static gchar *gtr_plugins_data_dir = NULL;
static gchar *gtr_pixmaps_dir = NULL;

void
gtr_dirs_init ()
{
#ifdef G_OS_WIN32
  gchar *win32_dir;

  win32_dir = g_win32_get_package_installation_directory_of_module (NULL);

  gtr_data_dir = g_build_filename (win32_dir, "share", "gtranslator", NULL);
  gtr_help_dir = g_build_filename (win32_dir,
                                   "share", "gtranslator", "help", NULL);
  gtr_locale_dir = g_build_filename (win32_dir, "share", "locale", NULL);
  gtr_lib_dir = g_build_filename (win32_dir, "lib", "gtranslator", NULL);

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
      gtr_help_dir = g_build_filename (bundle_data_dir,
                                       "gtranslator", "help" NULL);
      gtr_locale_dir = g_strdup (ige_mac_bundle_get_localedir (bundle));
      gtr_lib_dir = g_build_filename (bundle_resource_dir,
                                      "lib", "gtranslator", NULL);
    }
#endif /* !OS_OSX */
  if (gtr_data_dir == NULL)
    {
      gtr_data_dir = g_build_filename (PACKAGE_DATADIR, "gtranslator", NULL);
      gtr_help_dir = g_build_filename (PACKAGE_DATADIR, "help", NULL);
      gtr_locale_dir = g_build_filename (PACKAGE_DATADIR, "locale", NULL);
      gtr_lib_dir = g_build_filename (PACKAGE_LIBDIR, "gtranslator", NULL);
    }
#endif /* !G_OS_WIN32 */

  user_cache_dir = g_build_filename (g_get_user_cache_dir (), "gtranslator", NULL);
  user_config_dir = g_build_filename (g_get_user_config_dir (),
                                      "gtranslator", NULL);
  user_plugins_dir = g_build_filename (g_get_user_data_dir (),
                                       "gtranslator", "plugins", NULL);
  gtr_plugins_dir = g_build_filename (gtr_lib_dir, "plugins", NULL);
  gtr_plugins_data_dir = g_build_filename (gtr_data_dir, "plugins", NULL);

  gtr_pixmaps_dir = g_build_filename (gtr_data_dir, "pixmaps", NULL);
}

void
gtr_dirs_shutdown ()
{
  g_free (user_config_dir);
  g_free (user_cache_dir);
  g_free (user_plugins_dir);
  g_free (gtr_data_dir);
  g_free (gtr_help_dir);
  g_free (gtr_locale_dir);
  g_free (gtr_lib_dir);
  g_free (gtr_plugins_dir);
  g_free (gtr_plugins_data_dir);
  g_free (gtr_pixmaps_dir);
}

const gchar *
gtr_dirs_get_user_config_dir (void)
{
  return user_config_dir;
}

const gchar *
gtr_dirs_get_user_cache_dir (void)
{
  return user_cache_dir;
}

const gchar *
gtr_dirs_get_user_plugins_dir (void)
{
  return user_plugins_dir;
}

const gchar *
gtr_dirs_get_gtr_data_dir (void)
{
  return gtr_data_dir;
}

const gchar *
gtr_dirs_get_gtr_help_dir (void)
{
  return gtr_help_dir;
}

const gchar *
gtr_dirs_get_gtr_locale_dir (void)
{
  return gtr_locale_dir;
}

const gchar *
gtr_dirs_get_gtr_lib_dir (void)
{
  return gtr_lib_dir;
}

const gchar *
gtr_dirs_get_gtr_plugins_dir (void)
{
  return gtr_plugins_dir;
}

const gchar *
gtr_dirs_get_gtr_plugins_data_dir (void)
{
  return gtr_plugins_data_dir;
}

const gchar *
gtr_dirs_get_gtr_pixmaps_dir (void)
{
  return gtr_pixmaps_dir;
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
