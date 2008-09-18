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

#ifndef __NOTEBOOK_H__
#define __NOTEBOOK_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "tab.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_NOTEBOOK		(gtranslator_notebook_get_type ())
#define GTR_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_NOTEBOOK, GtranslatorNotebook))
#define GTR_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_NOTEBOOK, GtranslatorNotebookClass))
#define GTR_IS_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_NOTEBOOK))
#define GTR_IS_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_NOTEBOOK))
#define GTR_NOTEBOOK_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_NOTEBOOK, GtranslatorNotebookClass))

/* Private structure type */
typedef struct _GtranslatorNotebookPrivate	GtranslatorNotebookPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorNotebook		GtranslatorNotebook;

struct _GtranslatorNotebook
{
	GtkNotebook parent_instance;
	
	/*< private > */
	GtranslatorNotebookPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorNotebookClass	GtranslatorNotebookClass;

struct _GtranslatorNotebookClass
{
	GtkNotebookClass parent_class;

	void	 (* tab_close_request)
				    (GtranslatorNotebook *notebook,
				     GtranslatorTab      *tab);
};

/*
 * Public methods
 */
GType		 gtranslator_notebook_get_type	   	   (void) G_GNUC_CONST;

GType		 gtranslator_notebook_register_type	   (GTypeModule * module);

GtkWidget       *gtranslator_notebook_new	           (void);

void             gtranslator_notebook_add_page             (GtranslatorNotebook *notebook,
							    GtranslatorTab *pax);

void             gtranslator_notebook_remove_page          (GtranslatorNotebook *notebook,
							    gint i);

GtranslatorTab  *gtranslator_notebook_get_page             (GtranslatorNotebook *notebook);

G_END_DECLS

#endif /* __NOTEBOOK_H__ */
