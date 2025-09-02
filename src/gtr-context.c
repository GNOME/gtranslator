/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <icq@gnome.org>
 *               2022  Daniel Garcia Moreno <danigm@gnome.org>
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

#include "gtr-context.h"
#include "gtr-tab.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "translation-memory/gtr-translation-memory-ui.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

/*
 * Main object structure
 */
struct _GtrContextPanel
{
  GtkBox parent_instance;
};

/*
 * Class definition
 */
struct _GtrContextPanelClass
{
  GtkBoxClass parent_class;
  void (* reloaded) (GtrContextPanel *panel,
                     GtrMsg          *msg);
};

typedef struct
{
  GtkWidget *sw;
  GtkWidget *translation_memory_box;

  GtrTab *tab;
  GtrMsg *current_msg;

  // translation memory
  GtkWidget *translation_memory_ui;

  // notes
  GtkWidget *notes;
  GtkTextBuffer *notesbuffer;
  GtkWidget *edit_notes;

  // comments
  GtkWidget *comments;
  GtkTextBuffer *commentsbuffer;

  // paths
  GtkWidget *paths;
} GtrContextPanelPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GtrContextPanel, gtr_context_panel, GTK_TYPE_BOX)

enum
{
  PROP_0,
  PROP_TAB
};

/* Signals */
enum
{
  RELOADED,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void buffer_end_user_action (GtkTextBuffer *buffer, GtrContextPanel *panel);
static void showed_message_cb (GtrTab *tab, GtrMsg *msg, GtrContextPanel *panel);

typedef struct {
  GtrContextPanel *panel;
  GtkTextBuffer   *text_buffer;
  AdwDialog       *dialog;
} DialogData;

static void
dialog_data_free (DialogData *data)
{
  adw_dialog_close (data->dialog);

  g_free (data);
}

static void
reload_values (GtrContextPanel *panel)
{
  GtrContextPanelPrivate *priv;

  priv = gtr_context_panel_get_instance_private(panel);

  showed_message_cb (priv->tab, priv->current_msg, panel);
  g_signal_emit (G_OBJECT (panel), signals[RELOADED], 0, priv->current_msg);
}

static void
close_notes (GtkWidget *widget, gpointer user_data)
{
  DialogData *dd = user_data;
  reload_values (dd->panel);
  dialog_data_free (dd);
}

static void
save_notes (GtkWidget *widget, gpointer user_data)
{
  DialogData *dd = user_data;
  buffer_end_user_action (dd->text_buffer, dd->panel);
  close_notes (widget, user_data);
}

static void
setup_notes_edition (GtkWidget *button, GtrContextPanel *panel)
{
  GtrContextPanelPrivate *priv;
  AdwDialog *dialog;
  GtkWidget *scrolled_window;
  GtkWidget *toolbar_view;
  GtkWidget *text_view;

  GtkWidget *headerbar;
  GtkWidget *save;
  GtkWidget *cancel;

  GtkTextBuffer *text_buffer = gtk_text_buffer_new (NULL);
  DialogData *dd;

  priv = gtr_context_panel_get_instance_private (panel);

  headerbar = adw_header_bar_new ();
  cancel = gtk_button_new_with_label (_("_Cancel"));
  save = gtk_button_new_with_label (_("_Save"));
  gtk_button_set_can_shrink (GTK_BUTTON (cancel), TRUE);
  gtk_button_set_can_shrink (GTK_BUTTON (save), TRUE);
  gtk_button_set_use_underline (GTK_BUTTON (cancel), TRUE);
  gtk_button_set_use_underline (GTK_BUTTON (save), TRUE);
  gtk_widget_add_css_class (save, "suggested-action");
  adw_header_bar_pack_start (ADW_HEADER_BAR (headerbar), cancel);
  adw_header_bar_pack_end (ADW_HEADER_BAR (headerbar), save);
  adw_header_bar_set_show_start_title_buttons (ADW_HEADER_BAR (headerbar),
                                               FALSE);
  adw_header_bar_set_show_end_title_buttons (ADW_HEADER_BAR (headerbar),
                                             FALSE);

  dialog = adw_dialog_new ();
  toolbar_view = adw_toolbar_view_new ();
  gtk_widget_add_css_class (GTK_WIDGET (dialog), "view");
  adw_dialog_set_title (dialog, _("Edit Notes"));
  adw_dialog_set_child (dialog, toolbar_view);
  adw_toolbar_view_add_top_bar (ADW_TOOLBAR_VIEW (toolbar_view), headerbar);

  text_view = gtk_text_view_new_with_buffer (text_buffer);

  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (text_view),12);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (text_view),12);
  gtk_text_view_set_top_margin (GTK_TEXT_VIEW (text_view),12);
  gtk_text_view_set_bottom_margin (GTK_TEXT_VIEW (text_view),12);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_CHAR);
  gtk_text_view_set_pixels_inside_wrap (GTK_TEXT_VIEW (text_view),0);

  scrolled_window = gtk_scrolled_window_new ();
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);

  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scrolled_window), text_view);

  gtk_widget_set_vexpand (scrolled_window, TRUE);
  adw_toolbar_view_set_content (ADW_TOOLBAR_VIEW (toolbar_view), scrolled_window);

  text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
  gtk_text_buffer_set_text (text_buffer, gtr_msg_get_comment (priv->current_msg), -1);

  adw_dialog_set_focus (dialog, text_view);
  adw_dialog_set_content_height (dialog, 350);
  adw_dialog_set_content_width (dialog, 500);

  dd = g_new0 (DialogData, 1);
  dd->panel = panel;
  dd->text_buffer = text_buffer;
  dd->dialog = dialog;

  g_signal_connect (cancel, "clicked", G_CALLBACK (close_notes), dd);
  g_signal_connect (save, "clicked", G_CALLBACK (save_notes), dd);

  adw_dialog_present (dialog, GTK_WIDGET (panel));
}

static void
add_notes (GtrContextPanel *panel, GtrMsg *msg)
{
  const gchar *comments;
  GtrContextPanelPrivate *priv;
  GtkTextIter iter;

  priv = gtr_context_panel_get_instance_private (panel);
  gtk_text_buffer_set_text (priv->notesbuffer, "", 0);
  gtk_text_buffer_get_start_iter (priv->notesbuffer, &iter);

  comments = gtr_msg_get_comment (msg);
  gtk_text_buffer_insert (priv->notesbuffer, &iter, comments, -1);
}

static void
add_extracted_comments (GtrContextPanel *panel, GtrMsg *msg)
{
  const gchar *comments;
  GtrContextPanelPrivate *priv;
  GtkTextIter iter;

  priv = gtr_context_panel_get_instance_private (panel);

  gtk_text_buffer_set_text (priv->commentsbuffer, "", 0);
  gtk_text_buffer_get_start_iter (priv->commentsbuffer, &iter);

  comments = gtr_msg_get_extracted_comments (msg);
  if (comments == NULL || *comments == '\0')
    gtk_text_buffer_insert (priv->commentsbuffer, &iter, "", -1);
  else
    gtk_text_buffer_insert (priv->commentsbuffer, &iter, comments, -1);
}

static void
clean_paths (GtrContextPanel *panel)
{
  GtrContextPanelPrivate *priv = gtr_context_panel_get_instance_private (panel);

  gtk_list_box_remove_all (GTK_LIST_BOX (priv->paths));
}

static void
showed_message_cb (GtrTab *tab, GtrMsg *msg, GtrContextPanel *panel)
{
  GtrContextPanelPrivate *priv;
  priv = gtr_context_panel_get_instance_private (panel);

  /* Update current msg */
  priv->current_msg = msg;

  add_notes (panel, msg);
  add_extracted_comments (panel, msg);

  clean_paths (panel);

  // TODO: Add this to the panel
  // gtr_msg_get_format (msg)
}

static void
buffer_end_user_action (GtkTextBuffer *buffer, GtrContextPanel *panel)
{
  GtkTextIter start, end;
  gchar *text;
  GtrContextPanelPrivate *priv;
  GtrPo *po;
  GtrPoState po_state;

  priv = gtr_context_panel_get_instance_private(panel);

  gtk_text_buffer_get_bounds (buffer, &start, &end);

  text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

  gtr_msg_set_comment (priv->current_msg, text);

  g_free (text);

  /* Update the po state if needed after adding a comment */
  po = gtr_tab_get_po (priv->tab);
  po_state = gtr_po_get_state (po);
  if (po_state != GTR_PO_STATE_MODIFIED)
    gtr_po_set_state (po, GTR_PO_STATE_MODIFIED);
}

static void
gtr_context_panel_init (GtrContextPanel *panel)
{
  GtrContextPanelPrivate *priv = gtr_context_panel_get_instance_private(panel);

  gtk_widget_init_template (GTK_WIDGET (panel));
  g_signal_connect (priv->edit_notes, "clicked",
                    G_CALLBACK (setup_notes_edition), panel);
}

static void
gtr_context_panel_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GtrContextPanel *panel = GTR_CONTEXT_PANEL (object);
  GtrContextPanelPrivate *priv;

  priv = gtr_context_panel_get_instance_private(panel);

  switch (prop_id)
    {
    case PROP_TAB:
      priv->tab = GTR_TAB (g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_context_panel_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GtrContextPanel *panel = GTR_CONTEXT_PANEL (object);
  GtrContextPanelPrivate *priv;

  priv = gtr_context_panel_get_instance_private(panel);

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
gtr_context_panel_constructed (GObject *object)
{
  GtrContextPanel *panel = GTR_CONTEXT_PANEL (object);
  GtrContextPanelPrivate *priv;

  priv = gtr_context_panel_get_instance_private(panel);

  if (!priv->tab)
    {
      g_critical ("The context was not constructed well, this shouldn't happen!");
      return;
    }

  g_signal_connect (priv->tab,
                    "showed-message",
                    G_CALLBACK (showed_message_cb), panel);
}

static void
gtr_context_panel_dispose (GObject *object)
{
  g_debug ("Disposing context panel");

  G_OBJECT_CLASS (gtr_context_panel_parent_class)->dispose (object);
}

static void
gtr_context_panel_class_init (GtrContextPanelClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = gtr_context_panel_dispose;
  object_class->set_property = gtr_context_panel_set_property;
  object_class->get_property = gtr_context_panel_get_property;
  object_class->constructed = gtr_context_panel_constructed;

  signals[RELOADED] =
    g_signal_new ("reloaded",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (struct _GtrContextPanelClass, reloaded),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE,
                  1,
                  GTR_TYPE_MSG);

  g_object_class_install_property (object_class,
                                   PROP_TAB,
                                   g_param_spec_object ("tab",
                                                        "TAB",
                                                        "The active tab",
                                                        GTR_TYPE_TAB,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY));

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-context.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrContextPanel, sw);
  gtk_widget_class_bind_template_child_private (widget_class, GtrContextPanel, translation_memory_box);

  gtk_widget_class_bind_template_child_private (widget_class, GtrContextPanel, notes);
  gtk_widget_class_bind_template_child_private (widget_class, GtrContextPanel, notesbuffer);
  gtk_widget_class_bind_template_child_private (widget_class, GtrContextPanel, edit_notes);

  gtk_widget_class_bind_template_child_private (widget_class, GtrContextPanel, comments);
  gtk_widget_class_bind_template_child_private (widget_class, GtrContextPanel, commentsbuffer);

  gtk_widget_class_bind_template_child_private (widget_class, GtrContextPanel, paths);
}

/**
 * gtr_context_panel_new:
 * 
 * Creates a new #GtrContextPanel object.
 * 
 * Returns: a new #GtrContextPanel object
 */
GtkWidget *
gtr_context_panel_new (void)
{
  return g_object_new (GTR_TYPE_CONTEXT_PANEL, NULL);
}

void
gtr_context_init_tm (GtrContextPanel *panel,
                     GtrTranslationMemory *tm)
{
  GtrContextPanelPrivate *priv = gtr_context_panel_get_instance_private(panel);
  priv->translation_memory_ui = gtr_translation_memory_ui_new (GTK_WIDGET (priv->tab),
                                                               tm);
  gtk_widget_set_visible (priv->translation_memory_ui, TRUE);
  gtk_widget_set_size_request (priv->translation_memory_ui, 300, 300);

  GtkWidget* scrolled_win = gtk_scrolled_window_new ();
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW(scrolled_win), priv->translation_memory_ui);
  gtk_scrolled_window_set_min_content_width (GTK_SCROLLED_WINDOW (scrolled_win), 300);
  gtk_scrolled_window_set_min_content_height (GTK_SCROLLED_WINDOW (scrolled_win), 300);

  gtk_box_append (GTK_BOX (priv->translation_memory_box),
                  scrolled_win);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
}

typedef struct {
  char *filename;
  char *vcs_web;
  char *module_name;
  char *branch_name;
  int line;
} UriData;

static void
uri_data_free (UriData *ud)
{
  g_clear_pointer (&ud->vcs_web, g_free);
  g_clear_pointer (&ud->filename, g_free);
  g_clear_pointer (&ud->branch_name, g_free);
  g_clear_pointer (&ud->module_name, g_free);

  g_free (ud);
}

static void
on_row_activated (AdwActionRow *row,
                  gpointer      user_data)
{
  UriData *ud = user_data;

  GtrWindow *parent = GTR_WINDOW (gtk_widget_get_root (GTK_WIDGET (row)));

  gtr_window_open_file_in_browser (parent, ud->vcs_web, ud->module_name,
                                   ud->filename, ud->branch_name, ud->line);
}

void
gtr_context_add_path (GtrContextPanel *panel,
                      const char      *filename,
                      int             line)
{
  GtkWidget *row;
  GtrPo *po;
  GtrContextPanelPrivate *priv = gtr_context_panel_get_instance_private (panel);
  const char *module, *branch, *vcs_web;

  g_autofree char *text = g_strdup_printf ("%s:%d", filename, line);

  // TODO: make file path clickable it should open the dialog GtrViewer with
  // the source if it's found in the local filesystem or try to open the gnome
  // gitlab if this looks like a gnome project?
  row = adw_action_row_new ();
  adw_preferences_row_set_title (ADW_PREFERENCES_ROW (row), text);
  adw_preferences_row_set_title_selectable (ADW_PREFERENCES_ROW (row), true);

  po = gtr_tab_get_po (priv->tab);
  module = gtr_po_get_dl_module (po);
  branch = gtr_po_get_dl_branch (po);
  vcs_web = gtr_po_get_dl_vcs_web (po);

  // Check that we have enough information to form the URI.
  if (module && branch && vcs_web)
    {
      UriData *ud;
      GtkWidget *image;

      image = gtk_image_new_from_icon_name ("external-link-symbolic");
      adw_action_row_add_suffix (ADW_ACTION_ROW (row), image);
      gtk_list_box_row_set_activatable (GTK_LIST_BOX_ROW (row), true);

      ud = g_new0 (UriData, 1);
      g_set_str (&ud->filename, filename);
      g_set_str (&ud->branch_name, branch);
      g_set_str (&ud->module_name, module);
      g_set_str (&ud->vcs_web, vcs_web);
      ud->line = line;

      g_signal_connect_data (row, "activated", G_CALLBACK (on_row_activated),
                             ud, (GClosureNotify)uri_data_free, G_CONNECT_DEFAULT);
    }

  gtk_list_box_append (GTK_LIST_BOX (priv->paths), row);
}
