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
#include "gtr-debug.h"
#include "gtr-utils.h"
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
  GtkWidget *context;
  GtkWidget *translation_memory_box;

  GdkCursor *hand_cursor;
  GdkCursor *regular_cursor;
  gboolean hovering_over_link;

  GtrTab *tab;
  GtrMsg *current_msg;

  // translation memory
  GtkWidget *translation_memory_ui;
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
  GtrContextPanelPrivate *priv;

  priv = gtr_context_panel_get_instance_private(panel);

  /* Update current msg */
  priv->current_msg = msg;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->context));
  gtk_text_buffer_set_text (buffer, "", 0);
  gtk_text_buffer_get_start_iter (buffer, &iter);

  /* Create the bold tag for headers */
  bold = gtk_text_buffer_create_tag (buffer, NULL, "weight", PANGO_WEIGHT_BOLD,
                                     "weight-set", TRUE, NULL);

  /* Add custom comments */
  add_notes (buffer, &iter, bold, msg);

  add_text (buffer, bold, &iter, _("Extracted comments:"),
            gtr_msg_get_extracted_comments (msg));

  add_text (buffer, bold, &iter, _("Context:"),
            gtr_msg_get_msgctxt (msg));

  add_text (buffer, bold, &iter, _("Format:"),
            gtr_msg_get_format (msg));
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
  GtrContextPanelPrivate *priv;
  GtkTextBuffer *buffer;

  priv = gtr_context_panel_get_instance_private(panel);

  gtk_widget_init_template (GTK_WIDGET (panel));

  priv->hovering_over_link = FALSE;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->context));
  g_signal_connect (buffer, "end-user-action",
                    G_CALLBACK (buffer_end_user_action), panel);
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
  GtrContextPanel *panel = GTR_CONTEXT_PANEL (object);
  GtrContextPanelPrivate *priv;

  priv = gtr_context_panel_get_instance_private(panel);

  g_clear_object (&priv->hand_cursor);
  g_clear_object (&priv->regular_cursor);

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
  gtk_widget_class_bind_template_child_private (widget_class, GtrContextPanel, context);
  gtk_widget_class_bind_template_child_private (widget_class, GtrContextPanel, translation_memory_box);
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

/**
 * gtr_context_panel_get_context_text_view:
 * @panel: a #GtrContextPanel
 *
 * Returns: (transfer none): the context #GtkTextView
 */
GtkTextView *
gtr_context_panel_get_context_text_view (GtrContextPanel * panel)
{
  GtrContextPanelPrivate *priv;

  g_return_val_if_fail (GTR_IS_CONTEXT_PANEL (panel), NULL);

  priv = gtr_context_panel_get_instance_private(panel);
  return GTK_TEXT_VIEW (priv->context);
}

void
gtr_context_init_tm (GtrContextPanel *panel,
                     GtrTranslationMemory *tm)
{
  GtrContextPanelPrivate *priv = gtr_context_panel_get_instance_private(panel);
  priv->translation_memory_ui = gtr_translation_memory_ui_new (GTK_WIDGET (priv->tab),
                                                               tm);
  gtk_widget_show (priv->translation_memory_ui);
  gtk_widget_set_size_request (priv->translation_memory_ui, 300, 400);

  GtkWidget* scrolled_win = gtk_scrolled_window_new ();
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW(scrolled_win), priv->translation_memory_ui);
  gtk_scrolled_window_set_min_content_width (GTK_SCROLLED_WINDOW (scrolled_win), 300);
  gtk_scrolled_window_set_min_content_height (GTK_SCROLLED_WINDOW (scrolled_win), 400);

  gtk_box_append (GTK_BOX (priv->translation_memory_box),
                  scrolled_win);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
}
