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

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define MAX_ELEMENTS 9

// TODO:
// Implement right click menu functionality
//   * Use this translation
//   * Delete this translation

typedef struct
{
  GtrTranslationMemory *translation_memory;
  GtkWidget *view;
  GtrTab *tab;
  GListStore *model;

  gchar **tm_list;
  gint *tm_list_id;

  GtkWidget *popup_menu;
  GtrMsg *msg;
} GtrTranslationMemoryUiPrivate;

struct _GtrTranslationMemoryUi
{
  AdwBin parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrTranslationMemoryUi, gtr_translation_memory_ui, ADW_TYPE_BIN)

// Type to use in the model to store translation memory entries
#define GTR_TYPE_TM (gtr_tm_get_type ())
G_DECLARE_FINAL_TYPE (GtrTm, gtr_tm, GTR, TM, GObject)

struct _GtrTm {
  GObject parent_instance;
  uint shortcut;
  int level;
  char *match;
};

G_DEFINE_TYPE (GtrTm, gtr_tm, G_TYPE_OBJECT);

static void
gtr_tm_init (GtrTm *tm)
{
}

static void
gtr_tm_finalize (GObject *object)
{
  GtrTm *tm = GTR_TM (object);
  g_free (tm->match);
  G_OBJECT_CLASS (gtr_tm_parent_class)->finalize (object);
}

static void
gtr_tm_class_init (GtrTmClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  object_class->finalize = gtr_tm_finalize;
}

static GtrTm *
gtr_tm_new (uint shortcut, int level, const char *match)
{
  GtrTm *tm = g_object_new (GTR_TYPE_TM, NULL);
  tm->match = g_strdup (match);
  tm->shortcut = shortcut;
  tm->level = level;
  return tm;
}

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
  const gchar *msgid;
  gint i;
  GList *tm_list = NULL;
  GList *l = NULL;
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);

  g_list_store_remove_all (priv->model);

  if (priv->msg)
    g_object_unref (priv->msg);
  priv->msg = g_object_ref (msg);

  msgid = gtr_msg_get_msgid (msg);

  tm_list = gtr_translation_memory_lookup (priv->translation_memory, msgid);
  g_strfreev (priv->tm_list);

  priv->tm_list = g_new (gchar *, MAX_ELEMENTS + 1);
  if (priv->tm_list_id)
    g_free (priv->tm_list_id);
  priv->tm_list_id = g_new (gint, MAX_ELEMENTS + 1);

  i = 0;
  for (l = tm_list; l && i < MAX_ELEMENTS; l = l->next)
    {
      GtrTranslationMemoryMatch *match = (GtrTranslationMemoryMatch *) l->data;
      g_autoptr(GtrTm) tm = gtr_tm_new (GDK_KEY_1 + i, match->level, match->match);

      priv->tm_list_id[i] = match->id;
      priv->tm_list[i] = g_strdup (match->match);

      g_list_store_append (priv->model, tm);

      i++;
    }

  /* Ensure last element is NULL */
  priv->tm_list[i] = NULL;

  g_list_free_full (tm_list, free_match);
}

static void
row_activated (GtkListView            *view,
               uint                    position,
               GtrTranslationMemoryUi *tm_ui)
{
  GListModel *model;
  g_autoptr(GtrTm) tm = NULL;

  model = G_LIST_MODEL (gtk_list_view_get_model (view));
  tm = GTR_TM (g_list_model_get_object (model, position));
  choose_translation (tm_ui, tm->match);
}

static void
setup_widget (GtkSignalListItemFactory *factory,
              GtkListItem              *list_item)
{
  GtkWidget *box, *child;

  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_widget_set_margin_top (box, 6);
  gtk_widget_set_margin_bottom (box, 6);
  gtk_list_item_set_child (list_item, box);

  // Shortcut
  child = adw_shortcut_label_new ("");
  gtk_widget_set_valign (child, GTK_ALIGN_CENTER);
  gtk_box_append (GTK_BOX (box), child);

  // Level
  child = gtk_level_bar_new ();
  gtk_widget_set_vexpand (child, FALSE);
  gtk_widget_set_valign (child, GTK_ALIGN_CENTER);
  gtk_widget_set_size_request (GTK_WIDGET (child), 50, -1);
  gtk_box_append (GTK_BOX (box), child);

  // Text
  child = gtk_label_new ("");
  gtk_widget_set_hexpand (child, TRUE);
  gtk_label_set_xalign (GTK_LABEL (child), 0.0);
  gtk_label_set_wrap (GTK_LABEL (child), TRUE);
  gtk_box_append (GTK_BOX (box), child);
}

static void
bind_widget (GtkSignalListItemFactory *factory,
             GtkListItem              *list_item)
{
  GtkWidget *box, *child;
  GtrTm *tm;
  g_autofree char *sc = NULL;

  box = gtk_list_item_get_child (list_item);
  tm = gtk_list_item_get_item (list_item);

  // shortcut
  child = gtk_widget_get_first_child (box);
  sc = g_strdup_printf ("<Ctrl>%s", gdk_keyval_name (tm->shortcut));
  adw_shortcut_label_set_accelerator (ADW_SHORTCUT_LABEL (child), sc);

  // level
  child = gtk_widget_get_next_sibling (child);
  gtk_level_bar_set_value (GTK_LEVEL_BAR (child), tm->level / 100.0);

  // text
  child = gtk_widget_get_next_sibling (child);
  gtk_label_set_text (GTK_LABEL (child), tm->match);
}

static void
gtr_translation_memory_ui_init (GtrTranslationMemoryUi * tm_ui)
{
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);
  GtkListItemFactory *factory = NULL;
  GtkSelectionModel *model = NULL;

  priv->tm_list = NULL;
  priv->popup_menu = NULL;
  priv->msg = NULL;

  factory = gtk_signal_list_item_factory_new ();
  priv->model = g_list_store_new (GTR_TYPE_TM);
  g_signal_connect (factory, "setup", G_CALLBACK (setup_widget), NULL);
  g_signal_connect (factory, "bind", G_CALLBACK (bind_widget), NULL);

  model = GTK_SELECTION_MODEL (gtk_no_selection_new ((G_LIST_MODEL (priv->model))));
  priv->view = gtk_list_view_new (model, factory);
  gtk_widget_add_css_class (priv->view, "navigation-sidebar");
  gtk_widget_set_vexpand (priv->view, TRUE);
  gtk_widget_set_hexpand (priv->view, TRUE);
  g_signal_connect (priv->view, "activate",
                    G_CALLBACK (row_activated), tm_ui);
}

static void
gtr_translation_memory_ui_dispose (GObject * object)
{
  GtrTranslationMemoryUi *tm_ui = GTR_TRANSLATION_MEMORY_UI (object);
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);

  g_debug ("Dispose translation memory ui");

  g_clear_object (&priv->msg);

  G_OBJECT_CLASS (gtr_translation_memory_ui_parent_class)->dispose (object);
}

static void
gtr_translation_memory_ui_finalize (GObject * object)
{
  GtrTranslationMemoryUi *tm_ui = GTR_TRANSLATION_MEMORY_UI (object);
  GtrTranslationMemoryUiPrivate *priv = gtr_translation_memory_ui_get_instance_private (tm_ui);

  g_debug ("Finalize translation memory ui");

  g_strfreev (priv->tm_list);
  g_free (priv->tm_list_id);

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
  adw_bin_set_child (ADW_BIN (tm_ui), priv->view);

  return GTK_WIDGET (tm_ui);
}
