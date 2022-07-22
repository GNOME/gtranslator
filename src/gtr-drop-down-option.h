/*
 * Copyright (C) 2022  Daniel Garcia Moreno <danigm@gnome.org>
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

G_BEGIN_DECLS

#define GTR_TYPE_DROP_DOWN_OPTION (gtr_drop_down_option_get_type ())

G_DECLARE_FINAL_TYPE (GtrDropDownOption, gtr_drop_down_option, GTR, DROP_DOWN_OPTION, GObject)

GtrDropDownOption*  gtr_drop_down_option_new (const char *name, const char *description);
gboolean gtr_drop_down_option_equal (GtrDropDownOption *opt1, GtrDropDownOption *opt2);
const char *gtr_drop_down_option_get_name (GtrDropDownOption *opt);
const char *gtr_drop_down_option_get_description (GtrDropDownOption *opt);

G_END_DECLS

