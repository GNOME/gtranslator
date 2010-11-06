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

#ifndef __ALTERNATE_LANG_PANEL_H__
#define __ALTERNATE_LANG_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTR_TYPE_ALTERNATE_LANG_PANEL		(gtr_alternate_lang_panel_get_type ())
#define GTR_ALTERNATE_LANG_PANEL(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_ALTERNATE_LANG_PANEL, GtrAlternateLangPanel))
#define GTR_ALTERNATE_LANG_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_ALTERNATE_LANG_PANEL, GtrAlternateLangPanelClass))
#define GTR_IS_ALTERNATE_LANG_PANEL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_ALTERNATE_LANG_PANEL))
#define GTR_IS_ALTERNATE_LANG_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_ALTERNATE_LANG_PANEL))
#define GTR_ALTERNATE_LANG_PANEL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_ALTERNATE_LANG_PANEL, GtrAlternateLangPanelClass))

typedef struct _GtrAlternateLangPanel        GtrAlternateLangPanel;
typedef struct _GtrAlternateLangPanelPrivate GtrAlternateLangPanelPrivate;
typedef struct _GtrAlternateLangPanelClass   GtrAlternateLangPanelClass;

struct _GtrAlternateLangPanel
{
  GtkVBox parent_instance;

  /*< private > */
  GtrAlternateLangPanelPrivate *priv;
};

struct _GtrAlternateLangPanelClass
{
  GtkVBoxClass parent_class;
};

GType gtr_alternate_lang_panel_get_type (void) G_GNUC_CONST;

GtkWidget *gtr_alternate_lang_panel_new (GtkWidget *tab);

void _gtr_alternate_lang_panel_register_type (GTypeModule *type_module);

G_END_DECLS
#endif /* __ALTERNATE_LANG_PANEL_H__ */
