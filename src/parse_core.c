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

#include "parse.h"

#include <libgnome/gnome-i18n.h>

/*
 * The core parsing function for the given po file.
 */ 
void parse_core(const gchar *filename)
{
	if (!filename)
	{
		g_warning(_("There's no file to open!"));
			return;
	}
	else
	{
		gchar *base = g_basename(filename);

		if (base[0] == '\0')
		{
			g_warning(_("There's no file to open!"));
				return;
		}
	}

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
	if(gtranslator_check_file_perms(po)==FALSE)
	{
		return;
	}
	
	if (!actual_parse())
	{
		free_po();
		return;
	}
	
	/*
	 * If the first message is header (it always should be)
	 */
	po->header = get_header(GTR_MSG(po->messages->data));
	if (po->header)
	{
		GList *header_li;
		/*
		 * Unlink it from messages list
		 */
		header_li = po->messages;
		po->messages = g_list_remove_link(po->messages, header_li);
		free_a_message(header_li->data, NULL);
		g_list_free_1(header_li);
	}
	else
	{
		g_warning(_("The file has no header!"));
	}
	
	file_opened = TRUE;
	po->file_changed = FALSE;
	po->length = g_list_length(po->messages);
	
	/*
	 * Set the current message to the first message.
	 */
	po->current = g_list_first(po->messages);
}
