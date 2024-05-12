/*
 * Copyright (C) 2018  Daniel Garcia Moreno <danigm@gnome.org>
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
 */

#pragma once

#include <gtk/gtk.h>
#include <adwaita.h>

G_BEGIN_DECLS

#define GTR_TYPE_PROJECTS (gtr_projects_get_type())

G_DECLARE_FINAL_TYPE (GtrProjects, gtr_projects, GTR, PROJECTS,
                      AdwNavigationPage)

GtrProjects*  gtr_projects_new         (GtrWindow *window);
void          gtr_projects_recent_add  (GtrProjects *self,
                                        GFile *location,
                                        gchar *jkkproject_id);

G_END_DECLS

