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
#include "nautilus-string.h"
#include "parse.h"
#include "utils.h"

#include <libgnome/gnome-util.h>

#include <gal/util/e-util.h>

/*
 * The internally used variables for learning.
 */
static gboolean 	init_status=FALSE;
static GList		*learn_buffer=NULL;

/*
 * A maximum number of read entries can be specified here; unlimited numbers
 *  could eventually cause too much time-loss.
 */
#define MAX_LEARN_ENTRIES 1024

/*
 * Initialize our internal learn buffers .-)
 */
void gtransator_learn_init()
{
	gchar 	*learn_base_directory;
	gchar	*learn_base_file;
	
	g_return_if_fail(init_status==FALSE);

	/*
	 * Build the standard "learned-strings" directory in the user's
	 *  home directory.
	 */
	learn_base_directory=g_strdup_printf("%s/.gtranslator/", 
		g_get_home_dir());

	if(!g_file_test(learn_base_directory, G_FILE_TEST_ISDIR))
	{
		/*
		 * As e_mkdir_hier returns 0 on success, any other value must 
		 *  be an error.
		 */
		if(e_mkdir_hier(learn_base_directory, 0644))
		{
			return;
		}
	}
	else
	{
		learn_base_file=g_strconcat(learn_base_directory, 
			"learned-strings", NULL);

		/*
		 * If a current "learned-strings" file is present, parse it
		 *  and set up our "learn_buffer" list.
		 */
		if(g_file_test(learn_base_file, G_FILE_TEST_ISFILE))
		{
			gchar 	*content;
			gchar 	**entries;
			gint 	i=0;

			/*
			 * Read the file and split the fields (here: lines)
			 *  per "g_strsplit"; there's a maximal limit to avoid
			 *   parsing of too big "learned-strings" files.
			 */
			content=e_read_file(learn_base_file);
			entries=g_strsplit(content, "\n", MAX_LEARN_ENTRIES);

			while(entries[i]!=NULL && i <= MAX_LEARN_ENTRIES)
			{
				gchar *e=g_strstrip(entries[i]);
				
				/*
				 * The '#' should mark a comment line, so
				 *  we don't need to parse these lines.
				 */
				if(e[0]!='#')
				{
					learn_buffer=g_list_prepend(
						learn_buffer, g_strdup(e));
				}
				
				i++;
				g_free(e);
			}

			g_strfreev(entries);
			g_free(content);
		}
		else
		{
			learn_buffer=NULL;
		}

		/*
		 * Reverse and sort the entries list.
		 */
		if(learn_buffer)
		{
			learn_buffer=g_list_reverse(learn_buffer);
			learn_buffer=g_list_sort(learn_buffer, (GCompareFunc)
				nautilus_strcmp);
		}

		init_status=TRUE;
	}
}

/*
 * Return our internal state.
 */
gboolean gtranslator_learn_initialized()
{
	return init_status;
}

/*
 * Clean up our learn space -- we're empty afterwards.
 */
void gtranslator_learn_shutdown()
{
	GString 	*filestring=g_string_new("# gtranslator learned strings buffer");
	gchar 		*filename;
	
	g_return_if_fail(init_status==TRUE);

	while(learn_buffer!=NULL)
	{
		filestring=g_string_append_c(filestring, '\n');
		filestring=g_string_append(filestring, (learn_buffer->data));

		g_free(learn_buffer->data);
		learn_buffer=learn_buffer->next;
	}

	g_list_free(learn_buffer);

	g_return_if_fail(filestring->str!=NULL);
	filename=g_strdup_printf("%s/gtranslator/learned-strings", 
		g_get_home_dir());

	e_write_file(filename, filestring->str, 0644);

	g_string_free(filestring, FALSE);
	g_free(filename);
}

/*
 * Add it to our learned list!
 */
void gtranslator_learn_string(const gchar *string)
{
	gchar 	*learn_string;
	
	g_return_if_fail(string!=NULL);

	learn_string=g_strstrip(g_strdup(string));

	if(!gtranslator_learn_learned(learn_string))
	{
		learn_buffer=g_list_append(learn_buffer,
			g_strdup(learn_string));
	}

	g_free(learn_string);
}

/*
 * Returns the learn status.
 */
gboolean gtranslator_learn_learned(const gchar *string)
{
	gchar 	*result;
	gchar 	*search_string=g_strdup(string);
	
	g_return_val_if_fail(string!=NULL, FALSE);

	result=(gchar *) g_list_find(learn_buffer, search_string);
	g_free(search_string);

	if(!result)
	{
		return FALSE;
	}
	else
	{
		g_free(result);
		return TRUE;
	}
}
