/**
* Fatih Demir <kabalak@gmx.net>
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
gint gtranslator_dies_for_you(GnomeClient * client, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

/**
* The SaveYourself-function
**/
gint gtranslator_sleep(GnomeClient * client, gint phase,
		       GnomeSaveStyle s_style, gint shutdown,
		       GnomeInteractStyle i_style, gint fast, gpointer data)
{
	gchar *path;
	gchar *argv[] = {
		"rm",
		"-r",
		NULL
	};
	path = gnome_client_get_config_prefix(client);
	/**
	* State-saving ...
	**/
	gtranslator_config_init();
	gtranslator_config_set_int("State/Message number", 
			     g_list_position(po->messages, po->current));
	gtranslator_config_close();
	
	
	argv[2] = gnome_config_get_real_path(path);

	gnome_client_set_discard_command(client, 3, argv);

	argv[0] = (gchar *) data;
	argv[1] = po->filename;

	gnome_client_set_restart_command(client, 2, argv);

	return TRUE;
}

void restore_session(GnomeClient * client)
{
	guint num;
	gchar *prefix;

	prefix = gnome_client_get_config_prefix(client);
	gtranslator_config_init();
	num = gtranslator_config_get_int("State/Message number");
	gtranslator_config_close();

	goto_nth_msg(NULL, GUINT_TO_POINTER(num));
	g_snprintf(status, sizeof(status), _("Session restored successfully."));
	gnome_appbar_set_status(GNOME_APPBAR(appbar1), status);
}

