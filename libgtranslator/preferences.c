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
#endif

/**
* The init-function for libgtranslator.
**/
void gtranslator_config_init()
{
	#ifdef GCONF_IS_PRESENT
	client=gconf_client_get_default();
	gconf_client_add_dir(client,"/apps/gtranslator/",
		GCONF_CLIENT_PRELOAD_NONE,error);
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
	gconf_synchronous_sync(client->conf, error);
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
	#ifdef GCONF_IS_PRESENT
	return (gconf_client_get_string(client, path, error));
	#else
	return (gnome_config_get_string(path));
	#endif
}

void gtranslator_config_set_string(gchar *path, gchar *value)
{
	#ifdef GCONF_IS_PRESENT
	gconf_client_set_string(client, path, value, error);
	#else
	gnome_config_set_string(path, value);
	#endif
}

/**
* The integer methods ...
**/
gint gtranslator_config_get_int(gchar *path)
{
	#ifdef GCONF_IS_PRESENT
	return (gconf_client_get_int(client, path, error));
	#else
	return (gnome_config_get_int(path));
	#endif
}

void gtranslator_config_set_int(gchar *path, gint value)
{
	#ifdef GCONF_IS_PRESENT
	gconf_client_set_int(client, path, value, error);
	#else
	gnome_config_set_int(path, value);
	#endif
}

/**
* ... and finally the boolean functions ...
**/
gboolean gtranslator_config_get_bool(gchar *path)
{
	#ifdef GCONF_IS_PRESENT
	return (gconf_client_get_bool(client, path, error));
	#else
	return (gnome_config_get_bool(path));
	#endif
}

void gtranslator_config_set_bool(gchar *path, gboolean value)
{
	#ifdef GCONF_IS_PRESENT
	gconf_client_set_bool(client, path, value, error);
	#else
	gnome_config_set_bool(path, value);
	#endif
}
