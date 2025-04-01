/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2008  Igalia
 *               2022  Daniel Garcia Moreno <danigm@gnome.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Daniel Garcia Moreno <danigm@gnome.org>
 */

#pragma once

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-window.h"

G_BEGIN_DECLS

#define GTR_TYPE_APPLICATION (gtr_application_get_type ())

G_DECLARE_FINAL_TYPE (GtrApplication, gtr_application, GTR, APPLICATION, AdwApplication)

#define GTR_APP                         (GTR_APPLICATION (g_application_get_default ()))

GtrApplication  * gtr_application_new                    (void);
GtrWindow *gtr_application_create_window (GtrApplication *app);
GList *           gtr_application_get_views              (GtrApplication *app,
                                                          gboolean        original,
                                                          gboolean        translated);
GtrWindow        *gtr_application_get_active_window      (GtrApplication *app);

/* Non exported funcs */
const gchar     *_gtr_application_get_last_dir           (GtrApplication *app);
void             _gtr_application_set_last_dir           (GtrApplication *app,
                                                          const gchar    *last_dir);

G_END_DECLS
