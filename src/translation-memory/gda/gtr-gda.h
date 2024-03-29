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

G_DECLARE_FINAL_TYPE (GtrGda, gtr_gda, GTR, GDA, GObject)

struct _GtrGda
{
  GObject parent_instance;
};

struct _GtrGdaClass
{
  GObjectClass parent_class;
};

GtrGda                 *gtr_gda_new                     (void);

G_END_DECLS
#endif /* __GDA_BACKEND_H__ */
