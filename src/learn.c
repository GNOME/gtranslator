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

/*
 * The internally used variables for learning.
 */
static gboolean 	init_status=FALSE;
static GList		*learn_buffer=NULL;
static GCompletion	*completion_buffer=NULL;

/*
 * A maximum number of read entries can be specified here; unlimited numbers
 *  could eventually cause too much time-loss.
 */
#define MAX_LEARN_ENTRIES 2048

/*
 * Initialize our internal learn buffers .-)
 */
void gtranslator_learn_init()
{
	gchar	*learn_base_file;
	
	g_return_if_fail(init_status==FALSE);

	learn_base_file=g_strdup_printf("%s/.gtranslator/learned-strings", 
		g_get_home_dir());

	/*
	 * If a current "learned-strings" file is present, parse it
	 *  and set up our "learn_buffer" list.
	 */
	if(g_file_test(learn_base_file, G_FILE_TEST_ISFILE))
	{
		gchar 	*content;
		FILE 	*l_file;
		gint	counter=0;

		l_file=fopen(learn_base_file, "r");

		while((content=gtranslator_utils_getline(l_file))!=NULL &&
			counter <= MAX_LEARN_ENTRIES)
		{
			counter++;
			
			/*
			 * Ignore empty lines and lines starting with '#'.
			 */
			if(content && content[0]!='\0' && content[0]!='#')
			{
				learn_buffer=g_list_prepend(learn_buffer,
					g_strdup(content));
			}
		}

		fclose(l_file);
	}
	else
	{
		learn_buffer=NULL;
	}

	/*
	 * If there's no list yet, setup a minimal list containing one item:
	 *  "GNOME"; also reverse and sort the list afterwards.
	 */
	if(!learn_buffer)
	{
		learn_buffer=g_list_prepend(learn_buffer, "GNOME");
	}
	
	learn_buffer=g_list_reverse(learn_buffer);
	learn_buffer=g_list_sort(learn_buffer, (GCompareFunc) nautilus_strcmp);

	/*
	 * Setup the completion buffer with our read-in strings.
	 */
	completion_buffer=g_completion_new(NULL);
	g_completion_add_items(completion_buffer, learn_buffer);
	
	init_status=TRUE;
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
	gchar 		*filename;
	FILE		*l_file;
	gint		counter=0;
	
	g_return_if_fail(init_status==TRUE);

	filename=g_strdup_printf("%s/.gtranslator/learned-strings",
		g_get_home_dir());
	
	/*
	 * Open up the learned-strings file for writing the entries into it.
	 */
	l_file=fopen(filename, "w");
	g_return_if_fail(l_file!=NULL);

	/*
	 * A file identification string -- should be useful for mime-types
	 *  "magic".
	 */
	fprintf(l_file, "# gtranslator learned strings file\n");

	/*
	 * Don't write any further if there isn't any learned strings buffer.
	 */
	if(!learn_buffer)
	{
		fclose(l_file);
		g_free(filename);
		
		return;
	}

	/*
	 * Clean up our completion buffer.
	 */
	if(completion_buffer)
	{
		g_completion_clear_items(completion_buffer);
		g_completion_free(completion_buffer);
	}

	/*
	 * Clean up our internal learn buffer list.
	 */
	while(learn_buffer!=NULL && counter <= MAX_LEARN_ENTRIES)
	{
		counter++;
		
		fprintf(l_file, "%s\n", (gchar *)learn_buffer->data);

		g_free(learn_buffer->data);
		learn_buffer=learn_buffer->next;
	}

	/*
	 * Free all the used variables.
	 */
	g_list_free(learn_buffer);

	fclose(l_file);
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

/*
 * The "query" function which returns the string found in the learned_buffer;
 *  or NULL if nothing was found.
 */
gchar *gtranslator_learn_get_learned_string(const gchar *search_string)
{
	gchar	*found_string;
	
	g_return_val_if_fail(search_string!=NULL, NULL);

	g_completion_complete(completion_buffer, (gchar *)search_string, 
		&found_string);

	/*
	 * Return NULL if no strings could be completed.
	 */
	if(!found_string)
	{
		return NULL;
	}

	return g_strdup(found_string);
	g_free(found_string);
}
