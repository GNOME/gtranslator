/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
 *			Kevin Vandersloot <kfv101@psu.edu>
 *			Thomas Ziehmer <thomas@gtranslator.org>
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

#include "defines.include"
#include "gui.h"
#include "learn.h"
#include "messages-table.h"
#include "messages.h"
#include "message.h"
#include "parse.h"
#include "preferences.h"
#include "prefs.h"
#include "runtime-config.h"
#include "utils.h"
#include "utf8.h"

#include <gdk/gdkkeysyms.h>

/*
 * We're now defining the highlighting colors once here at the top via macros.
 */
#define TABLE_FUZZY_COLOR "#ff0000"
#define TABLE_UNTRANSLATED_COLOR "#a7453e"
#define TABLE_TRANSLATED_COLOR NULL

enum
{
  STATUS_COLUMN,
  ORIGINAL_COLUMN,
  TRANSLATION_COLUMN,
  MSG_PTR_COLUMN,
  N_COLUMNS
};

/*
 * Create the ETableExtras for our messages table.
 */

/*
 * Simply sets up/frees the generally used messages table colors.
 */
//static void read_messages_table_colors(void);
//static void free_messages_table_colors(void);
static void 
gtranslator_messages_table_selection_changed(GtkTreeSelection *selection,
					     gpointer data);


/*
 * Own messages table color container.
 */
typedef struct
{
	gchar	*untranslated;
	gchar	*fuzzy;
	gchar	*translated;
} GtrMessagesTableColors;

/*
 * Another own structure for the translation retrieval.
 */
typedef struct
{
	GtrMsg		*message;
	gchar		*found_translation;
	gboolean	 replace;
} GtrTranslationRetrieval;

/*
 * A new kind of popup menu for our beloved messages table.
 */

/*
 * An own insertion callback for the messages table's popup menu to insert
 *  the found, fitting translation from the learn buffer.
 */
//static void insert_translation(GtkWidget *widget, gpointer insertion_kind);

/*
 * Global variables
 */
static GtkWidget *tree;

static GtrMessagesTableColors *messages_table_colors;

static GtrTranslationRetrieval *retrieval=NULL;

/*
 * Hash table to associate an ETreePath with each message. Used
 * in update_row to determine the node given a message that has been
 * updated
 */
static GHashTable *hash_table=NULL;

/*
 * Pops up on a right click in the messages table -- should show any found 
 *  translation from the learn buffer.
 */


/*
 * Create the new messages table
 */
GtkWidget *gtranslator_messages_table_new()
{
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  GtkTreeStore *store;
  GtkTreeSelection *selection;
  gint i;

  gchar *titles[][2] = {{_("Status"),      "text"},
			{_("Original"),    "text"},
			{_("Translation"), "text"}};
  
  store = gtk_tree_store_new (N_COLUMNS,
			      G_TYPE_STRING,
			      G_TYPE_STRING,
			      G_TYPE_STRING,
			      G_TYPE_POINTER);
  
  tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (tree), TRUE);
  gtk_tree_view_set_search_column (GTK_TREE_VIEW (tree),
				   STATUS_COLUMN);
  
  g_object_unref (G_OBJECT (store));

  //we stop at N_COLUMNS-1 because we don't want to display the GtrMsg* 
  //we store in the GtkTreeStore
  for (i=0; i < N_COLUMNS-1; i++) {
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (titles[i][0], renderer,
						       titles[i][1], i,
						       NULL);
    gtk_tree_view_column_set_sort_column_id (column, i);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
  }

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
  g_signal_connect (G_OBJECT(selection), "changed", 
		    G_CALLBACK(gtranslator_messages_table_selection_changed), 
		    NULL);

  return tree;
}

/*
 * Clear the table
 */
void gtranslator_messages_table_clear(void)
{
  gtk_tree_store_clear(GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree))));
}		

/*
 * Populate with the messages
 */
void gtranslator_messages_table_create (void)
{
  GList *list;
  gint i=0, j=0, k=0;

  GtkTreeStore *model; // where to get it from ????
  GtkTreeIter unknown_node, fuzzy_node, translated_node;
  GtkTreeIter cur_node;
  if(!file_opened)
    return;
  list=po->messages;

  model = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));

  gtk_tree_store_append (model, &unknown_node, NULL);
  gtk_tree_store_set (model, &unknown_node, 
		      STATUS_COLUMN, _("Untranslated"), 
		      MSG_PTR_COLUMN, NULL,
		      -1);

  gtk_tree_store_append (model, &fuzzy_node, NULL);
  gtk_tree_store_set (model, &fuzzy_node, 
		      STATUS_COLUMN, _("Fuzzy"), 
		      MSG_PTR_COLUMN, NULL,
		      -1);

  gtk_tree_store_append (model, &translated_node, NULL);
  gtk_tree_store_set (model, &translated_node, 
		      STATUS_COLUMN, _("Translated"), 
		      MSG_PTR_COLUMN, NULL,
		      -1);

  while (list) {
    GtrMsg *message=list->data;

    switch (message->status){
    case GTR_MSG_STATUS_UNKNOWN:
      gtk_tree_store_append(model, &cur_node, &unknown_node);
      gtk_tree_store_set(model, &cur_node,
			 STATUS_COLUMN, "",
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 MSG_PTR_COLUMN, message,
			 -1);
      i++;
      break;
    case GTR_MSG_STATUS_TRANSLATED:
      gtk_tree_store_append(model, &cur_node, &translated_node);
      gtk_tree_store_set(model, &cur_node,
			 STATUS_COLUMN, "",
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 MSG_PTR_COLUMN, message,
			 -1);
      j++;
      break;
    case GTR_MSG_STATUS_STICK:
      //      node=NULL;
      break;
    case GTR_MSG_STATUS_FUZZY:
    default:
      gtk_tree_store_append(model, &cur_node, &fuzzy_node);
      gtk_tree_store_set(model, &cur_node,
			 STATUS_COLUMN, "",
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 MSG_PTR_COLUMN, message,
			 -1);
      k++;
    }
      list = g_list_next(list);
  }
}

/*
 * Update the data in a single row
 */
void gtranslator_messages_table_update_row(GtrMsg *message)
{

}

/*
 * Select given message
 */
void gtranslator_messages_table_select_row(GtrMsg *message)
{

}

/*
 * Update the status grouping of a message
 */
void gtranslator_messages_table_update_message_status(GtrMsg *message)
{
	
}

/*
 * Save the e-tree state
 */
void gtranslator_messages_table_save_state()
{

}


static void 
gtranslator_messages_table_selection_changed(GtkTreeSelection *selection,
					     gpointer data)
{
  GtkTreeIter iter;
  GtkTreeModel* model;
  GtrMsg* message;

  if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE) {
    gtk_tree_model_get(model, &iter, MSG_PTR_COLUMN, &message, -1);
    if (message != NULL)
      gtranslator_message_go_to(g_list_find(po->messages, message));
  }

}
