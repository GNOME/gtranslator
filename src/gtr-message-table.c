/*
 * Copyright (C) 2007   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *                      Fatih Demir <kabalak@kabalak.net>
 *                      Ross Golder <ross@golder.org>
 *                      Gediminas Paulauskas <menesis@kabalak.net>
 *                      Kevin Vandersloot <kfv101@psu.edu>
 *                      Thomas Ziehmer <thomas@kabalak.net>
 *                      Peeter Vois <peeter@kabalak.net>
 *                      Seán de Búrca <leftmostcat@gmail.com>
 *                      Daniel Garcia Moreno <danigm@gnome.org>
 *
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

#include "gtr-message-table-row.h"
#include "gtr-message-table.h"
#include "gtr-msg.h"
#include "gtr-po.h"
#include "gtr-settings.h"
#include "gtr-tab.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

enum
{
  PROP_0,
  PROP_TAB
};

typedef struct
{
  GtkWidget *messages;
  GtkWidget *idlabel;
  GListStore *store;
  GtkSortListModel *sort_model;
  GtkSingleSelection *selection;

  // Sorters
  GtkSorter *id_sorter;
  GtkSorter *status_sorter;
  GtkSorter *msg_sorter;
  GtkSorter *translation_sorter;

  GtrTab *tab;
  GtrMessageTableSortBy sort_status;

  GSettings *ui_settings;
} GtrMessageTablePrivate;

struct _GtrMessageTable
{
  GtkBox parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrMessageTable, gtr_message_table, GTK_TYPE_BOX)

static void
on_sort_order_changed (GSettings *settings, gchar *key, gpointer user_data)
{
  GtrMessageTable *table = GTR_MESSAGE_TABLE (user_data);
  GtrMessageTablePrivate *priv
      = gtr_message_table_get_instance_private (table);

  gtr_message_table_sort_by (
      table, g_settings_get_enum (priv->ui_settings, GTR_SETTINGS_SORT_ORDER));
}

static gboolean
scroll_to_selected (GtrMessageTable * table)
{
  int selected = 0;
  GtrMessageTablePrivate *priv;

  priv = gtr_message_table_get_instance_private (table);
  selected = gtk_single_selection_get_selected (GTK_SINGLE_SELECTION (priv->selection));

  if (selected == GTK_INVALID_LIST_POSITION)
    return FALSE;

  gtk_list_view_scroll_to (GTK_LIST_VIEW (priv->messages), selected,
                           GTK_LIST_SCROLL_NONE, NULL);

  return FALSE;
}

static void
showed_message_cb (GtrTab * tab, GtrMsg * msg, GtrMessageTable * table)
{
  g_idle_add ((GSourceFunc)scroll_to_selected, table);
}

static void
gtr_message_table_selection_changed (GObject *object,
                                     GParamSpec *pspec,
                                     GtrMessageTable *table)
{
  GtrMsg *msg;
  GtrMessageTablePrivate *priv;

  priv = gtr_message_table_get_instance_private (table);
  msg = gtk_single_selection_get_selected_item (GTK_SINGLE_SELECTION (object));
  if (msg != NULL)
    {
      gtr_tab_message_go_to (priv->tab, msg, FALSE, GTR_TAB_MOVE_NONE);
    }
}

static void
gtr_message_table_init (GtrMessageTable * table)
{
  GtrMessageTablePrivate *priv;
  priv = gtr_message_table_get_instance_private (table);

  // GtkSorter to use with the GtkSortListModel
  // Id column hidden for now
  priv->id_sorter = GTK_SORTER (gtk_numeric_sorter_new (gtk_property_expression_new (GTR_TYPE_MSG, NULL, "position")));
  // Status column hidden for now
  priv->status_sorter = GTK_SORTER (gtk_string_sorter_new (gtk_property_expression_new (GTR_TYPE_MSG, NULL, "status_str")));
  // First column, original message
  priv->msg_sorter = GTK_SORTER (gtk_string_sorter_new (gtk_property_expression_new (GTR_TYPE_MSG, NULL, "original")));
  // Second column, translation message
  priv->translation_sorter = GTK_SORTER (gtk_string_sorter_new (gtk_property_expression_new (GTR_TYPE_MSG, NULL, "translation")));

  priv->ui_settings = g_settings_new ("org.gnome.gtranslator.preferences.ui");

  priv->sort_status
      = g_settings_get_enum (priv->ui_settings, GTR_SETTINGS_SORT_ORDER);
  priv->store = NULL;

  gtk_orientable_set_orientation (GTK_ORIENTABLE (table),
                                  GTK_ORIENTATION_VERTICAL);

  priv->sort_model = gtk_sort_list_model_new (NULL, NULL);

  g_signal_connect (priv->ui_settings, "changed::sort-order",
                    G_CALLBACK (on_sort_order_changed), table);

  gtk_widget_init_template (GTK_WIDGET (table));

  g_settings_bind (priv->ui_settings, GTR_SETTINGS_SHOW_ID_COLUMN,
                   priv->idlabel, "visible",
                   G_SETTINGS_BIND_DEFAULT);
}

static void
gtr_message_table_finalize (GObject * object)
{
  GtrMessageTable *table = GTR_MESSAGE_TABLE (object);
  GtrMessageTablePrivate *priv;

  priv = gtr_message_table_get_instance_private (table);

  g_clear_object (&priv->ui_settings);
  g_clear_object (&priv->store);
  g_clear_object (&priv->selection);
  g_clear_object (&priv->id_sorter);
  g_clear_object (&priv->status_sorter);
  g_clear_object (&priv->translation_sorter);
  g_clear_object (&priv->msg_sorter);

  G_OBJECT_CLASS (gtr_message_table_parent_class)->finalize (object);
}

static void
gtr_message_table_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GtrMessageTable *table = GTR_MESSAGE_TABLE (object);
  GtrMessageTablePrivate *priv;

  priv = gtr_message_table_get_instance_private (table);

  switch (prop_id)
    {
    case PROP_TAB:
      priv->tab = GTR_TAB (g_value_get_object (value));
      g_signal_connect (priv->tab,
                        "showed-message", G_CALLBACK (showed_message_cb), table);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_message_table_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GtrMessageTable *table = GTR_MESSAGE_TABLE (object);
  GtrMessageTablePrivate *priv;

  priv = gtr_message_table_get_instance_private (table);

  switch (prop_id)
    {
    case PROP_TAB:
      g_value_set_object (value, priv->tab);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
gtr_message_table_class_init (GtrMessageTableClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtr_message_table_finalize;
  object_class->set_property = gtr_message_table_set_property;
  object_class->get_property = gtr_message_table_get_property;

  g_object_class_install_property (object_class,
                                   PROP_TAB,
                                   g_param_spec_object ("tab",
                                                        "TAB",
                                                        "The active tab",
                                                        GTR_TYPE_TAB,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY));

  g_type_ensure (gtr_message_table_row_get_type ());

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-message-table.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrMessageTable, messages);

  gtk_widget_class_bind_template_child_private (widget_class, GtrMessageTable, idlabel);
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
  return GTK_WIDGET (g_object_new (GTR_TYPE_MESSAGE_TABLE, NULL));
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
  int count = 0;
  int i = 0;

  priv = gtr_message_table_get_instance_private (table);

  if (priv->store)
    {
      gtk_list_view_set_model (GTK_LIST_VIEW (priv->messages), NULL);
      g_object_unref (priv->store);
      g_object_unref (priv->selection);
    }

  priv->store = g_list_store_new (GTR_TYPE_MSG);
  gtk_sort_list_model_set_model (priv->sort_model, G_LIST_MODEL (priv->store));
  priv->selection = gtk_single_selection_new (G_LIST_MODEL (priv->sort_model));

  gtk_list_view_set_model (GTK_LIST_VIEW (priv->messages), GTK_SELECTION_MODEL (priv->selection));
  g_signal_connect (priv->selection,
                    "notify::selected",
                    G_CALLBACK (gtr_message_table_selection_changed),
                    table);

  // Add all messages to the model
  count = gtr_message_container_get_count (container);
  for (i=0; i < count; i++)
    {
      GtrMsg *msg = gtr_message_container_get_message (container, i);
      g_list_store_append (priv->store, msg);
    }

  gtr_message_table_sort_by (table, priv->sort_status);
}

/**
 * gtr_message_table_navigate:
 * @table:
 * @navigation:
 * @func: (scope call):
 *
 * Returns: (transfer full):
 */
GtrMsg *
gtr_message_table_navigate (GtrMessageTable * table,
                            GtrMessageTableNavigation navigation,
                            GtrMessageTableNavigationFunc func)
{
  GtrMessageTablePrivate *priv = NULL;

  priv = gtr_message_table_get_instance_private (table);

  switch (navigation)
    {
    case GTR_NAVIGATE_FIRST:
      gtk_single_selection_set_selected (GTK_SINGLE_SELECTION (priv->selection), 0);
      break;
    case GTR_NAVIGATE_LAST:
      {
        int n_children = g_list_model_get_n_items (G_LIST_MODEL (priv->selection));
        gtk_single_selection_set_selected (GTK_SINGLE_SELECTION (priv->selection), n_children - 1);
      }
      break;
    case GTR_NAVIGATE_NEXT:
      if (func)
        {
          int next = gtk_single_selection_get_selected (GTK_SINGLE_SELECTION (priv->selection)) + 1;
          while (TRUE)
            {
              g_autoptr(GtrMsg) msg = NULL;
              msg = GTR_MSG (g_list_model_get_object (G_LIST_MODEL (priv->sort_model), next));
              if (!msg)
                return NULL;

              if (func (msg))
                {
                  gtk_single_selection_set_selected (GTK_SINGLE_SELECTION (priv->selection), next);
                  return g_steal_pointer (&msg);
                }

              next += 1;
            }
        }
      else
        {
          int selected = gtk_single_selection_get_selected (GTK_SINGLE_SELECTION (priv->selection));
          gtk_single_selection_set_selected (GTK_SINGLE_SELECTION (priv->selection), selected + 1);
        }

      break;
    case GTR_NAVIGATE_PREV:
      if (func)
        {
          int prev = gtk_single_selection_get_selected (GTK_SINGLE_SELECTION (priv->selection)) - 1;
          while (TRUE)
            {
              g_autoptr(GtrMsg) msg = NULL;
              msg = GTR_MSG (g_list_model_get_object (G_LIST_MODEL (priv->sort_model), prev));
              if (!msg)
                return NULL;

              if (func (msg))
                {
                  gtk_single_selection_set_selected (GTK_SINGLE_SELECTION (priv->selection), prev);
                  return g_steal_pointer (&msg);
                }

              prev -= 1;
            }
        }
      else
        {
          int selected = gtk_single_selection_get_selected (GTK_SINGLE_SELECTION (priv->selection));
          gtk_single_selection_set_selected (GTK_SINGLE_SELECTION (priv->selection), selected - 1);
        }
      break;
    default:
      break;
    }

  g_autoptr(GtrMsg) msg = NULL;
  msg = g_object_ref (gtk_single_selection_get_selected_item (GTK_SINGLE_SELECTION (priv->selection)));
  return g_steal_pointer (&msg);
}

void
gtr_message_table_sort_by (GtrMessageTable *table,
                           GtrMessageTableSortBy sort)
{
  GtrMessageTablePrivate *priv;
  priv = gtr_message_table_get_instance_private (table);
  priv->sort_status = sort;

  if (!priv->store)
    return;

  switch (sort)
    {
    case GTR_MESSAGE_TABLE_SORT_STATUS:
      gtk_sort_list_model_set_sorter (priv->sort_model, priv->status_sorter);
      break;
    case GTR_MESSAGE_TABLE_SORT_MSGID:
      gtk_sort_list_model_set_sorter (priv->sort_model, priv->msg_sorter);
      break;
    case GTR_MESSAGE_TABLE_SORT_TRANSLATED:
      gtk_sort_list_model_set_sorter (priv->sort_model, priv->translation_sorter);
      break;
    case GTR_MESSAGE_TABLE_SORT_ID:
    default:
      gtk_sort_list_model_set_sorter (priv->sort_model, priv->id_sorter);
    }
}

/**
 * gtr_message_table_select:
 * @msg: the #GtrMsg to select in the list view
 *
 * Select the @msg in the list view
 */
void
gtr_message_table_select (GtrMessageTable * table, GtrMsg * msg)
{
  GtrMessageTablePrivate *priv = NULL;
  unsigned int position = 0;
  int i = 0;
  int nitems = 0;

  priv = gtr_message_table_get_instance_private (table);
  nitems = g_list_model_get_n_items (G_LIST_MODEL (priv->selection));

  // look for the msg in the model
  for (i=0; i < nitems; i++)
    {
      g_autoptr(GtrMsg) item = NULL;
      item = g_list_model_get_item (G_LIST_MODEL (priv->selection), i);
      if (item == msg)
        {
          position = i;
          break;
        }
    }
  gtk_single_selection_set_selected (GTK_SINGLE_SELECTION (priv->selection), position);
}

GtkSingleSelection *
gtr_message_table_get_selection_model (GtrMessageTable *table)
{
  GtrMessageTablePrivate *priv = gtr_message_table_get_instance_private (table);
  return priv->selection;
}

GListStore *
gtr_message_table_get_model (GtrMessageTable *table)
{
  GtrMessageTablePrivate *priv
      = gtr_message_table_get_instance_private (table);
  return priv->store;
}
