/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The session-functions of gtranslator
*
* -- source
**/
#include "session.h"

/**
* The standard die-method
**/
gint gtranslator_dies_for_you(GnomeClient *client,gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

/**
* The SaveYourself-function
**/
gint gtranslator_sleep(GnomeClient *client, gint phase, GnomeSaveStyle s_style,
        gint shutdown, GnomeInteractStyle i_style,gint fast,gpointer data)
{
	gchar *path;
	gchar *argv[] = {
		"rm",
		"-r",
		NULL
	};
	path=gnome_client_get_config_prefix(client);
	/**
	* State-saving ...
	**/
	gnome_config_push_prefix(path);
	gnome_config_pop_prefix();
	/**
	* Simply sync the config-parameters again
	**/
	gnome_config_sync();
	/**
	* Get real path
	**/
	argv[2]=gnome_config_get_real_path(path);
	/**
	* Bind to Sessionmanagement-functions
	**/
	gnome_client_set_discard_command(client, 3, argv);
	argv[0]=(gchar *)data;
	/**
	* Bind to the clone/restart commands
	**/
	gnome_client_set_clone_command(client, 1, argv);
	gnome_client_set_restart_command(client, 1, argv);
	return TRUE;
}
