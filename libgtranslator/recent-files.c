/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
 *
 * libgtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * libgtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "recent-files.h"

/*
 * An utility function; checks if the given file does really exist
 *  and is not (!) empty.
 */
gboolean gtranslator_check_file(gchar *filename)
{
	FILE *fs;
	/*
	 * Open up the given file.
	 */
	fs=fopen(filename, "r");
	if(fs!=NULL)
	{
		fclose(fs);
		return TRUE;	
	}
	else
	{
		return FALSE;
	}
}

/*
 * Returns the list of the recent files.
 */ 
GList *gtranslator_get_recent_files(gboolean delete_nonexistent)
{
	GList 	*files=g_list_alloc();
	gchar 	*filename=g_new0(gchar,1);
	gchar	*request=g_new0(gchar,1);
	gint 	length=0;

	/*
	 * Get the stored length of the recent files' list.
	 */ 
	gtranslator_config_init();
	length=gtranslator_get_recent_files_length();
	
	/*
	 * Check the resulting length of the list.
	 */
	if(length<=0)
	{
		/*
		 * In this case, we don't have got any recent files.
		 */
		g_list_free(files);
		return NULL;
	}
	
	/*
	 * Now get every recent filename into the list.
	 */
	while(length>0)
	{
		filename=gtranslator_get_recent_files_entry(length);
		if(filename)
		{
			/*
			 * Check the files conditionally for existence and
			 *  don't add them to the recent files' list in this
			 *   case.
			 */   
			if(delete_nonexistent==TRUE)
			{
				if(gtranslator_check_file(filename))
				{
					g_list_prepend(files, (gpointer) filename);
				}
			}
			else
			{
				g_list_prepend(files, (gpointer) filename);
			}
		}
		length--;
	}
	
	gtranslator_config_close();
	
	/*
	 * Clean up the used variables.
	 */ 
	if(filename)
	{
		g_free(filename);
	}
	if(request)
	{
		g_free(request);
	}
	
	return files;
}

/*
 * Stores the given list as the recent files' list.
 */
void gtranslator_set_recent_files(GList *filenames)
{
	gint length=0;
	g_return_if_fail(filenames!=NULL);
	
	length=g_list_length(filenames);
	
	gtranslator_config_init();
	gtranslator_config_set_int("recent_files/length", length);
	
	while(length>0)
	{
		gchar *request=g_new0(gchar,1);
		request=g_strdup_printf("recent_files/%i", length);
		g_warning("Path \"%s\" value \"%s\"", request,
			(gchar *) g_list_nth_data(filenames, length));
		gtranslator_config_set_string(request,
			(gchar *) g_list_nth_data(filenames, length));
		g_free(request);
		length--;
	}
	
	gtranslator_config_close();
}

/*
 * Returns the stored length of the recent files' list.
 */
gint gtranslator_get_recent_files_length()
{
	return (gtranslator_config_get_int("recent_files/length"));
}

/*
 * Returns the n'th recent files' entry.
 */
gchar *gtranslator_get_recent_files_entry(gint n)
{
	gint length=0;
	
	gtranslator_config_init();
	length=gtranslator_get_recent_files_length();
	
	if(n<=length)
	{
		/*
		 * Simply get n'th filename stored in the recent files
		 *  list.
		 */  
		gchar *request, *filename;
		
		request=g_strdup_printf("recent_files/%i", n);
		filename=gtranslator_config_get_string(request);
		
		gtranslator_config_close();
		return filename;
	}	
	else
	{
		gtranslator_config_close();
		return NULL;
	}
}

/*
 * Clears the recent files' list by setting the length of the list
 *  to 0.
 */  
void gtranslator_clear_recent_files()
{
	gtranslator_config_init();
	gtranslator_config_set_int("recent_files/length", 0);
	gtranslator_config_close();
}

/*
 * Append the given filename to the recent files' list.
 */ 
void gtranslator_append_recent_file(gchar *filename)
{
	GList *list=g_list_alloc();
	g_return_if_fail(filename!=NULL);
	
	list=gtranslator_get_recent_files(TRUE);
	
	if(list)
	{
		g_list_prepend(list, (gpointer) filename);
		gtranslator_set_recent_files(list);
	}
}
