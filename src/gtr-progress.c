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
#include <adwaita.h>

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
  PROP_0,
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
on_draw (GtkDrawingArea  *widget,
         cairo_t         *cr,
         int              dwidth,
         int              dheight,
         gpointer         data)
{
  GtrProgressPrivate *priv = gtr_progress_get_instance_private (GTR_PROGRESS (widget));

  GdkRGBA background;
  GdkRGBA foreground;
  GdkRGBA fuzzy_color;
  float translated = 0.0, fuzzy = 0.0;
  int total = priv->trans + priv->untrans + priv->fuzzy;
  int width, height;

  gboolean dark = adw_style_manager_get_dark (
    adw_style_manager_get_default ()
  );

  // Light theme
  // Blue 3 (53, 132, 228) #3584e4
  gdk_rgba_parse (&foreground, "#3584e4");
  // Yellow 4 (245, 194, 17) #f5c211
  gdk_rgba_parse (&fuzzy_color, "#f5c211");
  // Red 4 (192, 28, 40) #c01c28
  gdk_rgba_parse (&background, "#c01c28");

  if (dark)
    {
      // Dark theme
      // Blue 1 (153, 193, 241) #99c1f1
      gdk_rgba_parse (&foreground, "#99c1f1");
      // Yellow 1 (249, 240, 107) #f9f06b
      gdk_rgba_parse (&fuzzy_color, "#f9f06b");
      // Red 1 (246, 97, 81) #f66151
      gdk_rgba_parse (&background, "#f66151");
    }

  translated = (float) (priv->trans) / (float) total;
  fuzzy = (float) (priv->fuzzy) / (float) total;

  width = gtk_widget_get_width (GTK_WIDGET (widget));
  height = gtk_widget_get_height (GTK_WIDGET (widget));

  gdk_cairo_set_source_rgba (cr, &background);
  cairo_rectangle (cr, 0, 0, width * total, height);
  cairo_fill (cr);

  gdk_cairo_set_source_rgba (cr, &foreground);
  cairo_rectangle (cr, 0, 0, width * translated, height);
  cairo_fill (cr);

  gdk_cairo_set_source_rgba (cr, &fuzzy_color);
  cairo_rectangle (cr, width * translated, 0, width * fuzzy, height);
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

  gtk_widget_set_size_request (GTK_WIDGET (self), -1, 8);
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (self), on_draw, NULL, NULL);
}

GtrProgress*
gtr_progress_new (void) {
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

  gtk_widget_queue_draw (GTK_WIDGET (self));
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

