/*
 * (C) 2001 	Fatih Demir <kabalak@kabalak.net>
 * 		Gediminas Paulauskas <menesis@kabalak.net>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dialogs.h"
#include "gui.h"
#include "history.h"
#include "nautilus-string.h"
#include "open.h"
#include "parse.h"
#include "prefs.h"
#include "utils.h"

#include <string.h>
#include <gtk/gtkmenushell.h>
#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-helper.h>

/*
 * From list remove all entries with same filename as entry->filename
 */
void remove_duplicate_entries(GList *list, GtrHistoryEntry *entry);

void gtranslator_open_file_dialog_from_history(GtkWidget *widget, gchar *filename);

/*
 * Utility callback to free userdata 
 */
void free_userdata(GtkWidget *widget, gpointer userdata);

/*
 * Happens to escape our '_'s in the menu entries.
 */
gchar *gtranslator_history_escape(const gchar *str);

/*
 * Save the given GList of GtrHistoryEntry's.
 */
void gtranslator_history_save(GList *list);

/*
 * Frees the GtrHistoryEntry.
 */
void gtranslator_history_entry_free(GtrHistoryEntry *e);

/*
 * Escape the menu display items rightly.
 */
gchar *gtranslator_history_escape(const gchar *str)
{
	gchar	*display_str=NULL;
	
	g_return_val_if_fail(str!=NULL, NULL);

	display_str=nautilus_str_replace_substring(str, "_", "__");
	return display_str;
}

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

	count=gtranslator_config_get_int("history/length");

	/*
	 * If we don't have got any history, return simply NULL before playing
	 *  'round.
	 */
	if(count <= 0)
	{
		return NULL;
	}
	
	/*
	 * Test if there are more entries stored then desired in the prefs; set
	 *  the maximal count from the prefs if needed.
	 */
	if(count > GtrPreferences.max_history_entries)
	{
		count=GtrPreferences.max_history_entries;
	}

	for(c=0;c < count; c++)
	{
		GtrHistoryEntry *myentry=g_new0(GtrHistoryEntry,1);
		
		subpath=g_strdup_printf("history_entry%d/", c);

		g_snprintf(path, 32, "%sfilename", subpath);
		myentry->filename=gtranslator_config_get_string(path);
		
		/*
		 * Check if the file exists before adding it to list
		 */
		if(GtrPreferences.check_recent_file)
		{
			if(!myentry->filename &&
				!g_file_test(myentry->filename, G_FILE_TEST_EXISTS))
			{
				GTR_FREE(subpath);
				GTR_FREE(myentry->filename);
				GTR_FREE(myentry);
				continue;
			}
		}
		
		g_snprintf(path, 32, "%sproject_name", subpath);
		myentry->project_name=gtranslator_config_get_string(path);
		
		g_snprintf(path, 32, "%sproject_version", subpath);
		myentry->project_version=gtranslator_config_get_string(path);

		hl=g_list_append(hl, myentry);

		GTR_FREE(subpath);
	}
	
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
	gnome_app_remove_menu_range(GNOME_APP(gtranslator_application), menupath, 0, len);

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

		menu=g_new0(GnomeUIInfo, 2);

		/*
		 * Set the label name.
		 */
		if(!entry->project_version || 
			!nautilus_strcasecmp(entry->project_version, "VERSION"))
		{
			menu->label=g_strdup_printf("_%i: %s -- %s", i--,
				gtranslator_history_escape(entry->project_name),
				gtranslator_history_escape(g_path_get_basename(entry->filename)));
		}
		else
		{
			menu->label=g_strdup_printf("_%i: %s %s -- %s", i--,
				gtranslator_history_escape(entry->project_name),
				gtranslator_history_escape(entry->project_version),
				gtranslator_history_escape(g_path_get_basename(entry->filename)));
		}
		
		/*
		 * Set the GnomeUIInfo settings and labels.
		 */
		menu->type=GNOME_APP_UI_ITEM;
		menu->hint=g_strdup_printf(_("Open %s"), entry->filename);
		menu->moreinfo=(gpointer)gtranslator_open_file_dialog_from_history;
		menu->user_data=entry->filename;
		(menu+1)->type=GNOME_APP_UI_ENDOFINFO;

		/*
		 * Insert this item into menu
		 */
		gnome_app_insert_menus(GNOME_APP(gtranslator_application), menupath, menu);
		gnome_app_install_menu_hints(GNOME_APP(gtranslator_application), menu);

		g_signal_connect(GTK_OBJECT(menu->widget), "destroy",
				   GTK_SIGNAL_FUNC(free_userdata), (gpointer) menu->hint);

		/*
		 * Free the string and the GnomeUIInfo structure.
		 */
		g_free((gpointer) menu->label);
		GTR_FREE(menu);
	}
}

void free_userdata(GtkWidget *widget, gpointer userdata)
{
	GTR_FREE(userdata);
}

void gtranslator_open_file_dialog_from_history(GtkWidget *widget, gchar *filename)
{
	if (!gtranslator_should_the_file_be_saved_dialog())
		return;
	gtranslator_file_close(NULL, NULL);

	gtranslator_open_file(filename);
}

void gtranslator_history_save(GList *list)
{
	GtrHistoryEntry *entry;
	gchar *subpath;
	gchar path[32];
	gint number=0;
	GList *rlist = g_list_first(list);

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

		GTR_FREE(subpath);
		
		number++;
	
		/*
		 * Save only as many entries as wished.
		 */
		if(number > GtrPreferences.max_history_entries)
		{
			break;
		}

		GTR_ITER(rlist);
	}

	gtranslator_config_set_int("history/length", number);
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
		/* 
		 * If filenames are equal, remove older record 
		 */
		if(GTR_HISTORY_ENTRY(rest->data)->filename &&
			!strcmp(entry->filename, 
				GTR_HISTORY_ENTRY(rest->data)->filename))
		{
			GList *r=rest;
			
			GTR_ITER(rest);
			g_list_remove_link(list, r);

			gtranslator_history_entry_free(GTR_HISTORY_ENTRY(r->data));
			g_list_free_1(r);
		}
		else
			GTR_ITER(rest);
	}
}

/*
 * Free the GtrHistoryEntry if available.
 */
void gtranslator_history_entry_free(GtrHistoryEntry *e)
{
	if(e)
	{
		GTR_FREE(e->filename);
		GTR_FREE(e->project_name);
		GTR_FREE(e->project_version);
		GTR_FREE(e);
	}
}
