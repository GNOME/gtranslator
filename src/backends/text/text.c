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
#include "../../message.h"
#include "../../nautilus-string.h"
#include "../../parse.h"
#include "../../prefs.h"

/*
 * The prototypes:
 */
void write_msg(GList *message, gpointer file);
gboolean backend_open(const gchar *filename);
gboolean backend_save(void);
gboolean backend_save_as(const gchar *filename);
void backend_close(void);

void write_msg(GList *message, gpointer file)
{
	if(message)
	{
		/*
		 * Write the current message to the output file stream.
		 */
		GtrMsg *msg=GTR_MSG(message->data);

		fprintf((FILE *) file, "%s", msg->msgstr);
	}
}

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

	if(!f)
	{
		/*
		 * If we couldn't open up the text file return FALSE.
		 */
		return FALSE;
	}

	while(fgets(line, sizeof(line), f))
	{
		/*
		 * Every newline should separate a "message".
		 */
		if(!g_strstrip(line))
		{
			GtrMsg *msg=g_new0(GtrMsg, 1);

			/*
			 * Construct a foo'sh message.
			 */
			msg->msgid=g_strdup(str);
			msg->msgstr="";
			msg->comment="";

			g_free(str);

			/*
			 * Set up our fake'sh messages list.
			 */
			po->messages=g_list_append(po->messages, msg);
		}
		else
		{
			/*
			 * Rescue and append all the lines till the next newline.
			 */
			e=str;
			str=g_strdup_printf("%s\n%s", e, line);

			g_free(e);
		}
	}

	fclose(f);
	
	return TRUE;
}

/*
 * The save "callback".
 */
gboolean backend_save(void)
{
	FILE *fs;
	
	if(!po)
	{
		return FALSE;
	}

	/*
	 * If we didn't have got a language code extended filename yet,
	 *  extend the current filename to be language code extended.
	 */
	if(!nautilus_istr_has_suffix(po->filename, lc))
	{
		gchar *oldname;
		
		oldname=g_strdup_printf("%s.%s", po->filename, lc);

		g_free(po->filename);
		po->filename=oldname;
	}

	/*
	 * Write all the "pairs" as plain text to the output file.
	 */
	fs=fopen(po->filename, "w");
	gtranslator_message_for_each(po->messages, (FEFunc) write_msg, (FILE *)fs);
	fclose(fs);

	return TRUE;
}

/*
 * Save to a different file.
 */
gboolean backend_save_as(const gchar *filename)
{
	FILE *fs;

	g_return_val_if_fail(filename!=NULL, FALSE);

	/*
	 * Save the strings to another specified output file.
	 */
	fs=fopen(filename, "w");
	gtranslator_message_for_each(po->messages, (FEFunc) write_msg, (FILE *)fs);
	fclose(fs);
	
	return TRUE;
}

/*
 * Cleanup functions should be here.
 */
void backend_close(void)
{
	gtranslator_po_free();
}
