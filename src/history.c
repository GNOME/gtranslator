/*
 * (C) 2001 	Fatih Demir <kabalak@gmx.net>
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

#include "history.h"
#include <libgtranslator/preferences.h>

/*
 * A helper function which should assort the given list.
 */
GList *gtranslator_history_assort(GList *list);

/*
 * Save the given GList of GtrHistoryEntry's.
 */
void gtranslator_history_save(GList *list);

/*
 * Frees the GtrHistoryEntry.
 */
void gtranslator_history_entry_free(GtrHistoryEntry *e);

/*
 * Adds a history entry.
 */
void gtranslator_history_add(gchar *filename,
	gchar *project_name, gchar *project_version)
{
	GtrHistoryEntry *entry=g_new0(GtrHistoryEntry,1);
	GList *ourhistory=g_list_alloc();

	g_return_if_fail(filename!=NULL);
	g_return_if_fail(project_name!=NULL);
	g_return_if_fail(project_version!=NULL);
	
	/*
	 * Assign the GtrHistoryEntry informations.
	 */ 
	entry->filename=g_strdup(filename);
	entry->project_name=g_strdup(project_name);
	entry->project_version=g_strdup(project_version);

	/*
	 * Add the current item to the history list.
	 */ 
	ourhistory=gtranslator_history_get();
	ourhistory=g_list_prepend(ourhistory, (gpointer)entry);
	ourhistory=gtranslator_history_assort(ourhistory);
	gtranslator_history_save(ourhistory);

	if(ourhistory)
	{
		g_list_free(ourhistory);
	}

	gtranslator_history_entry_free(entry);
}

/*
 * Returns the list of GtrHistoryEntry's.
 */
GList *gtranslator_history_get(void)
{
	gchar *path=g_new0(gchar,1);
	gchar *subpath=g_new0(gchar,1);
	GList *hl=g_list_alloc();
	gint count=0, c;

	gtranslator_config_init();

	count=gtranslator_config_get_int("history_length");

	for(c=0;c < count; c++)
	{
		GtrHistoryEntry *myentry=g_new0(GtrHistoryEntry,1);
		
		path=g_strdup_printf("history_entry%d", c);

		subpath=g_strdup_printf("%s/filename", path);

		myentry->filename=g_strdup(
			gtranslator_config_get_string(subpath));
		
		subpath=g_strdup_printf("%s/project_name", path);

		myentry->project_name=g_strdup(
			gtranslator_config_get_string(subpath));
		
		subpath=g_strdup_printf("%s/project_version", path);

		myentry->project_version=g_strdup(
			gtranslator_config_get_string(subpath));

		hl=g_list_prepend(hl, (gpointer)myentry);

		gtranslator_history_entry_free(myentry);
	}
	
	gtranslator_config_close();

	g_free(path);
	g_free(subpath);

	return hl;
}

void gtranslator_history_save(GList *list)
{
	GtrHistoryEntry *entry=g_new0(GtrHistoryEntry,1);
	gchar *path=g_new0(gchar,1);
	gchar *subpath=g_new0(gchar,1);
	gint number=0;

	gtranslator_config_init();
	
	while(list)
	{
		entry=GTR_HISTORY_ENTRY(list->data);

		if(!entry)
		{
			break;
		}
		
		path=g_strdup_printf("history_entry%d", number);
		
		subpath=g_strdup_printf("%s/filename", path);

		gtranslator_config_set_string(subpath,
			entry->filename);

		subpath=g_strdup_printf("%s/project_name", path);

		gtranslator_config_set_string(subpath,
			entry->project_name);	
		
		subpath=g_strdup_printf("%s/project_version", path);

		gtranslator_config_set_string(subpath,
			entry->project_version);
		
		number++;	
		list=list->next;
	}

	gtranslator_config_set_int("history_length", number);
	
	gtranslator_config_close();
	
	g_free(path);
	g_free(subpath);
	
	gtranslator_history_entry_free(entry);
	
	g_return_if_fail(list!=NULL);
}

/*
 * Assort the list.
 */ 
GList *gtranslator_history_assort(GList *list)
{
	GList *hlist=g_list_alloc();
	GtrHistoryEntry *entry=g_new0(GtrHistoryEntry,1);

	g_return_val_if_fail(list!=NULL, NULL);

	for(hlist=list; hlist!=NULL; hlist=hlist->next)
	{
		entry=GTR_HISTORY_ENTRY(hlist->data);

		if(hlist->next)
		{
			if(entry==GTR_HISTORY_ENTRY(hlist->next->data))
			{
				hlist=g_list_remove_link(hlist, (gpointer)
					hlist->next->data);
			}
		}
	}
	
	gtranslator_history_entry_free(entry);
	
	return hlist;
}

/*
 * Free the GtrHistoryEntry if available.
 */
void gtranslator_history_entry_free(GtrHistoryEntry *e)
{
	if(!e)
	{
		return;
	}
	else
	{
		g_free(e->filename);
		g_free(e->project_name);
		g_free(e->project_version);
		g_free(e);
	}
}
