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

/*
 * The plain text file backend -- translating README's is now possible.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../../header_stuff.h"
#include "../../nautilus-string.h"
#include "../../parse.h"

/*
 * Opens the given filename via it's own stuff.
 */
gboolean backend_open(const gchar *filename)
{
	FILE *f;
	gchar line[256];
	gchar *str=NULL;
	gchar *e;
	GtrHeader *h;
	
	h=gtranslator_header_create_from_prefs();
	h->prj_name=g_strdup(filename);
	h->prj_version="0.1";

	f=fopen(filename, "r");

	while(fgets(line, sizeof(line), f))
	{
		if(!g_strstrip(line))
		{
			GtrMsg *msg=g_new0(GtrMsg, 1);

			msg->msgid=g_strdup(str);
			msg->msgstr="";
			msg->comment="";

			g_free(str);

			po->messages=g_list_append(po->messages, msg);
		}
		else
		{
			e=str;
			str=g_strdup_printf("%s\n%s", e, line);

			g_free(e);
		}
	}
	
	return TRUE;
}

/*
 * The save "callback".
 */
gboolean backend_save(void)
{
	g_message("Write po -> text save functions..");
	return TRUE;
}

/*
 * Save to a different file.
 */
gboolean backend_save_as(const gchar *filename)
{
	g_message("Save to %s", filename);
	return TRUE;
}

/*
 * Should return TRUE/FALSE to enable/disable the Compile menu entries/toolbar
 *  icons.
 */
gboolean backend_is_compilable(void)
{
	return FALSE;
}

/*
 * Cleanup functions should be here.
 */
void backend_close(void)
{
	gtranslator_po_free();
}
