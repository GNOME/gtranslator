/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *		Kevin Vandersloot <kfv101@psu.edu>
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

#include "defines.include"
#include "gui.h"
#include "messages-table.h"
#include "messages.h"
#include "message.h"
#include "parse.h"
#include "preferences.h"
#include "utils.h"
#include "utf8.h"

#include <gal/e-paned/e-paned.h>

#include <gal/e-table/e-cell-combo.h>
#include <gal/e-table/e-cell-number.h>
#include <gal/e-table/e-cell-text.h>
#include <gal/e-table/e-table-extras.h>
#include <gal/e-table/e-tree-model.h>
#include <gal/e-table/e-tree-memory.h>
#include <gal/e-table/e-tree-scrolled.h>
#include <gal/e-table/e-tree-memory-callbacks.h>

/*
 * Create the ETableExtras for our messages table.
 */
static ETableExtras *table_extras_new(void);

/*
 * Internal ETableModel functions: the names should be self-explanatory.
 *
 * Prototypes:
 */
static GdkPixbuf *icon_at_function (ETreeModel *model, ETreePath path, void *data); 
static gint column_count_function(ETreeModel *model, void *data);

static gboolean is_cell_editable_function(ETreeModel *model, ETreePath path, 
	int column, void *data);
static gboolean is_empty_function(ETreeModel *model, int column, 
	const void *value, void *data);

static void free_value_function(ETreeModel *model, int column, 
	void *value, void *data);
static void set_value_at_function(ETreeModel *model, ETreePath path, int col, 
	const void *value, void *data);

static void *duplicate_value_function(ETreeModel *model, int column, 
	const void *value, void *data);
static void *initialize_value_function(ETreeModel *model, int column, void *data);
static void *value_at_function(ETreeModel *model, ETreePath path, int column, 
	void *data);

static gchar *return_string_for_value_function(ETreeModel *model, int column,
	const void *value, void *data);
	
/*
 * Global variables
 */
GtkWidget *tree;
ETreeModel *tree_model;
ETreeMemory *tree_memory;
ETreePath root_node = NULL;

/*
 * hash table to associate an ETreePath with each message. Used
 * in update_row to dtermine the node given a message that has been
 * updated
 */
GHashTable *hash_table=NULL;	


/* 
 * Functions:
 */
static GdkPixbuf *
icon_at_function (ETreeModel *model, ETreePath path, void *data)
{
	return NULL;
}

static gint column_count_function(ETreeModel *model, void *data)
{
	return 6;
}

static gboolean is_cell_editable_function(ETreeModel *model, ETreePath path, 
	int column, void *data)
{
	return FALSE;
}

static gboolean is_empty_function(ETreeModel *model, int column, 
	const void *value, void *data)
{
	switch (column) {
	case COL_NUMBER:
		return value == NULL;
	case COL_LINE:
		return value == NULL;
	case COL_ORIGINAL:
	case COL_TRANSLATION:
	case COL_COMMENT:
	case COL_STATUS:
		return !(value && *(char *)value);
	default:
		g_assert_not_reached ();
		return FALSE;
	}
}

static void free_value_function(ETreeModel *model, int column, 
	void *value, void *data)
{
	switch (column) {
	case COL_ORIGINAL:
	case COL_TRANSLATION:
	case COL_COMMENT:
	case COL_STATUS:
		if (value)
			GTR_FREE (value);
		break;
	case COL_NUMBER:
		break;
	case COL_LINE:
		break;
	default:
		g_assert_not_reached ();
	}
}

static void set_value_at_function(ETreeModel *model, ETreePath path, int col, 
	const void *value, void *data)
{
}

static void *duplicate_value_function(ETreeModel *model, int column, 
	const void *value, void *data)
{
	switch (column) {
	case COL_ORIGINAL:
	case COL_TRANSLATION:
	case COL_COMMENT:
	case COL_STATUS:
		return g_strdup (value);
		break;
	case COL_NUMBER:
		return (void *) value;
		break;
	case COL_LINE:
		return (void *) value;
		break;
	default:
		g_assert_not_reached ();
		return NULL;
	}
}

static void *initialize_value_function(ETreeModel *model, int column, void *data)
{
	switch (column) {
	case COL_ORIGINAL:
	case COL_TRANSLATION:
	case COL_COMMENT:
	case COL_STATUS:
		return g_strdup ("");
		break;
	case COL_NUMBER:
		return NULL;
		break;
	case COL_LINE:
		return NULL;
		break;
	default:
		g_assert_not_reached ();
		return NULL;
	}
}

static void *value_at_function(ETreeModel *model, ETreePath path, int column, 
	void *data)
{
	GtrMsg *message;
	
	message = e_tree_memory_node_get_data (tree_memory, path);
	g_return_val_if_fail(message!=NULL, NULL);
	g_return_val_if_fail(file_opened==TRUE, NULL);
	
	switch (column) {
	case COL_ORIGINAL:
		return message->msgid;
		break;
	case COL_TRANSLATION:
		if(message->msgstr)
		{
			return gtranslator_utf8_get_utf8_string(&message->msgstr);
		}
		else
		{
			return "";
		}
		break;
	case COL_COMMENT:
		return message->comment->pure_comment;
		break;
	case COL_STATUS:
		switch(message->status) {
		case GTR_MSG_STATUS_UNKNOWN:
			return _("Untranslated");
			break;
		case GTR_MSG_STATUS_TRANSLATED:
			return _("Translated");
			break;
		case GTR_MSG_STATUS_FUZZY:
			return _("Fuzzy");
			break;
		case GTR_MSG_STATUS_STICK:
			return _("Sticky");
			break;
		default:
			return NULL;
		}		
		break;
	case COL_NUMBER:
		return GINT_TO_POINTER(message->no);
		break;
	case COL_LINE:
		return GINT_TO_POINTER(message->pos);
		break;
	default:
		g_assert_not_reached ();
		return NULL;
	}
}

static gchar *return_string_for_value_function(ETreeModel *model, int column,
	const void *value, void *data)
{
	switch (column) {
	case COL_ORIGINAL:
	case COL_TRANSLATION:
	case COL_COMMENT:
	case COL_STATUS:
		return g_strdup (value);
		break;
	case COL_NUMBER:
		return g_strdup_printf ("%d", (gint) value);
		break;
	case COL_LINE:
		return g_strdup_printf("%d", (gint) value);
		break;
	default:
		g_assert_not_reached ();
		return NULL;
	}
}

static void
row_selected (ETree *tree, int row, ETreePath node, gpointer data)
{
	GtrMsg *message;
	gint model_row;
	
	message=e_tree_memory_node_get_data (tree_memory, node);
	g_return_if_fail(message!=NULL);
	
	/*
	 * This sucks. Should use e_tree_view_to_model_row here
	 * but that seems to return the view row. Sigh.
	 */
	model_row=g_list_index(po->messages, message);
	
	if (model_row<0)
		return;
	gtranslator_message_go_to(g_list_nth(po->messages, model_row));
}

/*
 * Creates all the nice and nifty stuff for the ETable.
 */
static ETableExtras *table_extras_new()
{
	ETableExtras 	*extras;
	ECell		*cell;
	gint		count=0;

	gchar *list_parts[] =
	{
		"number",
		"line",
		"original",
		"translation",
		"comment",
		"status",
		NULL
	};

	extras=e_table_extras_new();
	
	/*
	 * Fill the table parts independently here -- just for easification
	 *  of the process, we do use this contructions instead of many calls.
	 */
	cell=e_cell_number_new(NULL, GTK_JUSTIFY_RIGHT);
	e_table_extras_add_cell(extras, list_parts[count], cell); 
	count++;

	cell=e_cell_number_new(NULL, GTK_JUSTIFY_RIGHT);
	e_table_extras_add_cell(extras, list_parts[count], cell);
	count++;
	
	while(list_parts[count]!=NULL)
	{
		cell=e_cell_text_new(NULL, GTK_JUSTIFY_LEFT);

		e_table_extras_add_cell(extras, list_parts[count], cell);
		count++;
	}
	
	return extras;
}

/*
 * Create the new ETable with all the stuff needed for
 *  gal/gtranslator.
 */
GtkWidget *gtranslator_messages_table_new()
{
	gchar		*statusfile;
	
	GtkWidget 	*messages_tree;

	ETableExtras 	*tree_extras;
		
	tree_extras=table_extras_new();
	
	tree_model=e_tree_memory_callbacks_new(
		icon_at_function,
		column_count_function,
		NULL,
		NULL,
		NULL,
		NULL,
		value_at_function,
		set_value_at_function,
		is_cell_editable_function,
		duplicate_value_function,
		free_value_function,
		initialize_value_function,
		is_empty_function,
		return_string_for_value_function,
		NULL);
		
	tree_memory=E_TREE_MEMORY(tree_model);

	statusfile=gtranslator_utils_get_messages_table_state_file_name();

	/* Calling gtk_widget_new here is to work around a bug in gal
	** where e_tree_scrolled_new_from_spec is broken */
	messages_tree=gtk_widget_new(e_tree_scrolled_get_type (),
        	"hadjustment", NULL,
        	"vadjustment", NULL,
        	NULL);
	
	messages_tree = GTK_WIDGET(
		e_tree_scrolled_construct_from_spec_file(
			E_TREE_SCROLLED(messages_tree),
			tree_model,
			tree_extras,
			ETSPECS_DIR "/messages-table.etspec", 
			statusfile)
		);

	GTR_FREE(statusfile);
	
	tree = GTK_WIDGET (e_tree_scrolled_get_tree (E_TREE_SCROLLED (messages_tree)));
	gtk_signal_connect(GTK_OBJECT(tree), "cursor_activated",
		GTK_SIGNAL_FUNC(row_selected), NULL);

	return messages_tree;
}

/*
 * Clear the table
 */
void gtranslator_messages_table_clear(void)
{
	if(root_node)
	{
		e_tree_memory_node_remove(tree_memory, root_node);
		root_node=NULL;
	}
	
	if(hash_table)
	{
		g_hash_table_destroy(hash_table);
		hash_table=NULL;
	}
}		

/*
 * Populate with the messages
 */
void gtranslator_messages_table_create (void)
{
	GList *list;
	gint i=0;
	
	if(!file_opened)
		return;
	
	list=po->messages;
	
	gtranslator_messages_table_clear();
	
	root_node = e_tree_memory_node_insert (tree_memory, NULL, 0, NULL);
	e_tree_root_node_set_visible (E_TREE(tree), FALSE);
	
	hash_table=g_hash_table_new(g_direct_hash, g_direct_equal);
	
	while(list)
	{
		GtrMsg *message=list->data;
		ETreePath *node;
		
		node=e_tree_memory_node_insert(tree_memory, root_node,
			i, message);
		g_hash_table_insert(hash_table, message,node); 
		list = g_list_next(list);
		i++;
	}	
}

/*
 * Update the data in a single row
 */
void gtranslator_messages_table_update_row(GtrMsg *message)
{
	ETreePath node=NULL;

	g_return_if_fail(message!=NULL);

	node=g_hash_table_lookup(hash_table, message);

	if(node)
	{
		e_tree_model_node_data_changed (tree_model, node);
	}
}

/*
 * Select given message
 */
void gtranslator_messages_table_select_row(GtrMsg *message)
{
	ETreePath node=NULL;
	
	g_return_if_fail(message!=NULL);
	
	node=g_hash_table_lookup(hash_table, message);

	if(node)
	{
		e_tree_set_cursor(E_TREE(tree), node);
	}
}

/*
 * Save the e-tree state
 */
void gtranslator_messages_table_save_state()
{
	gchar	*statusfilename;

	statusfilename=gtranslator_utils_get_messages_table_state_file_name();
	e_tree_save_state (E_TREE (tree), statusfilename);

	GTR_FREE(statusfilename);
}

/*
 * Apply a very similar mechanism for hide/show/toggle-ing the
 *  messages table.
 */
gboolean gtranslator_messages_table_show()
{
	gint position=-1;

	position=e_paned_get_position(E_PANED(content_pane));

	if(position<=0)
	{
		return FALSE;
	}
	else
	{
		gtranslator_config_init();
		gtranslator_config_set_bool("toggles/show_content_pane", FALSE);
		gtranslator_config_close();

		e_paned_set_position(E_PANED(content_pane), 0);
		return TRUE;
	}
}

gboolean gtranslator_messages_table_hide()
{
	gint position=-1;

	gtranslator_config_init();
	position=gtranslator_config_get_int("interface/table_pane_position");
	gtranslator_config_close();

	if(position<=0)
	{
		return FALSE;
	}
	else
	{
		e_paned_set_position(E_PANED(content_pane), position);

		gtranslator_config_init();
		gtranslator_config_set_bool("toggles/show_content_pane", TRUE);
		gtranslator_config_close();

		return TRUE;
	}
}

void gtranslator_messages_table_toggle()
{
	if(!gtranslator_messages_table_hide())
	{
		gtranslator_messages_table_show();
	}
}
