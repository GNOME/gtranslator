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

#include "learn.h"
#include "parse.h"
#include "translation-memory.h"
#include "utils.h"

/*
 * Learn the given message specs and put'em into an eventual personal tm.
 */
void gtranslator_learn_message(GtrMsg *msg)
{
	gchar *id_informations;
	GList *foolist=NULL;
	
	g_return_if_fail(msg!=NULL);
	g_return_if_fail(msg->msgstr!=NULL);

	foolist=g_list_append(foolist, (gpointer) msg);

	id_informations=g_strdup_printf("%s/%s/%s/%s/%i", po->header->prj_name,
		po->header->language, po->header->prj_version,
		po->header->po_date,
		g_list_position(po->messages, foolist));

	if(gtranslator_tm_query_for_message(msg->msgid)==-1)
	{
		/*
		 * Add the translated message as a new entry to the current,
		 *  personal TM.
		 */
		gtranslator_tm_add(msg->msgid, msg->msgstr,
			gtranslator_utils_get_locale_name(), id_informations);
	}
	else
	{
		/*
		 * FIXME: Warning/error about it.
		 */
	}

	g_free(id_informations);
}
