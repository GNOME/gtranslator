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

#include "defines.h"
#include "learn.h"
#include "messages-table.h"
#include "message.h"
#include "page.h"
#include "preferences.h"
#include "prefs.h"
#include "runtime-config.h"
#include "utils.h"

#include <gdk/gdkkeysyms.h>

/*
 * We're now defining the highlighting colors once here at the top via macros.
 */
#define TABLE_FUZZY_COLOR "#ff0000"
#define TABLE_UNTRANSLATED_COLOR "#a7453e"
#define TABLE_TRANSLATED_COLOR "#00ff00"

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

static void 
gtranslator_messages_table_selection_changed(GtrPage *page, GtkTreeSelection *selection,
					     gpointer data);


/*
 * Message category nodes container.
 */
typedef struct {
	GtkTreeIter untranslated_node;
	GtkTreeIter fuzzy_node;
	GtkTreeIter translated_node;
} GtrMessagesTable;

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

static GtrMessagesTable *messages_table;

static GtrMessagesTableColors *messages_table_colors;

/*
 * Initialize and set up the generally used messages table colors.
 */
static void read_messages_table_colors()
{	
	messages_table_colors=g_new0(GtrMessagesTableColors, 1);

	/*
	 * Read the values from the prefs in -- but only if desired.
	 */
	if(GtrPreferences.use_own_mt_colors)
	{
		gchar	*value;
		
#define set_color(key,member,defaultcolor) \
		value=gtranslator_config_get_string("colors/messages_table_" key);  \
		if(value && value[0]=='#') {                                        \
			messages_table_colors->member=g_strdup(value);                  \
			g_free(value);                                                  \
		}                                                                   \
		else {                                                              \
			messages_table_colors->member=g_strdup(defaultcolor);           \
		}
		set_color("untranslated", untranslated, TABLE_UNTRANSLATED_COLOR);
		set_color("translated", translated, TABLE_TRANSLATED_COLOR);
		set_color("fuzzy", fuzzy, TABLE_FUZZY_COLOR);
#undef set_color
	}
	else
	{
		messages_table_colors->untranslated=g_strdup(TABLE_UNTRANSLATED_COLOR);
		messages_table_colors->fuzzy=g_strdup(TABLE_FUZZY_COLOR);
		messages_table_colors->translated=g_strdup(TABLE_TRANSLATED_COLOR);
	}

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
static void free_messages_table_colors()
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

void gtranslator_tree_size_allocate( 
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
 * Create the new messages table
 */
GtkWidget *gtranslator_messages_table_new(GtrPo *po)
{
  GtkWidget *tree;
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  GtkTreeStore *store;
  GtkTreeSelection *selection;

  store = gtk_tree_store_new (
		N_COLUMNS,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_POINTER,
		GDK_TYPE_COLOR);
  
  tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(tree), TRUE);

  g_object_unref (G_OBJECT (store));

  /*
   * pv, this for managing column widths
   */
  g_signal_connect(G_OBJECT(tree), "size-allocate",
		G_CALLBACK(gtranslator_tree_size_allocate), NULL );

  /*
   * Add the original msgid column with the color defs attached to it.
   */
  renderer=gtk_cell_renderer_text_new();
  column=gtk_tree_view_column_new_with_attributes(_("Original"), renderer,
	"text", ORIGINAL_COLUMN, "foreground-gdk", COLOR_COLUMN, NULL);

  gtk_tree_view_column_set_sort_column_id (column, ORIGINAL_COLUMN);
  gtk_tree_view_column_set_resizable(column, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW(tree), column);

  /*
   * The same now again for the translation - msgid.
   */
  renderer=gtk_cell_renderer_text_new();
  column=gtk_tree_view_column_new_with_attributes(_("Translation"), renderer,
	"text", TRANSLATION_COLUMN, "foreground-gdk", COLOR_COLUMN, NULL);

  gtk_tree_view_column_set_sort_column_id (column, TRANSLATION_COLUMN);
  gtk_tree_view_column_set_resizable(column, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW(tree), column);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(tree));
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
  g_signal_connect (G_OBJECT(selection), "changed", 
		    G_CALLBACK(gtranslator_messages_table_selection_changed), 
		    NULL);

	GList *list;
	GtrMsg *msg;
	const char *msgid, *msgstr;

	g_return_if_fail(po != NULL);
	g_assert(messages_table == NULL);

	list = po->messages;

	read_messages_table_colors();

	messages_table=g_new0(GtrMessagesTable, 1);

	gtk_tree_store_append (store, &messages_table->untranslated_node, NULL);
	gtk_tree_store_set (store, &messages_table->untranslated_node, 
		      ORIGINAL_COLUMN, _("Untranslated"), 
			  TRANSLATION_COLUMN, "",
		      MSG_PTR_COLUMN, NULL,
		      COLOR_COLUMN, &messages_table_colors->untranslated_color,
		      -1);

	gtk_tree_store_append (store, &messages_table->fuzzy_node, NULL);
	gtk_tree_store_set (store, &messages_table->fuzzy_node, 
		      ORIGINAL_COLUMN, _("Fuzzy"), 
			  TRANSLATION_COLUMN, "",
		      MSG_PTR_COLUMN, NULL,
		      COLOR_COLUMN, &messages_table_colors->fuzzy_color,
		      -1);

	gtk_tree_store_append (store, &messages_table->translated_node, NULL);
	gtk_tree_store_set (store, &messages_table->translated_node, 
		      ORIGINAL_COLUMN, _("Translated"), 
			  TRANSLATION_COLUMN, "",
		      MSG_PTR_COLUMN, NULL,
		      COLOR_COLUMN, &messages_table_colors->translated_color,
		      -1);

	while (list) {
		msg = GTR_MSG(list->data);
		msgid = po_message_msgstr(msg->message);
		msgstr = po_message_msgstr(msg->message);
		if(msg->is_fuzzy) {
			gtk_tree_store_append(store, &msg->iter,
				&messages_table->fuzzy_node);
			gtk_tree_store_set(store, &msg->iter, ORIGINAL_COLUMN,
				msgid, TRANSLATION_COLUMN,
				msgstr, MSG_PTR_COLUMN,
				msg, COLOR_COLUMN,
				&messages_table_colors->fuzzy_color, -1);
		}
		else if(msgstr[0] != '\0') {
			gtk_tree_store_append(store, &msg->iter,
				&messages_table->translated_node);
			gtk_tree_store_set(store, &msg->iter, ORIGINAL_COLUMN,
				msgid, TRANSLATION_COLUMN,
				msgstr, MSG_PTR_COLUMN, msg,
				COLOR_COLUMN,
				&messages_table_colors->translated_color, -1);
		}
		else {
			gtk_tree_store_append(store, &msg->iter,
				&messages_table->untranslated_node);
			gtk_tree_store_set(store, &msg->iter, ORIGINAL_COLUMN,
				msgid, TRANSLATION_COLUMN,
				msgstr, MSG_PTR_COLUMN, msg,
				COLOR_COLUMN,
				&messages_table_colors->untranslated_color, -1);
		}

		list = g_list_next(list);
	}

	if(GtrPreferences.collapse_all) {
		gtk_tree_view_collapse_all(GTK_TREE_VIEW(tree));
	}
	else {
		gtk_tree_view_expand_all(GTK_TREE_VIEW(tree));
	}
	
  return tree;


}

/*
 * Update the data in a single row
 */
void gtranslator_messages_table_update_row(GtrPage *page, GtrMsg *msg)
{
	GtkTreeStore *model;
	const char *msgid, *msgstr;

	g_return_if_fail(page->po != NULL);
	g_return_if_fail(page->messages_tree != NULL);

	model = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(page->messages_tree)));

	msgid = po_message_msgid(msg->message);
	msgstr = po_message_msgstr(msg->message);
	if(msg->is_fuzzy) {
		gtk_tree_store_append(model, &msg->iter, &messages_table->fuzzy_node);
		gtk_tree_store_set(model, &msg->iter, ORIGINAL_COLUMN,
			msgid, TRANSLATION_COLUMN, msgstr, MSG_PTR_COLUMN, msg,
			COLOR_COLUMN, messages_table_colors->fuzzy, -1);
	}
	else if(msgstr[0] != '\0') {
		gtk_tree_store_append(model, &msg->iter,
			&messages_table->translated_node);
		gtk_tree_store_set(model, &msg->iter, ORIGINAL_COLUMN, msgid,
			 TRANSLATION_COLUMN, msgstr, MSG_PTR_COLUMN, msg,
			 COLOR_COLUMN, messages_table_colors->translated, -1);
	}
	else {
		gtk_tree_store_append(model, &msg->iter,
			&messages_table->untranslated_node);
		gtk_tree_store_set(model, &msg->iter, ORIGINAL_COLUMN,
			msgid, TRANSLATION_COLUMN, msgstr, MSG_PTR_COLUMN, msg, 
			COLOR_COLUMN, messages_table_colors->untranslated, -1);
	}
}

/*
 * Select given message
 */
void gtranslator_messages_table_select_row(GtrPage *page, GtrMsg *msg)
{
	GtkTreeSelection	*selection=NULL;

	g_return_if_fail(page->messages_tree != NULL);

	selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(page->messages_tree));
	gtk_tree_selection_select_iter(selection, &msg->iter);
}

static void 
gtranslator_messages_table_selection_changed(GtrPage *page, GtkTreeSelection *selection,
					     gpointer data)
{
  GtkTreeIter iter;
  GtkTreeModel* model;
  GtrMsg* msg;

  if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE) {
    gtk_tree_model_get(model, &iter, MSG_PTR_COLUMN, &msg, -1);
    if (msg != NULL)
      gtranslator_message_go_to(g_list_find(page->po->messages, msg));
  }

}

/*
 * Finish with messages table
 */
void gtranslator_message_table_free(GtkWidget *messages_table) {
	gtk_widget_destroy(messages_table);
	free_messages_table_colors();
}
