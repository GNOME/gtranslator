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

#include "gtr-progress.h"

#include <glib/gi18n.h>

typedef struct
{
  gint trans;
  gint untrans;
  gint fuzzy;
} GtrProgressPrivate;

struct _GtrProgress
{
  GtkDrawingArea parent_instance;
};

enum
{
  PROP_TRANS,
  PROP_UNTRANS,
  PROP_FUZZY,
  N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE_WITH_PRIVATE (GtrProgress, gtr_progress, GTK_TYPE_DRAWING_AREA)

static void
gtr_progress_dispose (GObject *object)
{
  G_OBJECT_CLASS (gtr_progress_parent_class)->dispose (object);
}

static void
gtr_progress_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtr_progress_parent_class)->finalize (object);
}

static void
gtr_progress_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GtrProgressPrivate *priv = gtr_progress_get_instance_private (GTR_PROGRESS (object));

  switch (prop_id) {
  case PROP_TRANS:
      priv->trans = g_value_get_int (value);
      break;
  case PROP_UNTRANS:
      priv->untrans = g_value_get_int (value);
      break;
  case PROP_FUZZY:
      priv->fuzzy = g_value_get_int (value);
      break;
  default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtr_progress_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  GtrProgressPrivate *priv = gtr_progress_get_instance_private (GTR_PROGRESS (object));

  switch (prop_id) {
  case PROP_TRANS:
      g_value_set_int (value, priv->trans);
      break;
  case PROP_UNTRANS:
      g_value_set_int (value, priv->untrans);
      break;
  case PROP_FUZZY:
      g_value_set_int (value, priv->fuzzy);
      break;
  default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
on_draw (GtkWidget       *widget,
         cairo_t         *cr,
         gpointer        data)
{
  GtrProgressPrivate *priv = gtr_progress_get_instance_private (GTR_PROGRESS (widget));

  gdouble ratio, ratio_fuzzy;
  guint width;
  guint height;
  GdkRGBA background;
  GdkRGBA foreground;
  GdkRGBA fuzzy_color;
  GtkStyleContext *style_context;
  gfloat percentage = 0.0;
  gint total = priv->trans + priv->untrans + priv->fuzzy;

  style_context = gtk_widget_get_style_context (widget);
  gtk_style_context_lookup_color (style_context, "theme_selected_bg_color", &foreground);
  gtk_style_context_lookup_color (style_context, "warning_color", &fuzzy_color);
  gtk_style_context_get_color (style_context, gtk_widget_get_state_flags (widget), &background);
  background.alpha *= 0.3;

  percentage = (float) (priv->trans * 100) / (float) total;
  ratio = percentage / 100.0;

  percentage = (float) (priv->fuzzy * 100) / (float) total;
  ratio_fuzzy = percentage / 100.0;

  width = gtk_widget_get_allocated_width (widget);
  height = gtk_widget_get_allocated_height (widget);

  gdk_cairo_set_source_rgba (cr, &background);
  cairo_arc (cr,
             width / 2.0, height / 2.0,
             MIN (width, height) / 2.0,
             0, 2 * G_PI);
  cairo_fill (cr);

  cairo_move_to (cr, width / 2.0, height / 2.0);
  gdk_cairo_set_source_rgba (cr, &foreground);
  cairo_arc (cr,
             width / 2.0, height / 2.0,
             MIN (width, height) / 2.0,
             -G_PI / 2.0, ratio * 2 * G_PI - G_PI / 2.0);
  cairo_fill (cr);

  cairo_move_to (cr, width / 2.0, height / 2.0);
  gdk_cairo_set_source_rgba (cr, &fuzzy_color);
  cairo_arc (cr,
             width / 2.0, height / 2.0,
             MIN (width, height) / 2.0,
             ratio * 2 * G_PI - G_PI / 2.0,
             (ratio + ratio_fuzzy) * 2 * G_PI - G_PI / 2.0);

  cairo_fill (cr);
}

static void
gtr_progress_class_init (GtrProgressClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtr_progress_finalize;
  object_class->dispose = gtr_progress_dispose;
  object_class->set_property = gtr_progress_set_property;
  object_class->get_property = gtr_progress_get_property;

  properties[PROP_TRANS] =
      g_param_spec_int ("translated",
                        "Translated",
                        "Translated messages",
                        0,
                        INT_MAX,
                        0,
                        G_PARAM_READWRITE);
  properties[PROP_UNTRANS] =
      g_param_spec_int ("untranslated",
                        "Untranslated",
                        "Untranslated messages",
                        0,
                        INT_MAX,
                        0,
                        G_PARAM_READWRITE);
  properties[PROP_FUZZY] =
      g_param_spec_int ("fuzzy",
                        "Fuzzy",
                        "Fuzzy messages",
                        0,
                        INT_MAX,
                        0,
                        G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gtr_progress_init (GtrProgress *self)
{
  GtrProgressPrivate *priv = gtr_progress_get_instance_private (self);
  priv->trans = 0;
  priv->untrans = 0;
  priv->fuzzy = 0;

  gtk_widget_set_size_request (GTK_WIDGET (self), 16, 16);
  g_signal_connect (G_OBJECT (self), "draw",
                    G_CALLBACK (on_draw), NULL);
}

GtrProgress*
gtr_progress_new () {
  GtrProgress *self = g_object_new (GTR_TYPE_PROGRESS, NULL);
  return self;
}

void
gtr_progress_set (GtrProgress *self,
                  gint         trans,
                  gint         untrans,
                  gint         fuzzy)
{
  GtrProgressPrivate *priv = gtr_progress_get_instance_private (self);
  if (trans != priv->trans)
    {
      priv->trans = trans;
      g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_TRANS]);
    }
  if (untrans != priv->untrans)
    {
      priv->untrans = untrans;
      g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_UNTRANS]);
    }
  if (fuzzy != priv->fuzzy)
    {
      priv->fuzzy = fuzzy;
      g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_FUZZY]);
    }

  gtk_widget_queue_draw (self);
}

gint
gtr_progress_get_trans (GtrProgress *self)
{
  GtrProgressPrivate *priv = gtr_progress_get_instance_private (self);
  return priv->trans;
}

gint
gtr_progress_get_untrans (GtrProgress *self)
{
  GtrProgressPrivate *priv = gtr_progress_get_instance_private (self);
  return priv->untrans;
}

gint
gtr_progress_get_fuzzy (GtrProgress *self)
{
  GtrProgressPrivate *priv = gtr_progress_get_instance_private (self);
  return priv->fuzzy;
}

