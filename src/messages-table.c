/*
 * (C) 2001-2002 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
 *			Kevin Vandersloot <kfv101@psu.edu>
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

//#include <gal/widgets/e-unicode.h>

/*
 * We're now defining the highlighting colors once here at the top via macros.
 */
#define TABLE_FUZZY_COLOR "#ff0000"
#define TABLE_UNTRANSLATED_COLOR "#a7453e"
#define TABLE_TRANSLATED_COLOR NULL

enum
{
  STATUS_COLUMN,
  NUMBER_COLUMN,
  LINE_COLUMN,
  ORIGINAL_COLUMN,
  TRANSLATION_COLUMN,
  COMMENT_COLUMN,
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
GtkWidget *tree;

GtrMessagesTableColors *messages_table_colors;

GtrTranslationRetrieval *retrieval=NULL;

/*
 * Hash table to associate an ETreePath with each message. Used
 * in update_row to determine the node given a message that has been
 * updated
 */
GHashTable *hash_table=NULL;

/*
 * Pops up on a right click in the messages table -- should show any found 
 *  translation from the learn buffer.
 */


/*
 * Create the new ETable with all the stuff needed for
 *  gal/gtranslator.
 */
GtkWidget *gtranslator_messages_table_new()
{
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  GtkTreeStore *store;
  gint i;
  
  gchar *titles[][2] = {{_("Status"),      "text"},
			{_("Number"),      "text"},
			{_("Line"),        "text"},
			{_("Original"),    "text"},
			{_("Translation"), "text"},
			{_("Comment"),     "text"}};
  
  store = gtk_tree_store_new (N_COLUMNS,
			      G_TYPE_STRING,
			      G_TYPE_INT,
			      G_TYPE_INT,
			      G_TYPE_STRING,
			      G_TYPE_STRING,
			      G_TYPE_STRING);
  
  tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (tree), TRUE);
  gtk_tree_view_set_search_column (GTK_TREE_VIEW (tree),
				   NUMBER_COLUMN);
  
  g_object_unref (G_OBJECT (store));
  
  for (i=0; i < N_COLUMNS; i++) {
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (titles[i][0], renderer,
						       titles[i][1], i,
						       NULL);
    gtk_tree_view_column_set_sort_column_id (column, i);
    gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
  }
  
  return tree;
}

/*
 * Clear the table
 */
void gtranslator_messages_table_clear(void)
{

}		

/*
 * Populate with the messages
 */
void gtranslator_messages_table_create (void)
{
  GList *list;
  gint i=0, j=0, k=0;

  GtkTreeStore *model; // where to get it from ????
  GtkTreeIter root, unknown_node, fuzzy_node, translated_node;

  if(!file_opened)
    return;
  list=po->messages;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));

  gtk_tree_store_append (model, &root, NULL);

  gtk_tree_store_append (model, &unknown_node, &root);
  gtk_tree_store_set (model, &unknown_node, 
		      STATUS_COLUMN, _("unknown"), -1);
		      
  gtk_tree_store_append (model, &fuzzy_node, &root);
  gtk_tree_store_set (model, &unknown_node, 
		      STATUS_COLUMN, _("fuzzy"), -1);

  gtk_tree_store_append (model, &translated_node, &root);
  gtk_tree_store_set (model, &unknown_node, 
		      STATUS_COLUMN, _("translated"), -1);

  while (list) {
    GtrMsg *message=list->data;
    
    switch (message->status){
    case GTR_MSG_STATUS_UNKNOWN:
      gtk_tree_store_append(model, &unknown_node, &unknown_node);
      gtk_tree_store_set(model, &unknown_node,
			 STATUS_COLUMN, "",
			 NUMBER_COLUMN, i,
			 LINE_COLUMN, 42,
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 COMMENT_COLUMN, message->comment->pure_comment,
			 -1);
      i++;
      break;
    case GTR_MSG_STATUS_TRANSLATED:
      gtk_tree_store_append(model, &translated_node, &unknown_node);
      gtk_tree_store_set(model, &translated_node,
			 STATUS_COLUMN, "",
			 NUMBER_COLUMN, j,
			 LINE_COLUMN, 42,
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 COMMENT_COLUMN, message->comment->pure_comment,
			 -1);
      j++;
      break;
    case GTR_MSG_STATUS_STICK:
      //      node=NULL;
      break;
    case GTR_MSG_STATUS_FUZZY:
    default:
      gtk_tree_store_append(model, &fuzzy_node, &unknown_node);
      gtk_tree_store_set(model, &fuzzy_node,
			 STATUS_COLUMN, "",
			 NUMBER_COLUMN, k,
			 LINE_COLUMN, 42,
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 COMMENT_COLUMN, message->comment->pure_comment,
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
