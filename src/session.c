/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#include "gui.h"
#include "message.h"
#include "parse.h"
#include "preferences.h"
#include "session.h"

#include <gtk/gtkmain.h>
#include <libgnomeui/gnome-appbar.h>

/*
 * Quits via the normal quit.
 */
gint gtranslator_session_die(GnomeClient * client, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

/*
 * Saves the state of gtranslator before quitting.
 */
gint gtranslator_session_sleep(GnomeClient * client, gint phase,
		       GnomeSaveStyle s_style, gint shutdown,
		       GnomeInteractStyle i_style, gint fast, gpointer data)
{
	gchar *argv[] = {
		"rm",
		"-r",
		NULL
	};
	
	/*
	 * The state (for now only the current message number) is stored
	 *  in the preferences.
	 */
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

/*
 * Restores a previously closed session.
 */ 
void gtranslator_session_restore(GnomeClient * client)
{
	guint num;

	gtranslator_config_init();
	num = gtranslator_config_get_int("state/message_number");
	gtranslator_config_close();

	gtranslator_message_go_to_no(NULL, GUINT_TO_POINTER(num));
	g_snprintf(status, sizeof(status), _("Session restored successfully."));
	gnome_appbar_set_status(GNOME_APPBAR(gtranslator_application_bar), status);
}

