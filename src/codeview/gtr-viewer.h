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

#pragma once

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "gtr-window.h"
#include "gtr-msg.h"

G_BEGIN_DECLS

#define GTR_TYPE_VIEWER (gtr_viewer_get_type())

G_DECLARE_FINAL_TYPE (GtrViewer, gtr_viewer, GTR, VIEWER, GtkWindow)

GType   gtr_viewer_register_type      (GTypeModule  *module);

void    gtr_show_viewer               (GtrWindow    *window,
                                       const gchar  *path,
                                       gint          line);

G_END_DECLS
