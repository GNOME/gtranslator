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

#include "../../comment.h"
#include "../../header_stuff.h"
#include "../../message.h"
#include "../../nautilus-string.h"
#include "../../parse.h"
#include "../../prefs.h"
#include "../../utils.h"

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
	FILE 		*text_file;
	FILE		*po_file;

	GtrHeader	*header;
	
	gchar 	 	*line;
	gchar 		*string;
	gchar		*tempfilename;

	gint		 lines=0;

	g_return_val_if_fail(filename!=NULL, FALSE);
	tempfilename=string=line=NULL;

	/*
	 * Open up the text file for pure reading, parsing in.
	 */
	text_file=fopen(filename, "r");
	g_return_val_if_fail(text_file!=NULL, FALSE);

	/*
	 * Get a sane header from the preferences values for the translator.
	 */
	header=gtranslator_header_create_from_prefs();

	/*
	 * Open the resulting temporary file for gtranslator for writing.
	 */
	tempfilename=gtranslator_utils_get_temp_file_name(); 
	po_file=fopen(tempfilename, "w");
	g_return_val_if_fail(po_file!=NULL, FALSE);

	fprintf(po_file, "\
# gtranslator converted text file \"%s\".\n\
# %s <%s>.\n\
#\n\
msgid \"\"\n\
msgstr \"\"\n\
\"Project-Id-Version: %s\\n\"\n\
\"POT-Creation-Date: %s\\n\"\n\
\"PO-Revision-Date: %s\\n\"\n\
\"Last-Translator: %s <%s>\\n\"\n\
\"Language-Team: %s <%s>\\n\"\n\
\"MIME-Version: %s\\n\"\n\
\"Content-Type: text/plain; charset=%s\\n\"\n\
\"Content-Transfer-Encoding: %s\\n\"\n\n",
		filename, author, email,
		filename,
		header->pot_date, header->po_date,
		header->translator, header->tr_email,
		language, lg, header->mime_version,
		header->charset, header->encoding);

	while((line = gtranslator_utils_getline(text_file)) !=NULL)
	{

		lines++;
		
		/*
		 * Every newline should separate a "message".
		 */
		g_strchomp(line);
		
		if((!line[0] || line[0]=='\n') && string)
		{
			fprintf(po_file, "#: %s:%i\nmsgid \"%s\"\nmsgstr \"\"\n\n", filename, lines, string);

			GTR_FREE(string);
		}
		else
		{
			string=g_strdup(line);
		}
	}

	/*
	 * Close the used file streams.
	 */
	fclose(text_file);
	fclose(po_file);

	/*
	 * Now open the temporary filename.
	 */
	gtranslator_parse_main(tempfilename);
	GTR_FREE(tempfilename);
	
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
	gtranslator_message_for_each(po->messages, 
		(FEFunc) write_msg, (FILE *)fs);

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
	gtranslator_message_for_each(po->messages, 
		(FEFunc) write_msg, (FILE *)fs);
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
