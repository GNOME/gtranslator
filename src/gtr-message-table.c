/*
 * Copyright (C) 2007   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *			Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Kevin Vandersloot <kfv101@psu.edu>
 *			Thomas Ziehmer <thomas@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
			Seán de Búrca <leftmostcat@gmail.com>
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-message-table.h"
#include "gtr-message-table-model.h"
#include "gtr-msg.h"
#include "gtr-po.h"
#include "gtr-tab.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define GTR_MESSAGE_TABLE_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE (	\
							(object),		\
							GTR_TYPE_MESSAGE_TABLE,	\
							GtrMessageTablePrivate))

G_DEFINE_TYPE (GtrMessageTable, gtr_message_table,
	       GTK_TYPE_VBOX)
     struct _GtrMessageTablePrivate
     {
       GtkWidget *treeview;
       GtrMessageTableModel *store;
       GtkTreeModel *sort_model;

       GtrTab *tab;
     };

static void
gtr_message_table_selection_changed (GtkTreeSelection *
						    selection,
						    GtrMessageTable *
						    table)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GList *msg = NULL;
  GList *current_msg = NULL;
  GtrPo *po;

  g_return_if_fail (selection != NULL);

  po = gtr_tab_get_po (table->priv->tab);
  current_msg = gtr_po_get_current_message (po);

  if (gtk_tree_selection_get_selected (selection, &model, &iter) == TRUE)
    {
      gtk_tree_model_get (model, &iter,
			  GTR_MESSAGE_TABLE_MODEL_POINTER_COLUMN, &msg, -1);

      if (msg != NULL
	  && g_utf8_collate (gtr_msg_get_msgid (msg->data),
			     gtr_msg_get_msgid (current_msg->data)))
	{
	  gtr_tab_message_go_to (table->priv->tab, msg,
					 FALSE, GTR_TAB_MOVE_NONE);
	}
    }
}

static void
showed_message_cb (GtrTab * tab,
		   GtrMsg * msg, GtrMessageTable * table)
{
  GtkTreePath *path;
  GtkTreeSelection *selection;
  GtkTreeIter iter;

  selection =
    gtk_tree_view_get_selection (GTK_TREE_VIEW (table->priv->treeview));

  path =
    gtk_tree_row_reference_get_path (gtr_msg_get_row_reference (msg));

  gtk_tree_model_get_iter (table->priv->sort_model, &iter, path);

  gtk_tree_selection_select_iter (selection, &iter);

  gtk_tree_path_next (path);

  gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (table->priv->treeview),
				path, NULL, FALSE, 0.0, 0.0);
  gtk_tree_path_free (path);
}

static void
message_changed_cb (GtrTab * tab,
		    GtrMsg * msg, GtrMessageTable * table)
{
  GtkTreePath *sort_path, *path;
  GtkTreeRowReference *row;

  row = gtr_msg_get_row_reference (msg);
  sort_path = gtk_tree_row_reference_get_path (row);

  path =
    gtk_tree_model_sort_convert_path_to_child_path (GTK_TREE_MODEL_SORT
						    (table->priv->sort_model),
						    sort_path);

  gtr_message_table_model_update_row (table->priv->store, path);
}

static gint
compare_by_status (gint a_status, gint b_status, gint a_pos, gint b_pos)
{
  if (a_status == b_status)
    return a_pos - b_pos;
  else
    return a_status - b_status;
}

static gint
model_compare_by_status (GtkTreeModel * model,
			 GtkTreeIter * a, GtkTreeIter * b, gpointer user_data)
{
  gint a_status, b_status, a_pos, b_pos;

  gtk_tree_model_get (model, a,
		      GTR_MESSAGE_TABLE_MODEL_STATUS_COLUMN,
		      &a_status,
		      GTR_MESSAGE_TABLE_MODEL_ID_COLUMN, &a_pos, -1);
  gtk_tree_model_get (model, b,
		      GTR_MESSAGE_TABLE_MODEL_STATUS_COLUMN,
		      &b_status,
		      GTR_MESSAGE_TABLE_MODEL_ID_COLUMN, &b_pos, -1);

  return compare_by_status (a_status, b_status, a_pos, b_pos);
}

static gint
list_compare_by_status (gconstpointer a, gconstpointer b)
{
  gint a_status, b_status, a_pos, b_pos;

  a_status = gtr_msg_get_status (GTR_MSG (a));
  b_status = gtr_msg_get_status (GTR_MSG (b));

  a_pos = gtr_msg_get_po_position (GTR_MSG (a));
  b_pos = gtr_msg_get_po_position (GTR_MSG (b));

  return compare_by_status (a_status, b_status, a_pos, b_pos);
}

static gint
list_compare_by_position (gconstpointer a, gconstpointer b)
{
  gint a_pos, b_pos;
  a_pos = gtr_msg_get_po_position (GTR_MSG (a));
  b_pos = gtr_msg_get_po_position (GTR_MSG (b));

  return a_pos - b_pos;
}

static gint
list_compare_by_original (gconstpointer a, gconstpointer b)
{
  const gchar *a_original, *b_original;

  a_original = gtr_msg_get_msgid (GTR_MSG (a));
  b_original = gtr_msg_get_msgid (GTR_MSG (b));

  return g_utf8_collate (a_original, b_original);
}

static gint
list_compare_by_translation (gconstpointer a, gconstpointer b)
{
  const gchar *a_translated, *b_translated;

  a_translated = gtr_msg_get_msgstr (GTR_MSG (a));
  b_translated = gtr_msg_get_msgstr (GTR_MSG (b));

  return g_utf8_collate (a_translated, b_translated);
}

static void
sort_message_list (GtkTreeViewColumn * column,
		   GtrMessageTable * table)
{
  GtrPo *po;
  GList *messages;
  gint sort_column;

  po = gtr_tab_get_po (table->priv->tab);
  messages = gtr_po_get_messages (po);

  sort_column = gtk_tree_view_column_get_sort_column_id (column);
  switch (sort_column)
    {
    case GTR_MESSAGE_TABLE_MODEL_ID_COLUMN:
      messages = g_list_sort (messages, list_compare_by_position);
      break;
    case GTR_MESSAGE_TABLE_MODEL_STATUS_COLUMN:
      messages = g_list_sort (messages, list_compare_by_status);
      break;
    case GTR_MESSAGE_TABLE_MODEL_ORIGINAL_COLUMN:
      messages = g_list_sort (messages, list_compare_by_original);
      break;
    case GTR_MESSAGE_TABLE_MODEL_TRANSLATION_COLUMN:
      messages = g_list_sort (messages, list_compare_by_translation);
      break;
    }

  if (gtk_tree_view_column_get_sort_order (column) == GTK_SORT_DESCENDING)
    messages = g_list_reverse (messages);

  gtr_po_set_messages (po, messages);

  gtr_tab_message_go_to (table->priv->tab,
				 g_list_first (messages),
				 FALSE, GTR_TAB_MOVE_NONE);
}

static void
gtr_message_table_draw (GtrMessageTable * table)
{
  GtrMessageTablePrivate *priv = table->priv;

  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  GtkTreeSelection *selection;

  priv->store = gtr_message_table_model_new ();

  priv->sort_model =
    gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (priv->store));

  priv->treeview = gtk_tree_view_new ();

  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (priv->sort_model),
					GTR_MESSAGE_TABLE_MODEL_ID_COLUMN,
					GTK_SORT_ASCENDING);

  gtk_tree_sortable_set_default_sort_func (GTK_TREE_SORTABLE
					   (priv->sort_model), NULL, NULL,
					   NULL);

  gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (priv->sort_model),
				   GTR_MESSAGE_TABLE_MODEL_STATUS_COLUMN,
				   model_compare_by_status, NULL, NULL);

  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (priv->treeview), TRUE);

  renderer = gtk_cell_renderer_pixbuf_new ();
  column = gtk_tree_view_column_new_with_attributes (_("Status"),
						     renderer,
						     "icon-name",
						     GTR_MESSAGE_TABLE_MODEL_ICON_COLUMN,
						     NULL);

  gtk_tree_view_column_set_sort_column_id (column,
					   GTR_MESSAGE_TABLE_MODEL_STATUS_COLUMN);
  gtk_tree_view_column_set_resizable (column, FALSE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), column);

  /* Resort underlying GList when column header clicked */
  g_signal_connect (G_OBJECT (column), "clicked",
		    G_CALLBACK (sort_message_list), table);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (_("ID"),
						     renderer,
						     "text",
						     GTR_MESSAGE_TABLE_MODEL_ID_COLUMN,
						     NULL);

  gtk_tree_view_column_set_sort_column_id (column,
					   GTR_MESSAGE_TABLE_MODEL_ID_COLUMN);
  gtk_tree_view_column_set_resizable (column, FALSE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), column);

  /* Resort underlying GList when column header clicked */
  g_signal_connect (G_OBJECT (column), "clicked",
		    G_CALLBACK (sort_message_list), table);

  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
  if (gtk_widget_get_default_direction () == GTK_TEXT_DIR_RTL)
    g_object_set (renderer, "xalign", 1.0, NULL);
  column = gtk_tree_view_column_new_with_attributes (_("Original Text"),
						     renderer,
						     "text",
						     GTR_MESSAGE_TABLE_MODEL_ORIGINAL_COLUMN,
						     NULL);

  gtk_tree_view_column_set_sort_column_id (column,
					   GTR_MESSAGE_TABLE_MODEL_ORIGINAL_COLUMN);
  gtk_tree_view_column_set_expand (column, TRUE);
  gtk_tree_view_column_set_resizable (column, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), column);

  /* Resort underlying GList when column header clicked */
  g_signal_connect (G_OBJECT (column), "clicked",
		    G_CALLBACK (sort_message_list), table);

  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer, "ellipsize", PANGO_ELLIPSIZE_END, NULL);

  column = gtk_tree_view_column_new_with_attributes (_("Translated Text"),
						     renderer,
						     "text",
						     GTR_MESSAGE_TABLE_MODEL_TRANSLATION_COLUMN,
						     NULL);

  gtk_tree_view_column_set_sort_column_id (column,
					   GTR_MESSAGE_TABLE_MODEL_TRANSLATION_COLUMN);
  gtk_tree_view_column_set_expand (column, TRUE);
  gtk_tree_view_column_set_resizable (column, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), column);

  /* Resort underlying GList when column header clicked */
  g_signal_connect (G_OBJECT (column), "clicked",
		    G_CALLBACK (sort_message_list), table);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);

  g_signal_connect (G_OBJECT (selection), "changed",
		    G_CALLBACK (gtr_message_table_selection_changed),
		    table);
}


static void
gtr_message_table_init (GtrMessageTable * table)
{
  GtkWidget *scrolledwindow;

  table->priv = GTR_MESSAGE_TABLE_GET_PRIVATE (table);

  gtr_message_table_draw (table);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow),
				  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start (GTK_BOX (table), scrolledwindow, TRUE, TRUE, 0);
  gtk_widget_show (scrolledwindow);

  gtk_container_add (GTK_CONTAINER (scrolledwindow), table->priv->treeview);
  gtk_widget_show (table->priv->treeview);
}

static void
gtr_message_table_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_message_table_parent_class)->finalize (object);
}

static void
gtr_message_table_class_init (GtrMessageTableClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrMessageTablePrivate));

  object_class->finalize = gtr_message_table_finalize;
}

/**
 * gtr_message_table_new:
 * @tab: a #GtrTab
 *
 * Creates a new #GtrMessageTable object.
 *
 * Returns: the newly created #GtrMessageTable
 */
GtkWidget *
gtr_message_table_new (GtkWidget * tab)
{
  GtrMessageTable *obj =
    GTR_MESSAGE_TABLE (g_object_new (GTR_TYPE_MESSAGE_TABLE, NULL));
  obj->priv->tab = GTR_TAB (tab);
  g_signal_connect (obj->priv->tab,
		    "showed-message", G_CALLBACK (showed_message_cb), obj);
  g_signal_connect (obj->priv->tab,
		    "message-changed", G_CALLBACK (message_changed_cb), obj);

  return GTK_WIDGET (obj);
}

/**
 * gtr_message_table_populate:
 * @table: a #GtrMessageTable
 * @messages: a list of #GtrMsg
 *
 * Populates the #GtrMessageTable with the list of #GtrMsg and
 * sort them.
 */
void
gtr_message_table_populate (GtrMessageTable * table,
				    GList * messages)
{
  GtkTreeIter iter, sort_iter;
  GtkTreePath *path;
  GtkTreeRowReference *row;

  g_return_if_fail (table != NULL);
  g_return_if_fail (messages != NULL);

  while (messages)
    {
      gtr_message_table_model_append (table->priv->store,
					      messages->data, &iter);

      gtk_tree_model_sort_convert_child_iter_to_iter (GTK_TREE_MODEL_SORT
						      (table->priv->
						       sort_model),
						      &sort_iter, &iter);
      path = gtk_tree_model_get_path (table->priv->sort_model, &sort_iter);
      row = gtk_tree_row_reference_new (table->priv->sort_model, path);
      gtk_tree_path_free (path);

      gtr_msg_set_row_reference (GTR_MSG (messages->data), row);

      messages = g_list_next (messages);
    }
  /*
   * It is much faster set the model after list population
   */
  gtk_tree_view_set_model (GTK_TREE_VIEW (table->priv->treeview),
			   table->priv->sort_model);
}
