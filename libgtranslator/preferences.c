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
* The init-function for libgtranslator.
**/
void gtranslator_config_init()
{
	#ifdef GCONF_IS_PRESENT
	GConfClient 	*client;
	GError		*error;
	client=gconf_client_get_default();
	gconf_client_add_dir(client,"/gtranslator",
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
	#ifndef GCONF_IS_PRESENT	
	gnome_config_pop_prefix();
	gnome_config_sync();
	#endif
}
