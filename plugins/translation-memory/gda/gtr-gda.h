/*
 * Copyright (C) 2010  Andrey Kutejko <andy128k@gmail.com>
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

#ifndef __GDA_BACKEND_H__
#define __GDA_BACKEND_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTR_TYPE_GDA		(gtr_gda_get_type ())
#define GTR_GDA(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_GDA, GtrGda))
#define GTR_GDA_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_GDA, GtrGdaClass))
#define GTR_IS_GDA(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_GDA))
#define GTR_IS_GDA_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_GDA))
#define GTR_GDA_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_GDA, GtrGdaClass))

typedef struct _GtrGdaPrivate GtrGdaPrivate;
typedef struct _GtrGda        GtrGda;
typedef struct _GtrGdaClass   GtrGdaClass;

struct _GtrGda
{
  GObject parent_instance;

  /*< private > */
  GtrGdaPrivate *priv;
};

struct _GtrGdaClass
{
  GObjectClass parent_class;
};

GType                   gtr_gda_get_type                (void) G_GNUC_CONST;

GType                   gtr_gda_register_type           (GTypeModule *module);

GtrGda                 *gtr_gda_new                     (void);

G_END_DECLS
#endif /* __GDA_BACKEND_H__ */
