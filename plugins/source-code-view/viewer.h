/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#ifndef __VIEWER_H__
#define __VIEWER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "window.h"
#include "msg.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_VIEWER		(gtranslator_viewer_get_type ())
#define GTR_VIEWER(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_VIEWER, GtranslatorViewer))
#define GTR_VIEWER_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_VIEWER, GtranslatorViewerClass))
#define GTR_IS_VIEWER(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_VIEWER))
#define GTR_IS_VIEWER_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_VIEWER))
#define GTR_VIEWER_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_VIEWER, GtranslatorViewerClass))

/* Private structure type */
typedef struct _GtranslatorViewerPrivate	GtranslatorViewerPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorViewer		GtranslatorViewer;

struct _GtranslatorViewer
{
	GtkDialog parent_instance;
	
	/*< private > */
	GtranslatorViewerPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorViewerClass	GtranslatorViewerClass;

struct _GtranslatorViewerClass
{
	GtkDialogClass parent_class;
};

/*
 * Public methods
 */
GType		 gtranslator_viewer_get_type               (void) G_GNUC_CONST;

GType		 gtranslator_viewer_register_type          (GTypeModule * module);

void	         gtranslator_show_viewer                   (GtranslatorWindow *window,
							    const gchar *path,
							    gint line);

G_END_DECLS

#endif /* __VIEWER_H__ */
