/*
 * charmap-panel.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2006 - Steve Frécinaux
 *
 * gtranslator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gtranslator; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __GTR_CHARMAP_PANEL_H__
#define __GTR_CHARMAP_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#ifdef HAVE_GUCHARMAP_2
#include <gucharmap/gucharmap.h>
#else
#include <gucharmap/gucharmap-table.h>
#endif

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_CHARMAP_PANEL		(gtranslator_charmap_panel_get_type ())
#define GTR_CHARMAP_PANEL(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_CHARMAP_PANEL, GtrCharmapPanel))
#define GTR_CHARMAP_PANEL_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_CHARMAP_PANEL, GtrCharmapPanelClass))
#define GTR_IS_CHARMAP_PANEL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_CHARMAP_PANEL))
#define GTR_IS_CHARMAP_PANEL_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_CHARMAP_PANEL))
#define GTR_CHARMAP_PANEL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_CHARMAP_PANEL, GtrCharmapPanelClass))
/* Private structure type */
typedef struct _GtrCharmapPanelPrivate GtrCharmapPanelPrivate;

/*
 * Main object structure
 */
typedef struct _GtrCharmapPanel GtrCharmapPanel;

struct _GtrCharmapPanel
{
  GtkVBox parent_instance;

  /*< private > */
  GtrCharmapPanelPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrCharmapPanelClass GtrCharmapPanelClass;

struct _GtrCharmapPanelClass
{
  GtkVBoxClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_charmap_panel_get_type (void)
  G_GNUC_CONST;
     GType gtranslator_charmap_panel_register_type (GTypeModule * module);
     GtkWidget *gtranslator_charmap_panel_new (void);

#ifdef HAVE_GUCHARMAP_2
     GucharmapChartable
       *gtranslator_charmap_panel_get_chartable (GtrCharmapPanel *
						 panel);
#else
     GucharmapTable
       *gtranslator_charmap_panel_get_table (GtrCharmapPanel * panel);
#endif

G_END_DECLS
#endif /* __CHARMAP_PANEL_H__ */
