/*
 * history-entry.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2006 - Paolo Borelli
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA.
 */
 
/*
 * Modified by the gtranslator Team, 2006. See the AUTHORS file for a 
 * list of people on the gtranslator Team.  
 * See the ChangeLog files for a list of changes. 
 *
 * $Id: history-entry.c 5931 2007-09-25 20:05:40Z pborelli $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gconf/gconf-client.h>

#include "history-entry.h"

enum {
	PROP_0,
	PROP_HISTORY_ID,
	PROP_HISTORY_LENGTH
};

#define MIN_ITEM_LEN 3

#define GTR_HISTORY_ENTRY_HISTORY_LENGTH_DEFAULT 10

#define GTR_HISTORY_ENTRY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), \
						GTR_TYPE_HISTORY_ENTRY, \
						GtranslatorHistoryEntryPrivate))

struct _GtranslatorHistoryEntryPrivate
{
	gchar              *history_id;
	guint               history_length;
	
	GtkEntryCompletion *completion;
	
	GConfClient        *gconf_client;
};

G_DEFINE_TYPE(GtranslatorHistoryEntry, gtranslator_history_entry, GTK_TYPE_COMBO_BOX_ENTRY)

static void
gtranslator_history_entry_set_property (GObject      *object,
				  guint         prop_id,
				  const GValue *value,
				  GParamSpec   *spec)
{
	GtranslatorHistoryEntry *entry;

	g_return_if_fail (GTR_IS_HISTORY_ENTRY (object));

	entry = GTR_HISTORY_ENTRY (object);

	switch (prop_id) {
	case PROP_HISTORY_ID:
		entry->priv->history_id = g_value_dup_string (value);
		break;
	case PROP_HISTORY_LENGTH:
		gtranslator_history_entry_set_history_length (entry,
						     g_value_get_uint (value));
		break;
	default:
		break;
	}
}

static void
gtranslator_history_entry_get_property (GObject    *object,
				  guint       prop_id,
				  GValue     *value,
				  GParamSpec *spec)
{
	GtranslatorHistoryEntryPrivate *priv;

	g_return_if_fail (GTR_IS_HISTORY_ENTRY (object));

	priv = GTR_HISTORY_ENTRY (object)->priv;

	switch (prop_id) {
	case PROP_HISTORY_ID:
		g_value_set_string (value, priv->history_id);
		break;
	case PROP_HISTORY_LENGTH:
		g_value_set_uint (value, priv->history_length);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, spec);
	}
}

static void
gtranslator_history_entry_destroy (GtkObject *object)
{
	gtranslator_history_entry_set_enable_completion (GTR_HISTORY_ENTRY (object),
						   FALSE);

	GTK_OBJECT_CLASS (gtranslator_history_entry_parent_class)->destroy (object);
}

static void
gtranslator_history_entry_finalize (GObject *object)
{
	GtranslatorHistoryEntryPrivate *priv;

	priv = GTR_HISTORY_ENTRY (object)->priv;
	
	g_free (priv->history_id);

	if (priv->gconf_client != NULL)
	{
		g_object_unref (G_OBJECT (priv->gconf_client));
		priv->gconf_client = NULL;
	}

	G_OBJECT_CLASS (gtranslator_history_entry_parent_class)->finalize (object);
}

static void 
gtranslator_history_entry_class_init (GtranslatorHistoryEntryClass *klass)
{
	GObjectClass   *object_class = G_OBJECT_CLASS (klass);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
	
	object_class->set_property = gtranslator_history_entry_set_property;
	object_class->get_property = gtranslator_history_entry_get_property;
	object_class->finalize = gtranslator_history_entry_finalize;
	gtkobject_class->destroy = gtranslator_history_entry_destroy;
	
	g_object_class_install_property (object_class,
					 PROP_HISTORY_ID,
					 g_param_spec_string ("history-id",
							      "History ID",
							      "History ID",
							      NULL,
							      G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
					 PROP_HISTORY_LENGTH,
					 g_param_spec_uint ("history-length",
							    "Max History Length",
							    "Max History Length",
							    0,
							    G_MAXUINT,
							    GTR_HISTORY_ENTRY_HISTORY_LENGTH_DEFAULT,
							    G_PARAM_READWRITE));

	/* TODO: Add enable-completion property */

	g_type_class_add_private (object_class, sizeof(GtranslatorHistoryEntryPrivate));
}

static GtkListStore *
get_history_store (GtranslatorHistoryEntry *entry)
{
	GtkTreeModel *store;

	store = gtk_combo_box_get_model (GTK_COMBO_BOX (entry));
	g_return_val_if_fail (GTK_IS_LIST_STORE (store), NULL);

	return (GtkListStore *) store;
}

static char *
get_history_key (GtranslatorHistoryEntry *entry)
{
	gchar *tmp;
	gchar *key;

	/*
	 * We store the data under /apps/gnome-settings/
	 * like the old GnomeEntry did. Maybe we should
	 * consider moving it to the /gtranslator GConf prefix...
	 * Or maybe we should just switch away from GConf.
	 */

	tmp = gconf_escape_key (entry->priv->history_id, -1);
	key = g_strconcat ("/apps/gnome-settings/",
			   "gtranslator",
			   "/history-",
			   tmp,
			   NULL);
	g_free (tmp);

	return key;
}

static GSList *
get_history_list (GtranslatorHistoryEntry *entry)
{
	GtkListStore *store;
	GtkTreeIter iter;
	gboolean valid;
	GSList *list = NULL;

	store = get_history_store (entry);

	valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store),
					       &iter);

	while (valid)
	{
		gchar *str;

		gtk_tree_model_get (GTK_TREE_MODEL (store),
				    &iter,
				    0, &str,
				    -1);

		list = g_slist_prepend (list, str);

		valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (store),
						  &iter);
	}

	return g_slist_reverse (list);
}

static void
gtranslator_history_entry_save_history (GtranslatorHistoryEntry *entry)
{
	GSList *gconf_items;
	gchar *key;

	g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));

	gconf_items = get_history_list (entry);
	key = get_history_key (entry);

	gconf_client_set_list (entry->priv->gconf_client,
			      key,
			      GCONF_VALUE_STRING,
			      gconf_items,
			      NULL);

	g_slist_free (gconf_items);
	g_free (key);
}

static gboolean
remove_item (GtkListStore *store,
	     const gchar  *text)
{
	GtkTreeIter iter;

	g_return_val_if_fail (text != NULL, FALSE);

	if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &iter))
		return FALSE;

	do
	{
		gchar *item_text;

		gtk_tree_model_get (GTK_TREE_MODEL (store),
				    &iter,
				    0,
				    &item_text,
				    -1);

		if (item_text != NULL &&
		    strcmp (item_text, text) == 0)
		{
			gtk_list_store_remove (store, &iter);
			return TRUE;
		}

	} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter));

	return FALSE;
}

static void
clamp_list_store (GtkListStore *store,
		  guint         max)
{
	GtkTreePath *path;
	GtkTreeIter iter;

	/* -1 because TreePath counts from 0 */
	path = gtk_tree_path_new_from_indices (max - 1, -1);

	if (gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &iter, path))
	{
		while (1)
		{
			if (!gtk_list_store_remove (store, &iter))
				break;
		}
	}

	gtk_tree_path_free (path);
}

static void
insert_history_item (GtranslatorHistoryEntry *entry,
		     const gchar       *text,
		     gboolean           prepend)
{
	GtkListStore *store;
	GtkTreeIter iter;

	if (g_utf8_strlen (text, -1) <= MIN_ITEM_LEN)
		return;
		
	store = get_history_store (entry);

	/* remove the text from the store if it was already
	 * present. If it wasn't, clamp to max history - 1
	 * before inserting the new row, otherwise appending
	 * would not work */

	if (!remove_item (store, text))
		clamp_list_store (store,
				  entry->priv->history_length - 1);

	if (prepend)
		gtk_list_store_insert (store, &iter, 0);
	else
		gtk_list_store_append (store, &iter);

	gtk_list_store_set (store,
			    &iter,
			    0,
			    text,
			    -1);

	gtranslator_history_entry_save_history (entry);
}

void
gtranslator_history_entry_prepend_text (GtranslatorHistoryEntry *entry,
				  const gchar       *text)
{
	g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));
	g_return_if_fail (text != NULL);

	insert_history_item (entry, text, TRUE);
}

void
gtranslator_history_entry_append_text (GtranslatorHistoryEntry *entry,
				 const gchar       *text)
{
	g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));
	g_return_if_fail (text != NULL);

	insert_history_item (entry, text, FALSE);
}

static void
gtranslator_history_entry_load_history (GtranslatorHistoryEntry *entry)
{
	GSList *gconf_items, *l;
	GtkListStore *store;
	GtkTreeIter iter;
	gchar *key;
	gint i;

	g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));

	store = get_history_store (entry);
	key = get_history_key (entry);

	gconf_items = gconf_client_get_list (entry->priv->gconf_client,
					     key,
					     GCONF_VALUE_STRING,
					     NULL);

	gtk_list_store_clear (store);

	for (l = gconf_items, i = 0;
	     l != NULL && i < entry->priv->history_length;
	     l = l->next, i++)
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, 
				    &iter,
				    0,
				    l->data,
				    -1);
	}

	g_free (key);
	g_slist_free (gconf_items);
}

void
gtranslator_history_entry_clear (GtranslatorHistoryEntry *entry)
{
	GtkListStore *store;

	g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));

	store = get_history_store (entry);
	gtk_list_store_clear (store);

	gtranslator_history_entry_save_history (entry);
}

static void
gtranslator_history_entry_init (GtranslatorHistoryEntry *entry)
{
	GtranslatorHistoryEntryPrivate *priv;

	priv = GTR_HISTORY_ENTRY_GET_PRIVATE (entry);
	entry->priv = priv;

	priv->history_id = NULL;
	priv->history_length = GTR_HISTORY_ENTRY_HISTORY_LENGTH_DEFAULT;

	priv->completion = NULL;
	
	priv->gconf_client = gconf_client_get_default ();
}

void
gtranslator_history_entry_set_history_length (GtranslatorHistoryEntry *entry,
					guint              history_length)
{
	g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));
	g_return_if_fail (history_length > 0);

	entry->priv->history_length = history_length;

	/* TODO: update if we currently have more items than max */
}

guint
gtranslator_history_entry_get_history_length (GtranslatorHistoryEntry *entry)
{
	g_return_val_if_fail (GTR_IS_HISTORY_ENTRY (entry), 0);

	return entry->priv->history_length;
}

gchar *
gtranslator_history_entry_get_history_id (GtranslatorHistoryEntry *entry)
{
	g_return_val_if_fail (GTR_IS_HISTORY_ENTRY (entry), NULL);

	return g_strdup (entry->priv->history_id);
}

void
gtranslator_history_entry_set_enable_completion (GtranslatorHistoryEntry *entry,
					   gboolean           enable)
{
	g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));
	
	if (enable)
	{
		if (entry->priv->completion != NULL)
			return;
		
		entry->priv->completion = gtk_entry_completion_new ();
		gtk_entry_completion_set_model (entry->priv->completion, 
						GTK_TREE_MODEL (get_history_store (entry)));
		
		/* Use model column 0 as the text column */
		gtk_entry_completion_set_text_column (entry->priv->completion, 0);

		gtk_entry_completion_set_minimum_key_length (entry->priv->completion,
							     MIN_ITEM_LEN);

		gtk_entry_completion_set_popup_completion (entry->priv->completion, FALSE);
		gtk_entry_completion_set_inline_completion (entry->priv->completion, TRUE);
	
		/* Assign the completion to the entry */
		gtk_entry_set_completion (GTK_ENTRY (gtranslator_history_entry_get_entry(entry)), 
					  entry->priv->completion);
	}
	else
	{
		if (entry->priv->completion == NULL)
			return;

		gtk_entry_set_completion (GTK_ENTRY (gtranslator_history_entry_get_entry (entry)), 
					  NULL);
		
		g_object_unref (entry->priv->completion);
		
		entry->priv->completion = NULL;
	}
}
							 
gboolean
gtranslator_history_entry_get_enable_completion (GtranslatorHistoryEntry *entry)
{
	g_return_val_if_fail (GTR_IS_HISTORY_ENTRY (entry), FALSE);
	
	return entry->priv->completion != NULL;
}

GtkWidget *
gtranslator_history_entry_new (const gchar *history_id,
			 gboolean     enable_completion)
{
	GtkWidget *ret;
	GtkListStore *store;

	g_return_val_if_fail (history_id != NULL, NULL);

	/* Note that we are setting the model, so
	 * user must be careful to always manipulate
	 * data in the history through gtranslator_history_entry_
	 * functions.
	 */

	store = gtk_list_store_new (1, G_TYPE_STRING);

	ret = g_object_new (GTR_TYPE_HISTORY_ENTRY,
			    "history-id", history_id,
	                    "model", store,
			    "text-column", 0,
	                    NULL);

	g_object_unref (store);

	/* loading has to happen after the model
	 * has been set. However the model is not a 
	 * G_PARAM_CONSTRUCT property of GtkComboBox
	 * so we cannot do this in the constructor.
	 * For now we simply do here since this widget is 
	 * not bound to other programming languages.
	 * A maybe better alternative is to override the 
	 * model property of combobox and mark CONTRUCT_ONLY.
	 * This would also ensure that the model cannot be
	 * set explicitely at a later time.
	 */
	gtranslator_history_entry_load_history (GTR_HISTORY_ENTRY (ret));

	gtranslator_history_entry_set_enable_completion (GTR_HISTORY_ENTRY (ret),
						   enable_completion);
						   
	return ret;
}

/*
 * Utility function to get the editable text entry internal widget.
 * I would prefer to not expose this implementation detail and 
 * simply make the GtranslatorHistoryEntry widget implement the 
 * GtkEditable interface. Unfortunately both GtkEditable and
 * GtkComboBox have a "changed" signal and I am not sure how to
 * handle the conflict.
 */
GtkWidget *
gtranslator_history_entry_get_entry (GtranslatorHistoryEntry *entry)
{
	g_return_val_if_fail (GTR_IS_HISTORY_ENTRY (entry), NULL);

	return gtk_bin_get_child (GTK_BIN (entry));
}

static void
escape_cell_data_func (GtkTreeViewColumn           *col,
		       GtkCellRenderer             *renderer,
		       GtkTreeModel                *model,
		       GtkTreeIter                 *iter,
		       GtranslatorHistoryEntryEscapeFunc  escape_func)
{
	gchar *str;
	gchar *escaped;

	gtk_tree_model_get (model, iter, 0, &str, -1);
	escaped = escape_func (str);
	g_object_set (renderer, "text", escaped, NULL);

	g_free (str);
	g_free (escaped);
}

void
gtranslator_history_entry_set_escape_func (GtranslatorHistoryEntry           *entry,
				     GtranslatorHistoryEntryEscapeFunc  escape_func)
{
	GList *cells;

	g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));

	cells = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (entry));

	/* We only have one cell renderer */
	g_return_if_fail (cells->data != NULL && cells->next == NULL);

	if (escape_func != NULL)
		gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (entry),
						    GTK_CELL_RENDERER (cells->data),
						    (GtkCellLayoutDataFunc) escape_cell_data_func,
						    escape_func,
						    NULL);
	else
		gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (entry),
						    GTK_CELL_RENDERER (cells->data),
						    NULL,
						    NULL,
						    NULL);

	g_list_free (cells);
}
