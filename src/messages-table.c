/*
 * (C) 2001-2003 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Kevin Vandersloot <kfv101@psu.edu>
 *			Thomas Ziehmer <thomas@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 *			Ross Golder <ross@kabalak.net>
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
#ifdef UTF8_CODE
# include "utf8.h"
#endif

#include <gdk/gdkkeysyms.h>

/*
 * We're now defining the highlighting colors once here at the top via macros.
 */
#define TABLE_FUZZY_COLOR "#ff0000"
#define TABLE_UNTRANSLATED_COLOR "#a7453e"
#define TABLE_TRANSLATED_COLOR NULL

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
gtranslator_messages_table_selection_changed(GtkTreeSelection *selection,
					     gpointer data);


/*
 * Message category nodes container.
 */
typedef struct {
	GtkTreeIter unknown_node;
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

/*
 * Global variables
 */
static GtkWidget *tree;

static GtrMessagesTable *messages_table;

static GtrMessagesTableColors *messages_table_colors;

/*
 * Initialize and set up the generally used messages table colors.
 */
static void read_messages_table_colors()
{
	GtkStyle *style=NULL;
	
	messages_table_colors=g_new0(GtrMessagesTableColors, 1);

	/*
	 * Read the values from the prefs in -- but only if desired.
	 */
	if(GtrPreferences.use_own_mt_colors)
	{
		gchar	*value;
		
		value=gtranslator_config_get_string("colors/messages_table_untranslated");

		if(value && value[0]=='#')
		{
			messages_table_colors->untranslated=g_strdup(value);
			GTR_FREE(value);
		}
		else
		{
			messages_table_colors->untranslated=g_strdup(TABLE_UNTRANSLATED_COLOR);
		}

		value=gtranslator_config_get_string("colors/messages_table_fuzzy");

		if(value && value[0]=='#')
		{
			messages_table_colors->fuzzy=g_strdup(value);
			GTR_FREE(value);
		}
		else
		{
			messages_table_colors->fuzzy=g_strdup(TABLE_FUZZY_COLOR);
		}

		value=gtranslator_config_get_string("colors/messages_table_translated");

		if(value && value[0]=='#')
		{
			messages_table_colors->translated=g_strdup(value);
			GTR_FREE(value);
		}
		else
		{
			messages_table_colors->translated=TABLE_TRANSLATED_COLOR;
		}
	}
	else
	{
		messages_table_colors->untranslated=g_strdup(TABLE_UNTRANSLATED_COLOR);
		messages_table_colors->fuzzy=g_strdup(TABLE_FUZZY_COLOR);
		messages_table_colors->translated=TABLE_TRANSLATED_COLOR;
	}

	/*
	 * Now parse our defined color strings into the GdkColor structs of our messages table colors.
	 */
	gdk_color_parse(messages_table_colors->untranslated, &messages_table_colors->untranslated_color);
	gdk_color_parse(messages_table_colors->fuzzy, &messages_table_colors->fuzzy_color);

	/* If no custom color is set, use the default style color. */
	if (messages_table_colors->translated == NULL) {
		style=gtk_widget_get_style(GTK_WIDGET(trans_box));
		messages_table_colors->translated_color=style->text[GTK_STATE_NORMAL];
	} else {
		gdk_color_parse(messages_table_colors->translated, &messages_table_colors->translated_color);
	}
}

/*
 * Frees the internally used GtrMessagesTableColors structure.
 */
static void free_messages_table_colors()
{
	if(messages_table_colors)
	{
		GTR_FREE(messages_table_colors->untranslated);
		GTR_FREE(messages_table_colors->fuzzy);
		GTR_FREE(messages_table_colors->translated);

		GTR_FREE(messages_table_colors);
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
GtkWidget *gtranslator_messages_table_new()
{
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
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (tree), TRUE);

  g_object_unref (G_OBJECT (store));

  /*
   * pv, this for managing column widths
   */
  g_signal_connect( G_OBJECT( tree ), "size-allocate",
		G_CALLBACK( gtranslator_tree_size_allocate ), NULL );

  /*
   * Add the original msgid column with the color defs attached to it.
   */
  renderer=gtk_cell_renderer_text_new();
  column=gtk_tree_view_column_new_with_attributes(_("Original"), renderer,
	"text", ORIGINAL_COLUMN, "foreground-gdk", COLOR_COLUMN, NULL);

  gtk_tree_view_column_set_sort_column_id (column, ORIGINAL_COLUMN);
  gtk_tree_view_column_set_resizable(column, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

  /*
   * The same now again for the translation - msgid.
   */
  renderer=gtk_cell_renderer_text_new();
  column=gtk_tree_view_column_new_with_attributes(_("Translation"), renderer,
	"text", TRANSLATION_COLUMN, "foreground-gdk", COLOR_COLUMN, NULL);

  gtk_tree_view_column_set_sort_column_id (column, TRANSLATION_COLUMN);
  gtk_tree_view_column_set_resizable(column, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

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
  free_messages_table_colors();

  po->table_already_created=FALSE;
}		

/*
 * Populate with the messages
 */
void gtranslator_messages_table_create (void)
{
  GList *list;
  gint i=0, j=0, k=0;

  GtkTreeStore *model;

  if(!file_opened || po->table_already_created)
  {
	return;
  }

  list=po->messages;

  model = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));

  read_messages_table_colors();

  messages_table=g_new0(GtrMessagesTable, 1);

  gtk_tree_store_append (model, &messages_table->unknown_node, NULL);
  gtk_tree_store_set (model, &messages_table->unknown_node, 
		      ORIGINAL_COLUMN, _("Untranslated"), 
			  TRANSLATION_COLUMN, "",
		      MSG_PTR_COLUMN, NULL,
		      COLOR_COLUMN, &messages_table_colors->untranslated_color,
		      -1);


  gtk_tree_store_append (model, &messages_table->fuzzy_node, NULL);
  gtk_tree_store_set (model, &messages_table->fuzzy_node, 
		      ORIGINAL_COLUMN, _("Fuzzy"), 
			  TRANSLATION_COLUMN, "",
		      MSG_PTR_COLUMN, NULL,
		      COLOR_COLUMN, &messages_table_colors->fuzzy_color,
		      -1);

  gtk_tree_store_append (model, &messages_table->translated_node, NULL);
  gtk_tree_store_set (model, &messages_table->translated_node, 
		      ORIGINAL_COLUMN, _("Translated"), 
			  TRANSLATION_COLUMN, "",
		      MSG_PTR_COLUMN, NULL,
		      COLOR_COLUMN, &messages_table_colors->translated_color,
		      -1);

  while (list) {
    GtrMsg *message=list->data;

    switch (message->status){
    case GTR_MSG_STATUS_UNKNOWN:
      gtk_tree_store_append(model, &message->iter, &messages_table->unknown_node);
      gtk_tree_store_set(model, &message->iter,
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 MSG_PTR_COLUMN, message,
			 COLOR_COLUMN, &messages_table_colors->untranslated_color,
			 -1);
      i++;
      break;
    case GTR_MSG_STATUS_TRANSLATED:
      gtk_tree_store_append(model, &message->iter, &messages_table->translated_node);
      gtk_tree_store_set(model, &message->iter,
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 MSG_PTR_COLUMN, message,
			 COLOR_COLUMN, &messages_table_colors->translated_color,
			 -1);
      j++;
      break;
    case GTR_MSG_STATUS_STICK:
      /*      node=NULL;*/
      break;
    case GTR_MSG_STATUS_FUZZY:
    default:
      gtk_tree_store_append(model, &message->iter, &messages_table->fuzzy_node);
      gtk_tree_store_set(model, &message->iter,
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 MSG_PTR_COLUMN, message,
			 COLOR_COLUMN, &messages_table_colors->fuzzy_color,
			 -1);
      k++;
    }
      list = g_list_next(list);
  }

  if(GtrPreferences.collapse_all)
  {
	gtk_tree_view_collapse_all(GTK_TREE_VIEW(tree));
  }
  else
  {
	gtk_tree_view_expand_all(GTK_TREE_VIEW(tree));
  }

  po->table_already_created=TRUE;
}

/*
 * Update the data in a single row
 */
void gtranslator_messages_table_update_row(GtrMsg *message)
{
  GtkTreeStore *model;
  if(!file_opened)
    return;

  model = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));

  if(!gtk_tree_store_remove(model, &message->iter)) {
	  /* Iter is no longer valid */
  }

    switch (message->status){
    case GTR_MSG_STATUS_UNKNOWN:
      gtk_tree_store_append(model, &message->iter, &messages_table->unknown_node);
      gtk_tree_store_set(model, &message->iter,
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 MSG_PTR_COLUMN, message,
			 COLOR_COLUMN, messages_table_colors->untranslated,
			 -1);
      break;
    case GTR_MSG_STATUS_TRANSLATED:
      gtk_tree_store_append(model, &message->iter, &messages_table->translated_node);
      gtk_tree_store_set(model, &message->iter,
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 MSG_PTR_COLUMN, message,
			 COLOR_COLUMN, messages_table_colors->translated,
			 -1);
      break;
    case GTR_MSG_STATUS_STICK:
      /*      node=NULL;*/
      break;
    case GTR_MSG_STATUS_FUZZY:
    default:
      gtk_tree_store_append(model, &message->iter, &messages_table->fuzzy_node);
      gtk_tree_store_set(model, &message->iter,
			 ORIGINAL_COLUMN, message->msgid,
			 TRANSLATION_COLUMN, message->msgstr,
			 MSG_PTR_COLUMN, message,
			 COLOR_COLUMN, messages_table_colors->fuzzy,
			 -1);
    }  
}

/*
 * Select given message
 */
void gtranslator_messages_table_select_row(GtrMsg *message)
{
	GtkTreeSelection	*selection=NULL;

	if(!file_opened)
	{
		return;
	}

	selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	gtk_tree_selection_select_iter(selection, &message->iter);
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
