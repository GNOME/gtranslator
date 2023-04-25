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
#include "gtr-translation-memory.h"
#include "gtr-translation-memory-ui.h"
#include "gtr-tab.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "gtr-debug.h"

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define MAX_ELEMENTS 9

typedef struct
{
  GtrTranslationMemory *translation_memory;
  GtkWidget *tree_view;
  GtrTab *tab;

  gchar **tm_list;
  gint *tm_list_id;

  GtkWidget *popup_menu;
  GtrMsg *msg;
} GtrTranslationMemoryUiPrivate;


G_DEFINE_TYPE_WITH_PRIVATE (GtrTranslationMemoryUi, gtr_translation_memory_ui, GTK_TYPE_FRAME)

enum
{
  SHORTCUT_COLUMN,
  LEVEL_COLUMN,
  STRING_COLUMN,
  N_COLUMNS
};

static void
tree_view_size_cb (GtkWidget * widget,
                   GtkAllocation * allocation,
                   gpointer user_data);

static void
choose_translation (GtrTranslationMemoryUi *tm_ui, const gchar *translation)
{
  GtrView *view;
  GtkTextBuffer *buffer;
  GtrPo *po;
  GList *current_msg = NULL;
  GtrMsg *msg;
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);

  view = gtr_tab_get_active_view (priv->tab);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  po = gtr_tab_get_po (priv->tab);
  current_msg = gtr_po_get_current_message (po);

  msg = GTR_MSG (current_msg->data);

  gtr_msg_set_msgstr (msg, translation);

  gtk_text_buffer_begin_user_action (buffer);
  gtk_text_buffer_set_text (buffer, translation, -1);
  gtk_text_buffer_end_user_action (buffer);

  gtr_po_set_state (po, GTR_PO_STATE_MODIFIED);
}

static void
free_match (gpointer data)
{
  GtrTranslationMemoryMatch *match = (GtrTranslationMemoryMatch *) data;

  g_free (match->match);
  g_slice_free (GtrTranslationMemoryMatch, match);
}

static void
showed_message_cb (GtrTab *tab, GtrMsg *msg, GtrTranslationMemoryUi *tm_ui)
{
  GtkListStore *model;
  GtkTreeIter iter;
  GtkTreeViewColumn *level_column;
  const gchar *msgid;
  gint i;
  GList *tm_list = NULL;
  GList *l = NULL;
  GList *renderers_list = NULL;
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);

  model = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (priv->tree_view)));

  /*g_signal_connect (priv->tree_view,
                    "size_allocate",
                    G_CALLBACK (tree_view_size_cb), priv->tree_view);*/

  if (priv->msg)
    g_object_unref (priv->msg);
  priv->msg = g_object_ref (msg);

  msgid = gtr_msg_get_msgid (msg);

  tm_list = gtr_translation_memory_lookup (priv->translation_memory, msgid);
  g_strfreev (priv->tm_list);

  gtk_list_store_clear (model);
  priv->tm_list = g_new (gchar *, MAX_ELEMENTS + 1);
  priv->tm_list_id = g_new (gint, MAX_ELEMENTS + 1);

  i = 0;
  for (l = tm_list; l && i < MAX_ELEMENTS; l = l->next)
    {
      GtrTranslationMemoryMatch *match = (GtrTranslationMemoryMatch *) l->data;

      priv->tm_list_id[i] = match->id;
      priv->tm_list[i] = g_strdup (match->match);
      level_column = gtk_tree_view_get_column (GTK_TREE_VIEW (priv->tree_view), 0);
      renderers_list = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (level_column));

      g_object_set (renderers_list->data,
                    "accel-mods", GDK_CONTROL_MASK, NULL);
      g_list_free (renderers_list);

      gtk_list_store_append (model, &iter);
      gtk_list_store_set (model, &iter,
                          SHORTCUT_COLUMN, GDK_KEY_1 + i,
                          STRING_COLUMN, match->match,
                          LEVEL_COLUMN, match->level,
                          -1);
      i++;
    }

  /* Ensure last element is NULL */
  priv->tm_list[i] = NULL;

  g_list_free_full (tm_list, free_match);
}

static void
tree_view_row_activated (GtkTreeView *tree_view,
                         GtkTreePath *path,
                         GtkTreeViewColumn *column,
                         GtrTranslationMemoryUi *tm_ui)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *translation;

  model = gtk_tree_view_get_model (tree_view);

  if (!gtk_tree_model_get_iter (model, &iter, path))
    return;

  gtk_tree_model_get (model, &iter,
                      STRING_COLUMN, &translation,
                      -1);

  choose_translation (tm_ui, translation);

  g_free (translation);
}

/*static void
popup_menu_translation_activate (GtkMenuItem *menuitem,
                                 GtrTranslationMemoryUi *tm_ui)
{
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *translation;
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->tree_view));
  if (!selection || !gtk_tree_selection_get_selected (selection, &model, &iter))
    return;

  gtk_tree_model_get (model, &iter,
                      STRING_COLUMN, &translation,
                      -1);

  choose_translation (tm_ui, translation);

  g_free (translation);
}*/

/*static void
popup_menu_remove_from_memory (GtkMenuItem *menuitem,
                               GtrTranslationMemoryUi *tm_ui)
{
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gint i;
  gchar *translation;
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->tree_view));
  if (!selection || !gtk_tree_selection_get_selected (selection, &model, &iter))
    return;

  gtk_tree_model_get (model, &iter,
                      STRING_COLUMN, &translation,
                      -1);

  for (i = 0; priv->tm_list[i]; i++)
    if (!strcmp (priv->tm_list[i], translation))
      break;

  gtr_translation_memory_remove (priv->translation_memory, priv->tm_list_id[i]);

  g_free (translation);

  // update list //
  showed_message_cb (priv->tab, priv->msg, tm_ui);
}*/

/*static GtkWidget *
create_tree_popup_menu (GtrTranslationMemoryUi *self)
{
  GtkWidget *menu;
  GtkWidget *item;

  menu = gtk_menu_new ();

  item = gtk_menu_item_new_with_mnemonic (_("_Use this translation"));
  //g_signal_connect (item, "activate",
                    //G_CALLBACK (popup_menu_translation_activate), self);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

  item = gtk_menu_item_new_with_mnemonic (_("_Remove"));
  //g_signal_connect (item, "activate",
                    //G_CALLBACK (popup_menu_remove_from_memory), self);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  gtk_widget_set_sensitive (item, TRUE);

  gtk_widget_show_all (menu);

  return menu;
}*/

/*static void
tree_popup_menu_detach (GtrTranslationMemoryUi *self, GtkMenu * menu)
{
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (self);
  priv->popup_menu = NULL;
}*/

/*static void
gtr_translation_memory_ui_show_menu (GtrTranslationMemoryUi *self,
                                     GdkEventButton * event)
{
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (self);
  if (priv->popup_menu)
    gtk_widget_destroy (priv->popup_menu);

  priv->popup_menu = create_tree_popup_menu (self);

  gtk_menu_attach_to_widget (GTK_MENU (priv->popup_menu),
                             GTK_WIDGET (self),
                             (GtkMenuDetachFunc) tree_popup_menu_detach);

  if (event != NULL)
    gtk_menu_popup_at_pointer (GTK_MENU (priv->popup_menu), (GdkEvent*)event);
  else
    {
      GdkEvent *ev = gtk_get_current_event ();
      gtk_menu_popup_at_pointer (GTK_MENU (priv->popup_menu), ev);
    }
}*/

static void
tree_view_size_cb (GtkWidget * widget,
                   GtkAllocation * allocation,
                   gpointer user_data)
{
  GtkTreeView *treeview;
  GtkTreeViewColumn *column;
  GList *renderers_list = NULL;
  gint size;

  treeview = GTK_TREE_VIEW (user_data);

  column = gtk_tree_view_get_column (treeview, 2);
  renderers_list = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (column));

  size = gtk_tree_view_column_get_width (column);

  g_object_set (renderers_list->data, "wrap-width", MAX(1, size - 10), NULL);

  g_list_free (renderers_list);
}

/*static gboolean
tree_view_button_press_event (GtkTreeView *tree,
                              GdkEventButton *event,
                              GtrTranslationMemoryUi *tm_ui)
{
  GtkTreePath *path;

  if (GDK_BUTTON_PRESS != event->type || 3 != event->button)
    return FALSE;

  if (!gtk_tree_view_get_path_at_pos (tree, event->x, event->y,
                                      &path, NULL, NULL, NULL))
    return FALSE;

  gtk_widget_grab_focus (GTK_WIDGET (tree));
  gtk_tree_view_set_cursor (tree, path, NULL, FALSE);

  //gtr_translation_memory_ui_show_menu (tm_ui, event);
  return TRUE;
}*/

/*static gboolean
tree_view_popup_menu (GtkTreeView *tree, GtrTranslationMemoryUi *tm_ui)
{
  gtr_translation_memory_ui_show_menu (tm_ui, NULL);
  return TRUE;
}*/

static void
gtr_translation_memory_ui_init (GtrTranslationMemoryUi * tm_ui)
{
  GtkListStore *model;
  GtkCellRenderer *level_renderer, *string_renderer, *shortcut_renderer;
  GtkTreeViewColumn *shortcut, *string, *level;
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);

  priv->tm_list = NULL;
  priv->popup_menu = NULL;
  priv->msg = NULL;

  priv->tree_view = gtk_tree_view_new ();
  gtk_widget_show (priv->tree_view);

  model = gtk_list_store_new (N_COLUMNS, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING);
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
                "yalign", 0.0,
                "wrap-mode", PANGO_WRAP_WORD_CHAR,
                NULL);

  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->tree_view), string);

  /*g_signal_connect (priv->tree_view, "button-press-event",
                    G_CALLBACK (tree_view_button_press_event), tm_ui);*/

  /*g_signal_connect (priv->tree_view, "popup-menu",
                    G_CALLBACK (tree_view_popup_menu), tm_ui);*/

  g_signal_connect (priv->tree_view, "row-activated",
                    G_CALLBACK (tree_view_row_activated), tm_ui);
}

static void
gtr_translation_memory_ui_dispose (GObject * object)
{
  GtrTranslationMemoryUi *tm_ui = GTR_TRANSLATION_MEMORY_UI (object);
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);

  DEBUG_PRINT ("Dispose translation memory ui");

  g_clear_object (&priv->msg);

  G_OBJECT_CLASS (gtr_translation_memory_ui_parent_class)->dispose (object);
}

static void
gtr_translation_memory_ui_finalize (GObject * object)
{
  GtrTranslationMemoryUi *tm_ui = GTR_TRANSLATION_MEMORY_UI (object);
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);

  DEBUG_PRINT ("Finalize translation memory ui");

  g_strfreev (priv->tm_list);

  G_OBJECT_CLASS (gtr_translation_memory_ui_parent_class)->finalize (object);
}

static void
gtr_translation_memory_ui_class_init (GtrTranslationMemoryUiClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gtr_translation_memory_ui_dispose;
  object_class->finalize = gtr_translation_memory_ui_finalize;
}

GtkWidget *
gtr_translation_memory_ui_new (GtkWidget *tab,
                               GtrTranslationMemory *translation_memory)
{
  GtrTranslationMemoryUi *tm_ui;
  GtrTranslationMemoryUiPrivate *priv;
  tm_ui = g_object_new (GTR_TYPE_TRANSLATION_MEMORY_UI, NULL);

  priv = gtr_translation_memory_ui_get_instance_private (tm_ui);
  priv->tab = GTR_TAB (tab);
  priv->translation_memory = translation_memory;

  g_signal_connect (tab,
                    "showed-message", G_CALLBACK (showed_message_cb), tm_ui);

  /* Scrolledwindow needs to be realized to add a widget */
  gtk_frame_set_child (GTK_FRAME (tm_ui), priv->tree_view);

  return GTK_WIDGET (tm_ui);
}
