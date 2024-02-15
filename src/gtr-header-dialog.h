/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include "gtr-window.h"

G_BEGIN_DECLS

#define GTR_TYPE_HEADER_DIALOG (gtr_header_dialog_get_type ())
G_DECLARE_FINAL_TYPE (GtrHeaderDialog, gtr_header_dialog, GTR, HEADER_DIALOG, AdwPreferencesDialog)

/*
 * Public methods
 */
void gtr_show_header_dialog (GtrWindow * window);

 G_END_DECLS
