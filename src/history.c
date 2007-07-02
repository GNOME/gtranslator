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
#include "parse.h"
#include "prefs.h"
#include "utils.h"

#include <string.h>
#include <gtk/gtkmenushell.h>

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
void gtranslator_history_add(gchar *filename, gchar *project_id)
{
	GtrHistoryEntry *entry;
	GList *ourhistory=NULL;

	g_return_if_fail(filename!=NULL);
	g_return_if_fail(project_id!=NULL);

	entry=g_new(GtrHistoryEntry,1);

	/*
	 * Assign the GtrHistoryEntry informations.
	 */ 
	entry->filename=g_strdup(filename);
	entry->project_id=g_strdup(project_id);

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
				g_free(subpath);
				g_free(myentry->filename);
				g_free(myentry);
				continue;
			}
		}
		
		g_snprintf(path, 32, "%sproject_id", subpath);
		myentry->project_id=gtranslator_config_get_string(path);

		hl=g_list_append(hl, myentry);

		g_free(subpath);
	}
	
	return hl;
}

#define GLADE_MENU_ITEM_RECENT_FILES "recent_files"
/*
 * The recent menus stuff. TODO: steal the code that works from the standard
 * GNOME2 '~/.recently-used' file (libegg/egg-recent-*.[ch]).
 * Convert gnome app to gtk
 */
void gtranslator_history_show(void)
{
	/*
	 * Couldn't we do that better with bonobo?
	 */
	static gint len = 0;
	gint i;
	GList *list, *onelist;
	GtrHistoryEntry *entry;
	GtkWidget *menu, *item_files, *item;

	/*
	 * Get glade GtkMenuItem
	 */
	item_files = glade_xml_get_widget(glade, GLADE_MENU_ITEM_RECENT_FILES);
	
	menu = gtk_menu_new();

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
	
		/*
		 * Set the GnomeUIInfo settings and labels.
		 */
		/*menu->type=GNOME_APP_UI_ITEM;
		menu->hint=g_strdup_printf(_("Open %s"), entry->filename);
		menu->moreinfo=(gpointer)gtranslator_open_file_dialog_from_history;
		menu->user_data=entry->filename;
		(menu+1)->type=GNOME_APP_UI_ENDOFINFO;*/
		
		/*
		 * Make the new item
		 */
		item = gtk_menu_item_new_with_mnemonic(g_strdup_printf("_%i: %s -- %s", i--,
			gtranslator_history_escape(entry->project_id),
			gtranslator_history_escape(g_path_get_basename(entry->filename))));
		
		//Set signal
		g_signal_connect (item, "activate",
				  G_CALLBACK (gtranslator_open_file_dialog_from_history),
				  entry->filename);
		
		g_signal_connect (item, "select",
				  G_CALLBACK (push_statusbar_data),
				  g_strdup_printf(_("Open %s"), entry->filename));
		
		g_signal_connect (item, "deselect",
				  G_CALLBACK (pop_statusbar_data),
				  NULL);
		
		/*
		 * TODO: with "select" and "deselect" signals make push and pop 
		 * on statusbar
		 */
		
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		
		gtk_widget_show(item);

	}
	// Set menu to recent_files item
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_files), menu);
}

void free_userdata(GtkWidget *widget, gpointer userdata)
{
	g_free(userdata);
}

void gtranslator_open_file_dialog_from_history(GtkWidget *widget, gchar *filename)
{
	GError *error = NULL;

	if(!gtranslator_open(filename, &error)) {
		if(error) {
			gtranslator_show_message(error->message, NULL);
			g_error_free(error);
		}
	}
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

		g_snprintf(path, 32, "%sproject_id", subpath);
		gtranslator_config_set_string(path,
			entry->project_id);	

		g_free(subpath);
		
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
			list = g_list_remove_link(list, r);

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
	g_assert(e != NULL);

	g_free(e->filename);
	g_free(e->project_id);
	g_free(e);
}
