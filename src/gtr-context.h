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

#ifndef __CONTEXT_PANEL_H__
#define __CONTEXT_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_CONTEXT_PANEL		(gtranslator_context_panel_get_type ())
#define GTR_CONTEXT_PANEL(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_CONTEXT_PANEL, GtrContextPanel))
#define GTR_CONTEXT_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_CONTEXT_PANEL, GtrContextPanelClass))
#define GTR_IS_CONTEXT_PANEL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_CONTEXT_PANEL))
#define GTR_IS_CONTEXT_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_CONTEXT_PANEL))
#define GTR_CONTEXT_PANEL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_CONTEXT_PANEL, GtrContextPanelClass))
/* Private structure type */
typedef struct _GtrContextPanelPrivate GtrContextPanelPrivate;

/*
 * Main object structure
 */
typedef struct _GtrContextPanel GtrContextPanel;

struct _GtrContextPanel
{
  GtkVBox parent_instance;

  /*< private > */
  GtrContextPanelPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrContextPanelClass GtrContextPanelClass;

struct _GtrContextPanelClass
{
  GtkVBoxClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_context_panel_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_context_panel_register_type (GTypeModule * module);

     GtkWidget *gtranslator_context_panel_new (GtkWidget * tab);

     GtkTextView
       *gtranslator_context_panel_get_context_text_view
       (GtrContextPanel * panel);

G_END_DECLS
#endif /* __CONTEXT_PANEL_H__ */
