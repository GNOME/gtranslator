/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "insert-tags-plugin.h"
#include "msg.h"
#include "notebook.h"
#include "window.h"

#include <glib/gi18n-lib.h>
#include <string.h>

#define WINDOW_DATA_KEY "GtranslatorInsertTagsPluginWindowData"

GTR_PLUGIN_REGISTER_TYPE(GtranslatorInsertTagsPlugin, gtranslator_insert_tags_plugin)

static const GtkActionEntry action_entries[] =
{
	{ "InsertTags", NULL, N_("_Insert Tags") }
};

const gchar submenu[] =
"<ui>"
"  <menubar name='MainMenu'>"
"    <menu name='EditMenu' action='Edit'>"
"      <placeholder name='EditOps_1'>"
"        <menuitem name='EditInsertTags' action='InsertTags' />"
"      </placeholder>"
"    </menu>"
"  </menubar>"
"</ui>";

typedef struct
{
	GtkActionGroup *action_group;
	guint           ui_id;
} WindowData;

static void
free_window_data (WindowData *data)
{
	g_return_if_fail (data != NULL);

	g_free (data);
}

static void
update_ui_real (GtranslatorWindow *window,
		WindowData   *data)
{
	GtkTextView *view;
	GtkAction *action;

	view = GTK_TEXT_VIEW (gtranslator_window_get_active_view (window));

	action = gtk_action_group_get_action (data->action_group,
					      "InsertTags");
	gtk_action_set_sensitive (action,
				  (view != NULL) &&
				  gtk_text_view_get_editable (view));
}

static void
gtranslator_insert_tags_plugin_init (GtranslatorInsertTagsPlugin *message_table)
{
}

static void
gtranslator_insert_tags_plugin_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_insert_tags_plugin_parent_class)->finalize (object);
}

static void
on_menuitem_activated (GtkMenuItem *item,
		       GtranslatorWindow *window)
{
	const gchar *name;
	GtkWidget *label;
	gchar **str;
	GtranslatorView *view;
	GtkTextBuffer *buffer;
	GtkTextIter iter;
	
	label = gtk_bin_get_child (GTK_BIN (item));
	name = gtk_label_get_text (GTK_LABEL (label));
	
	str = g_strsplit (name, "|", 2);
	
	view = gtranslator_window_get_active_view (window);
	
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	
	gtk_text_buffer_begin_user_action (buffer);
	gtk_text_buffer_insert_at_cursor (buffer,
					  str[0], strlen (str[0]));
					  
	gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)),
					  str[1], strlen (str[1]));
	gtk_text_buffer_end_user_action (buffer);
	
	gtk_text_buffer_get_iter_at_mark (buffer, &iter,
					  gtk_text_buffer_get_insert (buffer));
	
	gtk_text_iter_backward_cursor_positions (&iter, strlen (str[1]));
	gtk_text_buffer_place_cursor (buffer, &iter);
	
	g_strfreev (str);
}

static void
parse_arrays (GSList *start,
	      GSList *close,
	      GtranslatorWindow *window)
{
	GtkUIManager *manager;
	GtkWidget *insert_tags;
	GtkWidget *menuitem;
	GtkWidget *menu;
	static guint i = 1;
	
	manager = gtranslator_window_get_ui_manager (window);
	
	insert_tags = gtk_ui_manager_get_widget (manager,
					  "/MainMenu/EditMenu/EditOps_1/EditInsertTags");
	
	if (start == NULL || close == NULL)
	{
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (insert_tags), NULL);
		gtk_widget_set_sensitive (insert_tags, FALSE);
		i = 1;
		return;
	}
	
	gtk_widget_set_sensitive (insert_tags, TRUE);
	
	menu = gtk_menu_new ();
	gtk_menu_set_accel_group (GTK_MENU (menu),
				  gtk_ui_manager_get_accel_group(manager));
	
	close = g_slist_reverse (close);
	
	do{
		gchar *name, *accel_path;
		
		if (!close)
			return;
		
		name = g_strdup_printf ("%s|%s", (gchar *)start->data, (gchar *)close->data);
		menuitem = gtk_menu_item_new_with_label (name);
		gtk_widget_show (menuitem);
		
		accel_path = g_strdup_printf ("<Gtranslator-sheet>/Edit/_Insert Tags/%s", name);
		
		gtk_menu_item_set_accel_path (GTK_MENU_ITEM (menuitem), accel_path);
		gtk_accel_map_add_entry (accel_path, i+48, GDK_CONTROL_MASK);

		g_free (accel_path);
		g_free (name);
		
		g_signal_connect (menuitem, "activate",
				  G_CALLBACK (on_menuitem_activated), window);
		
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
		i++;
	}while ((start = g_slist_next (start)));
	
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (insert_tags), menu);
}

static void
showed_message_cb (GtranslatorTab *tab,
		   GtranslatorMsg *msg,
		   GtranslatorWindow *window)
{
	const gchar *msgid;
	GRegex *regex;
	GMatchInfo *match_info;
	GSList *start = NULL, *close = NULL;
	gchar *word;
	
	msgid = gtranslator_msg_get_msgid (msg);
	
	/*
	 * Start regular expression like "<b>"
	 */
	regex = g_regex_new ("<[a-zA-Z=\" ]+>", 0, 0, NULL);
	g_regex_match (regex, msgid, 0, &match_info);
	while (g_match_info_matches (match_info))
	{
		word = g_match_info_fetch (match_info, 0);
		start = g_slist_append (start, word);
		g_match_info_next (match_info, NULL);
	}
	g_match_info_free (match_info);
	g_regex_unref (regex);
	
	/*
	 * Close regular expression like "</b>"
	 */
	regex = g_regex_new ("</[a-zA-Z ]+>", 0, 0, NULL);
	g_regex_match (regex, msgid, 0, &match_info);
	while (g_match_info_matches (match_info))
	{
		word = g_match_info_fetch (match_info, 0);
		close = g_slist_append (close, word);
		g_match_info_next (match_info, NULL);
	}
	g_match_info_free (match_info);
	g_regex_unref (regex);
	
	parse_arrays (start, close, window);
}


static void
page_added_cb (GtkNotebook *notebook,
	       GtkWidget   *child,
	       guint        page_num,
	       GtranslatorWindow *window)
{	
	g_signal_connect (child, "showed-message",
			  G_CALLBACK (showed_message_cb), window);
}

static void
impl_activate (GtranslatorPlugin *plugin,
	       GtranslatorWindow *window)
{
	GtkUIManager *manager;
	WindowData *data;
	GError *error = NULL;
	GtranslatorNotebook *notebook;
	GList *tabs = NULL;
	
	g_return_if_fail (GTR_IS_WINDOW (window));

	data = g_new (WindowData, 1);

	manager = gtranslator_window_get_ui_manager (window);

	data->action_group = gtk_action_group_new ("GtranslatorInsertTagsPluginActions");
	gtk_action_group_set_translation_domain (data->action_group, 
						 GETTEXT_PACKAGE);
	gtk_action_group_add_actions (data->action_group,
				      action_entries,
				      G_N_ELEMENTS (action_entries), 
				      window);

	gtk_ui_manager_insert_action_group (manager, data->action_group, -1);

	data->ui_id = gtk_ui_manager_add_ui_from_string (manager,
							 submenu,
							 -1,
							 &error);
	if (data->ui_id == 0)
	{
		g_warning (error->message);
		return;
	}

	g_object_set_data_full (G_OBJECT (window), 
				WINDOW_DATA_KEY, 
				data,
				(GDestroyNotify) free_window_data);

	update_ui_real (window, data);
	
	/*Adding menuitems*/
	
	notebook = gtranslator_window_get_notebook (window);
	
	g_signal_connect (GTK_NOTEBOOK (notebook),
			  "page-added",
			  G_CALLBACK (page_added_cb), window);
			 
	tabs = gtranslator_window_get_all_tabs (window);
	
	if(tabs == NULL)
		return;
	do{
		g_signal_connect (tabs->data, "showed-message",
				  G_CALLBACK (showed_message_cb), window);
	}while((tabs = g_list_next(tabs)));
}

static void
impl_deactivate(GtranslatorPlugin *plugin,
	        GtranslatorWindow *window)
{
	GtranslatorNotebook *notebook;
	GtkUIManager *manager;
	WindowData *data;
	
	manager = gtranslator_window_get_ui_manager (window);

	data = (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
	g_return_if_fail (data != NULL);

	gtk_ui_manager_remove_ui (manager, data->ui_id);
	gtk_ui_manager_remove_action_group (manager, data->action_group);

	g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);	
	
	notebook = gtranslator_window_get_notebook(window);
	
	g_signal_handlers_disconnect_by_func(notebook,
					     page_added_cb,
					     window);
}

static void
impl_update_ui (GtranslatorPlugin *plugin,
		GtranslatorWindow *window)
{
	WindowData *data;

	data = (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
	g_return_if_fail (data != NULL);

	update_ui_real (window, data);
}

static void
gtranslator_insert_tags_plugin_class_init (GtranslatorInsertTagsPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

	object_class->finalize = gtranslator_insert_tags_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
	plugin_class->update_ui = impl_update_ui;
}
