/**
*
* (C) 2000 Fatih Demir -- kabalak / kabalak@gmx.net
*
* This is distributed under the GNU GPL V 2.0 or higher which can be
*  found in the file COPYING for further studies.
*
* Enjoy this piece of software, brain-crack and other nice things.
*
* WARNING: Trying to decode the source-code may be hazardous for all your
*	future development in direction to better IQ-Test rankings!
*
* PSC: This has been completely written with vim; the best editor of all.
*
**/

#include <libgnome/libgnome.h>
#include <libgtranslator/preferences.h>

/**
* The globally used GConf client and error variable.
**/
#ifdef GCONF_IS_PRESENT
GConfClient	*client;
GError		*error;
gchar		*private_path=NULL;
#endif

/**
* The init-function for libgtranslator.
**/
void gtranslator_config_init(void)
{
	#ifdef GCONF_IS_PRESENT
	client=gconf_client_get_default();
	gconf_client_add_dir(client, "/apps/gtranslator",
		GCONF_CLIENT_PRELOAD_NONE, &error);
	/**
	* Print a more detailed error message.
	**/
	if(error)	
	{
		g_error(_("Error during GConf initialization through libgtranslator:\n%s"),error->message);
	}
	#else
	gnome_config_push_prefix("/gtranslator/");
	#endif
}

/**
* This function closes all configuration actions.
**/
void gtranslator_config_close(void)
{
	#ifdef GCONF_IS_PRESENT
	/**
	* Synchronize the preferences.
	**/
	gconf_client_suggest_sync(client, &error);
        /**
        * And remove the corresponding dir from the client.
        **/
        gconf_client_remove_dir(client, "/apps/gtranslator", &error);
	#else
	gnome_config_pop_prefix();
	gnome_config_sync();
	#endif
}

/**
* These function returns the path-element from the configuration.
**/
gchar *gtranslator_config_get_string(gchar *path)
{
	gchar *str;
	g_return_val_if_fail(path != 0, NULL);
	
	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("/apps/gtranslator/%s",path);
	str = gconf_client_get_string(client, private_path, &error);
	g_free(private_path);
	#else
	str = gnome_config_get_string(path);
	#endif
	return str;
}

/**
* And this method sets a string value in path.
**/
void gtranslator_config_set_string(gchar *path, gchar *value)
{
	g_return_if_fail(path != NULL);
	g_return_if_fail(value != NULL);

	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("/apps/gtranslator/%s", path);
	gconf_client_set_string(client, private_path, value, &error);
	g_free(private_path);
	#else
	gnome_config_set_string(path, value);
	#endif
}

/**
* The integer methods ...
*
* 1) For getting integer values ..
**/
gint gtranslator_config_get_int(gchar *path)
{
	gint i;
	g_return_val_if_fail(path != NULL, 1);

	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("/apps/gtranslator/%s",path);
	i = gconf_client_get_int(client, private_path, &error);
	g_free(private_path);
	#else
	i = gnome_config_get_int(path);
	#endif
	return i;
}

/**
* and 2) for setting integer values.
**/
void gtranslator_config_set_int(gchar *path, gint value)
{
	g_return_if_fail(path != NULL);

	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("/apps/gtranslator/%s", path);
	gconf_client_set_int(client, private_path, value, &error);
	g_free(private_path);
	#else
	gnome_config_set_int(path, value);
	#endif
}

/**
* ... and finally the boolean functions ...
**/
gboolean gtranslator_config_get_bool(gchar *path)
{
	gboolean b;
	g_return_val_if_fail(path != NULL, FALSE);

	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("/apps/gtranslator/%s",path);
	b = gconf_client_get_bool(client, private_path, &error);
	g_free(private_path);
	#else
	b = gnome_config_get_bool(path);
	#endif
	return b;
}

void gtranslator_config_set_bool(gchar *path, gboolean value)
{
	g_return_if_fail(path != NULL);

	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("/apps/gtranslator/%s", path);
	gconf_client_set_bool(client, private_path, value, &error);
	g_free(private_path);
	#else
	gnome_config_set_bool(path, value);
	#endif
}

/**
* This gets the gfloat value.
**/
gfloat gtranslator_config_get_float(gchar *path)
{
	gfloat f;
	g_return_val_if_fail(path != NULL, 0.0);

	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("/apps/gtranslator/%s", path);
	f = gconf_client_get_float(client,  private_path, &error);
	g_free(private_path);
	#else
	f = gnome_config_get_float(path);
	#endif
	return f;
}

/**
* The gfloat stuff.
**/
void gtranslator_config_set_float(gchar *path, gfloat value)
{
	g_return_if_fail(path != NULL);

	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("/apps/gtranslator/%s", path);
	gconf_client_set_float(client, private_path, value, &error);	
	g_free(private_path);
	#else
	gnome_config_set_float(path, value);
	#endif
}

/**
* This is the last time gtranslator has run.
**/
gchar *gtranslator_config_get_last_run_date()
{
	return gtranslator_config_get_string("informations/last_run_on");
}

/**
* This writes the last run date.
**/
void gtranslator_config_set_last_run_date(void)
{
	/**
	* Some time stuff.
	**/
	time_t present_time;
	struct tm *timebox;
	gchar date[30];
	/**
	* Get the current time.
	**/
	present_time=time(NULL);
	timebox=localtime(&present_time);
	/**
	* Set a date identifier; this ought to be also 
	*  localized!
	**/
	strftime(date, 30, _("%Y-%m-%d %H:%M"), timebox);
	/**
	* Test the date string before trying to store it.
	**/
	if(!date)
	{
		g_warning(_("Couldn't generate the current date!"));
	}
	else
	{
		/**
		* Store the date string.
		**/
		gtranslator_config_set_string("informations/last_run_on", date);
	}
}
