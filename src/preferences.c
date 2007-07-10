/*
 * (C) 2000-2001 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
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

#include "preferences.h"
#include "utils.h"

#include <time.h>

#include <gconf/gconf-client.h>
#include <gdk/gdk.h>

//#include <libgnome/gnome-defs.h>
#include <glib/gi18n.h>

GConfClient	*client=NULL;
gchar		*private_path=NULL;

#define CONFIG_PREFIX "/apps/gtranslator/"

/*
 * Initializes the configuration access.
 */
void gtranslator_config_init(void)
{
	/* We need to do it only once */
	if(client==NULL)
	{
		client=gconf_client_get_default();
		/*
		 * add_dir is useful only for listeners...
		gconf_client_add_dir(client, CONFIG_PREFIX,
			GCONF_CLIENT_PRELOAD_NONE, NULL);
		 */
	}
}

/*
 * String methods:
 */
gchar *gtranslator_config_get_string(gchar *path)
{
	gchar *str;
	g_return_val_if_fail(path != 0, NULL);
	
	private_path=g_strconcat(CONFIG_PREFIX, path, NULL);
	str = gconf_client_get_string(client, private_path, NULL);
	g_free(private_path);
	return str;
}

void gtranslator_config_set_string(gchar *path, gchar *value)
{
	g_return_if_fail(path != NULL);
	g_return_if_fail(value != NULL);

	private_path=g_strconcat(CONFIG_PREFIX, path, NULL);
	gconf_client_set_string(client, private_path, value, NULL);
	g_free(private_path);
}

/*
 * Color methods:
 */
void
gtranslator_config_set_color(gchar *path, GdkColor *color)
{
	g_return_if_fail(path != NULL);
	g_return_if_fail(color != NULL);

	gchar *value;
	value = gdk_color_to_string(color);
	private_path=g_strconcat(CONFIG_PREFIX, path, NULL);
	gconf_client_set_string(client, private_path, value, NULL);
	g_free(private_path);
}


/*
 * Integer methods:
 */
gint gtranslator_config_get_int(gchar *path)
{
	gint i;
	g_return_val_if_fail(path != NULL, 1);

	private_path=g_strconcat(CONFIG_PREFIX, path, NULL);
	i = gconf_client_get_int(client, private_path, NULL);
	g_free(private_path);
	return i;
}

void gtranslator_config_set_int(gchar *path, gint value)
{
	g_return_if_fail(path != NULL);

	private_path=g_strconcat(CONFIG_PREFIX, path, NULL);
	gconf_client_set_int(client, private_path, value, NULL);
	g_free(private_path);
}

/*
 * Boolean methods:
 */
gboolean gtranslator_config_get_bool(gchar *path)
{
	gboolean b;
	g_return_val_if_fail(path != NULL, FALSE);

	private_path=g_strconcat(CONFIG_PREFIX, path, NULL);
	b = gconf_client_get_bool(client, private_path, NULL);
	g_free(private_path);
	return b;
}

void gtranslator_config_set_bool(gchar *path, gboolean value)
{
	g_return_if_fail(path != NULL);

	private_path=g_strconcat(CONFIG_PREFIX, path, NULL);
	gconf_client_set_bool(client, private_path, value, NULL);
	g_free(private_path);
}

/*
 * Gfloat methods:
 */
gfloat gtranslator_config_get_float(gchar *path)
{
	gfloat f;
	g_return_val_if_fail(path != NULL, 0.0);

	private_path=g_strconcat(CONFIG_PREFIX, path, NULL);
	f = gconf_client_get_float(client,  private_path, NULL);
	g_free(private_path);
	return f;
}

void gtranslator_config_set_float(gchar *path, gfloat value)
{
	g_return_if_fail(path != NULL);

	private_path=g_strconcat(CONFIG_PREFIX, path, NULL);
	gconf_client_set_float(client, private_path, value, NULL);
	g_free(private_path);
}

/*
 * Recalls the last date when gtranslator was run.
 */
gchar *gtranslator_config_get_last_run_date()
{
	return gtranslator_config_get_string("informations/last_run_on");
}

/*
 * Stores the current date as the last run date for gtranslator
 */
void gtranslator_config_set_last_run_date(void)
{
	time_t present_time;
	struct tm *timebox;
	gchar date[30];
	
	/*
	 * Get the current time.
	 */
	present_time=time(NULL);
	timebox=localtime(&present_time);
	
	strftime(date, 30, "%Y-%m-%d %H:%M", timebox);
	
	/*
	 * Check if we did get any date string.
	 */
	if(!date)
	{
		g_warning(_("Couldn't generate the current date!"));
	}
	else
	{
		/*
		 * Store the date as a string in the preferences.
		 */
		gtranslator_config_set_string("informations/last_run_on", date);
	}
}
