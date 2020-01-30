/*
 * Copyright (C) 2018  Daniel Garcia Moreno <danigm@gnome.org>
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
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-codeview.h"
#include "gtr-context.h"
#include "gtr-dirs.h"
#include "gtr-utils.h"
#include "gtr-viewer.h"
#include "gtr-window.h"
#include "gtr-window-activatable.h"

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <gio/gio.h>
#include <ctype.h>

typedef struct
{
  GtrWindow *window;
  GSList *tags;
} GtrCodeViewPrivate;

struct _GtrCodeView
{
  GObject parent_instance;
};

enum
{
  PROP_0,
  PROP_WINDOW
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrCodeView, gtr_code_view, G_TYPE_OBJECT)

static char *
find_source_file (GtrCodeView *codeview,
                  const char  *path)
{
  GtrTab *tab;
  GtrPo *po;
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (codeview);

  g_autofree char *fullpath = NULL;
  g_autofree char *dirname = NULL;
  g_autoptr(GFile) location = NULL;
  g_autoptr(GFile) podir = NULL;
  g_autoptr(GFile) parent = NULL;

  if (g_file_test (path, G_FILE_TEST_EXISTS))
    return g_strdup (path);

  tab = gtr_window_get_active_tab (priv->window);

  if (!tab)
    return NULL;
  po = gtr_tab_get_po (tab);

  // .po files should live in PROJECT/po/LANG.po and the path inside the po
  // usually is relative to the PROJECT root, so we get the file path and
  // go one directory up to prepend to the file path
  location = gtr_po_get_location (po);
  podir = g_file_get_parent (location);
  parent = g_file_get_parent (podir);
  dirname = g_file_get_path (parent);
  fullpath = g_build_filename (dirname, path, NULL);

  if (g_file_test (fullpath, G_FILE_TEST_EXISTS))
    return g_strdup (fullpath);

  return NULL;
}

static void
insert_link (GtkTextBuffer *buffer,
             GtkTextIter   *iter,
             const gchar   *path,
             gint          *line,
             GtrCodeView   *codeview,
             const gchar   *msgid)
{
  GtkTextTag *tag;
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (codeview);
  g_autofree char *text = NULL;
  g_autofree char *fullpath = NULL;

  fullpath = find_source_file (codeview, path);

  text = g_strdup_printf ("%s:%d\n", path, GPOINTER_TO_INT (line));

  if (!fullpath)
    {
      gtk_text_buffer_insert (buffer, iter, text, -1);
      return;
    }

  tag = gtk_text_buffer_create_tag (buffer, NULL,
                                    "foreground", "blue",
                                    "underline", PANGO_UNDERLINE_SINGLE,
                                    NULL);

  g_object_set_data (G_OBJECT (tag), "line", line);
  g_object_set_data_full (G_OBJECT (tag), "path", g_strdup (path), g_free);
  g_object_set_data_full (G_OBJECT (tag), "msgid", g_strdup (msgid), g_free);

  priv->tags = g_slist_prepend (priv->tags, tag);

  gtk_text_buffer_insert_with_tags (buffer, iter, text, -1, tag, NULL);
}

static void
show_source (GtrCodeView *codeview, const gchar *path, gint line)
{
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (codeview);
  gtr_show_viewer (priv->window, path, line);
}

static gboolean
path_is_fake (const gchar *path)
{
  return g_str_has_suffix (path, ".h")
    && !g_file_test (path, G_FILE_TEST_EXISTS);
}

static gchar *
real_path (const gchar *path)
{
  gchar *result = g_strdup (path);

  if (path_is_fake (path))
    result[strlen (result) - 2] = '\0';

  return result;
}

static int
get_line_for_text (const gchar *path, const gchar *msgid)
{
  gchar *content, *str_found, *i, *escaped;
  int result;

  content = NULL;
  result = 1;
  escaped = g_markup_escape_text (msgid, -1);

  if (!g_file_get_contents (path, &content, NULL, NULL))
    goto out;

  i = content;
  while ((str_found = g_strstr_len (i, -1, escaped)))
    {
      gchar c;

      i = str_found + strlen (escaped);
      c = *i;
      if (!isalpha (c) &&
          !isalpha (*(str_found - 1)) && !(c == ':') && !(c == '_'))
        break;
    }
  if (!str_found)
    goto out;

  for (i = content; i < str_found; i++)
    if (*i == '\n')
      result++;

out:
  g_free (content);
  g_free (escaped);

  return result;
}

static void
follow_if_link (GtrCodeView *codeview, GtkWidget *text_view, GtkTextIter *iter)
{
  GSList *tags = NULL, *tagp = NULL;

  tags = gtk_text_iter_get_tags (iter);
  for (tagp = tags; tagp != NULL; tagp = tagp->next)
    {
      GtkTextTag *tag = tagp->data;
      char *path = g_object_get_data (G_OBJECT (tag), "path");
      int line = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (tag), "line"));
      g_autofree char *fullpath = NULL;

      fullpath = find_source_file (codeview, path);

      if (fullpath && path_is_fake (fullpath))
        {
          char *msgid = g_object_get_data (G_OBJECT (tag), "msgid");
          g_autofree char *realpath = NULL;
          realpath = real_path (fullpath);
          line = get_line_for_text (fullpath, msgid);
        }

      show_source (codeview, fullpath, line);
    }

  if (tags)
    g_slist_free (tags);
}

static gboolean
event_after (GtkWidget *text_view, GdkEvent *ev, GtrCodeView *codeview)
{
  GtkTextIter start, end, iter;
  GtkTextBuffer *buffer;
  GdkEventButton *event;
  gint x, y;

  if (ev->type != GDK_BUTTON_RELEASE)
    return FALSE;

  event = (GdkEventButton *) ev;

  if (event->button != 1)
    return FALSE;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

  /* we shouldn't follow a link if the user has selected something */
  gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
  if (gtk_text_iter_get_offset (&start) != gtk_text_iter_get_offset (&end))
    return FALSE;

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         event->x, event->y, &x, &y);

  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (text_view), &iter, x, y);

  follow_if_link (codeview, text_view, &iter);

  return FALSE;
}

static gboolean hovering_over_link = FALSE;
static GdkCursor *hand_cursor = NULL;
static GdkCursor *regular_cursor = NULL;

/* Looks at all tags covering the position (x, y) in the text view,
 * and if one of them is a link, change the cursor to the "hands" cursor
 * typically used by web browsers.
 */
static void
set_cursor_if_appropriate (GtkTextView *text_view, gint x, gint y)
{
  GSList *tags = NULL, *tagp = NULL;
  GtkTextIter iter;
  gboolean hovering = FALSE;

  gtk_text_view_get_iter_at_location (text_view, &iter, x, y);

  tags = gtk_text_iter_get_tags (&iter);
  for (tagp = tags; tagp != NULL; tagp = tagp->next)
    {
      GtkTextTag *tag = tagp->data;
      gchar *path = g_object_get_data (G_OBJECT (tag), "path");

      if (path)
        {
          hovering = TRUE;
          break;
        }
    }

  if (hovering != hovering_over_link)
    {
      hovering_over_link = hovering;

      if (hovering_over_link)
        gdk_window_set_cursor (gtk_text_view_get_window (text_view,
                                                         GTK_TEXT_WINDOW_TEXT),
                               hand_cursor);
      else
        gdk_window_set_cursor (gtk_text_view_get_window (text_view,
                                                         GTK_TEXT_WINDOW_TEXT),
                               regular_cursor);
    }

  if (tags)
    g_slist_free (tags);
}

/*
 * Update the cursor image if the pointer moved.
 */
static gboolean
motion_notify_event (GtkWidget *text_view, GdkEventMotion *event)
{
  gint x, y;

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         event->x, event->y, &x, &y);

  set_cursor_if_appropriate (GTK_TEXT_VIEW (text_view), x, y);

  return FALSE;
}

/* Also update the cursor image if the window becomes visible
 * (e.g. when a window covering it got iconified).
 */
static gboolean
visibility_notify_event (GtkWidget *text_view, GdkEventVisibility *event)
{
  GdkDevice *pointer;
  GdkSeat *seat;
  gint wx, wy, bx, by;

  seat = gdk_display_get_default_seat (gtk_widget_get_display (text_view));
  pointer = gdk_seat_get_pointer (seat);
  gdk_window_get_device_position (gtk_widget_get_window (text_view), pointer,
                                  &wx, &wy, NULL);

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         wx, wy, &bx, &by);

  set_cursor_if_appropriate (GTK_TEXT_VIEW (text_view), bx, by);

  return FALSE;
}

static void
showed_message_cb (GtrTab *tab, GtrMsg *msg, GtrCodeView *codeview)
{
  const gchar *filename = NULL;
  gint i = 0;
  gint *line = NULL;
  GtkTextIter iter;
  GtkTextBuffer *buffer;
  GtkTextView *view;
  GtrContextPanel *panel;
  GtkTextTag *bold;
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (codeview);

  panel = gtr_tab_get_context_panel (tab);
  view = gtr_context_panel_get_context_text_view (panel);

  buffer = gtk_text_view_get_buffer (view);

  gtk_text_buffer_get_end_iter (buffer, &iter);

  /* Create the bold tag for header */
  bold =
    gtk_text_buffer_create_tag (buffer, NULL, "weight", PANGO_WEIGHT_BOLD,
                                "weight-set", TRUE, NULL);

  gtk_text_buffer_insert (buffer, &iter, "\n", 1);
  gtk_text_buffer_insert_with_tags (buffer, &iter, _("Paths:"), -1, bold,
                                    NULL);
  gtk_text_buffer_insert (buffer, &iter, "\n", 1);

  filename = gtr_msg_get_filename (msg, i);
  while (filename)
    {
      line = gtr_msg_get_file_line (msg, i);
      insert_link (buffer, &iter, filename, line, codeview,
                   gtr_msg_get_msgid (msg));
      i++;
      filename = gtr_msg_get_filename (msg, i);
    }

  /*
   * The tags are managed by buffer, so lets add a reference in the buffer
   */
  g_object_set_data (G_OBJECT (buffer), "link_tags", priv->tags);
  priv->tags = NULL;
}

static void
delete_text_and_tags (GtrTab *tab, GtrCodeView *codeview)
{
  GSList *tagp = NULL, *tags;
  GtkTextBuffer *buffer;
  GtrContextPanel *panel;
  GtkTextView *view;
  GtkTextIter start, end;
  GtkTextMark *path_start, *path_end;

  panel = gtr_tab_get_context_panel (tab);
  view = gtr_context_panel_get_context_text_view (panel);

  buffer = gtk_text_view_get_buffer (view);
  path_start = gtk_text_buffer_get_mark (buffer, "path_start");

  if (path_start == NULL)
    return;

  path_end = gtk_text_buffer_get_mark (buffer, "path_end");
  tags = g_object_get_data (G_OBJECT (buffer), "link_tags");

  for (tagp = tags; tagp != NULL; tagp = tagp->next)
    {
      GtkTextTag *tag = tagp->data;

      g_object_set_data (G_OBJECT (tag), "path", NULL);
      g_object_set_data (G_OBJECT (tag), "msgid", NULL);
    }
  g_slist_free (tags);

  /*
   * Deleting the text
   */
  gtk_text_buffer_get_iter_at_mark (buffer, &start, path_start);
  gtk_text_buffer_get_iter_at_mark (buffer, &end, path_end);
  gtk_text_buffer_delete (buffer, &start, &end);

  /*
   * Deleting the marks
   */
  gtk_text_buffer_delete_mark (buffer, path_start);
  gtk_text_buffer_delete_mark (buffer, path_end);
}

static void
message_edition_finished_cb (GtrTab      *tab,
                             GtrMsg      *msg,
                             GtrCodeView *codeview)
{
  delete_text_and_tags (tab, codeview);
}

static void
on_context_panel_reloaded (GtrContextPanel *panel,
                           GtrMsg          *msg,
                           GtrCodeView     *codeview)
{
  GtrTab *tab;
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (codeview);

  tab = gtr_window_get_active_tab (priv->window);

  showed_message_cb (tab, msg, codeview);
}

static void
page_added_cb (GtkNotebook *notebook,
               GtkWidget   *child,
               guint        page_num,
               GtrCodeView *codeview)
{
  GtrContextPanel *panel;
  GtkTextView *view;

  panel = gtr_tab_get_context_panel (GTR_TAB (child));
  view = gtr_context_panel_get_context_text_view (panel);

  g_return_if_fail (GTK_IS_TEXT_VIEW (view));

  g_signal_connect_after (child, "showed-message",
                          G_CALLBACK (showed_message_cb), codeview);
  g_signal_connect (child, "message-edition-finished",
                    G_CALLBACK (message_edition_finished_cb), codeview);

  g_signal_connect (view, "event-after", G_CALLBACK (event_after), codeview);
  g_signal_connect (view, "motion-notify-event",
                    G_CALLBACK (motion_notify_event), NULL);
  g_signal_connect (view, "visibility-notify-event",
                    G_CALLBACK (visibility_notify_event), NULL);
  g_signal_connect (panel, "reloaded",
                    G_CALLBACK (on_context_panel_reloaded), codeview);
}

static void
gtr_code_view_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GtrCodeView *code_view = GTR_CODE_VIEW (object);
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (code_view);

  switch (prop_id)
    {
    case PROP_WINDOW:
      priv->window = GTR_WINDOW (g_value_dup_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_code_view_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GtrCodeView *code_view = GTR_CODE_VIEW (object);
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (code_view);

  switch (prop_id)
    {
    case PROP_WINDOW:
      g_value_set_object (value, priv->window);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_code_view_dispose (GObject *object)
{
  G_OBJECT_CLASS (gtr_code_view_parent_class)->dispose (object);
}

static void
gtr_code_view_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtr_code_view_parent_class)->finalize (object);
}

static void
gtr_code_view_class_init (GtrCodeViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *param;

  object_class->finalize = gtr_code_view_finalize;
  object_class->dispose = gtr_code_view_dispose;
  object_class->get_property = gtr_code_view_get_property;
  object_class->set_property = gtr_code_view_set_property;

  param = g_param_spec_object ("window", "WINDOW", "The window",
                               GTR_TYPE_WINDOW,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT);

  g_object_class_install_property (object_class, PROP_WINDOW, param);
}

static void
gtr_code_view_init (GtrCodeView *self)
{
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (self);
  priv->window = NULL;
}

GtrCodeView *
gtr_code_view_new (GtrWindow *window)
{
  GtrCodeView *self = g_object_new (GTR_TYPE_CODE_VIEW,
                                    "window", window, NULL);
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (self);
  GtkWidget *notebook;
  GdkDisplay *display;
  GList *tabs, *l;

  notebook = GTK_WIDGET (gtr_window_get_notebook (priv->window));
  display = gtk_widget_get_display (notebook);

  /*
   * Cursors
   */
  hand_cursor = gdk_cursor_new_for_display (display, GDK_HAND2);
  regular_cursor = gdk_cursor_new_for_display (display, GDK_XTERM);

  g_signal_connect (notebook, "page-added", G_CALLBACK (page_added_cb), self);

  /*
   * If we already have tabs opened we have to add them
   */
  tabs = gtr_window_get_all_tabs (priv->window);
  for (l = tabs; l != NULL; l = g_list_next (l))
    {
      GtrPo *po;
      GList *msg;

      page_added_cb (GTK_NOTEBOOK (notebook), l->data, 0, self);

      po = gtr_tab_get_po (GTR_TAB (l->data));
      msg = gtr_po_get_current_message (po);

      showed_message_cb (GTR_TAB (l->data), msg->data, self);
    }

  return self;
}
