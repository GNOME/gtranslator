/* 
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#ifndef __DICT_PANEL_H__
#define __DICT_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-window.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_DICT_PANEL		(gtr_dict_panel_get_type ())
#define GTR_DICT_PANEL(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_DICT_PANEL, GtrDictPanel))
#define GTR_DICT_PANEL_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_DICT_PANEL, GtrDictPanelClass))
#define GTR_IS_DICT_PANEL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_DICT_PANEL))
#define GTR_IS_DICT_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_DICT_PANEL))
#define GTR_DICT_PANEL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_DICT_PANEL, GtrDictPanelClass))

typedef struct _GtrDictPanel            GtrDictPanel;
typedef struct _GtrDictPanelClass       GtrDictPanelClass;
typedef struct _GtrDictPanelPrivate     GtrDictPanelPrivate;

struct _GtrDictPanel
{
  GtkBox parent_instance;

  /*< private > */
  GtrDictPanelPrivate *priv;
};

struct _GtrDictPanelClass
{
  GtkBoxClass parent_class;
};

/*
 * Public methods
 */
GType gtr_dict_panel_get_type (void) G_GNUC_CONST;

GtkWidget *gtr_dict_panel_new (GtrWindow *window);

void _gtr_dict_panel_register_type (GTypeModule *type_module);

void gtr_dict_panel_set_position (GtrDictPanel * panel, gint pos);

G_END_DECLS
#endif /* __DICT_PANEL_H__ */
