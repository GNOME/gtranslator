/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *		Gediminas Paulauskas <menesis@gtranslator.org>
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

#include <limits.h>
#include <stdlib.h>
#include <sys/param.h>

#include "message.h"
#include "parse.h"
#include "utf8.h"
#include "utils.h"
#include "utils_gui.h"

#include <libgnome/gnome-i18n.h>

/*
 * The core parsing function for the given po file.
 */ 
void gtranslator_parse(const gchar *filename)
{
	gboolean first_is_fuzzy;
	gchar *base;

	g_return_if_fail(filename!=NULL);

	base=g_basename(filename);
	g_return_if_fail(base[0]!='\0');

	po = g_new0(GtrPo, 1);
	/*
	 * Get absolute filename.
	 */
	if (!g_path_is_absolute(filename)) 
	{
		char absol[MAXPATHLEN + 1];
		realpath(filename, absol);
		po->filename = g_strdup(absol);
	}
	else
	{
		po->filename = g_strdup(filename);
	}
	
	/*
	 * Check the right file access permissions.
	 */
	if(gtranslator_utils_check_file_permissions(po)==FALSE)
	{
		return;
	}
	
	if (!gtranslator_parse_core())
	{
		gtranslator_po_free();
		return;
	}

#define FIRST_MSG GTR_MSG(po->messages->data)
	first_is_fuzzy=(FIRST_MSG->status & GTR_MSG_STATUS_FUZZY) != 0;
	gtranslator_message_status_set_fuzzy(FIRST_MSG, FALSE);
	
	/*
	 * If the first message is header (it always should be)
	 */
	po->header = gtranslator_header_get(FIRST_MSG);

	if (po->header)
	{
		GList *header_li;
		
		/*
		 * Unlink it from messages list
		 */
		header_li = po->messages;
		po->messages = g_list_remove_link(po->messages, header_li);
		gtranslator_message_free(header_li->data, NULL);
		g_list_free_1(header_li);
	}
	else
	{
		gtranslator_message_status_set_fuzzy(FIRST_MSG, first_is_fuzzy);
	}
	
	file_opened = TRUE;
	po->file_changed = FALSE;
	po->length = g_list_length(po->messages);

	/*
	 * Set the utf8 field of the GtrPo to TRUE if we are editing an UTF-8 
	 *  encoded file.
	 */
	if(gtranslator_utf8_po_file_is_utf8())
	{
		po->utf8=TRUE;
		po->locale_charset=gtranslator_utils_get_locale_charset();
	}
	else
	{
		po->utf8=FALSE;
		po->locale_charset=NULL;
	}

	/*
	 * Set the current message to the first message.
	 */
	po->current = g_list_first(po->messages);
}
