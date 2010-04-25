/*
 * Copyright (C) 2008 Igalia  
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-application.h"
#include "gtr-po.h"
#include "gtr-prefs-manager.h"
#include "gtr-translation-memory.h"
#include "gtr-translation-memory-ui.h"
#include "gtr-tab.h"
#include "gtr-window.h"

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define MAX_ELEMENTS 10

#define GTR_TRANSLATION_MEMORY_UI_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
							(object),	               \
							GTR_TYPE_TRANSLATION_MEMORY_UI,	       \
							GtrTranslationMemoryUiPrivate))

G_DEFINE_TYPE (GtrTranslationMemoryUi,
               gtr_translation_memory_ui, GTK_TYPE_SCROLLED_WINDOW)
     struct _GtrTranslationMemoryUiPrivate
     {
       GtkWidget *tree_view;
       GtrTab *tab;

       gchar **tm_list;
     };

     enum
     {
       SHORTCUT_COLUMN,
       LEVEL_COLUMN,
       STRING_COLUMN,
       N_COLUMNS
     };

     static void
       tree_view_size_cb (GtkWidget * widget,
                          GtkAllocation * allocation, gpointer user_data);

     static void
       on_activate_item_cb (GtkMenuItem * menuitem,
                            GtrTranslationMemoryUi * tm_ui)
{
  GtrView *view;
  GtkTextBuffer *buffer;
  GtrPo *po;
  GList *current_msg = NULL;
  GtrMsg *msg;
  gint index;
  GtrWindow *window;

  window = gtr_application_get_active_window (GTR_APP);

  view = gtr_window_get_active_view (window);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  /* Possible this hack is not going to work with all languages neither, we
     are supposing the integer at the end of the string */
  index = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (menuitem), "option"));

  po = gtr_tab_get_po (tm_ui->priv->tab);
  current_msg = gtr_po_get_current_message (po);

  msg = GTR_MSG (current_msg->data);

  gtr_msg_set_msgstr (msg, tm_ui->priv->tm_list[index - 1]);

  gtk_text_buffer_begin_user_action (buffer);
  gtk_text_buffer_set_text (buffer, tm_ui->priv->tm_list[index - 1], -1);
  gtk_text_buffer_end_user_action (buffer);

  gtr_po_set_state (po, GTR_PO_STATE_MODIFIED);
}

static void
free_list (gpointer data, gpointer useless)
{
  GtrTranslationMemoryMatch *match = (GtrTranslationMemoryMatch *) data;

  g_free (match->match);
  g_slice_free (GtrTranslationMemoryMatch, match);
}

static void
showed_message_cb (GtrTab * tab, GtrMsg * msg, GtrTranslationMemoryUi * tm_ui)
{
  GtrTranslationMemory *tm;
  GtkListStore *model;
  GtkTreeIter iter;
  GtkTreeViewColumn *level_column;
  const gchar *msgid;
  gint i = 1;
  gint j = 1;
  gint k = 0;
  GList *tm_list = NULL;
  GList *l = NULL;
  GList *renderers_list = NULL;
  GtkWidget *tm_item;
  GtkWidget *tm_menu;
  GtkWidget *items_menu;
  GtrWindow *window;
  GtkUIManager *manager;
  gchar *item_name;

  model =
    GTK_LIST_STORE (gtk_tree_view_get_model
                    (GTK_TREE_VIEW (tm_ui->priv->tree_view)));

  window = gtr_application_get_active_window (GTR_APP);
  tm_menu = gtr_window_get_tm_menu (window);

  g_signal_connect (tm_ui->priv->tree_view,
                    "size_allocate",
                    G_CALLBACK (tree_view_size_cb), tm_ui->priv->tree_view);

  msgid = gtr_msg_get_msgid (msg);

  tm =
    GTR_TRANSLATION_MEMORY (gtr_application_get_translation_memory (GTR_APP));

  tm_list = gtr_translation_memory_lookup (tm, msgid);

  if (tm_list == NULL)
    {
      gtk_widget_set_sensitive (tm_menu, FALSE);
    }
  else
    {
      gtk_widget_set_sensitive (tm_menu, TRUE);
    }

  gtk_list_store_clear (model);

  g_strfreev (tm_ui->priv->tm_list);
  tm_ui->priv->tm_list = g_new (gchar *, MAX_ELEMENTS + 1);

  for (l = tm_list; l; l = l->next)
    {
      GtrTranslationMemoryMatch *match;
      match = (GtrTranslationMemoryMatch *) l->data;
      tm_ui->priv->tm_list[i - 1] = g_strdup (match->match);
      level_column =
        gtk_tree_view_get_column (GTK_TREE_VIEW (tm_ui->priv->tree_view), 0);
      renderers_list =
        gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (level_column));

      g_object_set (renderers_list->data,
                    "accel-mods", GDK_CONTROL_MASK, NULL);
      g_list_free (renderers_list);

      gtk_list_store_append (model, &iter);
      gtk_list_store_set (model,
                          &iter,
                          SHORTCUT_COLUMN,
                          GDK_0 + k,
                          STRING_COLUMN,
                          match->match, LEVEL_COLUMN, match->level, -1);
      i++;
      k++;
      if (k == MAX_ELEMENTS)
        break;
    }

  /* Ensure last element is NULL */
  tm_ui->priv->tm_list[i - 1] = NULL;

  /* MenuBar stuff */

  items_menu = gtk_menu_new ();

  manager = gtr_window_get_ui_manager (window);

  gtk_menu_set_accel_group (GTK_MENU (items_menu),
                            gtk_ui_manager_get_accel_group (manager));

  do
    {
      gchar *accel_path;

      item_name = g_strdup_printf (_("Insert Option nº %d"), j);

      tm_item = gtk_menu_item_new_with_label (item_name);
      g_object_set_data (G_OBJECT (tm_item), "option", GINT_TO_POINTER (j));
      gtk_widget_show (tm_item);

      accel_path =
        g_strdup_printf ("<Gtr-sheet>/Edit/_Translation Memory/%s",
                         item_name);

      gtk_menu_item_set_accel_path (GTK_MENU_ITEM (tm_item), accel_path);
      gtk_accel_map_add_entry (accel_path, GDK_0 + (j - 1), GDK_CONTROL_MASK);

      g_free (accel_path);
      g_free (item_name);

      g_signal_connect (tm_item, "activate",
                        G_CALLBACK (on_activate_item_cb), tm_ui);

      gtk_menu_shell_append (GTK_MENU_SHELL (items_menu), tm_item);

      j++;
      if (j > MAX_ELEMENTS)
        break;

    }
  while ((tm_list = g_list_next (tm_list)));

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (tm_menu), items_menu);

  /* Freeing the list */
  g_list_foreach (tm_list, (GFunc) free_list, NULL);
  g_list_free (tm_list);
}

static void
tree_view_size_cb (GtkWidget * widget,
                   GtkAllocation * allocation, gpointer user_data)
{
  GtkTreeView *treeview;
  GtkTreeViewColumn *column;
  GList *renderers_list = NULL;
  gint size;

  treeview = GTK_TREE_VIEW (user_data);

  column = gtk_tree_view_get_column (treeview, 2);
  renderers_list = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (column));

  size = gtk_tree_view_column_get_width (column);

  g_object_set (renderers_list->data, "wrap-width", size - 10, NULL);

  g_list_free (renderers_list);
}


static void
gtr_translation_memory_ui_draw (GtrTranslationMemoryUi * tm_ui)
{
  GtrTranslationMemoryUiPrivate *priv = tm_ui->priv;
  GtkListStore *model;
  GtkCellRenderer *level_renderer, *string_renderer, *shortcut_renderer;
  GtkTreeViewColumn *shortcut, *string, *level;

  priv->tree_view = gtk_tree_view_new ();
  gtk_widget_show (priv->tree_view);

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (tm_ui),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  model =
    gtk_list_store_new (N_COLUMNS, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (priv->tree_view),
                           GTK_TREE_MODEL (model));

  shortcut_renderer = gtk_cell_renderer_accel_new ();
  shortcut = gtk_tree_view_column_new_with_attributes (_("Shortcut"),
                                                       shortcut_renderer,
                                                       "accel-key",
                                                       SHORTCUT_COLUMN, NULL);
  g_object_set (shortcut_renderer, "width", 80, NULL);

  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->tree_view), shortcut);

  level_renderer = gtk_cell_renderer_progress_new ();
  level = gtk_tree_view_column_new_with_attributes (_("Level"),
                                                    level_renderer,
                                                    "value", LEVEL_COLUMN,
                                                    NULL);
  g_object_set (level_renderer, "width", 80, NULL);

  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->tree_view), level);

  string_renderer = gtk_cell_renderer_text_new ();
  string = gtk_tree_view_column_new_with_attributes (_("String"),
                                                     string_renderer,
                                                     "text", STRING_COLUMN,
                                                     NULL);
  gtk_tree_view_column_set_sizing (string, GTK_TREE_VIEW_COLUMN_FIXED);

  g_object_set (string_renderer,
                "ypad", 0,
                "xpad", 5,
                "yalign", 0.0, "wrap-mode", PANGO_WRAP_WORD_CHAR, NULL);

  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->tree_view), string);
}

static void
gtr_translation_memory_ui_init (GtrTranslationMemoryUi * tm_ui)
{
  tm_ui->priv = GTR_TRANSLATION_MEMORY_UI_GET_PRIVATE (tm_ui);
  tm_ui->priv->tm_list = NULL;

  gtr_translation_memory_ui_draw (tm_ui);
}

static void
gtr_translation_memory_ui_finalize (GObject * object)
{
  GtrTranslationMemoryUi *tm_ui = GTR_TRANSLATION_MEMORY_UI (object);

  g_strfreev (tm_ui->priv->tm_list);

  G_OBJECT_CLASS (gtr_translation_memory_ui_parent_class)->finalize (object);
}

static void
  gtr_translation_memory_ui_class_init (GtrTranslationMemoryUiClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrTranslationMemoryUiPrivate));

  object_class->finalize = gtr_translation_memory_ui_finalize;
}

GtkWidget *
gtr_translation_memory_ui_new (GtkWidget * tab)
{
  GtrTranslationMemoryUi *tm_ui;
  tm_ui = g_object_new (GTR_TYPE_TRANSLATION_MEMORY_UI, NULL);

  tm_ui->priv->tab = GTR_TAB (tab);
  g_signal_connect (tab,
                    "showed-message", G_CALLBACK (showed_message_cb), tm_ui);

  /* Scrolledwindow needs to be realized to add a widget */
  gtk_container_add (GTK_CONTAINER (tm_ui), tm_ui->priv->tree_view);

  return GTK_WIDGET (tm_ui);
}
