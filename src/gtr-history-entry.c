/*
 * gtr-history-entry.c
 * This file is part of gtr
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * Modified by the gtr Team, 2006. See the AUTHORS file for a
 * list of people on the gtr Team.
 * See the ChangeLog files for a list of changes.
 *
 * $Id$
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "gtr-history-entry.h"

enum
{
  PROP_0,
  PROP_HISTORY_ID,
  PROP_HISTORY_LENGTH,
  PROP_ENABLE_COMPLETION
};

#define MIN_ITEM_LEN 3

#define GTR_HISTORY_ENTRY_HISTORY_LENGTH_DEFAULT 10

#define GTR_HISTORY_ENTRY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), \
						GTR_TYPE_HISTORY_ENTRY, \
						GtrHistoryEntryPrivate))

struct _GtrHistoryEntryPrivate
{
  gchar *history_id;
  guint history_length;

  GtkEntryCompletion *completion;

  GSettings *settings;
};

G_DEFINE_TYPE (GtrHistoryEntry, gtr_history_entry, GTK_TYPE_COMBO_BOX_TEXT)
     static void
       gtr_history_entry_set_property (GObject * object,
                                       guint prop_id,
                                       const GValue * value,
                                       GParamSpec * spec)
{
  GtrHistoryEntry *entry;

  g_return_if_fail (GTR_IS_HISTORY_ENTRY (object));

  entry = GTR_HISTORY_ENTRY (object);

  switch (prop_id)
    {
    case PROP_HISTORY_ID:
      entry->priv->history_id = g_value_dup_string (value);
      break;
    case PROP_HISTORY_LENGTH:
      gtr_history_entry_set_history_length (entry, g_value_get_uint (value));
      break;
    case PROP_ENABLE_COMPLETION:
      gtr_history_entry_set_enable_completion (entry,
                                               g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, spec);
    }
}

static void
gtr_history_entry_get_property (GObject * object,
                                guint prop_id,
                                GValue * value, GParamSpec * spec)
{
  GtrHistoryEntryPrivate *priv;

  g_return_if_fail (GTR_IS_HISTORY_ENTRY (object));

  priv = GTR_HISTORY_ENTRY (object)->priv;

  switch (prop_id)
    {
    case PROP_HISTORY_ID:
      g_value_set_string (value, priv->history_id);
      break;
    case PROP_HISTORY_LENGTH:
      g_value_set_uint (value, priv->history_length);
      break;
    case PROP_ENABLE_COMPLETION:
      g_value_set_boolean (value,
                           gtr_history_entry_get_enable_completion
                           (GTR_HISTORY_ENTRY (object)));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, spec);
    }
}

static void
gtr_history_entry_dispose (GObject * object)
{
  GtrHistoryEntryPrivate *priv;

  priv = GTR_HISTORY_ENTRY (object)->priv;

  gtr_history_entry_set_enable_completion (GTR_HISTORY_ENTRY (object), FALSE);

  g_clear_object (&priv->settings);

  G_OBJECT_CLASS (gtr_history_entry_parent_class)->dispose (object);
}

static void
gtr_history_entry_finalize (GObject * object)
{
  GtrHistoryEntryPrivate *priv;

  priv = GTR_HISTORY_ENTRY (object)->priv;

  g_free (priv->history_id);

  G_OBJECT_CLASS (gtr_history_entry_parent_class)->finalize (object);
}

static void
gtr_history_entry_load_history (GtrHistoryEntry * entry)
{
  gchar **items;
  gsize i;

  items = g_settings_get_strv (entry->priv->settings,
                               entry->priv->history_id);
  i = 0;

  gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (entry));

  /* Now the default value is an empty string so we have to take care
     of it to not add the empty string in the search list */
  while (items[i] != NULL && *items[i] != '\0' &&
         i < entry->priv->history_length)
    {
      gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (entry), items[i]);
      i++;
    }

  g_strfreev (items);
}

static void
gtr_history_entry_class_init (GtrHistoryEntryClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = gtr_history_entry_set_property;
  object_class->get_property = gtr_history_entry_get_property;
  object_class->dispose = gtr_history_entry_dispose;
  object_class->finalize = gtr_history_entry_finalize;

  g_object_class_install_property (object_class,
                                   PROP_HISTORY_ID,
                                   g_param_spec_string ("history-id",
                                                        "History ID",
                                                        "History ID",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY
                                                        |
                                                        G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (object_class,
                                   PROP_HISTORY_LENGTH,
                                   g_param_spec_uint ("history-length",
                                                      "Max History Length",
                                                      "Max History Length",
                                                      0,
                                                      G_MAXUINT,
                                                      GTR_HISTORY_ENTRY_HISTORY_LENGTH_DEFAULT,
                                                      G_PARAM_READWRITE |
                                                      G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (object_class,
                                   PROP_ENABLE_COMPLETION,
                                   g_param_spec_boolean ("enable-completion",
                                                         "Enable Completion",
                                                         "Wether the completion is enabled",
                                                         TRUE,
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_STATIC_STRINGS));

  g_type_class_add_private (object_class, sizeof (GtrHistoryEntryPrivate));
}

static GtkListStore *
get_history_store (GtrHistoryEntry * entry)
{
  GtkTreeModel *store;

  store = gtk_combo_box_get_model (GTK_COMBO_BOX (entry));
  g_return_val_if_fail (GTK_IS_LIST_STORE (store), NULL);

  return (GtkListStore *) store;
}

static gchar **
get_history_items (GtrHistoryEntry * entry)
{
  GtkListStore *store;
  GtkTreeIter iter;
  GPtrArray *array;
  gboolean valid;
  gint n_children;
  gint text_column;

  store = get_history_store (entry);
  text_column = gtk_combo_box_get_entry_text_column (GTK_COMBO_BOX (entry));

  valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &iter);
  n_children = gtk_tree_model_iter_n_children (GTK_TREE_MODEL (store), NULL);

  array = g_ptr_array_sized_new (n_children + 1);

  while (valid)
    {
      gchar *str;

      gtk_tree_model_get (GTK_TREE_MODEL (store),
                          &iter, text_column, &str, -1);

      g_ptr_array_add (array, str);

      valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter);
    }

  g_ptr_array_add (array, NULL);

  return (gchar **) g_ptr_array_free (array, FALSE);
}

static void
gtr_history_entry_save_history (GtrHistoryEntry * entry)
{
  gchar **items;

  g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));

  items = get_history_items (entry);

  g_settings_set_strv (entry->priv->settings,
                       entry->priv->history_id,
                       (const gchar * const *) items);

  g_strfreev (items);
}

static gboolean
remove_item (GtrHistoryEntry * entry, const gchar * text)
{
  GtkListStore *store;
  GtkTreeIter iter;
  gint text_column;

  g_return_val_if_fail (text != NULL, FALSE);

  store = get_history_store (entry);
  text_column = gtk_combo_box_get_entry_text_column (GTK_COMBO_BOX (entry));

  if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &iter))
    return FALSE;

  do
    {
      gchar *item_text;

      gtk_tree_model_get (GTK_TREE_MODEL (store),
                          &iter, text_column, &item_text, -1);

      if (item_text != NULL && strcmp (item_text, text) == 0)
        {
          gtk_list_store_remove (store, &iter);
          g_free (item_text);
          return TRUE;
        }

      g_free (item_text);

    }
  while (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter));

  return FALSE;
}

static void
clamp_list_store (GtkListStore * store, guint max)
{
  GtkTreePath *path;
  GtkTreeIter iter;

  /* -1 because TreePath counts from 0 */
  path = gtk_tree_path_new_from_indices (max - 1, -1);

  if (gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &iter, path))
    {
      while (TRUE)
        {
          if (!gtk_list_store_remove (store, &iter))
            break;
        }
    }

  gtk_tree_path_free (path);
}

static void
insert_history_item (GtrHistoryEntry * entry,
                     const gchar * text, gboolean prepend)
{
  GtkListStore *store;

  if (g_utf8_strlen (text, -1) <= MIN_ITEM_LEN)
    return;

  store = get_history_store (entry);

  /* remove the text from the store if it was already
   * present. If it wasn't, clamp to max history - 1
   * before inserting the new row, otherwise appending
   * would not work */

  if (!remove_item (entry, text))
    clamp_list_store (store, entry->priv->history_length - 1);

  if (prepend)
    gtk_combo_box_text_prepend_text (GTK_COMBO_BOX_TEXT (entry), text);
  else
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (entry), text);

  gtr_history_entry_save_history (entry);
}

void
gtr_history_entry_prepend_text (GtrHistoryEntry * entry, const gchar * text)
{
  g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));
  g_return_if_fail (text != NULL);

  insert_history_item (entry, text, TRUE);
}

void
gtr_history_entry_append_text (GtrHistoryEntry * entry, const gchar * text)
{
  g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));
  g_return_if_fail (text != NULL);

  insert_history_item (entry, text, FALSE);
}

void
gtr_history_entry_clear (GtrHistoryEntry * entry)
{
  g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));

  gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (entry));

  gtr_history_entry_save_history (entry);
}

static void
gtr_history_entry_init (GtrHistoryEntry * entry)
{
  GtrHistoryEntryPrivate *priv;

  priv = GTR_HISTORY_ENTRY_GET_PRIVATE (entry);
  entry->priv = priv;

  priv->history_id = NULL;
  priv->history_length = GTR_HISTORY_ENTRY_HISTORY_LENGTH_DEFAULT;

  priv->completion = NULL;

  priv->settings = g_settings_new ("org.gnome.gtranslator.state.history-entry");
}

void
gtr_history_entry_set_history_length (GtrHistoryEntry * entry,
                                      guint history_length)
{
  g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));
  g_return_if_fail (history_length > 0);

  entry->priv->history_length = history_length;

  /* TODO: update if we currently have more items than max */
}

guint
gtr_history_entry_get_history_length (GtrHistoryEntry * entry)
{
  g_return_val_if_fail (GTR_IS_HISTORY_ENTRY (entry), 0);

  return entry->priv->history_length;
}

void
gtr_history_entry_set_enable_completion (GtrHistoryEntry * entry,
                                         gboolean enable)
{
  g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));

  if (enable)
    {
      if (entry->priv->completion != NULL)
        return;

      entry->priv->completion = gtk_entry_completion_new ();
      gtk_entry_completion_set_model (entry->priv->completion,
                                      GTK_TREE_MODEL (get_history_store
                                                      (entry)));

      /* Use model column 0 as the text column */
      gtk_entry_completion_set_text_column (entry->priv->completion, 0);

      gtk_entry_completion_set_minimum_key_length (entry->priv->completion,
                                                   MIN_ITEM_LEN);

      gtk_entry_completion_set_popup_completion (entry->priv->completion,
                                                 FALSE);
      gtk_entry_completion_set_inline_completion (entry->priv->completion,
                                                  TRUE);

      /* Assign the completion to the entry */
      gtk_entry_set_completion (GTK_ENTRY
                                (gtr_history_entry_get_entry (entry)),
                                entry->priv->completion);
    }
  else
    {
      if (entry->priv->completion == NULL)
        return;

      gtk_entry_set_completion (GTK_ENTRY
                                (gtr_history_entry_get_entry (entry)), NULL);

      g_object_unref (entry->priv->completion);

      entry->priv->completion = NULL;
    }
}

gboolean
gtr_history_entry_get_enable_completion (GtrHistoryEntry * entry)
{
  g_return_val_if_fail (GTR_IS_HISTORY_ENTRY (entry), FALSE);

  return entry->priv->completion != NULL;
}

GtkWidget *
gtr_history_entry_new (const gchar * history_id, gboolean enable_completion)
{
  GtrHistoryEntry *entry;

  g_return_val_if_fail (history_id != NULL, NULL);

  enable_completion = (enable_completion != FALSE);

  entry = g_object_new (GTR_TYPE_HISTORY_ENTRY,
                        "has-entry", TRUE,
                        "entry-text-column", 0,
                        "id-column", 1,
                        "history-id", history_id,
                        "enable-completion", enable_completion, NULL);

  /* We must load the history after the object has been constructed,
   * to ensure that the model is set properly.
   */
  gtr_history_entry_load_history (entry);

  return GTK_WIDGET (entry);
}

/*
 * Utility function to get the editable text entry internal widget.
 * I would prefer to not expose this implementation detail and
 * simply make the GtrHistoryEntry widget implement the
 * GtkEditable interface. Unfortunately both GtkEditable and
 * GtkComboBox have a "changed" signal and I am not sure how to
 * handle the conflict.
 */
GtkWidget *
gtr_history_entry_get_entry (GtrHistoryEntry * entry)
{
  g_return_val_if_fail (GTR_IS_HISTORY_ENTRY (entry), NULL);

  return gtk_bin_get_child (GTK_BIN (entry));
}

static void
escape_cell_data_func (GtkTreeViewColumn * col,
                       GtkCellRenderer * renderer,
                       GtkTreeModel * model,
                       GtkTreeIter * iter,
                       GtrHistoryEntryEscapeFunc escape_func)
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
gtr_history_entry_set_escape_func (GtrHistoryEntry * entry,
                                   GtrHistoryEntryEscapeFunc escape_func)
{
  GList *cells;

  g_return_if_fail (GTR_IS_HISTORY_ENTRY (entry));

  cells = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (entry));

  /* We only have one cell renderer */
  g_return_if_fail (cells->data != NULL && cells->next == NULL);

  if (escape_func != NULL)
    {
      gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (entry),
                                          GTK_CELL_RENDERER (cells->data),
                                          (GtkCellLayoutDataFunc)
                                          escape_cell_data_func, escape_func,
                                          NULL);
    }
  else
    {
      gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (entry),
                                          GTK_CELL_RENDERER (cells->data),
                                          NULL, NULL, NULL);
    }

  g_list_free (cells);
}

/* ex:set ts=8 noet: */
