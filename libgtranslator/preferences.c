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

#include <preferences.h>

/**
* Cleanup the namespace. Remove the '_' defines for now ...
**/
#ifdef _
#undef _
#endif
#include <libgnome/libgnome.h>

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
void gtranslator_config_init()
{
	#ifdef GCONF_IS_PRESENT
	client=gconf_client_get_default();
	gconf_client_add_dir(client,"/apps/gtranslator",
		GCONF_CLIENT_PRELOAD_NONE, &error);
	/**
	* Print a more detailted error message.
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
void gtranslator_config_close()
{
	#ifdef GCONF_IS_PRESENT
	gconf_client_suggest_sync(client, &error);
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
	if(!path)
	{
		g_warning(_("No path defined where I could get the string from."));
		/**
		* you know the configure.in scrabble ;-)
		**/
		return "";
	}
	gtranslator_config_init();
	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("%s/%s","/apps/gtranslator",path);
	return (gconf_client_get_string(client, private_path, &error));
	#else
	return (gnome_config_get_string(path));
	#endif
	gtranslator_config_close();
}

/**
* And this method sets a string value in path.
**/
void gtranslator_config_set_string(gchar *path, gchar *value)
{
	if((!path) && value)
	{
		g_warning(_("Can't set the string `%s' for a non-defined path!\n"),
			value);
	}
	else
	{
		if(!value)
		{
			g_warning(_("No string defined for the path `%s'!"),
				path);
		}
		else
		{
			gtranslator_config_init();
			#ifdef GCONF_IS_PRESENT
			private_path=g_strdup_printf("%s/%s","/apps/gtranslator",
				path);
			gconf_client_set_string(client, private_path, value,
				&error);
			#else
			gnome_config_set_string(path, value);
			#endif
			gtranslator_config_close();
		}
	}	
}

/**
* The integer methods ...
*
* 1) For getting integer values ..
**/
gint gtranslator_config_get_int(gchar *path)
{
	if(!path)
	{
		g_warning(_("No path defined where I could get the integer from!"));
		g_warning(_("Returning `1' for assurance .."));
		return 1;
	}
	gtranslator_config_init();
	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("%s/%s","/apps/gtranslator"
		,path);
	return (gconf_client_get_int(client, private_path,
		&error));
	#else
	return (gnome_config_get_int(path));
	#endif
	gtranslator_config_close();
}

/**
* and 2) for setting integer values.
**/
void gtranslator_config_set_int(gchar *path, gint value)
{
	if((!path) && value)
	{
		g_warning(_("Can't set the value `%i' for a non-defined path!"),
			value);
	}
	else
	{
		if(!value)
		{
			g_warning(_("No value defined for the path `%s'!"),
				path);
		}
		else
		{
			gtranslator_config_init();
			#ifdef GCONF_IS_PRESENT
			private_path=g_strdup_printf("%s/%s","/apps/gtranslator",
				path);
			gconf_client_set_int(client, private_path, value,
				&error);
			#else
			gnome_config_set_int(path, value);
			#endif
			gtranslator_config_close();
		}	
	}
}

/**
* ... and finally the boolean functions ...
**/
gboolean gtranslator_config_get_bool(gchar *path)
{
	if(!path)
	{
		g_warning(_("No path defined where I could get the boolean from."));
		g_warning(_("Returning `FALSE' for assurance ..."));
		return FALSE;
	}
	gtranslator_config_init();
	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("%s/%s","/apps/gtranslator",path);
	return (gconf_client_get_bool(client, private_path,
		&error));
	#else
	return (gnome_config_get_bool(path));
	#endif
	gtranslator_config_close();
}

void gtranslator_config_set_bool(gchar *path, gboolean value)
{
	if((!path) && value)
	{
		g_warning(_("Can't set a boolean value for a non-defined path!"));
	}
	else
	{
		gtranslator_config_init();
		#ifdef GCONF_IS_PRESENT
		private_path=g_strdup_printf("%s/%s","/apps/gtranslator",
			path);
		gconf_client_set_bool(client, private_path, value,
			&error);
		#else
		gnome_config_set_bool(path, value);
		#endif
		gtranslator_config_close();
	}	
}

/**
* This is the last time gtranslator has run.
**/
gchar *gtranslator_config_get_last_run_date()
{
	gtranslator_config_init();
	#ifdef GCONF_IS_PRESENT
	return (gconf_client_get_string(client, "informations/last_run_on",
		&error));
	#else
	return (gnome_config_get_string("informations/last_run_on"));
	#endif
	gtranslator_config_close();
}

/**
* This writes the last run date.
**/
void gtranslator_config_set_last_run_date()
{
	/**
	* Some time stuff.
	**/
	time_t present_time;
	struct tm *timebox;
	gchar date[17];
	/**
	* Get the current time.
	**/
	present_time=time(NULL);
	timebox=localtime(&present_time);
	/**
	* Set a date identifier.
	**/
	strftime(date, 17, "%Y-%m-%d %H:%M", timebox);
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
		gtranslator_config_init();
		#ifdef GCONF_IS_PRESENT
		gconf_client_set_string(client, "/apps/gtranslator/informations/last_run_on",
			date, &error);
		#else
		gnome_config_set_string("informations/last_run_on", date);
		#endif
		gtranslator_config_close();
	}	
}
