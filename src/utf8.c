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

#include "utf8.h"
#include "utils.h"

#include <locale.h>

#include <gal/widgets/e-unicode.h>

/*
 * Internal capsulating function for converting to/from UTF-8.
 */
void convert_all_messages(GList *list, GFreeFunc function);

/*
 * Convert the given GtrMsg to UTF-8.
 */
void gtranslator_utf8_convert_message_to_utf8(GtrMsg **msg)
{
	gchar *msgstr;

	g_return_if_fail(*msg!=NULL);
	msgstr=g_strdup(GTR_MSG(*msg)->msgstr);

	if(msgstr)
	{
		GTR_MSG(*msg)->msgstr=e_utf8_from_locale_string(msgstr);
	}

	g_free(msgstr);
}

/*
 * Convert the given GtrMsg from UTF-8 to a "normal" string.
 */
void gtranslator_utf8_convert_message_from_utf8(GtrMsg **msg)
{
	gchar *msgstr;

	g_return_if_fail(*msg!=NULL);
	msgstr=g_strdup(GTR_MSG(*msg)->msgstr);

	if(msgstr)
	{
		GTR_MSG(*msg)->msgstr=e_utf8_to_locale_string(msgstr);
	}

	g_free(msgstr);
}

/*
 * Convert all messages completely to UTF-8.
 */
void gtranslator_utf8_convert_po_to_utf8(void)
{
	gchar *old_env;
	
	if(!file_opened)
	{
		return;
	}

	old_env=setlocale(LC_ALL, "");
	
	setlocale(LC_ALL, 
		gtranslator_utils_get_locale_name());
	
	convert_all_messages(po->messages, 
		(GFreeFunc) gtranslator_utf8_convert_message_to_utf8);

	/*
	 * Set the po file charset to UTF-8 as we did convert it now.
	 */
	po->header->charset="UTF-8";

	setlocale(LC_ALL, old_env);
	g_free(old_env);
}

/*
 * Convert all messages completely from UTF-8.
 */
void gtranslator_utf8_convert_po_from_utf8(void)
{
	gchar *old_env;
	
	if(!file_opened)
	{
		return;
	}

	old_env=setlocale(LC_ALL, "");
	setlocale(LC_ALL, 
		gtranslator_utils_get_locale_name());

	convert_all_messages(po->messages,
		(GFreeFunc) gtranslator_utf8_convert_message_from_utf8);

	setlocale(LC_ALL, old_env);
	g_free(old_env);
}
