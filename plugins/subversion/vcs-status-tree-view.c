/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
* gtranslator
* Copyright (C) James Liggett 2007 <jrliggett@cox.net>
* 
* gtranslator is free software.
* 
* You may redistribute it and/or modify it under the terms of the
* GNU General Public License, as published by the Free Software
* Foundation; either version 2 of the License, or (at your option)
* any later version.
* 
* gtranslator is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with gtranslator.  If not, write to:
* 	The Free Software Foundation, Inc.,
* 	51 Franklin Street, Fifth Floor
* 	Boston, MA  02110-1301, USA.
*/

#include "vcs-status-tree-view.h"
#include <string.h>
#include <glib/gi18n.h>

enum
{
  COL_SELECTED,
  COL_STATUS,
  COL_PATH,
  NUM_COLS
};

enum
{
  GTR_VCS_STATUS_TREE_VIEW_CONSTRUCT_STATUS_CODES = 1,
  GTR_VCS_STATUS_TREE_VIEW_SHOW_STATUS
};

struct _GtranslatorVcsStatusTreeViewPriv
{
  GtkListStore *store;
  GHashTable *selected_paths;
  guint status_codes;
  gboolean show_status;
};

G_DEFINE_TYPE (GtranslatorVcsStatusTreeView, gtranslator_vcs_status_tree_view,
	       GTK_TYPE_TREE_VIEW);

static void
on_selected_column_toggled (GtkCellRendererToggle * renderer,
			    gchar * tree_path,
			    GtranslatorVcsStatusTreeView * self)
{
  GtkTreeIter iter;
  gchar *vcs_path;
  gboolean selected;

  gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (self->priv->store),
				       &iter, tree_path);

  gtk_tree_model_get (GTK_TREE_MODEL (self->priv->store), &iter,
		      COL_PATH, &vcs_path, COL_SELECTED, &selected, -1);

  gtk_list_store_set (self->priv->store, &iter, COL_SELECTED, !selected, -1);

  g_free (vcs_path);

}

static void
gtranslator_vcs_status_tree_view_create_columns (GtranslatorVcsStatusTreeView
						 * self)
{
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;

  /* Selected column */
  column = gtk_tree_view_column_new ();
  renderer = gtk_cell_renderer_toggle_new ();
  gtk_tree_view_column_pack_start (column, renderer, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (self), column);
  gtk_tree_view_column_add_attribute (column, renderer, "active",
				      COL_SELECTED);

  g_signal_connect (G_OBJECT (renderer), "toggled",
		    G_CALLBACK (on_selected_column_toggled), self);

  /* Status column */
  column = gtk_tree_view_column_new ();
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, renderer, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (self), column);
  gtk_tree_view_column_add_attribute (column, renderer, "text", COL_STATUS);

  /* Path column */
  column = gtk_tree_view_column_new ();
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, renderer, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (self), column);
  gtk_tree_view_column_add_attribute (column, renderer, "text", COL_PATH);

  gtk_tree_view_set_model (GTK_TREE_VIEW (self),
			   GTK_TREE_MODEL (self->priv->store));
  g_object_unref (self->priv->store);

}

static gint
path_sort (GtkTreeModel * model, GtkTreeIter * a, GtkTreeIter * b,
	   gpointer user_data)
{
  gint compare_value;
  gchar *path1;
  gchar *path2;

  gtk_tree_model_get (model, a, COL_PATH, &path1, -1);
  gtk_tree_model_get (model, b, COL_PATH, &path2, -1);

  compare_value = strcmp (path1, path2);

  g_free (path1);
  g_free (path2);

  return compare_value;
}

static void
gtranslator_vcs_status_tree_view_init (GtranslatorVcsStatusTreeView * self)
{
  GtkTreeSortable *sortable;

  self->priv = g_new0 (GtranslatorVcsStatusTreeViewPriv, 1);
  self->priv->store = gtk_list_store_new (NUM_COLS,
					  G_TYPE_BOOLEAN,
					  G_TYPE_STRING, G_TYPE_STRING);
  self->priv->selected_paths = g_hash_table_new_full (g_str_hash,
						      g_str_equal,
						      g_free, NULL);

  gtranslator_vcs_status_tree_view_create_columns (self);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (self), FALSE);

  sortable = GTK_TREE_SORTABLE (self->priv->store);
  gtk_tree_sortable_set_sort_column_id (sortable, COL_PATH,
					GTK_SORT_ASCENDING);
  gtk_tree_sortable_set_sort_func (sortable, COL_PATH, path_sort, NULL, NULL);
}

static void
gtranslator_vcs_status_tree_view_finalize (GObject * object)
{
  GtranslatorVcsStatusTreeView *self;

  self = GTR_VCS_STATUS_TREE_VIEW (object);

  g_hash_table_destroy (self->priv->selected_paths);
  g_free (self->priv);

  G_OBJECT_CLASS (gtranslator_vcs_status_tree_view_parent_class)->
    finalize (object);
}

static void
gtranslator_vcs_status_tree_view_set_property (GObject * object,
					       guint property_id,
					       const GValue * value,
					       GParamSpec * param_spec)
{
  GtranslatorVcsStatusTreeView *self;
  GtkTreeView *tree_view;
  GtkTreeViewColumn *column;

  self = GTR_VCS_STATUS_TREE_VIEW (object);

  switch (property_id)
    {
    case GTR_VCS_STATUS_TREE_VIEW_CONSTRUCT_STATUS_CODES:
      self->priv->status_codes = g_value_get_flags (value);
      break;
    case GTR_VCS_STATUS_TREE_VIEW_SHOW_STATUS:
      tree_view = GTK_TREE_VIEW (object);
      column = gtk_tree_view_get_column (tree_view, COL_STATUS);
      self->priv->show_status = g_value_get_boolean (value);

      gtk_tree_view_column_set_visible (column, self->priv->show_status);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, param_spec);
      break;
    }
}

static void
gtranslator_vcs_status_tree_view_get_property (GObject * object,
					       guint property_id,
					       GValue * value,
					       GParamSpec * param_spec)
{
  GtranslatorVcsStatusTreeView *self;

  self = GTR_VCS_STATUS_TREE_VIEW (object);

  switch (property_id)
    {
    case GTR_VCS_STATUS_TREE_VIEW_CONSTRUCT_STATUS_CODES:
      g_value_set_flags (value, self->priv->status_codes);
      break;
    case GTR_VCS_STATUS_TREE_VIEW_SHOW_STATUS:
      g_value_set_boolean (value, self->priv->show_status);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, param_spec);
      break;
    }
}

static void
gtranslator_vcs_status_tree_view_class_init (GtranslatorVcsStatusTreeViewClass
					     * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *param_spec;

  object_class->finalize = gtranslator_vcs_status_tree_view_finalize;
  object_class->get_property = gtranslator_vcs_status_tree_view_get_property;
  object_class->set_property = gtranslator_vcs_status_tree_view_set_property;

  param_spec = g_param_spec_flags ("status-codes", "Status codes",
				   "Control which status codes are shown in "
				   "the list.",
				   GTR_TYPE_VCS_STATUS,
				   GTR_VCS_DEFAULT_STATUS_CODES,
				   G_PARAM_READWRITE |
				   G_PARAM_CONSTRUCT_ONLY);
  g_object_class_install_property (object_class,
				   GTR_VCS_STATUS_TREE_VIEW_CONSTRUCT_STATUS_CODES,
				   param_spec);

  param_spec = g_param_spec_boolean ("show-status", "Show status",
				     "Show or hide status description",
				     TRUE, G_PARAM_READWRITE);
  g_object_class_install_property (object_class,
				   GTR_VCS_STATUS_TREE_VIEW_SHOW_STATUS,
				   param_spec);
}


GtkWidget *
gtranslator_vcs_status_tree_view_new (void)
{
  return g_object_new (GTR_VCS_TYPE_STATUS_TREE_VIEW, NULL);
}

void
gtranslator_vcs_status_tree_view_destroy (GtranslatorVcsStatusTreeView * self)
{
  g_object_unref (self);
}

void
gtranslator_vcs_status_tree_view_add (GtranslatorVcsStatusTreeView * self,
				      gchar * path,
				      GtranslatorVcsStatus status,
				      gboolean selected)
{
  GtkTreeIter iter;

  if (status & self->priv->status_codes)
    {
      gtk_list_store_append (self->priv->store, &iter);

      gtk_list_store_set (self->priv->store, &iter,
			  COL_SELECTED, selected, COL_PATH, path, -1);

      if (selected)
	{
	  g_hash_table_insert (self->priv->selected_paths, g_strdup (path),
			       NULL);
	}

      switch (status)
	{
	case GTR_VCS_STATUS_MODIFIED:
	  gtk_list_store_set (self->priv->store, &iter, COL_STATUS,
			      _("Modified"), -1);
	  break;
	case GTR_VCS_STATUS_ADDED:
	  gtk_list_store_set (self->priv->store, &iter, COL_STATUS,
			      _("Added"), -1);
	  break;
	case GTR_VCS_STATUS_DELETED:
	  gtk_list_store_set (self->priv->store, &iter, COL_STATUS,
			      _("Deleted"), -1);
	  break;
	case GTR_VCS_STATUS_CONFLICTED:
	  gtk_list_store_set (self->priv->store, &iter, COL_STATUS,
			      _("Conflicted"), -1);
	  break;
	case GTR_VCS_STATUS_MISSING:
	  gtk_list_store_set (self->priv->store, &iter, COL_STATUS,
			      _("Missing"), -1);
	  break;
	case GTR_VCS_STATUS_NONE:
	default:
	  break;

	}
    }
}

static gboolean
select_all_paths (GtkTreeModel * model, GtkTreePath * path,
		  GtkTreeIter * iter, GtranslatorVcsStatusTreeView * self)
{
  gtk_list_store_set (self->priv->store, iter, COL_SELECTED, TRUE, -1);

  return FALSE;
}

void
gtranslator_vcs_status_tree_view_select_all (GtranslatorVcsStatusTreeView *
					     self)
{
  gtk_tree_model_foreach (GTK_TREE_MODEL (self->priv->store),
			  (GtkTreeModelForeachFunc) select_all_paths, self);
}

static gboolean
unselect_all_paths (GtkTreeModel * model, GtkTreePath * path,
		    GtkTreeIter * iter, GtranslatorVcsStatusTreeView * self)
{
  gtk_list_store_set (self->priv->store, iter, COL_SELECTED, FALSE, -1);

  return FALSE;
}

void
gtranslator_vcs_status_tree_view_unselect_all (GtranslatorVcsStatusTreeView *
					       self)
{
  gtk_tree_model_foreach (GTK_TREE_MODEL (self->priv->store),
			  (GtkTreeModelForeachFunc) unselect_all_paths, self);
}

static gboolean
create_selected_paths_list (GtkTreeModel * model, GtkTreePath * path,
			    GtkTreeIter * iter, GList ** list)
{
  gboolean selected;
  gchar *file_path;

  gtk_tree_model_get (model, iter,
		      COL_SELECTED, &selected, COL_PATH, &file_path, -1);

  if (selected)
    *list = g_list_append (*list, g_strdup (file_path));

  return FALSE;
}

GList *
gtranslator_vcs_status_tree_view_get_selected (GtranslatorVcsStatusTreeView *
					       self)
{
  GList *list;

  list = NULL;

  gtk_tree_model_foreach (GTK_TREE_MODEL (self->priv->store),
			  (GtkTreeModelForeachFunc)
			  create_selected_paths_list, &list);

  return list;
}

void
gtranslator_vsc_status_tree_view_clear (GtranslatorVcsStatusTreeView * self)
{
  g_return_if_fail (GTR_VCS_IS_STATUS_TREE_VIEW (self));

  gtk_list_store_clear (self->priv->store);
}
