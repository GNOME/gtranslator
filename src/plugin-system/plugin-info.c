/*
 * plugin-info.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2002-2005 - Paolo Maggi 
 * Copyright (C) 2007 - Paolo Maggi, Steve Frécinaux
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
 
/*
 * Modified by the gtranslator Team, 2002-2007. See the AUTHORS file for a
 * list of people on the gtranslator Team.
 * See the ChangeLog files for a list of changes.
 *
 * $Id$
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <glib/gi18n.h>
#include <glib.h>

#include "plugin-info.h"
#include "plugin-info-priv.h"
#include "debug.h"
#include "plugin.h"

#ifdef ENABLE_PYTHON
#include "gtranslator-python-module.h"
#endif

void
_gtranslator_plugin_info_ref (GtranslatorPluginInfo *info)
{
	g_atomic_int_inc (&info->refcount);
}

static GtranslatorPluginInfo *
gtranslator_plugin_info_copy (GtranslatorPluginInfo *info)
{
	_gtranslator_plugin_info_ref (info);
	return info;
}

void
_gtranslator_plugin_info_unref (GtranslatorPluginInfo *info)
{
	if (!g_atomic_int_dec_and_test (&info->refcount))
		return;

	if (info->plugin != NULL)
	{
		DEBUG_PRINT ( "Unref plugin %s", info->name);

		g_object_unref (info->plugin);
		
		/* info->module must not be unref since it is not possible to finalize 
		 * a type module */
	}

	g_free (info->file);
	g_free (info->module_name);
	g_strfreev (info->dependencies);
	g_free (info->name);
	g_free (info->desc);
	g_free (info->icon_name);
	g_free (info->website);
	g_free (info->copyright);
	g_strfreev (info->authors);

	g_free (info);
}

/**
 * gtranslator_plugin_info_get_type:
 *
 * Retrieves the #GType object which is associated with the #GtranslatorPluginInfo
 * class.
 *
 * Return value: the GType associated with #GtranslatorPluginInfo.
 **/
GType
gtranslator_plugin_info_get_type (void)
{
	static GType the_type = 0;

	if (G_UNLIKELY (!the_type))
		the_type = g_boxed_type_register_static (
					"GtranslatorPluginInfo",
					(GBoxedCopyFunc) gtranslator_plugin_info_copy,
					(GBoxedFreeFunc) _gtranslator_plugin_info_unref);

	return the_type;
} 

/**
 * gtranslator_plugin_info_new:
 * @filename: the filename where to read the plugin information
 *
 * Creates a new #GtranslatorPluginInfo from a file on the disk.
 *
 * Return value: a newly created #GtranslatorPluginInfo.
 */
GtranslatorPluginInfo *
_gtranslator_plugin_info_new (const gchar *file)
{
	GtranslatorPluginInfo *info;
	GKeyFile *plugin_file = NULL;
	gchar *str;

	g_return_val_if_fail (file != NULL, NULL);

	DEBUG_PRINT ( "Loading plugin: %s", file);

	info = g_new0 (GtranslatorPluginInfo, 1);
	info->refcount = 1;
	info->file = g_strdup (file);

	plugin_file = g_key_file_new ();
	if (!g_key_file_load_from_file (plugin_file, file, G_KEY_FILE_NONE, NULL))
	{
		g_warning ("Bad plugin file: %s", file);
		goto error;
	}

	if (!g_key_file_has_key (plugin_file,
			   	 "Gtranslator Plugin",
				 "IAge",
				 NULL))
	{
		DEBUG_PRINT (
				     "IAge key does not exist in file: %s", file);
		goto error;
	}
	
	/* Check IAge=2 */
	if (g_key_file_get_integer (plugin_file,
				    "Gtranslator Plugin",
				    "IAge",
				    NULL) != 2)
	{
		DEBUG_PRINT (
				     "Wrong IAge in file: %s", file);
		goto error;
	}
				    
	/* Get module name */
	str = g_key_file_get_string (plugin_file,
				     "Gtranslator Plugin",
				     "Module",
				     NULL);

	if ((str != NULL) && (*str != '\0'))
	{
		info->module_name = str;
	}
	else
	{
		g_warning ("Could not find 'Module' in %s", file);
		goto error;
	}

	/* Get the dependency list */
	info->dependencies = g_key_file_get_string_list (plugin_file,
							 "Gtranslator Plugin",
							 "Depends",
							 NULL,
							 NULL);
	if (info->dependencies == NULL)
	{
		DEBUG_PRINT ( "Could not find 'Depends' in %s", file);
		info->dependencies = g_new0 (gchar *, 1);
	}

	/* Get the loader for this plugin */
	str = g_key_file_get_string (plugin_file,
				     "Gtranslator Plugin",
				     "Loader",
				     NULL);
	if (str && strcmp(str, "python") == 0)
	{
#ifndef ENABLE_PYTHON
		g_warning ("Cannot load Python plugin '%s' since gtranslator was not "
			   "compiled with Python support.", file);
		goto error;
#else
		info->module_type = GTR_TYPE_PYTHON_MODULE;
#endif
	}
	else
	{
		info->module_type = GTR_TYPE_MODULE;
	}
	g_free (str);

	/* Get Name */
	str = g_key_file_get_locale_string (plugin_file,
					    "Gtranslator Plugin",
					    "Name",
					    NULL, NULL);
	if (str)
		info->name = str;
	else
	{
		g_warning ("Could not find 'Name' in %s", file);
		goto error;
	}

	/* Get Description */
	str = g_key_file_get_locale_string (plugin_file,
					    "Gtranslator Plugin",
					    "Description",
					    NULL, NULL);
	if (str)
		info->desc = str;
	else
		DEBUG_PRINT ( "Could not find 'Description' in %s", file);

	/* Get Icon */
	str = g_key_file_get_locale_string (plugin_file,
					    "Gtranslator Plugin",
					    "Icon",
					    NULL, NULL);
	if (str)
		info->icon_name = str;
	else
		DEBUG_PRINT ( "Could not find 'Icon' in %s, using 'plugin'", file);
	

	/* Get Authors */
	info->authors = g_key_file_get_string_list (plugin_file,
						    "Gtranslator Plugin",
						    "Authors",
						    NULL,
						    NULL);
	if (info->authors == NULL)
		DEBUG_PRINT ( "Could not find 'Authors' in %s", file);


	/* Get Copyright */
	str = g_key_file_get_string (plugin_file,
				     "Gtranslator Plugin",
				     "Copyright",
				     NULL);
	if (str)
		info->copyright = str;
	else
		DEBUG_PRINT ( "Could not find 'Copyright' in %s", file);

	/* Get License */
	str = g_key_file_get_string (plugin_file,
				     "Gtranslator Plugin",
				     "License",
				     NULL);
	if (str)
		info->license = str;
	else
		DEBUG_PRINT ( "Could not find 'Copyright' in %s", file);

	/* Get Website */
	str = g_key_file_get_string (plugin_file,
				     "Gtranslator Plugin",
				     "Website",
				     NULL);
	if (str)
		info->website = str;
	else
		DEBUG_PRINT ( "Could not find 'Website' in %s", file);
		
	g_key_file_free (plugin_file);
	
	/* If we know nothing about the availability of the plugin,
	   set it as available */
	info->available = TRUE;
	
	return info;

error:
	g_free (info->file);
	g_free (info->module_name);
	g_free (info->name);
	g_free (info);
	g_key_file_free (plugin_file);

	return NULL;
}

gboolean
gtranslator_plugin_info_is_active (GtranslatorPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, FALSE);

	return info->available && info->active;
}

gboolean
gtranslator_plugin_info_is_available (GtranslatorPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, FALSE);

	return info->available != FALSE;
}

gboolean
gtranslator_plugin_info_is_configurable (GtranslatorPluginInfo *info)
{
	DEBUG_PRINT ( "Is '%s' configurable?", info->name);

	g_return_val_if_fail (info != NULL, FALSE);

	if (info->plugin == NULL || !info->active || !info->available)
		return FALSE;

	return gtranslator_plugin_is_configurable (info->plugin);
}

const gchar *
gtranslator_plugin_info_get_name (GtranslatorPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->name;
}

const gchar *
gtranslator_plugin_info_get_description (GtranslatorPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->desc;
}

const gchar *
gtranslator_plugin_info_get_icon_name (GtranslatorPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	/* use the plugin icon as a default if the plugin does not
	   have its own */
	if (info->icon_name != NULL && 
	    gtk_icon_theme_has_icon (gtk_icon_theme_get_default (),
				     info->icon_name))
		return info->icon_name;
	else
		return "gtranslator-plugin";
}

const gchar **
gtranslator_plugin_info_get_authors (GtranslatorPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, (const gchar **)NULL);

	return (const gchar **) info->authors;
}

const gchar *
gtranslator_plugin_info_get_website (GtranslatorPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->website;
}

const gchar *
gtranslator_plugin_info_get_copyright (GtranslatorPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->copyright;
}

const gchar *
gtranslator_plugin_info_get_license (GtranslatorPluginInfo *info)
{
        g_return_val_if_fail (info != NULL, NULL);
	
	return info->license;
}
