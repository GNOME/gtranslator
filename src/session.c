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
#include <libgtranslator/preferences.h>

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
	gchar *argv[] = {
		"rm",
		"-r",
		NULL
	};
	/**
	* State-saving ...
	**/
	gtranslator_config_init();
	gtranslator_config_set_int("state/message_number", 
			     g_list_position(po->messages, po->current));
	gtranslator_config_close();
	
	
	argv[2] = NULL;

	gnome_client_set_discard_command(client, 3, argv);

	argv[0] = (gchar *) data;
	argv[1] = po->filename;

	gnome_client_set_restart_command(client, 2, argv);

	return TRUE;
}

void restore_session(GnomeClient * client)
{
	guint num;

	gtranslator_config_init();
	num = gtranslator_config_get_int("state/message_number");
	gtranslator_config_close();

	goto_nth_msg(NULL, GUINT_TO_POINTER(num));
	g_snprintf(status, sizeof(status), _("Session restored successfully."));
	gnome_appbar_set_status(GNOME_APPBAR(appbar1), status);
}

