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
#include "gtr-message-container.h"
#include "gtr-msg.h"

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define G_LIST(x) ((GList *) x)

#define TABLE_FUZZY_ICON	"gtk-dialog-warning"
#define TABLE_UNTRANSLATED_ICON	"gtk-dialog-error"
#define TABLE_TRANSLATED_ICON	NULL

enum {
  PROP_0,
  PROP_CONTAINER
};

static GObjectClass *parent_class;

static guint
gtr_message_table_model_get_flags (GtkTreeModel * self)
{
  return 0;
}

static gint
gtr_message_table_model_get_n_columns (GtkTreeModel * self)
{
  return GTR_MESSAGE_TABLE_MODEL_N_COLUMNS;
}

static GType
gtr_message_table_model_get_column_type (GtkTreeModel * self, gint column)
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
gtr_message_table_model_get_iter (GtkTreeModel * self,
                                  GtkTreeIter * iter, GtkTreePath * path)
{
  GtrMessageTableModel *list_model = GTR_MESSAGE_TABLE_MODEL (self);
  gint i;

  g_return_val_if_fail (gtk_tree_path_get_depth (path) > 0, FALSE);

  /* Fill a GtkTreeIter using a path */

  i = gtk_tree_path_get_indices (path)[0];

  if (G_UNLIKELY (i >= gtr_message_container_get_count (list_model->container)))
    return FALSE;

  iter->stamp = list_model->stamp;
  iter->user_data = gtr_message_container_get_message (list_model->container, i);
  iter->user_data2 = GINT_TO_POINTER (i);

  return TRUE;
}

static GtkTreePath *
gtr_message_table_model_get_path (GtkTreeModel * tree_model,
                                  GtkTreeIter  * iter)
{
  GtrMessageTableModel *model = GTR_MESSAGE_TABLE_MODEL (tree_model);
  GtkTreePath *tree_path;
  GtrMsg *msg;
  gint i;

  g_return_val_if_fail (iter->stamp == model->stamp, NULL);

  /* ensure iter is valid */
  i = GPOINTER_TO_INT (iter->user_data2);
  msg = gtr_message_container_get_message (model->container, i);

  if (msg != iter->user_data)
    return NULL;

  tree_path = gtk_tree_path_new ();
  gtk_tree_path_append_index (tree_path, i);

  return tree_path;
}

static void
gtr_message_table_model_get_value (GtkTreeModel * self,
                                   GtkTreeIter * iter,
                                   gint column, GValue * value)
{
  GtrMsg *msg;
  gchar *text;
  GtrMsgStatus status;
  gint i;

  g_return_if_fail (iter->stamp == GTR_MESSAGE_TABLE_MODEL (self)->stamp);

  msg = GTR_MSG (iter->user_data);

  switch (column)
    {
    case GTR_MESSAGE_TABLE_MODEL_ICON_COLUMN:
      g_value_init (value, G_TYPE_STRING);

      status = gtr_msg_get_status (msg);

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

      i = GPOINTER_TO_INT (iter->user_data2);
      g_value_set_int (value, i + 1);
      break;

    case GTR_MESSAGE_TABLE_MODEL_ORIGINAL_COLUMN:
      g_value_init (value, G_TYPE_STRING);

      text = g_strdup (gtr_msg_get_msgid (msg));
      g_value_set_string (value, text);
      g_free (text);
      break;

    case GTR_MESSAGE_TABLE_MODEL_TRANSLATION_COLUMN:
      g_value_init (value, G_TYPE_STRING);

      text = g_strdup (gtr_msg_get_msgstr (msg));
      g_value_set_string (value, text);
      g_free (text);
      break;

    case GTR_MESSAGE_TABLE_MODEL_STATUS_COLUMN:
      g_value_init (value, G_TYPE_INT);

      status = gtr_msg_get_status (msg);
      g_value_set_int (value, status);
      break;

    case GTR_MESSAGE_TABLE_MODEL_POINTER_COLUMN:
      g_value_init (value, G_TYPE_POINTER);

      g_value_set_pointer (value, msg);
      break;

    default:
      break;
    }

}

static gboolean
gtr_message_table_model_iter_previous (GtkTreeModel * tree_model,
                                       GtkTreeIter * iter)
{
  GtrMessageTableModel *model = GTR_MESSAGE_TABLE_MODEL (tree_model);
  gint i;

  g_return_val_if_fail (iter->stamp == model->stamp, FALSE);

  i = GPOINTER_TO_INT (iter->user_data2) - 1;

  if (i < 0)
    return FALSE;

  iter->user_data = gtr_message_container_get_message (model->container, i);
  iter->user_data2 = GINT_TO_POINTER (i);

  return TRUE;
}

static gboolean
gtr_message_table_model_iter_next (GtkTreeModel * tree_model,
                                   GtkTreeIter * iter)
{
  GtrMessageTableModel *model = GTR_MESSAGE_TABLE_MODEL (tree_model);
  gint i;

  g_return_val_if_fail (iter->stamp == model->stamp, FALSE);

  i = GPOINTER_TO_INT (iter->user_data2) + 1;

  if (i >= gtr_message_container_get_count (model->container))
    return FALSE;

  iter->user_data = gtr_message_container_get_message (model->container, i);
  iter->user_data2 = GINT_TO_POINTER (i);

  return TRUE;
}

static gboolean
gtr_message_table_model_iter_has_child (GtkTreeModel * tree_model,
                                        GtkTreeIter * iter)
{
  return FALSE;
}

static gint
gtr_message_table_model_iter_n_children (GtkTreeModel * tree_model,
                                         GtkTreeIter * iter)
{
  GtrMessageTableModel *model = GTR_MESSAGE_TABLE_MODEL (tree_model);

  /* it should ask for the root node, because we're a list */
  if (!iter)
    return gtr_message_container_get_count (model->container);

  return -1;
}

static gboolean
gtr_message_table_model_iter_nth_child (GtkTreeModel * tree_model,
                                        GtkTreeIter * iter,
                                        GtkTreeIter * parent, gint n)
{
  GtrMessageTableModel *model = GTR_MESSAGE_TABLE_MODEL (tree_model);

  if (parent)
    return FALSE;

  if (n < 0 || n >= gtr_message_container_get_count (model->container))
    return FALSE;

  iter->stamp = GTR_MESSAGE_TABLE_MODEL (tree_model)->stamp;
  iter->user_data = gtr_message_container_get_message (model->container, n);
  iter->user_data2 = GINT_TO_POINTER (n);

  return TRUE;
}

static gboolean
gtr_message_table_model_iter_children (GtkTreeModel * tree_model,
                                       GtkTreeIter * iter,
                                       GtkTreeIter * parent)
{
  GtrMessageTableModel *model = GTR_MESSAGE_TABLE_MODEL (tree_model);

  /* this is a list, nodes have no children */
  if (parent)
    return FALSE;

  if (gtr_message_container_get_count (model->container) == 0)
    return FALSE;

  iter->stamp = model->stamp;
  iter->user_data = gtr_message_container_get_message (model->container, 0);
  iter->user_data2 = 0;

  return TRUE;
}

static void
gtr_message_table_model_tree_model_init (GtkTreeModelIface * iface)
{
  iface->get_flags = gtr_message_table_model_get_flags;
  iface->get_n_columns = gtr_message_table_model_get_n_columns;
  iface->get_column_type = gtr_message_table_model_get_column_type;
  iface->get_iter = gtr_message_table_model_get_iter;
  iface->get_path = gtr_message_table_model_get_path;
  iface->get_value = gtr_message_table_model_get_value;
  iface->iter_next = gtr_message_table_model_iter_next;
  iface->iter_previous = gtr_message_table_model_iter_previous;
  iface->iter_has_child = gtr_message_table_model_iter_has_child;
  iface->iter_n_children = gtr_message_table_model_iter_n_children;
  iface->iter_nth_child = gtr_message_table_model_iter_nth_child;
  iface->iter_children = gtr_message_table_model_iter_children;
}

static void
gtr_message_table_model_init (GtrMessageTableModel * model)
{
  model->stamp = g_random_int ();
}

static void
gtr_message_table_model_finalize (GObject * object)
{
  parent_class->finalize (object);
}

static void
gtr_message_table_set_property (GObject      * object,
                                guint          prop_id,
                                const GValue * value,
                                GParamSpec   * pspec)
{
  GtrMessageTableModel * model = GTR_MESSAGE_TABLE_MODEL (object);

  switch (prop_id)
    {
    case PROP_CONTAINER:
      model->container = g_value_get_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_message_table_get_property (GObject    * object,
                                guint        prop_id,
                                GValue     * value,
                                GParamSpec * pspec)
{
  GtrMessageTableModel * model = GTR_MESSAGE_TABLE_MODEL (object);

  switch (prop_id)
    {
    case PROP_CONTAINER:
      g_value_set_object (value, model->container);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_message_table_model_class_init (GtrMessageTableModelClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtr_message_table_model_finalize;
  object_class->set_property = gtr_message_table_set_property;
  object_class->get_property = gtr_message_table_get_property;

  g_object_class_install_property (object_class,
                                   PROP_CONTAINER,
                                   g_param_spec_object ("container",
                                                        "container",
                                                        "message container",
                                                        GTR_TYPE_MESSAGE_CONTAINER,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY));

  parent_class = g_type_class_peek_parent (klass);
}

/***************************** Public funcs ***********************************/

/**
 * gtr_message_table_model_new:
 * 
 * Return value: a new #GtrMessageTableModel object
 **/
GtrMessageTableModel *
gtr_message_table_model_new (GtrMessageContainer *container)
{
  GtrMessageTableModel *model;

  model = g_object_new (GTR_TYPE_MESSAGE_TABLE_MODEL,
                        "container", container,
                        NULL);

  return model;
}

gboolean
gtr_message_table_get_message_iter (GtrMessageTableModel * model,
                                    GtrMsg * msg, GtkTreeIter * iter)
{
  gint n_msg;

  g_return_val_if_fail (model != NULL, FALSE);
  g_return_val_if_fail (iter != NULL, FALSE);

  n_msg = gtr_message_container_get_message_number (model->container, msg);

  if (n_msg < 0)
    return FALSE;

  iter->stamp = model->stamp;
  iter->user_data = msg;
  iter->user_data2 = GINT_TO_POINTER (n_msg);

  return TRUE;
}

void
gtr_message_table_model_update_row (GtrMessageTableModel *
                                    model, GtkTreePath * path)
{
  GtkTreeIter iter;

  gtr_message_table_model_get_iter (GTK_TREE_MODEL (model), &iter, path);

  gtk_tree_model_row_changed (GTK_TREE_MODEL (model), path, &iter);
}

GType
gtr_message_table_model_get_type (void)
{
  static GType object_type = 0;

  if (G_UNLIKELY (object_type == 0))
    {
      static const GTypeInfo object_info = {
        sizeof (GtrMessageTableModelClass),
        NULL,                   /* base_init */
        NULL,                   /* base_finalize */
        (GClassInitFunc) gtr_message_table_model_class_init,
        NULL,                   /* class_finalize */
        NULL,                   /* class_data */
        sizeof (GtrMessageTableModel),
        0,                      /* n_preallocs */
        (GInstanceInitFunc) gtr_message_table_model_init,
        NULL
      };

      static const GInterfaceInfo tree_model_info = {
        (GInterfaceInitFunc) gtr_message_table_model_tree_model_init,
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
