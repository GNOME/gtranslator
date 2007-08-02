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
 *
 */

#ifndef __VIEW_H__
#define __VIEW_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_VIEW		(gtranslator_view_get_type ())
#define GTR_VIEW(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_VIEW, GtranslatorView))
#define GTR_VIEW_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_VIEW, GtranslatorViewClass))
#define GTR_IS_VIEW(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_VIEW))
#define GTR_IS_VIEW_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_VIEW))
#define GTR_VIEW_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_VIEW, GtranslatorViewClass))

/* Private structure type */
typedef struct _GtranslatorViewPrivate	GtranslatorViewPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorView		GtranslatorView;

struct _GtranslatorView
{
	GtkSourceView parent_instance;
	
	/*< private > */
	GtranslatorViewPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorViewClass	GtranslatorViewClass;

struct _GtranslatorViewClass
{
	GtkSourceViewClass parent_class;
};

/*
 * Public methods
 */
GType		 gtranslator_view_get_type	   (void) G_GNUC_CONST;
GType		 gtranslator_view_register_type    (GTypeModule * module);
GtkWidget	*gtranslator_view_new	           (void);

G_END_DECLS

#endif /* __VIEW_H__ */
