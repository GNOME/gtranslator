/*
 * (C) 2001-2004 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Kevin Vandersloot <kfv101@psu.edu>
 *			Thomas Ziehmer <thomas@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
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

#include "gui.h"
#include "learn.h"
#include "messages-table.h"
#include "message.h"
#include "page.h"
#include "preferences.h"
#include "prefs.h"
#include "runtime-config.h"
#include "utils.h"

/*
 * We're now defining the highlighting colors once here at the top via macros.
 */
#define TABLE_FUZZY_COLOR "#ff0000"
#define TABLE_UNTRANSLATED_COLOR "#a7453e"
#define TABLE_TRANSLATED_COLOR "#00ff00"

/*
 * Glade defines
 */
#define GLADE_MESSAGES_TREE_VIEW "treeview_messages"

enum
{
  ORIGINAL_COLUMN,
  TRANSLATION_COLUMN,
  MSG_PTR_COLUMN,
  COLOR_COLUMN,
  N_COLUMNS
};

/*
 * Simply sets up/frees the generally used messages table colors.
 */
static void read_messages_table_colors(void);
static void free_messages_table_colors(void);


/*
 * Own messages table color container - contains the defined color strings and the GdkColors.
 */
typedef struct
{
	gchar	*untranslated;
	gchar	*fuzzy;
	gchar	*translated;

	GdkColor untranslated_color;
	GdkColor fuzzy_color;
	GdkColor translated_color;
} GtrMessagesTableColors;

static GtrMessagesTableColors *messages_table_colors;

/*
 * Initialize and set up the generally used messages table colors.
 */
static void 
read_messages_table_colors()
{	
	messages_table_colors=g_new0(GtrMessagesTableColors, 1);

	/*
	 * Read the values from the prefs in -- but only if desired.
	 */
		messages_table_colors->untranslated=g_strdup(TABLE_UNTRANSLATED_COLOR);
		messages_table_colors->fuzzy=g_strdup(TABLE_FUZZY_COLOR);
		messages_table_colors->translated=g_strdup(TABLE_TRANSLATED_COLOR);


	/*
	 * Now parse our defined color strings into the GdkColor structs of 
	 * our messages table colors.
	 */
	gdk_color_parse(messages_table_colors->untranslated, &messages_table_colors->untranslated_color);
	gdk_color_parse(messages_table_colors->translated, &messages_table_colors->translated_color);
	gdk_color_parse(messages_table_colors->fuzzy, &messages_table_colors->fuzzy_color);
}

/*
 * Frees the internally used GtrMessagesTableColors structure.
 */
static void 
free_messages_table_colors()
{
	if(messages_table_colors)
	{
		g_free(messages_table_colors->untranslated);
		g_free(messages_table_colors->fuzzy);
		g_free(messages_table_colors->translated);

		g_free(messages_table_colors);
	}
}

/*
 * Pops up on a right click in the messages table -- should show any found 
 *  translation from the learn buffer.
 */

void 
gtranslator_tree_size_allocate( 
		GtkTreeView *widget, 
		GtkAllocation *allocation, 
		gpointer data )
{
	/*
	 * Here we will change the column widths so that the message table will look nice.
	 * This function will be called when the size of treeview widget will be changed.
	 */
	GtkTreeViewColumn *col;
	gint width;
	width = allocation->width >> 1;
	col = gtk_tree_view_get_column( widget, ORIGINAL_COLUMN );
	gtk_tree_view_column_set_min_width( col, width );
	gtk_tree_view_column_set_max_width( col, width );
	col = gtk_tree_view_get_column( widget, TRANSLATION_COLUMN );
	gtk_tree_view_column_set_min_width( col, width );
	gtk_tree_view_column_set_max_width( col, width );
}


/*
 * Callback for when Select message based on the GtkTreeSelection chosen
 */
static void 
gtranslator_messages_table_selection_changed(GtkTreeSelection *selection, gpointer data)
{
  GtkTreeIter iter;
  GtkTreeModel* model;
  GtrMsg* msg;
  
  /* Ignore 'null' selections */
  if(!selection) return;
  
  if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE) {
    gtk_tree_model_get(model, &iter, MSG_PTR_COLUMN, &msg, -1);
    if (msg != NULL)
      gtranslator_message_show(msg);
  }
}


/*
 * Create the new messages table
 */
GtrMessagesTable *
gtranslator_messages_table_new()
{
	GtkTreeViewColumn *column;
  	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	GtrMessagesTable *table;

	/*
	 * Allocate some memory
	 */
	table = g_new0(GtrMessagesTable, 1);

	/*
	 * FIXME: move this call to application start-up and put a 'free/read' pair
	 * into the preferences in case the user changes them mid-session. Othewise
	 * it will be possible to double-allocate.
	 */	 
	read_messages_table_colors();

	/*
	 * Create the store and the view
	 */
	table->store = gtk_tree_store_new (
		N_COLUMNS,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_POINTER,
		GDK_TYPE_COLOR);
	//table->widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL (table->store));
	table->widget = glade_xml_get_widget(glade, GLADE_MESSAGES_TREE_VIEW);
	gtk_tree_view_set_model(GTK_TREE_VIEW(table->widget), GTK_TREE_MODEL(table->store));
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(table->widget), TRUE);

	/* ? */
	//g_object_unref (G_OBJECT (store));

	/*
   	 * pv, this for managing column widths
	 */
	g_signal_connect(G_OBJECT(table->widget), "size-allocate",
		G_CALLBACK(gtranslator_tree_size_allocate), NULL );

	/*
	 * Add the original msgid column with the color defs attached to it.
	 */
	renderer=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes(_("Original"), renderer,
		"text", ORIGINAL_COLUMN,
		"foreground-gdk", COLOR_COLUMN,
		NULL);
	gtk_tree_view_column_set_sort_column_id (column, ORIGINAL_COLUMN);
	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(table->widget), column);

	/*
	 * The same now again for the translation - msgid.
	 */
	renderer=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes(_("Translation"), renderer,
		"text", TRANSLATION_COLUMN,
		"foreground-gdk", COLOR_COLUMN,
		NULL);
	gtk_tree_view_column_set_sort_column_id (column, TRANSLATION_COLUMN);
	gtk_tree_view_column_set_resizable(column, TRUE);
  	gtk_tree_view_append_column (GTK_TREE_VIEW(table->widget), column);

	/*
	 * Hook up the 'selection changed' callback
	 */
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(table->widget));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT(selection), "changed", 
		G_CALLBACK(gtranslator_messages_table_selection_changed), 
		NULL);

	/*
	 * Set up the initial Untranslated/Fuzzy/Translated parent nodes
	 */
	gtk_tree_store_append (table->store, &table->untranslated_node, NULL);
	gtk_tree_store_set (table->store, &table->untranslated_node, 
		      ORIGINAL_COLUMN, _("Untranslated"), 
			  TRANSLATION_COLUMN, "",
		      MSG_PTR_COLUMN, NULL,
		      COLOR_COLUMN, &messages_table_colors->untranslated_color,
		      -1);

	gtk_tree_store_append (table->store, &table->fuzzy_node, NULL);
	gtk_tree_store_set (table->store, &table->fuzzy_node, 
		      ORIGINAL_COLUMN, _("Fuzzy"), 
			  TRANSLATION_COLUMN, "",
		      MSG_PTR_COLUMN, NULL,
		      COLOR_COLUMN, &messages_table_colors->fuzzy_color,
		      -1);

	gtk_tree_store_append (table->store, &table->translated_node, NULL);
	gtk_tree_store_set (table->store, &table->translated_node, 
		      ORIGINAL_COLUMN, _("Translated"), 
			  TRANSLATION_COLUMN, "",
		      MSG_PTR_COLUMN, NULL,
		      COLOR_COLUMN, &messages_table_colors->translated_color,
		      -1);

	if(GtrPreferences.collapse_all) {
		gtk_tree_view_collapse_all(GTK_TREE_VIEW(table->widget));
	}
	else {
		gtk_tree_view_expand_all(GTK_TREE_VIEW(table->widget));
	}

	return table;
}


/*
 * Populate a messages table from a given po
 */
void 
gtranslator_messages_table_populate(GtrMessagesTable *table, GList *messages)
{
	GtrMsg *msg;
	const char *msgid, *msgstr;
	
	g_assert(table!=NULL);

	while (messages) {
		msg = GTR_MSG(messages->data);
		msgid = po_message_msgid(msg->message);
		msgstr = po_message_msgstr(msg->message);
		if(msg->is_fuzzy) {
			gtk_tree_store_append(table->store, &msg->iter,
				&table->fuzzy_node);
			gtk_tree_store_set(table->store, &msg->iter,
				ORIGINAL_COLUMN, msgid,
				TRANSLATION_COLUMN, msgstr,
				MSG_PTR_COLUMN,	msg,
				COLOR_COLUMN, &messages_table_colors->fuzzy_color,
				-1);
		}
		else if(msgstr[0] != '\0') {
			gtk_tree_store_append(table->store, &msg->iter,
				&table->translated_node);
			gtk_tree_store_set(table->store, &msg->iter,
				ORIGINAL_COLUMN, msgid,
				TRANSLATION_COLUMN, msgstr,
				MSG_PTR_COLUMN, msg,
				COLOR_COLUMN, &messages_table_colors->translated_color,
				-1);
		}
		else {
			gtk_tree_store_append(table->store, &msg->iter,
				&table->untranslated_node);
			gtk_tree_store_set(table->store, &msg->iter,
				ORIGINAL_COLUMN, msgid,
				TRANSLATION_COLUMN, msgstr,
				MSG_PTR_COLUMN, msg,
				COLOR_COLUMN, &messages_table_colors->untranslated_color,
				-1);
		}

		messages = g_list_next(messages);
	}
}

/*
 * Update the data in a single row
 */
void 
gtranslator_messages_table_update_row(GtrMessagesTable *table, GtrMsg *msg)
{
	const char *msgid, *msgstr;

	g_assert(table != NULL);

	msgid = po_message_msgid(msg->message);
	msgstr = po_message_msgstr(msg->message);
	if(msg->is_fuzzy) {
		gtk_tree_store_append(table->store, &msg->iter, &table->fuzzy_node);
		gtk_tree_store_set(table->store, &msg->iter, ORIGINAL_COLUMN,
			msgid, TRANSLATION_COLUMN, msgstr, MSG_PTR_COLUMN, msg,
			COLOR_COLUMN, messages_table_colors->fuzzy, -1);
	}
	else if(msgstr[0] != '\0') {
		gtk_tree_store_append(table->store, &msg->iter,
			&table->translated_node);
		gtk_tree_store_set(table->store, &msg->iter, ORIGINAL_COLUMN, msgid,
			 TRANSLATION_COLUMN, msgstr, MSG_PTR_COLUMN, msg,
			 COLOR_COLUMN, messages_table_colors->translated, -1);
	}
	else {
		gtk_tree_store_append(table->store, &msg->iter,
			&table->untranslated_node);
		gtk_tree_store_set(table->store, &msg->iter, ORIGINAL_COLUMN,
			msgid, TRANSLATION_COLUMN, msgstr, MSG_PTR_COLUMN, msg, 
			COLOR_COLUMN, messages_table_colors->untranslated, -1);
	}
}

/*
 * Select given message
 */
void 
gtranslator_messages_table_select_row(GtrMessagesTable *table, GtrMsg *msg)
{
	GtkTreeSelection *selection=NULL;

	g_return_if_fail(table->store!=NULL);

	selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(table->store));
	gtk_tree_selection_select_iter(selection, &msg->iter);
}

/*
 * Finish with messages table
 */
void 
gtranslator_message_table_free(GtrMessagesTable *table) {
	gtk_widget_destroy(table->widget);
	gtk_object_destroy(GTK_OBJECT(table->widget));
	free_messages_table_colors();
	g_free(table);
}

