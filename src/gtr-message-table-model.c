/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANMsgILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-message-table-model.h"
#include "gtr-msg.h"

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define G_LIST(x) ((GList *) x)

#define TABLE_FUZZY_ICON	"gtk-dialog-warning"
#define TABLE_UNTRANSLATED_ICON	"gtk-dialog-error"
#define TABLE_TRANSLATED_ICON	NULL

static GObjectClass *parent_class;

static guint
gtranslator_message_table_model_get_flags (GtkTreeModel * self)
{
  return 0;
}

static gint
gtranslator_message_table_model_get_n_columns (GtkTreeModel * self)
{
  return GTR_MESSAGE_TABLE_MODEL_N_COLUMNS;
}

static GType
gtranslator_message_table_model_get_column_type (GtkTreeModel * self,
						 gint column)
{
  GType retval;

  switch (column)
    {
    case GTR_MESSAGE_TABLE_MODEL_ICON_COLUMN:
    case GTR_MESSAGE_TABLE_MODEL_ORIGINAL_COLUMN:
    case GTR_MESSAGE_TABLE_MODEL_TRANSLATION_COLUMN:
      retval = G_TYPE_STRING;
      break;
    case GTR_MESSAGE_TABLE_MODEL_POINTER_COLUMN:
      retval = G_TYPE_POINTER;
      break;
    case GTR_MESSAGE_TABLE_MODEL_ID_COLUMN:
    case GTR_MESSAGE_TABLE_MODEL_STATUS_COLUMN:
      retval = G_TYPE_INT;
      break;
    default:
      retval = G_TYPE_INVALID;
      break;
    }


  return retval;
}

static gboolean
gtranslator_message_table_model_get_iter (GtkTreeModel * self,
					  GtkTreeIter * iter,
					  GtkTreePath * path)
{
  GtrMessageTableModel *list_model = GTR_MESSAGE_TABLE_MODEL (self);
  gint i;
  GList *list;

  g_return_val_if_fail (gtk_tree_path_get_depth (path) > 0, FALSE);

  /* Fill a GtkTreeIter using a path */

  i = gtk_tree_path_get_indices (path)[0];

  if (G_UNLIKELY (i >= list_model->length))
    return FALSE;

  list = g_list_nth (list_model->values, i);

  iter->stamp = list_model->stamp;
  iter->user_data = list;

  return TRUE;
}

static GtkTreePath *
gtranslator_message_table_model_get_path (GtkTreeModel * tree_model,
					  GtkTreeIter * iter)
{
  GList *list;
  GtkTreePath *tree_path;
  gint i = 0;

  g_return_val_if_fail (iter->stamp ==
			GTR_MESSAGE_TABLE_MODEL (tree_model)->stamp, NULL);

  for (list = GTR_MESSAGE_TABLE_MODEL (tree_model)->values; list;
       list = list->next)
    {
      if (list == iter->user_data)
	break;
      i++;
    }
  if (list == NULL)
    return NULL;

  tree_path = gtk_tree_path_new ();
  gtk_tree_path_append_index (tree_path, i);

  return tree_path;
}

static void
gtranslator_message_table_model_get_value (GtkTreeModel * self,
					   GtkTreeIter * iter,
					   gint column, GValue * value)
{
  GtrMessageTableModel *model = GTR_MESSAGE_TABLE_MODEL (self);
  GtrMsg *msg;
  gchar *text;
  GtrMsgStatus status;
  gint i;
  GList *list;

  g_return_if_fail (iter->stamp == GTR_MESSAGE_TABLE_MODEL (self)->stamp);

  list = G_LIST (iter->user_data);
  msg = GTR_MSG (list->data);

  switch (column)
    {
    case GTR_MESSAGE_TABLE_MODEL_ICON_COLUMN:
      g_value_init (value, G_TYPE_STRING);

      status = gtranslator_msg_get_status (msg);

      if (status == GTR_MSG_STATUS_UNTRANSLATED)
	text = TABLE_UNTRANSLATED_ICON;
      else if (status == GTR_MSG_STATUS_FUZZY)
	text = TABLE_FUZZY_ICON;
      else
	text = TABLE_TRANSLATED_ICON;

      g_value_set_string (value, text);
      break;

    case GTR_MESSAGE_TABLE_MODEL_ID_COLUMN:
      g_value_init (value, G_TYPE_INT);

      i = g_list_position (model->values, list);
      g_value_set_int (value, i + 1);
      break;

    case GTR_MESSAGE_TABLE_MODEL_ORIGINAL_COLUMN:
      g_value_init (value, G_TYPE_STRING);

      text = g_strdup (gtranslator_msg_get_msgid (msg));
      g_value_set_string (value, text);
      g_free (text);
      break;

    case GTR_MESSAGE_TABLE_MODEL_TRANSLATION_COLUMN:
      g_value_init (value, G_TYPE_STRING);

      text = g_strdup (gtranslator_msg_get_msgstr (msg));
      g_value_set_string (value, text);
      g_free (text);
      break;

    case GTR_MESSAGE_TABLE_MODEL_STATUS_COLUMN:
      g_value_init (value, G_TYPE_INT);

      status = gtranslator_msg_get_status (msg);
      g_value_set_int (value, status);
      break;

    case GTR_MESSAGE_TABLE_MODEL_POINTER_COLUMN:
      g_value_init (value, G_TYPE_POINTER);

      g_value_set_pointer (value, iter->user_data);
      break;

    default:
      break;
    }

}

static gboolean
gtranslator_message_table_model_iter_next (GtkTreeModel * tree_model,
					   GtkTreeIter * iter)
{
  g_return_val_if_fail (iter->stamp ==
			GTR_MESSAGE_TABLE_MODEL (tree_model)->stamp, FALSE);

  iter->user_data = G_LIST (iter->user_data)->next;

  return (iter->user_data != NULL);
}

static gboolean
gtranslator_message_table_model_iter_has_child (GtkTreeModel * tree_model,
						GtkTreeIter * iter)
{
  return FALSE;
}

static gint
gtranslator_message_table_model_iter_n_children (GtkTreeModel * tree_model,
						 GtkTreeIter * iter)
{
  /* it should ask for the root node, because we're a list */
  if (!iter)
    return g_list_length (GTR_MESSAGE_TABLE_MODEL (tree_model)->values);

  return -1;
}

static gboolean
gtranslator_message_table_model_iter_nth_child (GtkTreeModel * tree_model,
						GtkTreeIter * iter,
						GtkTreeIter * parent, gint n)
{
  GList *child;

  if (parent)
    return FALSE;

  child = g_list_nth (GTR_MESSAGE_TABLE_MODEL (tree_model)->values, n);

  if (child)
    {
      iter->stamp = GTR_MESSAGE_TABLE_MODEL (tree_model)->stamp;
      iter->user_data = child;
      return TRUE;
    }
  else
    return FALSE;
}

static gboolean
gtranslator_message_table_model_iter_children (GtkTreeModel * tree_model,
					       GtkTreeIter * iter,
					       GtkTreeIter * parent)
{
  GtrMessageTableModel *model = GTR_MESSAGE_TABLE_MODEL (tree_model);

  /* this is a list, nodes have no children */
  if (parent)
    return FALSE;

  if (g_list_length (model->values) > 0)
    {
      iter->stamp = model->stamp;
      iter->user_data = g_list_first (model->values);
      return TRUE;
    }
  else
    return FALSE;
}

static void
gtranslator_message_table_model_tree_model_init (GtkTreeModelIface * iface)
{
  iface->get_flags = gtranslator_message_table_model_get_flags;
  iface->get_n_columns = gtranslator_message_table_model_get_n_columns;
  iface->get_column_type = gtranslator_message_table_model_get_column_type;
  iface->get_iter = gtranslator_message_table_model_get_iter;
  iface->get_path = gtranslator_message_table_model_get_path;
  iface->get_value = gtranslator_message_table_model_get_value;
  iface->iter_next = gtranslator_message_table_model_iter_next;
  iface->iter_has_child = gtranslator_message_table_model_iter_has_child;
  iface->iter_n_children = gtranslator_message_table_model_iter_n_children;
  iface->iter_nth_child = gtranslator_message_table_model_iter_nth_child;
  iface->iter_children = gtranslator_message_table_model_iter_children;
}

static void
gtranslator_message_table_model_init (GtrMessageTableModel * model)
{
  model->length = 0;
  model->stamp = g_random_int ();
}

static void
gtranslator_message_table_model_finalize (GObject * object)
{
  parent_class->finalize (object);
}

static void
gtranslator_message_table_model_class_init (GtrMessageTableModelClass
					    * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtranslator_message_table_model_finalize;

  parent_class = g_type_class_peek_parent (klass);
}

/***************************** Public funcs ***********************************/

/**
 * gtranslator_message_table_model_new:
 * 
 * Return value: a new #GtrMessageTableModel object
 **/
GtrMessageTableModel *
gtranslator_message_table_model_new (void)
{
  GtrMessageTableModel *model;

  model = g_object_new (GTR_TYPE_MESSAGE_TABLE_MODEL, NULL);

  return model;
}

void
gtranslator_message_table_model_append (GtrMessageTableModel * model,
					GtrMsg * msg,
					GtkTreeIter * iter)
{
  GList *list;
  GtkTreePath *path;

  //The sort stuff can be improved using a GPtrArray or gsecuence instead of a GList
  model->values = g_list_append (model->values, msg);
  model->length++;

  model->stamp++;

  iter->stamp = model->stamp;
  iter->user_data = g_list_last (model->values);

  path = gtk_tree_model_get_path (GTK_TREE_MODEL (model), iter);
  gtk_tree_model_row_inserted (GTK_TREE_MODEL (model), path, iter);
  gtk_tree_path_free (path);
}

void
gtranslator_message_table_model_update_row (GtrMessageTableModel *
					    model, GtkTreePath * path)
{
  GtkTreeIter iter;

  gtranslator_message_table_model_get_iter (GTK_TREE_MODEL (model),
					    &iter, path);

  gtk_tree_model_row_changed (GTK_TREE_MODEL (model), path, &iter);
}

GType
gtranslator_message_table_model_get_type (void)
{
  static GType object_type = 0;

  if (G_UNLIKELY (object_type == 0))
    {
      static const GTypeInfo object_info = {
	sizeof (GtrMessageTableModelClass),
	NULL,			/* base_init */
	NULL,			/* base_finalize */
	(GClassInitFunc) gtranslator_message_table_model_class_init,
	NULL,			/* class_finalize */
	NULL,			/* class_data */
	sizeof (GtrMessageTableModel),
	0,			/* n_preallocs */
	(GInstanceInitFunc) gtranslator_message_table_model_init,
	NULL
      };

      static const GInterfaceInfo tree_model_info = {
	(GInterfaceInitFunc) gtranslator_message_table_model_tree_model_init,
	NULL,
	NULL
      };

      object_type = g_type_register_static (G_TYPE_OBJECT,
					    "GtrMessageTableModel",
					    &object_info, 0);

      g_type_add_interface_static (object_type, GTK_TYPE_TREE_MODEL,
				   &tree_model_info);

    }

  return object_type;
}
