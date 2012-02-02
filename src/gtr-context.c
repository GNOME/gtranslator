/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <icq@gnome.org>
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
#include "gtr-debug.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define GTR_CONTEXT_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_CONTEXT_PANEL,     \
						 GtrContextPanelPrivate))

G_DEFINE_TYPE (GtrContextPanel, gtr_context_panel, GTK_TYPE_BOX)

struct _GtrContextPanelPrivate
{
  GtkWidget *sw;
  GtkWidget *context;
  GtkWidget *button_box;

  GdkCursor *hand_cursor;
  GdkCursor *regular_cursor;
  gboolean hovering_over_link;

  GtrTab *tab;
  GtrMsg *current_msg;
};

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

static void
setup_notes_edition (GtrContextPanel *panel)
{
  GtkTextBuffer *buffer;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (panel->priv->context));
  gtk_text_buffer_set_text (buffer, gtr_msg_get_comment (panel->priv->current_msg), -1);

  gtk_text_view_set_editable (GTK_TEXT_VIEW (panel->priv->context), TRUE);
  gtk_widget_show (panel->priv->button_box);
}

static void
follow_if_link (GtrContextPanel *panel, GtkWidget *text_view, GtkTextIter *iter)
{
  GSList *tags = NULL, *tagp = NULL;

  tags = gtk_text_iter_get_tags (iter);
  for (tagp = tags; tagp != NULL; tagp = tagp->next)
    {
      GtkTextTag *tag = tagp->data;
      gint *is_path = g_object_get_data (G_OBJECT (tag), "is_path");

      if (is_path)
        {
          setup_notes_edition (panel);
          break;
        }
    }

  if (tags)
    g_slist_free (tags);
}

static gboolean
event_after (GtkWidget *text_view,
             GdkEvent *ev, GtrContextPanel *panel)
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

  follow_if_link (panel, text_view, &iter);

  return FALSE;
}

/* Looks at all tags covering the position (x, y) in the text view, 
 * and if one of them is a link, change the cursor to the "hands" cursor
 * typically used by web browsers.
 */
static void
set_cursor_if_appropriate (GtkTextView * text_view, gint x, gint y,
                           GtrContextPanel *panel)
{
  GSList *tags = NULL, *tagp = NULL;
  GtkTextIter iter;
  gboolean hovering = FALSE;

  gtk_text_view_get_iter_at_location (text_view, &iter, x, y);

  tags = gtk_text_iter_get_tags (&iter);
  for (tagp = tags; tagp != NULL; tagp = tagp->next)
    {
      GtkTextTag *tag = tagp->data;
      gint *is_path = g_object_get_data (G_OBJECT (tag), "is_path");

      if (is_path)
        {
          hovering = TRUE;
          break;
        }
    }

  if (hovering != panel->priv->hovering_over_link)
    {
      panel->priv->hovering_over_link = hovering;

      if (panel->priv->hovering_over_link)
        gdk_window_set_cursor (gtk_text_view_get_window (text_view,
                                                         GTK_TEXT_WINDOW_TEXT),
                               panel->priv->hand_cursor);
      else
        gdk_window_set_cursor (gtk_text_view_get_window (text_view,
                                                         GTK_TEXT_WINDOW_TEXT),
                               panel->priv->regular_cursor);
    }

  if (tags)
    g_slist_free (tags);
}

/* Update the cursor image if the pointer moved. */
static gboolean
motion_notify_event (GtkWidget *text_view, GdkEventMotion *event,
                     GtrContextPanel *panel)
{
  gint x, y;

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         event->x, event->y, &x, &y);

  set_cursor_if_appropriate (GTK_TEXT_VIEW (text_view), x, y, panel);

  return FALSE;
}

/* Also update the cursor image if the window becomes visible
 * (e.g. when a window covering it got iconified). */
static gboolean
visibility_notify_event (GtkWidget *text_view, GdkEventVisibility *event,
                         GtrContextPanel *panel)
{
  GdkDeviceManager *device_manager;
  GdkDevice *pointer;
  gint wx, wy, bx, by;

  device_manager = gdk_display_get_device_manager (gtk_widget_get_display (text_view));
  pointer = gdk_device_manager_get_client_pointer (device_manager);
  gdk_window_get_device_position (gtk_widget_get_window (text_view), pointer, &wx, &wy, NULL);

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         wx, wy, &bx, &by);

  set_cursor_if_appropriate (GTK_TEXT_VIEW (text_view), bx, by, panel);

  return FALSE;
}

static void
add_notes (GtkTextBuffer *buffer, GtkTextIter *pos, GtkTextTag *header_tag,
           GtrMsg *msg)
{
  GtkTextTag *tag;
  const gchar *comments;

  tag = gtk_text_buffer_create_tag (buffer, NULL,
                                    "foreground", "blue",
                                    "underline", PANGO_UNDERLINE_SINGLE,
                                    NULL);

  gtk_text_buffer_insert_with_tags (buffer, pos, _("Notes:"), -1, header_tag, NULL);
  gtk_text_buffer_insert (buffer, pos, "\n", 1);

  g_object_set_data (G_OBJECT (tag), "is_path", GINT_TO_POINTER (1));

  comments = gtr_msg_get_comment (msg);

  if (comments == NULL || *comments == '\0')
    gtk_text_buffer_insert_with_tags (buffer, pos, _("Add Note"), -1, tag, NULL);
  else
    {
      gtk_text_buffer_insert (buffer, pos, comments, -1);
      gtk_text_buffer_insert (buffer, pos, "(", 1);
      gtk_text_buffer_insert_with_tags (buffer, pos, _("Edit"), -1, tag, NULL);
      gtk_text_buffer_insert (buffer, pos, ")", 1);
    }

  gtk_text_buffer_insert (buffer, pos, "\n", 1);
}

static void
add_text (GtkTextBuffer *buffer, GtkTextTag *tag, GtkTextIter *pos,
          const gchar *header, const gchar *text)
{
  if (text && *text != '\0')
    {
      gtk_text_buffer_insert (buffer, pos, "\n", 1);
      gtk_text_buffer_insert_with_tags (buffer, pos, header, -1, tag, NULL);
      gtk_text_buffer_insert (buffer, pos, "\n", 1);
      gtk_text_buffer_insert (buffer, pos, text, -1);
      gtk_text_buffer_insert (buffer, pos, "\n", 1);
    }
}

static void
showed_message_cb (GtrTab *tab, GtrMsg *msg, GtrContextPanel *panel)
{
  GtkTextBuffer *buffer;
  GtkTextIter iter;
  GtkTextTag *bold;

  /* Update current msg */
  panel->priv->current_msg = msg;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (panel->priv->context));
  gtk_text_buffer_set_text (buffer, "", 0);
  gtk_text_buffer_get_start_iter (buffer, &iter);

  /* Create the bold tag for headers */
  bold = gtk_text_buffer_create_tag (buffer, NULL, "weight", PANGO_WEIGHT_BOLD,
                                     "weight-set", TRUE, NULL);

  /* Add custom comments */
  add_notes (buffer, &iter, bold, msg);

  /* Extracted comments */
  add_text (buffer, bold, &iter, _("Extracted comments:"),
            gtr_msg_get_extracted_comments (msg));

  /* Context */
  add_text (buffer, bold, &iter, _("Context:"),
            gtr_msg_get_msgctxt (msg));

  /* Format */
  add_text (buffer, bold, &iter, _("Format:"),
            gtr_msg_get_format (msg));
}

static void
reload_values (GtrContextPanel *panel)
{
  showed_message_cb (panel->priv->tab, panel->priv->current_msg, panel);
  gtk_widget_hide (panel->priv->button_box);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (panel->priv->context), FALSE);

  g_signal_emit (G_OBJECT (panel), signals[RELOADED], 0, panel->priv->current_msg);
}

static void
on_accept_button_clicked (GtkButton *button, GtrContextPanel *panel)
{
  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  gchar *text;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (panel->priv->context));

  gtk_text_buffer_get_bounds (buffer, &start, &end);

  text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

  gtr_msg_set_comment (panel->priv->current_msg, text);

  g_free (text);

  reload_values (panel);
}

static void
on_cancel_button_clicked (GtkButton *button, GtrContextPanel *panel)
{
  reload_values (panel);
}

static void
gtr_context_panel_init (GtrContextPanel *panel)
{
  GtrContextPanelPrivate *priv;
  GtkWidget *button;

  panel->priv = GTR_CONTEXT_PANEL_GET_PRIVATE (panel);
  priv = panel->priv;

  priv->hovering_over_link = FALSE;

  priv->hand_cursor = gdk_cursor_new (GDK_HAND2);
  priv->regular_cursor = gdk_cursor_new (GDK_XTERM);

  gtk_orientable_set_orientation (GTK_ORIENTABLE (panel),
                                  GTK_ORIENTATION_VERTICAL);

  /* Set up the scrolling window for the extracted context display */
  priv->sw = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (priv->sw),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (priv->sw),
                                       GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (panel), priv->sw, TRUE, TRUE,
                      0);
  gtk_widget_show (priv->sw);

  /* Context text view */
  priv->context = gtk_text_view_new ();

  gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->context), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (priv->context), GTK_WRAP_WORD);
  gtk_container_add (GTK_CONTAINER (priv->sw),
                     GTK_WIDGET (priv->context));
  gtk_widget_show (priv->context);

  g_signal_connect (priv->context, "event-after",
                    G_CALLBACK (event_after), panel);
  g_signal_connect (priv->context, "motion-notify-event",
                    G_CALLBACK (motion_notify_event), panel);
  g_signal_connect (priv->context, "visibility-notify-event",
                    G_CALLBACK (visibility_notify_event), panel);

  /* Buttons */
  priv->button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start (GTK_BOX (panel), priv->button_box, FALSE, FALSE, 0);

  button = gtk_button_new_from_stock (GTK_STOCK_OK);
  gtk_widget_show (button);
  g_signal_connect (button, "clicked",
                    G_CALLBACK (on_accept_button_clicked), panel);

  gtk_box_pack_start (GTK_BOX (priv->button_box), button, FALSE, FALSE, 0);

  button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_widget_show (button);
  g_signal_connect (button, "clicked",
                    G_CALLBACK (on_cancel_button_clicked), panel);

  gtk_box_pack_start (GTK_BOX (priv->button_box), button, FALSE, FALSE, 0);
}

static void
gtr_context_panel_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GtrContextPanel *panel = GTR_CONTEXT_PANEL (object);

  switch (prop_id)
    {
    case PROP_TAB:
      panel->priv->tab = GTR_TAB (g_value_get_object (value));
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

  switch (prop_id)
    {
    case PROP_TAB:
      g_value_set_object (value, panel->priv->tab);
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

  if (!panel->priv->tab)
    {
      g_critical ("The context was not constructed well, this shouldn't happen!");
      return;
    }

  g_signal_connect (panel->priv->tab,
                    "showed-message",
                    G_CALLBACK (showed_message_cb), panel);
}

static void
gtr_context_panel_dispose (GObject *object)
{
  GtrContextPanel *panel = GTR_CONTEXT_PANEL (object);

  DEBUG_PRINT ("Dispose context");

  g_clear_object (&panel->priv->hand_cursor);
  g_clear_object (&panel->priv->regular_cursor);

  G_OBJECT_CLASS (gtr_context_panel_parent_class)->dispose (object);
}

static void
gtr_context_panel_class_init (GtrContextPanelClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrContextPanelPrivate));

  object_class->dispose = gtr_context_panel_dispose;
  object_class->set_property = gtr_context_panel_set_property;
  object_class->get_property = gtr_context_panel_get_property;
  object_class->constructed = gtr_context_panel_constructed;

  signals[RELOADED] =
    g_signal_new ("reloaded",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtrContextPanelClass, reloaded),
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
}

/**
 * gtr_context_panel_new:
 * @tab: a #GtrTab
 * 
 * Creates a new #GtrContextPanel object.
 * 
 * Returns: a new #GtrContextPanel object
 */
GtkWidget *
gtr_context_panel_new (GtkWidget *tab)
{
  return g_object_new (GTR_TYPE_CONTEXT_PANEL, "tab", tab, NULL);
}

/**
 * gtr_context_panel_get_context_text_view:
 * @panel: a #GtrContextPanel
 *
 * Returns: (transfer none): the context #GtkTextView
 */
GtkTextView *
gtr_context_panel_get_context_text_view (GtrContextPanel * panel)
{
  g_return_val_if_fail (GTR_IS_CONTEXT_PANEL (panel), NULL);

  return GTK_TEXT_VIEW (panel->priv->context);
}
