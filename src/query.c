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

#include "actions.h"
#include "gui.h"
#include "learn.h"
#include "message.h"
#include "messages-table.h"
#include "nautilus-string.h"
#include "prefs.h"
#include "query.h"
#include "translator.h"
#include "utils.h"

#include <libgnome/gnome-i18n.h>
#include <libgnome/gnome-util.h>

#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <dirent.h>

/*
 * Get all empty msgstr's filled with found translation from the learn buffer.
 */
void gtranslator_query_gtr_msg(gpointer data, gpointer yeah)
{
	GtrMsg *msg=GTR_MSG(data);

	if(msg && msg->msgid && !msg->msgstr)
	{
		gchar	*result;
		
		result=gtranslator_learn_get_learned_string(msg->msgid);
		
		if(result)
		{
			/*
			 * Set the translation content, status etc. from the learn buffer
			 *  query result .-)
			 */
			msg->msgstr=g_strdup(result);
			msg->status |= GTR_MSG_STATUS_TRANSLATED;
			po->file_changed=TRUE;
			
			GTR_FREE(result);
		}
	}
}

/*
 * Simply execute the gtranslator_query_gtr_msg for every message in the
 *  po file.
 */
void gtranslator_query_translate(gboolean gui)
{
	g_return_if_fail(po!=NULL);
	g_return_if_fail(po->messages!=NULL);
	
	g_list_foreach(po->messages, (GFunc) gtranslator_query_gtr_msg,
		NULL);

	/*
	 * Activate the Save menu/tollbar items on changes.
	 */
	if(po->file_changed && gui)
	{
		gtranslator_actions_enable(ACT_SAVE);
		
		gtranslator_messages_table_clear();
		gtranslator_messages_table_create();

		gtranslator_get_translated_count();
	}
	
	if(gui)
	{
		gtranslator_message_show(po->current);
	}
}