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
#include "gtr-debug.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

typedef struct
{
  GtkWidget *treeview;
  GtrMessageTableModel *store;
  GtkTreeModel *sort_model;

  GtrTab *tab;
} GtrMessageTablePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrMessageTable, gtr_message_table, GTK_TYPE_BOX)

static void
showed_message_cb (GtrTab * tab, GtrMsg * msg, GtrMessageTable * table)
{
  GtkTreePath *path;
  GtkTreeSelection *selection;
  GtkTreeIter iter, child_iter;
  GtrMessageTablePrivate *priv;

  priv = gtr_message_table_get_instance_private (table);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
  gtr_message_table_get_message_iter (priv->store, msg, &child_iter);

  gtk_tree_model_sort_convert_child_iter_to_iter (GTK_TREE_MODEL_SORT
                                                  (priv->sort_model),
                                                  &iter, &child_iter);

  gtk_tree_selection_select_iter (selection, &iter);
  path = gtk_tree_model_get_path (priv->sort_model, &iter);

  gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (priv->treeview),
                                path, NULL, TRUE, 0.5, 0.0);

  gtk_tree_path_free (path);
}

static void
gtr_message_table_selection_changed (GtkTreeSelection *selection,
                                     GtrMessageTable *table)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtrMsg *msg;
  GList *current_msg = NULL;
  GtrMessageTablePrivate *priv;
  GtrPo *po;

  g_return_if_fail (selection != NULL);

  priv = gtr_message_table_get_instance_private (table);

  po = gtr_tab_get_po (priv->tab);
  current_msg = gtr_po_get_current_message (po);

  if (gtk_tree_selection_get_selected (selection, &model, &iter) == TRUE)
    {
      gtk_tree_model_get (model, &iter,
                          GTR_MESSAGE_TABLE_MODEL_POINTER_COLUMN, &msg, -1);

      if (msg != NULL
          && g_utf8_collate (gtr_msg_get_msgid (msg),
                             gtr_msg_get_msgid (current_msg->data)))
        {
          g_signal_handlers_block_by_func (priv->tab, showed_message_cb, table);
          gtr_tab_message_go_to (priv->tab, msg,
                                 FALSE, GTR_TAB_MOVE_NONE);
          g_signal_handlers_unblock_by_func (priv->tab, showed_message_cb, table);
        }
    }
}

static void
message_changed_cb (GtrTab * tab, GtrMsg * msg, GtrMessageTable * table)
{
  GtkTreePath *path;
  GtkTreeIter iter;
  GtrMessageTablePrivate *priv;

  priv = gtr_message_table_get_instance_private (table);

  if (!gtr_message_table_get_message_iter (GTR_MESSAGE_TABLE_MODEL (priv->store),
                                           msg, &iter))
    return;

  path = gtk_tree_model_get_path (GTK_TREE_MODEL (priv->store), &iter);
  gtr_message_table_model_update_row (GTR_MESSAGE_TABLE_MODEL (priv->store),
                                      path);
  gtk_tree_path_free (path);
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

static void
gtr_message_table_init (GtrMessageTable * table)
{
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  GtkTreeSelection *selection;
  GtrMessageTablePrivate *priv;

  priv = gtr_message_table_get_instance_private (table);

  gtk_orientable_set_orientation (GTK_ORIENTABLE (table),
                                  GTK_ORIENTATION_VERTICAL);

  gtk_widget_init_template (GTK_WIDGET (table));

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

  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
  if (gtk_widget_get_default_direction () == GTK_TEXT_DIR_RTL)
    g_object_set (renderer, "xalign", 1.0, NULL);
  column = gtk_tree_view_column_new_with_attributes (_("Original Message"),
                                                     renderer,
                                                     "text",
                                                     GTR_MESSAGE_TABLE_MODEL_ORIGINAL_COLUMN,
                                                     NULL);

  gtk_tree_view_column_set_sort_column_id (column,
                                           GTR_MESSAGE_TABLE_MODEL_ORIGINAL_COLUMN);
  gtk_tree_view_column_set_expand (column, TRUE);
  gtk_tree_view_column_set_resizable (column, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), column);

  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer, "ellipsize", PANGO_ELLIPSIZE_END, NULL);

  column = gtk_tree_view_column_new_with_attributes (_("Translated Message"),
                                                     renderer,
                                                     "text",
                                                     GTR_MESSAGE_TABLE_MODEL_TRANSLATION_COLUMN,
                                                     NULL);

  gtk_tree_view_column_set_sort_column_id (column,
                                           GTR_MESSAGE_TABLE_MODEL_TRANSLATION_COLUMN);
  gtk_tree_view_column_set_expand (column, TRUE);
  gtk_tree_view_column_set_resizable (column, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), column);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);

  g_signal_connect (G_OBJECT (selection), "changed",
                    G_CALLBACK (gtr_message_table_selection_changed), table);
}

static void
gtr_message_table_finalize (GObject * object)
{
  DEBUG_PRINT ("Finalize message table");

  G_OBJECT_CLASS (gtr_message_table_parent_class)->finalize (object);
}

static void
gtr_message_table_class_init (GtrMessageTableClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_message_table_finalize;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/gtranslator/ui/gtr-message-table.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrMessageTable, treeview);
}

/**
 * gtr_message_table_new:
 *
 * Creates a new #GtrMessageTable object.
 *
 * Returns: the newly created #GtrMessageTable
 */
GtkWidget *
gtr_message_table_new (void)
{
  GtrMessageTable *obj =
    GTR_MESSAGE_TABLE (g_object_new (GTR_TYPE_MESSAGE_TABLE, NULL));
  GtrMessageTablePrivate *priv;

  priv = gtr_message_table_get_instance_private (obj);

  g_signal_connect (priv->tab,
                    "showed-message", G_CALLBACK (showed_message_cb), obj);
  g_signal_connect (priv->tab,
                    "message-changed", G_CALLBACK (message_changed_cb), obj);

  return GTK_WIDGET (obj);
}

void
gtr_message_table_set_tab (GtrMessageTable *table, GtrTab *tab)
{
  GtrMessageTablePrivate *priv;

  priv = gtr_message_table_get_instance_private (table);
  priv->tab = tab;
}

/**
 * gtr_message_table_populate:
 * @table: a #GtrMessageTable
 * @container: a list of #GtrMsg
 *
 * Populates the #GtrMessageTable with the list of #GtrMsg and
 * sort them.
 */
void
gtr_message_table_populate (GtrMessageTable * table, GtrMessageContainer * container)
{
  GtrMessageTablePrivate *priv;

  g_return_if_fail (table != NULL);
  g_return_if_fail (container != NULL);

  priv = gtr_message_table_get_instance_private (table);

  if (priv->store)
    {
      gtk_tree_view_set_model (GTK_TREE_VIEW (priv->treeview), NULL);
      g_object_unref (priv->sort_model);
      g_object_unref (priv->store);
    }

  priv->store = gtr_message_table_model_new (container);
  priv->sort_model =
    gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (priv->store));

  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE
                                        (priv->sort_model),
                                        GTR_MESSAGE_TABLE_MODEL_ID_COLUMN,
                                        GTK_SORT_ASCENDING);

  gtk_tree_sortable_set_default_sort_func (GTK_TREE_SORTABLE
                                           (priv->sort_model),
                                           NULL, NULL, NULL);

  gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (priv->sort_model),
                                   GTR_MESSAGE_TABLE_MODEL_STATUS_COLUMN,
                                   model_compare_by_status, NULL, NULL);

  gtk_tree_view_set_model (GTK_TREE_VIEW (priv->treeview),
                           priv->sort_model);
}

/**
 * gtr_message_table_navigate:
 * @table:
 * @navigation:
 * @func: (scope call):
 *
 * Returns: (transfer none):
 */
GtrMsg *
gtr_message_table_navigate (GtrMessageTable * table,
                            GtrMessageTableNavigation navigation,
                            GtrMessageTableNavigationFunc func)
{
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreePath *path;
  GtkTreeIter iter;
  GtrMessageTablePrivate *priv;
  GtrMsg *msg;
  gboolean cont = TRUE;

  priv = gtr_message_table_get_instance_private (table);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));

  if (!gtk_tree_selection_get_selected (selection, &model, &iter))
    return NULL;

  switch (navigation)
    {
    case GTR_NAVIGATE_FIRST:
      if (!gtk_tree_model_get_iter_first (model, &iter))
        return NULL;
      break;
    case GTR_NAVIGATE_LAST:
      {
        gint n_children;

        n_children = gtk_tree_model_iter_n_children (model, NULL);

        if (n_children <= 0)
          return NULL;

        if (!gtk_tree_model_iter_nth_child (model, &iter, NULL, n_children - 1))
          return NULL;
      }
      break;
    case GTR_NAVIGATE_NEXT:
      if (func)
        {
          while (cont)
            {
              if (!gtk_tree_model_iter_next (model, &iter))
                return NULL;

              gtk_tree_model_get (model, &iter,
                                  GTR_MESSAGE_TABLE_MODEL_POINTER_COLUMN, &msg,
                                  -1);

              if (func (msg))
                cont = FALSE;
            }
        }
      else if (!gtk_tree_model_iter_next (model, &iter))
        return NULL;

      break;
    case GTR_NAVIGATE_PREV:
      if (func)
        {
          while (cont)
            {
              if (!gtk_tree_model_iter_previous (model, &iter))
                return NULL;

              gtk_tree_model_get (model, &iter,
                                  GTR_MESSAGE_TABLE_MODEL_POINTER_COLUMN, &msg,
                                  -1);

              if (func (msg))
                cont = FALSE;
            }
        }
      else if (!gtk_tree_model_iter_previous (model, &iter))
        return NULL;
      break;
    }

  gtk_tree_selection_select_iter (selection, &iter);
  path = gtk_tree_model_get_path (model, &iter);
  gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (priv->treeview),
                                path, NULL, TRUE, 0.5, 0.0);

  gtk_tree_model_get (model, &iter,
                      GTR_MESSAGE_TABLE_MODEL_POINTER_COLUMN, &msg,
                      -1);

  return msg;
}
