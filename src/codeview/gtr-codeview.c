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

#include <ctype.h>
#include <string.h>

#include <gio/gio.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include "gtr-context.h"
#include "gtr-dirs.h"
#include "gtr-utils.h"
#include "gtr-window.h"

typedef struct
{
  GtrWindow *window;
  GSList *tags;
  GtrTab *tab;
  GtrContextPanel *panel;
} GtrCodeViewPrivate;

struct _GtrCodeView
{
  GObject parent_instance;
};

enum
{
  PROP_0,
  PROP_WINDOW,
  N_PROPS
};
static GParamSpec *props[N_PROPS];

G_DEFINE_FINAL_TYPE_WITH_PRIVATE (GtrCodeView, gtr_code_view, G_TYPE_OBJECT);

static void
update_filename (GtrCodeView *self, GtrMsg *msg)
{
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (self);

  for (int i = 0; ; i++)
  {
    int *line = NULL;
    const gchar *filename = NULL;

    filename = gtr_msg_get_filename (msg, i);

    if (!filename)
      break;

    line = gtr_msg_get_file_line (msg, i);
    gtr_context_add_path (priv->panel, filename, GPOINTER_TO_INT (line));
  }
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
  GtrCodeView *self = GTR_CODE_VIEW (object);
  GtrCodeViewPrivate *priv = gtr_code_view_get_instance_private (self);

  if (priv->tab)
    g_signal_handlers_disconnect_by_func (priv->tab, update_filename, self);

  if (priv->panel)
    g_signal_handlers_disconnect_by_func (priv->panel, update_filename, self);

  G_OBJECT_CLASS (gtr_code_view_parent_class)->finalize (object);
}

static void
gtr_code_view_class_init (GtrCodeViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtr_code_view_finalize;
  object_class->dispose = gtr_code_view_dispose;
  object_class->get_property = gtr_code_view_get_property;
  object_class->set_property = gtr_code_view_set_property;

  props[PROP_WINDOW] =
    g_param_spec_object ("window", NULL, NULL,
                         GTR_TYPE_WINDOW,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPS, props);
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
  GtrPo *po;
  GList *msg;

  priv->tab = gtr_window_get_active_tab (priv->window);

  po = gtr_tab_get_po (priv->tab);
  msg = gtr_po_get_current_message (po);
  priv->panel = gtr_tab_get_context_panel (priv->tab);

  g_signal_connect_swapped (priv->tab, "showed-message",
                            G_CALLBACK (update_filename), self);

  g_signal_connect_swapped (priv->panel, "reloaded",
                            G_CALLBACK (update_filename), self);

  update_filename (self, msg->data);

  return self;
}
