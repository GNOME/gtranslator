/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2008  Igalia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "actions.h"
#include "application.h"
#include "header.h"
#include "msg.h"
#include "notebook.h"
#include "tab.h"
#include "plugins-engine.h"
#include "po.h"
#include "statusbar.h"
#include "utils.h"
#include "window.h"

#include "egg-toolbars-model.h"
#include "egg-toolbar-editor.h"
#include "egg-editable-toolbar.h"

#include <gdl/gdl-dock.h>
#include <gdl/gdl-dock-bar.h>
#include <gdl/gdl-dock-layout.h>
#include <gdl/gdl-switcher.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_STOCK_FUZZY_NEXT "gtranslator-fuzzy-next"
#define GTR_STOCK_FUZZY_PREV "gtranslator-fuzzy-prev"
#define GTR_STOCK_UNTRANS_NEXT "gtranslator-untranslated-next"
#define GTR_STOCK_UNTRANS_PREV "gtranslator-untranslated-prev"
#define GTR_STOCK_FUZZY_UNTRANS_NEXT "gtranslator-fuzzy-untranslated-next"
#define GTR_STOCK_FUZZY_UNTRANS_PREV "gtranslator-fuzzy-untranslated-prev"

#define GTR_WINDOW_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_WINDOW,     \
					 GtranslatorWindowPrivate))

static void gtranslator_window_cmd_edit_toolbar (GtkAction *action, GtranslatorWindow *window);


G_DEFINE_TYPE(GtranslatorWindow, gtranslator_window, GTK_TYPE_WINDOW)

struct _GtranslatorWindowPrivate
{
	GtkWidget *main_box;
	
	GtkWidget *menubar;
	GtkWidget *view_menu;
	GtkWidget *toolbar;
	GtkActionGroup *always_sensitive_action_group;
	GtkActionGroup *action_group;
	
	GtkWidget *notebook;
	GtranslatorTab *active_tab;
	
	GtkWidget *dock;
 	GdlDockLayout *layout_manager;
	GHashTable *widgets;
	
	GtkWidget *statusbar;
		
	GtkUIManager *ui_manager;
	GtkRecentManager *recent_manager;
	GtkWidget *recent_menu;

        GtkWidget *tm_menu;
	
	gint            width;
        gint            height; 
	GdkWindowState  window_state;
	
	gboolean destroy_has_run : 1;
};

enum
{
	TARGET_URI_LIST = 100
};	

static const GtkActionEntry always_sensitive_entries[] = {
	
	{ "File", NULL, N_("_File") },
        { "Edit", NULL, N_("_Edit") },
	{ "View", NULL, N_("_View") },
	//{ "Bookmarks", NULL, N_("_Bookmarks") },
	//{ "Actions", NULL, N_("_Actions") },
	{ "Search", NULL, N_("_Search") },
        { "Go", NULL, N_("_Go") },
	{ "Help", NULL, N_("_Help") },
	
	/* File menu */
	{ "FileOpen", GTK_STOCK_OPEN, NULL, "<control>O",
	  N_("Open a PO file"),
	  G_CALLBACK (gtranslator_open_file_dialog) },
	{ "FileRecentFiles", NULL, N_("_Recent Files"), NULL,
	  NULL, NULL },
	{ "FileQuitWindow", GTK_STOCK_QUIT, NULL, "<control>Q", 
	  N_("Quit the program"),
	  G_CALLBACK (gtranslator_file_quit) },
	
	/* Edit menu */
	{ "EditToolbar", NULL, N_("T_oolbar"), NULL, NULL,
          G_CALLBACK (gtranslator_window_cmd_edit_toolbar) },
	{ "EditPreferences", GTK_STOCK_PREFERENCES, NULL, NULL,
	  N_("Edit gtranslator preferences"),
	  G_CALLBACK (gtranslator_actions_edit_preferences) },
	{ "EditHeader", GTK_STOCK_PROPERTIES, N_("_Header..."), NULL, NULL,
	  G_CALLBACK (gtranslator_actions_edit_header) },
		
	/* Help menu */
	{ "HelpContents", GTK_STOCK_HELP, N_("_Contents"), "F1", NULL,
	  G_CALLBACK (gtranslator_cmd_help_contents) },
	{ "HelpAbout", GTK_STOCK_ABOUT, NULL, NULL, NULL,
	  G_CALLBACK (gtranslator_about_dialog) },
};
	

/* Normal items */
static const GtkActionEntry entries[] = {
	
	/* File menu */
       	{ "FileSave", GTK_STOCK_SAVE, NULL, "<control>S",
	  N_("Save the current file"),
	  G_CALLBACK (gtranslator_save_current_file_dialog) },
	{ "FileSaveAs", GTK_STOCK_SAVE_AS, NULL, "<shift><control>S",
	  N_("Save the current file with another name"), 
	  G_CALLBACK (gtranslator_save_file_as_dialog) },
	/*{ "FileRevert", GTK_STOCK_REVERT_TO_SAVED, N_("_Revert"), NULL,
	  N_(),
	  G_CALLBACK (gtranslator_save_file_as_dialog) },*/
	{ "FileCloseWindow", GTK_STOCK_CLOSE, NULL, "<control>W", 
	  N_("Close the current file"),
	  G_CALLBACK (gtranslator_file_close) },
	
        /* Edit menu */
        { "EditUndo", GTK_STOCK_UNDO, NULL, "<control>Z", 
	  N_("Undo last operation"),
	  G_CALLBACK(gtranslator_actions_edit_undo) },
	{ "EditRedo", GTK_STOCK_REDO, NULL, "<shift><control>Z",
	  N_("Redo last undone operation"),
	  G_CALLBACK(gtranslator_actions_edit_redo) },
	{ "EditCut", GTK_STOCK_CUT, NULL, "<control>X",
	  N_("Cut the selected text"), 
	  G_CALLBACK (gtranslator_actions_edit_cut) },	
	{ "EditCopy", GTK_STOCK_COPY, NULL, "<control>C",
	  N_("Copy the selected text"),
	  G_CALLBACK (gtranslator_actions_edit_copy) },
	{ "EditPaste", GTK_STOCK_PASTE, NULL, "<control>V",
	  N_("Paste the contents of the clipboard"),
	  G_CALLBACK (gtranslator_actions_edit_paste) },
	{ "EditClear", GTK_STOCK_CLEAR, NULL, NULL,
	  N_("Clear the selected translation"),
	  G_CALLBACK (gtranslator_actions_edit_clear) },
	{ "EditHeader", GTK_STOCK_PROPERTIES, N_("_Header..."), NULL, NULL,
	  G_CALLBACK (gtranslator_actions_edit_header) },
	{ "EditComment", GTK_STOCK_INDEX, N_("C_omment..."), NULL,
	  N_("Edit message comment"), 
	  G_CALLBACK (gtranslator_edit_message_comment) },
	{ "EditMessage2Trans", NULL, N_("Copy _Message to Translation"), "<control>space",
	  N_("Copy original message contents to the translation field"),
	  G_CALLBACK (gtranslator_message_copy_to_translation) },
	{ "EditFuzzy", NULL, N_("Toggle _Fuzzy Status"), "<control>U",
	  N_("Toggle fuzzy status of a message"),
	  G_CALLBACK (gtranslator_message_status_toggle_fuzzy) },
	{ "EditTranslationMemory", NULL, N_("_Translation Memory"), NULL, NULL, NULL},


	/* View menu */
	{ "ViewContext", NULL, N_("_Context"), "<control>J",
	  N_("Show the Context panel"),
	  G_CALLBACK (gtranslator_actions_view_context) },
	{ "ViewTranslationMemory", NULL, N_("_Translation Memory"), "<control>K",
	  N_("Show the Translation Memory panel"),
	  G_CALLBACK (gtranslator_actions_view_translation_memory) },
	/*{ "ViewSidePane", NULL, N_("Side _Pane"), "F9",
	  N_("Show or hide the side pane in the current window"),
	  NULL },*/
	
	/* Bookmarks menu */
	/*{ "BookmarksAdd", GTK_STOCK_ADD, N_("_Add Bookmark"), "<control>D",
          N_("Add a bookmark to the current message"), NULL},
	{ "BookmarksEdit", GTK_STOCK_EDIT, N_("_Edit Bookmarks"), "<control>B",
          N_("Edit stored bookmarks"), NULL},*/
	
	/* Action menu */
	/*{ "ActionsCompile", GTK_STOCK_CONVERT, N_("_Compile"), NULL,
          N_("Compile the current file to a MO file"), NULL },
	{ "ActionsRefresh", GTK_STOCK_REFRESH, NULL, NULL,
          N_("  "), NULL },
	  //G_CALLBACK(gtranslator_bookmark_adding_dialog) },
	{ "ActionsAutotranslate", NULL, N_("Aut_otranslate..."), NULL,
          N_("Autotranslate the current file using a translation memory"), NULL},
	  //G_CALLBACK(gtranslator_auto_translation_dialog) },
	{ "ActionsRemoveTranslations", GTK_STOCK_REMOVE, N_("Remo_ve All Translations..."), NULL,
          N_("Remove all existing translations"), NULL},
	  //G_CALLBACK(gtranslator_remove_all_translations_dialog) },*/
	
        /* Go menu */
        { "GoFirst", GTK_STOCK_GOTO_FIRST, NULL, NULL,
          N_("Go to the first message"),
          G_CALLBACK (gtranslator_message_go_to_first) },
	{ "GoPrevious", GTK_STOCK_GO_BACK, NULL, "<alt>Left",
          N_("Move back one message"),
          G_CALLBACK (gtranslator_message_go_to_previous) },
	{ "GoForward", GTK_STOCK_GO_FORWARD, NULL, "<alt>Right",
          N_("Move forward one message"),
          G_CALLBACK (gtranslator_message_go_to_next) },
	{ "GoLast", GTK_STOCK_GOTO_LAST, NULL, NULL,
          N_("Go to the last message"),
          G_CALLBACK (gtranslator_message_go_to_last) },
	{ "GoNextFuzzy", GTR_STOCK_FUZZY_NEXT, N_("Next Fuz_zy"),
	  "<control>Page_Down", N_("Go to the next fuzzy message"),
          G_CALLBACK (gtranslator_message_go_to_next_fuzzy) },
	{ "GoPreviousFuzzy", GTR_STOCK_FUZZY_PREV, N_("Previous Fuzz_y"),
	  "<control>Page_Up", N_("Go to the previous fuzzy message"),
          G_CALLBACK (gtranslator_message_go_to_prev_fuzzy) },
	{ "GoNextUntranslated", GTR_STOCK_UNTRANS_NEXT, N_("Next _Untranslated"),
	  "<alt>Page_Down", N_("Go to the next untranslated message"),
          G_CALLBACK (gtranslator_message_go_to_next_untranslated) },
	{ "GoPreviousUntranslated", GTR_STOCK_UNTRANS_PREV, N_("Previ_ous Untranslated"),
	  "<alt>Page_Up", N_("Go to the previous untranslated message"),
          G_CALLBACK (gtranslator_message_go_to_prev_untranslated) },
	{ "GoNextFuzzyUntranslated", GTR_STOCK_FUZZY_UNTRANS_NEXT, N_("Next Fu_zzy or Untranslated"),
	  "<control><shift>Page_Down", N_("Go to the next fuzzy or untranslated message"),
          G_CALLBACK (gtranslator_message_go_to_next_fuzzy_or_untranslated) },
	{ "GoPreviousFuzzyUntranslated", GTR_STOCK_FUZZY_UNTRANS_PREV, N_("Pre_vious Fuzzy or Untranslated"),
	  "<control><shift>Page_Up", N_("Go to the previous fuzzy or untranslated message"),
          G_CALLBACK (gtranslator_message_go_to_prev_fuzzy_or_untranslated) },
	{ "GoJump", GTK_STOCK_JUMP_TO, NULL,
	  "<control>G", N_("Jumps to a specific message"),
          G_CALLBACK (gtranslator_message_jump) },

	/* Search menu*/
	{ "SearchFind", GTK_STOCK_FIND, NULL, "<control>F",
	  N_("Search for text"),
	  G_CALLBACK(_gtranslator_actions_search_find) },
	/*{ "SearchFindNext", NULL, N_("Find Ne_xt"), NULL,
	  N_("Search forward for the same text"), NULL},
	 // G_CALLBACK (gtranslator_find) },
	{ "SearchFindPrevious", NULL, N_("Find _Previous"), NULL,
	  N_("Search backward for the same text"), NULL},
	 // G_CALLBACK (gtranslator_find) },*/
	{ "SearchReplace", GTK_STOCK_FIND_AND_REPLACE, NULL, "<control>H",
	  N_("Search for and replace text"),
	  G_CALLBACK (_gtranslator_actions_search_replace) },
	
};

/*
 * Dock funcs
 */
static void
on_toggle_widget_view (GtkCheckMenuItem *menuitem,
		       GtkWidget *dockitem)
{
	gboolean state;
	state = gtk_check_menu_item_get_active (menuitem);
	if (state)
		gdl_dock_item_show_item (GDL_DOCK_ITEM (dockitem));
	else
		gdl_dock_item_hide_item (GDL_DOCK_ITEM (dockitem));
}

static void
on_update_widget_view_menuitem (gpointer key,
				gpointer wid,
				gpointer data)
{
	GtkCheckMenuItem *menuitem;
	GdlDockItem *dockitem;
	
	dockitem = g_object_get_data (G_OBJECT (wid), "dockitem");
	menuitem = g_object_get_data (G_OBJECT (wid), "menuitem");
	
	g_signal_handlers_block_by_func (menuitem,
					 G_CALLBACK (on_toggle_widget_view),
					 dockitem);
	
	if (GDL_DOCK_OBJECT_ATTACHED (dockitem))
		gtk_check_menu_item_set_active (menuitem, TRUE);
	else
		gtk_check_menu_item_set_active (menuitem, FALSE);
	
	g_signal_handlers_unblock_by_func (menuitem,
					   G_CALLBACK (on_toggle_widget_view),
					   dockitem);
}

static void 
on_layout_dirty_notify (GObject *object,
			GParamSpec *pspec,
			GtranslatorWindow *window)
{
	if (!strcmp (pspec->name, "dirty")) {
		gboolean dirty;
		g_object_get (object, "dirty", &dirty, NULL);
		if (dirty) {
			/* Update UI toggle buttons */
			g_hash_table_foreach (window->priv->widgets,
					      on_update_widget_view_menuitem,
					      NULL);
		}
	}
}

static void
gtranslator_window_layout_save (GtranslatorWindow *window,
			const gchar *filename,
			const gchar *name)
{
	g_return_if_fail (GTR_IS_WINDOW (window));
	g_return_if_fail (filename != NULL);

	gdl_dock_layout_save_layout (window->priv->layout_manager, name);
	if (!gdl_dock_layout_save_to_file (window->priv->layout_manager, filename))
		g_warning ("Saving dock layout to '%s' failed!", filename);
}

static void
gtranslator_window_layout_load (GtranslatorWindow *window,
			const gchar *layout_filename,
			const gchar *name)
{
	g_return_if_fail (GTR_IS_WINDOW (window));

	if (!layout_filename ||
		!gdl_dock_layout_load_from_file (window->priv->layout_manager,
						 layout_filename))
	{
		gchar *filename;
		
		filename = g_build_filename (PKGDATADIR"/layout.xml", NULL);
		//DEBUG_PRINT ("Layout = %s", filename);
		if (!gdl_dock_layout_load_from_file (window->priv->layout_manager,
						     filename))
			g_warning ("Loading layout from '%s' failed!!", filename);
		g_free (filename);
	}
	
	if (!gdl_dock_layout_load_layout (window->priv->layout_manager, name))
		g_warning ("Loading layout failed!!");
}


static gboolean
remove_from_widgets_hash (gpointer name,
			  gpointer hash_widget,
			  gpointer widget)
{
	if (hash_widget == widget)
		return TRUE;
	return FALSE;
}

static void
on_widget_destroy (GtkWidget *widget,
		   GtranslatorWindow *window)
{
	//DEBUG_PRINT ("Widget about to be destroyed");
	g_hash_table_foreach_remove (window->priv->widgets,
				     remove_from_widgets_hash,
				     widget);
}

static void
on_widget_remove (GtkWidget *container,
		  GtkWidget *widget,
		  GtranslatorWindow *window)
{
	GtkWidget *dock_item;

	dock_item = g_object_get_data (G_OBJECT (widget), "dockitem");
	if (dock_item)
	{
		gchar* unique_name = g_object_get_data(G_OBJECT(dock_item), "unique_name");
		g_free(unique_name);
		g_signal_handlers_disconnect_by_func (G_OBJECT (dock_item),
						      G_CALLBACK (on_widget_remove),
						      window);
		gdl_dock_item_unbind (GDL_DOCK_ITEM(dock_item));
	}
	if (g_hash_table_foreach_remove (window->priv->widgets,
					 remove_from_widgets_hash,
					 widget)){
		//DEBUG_PRINT ("Widget removed from container");
	}
}

static void
on_widget_removed_from_hash (gpointer widget)
{
	GtranslatorWindow *window;
	GtkWidget *menuitem;
	GdlDockItem *dockitem;
	
	//DEBUG_PRINT ("Removing widget from hash");
	
	window = g_object_get_data (G_OBJECT (widget), "window-object");
	dockitem = g_object_get_data (G_OBJECT (widget), "dockitem");
	menuitem = g_object_get_data (G_OBJECT (widget), "menuitem");
	
	gtk_widget_destroy (menuitem);
	
	g_object_set_data (G_OBJECT (widget), "dockitem", NULL);
	g_object_set_data (G_OBJECT (widget), "menuitem", NULL);

	g_signal_handlers_disconnect_by_func (G_OBJECT (widget),
					      G_CALLBACK (on_widget_destroy), window);
	g_signal_handlers_disconnect_by_func (G_OBJECT (dockitem),
					      G_CALLBACK (on_widget_remove), window);
	
	g_object_unref (G_OBJECT (widget));
}

static void 
add_widget_full (GtranslatorWindow *window, 
		 GtkWidget *widget,
		 const char *name,
		 const char *title,
		 const char *stock_id,
		 GtranslatorWindowPlacement placement,
		 gboolean locked,
		 GError **error)
{
	GtkWidget *item;
	GtkCheckMenuItem* menuitem;

	g_return_if_fail (GTR_IS_WINDOW (window));
	g_return_if_fail (GTK_IS_WIDGET (widget));
	g_return_if_fail (name != NULL);
	g_return_if_fail (title != NULL);

	/* Add the widget to hash */
	if (window->priv->widgets == NULL)
	{
		window->priv->widgets = g_hash_table_new_full (g_str_hash, g_str_equal,
							       g_free,
							       on_widget_removed_from_hash);
	}
	g_hash_table_insert (window->priv->widgets, g_strdup (name), widget);
	g_object_ref (widget);
	
	/* Add the widget to dock */
	if (stock_id == NULL)
		item = gdl_dock_item_new (name, title, GDL_DOCK_ITEM_BEH_NORMAL);
	else
		item = gdl_dock_item_new_with_stock (name, title, stock_id,
						     GDL_DOCK_ITEM_BEH_NORMAL);
	if (locked)
	{
		guint flags = 0;
		flags |= GDL_DOCK_ITEM_BEH_NEVER_FLOATING;
		flags |= GDL_DOCK_ITEM_BEH_CANT_CLOSE;
		flags |= GDL_DOCK_ITEM_BEH_CANT_ICONIFY;
		flags |= GDL_DOCK_ITEM_BEH_NO_GRIP;
		g_object_set(G_OBJECT(item), "behavior", flags, NULL);
	}
	
	gtk_container_add (GTK_CONTAINER (item), widget);
	gdl_dock_add_item (GDL_DOCK (window->priv->dock),
			   GDL_DOCK_ITEM (item), placement);
	gtk_widget_show_all (item);
	
	/* Add toggle button for the widget */
	menuitem = GTK_CHECK_MENU_ITEM (gtk_check_menu_item_new_with_label (title));
	gtk_widget_show (GTK_WIDGET (menuitem));
	gtk_check_menu_item_set_active (menuitem, TRUE);
	gtk_menu_append (GTK_MENU (window->priv->view_menu), GTK_WIDGET (menuitem));

	if (locked)
		g_object_set( G_OBJECT(menuitem), "visible", FALSE, NULL);

	
	g_object_set_data (G_OBJECT (widget), "window-object", window);
	g_object_set_data (G_OBJECT (widget), "menuitem", menuitem);
	g_object_set_data (G_OBJECT (widget), "dockitem", item);
	
	/* For toggling widget view on/off */
	g_signal_connect (G_OBJECT (menuitem), "toggled",
			  G_CALLBACK (on_toggle_widget_view), item);
	
	/*
	  Watch for widget removal/destruction so that it could be
	  removed from widgets hash.
	*/
	g_signal_connect (G_OBJECT (item), "remove",
			  G_CALLBACK (on_widget_remove), window);
	g_signal_connect_after (G_OBJECT (widget), "destroy",
				G_CALLBACK (on_widget_destroy), window);
}

static void 
remove_widget (GtranslatorWindow *window,
	       GtkWidget *widget,
	       GError **error)
{
	GtkWidget *dock_item;

	g_return_if_fail (GTR_IS_WINDOW (window));
	g_return_if_fail (GTK_IS_WIDGET (widget));

	g_return_if_fail (window->priv->widgets != NULL);
	
	dock_item = g_object_get_data (G_OBJECT (widget), "dockitem");
	g_return_if_fail (dock_item != NULL);
	
	/* Remove the widget from container */
	g_object_ref (widget);
	/* It should call on_widget_remove() and clean up should happen */
	gtk_container_remove (GTK_CONTAINER (dock_item), widget);
	g_object_unref (widget);
}

static void 
gtranslator_app_present_widget (GtranslatorWindow *window,
			   GtkWidget *widget,
			   GError **error)
{
	GdlDockItem *dock_item;
	GtkWidget *parent;
	
	g_return_if_fail (GTR_IS_WINDOW (window));
	g_return_if_fail (GTK_IS_WIDGET (widget));
	
	g_return_if_fail (window->priv->widgets != NULL);
	
	dock_item = g_object_get_data (G_OBJECT(widget), "dockitem");
	g_return_if_fail (dock_item != NULL);
	
	/* Hack to present the dock item if it's in a notebook dock item */
	parent = gtk_widget_get_parent (GTK_WIDGET(dock_item) );
	if (GTK_IS_NOTEBOOK (parent))
	{
		gint pagenum;
		pagenum = gtk_notebook_page_num (GTK_NOTEBOOK (parent), GTK_WIDGET (dock_item));
		gtk_notebook_set_current_page (GTK_NOTEBOOK (parent), pagenum);
	} 
	else if (!GDL_DOCK_OBJECT_ATTACHED (dock_item)) 
	{ 
	    gdl_dock_item_show_item (GDL_DOCK_ITEM (dock_item));  
	}
	
	/* FIXME: If the item is floating, present the window */
	/* FIXME: There is no way to detect if a widget was floating before it was
	detached since it no longer has a parent there is no way to access the
	floating property of the GdlDock structure.*/
}

void
set_sensitive_according_to_message(GtranslatorWindow *window,
				   GtranslatorPo *po)
{
	GList *current;
	GtkAction *action;
		
	current = gtranslator_po_get_current_message(po);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoForward");
	gtk_action_set_sensitive (action, 
				  g_list_next(current) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoPrevious");
	gtk_action_set_sensitive (action, 
				  g_list_previous(current) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoFirst");
	gtk_action_set_sensitive (action, 
				  g_list_first(current) != current);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoLast");
	gtk_action_set_sensitive (action, 
				  g_list_last(current) != current);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoNextFuzzy");
	gtk_action_set_sensitive (action, 
				  gtranslator_po_get_next_fuzzy(po) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoPreviousFuzzy");
	gtk_action_set_sensitive (action, 
				  gtranslator_po_get_prev_fuzzy(po) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoNextUntranslated");
	gtk_action_set_sensitive (action, 
				  gtranslator_po_get_next_untrans(po) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoPreviousUntranslated");
	gtk_action_set_sensitive (action, 
				  gtranslator_po_get_prev_untrans(po) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoNextFuzzyUntranslated");
	gtk_action_set_sensitive (action, 
				  gtranslator_po_get_next_fuzzy_or_untrans (po) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoPreviousFuzzyUntranslated");
	gtk_action_set_sensitive (action, 
				  gtranslator_po_get_prev_fuzzy_or_untrans (po) != NULL);
}

static void
set_sensitive_according_to_tab(GtranslatorWindow *window,
			       GtranslatorTab *tab)
{
	GtranslatorView *view;
	GtranslatorPo *po;
	GtkSourceBuffer *buf;
	GtkAction *action;
	GList *current;
	GtranslatorPoState state;
	
	view = gtranslator_tab_get_active_view(tab);
	po = gtranslator_tab_get_po(tab);
	current = gtranslator_po_get_current_message(po);
	buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));
	
	if(gtk_action_group_get_sensitive(window->priv->action_group) == FALSE)
		gtk_action_group_set_sensitive(window->priv->action_group, TRUE);
	
	/*File*/
	state = gtranslator_po_get_state(po);
	action = gtk_action_group_get_action (window->priv->action_group,
					      "FileSave");
	gtk_action_set_sensitive (action,
				  state == GTR_PO_STATE_MODIFIED);
	
	/*Edit*/
	action = gtk_action_group_get_action(window->priv->action_group,
					     "EditUndo");
	gtk_action_set_sensitive (action, 
				  gtk_source_buffer_can_undo (buf));
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "EditRedo");
	gtk_action_set_sensitive (action, 
				  gtk_source_buffer_can_redo (buf));
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "EditCut");
	gtk_action_set_sensitive (action, 
				  gtk_text_buffer_get_has_selection (GTK_TEXT_BUFFER (buf)));
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "EditCopy");
	gtk_action_set_sensitive (action, 
				  gtk_text_buffer_get_has_selection (GTK_TEXT_BUFFER (buf)));
	
	/*Go*/
	set_sensitive_according_to_message(window, po);
}

void
set_sensitive_according_to_window(GtranslatorWindow *window)
{
	gint pages;
	
	pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(window->priv->notebook));
	
	gtk_action_group_set_sensitive(window->priv->action_group,
				       pages > 0);
				       
}

/*
 * gtranslator_window_update_statusbar_message_count:
 * 
 * This func is used to show the global status of the message list
 * in the statusbar widget.
 */
static void
gtranslator_window_update_statusbar_message_count(GtranslatorTab *tab,
						  GtranslatorMsg *message,
						  GtranslatorWindow *window)
{
	GtranslatorPo *po;
	gchar *msg;
	gchar *status, *status_msg;
	gchar *current;
	gchar *total;
	gchar *fuzzy_msg;
	gchar *untranslated_msg;
	gint pos, message_count, fuzzy, untranslated;
	
	g_return_if_fail(GTR_IS_MSG(message));

	po = gtranslator_tab_get_po(tab);
	
	message_count = gtranslator_po_get_messages_count(po);
	pos = gtranslator_po_get_message_position(po);
	fuzzy = gtranslator_po_get_fuzzy_count(po);
	untranslated = gtranslator_po_get_untranslated_count(po);

	switch(gtranslator_msg_get_status(message))
	{
		case GTR_MSG_STATUS_UNTRANSLATED: status = _("Untranslated");
						  break;
		case GTR_MSG_STATUS_TRANSLATED: status = _("Translated");
						  break;
		case GTR_MSG_STATUS_FUZZY: status = _("Fuzzy");
						  break;
		default: break;
	}

	status_msg = g_strdup_printf(_("Status: %s"), status);
	current = g_strdup_printf(_("Current: %d"), pos);
	total = g_strdup_printf(_("Total: %d"), message_count);
	fuzzy_msg = g_strdup_printf(_("Fuzzy: %d"), fuzzy);
	untranslated_msg = g_strdup_printf(_("Untranslated: %d"), untranslated);
		
	msg = g_strconcat("    ", current, "    ", status_msg, "    ", total,
			  "    ", fuzzy_msg, "    ", untranslated_msg, NULL);
	
	gtranslator_statusbar_pop (GTR_STATUSBAR (window->priv->statusbar),
				   0);
	
	gtranslator_statusbar_push (GTR_STATUSBAR (window->priv->statusbar),
				    0,
				    msg);
	
	g_free(msg);			 
	g_free(current);
	g_free(status_msg);
	g_free(total);
	g_free(fuzzy_msg);
	g_free(untranslated_msg);
	
	/*
	 * We have to update the progress bar too
	 */
	gtranslator_statusbar_update_progress_bar (GTR_STATUSBAR (window->priv->statusbar),
						   (gdouble)gtranslator_po_get_translated_count (po),
						   (gdouble)gtranslator_po_get_messages_count (po));
}

static GtranslatorWindow *
get_drop_window (GtkWidget *widget)
{
	GtkWidget *target_window;

	target_window = gtk_widget_get_toplevel (widget);
	g_return_val_if_fail (GTR_IS_WINDOW (target_window), NULL);
	
	return GTR_WINDOW (target_window);
}

/* Handle drops on the GtranslatorWindow */
static void
drag_data_received_cb (GtkWidget        *widget,
		       GdkDragContext   *context,
		       gint              x,
		       gint              y,
		       GtkSelectionData *selection_data,
		       guint             info,
		       guint             time,
		       gpointer          data)
{
	GtranslatorWindow *window;
	GSList *locations;

	window = get_drop_window (widget);
	
	if (window == NULL)
		return;

	if (info == TARGET_URI_LIST)
	{
		locations = gtranslator_utils_drop_get_locations (selection_data);
		gtranslator_actions_load_locations (window, locations);
		
		g_slist_foreach (locations, (GFunc)g_object_unref, NULL);
		g_slist_free (locations);
	}
}

static void
update_overwrite_mode_statusbar (GtkTextView *view, 
				 GtranslatorWindow *window)
{
	if (view != GTK_TEXT_VIEW (gtranslator_window_get_active_view (window)))
		return;
		
	/* Note that we have to use !gtk_text_view_get_overwrite since we
	   are in the in the signal handler of "toggle overwrite" that is
	   G_SIGNAL_RUN_LAST
	*/
	gtranslator_statusbar_set_overwrite (
			GTR_STATUSBAR (window->priv->statusbar),
			!gtk_text_view_get_overwrite (view));
}

static void
set_window_title (GtranslatorWindow *window,
		  gboolean with_path)
{
  GtranslatorPo *po;
  GtranslatorPoState state;
  GtranslatorTab *active_tab;
  GFile *file;
  gchar *title;
  
  if (with_path)
    {
      po = gtranslator_tab_get_po (GTR_TAB (gtranslator_window_get_active_tab (window)));
      active_tab = gtranslator_window_get_active_tab (window);
      state = gtranslator_po_get_state (gtranslator_tab_get_po(active_tab));
      po = gtranslator_tab_get_po (active_tab);
      file = gtranslator_po_get_location (po);
      
      /*
       * Translators: The title of the window when there is only one tab
       */
      title = g_strdup_printf (_("gtranslator - %s"), g_file_get_path (file));
      if (state == GTR_PO_STATE_MODIFIED)
	title = g_strdup_printf (_("gtranslator - *%s"), g_file_get_path (file));
      else
	title = g_strdup_printf (_("gtranslator - %s"), g_file_get_path (file));
      
      g_object_unref (file);
    }
  else
    title = g_strdup (_("gtranslator"));
  
  gtk_window_set_title (GTK_WINDOW (window), title);
  g_free (title);
}

static void
notebook_switch_page(GtkNotebook *nb,
		     GtkNotebookPage *page,
		     gint page_num,
		     GtranslatorWindow *window)
{
	GtranslatorTab *tab;
	GList *msg;
	GtranslatorView *view;
	GtranslatorPo *po;
	gint n_pages;
	
	tab = GTR_TAB (gtk_notebook_get_nth_page (nb, page_num));
	if (tab == window->priv->active_tab)
		return;
	
	/*
	 * Set the window title
	 */
	n_pages = gtk_notebook_get_n_pages (nb);
	if (n_pages == 1)
		set_window_title (window, TRUE);
	else
		set_window_title (window, FALSE);
	
	window->priv->active_tab = tab;
	view = gtranslator_tab_get_active_view (tab);
	
	set_sensitive_according_to_tab(window, tab);
	
	
	/* sync the statusbar */
	gtranslator_statusbar_set_overwrite (GTR_STATUSBAR (window->priv->statusbar),
					     gtk_text_view_get_overwrite (GTK_TEXT_VIEW (view)));
	
	po = gtranslator_tab_get_po(tab);
	msg = gtranslator_po_get_current_message(po);
	gtranslator_window_update_statusbar_message_count(tab,msg->data, window);

	gtranslator_plugins_engine_update_plugins_ui (gtranslator_plugins_engine_get_default (),
						      window, FALSE);
}

static void
notebook_page_removed (GtkNotebook *notebook,
		       GtkWidget   *child,
		       guint        page_num,
		       GtranslatorWindow *window)
{
	gint n_pages;
	
	/*
	 * Set the window title
	 */
	n_pages = gtk_notebook_get_n_pages (notebook);
	if (n_pages == 1)
		set_window_title (window, TRUE);
	else
		set_window_title (window, FALSE);
}

static void
notebook_tab_close_request (GtranslatorNotebook *notebook,
			    GtranslatorTab      *tab,
			    GtranslatorWindow     *window)
{
	/* Note: we are destroying the tab before the default handler
	 * seems to be ok, but we need to keep an eye on this. */
	gtranslator_close_tab (tab, window);
	
	gtranslator_plugins_engine_update_plugins_ui (gtranslator_plugins_engine_get_default (),
						      window, FALSE);
}

static void
can_undo(GtkSourceBuffer *doc,
	 GParamSpec *pspec,
	 GtranslatorWindow *window)
{
	GtkAction *action;
	gboolean sensitive;
	GtranslatorView *view;
	GtkSourceBuffer *buf;

	view = gtranslator_window_get_active_view (window);
	buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));
	
	sensitive = gtk_source_buffer_can_undo(buf);
	
	if (doc != buf)
		return;

	action = gtk_action_group_get_action (window->priv->action_group,
					     "EditUndo");
	gtk_action_set_sensitive (action, sensitive);
}

static void
can_redo(GtkSourceBuffer *doc,
	 GParamSpec *spec,
	 GtranslatorWindow *window)
{
	GtkAction *action;
	gboolean sensitive;
	GtranslatorView *view;
	GtkSourceBuffer *buf;

	view = gtranslator_window_get_active_view (window);
	buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));
	
	sensitive = gtk_source_buffer_can_redo(buf);
	
	if (doc != buf)
		return;

	action = gtk_action_group_get_action (window->priv->action_group,
					     "EditRedo");
	gtk_action_set_sensitive (action, sensitive);
}

static void
sync_state (GtranslatorPo    *po,
	    GParamSpec  *pspec,
	    GtranslatorWindow *window)
{
  int n_pages = 0;

  set_sensitive_according_to_tab (window,
				  gtranslator_tab_get_from_document (po));
  n_pages = gtk_notebook_get_n_pages (GTK_NOTEBOOK(window->priv->notebook));

  if (n_pages == 1)
    set_window_title (window, TRUE);
}

static void
showed_message_cb (GtranslatorTab *tab,
		   GtranslatorMsg *msg,
		   GtranslatorWindow *window)
{
	g_return_if_fail (GTR_IS_TAB (tab));
	
	gtranslator_window_update_statusbar_message_count (tab, msg, window);
	
	set_sensitive_according_to_message (window, gtranslator_tab_get_po (tab));
}

static void
notebook_tab_added(GtkNotebook *notebook,
		   GtkWidget   *child,
		   guint        page_num,
		   GtranslatorWindow *window)
{
	GList *views;
	GtranslatorTab *tab = GTR_TAB(child);
	GtkTextBuffer *buffer;
	gint n_pages;
	
	g_return_if_fail(GTR_IS_TAB(tab));
	
	/*
	 * Set the window title
	 */
	n_pages = gtk_notebook_get_n_pages (notebook);
	if (n_pages == 1)
		set_window_title (window, TRUE);
	else
		set_window_title (window, FALSE);
	
	views = gtranslator_tab_get_all_views(tab, FALSE, TRUE);
	
	while(views)
	{
		buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(views->data));
	
	
		g_signal_connect(GTK_SOURCE_BUFFER(buffer),
				 "notify::can-undo",
				 G_CALLBACK(can_undo),
				 window);
	
		g_signal_connect(GTK_SOURCE_BUFFER(buffer),
				 "notify::can-redo",
				 G_CALLBACK(can_redo),
				 window);
		
		g_signal_connect (views->data,
				  "toggle_overwrite",
				  G_CALLBACK (update_overwrite_mode_statusbar),
				  window);
		
		views = views->next;
	}
	
	g_signal_connect_after (child,
				"message_changed",
				G_CALLBACK(gtranslator_window_update_statusbar_message_count),
				window);
	g_signal_connect_after (child,
				"showed_message",
				G_CALLBACK(showed_message_cb),
				window);
				
	g_signal_connect (gtranslator_tab_get_po (tab), 
			 "notify::state",
			  G_CALLBACK (sync_state), 
			  window);
			  
	gtranslator_plugins_engine_update_plugins_ui (gtranslator_plugins_engine_get_default (),
						      window, FALSE);
}

void
gtranslator_recent_add (GtranslatorWindow *window,
			GFile *location,
			gchar *project_id)
{
	GtkRecentData *recent_data;
	gchar *uri;
	GError *error = NULL;
	gchar *path;
	gchar *display_name;

	uri = g_file_get_uri (location);
	path = g_file_get_path (location);
	display_name = gtranslator_utils_reduce_path ((const gchar *)path);

	recent_data = g_slice_new (GtkRecentData);

	recent_data->display_name   = display_name;
	recent_data->description    = NULL;
	recent_data->mime_type      = "text/x-gettext-translation";
	recent_data->app_name       = (gchar *) g_get_application_name ();
	recent_data->app_exec       = g_strjoin (" ", g_get_prgname (), "%u", NULL);
	recent_data->groups         = NULL;
	recent_data->is_private     = FALSE;

	if (!gtk_recent_manager_add_full (window->priv->recent_manager,
				          uri,
				          recent_data))
	{
      		g_warning ("Unable to add '%s' to the list of recently used documents", uri);
	}

	g_free (uri);
	g_free (path);
	g_free (display_name);
	g_free (recent_data->app_exec);
	g_slice_free (GtkRecentData, recent_data);
}

void
gtranslator_recent_remove (GtranslatorWindow *window,
			   const gchar *path)
{
	gchar *uri;
	GError *error = NULL;

	uri = g_filename_to_uri (path, NULL, &error);
	if (error)
	{	
		g_warning ("Could not convert uri \"%s\" to a local path: %s",
			   uri, error->message);
		g_error_free (error);
		return;
	}
	
	gtk_recent_manager_remove_item (window->priv->recent_manager, uri, &error);
	if (error)
	{
		g_warning ("Could not remove recent-files uri \"%s\": %s",
			   uri, error->message);
		g_error_free (error);
	}
	
	g_free (uri);
}

static void
gtranslator_recent_chooser_item_activated_cb (GtkRecentChooser *chooser,
					      GtranslatorWindow *window)
{
	gchar *uri;
	GError *error = NULL;
	GtkWidget *dialog;
	GSList *list = NULL;
	GFile *location;

	uri = gtk_recent_chooser_get_current_uri (chooser);
	location = g_file_new_for_uri (uri);
	g_free (uri);
	
	list = g_slist_prepend (list, location);
	
	gtranslator_actions_load_locations (window, list);
	g_slist_foreach (list, (GFunc)g_object_unref, NULL);
	g_slist_free (list);
}

static GtkWidget *
create_recent_chooser_menu (GtranslatorWindow *window,
			    GtkRecentManager *manager)
{
	GtkWidget *recent_menu;
	GtkRecentFilter *filter;

	recent_menu = gtk_recent_chooser_menu_new_for_manager (manager);

	gtk_recent_chooser_set_local_only (GTK_RECENT_CHOOSER (recent_menu), TRUE);
	gtk_recent_chooser_set_show_icons (GTK_RECENT_CHOOSER (recent_menu), FALSE);
	gtk_recent_chooser_set_sort_type (GTK_RECENT_CHOOSER (recent_menu), GTK_RECENT_SORT_MRU);
	gtk_recent_chooser_menu_set_show_numbers (GTK_RECENT_CHOOSER_MENU (recent_menu), TRUE);
	gtk_recent_chooser_set_show_tips (GTK_RECENT_CHOOSER (recent_menu), TRUE);

	filter = gtk_recent_filter_new ();
	gtk_recent_filter_add_application (filter, g_get_application_name());
	gtk_recent_chooser_set_filter (GTK_RECENT_CHOOSER (recent_menu), filter);

	return recent_menu;
}

static void
gtranslator_window_set_action_sensitive (GtranslatorWindow *window,
					 const gchar *name,
					 gboolean sensitive)
{
	GtkAction *action = gtk_action_group_get_action (window->priv->action_group,
							 name);
	gtk_action_set_sensitive (action, sensitive);
}

static void
gtranslator_window_cmd_edit_toolbar_cb (GtkDialog *dialog,
					gint response,
					gpointer data)
{
	GtranslatorWindow *window = GTR_WINDOW (data);
        egg_editable_toolbar_set_edit_mode
			(EGG_EDITABLE_TOOLBAR (window->priv->toolbar), FALSE);
	_gtranslator_application_save_toolbars_model (GTR_APP);
        gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
gtranslator_window_cmd_edit_toolbar (GtkAction *action,
				     GtranslatorWindow *window)
{
	GtkWidget *dialog;
	GtkWidget *editor;

	dialog = gtk_dialog_new_with_buttons (_("Toolbar Editor"),
					      GTK_WINDOW (window), 
				              GTK_DIALOG_DESTROY_WITH_PARENT, 
					      GTK_STOCK_CLOSE,
					      GTK_RESPONSE_CLOSE, 
					      NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_CLOSE);
	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dialog)), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dialog)->vbox), 2);
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
	gtk_window_set_default_size (GTK_WINDOW (dialog), 500, 400);
	  
	editor = egg_toolbar_editor_new (window->priv->ui_manager,
					 EGG_TOOLBARS_MODEL (_gtranslator_application_get_toolbars_model (GTR_APP)));
	gtk_container_set_border_width (GTK_CONTAINER (editor), 5);
	gtk_box_set_spacing (GTK_BOX (EGG_TOOLBAR_EDITOR (editor)), 5);
             
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), editor);

	egg_editable_toolbar_set_edit_mode
		(EGG_EDITABLE_TOOLBAR (window->priv->toolbar), TRUE);

	g_signal_connect (dialog, "response",
			  G_CALLBACK (gtranslator_window_cmd_edit_toolbar_cb),
			  window);
	gtk_widget_show_all (dialog);
}

static void
menu_item_select_cb (GtkMenuItem *proxy,
		     GtranslatorWindow *window)
{
	GtkAction *action;
	char *message;

	action = g_object_get_data (G_OBJECT (proxy),  "gtk-action");
	g_return_if_fail (action != NULL);

	g_object_get (G_OBJECT (action), "tooltip", &message, NULL);
	if (message)
	{
		gtranslator_statusbar_push_default (GTR_STATUSBAR (window->priv->statusbar),
						    message);
		g_free (message);
	}
}

static void
menu_item_deselect_cb (GtkMenuItem *proxy,
                       GtranslatorWindow *window)
{
	gtranslator_statusbar_pop_default (GTR_STATUSBAR (window->priv->statusbar));
}

static void
connect_proxy_cb (GtkUIManager *manager,
                  GtkAction *action,
                  GtkWidget *proxy,
                  GtranslatorWindow *window)
{
	if (GTK_IS_MENU_ITEM (proxy))
	{
		g_signal_connect (proxy, "select",
				  G_CALLBACK (menu_item_select_cb), window);
		g_signal_connect (proxy, "deselect",
				  G_CALLBACK (menu_item_deselect_cb), window);
	}
}

static void
disconnect_proxy_cb (GtkUIManager *manager,
                     GtkAction *action,
                     GtkWidget *proxy,
                     GtranslatorWindow *window)
{
	if (GTK_IS_MENU_ITEM (proxy))
	{
		g_signal_handlers_disconnect_by_func
			(proxy, G_CALLBACK (menu_item_select_cb), window);
		g_signal_handlers_disconnect_by_func
			(proxy, G_CALLBACK (menu_item_deselect_cb), window);
	}
}

static void
gtranslator_window_draw (GtranslatorWindow *window)
{
	GtkWidget *hbox; //Statusbar and progressbar
	GtkWidget *widget;
	GError *error = NULL;
	GtkWidget *dockbar;
	GtkWidget *hbox_dock;
	GtkWidget *tm_widget;

	GtranslatorWindowPrivate *priv = window->priv;
	
	/*
	 * Main box
	 */
	priv->main_box = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), priv->main_box);
	gtk_widget_show (priv->main_box);
	
	/*
	 * Menus
	 */
	priv->ui_manager = gtk_ui_manager_new ();
	
	gtk_window_add_accel_group(GTK_WINDOW(window), 
				   gtk_ui_manager_get_accel_group(priv->ui_manager));
	
	priv->always_sensitive_action_group = gtk_action_group_new ("AlwaysSensitiveMenuActions");
	gtk_action_group_set_translation_domain(priv->always_sensitive_action_group, NULL);
	gtk_action_group_add_actions (priv->always_sensitive_action_group,
				      always_sensitive_entries,
				      G_N_ELEMENTS(always_sensitive_entries),
				      window);

	gtk_ui_manager_insert_action_group (priv->ui_manager,
					    priv->always_sensitive_action_group, 0);
	
	priv->action_group = gtk_action_group_new ("MenuActions");
	gtk_action_group_set_translation_domain (priv->action_group, NULL);
	gtk_action_group_add_actions (priv->action_group, entries,
				      G_N_ELEMENTS (entries), window);

	gtk_ui_manager_insert_action_group (priv->ui_manager,
					    priv->action_group, 0);


	if (!gtk_ui_manager_add_ui_from_file (priv->ui_manager,
					      PKGDATADIR"/gtranslator-ui.xml",
					      &error)) {
		g_warning ("building menus failed: %s", error->message);
		g_error_free (error);
	}
	
	/* show tooltips in the statusbar */
	g_signal_connect (priv->ui_manager,
			  "connect_proxy",
			  G_CALLBACK (connect_proxy_cb),
			  window);
	g_signal_connect (priv->ui_manager,
			  "disconnect_proxy",
			  G_CALLBACK (disconnect_proxy_cb),
			  window);
	
	priv->menubar =	 gtk_ui_manager_get_widget (priv->ui_manager,
							   "/MainMenu");
	gtk_box_pack_start (GTK_BOX (priv->main_box),
			    priv->menubar,
			    FALSE, FALSE, 0);
	
	/*
	 * Recent files 
	 */	
	priv->recent_manager = gtk_recent_manager_get_default();

	priv->recent_menu = create_recent_chooser_menu (window, priv->recent_manager);

	g_signal_connect (priv->recent_menu,
			  "item-activated",
			  G_CALLBACK (gtranslator_recent_chooser_item_activated_cb),
			  window);
			  
	widget = gtk_ui_manager_get_widget (priv->ui_manager,
					    "/MainMenu/FileMenu/FileRecentFilesMenu");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (widget), priv->recent_menu);
	
	/*
	 * Translation Memory
	 */
	priv->tm_menu= gtk_ui_manager_get_widget (priv->ui_manager,
					       "/MainMenu/EditMenu/EditTranslationMemory");   

	/*
	 * Toolbar
	 */
	priv->toolbar = GTK_WIDGET 
	  (g_object_new (EGG_TYPE_EDITABLE_TOOLBAR,
			 "ui-manager", priv->ui_manager,
			 "model", _gtranslator_application_get_toolbars_model (GTR_APP),
			 NULL));

	egg_editable_toolbar_show (EGG_EDITABLE_TOOLBAR (priv->toolbar),
				   "DefaultToolBar");
	gtk_box_pack_start (GTK_BOX (priv->main_box),
			    priv->toolbar,
			    FALSE, FALSE, 0);
	gtk_widget_show (priv->toolbar);
	
	/*
	 * Docker
	 */
	hbox = gtk_hbox_new (FALSE, 0);
	priv->dock = gdl_dock_new ();
	gtk_widget_show (priv->dock);
	gtk_box_pack_end(GTK_BOX (hbox),
			   priv->dock, TRUE, TRUE, 0);
	
	dockbar = gdl_dock_bar_new (GDL_DOCK(priv->dock));
	gtk_widget_show (dockbar);
	gtk_box_pack_start (GTK_BOX (hbox),
			    dockbar, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (priv->main_box),
			    hbox, TRUE, TRUE, 0);
	gtk_widget_show (hbox);
	
	priv->layout_manager = gdl_dock_layout_new (GDL_DOCK (priv->dock));
	g_object_set (priv->layout_manager->master,
		      "switcher-style",
		      gtranslator_prefs_manager_get_gdl_style (),
		      NULL);
	g_signal_connect (priv->layout_manager,
			  "notify::dirty",
			  G_CALLBACK (on_layout_dirty_notify),
			  window);

	/*
	 * notebook
	 */
	priv->notebook = GTK_WIDGET(gtranslator_notebook_new());
	g_signal_connect(priv->notebook, "switch-page",
			 G_CALLBACK(notebook_switch_page), window);
	g_signal_connect(priv->notebook, "page-added",
			 G_CALLBACK(notebook_tab_added), window);
	g_signal_connect (priv->notebook, "page-removed",
			  G_CALLBACK (notebook_page_removed), window);
	g_signal_connect (priv->notebook,
			  "tab_close_request",
			  G_CALLBACK (notebook_tab_close_request),
			  window);
	/*
	 * hbox
	 */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start( GTK_BOX(priv->main_box), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);
			
	/*
	 * statusbar & progress bar
	 */
	window->priv->statusbar = gtranslator_statusbar_new ();

	gtk_box_pack_end (GTK_BOX (hbox),
			  window->priv->statusbar,
			  TRUE, 
			  TRUE, 
			  0);
	
	gtk_widget_show(priv->statusbar);
}

static void
gtranslator_window_init (GtranslatorWindow *window)
{
	GtkTargetList *tl;
	gint active_page;
	GtkWidget *view_menu;
	gchar *filename;
	gchar *config_folder;
	
	window->priv = GTR_WINDOW_GET_PRIVATE (window);
	
	window->priv->destroy_has_run = FALSE;
	
	gtranslator_window_draw(window);
		
	set_sensitive_according_to_window(window);
	
	/* Drag and drop support, set targets to NULL because we add the
	   default uri_targets below */
	gtk_drag_dest_set (GTK_WIDGET (window),
			   GTK_DEST_DEFAULT_MOTION |
			   GTK_DEST_DEFAULT_HIGHLIGHT |
			   GTK_DEST_DEFAULT_DROP,
			   NULL,
			   0,
			   GDK_ACTION_COPY);

	/* Add uri targets */
	tl = gtk_drag_dest_get_target_list (GTK_WIDGET (window));
	
	if (tl == NULL)
	{
		tl = gtk_target_list_new (NULL, 0);
		gtk_drag_dest_set_target_list (GTK_WIDGET (window), tl);
		gtk_target_list_unref (tl);
	}
	
	gtk_target_list_add_uri_targets (tl, TARGET_URI_LIST);
	
	/* Connect signals */
	g_signal_connect (window,
			  "drag_data_received",
	                  G_CALLBACK (drag_data_received_cb), 
	                  NULL);

	/*
	 * Create widgets menu 
	 */
	view_menu = 
		gtk_ui_manager_get_widget (window->priv->ui_manager,
					   "/MainMenu/ViewMenu");
	window->priv->view_menu = gtk_menu_item_get_submenu (GTK_MENU_ITEM (view_menu));
	
	/*
	 * Plugins
	 */
	gtranslator_plugins_engine_update_plugins_ui (gtranslator_plugins_engine_get_default (),
						window, TRUE);
	
	/*
	 * Adding notebook to dock
	 */
	add_widget_full (window,
			 window->priv->notebook,
			 "GtranslatorNotebook",
			 _("Documents"),
			 NULL,
			 GTR_WINDOW_PLACEMENT_CENTER,
			 TRUE,
			 NULL);
	
	/*
	 * Loading dock layout
	 */
	config_folder = gtranslator_utils_get_user_config_dir ();
	filename = g_build_filename (config_folder,
				     "gtranslator-layout.xml",
				     NULL);
				    
	gtranslator_window_layout_load (window,
					filename,
					NULL);
	g_free (filename);
	g_free (config_folder);
}

static void
gtranslator_window_dispose (GObject *object)
{
	GtranslatorWindow *window = GTR_WINDOW(object);
	GtranslatorWindowPrivate *priv = window->priv;
	
	if(priv->ui_manager)
	{
		g_object_unref(priv->ui_manager);
		priv->ui_manager = NULL;
	}
	if(priv->action_group)
	{
		g_object_unref(priv->action_group);
		priv->action_group = NULL;
	}
	
	G_OBJECT_CLASS (gtranslator_window_parent_class)->dispose (object);
}

static void
gtranslator_window_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_window_parent_class)->finalize (object);
}

static void
save_panes_state(GtranslatorWindow *window)
{
	gchar *filename;
	gchar *config_folder;

        if (gtranslator_prefs_manager_window_size_can_set ())
        	gtranslator_prefs_manager_set_window_size (window->priv->width,
							   window->priv->height);

        if (gtranslator_prefs_manager_window_state_can_set ())
		gtranslator_prefs_manager_set_window_state (window->priv->window_state);

	config_folder = gtranslator_utils_get_user_config_dir ();
	filename = g_build_filename (config_folder,
				     "gtranslator-layout.xml",
				     NULL);
        gtranslator_window_layout_save (window,
					filename, NULL);
					
	g_free (filename);
	g_free (config_folder);
}

static void
gtranslator_window_destroy (GtkObject *object)
{
        GtranslatorWindow *window;

        window = GTR_WINDOW (object);

        if (!window->priv->destroy_has_run)
        {
                save_panes_state (window);
                window->priv->destroy_has_run = TRUE;
        }

        GTK_OBJECT_CLASS (gtranslator_window_parent_class)->destroy (object);
}


static gboolean 
gtranslator_window_configure_event (GtkWidget         *widget,
				    GdkEventConfigure *event)
{
        GtranslatorWindow *window = GTR_WINDOW (widget);

        window->priv->width = event->width;
        window->priv->height = event->height;
	
        return GTK_WIDGET_CLASS (gtranslator_window_parent_class)->configure_event (widget, event);
}

static void
gtranslator_window_class_init (GtranslatorWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkObjectClass *gobject_class = GTK_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof (GtranslatorWindowPrivate));

	object_class->finalize = gtranslator_window_finalize;
	object_class->dispose = gtranslator_window_dispose;
	
	gobject_class->destroy = gtranslator_window_destroy;
	
	widget_class->configure_event = gtranslator_window_configure_event;
}

/***************************** Public funcs ***********************************/

/**
 * gtranslator_window_create_tab:
 * @window: a #GtranslatorWindow
 * @po: a #GtranslatorPo
 * 
 * Adds a new #GtranslatorTab to the #GtranslatorNotebook and returns the
 * #GtranslatorTab.
 * 
 * Returns: a new #GtranslatorTab object
 */
GtranslatorTab *
gtranslator_window_create_tab(GtranslatorWindow *window,
			      GtranslatorPo *po)
{
	GtranslatorTab *tab;

	tab = gtranslator_tab_new(po);
	gtk_widget_show(GTK_WIDGET(tab));

	gtranslator_notebook_add_page(GTR_NOTEBOOK(window->priv->notebook),
				      tab);

	return tab;
}

/**
 * gtranslator_window_get_active_tab:
 * @window: a #GtranslatorWindow
 * 
 * Gets the active #GtranslatorTab of the @window.
 *
 * Returns: the active #GtranslatorTab of the @window.
 */
GtranslatorTab *
gtranslator_window_get_active_tab(GtranslatorWindow *window)
{
	return gtranslator_notebook_get_page(GTR_NOTEBOOK(window->priv->notebook));
}

/**
 * gtranslator_window_get_all_tabs:
 * @window: a #GtranslatorWindow
 *
 * Gets a list of all tabs in the @window or NULL if there is no tab opened.
 *
 * Returns: a list of all tabs in the @window or NULL if there is no tab opened.
 */
GList *
gtranslator_window_get_all_tabs(GtranslatorWindow *window)
{
	gint num_pages;
	gint i = 0;
	GList *toret = NULL;

	num_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(window->priv->notebook));

	while(i < num_pages)
	{
		toret = g_list_append(toret,
				      gtk_notebook_get_nth_page(GTK_NOTEBOOK(window->priv->notebook),i));
		i++;
	}

	return toret;
}

/**
 * gtranslator_window_get_header_from_active_tab:
 * @window: a #GtranslatorWindow
 *
 * Gets the #GtranslatorHeader of the #GtranslatorPo of in the active
 * #GtranslatorTab.
 *
 * Returns: the #GtranslatorHeader of the #GtranslatorPo of in the active
 * #GtranslatorTab
 */
GtranslatorHeader *
gtranslator_window_get_header_from_active_tab(GtranslatorWindow *window)
{
	GtranslatorTab *current_page;
	GtranslatorPo *po;
	GtranslatorHeader *header;

	g_return_val_if_fail(GTR_IS_WINDOW(window), NULL);

	current_page = gtranslator_window_get_active_tab(window);
	if(!current_page)
		return NULL;

	po = gtranslator_tab_get_po(current_page);
	header = gtranslator_po_get_header(po);

	return header;	
}

/**
 * gtranslator_window_get_notebook:
 * @window: a #GtranslatorWindow
 * 
 * Gets the main #GtranslatorNotebook of the @window.
 *
 * Returns: the #GtranslatorNotebook of the @window
 */
GtranslatorNotebook *
gtranslator_window_get_notebook(GtranslatorWindow *window)
{
	return GTR_NOTEBOOK(window->priv->notebook);
}

/**
 * gtranslator_window_get_statusbar:
 * @window: a #GtranslatorWindow
 *
 * Gets the statusbar widget of the window.
 *
 * Returns: the statusbar widget of the window
 */
GtkWidget *
gtranslator_window_get_statusbar(GtranslatorWindow *window)
{
	return window->priv->statusbar;
}

/**
 * gtranslator_window_get_ui_manager:
 * @window: a #GtranslatorWindow
 *
 * Gets the #GtkUIManager of the window.
 *
 * Returns: the #GtkUIManager of the @window
 */
GtkUIManager *
gtranslator_window_get_ui_manager(GtranslatorWindow *window)
{
	return window->priv->ui_manager;
}

/**
 * gtranslator_window_get_active_view:
 * @window: a #GtranslationWindow
 *
 * Gets the active translation view in the #GtranslationWindow or
 * NULL if there is not tab opened.
 *
 * Returns: the active translation view in the #GtranslationWindow or
 * NULL if there is not tab opened.
 **/
GtranslatorView *
gtranslator_window_get_active_view(GtranslatorWindow *window)
{
	GtranslatorTab *current_tab;
	current_tab = gtranslator_window_get_active_tab(window);
	
	if (!current_tab)
		return NULL;
	
	return gtranslator_tab_get_active_view(current_tab);
}

/**
 * gtranslator_window_get_all_views:
 * @window: the #GtranslationWindow
 * @original: TRUE if you want original TextViews.
 * @translated: TRUE if you want transtated TextViews.
 *
 * Returns all the views currently present in #GtranslationWindow
 *
 * Return value: a newly allocated list of #GtranslationWindow objects
 **/
GList *
gtranslator_window_get_all_views(GtranslatorWindow *window,
				 gboolean original,
				 gboolean translated)
{
	gint numtabs;
	gint i;
	GList *views = NULL;
	GtkWidget *tab;
	
	g_return_val_if_fail(GTR_IS_WINDOW(window), NULL);
	
	numtabs = gtk_notebook_get_n_pages(GTK_NOTEBOOK(window->priv->notebook));
	i = numtabs - 1;
	
	while(i >= 0 && numtabs != 0)
	{
		tab = gtk_notebook_get_nth_page(GTK_NOTEBOOK(window->priv->notebook),
						i);
		views = g_list_concat(views, gtranslator_tab_get_all_views(GTR_TAB(tab),
									   original,
									   translated));
		i--;
	}
	
	return views;
}

/**
 * gtranslator_window_add_widget:
 * @window: a #GtranslatorWindow
 * @widget: the widget to add in the window
 * @name: the name of the widged
 * @title: the title
 * @stock_id: the stock id for the icon
 * @placement: a #GtranslatorWindowPlacement
 *
 * Adds a new widget to the @window in the placement you prefer with and 
 * specific name, title and icon you want.
 */
void
gtranslator_window_add_widget (GtranslatorWindow *window,
			       GtkWidget *widget,
			       const gchar *name,
			       const gchar *title,
			       const gchar *stock_id,
			       GtranslatorWindowPlacement placement)
{
	/*FIXME: We have to manage the error*/
	add_widget_full (window, widget,
			 name, title, stock_id,
			 placement, FALSE, NULL);
}

/**
 * gtranslator_window_remove_widget:
 * @window: a #GtranslatorWindow
 * @widget: the widget to remove
 *
 * Removes from the @window the @widget if it exists.
 */
void
gtranslator_window_remove_widget (GtranslatorWindow *window,
				  GtkWidget *widget)
{
	/*FIXME: We have to manage the error*/
	remove_widget (window, widget, NULL);
}

/**
 * _gtranslator_window_get_layout_manager:
 * @window: a #GtranslatorWindow
 * 
 * Gets the GDL layout manager of the window.
 * 
 * Returns: the GDL layout manager of the window.
 */
GObject *
_gtranslator_window_get_layout_manager (GtranslatorWindow *window)
{
	g_return_val_if_fail (GTR_IS_WINDOW (window), NULL);
	
	return G_OBJECT (window->priv->layout_manager);
}

/**
 * gtranslator_window_get_tab_from_uri:
 * @window: a #GtranslatorWindow
 * @location: the GFile of the po file of the #GtranslatorTab
 *
 * Gets the #GtranslatorTab of the #GtranslatorWindows that matches with the
 * @location.
 *
 * Returns: the #GtranslatorTab which @location matches with its po file.
 */
GtkWidget *
gtranslator_window_get_tab_from_location (GtranslatorWindow *window,
					  GFile *location)
{
	GList *tabs, *l;
	GtranslatorPo *po;
	GFile *po_location;
	
	g_return_if_fail (GTR_IS_WINDOW (window));

	tabs = gtranslator_window_get_all_tabs (window);
	
	for (l = tabs; l != NULL; l = g_list_next (l))
	{
		po = gtranslator_tab_get_po (GTR_TAB (l->data));
		
		po_location = gtranslator_po_get_location (po);

		if (g_file_equal (location, po_location) == TRUE)
		{
			g_object_unref (po_location);
			
			return l->data;
		}
		g_object_unref (po_location);
	}
	
	return NULL;
}

/**
 * gtranslator_window_set_active_tab:
 * @window: a #GtranslatorWindow
 * @tab: a #GtranslatorTab
 *
 * Sets the active tab for the @window.
 */
void
gtranslator_window_set_active_tab (GtranslatorWindow *window,
				   GtkWidget *tab)
{
	gint page;
	
	page = gtk_notebook_page_num (GTK_NOTEBOOK (window->priv->notebook),
				      tab);
	
	gtk_notebook_set_current_page (GTK_NOTEBOOK (window->priv->notebook),
				       page);
}

/**
 * _gtranslator_window_close_tab:
 * @window: a #GtranslatorWindow
 * @tab: a #GtranslatorTab
 *
 * Closes the opened @tab of the @window and sets the right sensitivity of the
 * widgets.
 */
void
_gtranslator_window_close_tab (GtranslatorWindow *window,
			       GtranslatorTab *tab)
{
	gint i;
	
	g_return_if_fail (GTR_IS_TAB (tab));
	
	i = gtk_notebook_page_num (GTK_NOTEBOOK (window->priv->notebook),
				   GTK_WIDGET (tab));
	if (i != -1)
		gtranslator_notebook_remove_page (GTR_NOTEBOOK (window->priv->notebook), i);
	
	set_sensitive_according_to_window (window);
}

GtkWidget
*gtranslator_window_get_tm_menu (GtranslatorWindow *window)
{
        return window->priv->tm_menu;
}
