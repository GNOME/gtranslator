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
	* If there's an error.
	**/
	if(error)
	{
		g_error(_("Error during Gconf initialization: `%s'!"),error->message);
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
	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("%s/%s","/apps/gtranslator",path);
	return (gconf_client_get_string(client, private_path, &error));
	#else
	return (gnome_config_get_string(path));
	#endif
}

/**
* And this method sets a string value in path.
**/
void gtranslator_config_set_string(gchar *path, gchar *value)
{
	if((!path) && value)
	{
		g_warning(_("Can't set the string `%s' for a non-defined path!\n"),value);
	}
	else
	{
		if(!value)
		{
			g_warning(_("No string defined for the path `%s'!"),path);
		}
		else
		{
			#ifdef GCONF_IS_PRESENT
			private_path=g_strdup_printf("%s/%s","/apps/gtranslator",path);
			gconf_client_set_string(client, private_path, value, &error);
			#else
			gnome_config_set_string(path, value);
			#endif
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
	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("%s/%s","/apps/gtranslator",path);
	return (gconf_client_get_int(client, private_path, &error));
	#else
	return (gnome_config_get_int(path));
	#endif
}

/**
* and 2) for setting integer values.
**/
void gtranslator_config_set_int(gchar *path, gint value)
{
	if((!path) && value)
	{
		g_warning(_("Can't set the value `%i' for a non-defined path!"),value);
	}
	else
	{
		if(!value)
		{
			g_warning(_("No value defined for the path `%s'!"),path);
		}
		else
		{
			#ifdef GCONF_IS_PRESENT
			private_path=g_strdup_printf("%s/%s","/apps/gtranslator",path);
			gconf_client_set_int(client, private_path, value, &error);
			#else
			gnome_config_set_int(path, value);
			#endif
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
	#ifdef GCONF_IS_PRESENT
	private_path=g_strdup_printf("%s/%s","/apps/gtranslator",path);
	return (gconf_client_get_bool(client, private_path, &error));
	#else
	return (gnome_config_get_bool(path));
	#endif
}

void gtranslator_config_set_bool(gchar *path, gboolean value)
{
	if((!path) && value)
	{
		g_warning(_("Can't set a boolean value for a non-defined path!"));
	}
	else
	{
		#ifdef GCONF_IS_PRESENT
		private_path=g_strdup_printf("%s/%s","/apps/gtranslator",path);
		gconf_client_set_bool(client, private_path, value, &error);
		#else
		gnome_config_set_bool(path, value);
		#endif
	}	
}

/**
* This returns the config-files location.
**/
gchar *gtranslator_config_get_absolute_path()
{
	#ifdef GCONF_IS_PRESENT
	g_warning("Hmm, I'd learn how to get gconf's file location :)");
		return "HEHE";
	#else
	return( g_strdup_printf("%s/%s",g_get_home_dir(),".gnome/gtranslator"));
	#endif
}
