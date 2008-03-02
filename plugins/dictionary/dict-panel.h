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

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_DICT_PANEL		(gtranslator_dict_panel_get_type ())
#define GTR_DICT_PANEL(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_DICT_PANEL, GtranslatorDictPanel))
#define GTR_DICT_PANEL_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_DICT_PANEL, GtranslatorDictPanelClass))
#define GTR_IS_DICT_PANEL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_DICT_PANEL))
#define GTR_IS_DICT_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_DICT_PANEL))
#define GTR_DICT_PANEL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_DICT_PANEL, GtranslatorDictPanelClass))

/* Private structure type */
typedef struct _GtranslatorDictPanelPrivate	GtranslatorDictPanelPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorDictPanel		GtranslatorDictPanel;

struct _GtranslatorDictPanel
{
	GtkVBox parent_instance;
	
	/*< private > */
	GtranslatorDictPanelPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorDictPanelClass	GtranslatorDictPanelClass;

struct _GtranslatorDictPanelClass
{
	GtkVBoxClass parent_class;
};

/*
 * Public methods
 */
GType		 gtranslator_dict_panel_get_type	   (void) G_GNUC_CONST;

GType		 gtranslator_dict_panel_register_type    (GTypeModule * module);

GtkWidget *	 gtranslator_dict_panel_new 		   (void);

void             gtranslator_dict_panel_set_position     (GtranslatorDictPanel *panel,
						    gint pos);

G_END_DECLS

#endif /* __DICT_PANEL_H__ */
