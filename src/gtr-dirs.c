/*
 * gtranslator-dirs.c
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA. 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-dirs.h"

gchar *
gtranslator_dirs_get_user_config_dir (void)
{
	gchar *config_dir = NULL;

	config_dir = g_build_filename (g_get_user_config_dir (),
				       "gtranslator",
				       NULL);

	return config_dir;
}

gchar *
gtranslator_dirs_get_user_cache_dir (void)
{
	const gchar *cache_dir;

	cache_dir = g_get_user_cache_dir ();

	return g_build_filename (cache_dir,
				 "gtranslator",
				 NULL);
}

gchar *
gtranslator_dirs_get_user_plugins_dir (void)
{
	gchar *config_dir;
	gchar *plugin_dir;

	config_dir = gtranslator_dirs_get_user_config_dir ();

	plugin_dir = g_build_filename (config_dir,
				       "plugins",
				       NULL);
	g_free (config_dir);
	
	return plugin_dir;
}

gchar *
gtranslator_dirs_get_user_accels_file (void)
{
	gchar *accels = NULL;
	gchar *config_dir = NULL;

	config_dir = gtranslator_dirs_get_user_config_dir ();
	accels = g_build_filename (config_dir,
				   "accels",
				   "gtranslator",
				   NULL);

	g_free (config_dir);

	return accels;
}

gchar *
gtranslator_dirs_get_gtranslator_data_dir (void)
{
	gchar *data_dir;

#ifdef G_OS_WIN32
	gchar *win32_dir;
	
	win32_dir = g_win32_get_package_installation_directory_of_module (NULL);

	data_dir = g_build_filename (win32_dir,
				     "share",
				     "gtranslator",
				     NULL);
	
	g_free (win32_dir);
#else
	data_dir = g_build_filename (DATADIR,
	                             "gtranslator",
	                             NULL);
#endif

	return data_dir;
}

gchar *
gtranslator_dirs_get_gtranslator_locale_dir (void)
{
	gchar *locale_dir;

#ifdef G_OS_WIN32
	gchar *win32_dir;
	
	win32_dir = g_win32_get_package_installation_directory_of_module (NULL);

	locale_dir = g_build_filename (win32_dir,
				       "share",
				       "locale",
				       NULL);
	
	g_free (win32_dir);
#else
	locale_dir = g_build_filename (DATADIR,
				       "locale",
				       NULL);
#endif

	return locale_dir;
}

gchar *
gtranslator_dirs_get_gtranslator_lib_dir (void)
{
	gchar *lib_dir;

#ifdef G_OS_WIN32
	gchar *win32_dir;
	
	win32_dir = g_win32_get_package_installation_directory_of_module (NULL);

	lib_dir = g_build_filename (win32_dir,
				    "lib",
				    "gtranslator",
				    NULL);
	
	g_free (win32_dir);
#else
	lib_dir = g_build_filename (LIBDIR,
				    "gtranslator",
				    NULL);
#endif

	return lib_dir;
}

gchar *
gtranslator_dirs_get_gtranslator_plugins_dir (void)
{
	gchar *lib_dir;
	gchar *plugin_dir;
	
	lib_dir = gtranslator_dirs_get_gtranslator_lib_dir ();
	
	plugin_dir = g_build_filename (lib_dir,
				       "plugins",
				       NULL);
	g_free (lib_dir);
	
	return plugin_dir;
}

gchar *
gtranslator_dirs_get_ui_file (const gchar *file)
{
	gchar *datadir;
	gchar *ui_file;

	g_return_val_if_fail (file != NULL, NULL);
	
	datadir = gtranslator_dirs_get_gtranslator_data_dir ();
	ui_file = g_build_filename (datadir,
				    "ui",
				    file,
				    NULL);
	g_free (datadir);
	
	return ui_file;
}

gchar *
gtranslator_dirs_get_pixmaps_dir (void)
{
	gchar *datadir;
	gchar *pixmapsdir;
	
	datadir = gtranslator_dirs_get_gtranslator_data_dir ();
	pixmapsdir = g_build_filename (datadir,
				       "pixmaps",
				       NULL);
	g_free (datadir);
	
	return pixmapsdir;
}
