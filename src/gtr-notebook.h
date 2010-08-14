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

#include "gtr-tab.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_NOTEBOOK		(gtr_notebook_get_type ())
#define GTR_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_NOTEBOOK, GtrNotebook))
#define GTR_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_NOTEBOOK, GtrNotebookClass))
#define GTR_IS_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_NOTEBOOK))
#define GTR_IS_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_NOTEBOOK))
#define GTR_NOTEBOOK_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_NOTEBOOK, GtrNotebookClass))

/*
 * Main object structure
 */
typedef struct _GtrNotebook GtrNotebook;

struct _GtrNotebook
{
  GtkNotebook parent_instance;
};

/*
 * Class definition
 */
typedef struct _GtrNotebookClass GtrNotebookClass;

struct _GtrNotebookClass
{
  GtkNotebookClass parent_class;

  void (*tab_close_request) (GtrNotebook * notebook, GtrTab * tab);
};

/*
 * Public methods
 */
GType
gtr_notebook_get_type (void)
  G_GNUC_CONST;

     GType gtr_notebook_register_type (GTypeModule * module);

     GtkWidget *gtr_notebook_new (void);

     void gtr_notebook_add_page (GtrNotebook * notebook, GtrTab * pax);

     void gtr_notebook_remove_page (GtrNotebook * notebook, gint i);

     GtrTab *gtr_notebook_get_page (GtrNotebook * notebook);

G_END_DECLS
#endif /* __NOTEBOOK_H__ */
