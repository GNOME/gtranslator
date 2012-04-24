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
#include "gtr-window.h"
#include "gtr-msg.h"

G_BEGIN_DECLS

#define GTR_TYPE_VIEWER		(gtr_viewer_get_type ())
#define GTR_VIEWER(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_VIEWER, GtrViewer))
#define GTR_VIEWER_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_VIEWER, GtrViewerClass))
#define GTR_IS_VIEWER(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_VIEWER))
#define GTR_IS_VIEWER_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_VIEWER))
#define GTR_VIEWER_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_VIEWER, GtrViewerClass))

typedef struct _GtrViewer        GtrViewer;
typedef struct _GtrViewerPrivate GtrViewerPrivate;
typedef struct _GtrViewerClass   GtrViewerClass;

struct _GtrViewer
{
  GtkDialog parent_instance;

  /*< private > */
  GtrViewerPrivate *priv;
};

struct _GtrViewerClass
{
  GtkDialogClass parent_class;
};

GType   gtr_viewer_get_type           (void) G_GNUC_CONST;

GType   gtr_viewer_register_type      (GTypeModule  *module);

void    gtr_show_viewer               (GtrWindow    *window,
                                       const gchar  *path,
                                       gint          line);

G_END_DECLS
#endif /* __VIEWER_H__ */
