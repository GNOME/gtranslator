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

#include "preferences.h"
#include "history.h"
#include "gui.h"
#include "dialogs.h"
#include "parse.h"
#include "open-differently.h"
#include "prefs.h"

/*
 * From list remove all entries with same filename as entry->filename
 */
void remove_duplicate_entries(GList *list, GtrHistoryEntry *entry);

void open_file_from_history(GtkWidget *widget, gchar *filename);

/* Utility callback to free userdata */
void free_userdata(GtkWidget *widget, gpointer userdata);

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
	GtrHistoryEntry *entry;
	GList *ourhistory=NULL;

	g_return_if_fail(filename!=NULL);
	g_return_if_fail(project_name!=NULL);
	g_return_if_fail(project_version!=NULL);

	entry=g_new(GtrHistoryEntry,1);
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
	ourhistory=g_list_prepend(ourhistory, entry);
	remove_duplicate_entries(ourhistory, entry);
	gtranslator_history_save(ourhistory);

	g_list_foreach(ourhistory, (GFunc)gtranslator_history_entry_free, NULL);
	g_list_free(ourhistory);
}

/*
 * Reads the list of GtrHistoryEntry's from configuration.
 */
GList *gtranslator_history_get(void)
{
	gchar *subpath;
	gchar path[32];
	GList *hl=NULL;
	gint count, c;

	gtranslator_config_init();

	count=gtranslator_config_get_int("history/length");

	for(c=0;c < count; c++)
	{
		GtrHistoryEntry *myentry=g_new0(GtrHistoryEntry,1);
		
		subpath=g_strdup_printf("history_entry%d/", c);

		g_snprintf(path, 32, "%sfilename", subpath);
		myentry->filename=gtranslator_config_get_string(path);
		
		/*
		 * Check if the file exists before adding it to list
		 */
		if(wants.check_recent_file)
		{
			if(!g_file_exists(myentry->filename))
			{
				g_free(subpath);
				g_free(myentry->filename);
				g_free(myentry);
				continue;
			}
		}
		
		g_snprintf(path, 32, "%sproject_name", subpath);
		myentry->project_name=gtranslator_config_get_string(path);
		
		g_snprintf(path, 32, "%sproject_version", subpath);
		myentry->project_version=gtranslator_config_get_string(path);

		hl=g_list_append(hl, myentry);

		g_free(subpath);
	}
	
	gtranslator_config_close();

	return hl;
}

/*
 * The recent menus stuff.
 */
void gtranslator_history_show(void)
{
	/*
	 * Couldn't we do that better with bonobo?
	 */
	static gint len = 0;
	gint i;
	GnomeUIInfo *menu;
	GList *list, *onelist;
	GtrHistoryEntry *entry;
	
	gchar *menupath = _("_File/Recen_t files/");

	/*
	 * Delete the old entries.
	 */
	gnome_app_remove_menu_range(GNOME_APP(app1), menupath, 0, len);

	/*
	 * Get the old history entries.
	 */ 
	list=gtranslator_history_get();

	i=len=g_list_length(list);

	/*
	 * Parse the list.
	 */
	for(onelist=g_list_last(list); onelist!=NULL; onelist=g_list_previous(onelist))
	{
		/*
		 * Get the history entry.
		 */
		entry=GTR_HISTORY_ENTRY(onelist->data);

		menu=g_new0(GnomeUIInfo,2);

		/*
		 * Set the label name.
		 */
		menu->label=g_strdup_printf("_%i: %s %s -- %s", i--,
		                            entry->project_name,
					    entry->project_version,
					    g_basename(entry->filename));
		
		/*
		 * Set the GnomeUIInfo settings and labels.
		 */
		menu->type=GNOME_APP_UI_ITEM;
		menu->hint=g_strdup_printf(_("Open %s"), entry->filename);
		menu->moreinfo=(gpointer)open_file_from_history;
		menu->user_data=entry->filename;
		(menu+1)->type=GNOME_APP_UI_ENDOFINFO;

		/*
		 * Insert this item into menu
		 */
		gnome_app_insert_menus(GNOME_APP(app1), menupath, menu);
		gnome_app_install_menu_hints(GNOME_APP(app1), menu);

		/*
		 * FIXME: this is intended to free hint
		 */
		gtk_signal_connect(GTK_OBJECT(menu->widget), "destroy",
				   GTK_SIGNAL_FUNC(free_userdata), menu->hint);

		/*
		 * Free the string and the GnomeUIInfo structure.
		 */
		g_free(menu->label);
		g_free(menu);
	}
}

void free_userdata(GtkWidget *widget, gpointer userdata)
{
	g_free(userdata);
}

void open_file_from_history(GtkWidget *widget, gchar *filename)
{
	if (!ask_to_save_file())
		return;
	close_file(NULL, NULL);
	/*
	 * Also detect the right open function in the recent files' list.
	 */
	if(!gtranslator_open_po_file(filename))
		parse(filename);
}

void gtranslator_history_save(GList *list)
{
	GtrHistoryEntry *entry;
	gchar *subpath;
	gchar path[32];
	gint number=0;
	GList *rlist = g_list_first(list);

	gtranslator_config_init();

	while(rlist != NULL)
	{
		entry=GTR_HISTORY_ENTRY(rlist->data);
		
		subpath=g_strdup_printf("history_entry%d/", number);

		g_snprintf(path, 32, "%sfilename", subpath);
		gtranslator_config_set_string(path,
			entry->filename);

		g_snprintf(path, 32, "%sproject_name", subpath);
		gtranslator_config_set_string(path,
			entry->project_name);	
		
		g_snprintf(path, 32, "%sproject_version", subpath);
		gtranslator_config_set_string(path,
			entry->project_version);

		g_free(subpath);
		
		number++;
		/*
		 * Save only 8 entries. It could be made configurable
		 */
		if(number>=8)
			break;
		rlist=g_list_next(rlist);
	}

	gtranslator_config_set_int("history/length", number);
	
	gtranslator_config_close();
}

/*
 * Assort the list.
 */ 
void remove_duplicate_entries(GList *list, GtrHistoryEntry *entry)
{
	GList *rest;

	g_return_if_fail(list!=NULL);
	
	/* look from next entry till the end */
	rest=list->next;
	while(rest!=NULL)
	{
		/* if filenames are equal, remove older record */
		if(!strcmp(entry->filename, GTR_HISTORY_ENTRY(rest->data)->filename))
		{
			GList *r=rest;
			rest=rest->next;
			g_list_remove_link(list, r);
			gtranslator_history_entry_free(GTR_HISTORY_ENTRY(r->data));
			g_list_free_1(r);
		}
		else
			rest=rest->next;
	}
}

/*
 * Free the GtrHistoryEntry if available.
 */
void gtranslator_history_entry_free(GtrHistoryEntry *e)
{
	if(e)
	{
		g_free(e->filename);
		g_free(e->project_name);
		g_free(e->project_version);
		g_free(e);
	}
}
