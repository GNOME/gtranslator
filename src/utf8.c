/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
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

#include "gui.h"
#include "utf8.h"
#include "utils.h"

#include <locale.h>

#include <libgnome/gnome-i18n.h>
#include <gal/widgets/e-unicode.h>

/*
 * The static old environment variable.
 */
static gchar	*old_environment;

/*
 * Prototypes:
 */
void get_old_environment(void);
void set_old_environment(void);
void set_new_environment(void);

/*
 * Get/set up the old environment.
 */
void get_old_environment()
{
	old_environment=setlocale(LC_ALL, "");
	g_return_if_fail(old_environment!=NULL);
}

void set_old_environment()
{
	g_return_if_fail(old_environment!=NULL);
	setlocale(LC_ALL, old_environment);
}

/*
 * Set up the new environment, like desired by the functions .-)
 */
void set_new_environment()
{
	gchar	*localename;

	localename=gtranslator_utils_get_locale_name();
	g_return_if_fail(localename!=NULL);

	/*
	 * Get the full-length language name to satisfy our needs.
	 */
	localename=gtranslator_utils_get_full_language_name(localename);
	setlocale(LC_ALL, localename);
}

/*
 * Return whether we do have got a UTF-8 file or not.
 */
gboolean gtranslator_utf8_po_file_is_utf8()
{
	g_return_val_if_fail(po->header->charset!=NULL, FALSE);

	if(!g_strcasecmp(po->header->charset, "UTF-8"))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * Convert the given GtrMsg to UTF-8.
 */
void gtranslator_utf8_convert_message_to_utf8(GtrMsg *msg)
{
	gchar *msgstr;

	g_return_if_fail(msg!=NULL);

	msgstr=(msg)->msgstr;
	(msg)->msgstr=e_utf8_from_locale_string(msgstr);
	g_free(msgstr);
}

/*
 * Convert the given GtrMsg from UTF-8 to a "normal" string.
 */
void gtranslator_utf8_convert_message_from_utf8(GtrMsg *msg)
{
	gchar *msgstr;

	g_return_if_fail(msg!=NULL);

	msgstr=(msg)->msgstr;
	(msg)->msgstr=e_utf8_to_locale_string(msgstr);
	g_free(msgstr);
}

/*
 * Convert all messages completely to UTF-8.
 */
void gtranslator_utf8_convert_po_to_utf8(void)
{
	get_old_environment();
	set_new_environment();

	g_list_foreach(po->messages, 
		(GFunc) gtranslator_utf8_convert_message_to_utf8, NULL);
		
	/*
	 * Set the po file charset to UTF-8 as we did convert it now.
	 */
	g_free(po->header->charset); 
	po->header->charset=g_strdup("UTF-8");

	set_old_environment();
}

/*
 * Convert all messages completely from UTF-8.
 */
void gtranslator_utf8_convert_po_from_utf8(void)
{
	gchar *charset;

	get_old_environment();
	set_new_environment();

	charset=gtranslator_utils_get_locale_charset();

	g_list_foreach(po->messages,
		(GFunc) gtranslator_utf8_convert_message_from_utf8, NULL);

	/*
	 * Assign the converted charset value.
	 */
	if(charset)
	{
		g_free(po->header->charset);
		po->header->charset=g_strdup(charset);
		g_free(charset);
	}

	set_old_environment();
}

/*
 * Set the content of the message right from the text boxes.
 */
void gtranslator_utf8_get_utf8_for_current_message()
{
	gchar *msgstr;
	GtrMsg *current_message;
	
	g_return_if_fail(po->current!=NULL);
	current_message=GTR_MSG(po->current);
	
	get_old_environment();
	set_new_environment();

	msgstr=e_utf8_gtk_editable_get_text(GTK_EDITABLE(trans_box));
	g_return_if_fail(msgstr!=NULL);

	current_message->msgstr=g_strdup(msgstr);
	g_free(msgstr);

	set_old_environment();
}

/*
 * Return the plain string from the given UTF-8 string.
 */
gchar *gtranslator_utf8_get_plain_string(gchar **string)
{
	gchar *plain_string;
	
	g_return_val_if_fail(*string!=NULL, NULL);

	get_old_environment();
	set_new_environment();

	plain_string=e_utf8_to_locale_string(*string);

	set_old_environment();

	return plain_string;
}

/*
 * Return a pure UTF-8 string back.
 */
gchar *gtranslator_utf8_get_utf8_string(gchar **string)
{
	gchar *utf8_string;

	g_return_val_if_fail(*string!=NULL, NULL);

	get_old_environment();
	set_new_environment();

	utf8_string=e_utf8_from_locale_string(*string);

	set_old_environment();

	return utf8_string;
}

/*
 * Return the plain msgstr (uses the function above).
 */
gchar *gtranslator_utf8_get_plain_msgstr(GtrMsg **message)
{
	g_return_val_if_fail(GTR_MSG(*message)->msgstr!=NULL, NULL);

	return (gtranslator_utf8_get_plain_string(&GTR_MSG(*message)->msgstr));
}
