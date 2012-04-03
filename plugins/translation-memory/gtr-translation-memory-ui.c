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

#define MAX_ELEMENTS 10

#define GTR_TRANSLATION_MEMORY_UI_GET_PRIVATE(object)           \
  (G_TYPE_INSTANCE_GET_PRIVATE ((object),                       \
                                GTR_TYPE_TRANSLATION_MEMORY_UI, \
                                GtrTranslationMemoryUiPrivate))

G_DEFINE_TYPE (GtrTranslationMemoryUi, gtr_translation_memory_ui, GTK_TYPE_SCROLLED_WINDOW)

struct _GtrTranslationMemoryUiPrivate
{
  GtrTranslationMemory *translation_memory;
  GtkWidget *tree_view;
  GtrTab *tab;

  gchar **tm_list;

  GtkWidget *popup_menu;
  GtrMsg *msg;
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

  view = gtr_tab_get_active_view (tm_ui->priv->tab);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  po = gtr_tab_get_po (tm_ui->priv->tab);
  current_msg = gtr_po_get_current_message (po);

  msg = GTR_MSG (current_msg->data);

  gtr_msg_set_msgstr (msg, translation);

  gtk_text_buffer_begin_user_action (buffer);
  gtk_text_buffer_set_text (buffer, translation, -1);
  gtk_text_buffer_end_user_action (buffer);

  gtr_po_set_state (po, GTR_PO_STATE_MODIFIED);
}

static void
on_activate_item_cb (GtkMenuItem            *menuitem,
                     GtrTranslationMemoryUi *tm_ui)
{
  gint index;

  /* Possible this hack is not going to work with all languages neither, we
     are supposing the integer at the end of the string */
  index = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (menuitem), "option"));

  choose_translation (tm_ui, tm_ui->priv->tm_list[index - 1]);
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
  gint i = 1;
  gint j = 1;
  gint k = 0;
  GList *tm_list = NULL;
  GList *l = NULL;
  GList *renderers_list = NULL;
  GtkWidget *tm_item;
  GtkWidget *tm_menu;
  GtkWidget *items_menu;
  GtkWidget *window;
  GtkUIManager *manager;
  gchar *item_name;

  model = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (tm_ui->priv->tree_view)));

  window = gtk_widget_get_toplevel (GTK_WIDGET (tm_ui));
  manager = gtr_window_get_ui_manager (GTR_WINDOW (window));
  tm_menu = gtk_ui_manager_get_widget (manager, "/MainMenu/EditMenu/EditOps_1/EditTranslationMemory");

  g_signal_connect (tm_ui->priv->tree_view,
                    "size_allocate",
                    G_CALLBACK (tree_view_size_cb), tm_ui->priv->tree_view);

  g_object_ref (msg);
  if (tm_ui->priv->msg)
    g_object_unref (tm_ui->priv->msg);
  tm_ui->priv->msg = msg;

  msgid = gtr_msg_get_msgid (msg);

  tm_list = gtr_translation_memory_lookup (tm_ui->priv->translation_memory, msgid);

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
      gtk_list_store_set (model, &iter,
                          SHORTCUT_COLUMN, GDK_KEY_1 + k,
                          STRING_COLUMN, match->match,
                          LEVEL_COLUMN, match->level,
                          -1);
      i++;
      k++;
      if (k == MAX_ELEMENTS - 1)
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

      accel_path = g_strdup_printf ("<Gtr-sheet>/Edit/Translation Memory/%s",
                                    item_name);

      gtk_menu_item_set_accel_path (GTK_MENU_ITEM (tm_item), accel_path);
      gtk_accel_map_add_entry (accel_path, GDK_KEY_1 + (j - 1), GDK_CONTROL_MASK);

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

static void
popup_menu_translation_activate (GtkMenuItem *menuitem,
                                 GtrTranslationMemoryUi *tm_ui)
{
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *translation;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tm_ui->priv->tree_view));
  if (!selection || !gtk_tree_selection_get_selected (selection, &model, &iter))
    return;

  gtk_tree_model_get (model, &iter,
                      STRING_COLUMN, &translation,
                      -1);

  choose_translation (tm_ui, translation);

  g_free (translation);
}

static void
popup_menu_remove_from_memory (GtkMenuItem *menuitem,
                               GtrTranslationMemoryUi *tm_ui)
{
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gint level;
  const gchar *original;
  gchar *translation;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tm_ui->priv->tree_view));
  if (!selection || !gtk_tree_selection_get_selected (selection, &model, &iter))
    return;

  gtk_tree_model_get (model, &iter,
                      LEVEL_COLUMN, &level,
                      -1);

  if (level != 100)
    return;

  gtk_tree_model_get (model, &iter,
                      STRING_COLUMN, &translation,
                      -1);

  original = gtr_msg_get_msgid (tm_ui->priv->msg);
  gtr_translation_memory_remove (tm_ui->priv->translation_memory, original, translation);

  g_free (translation);

  /* update list */
  showed_message_cb (tm_ui->priv->tab, tm_ui->priv->msg, tm_ui);
}

static GtkWidget *
create_tree_popup_menu (GtrTranslationMemoryUi *self)
{
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gboolean remove_available = FALSE;
  GtkWidget *menu;
  GtkWidget *item;
  GtkWidget *image;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self->priv->tree_view));
  if (selection && gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      gint level;

      gtk_tree_model_get (model, &iter,
                          LEVEL_COLUMN, &level,
                          -1);

      remove_available = (level == 100);
    }

  menu = gtk_menu_new ();

  item = gtk_menu_item_new_with_mnemonic (_("_Use this translation"));
  g_signal_connect (item, "activate",
                    G_CALLBACK (popup_menu_translation_activate), self);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

  item = gtk_image_menu_item_new_with_mnemonic (_("_Remove"));
  image = gtk_image_new_from_stock (GTK_STOCK_DELETE, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), image);
  g_signal_connect (item, "activate",
                    G_CALLBACK (popup_menu_remove_from_memory), self);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  gtk_widget_set_sensitive (item, remove_available);

  gtk_widget_show_all (menu);

  return menu;
}

static void
tree_popup_menu_detach (GtrTranslationMemoryUi *self, GtkMenu * menu)
{
  self->priv->popup_menu = NULL;
}

static void
gtr_translation_memory_ui_show_menu (GtrTranslationMemoryUi *self,
                                     GdkEventButton * event)
{
  if (self->priv->popup_menu)
    gtk_widget_destroy (self->priv->popup_menu);

  self->priv->popup_menu = create_tree_popup_menu (self);

  gtk_menu_attach_to_widget (GTK_MENU (self->priv->popup_menu),
                             GTK_WIDGET (self),
                             (GtkMenuDetachFunc) tree_popup_menu_detach);

  if (event != NULL)
    gtk_menu_popup (GTK_MENU (self->priv->popup_menu), NULL, NULL,
                    NULL, NULL,
                    event->button, event->time);
  else
    gtk_menu_popup (GTK_MENU (self->priv->popup_menu), NULL, NULL,
                    gtr_utils_menu_position_under_tree_view,
                    self->priv->tree_view,
                    0, gtk_get_current_event_time ());
}

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

  g_object_set (renderers_list->data, "wrap-width", size - 10, NULL);

  g_list_free (renderers_list);
}

static gboolean
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

  gtr_translation_memory_ui_show_menu (tm_ui, event);
  return TRUE;
}

static gboolean
tree_view_popup_menu (GtkTreeView *tree, GtrTranslationMemoryUi *tm_ui)
{
  gtr_translation_memory_ui_show_menu (tm_ui, NULL);
  return TRUE;
}

static void
gtr_translation_memory_ui_init (GtrTranslationMemoryUi * tm_ui)
{
  GtrTranslationMemoryUiPrivate *priv;
  GtkListStore *model;
  GtkCellRenderer *level_renderer, *string_renderer, *shortcut_renderer;
  GtkTreeViewColumn *shortcut, *string, *level;

  tm_ui->priv = GTR_TRANSLATION_MEMORY_UI_GET_PRIVATE (tm_ui);
  priv = tm_ui->priv;
  tm_ui->priv->tm_list = NULL;
  tm_ui->priv->popup_menu = NULL;
  tm_ui->priv->msg = NULL;

  priv->tree_view = gtk_tree_view_new ();
  gtk_widget_show (priv->tree_view);

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (tm_ui),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

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

  g_signal_connect (priv->tree_view, "button-press-event",
                    G_CALLBACK (tree_view_button_press_event), tm_ui);

  g_signal_connect (priv->tree_view, "popup-menu",
                    G_CALLBACK (tree_view_popup_menu), tm_ui);

  g_signal_connect (priv->tree_view, "row-activated",
                    G_CALLBACK (tree_view_row_activated), tm_ui);
}

static void
gtr_translation_memory_ui_dispose (GObject * object)
{
  GtrTranslationMemoryUi *tm_ui = GTR_TRANSLATION_MEMORY_UI (object);

  DEBUG_PRINT ("Dispose translation memory ui");

  if (tm_ui->priv->msg)
    {
      g_object_unref (tm_ui->priv->msg);
      tm_ui->priv->msg = NULL;
    }

  G_OBJECT_CLASS (gtr_translation_memory_ui_parent_class)->dispose (object);
}

static void
gtr_translation_memory_ui_finalize (GObject * object)
{
  GtrTranslationMemoryUi *tm_ui = GTR_TRANSLATION_MEMORY_UI (object);

  DEBUG_PRINT ("Finalize translation memory ui");

  g_strfreev (tm_ui->priv->tm_list);

  G_OBJECT_CLASS (gtr_translation_memory_ui_parent_class)->finalize (object);
}

static void
gtr_translation_memory_ui_class_init (GtrTranslationMemoryUiClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrTranslationMemoryUiPrivate));

  object_class->dispose = gtr_translation_memory_ui_dispose;
  object_class->finalize = gtr_translation_memory_ui_finalize;
}

GtkWidget *
gtr_translation_memory_ui_new (GtkWidget *tab,
                               GtrTranslationMemory *translation_memory)
{
  GtrTranslationMemoryUi *tm_ui;
  tm_ui = g_object_new (GTR_TYPE_TRANSLATION_MEMORY_UI, NULL);

  tm_ui->priv->tab = GTR_TAB (tab);
  tm_ui->priv->translation_memory = translation_memory;

  g_signal_connect (tab,
                    "showed-message", G_CALLBACK (showed_message_cb), tm_ui);

  /* Scrolledwindow needs to be realized to add a widget */
  gtk_container_add (GTK_CONTAINER (tm_ui), tm_ui->priv->tree_view);

  return GTK_WIDGET (tm_ui);
}
