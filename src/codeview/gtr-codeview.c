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
#include "gtr-window.h"

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

static void
showed_message_cb (GtrTab *tab, GtrMsg *msg, GtrCodeView *codeview)
{
  const gchar *filename = NULL;
  gint i = 0;
  gint *line = NULL;
  GtrContextPanel *panel;
  panel = gtr_tab_get_context_panel (tab);

  filename = gtr_msg_get_filename (msg, i);
  while (filename)
    {
      line = gtr_msg_get_file_line (msg, i);
      gtr_context_add_path (panel, filename, GPOINTER_TO_INT (line));
      filename = gtr_msg_get_filename (msg, ++i);
    }
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
page_added_cb (GtkWidget   *tab,
               GtrCodeView *codeview)
{
  GtrContextPanel *panel;
  panel = gtr_tab_get_context_panel (GTR_TAB (tab));

  g_signal_connect_after (tab, "showed-message",
                          G_CALLBACK (showed_message_cb), codeview);

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
      priv->window = GTR_WINDOW (g_value_get_object (value));
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
  GtkWidget *tab;
  GtrPo *po;
  GList *msg;

  tab = GTK_WIDGET (gtr_window_get_active_tab (priv->window));

  po = gtr_tab_get_po (GTR_TAB (tab));
  msg = gtr_po_get_current_message (po);
  page_added_cb (tab, self);
  showed_message_cb (GTR_TAB (tab), msg->data, self);

  return self;
}
