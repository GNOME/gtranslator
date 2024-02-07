/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 *               2008  Igalia
 * Copyright (C) 2022 Daniel Garcia <dani@danigm.net>
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
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Author: Daniel Garcia <dani@danigm.net>
 */

#pragma once

#include <adwaita.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "gtr-profile.h"

G_BEGIN_DECLS

#define GTR_TYPE_PROFILE_DIALOG (gtr_profile_dialog_get_type ())

G_DECLARE_FINAL_TYPE (GtrProfileDialog, gtr_profile_dialog, GTR,
                      PROFILE_DIALOG, AdwDialog)

/*
 * Public methods
 */
GtrProfileDialog   *gtr_profile_dialog_new              (GtrProfile *profile);
GtrProfile         *gtr_profile_dialog_get_profile      (GtrProfileDialog *dlg);
gboolean            gtr_profile_dialog_get_editing      (GtrProfileDialog *dlg);

G_END_DECLS
