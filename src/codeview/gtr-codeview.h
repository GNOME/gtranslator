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

#include <glib.h>
#include <glib-object.h>
#include "gtr-window.h"

G_BEGIN_DECLS

#define GTR_TYPE_CODE_VIEW (gtr_code_view_get_type())

G_DECLARE_FINAL_TYPE (GtrCodeView, gtr_code_view, GTR, CODE_VIEW, GObject)

GtrCodeView*  gtr_code_view_new (GtrWindow *window);

G_END_DECLS

