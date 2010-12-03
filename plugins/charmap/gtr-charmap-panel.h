/*
 * charmap-panel.h
 * This file is part of gtr
 *
 * Copyright (C) 2006 - Steve Frécinaux
 *
 * gtr is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * gtr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gtr; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __GTR_CHARMAP_PANEL_H__
#define __GTR_CHARMAP_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <gucharmap/gucharmap.h>

G_BEGIN_DECLS

#define GTR_TYPE_CHARMAP_PANEL          (gtr_charmap_panel_get_type ())
#define GTR_CHARMAP_PANEL(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_CHARMAP_PANEL, GtrCharmapPanel))
#define GTR_CHARMAP_PANEL_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_CHARMAP_PANEL, GtrCharmapPanelClass))
#define GTR_IS_CHARMAP_PANEL(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_CHARMAP_PANEL))
#define GTR_IS_CHARMAP_PANEL_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_CHARMAP_PANEL))
#define GTR_CHARMAP_PANEL_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_CHARMAP_PANEL, GtrCharmapPanelClass))

typedef struct _GtrCharmapPanel         GtrCharmapPanel;
typedef struct _GtrCharmapPanelClass    GtrCharmapPanelClass;
typedef struct _GtrCharmapPanelPrivate  GtrCharmapPanelPrivate;

struct _GtrCharmapPanel
{
  GtkVBox parent_instance;

  /*< private > */
  GtrCharmapPanelPrivate *priv;
};

struct _GtrCharmapPanelClass
{
  GtkVBoxClass parent_class;
};

GType           gtr_charmap_panel_get_type (void) G_GNUC_CONST;
GtkWidget      *gtr_charmap_panel_new      (void);

GucharmapChartable * gtr_charmap_panel_get_chartable (GtrCharmapPanel *panel);

void            _gtr_charmap_panel_register_type (GTypeModule *type_module);

G_END_DECLS

#endif /* __CHARMAP_PANEL_H__ */
