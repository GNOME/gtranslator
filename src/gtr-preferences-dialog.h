/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * Copyright (C) 2022 Daniel Garcia <danigm@gnome.org>
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

#include <gtk/gtk.h>
#include <adwaita.h>

#include "gtr-window.h"

G_BEGIN_DECLS

#define GTR_TYPE_PREFERENCES_DIALOG (gtr_preferences_dialog_get_type ())

G_DECLARE_FINAL_TYPE (GtrPreferencesDialog, gtr_preferences_dialog, GTR, PREFERENCES_DIALOG, AdwPreferencesWindow)

/* Public methods */
GType           gtr_preferences_dialog_get_type         (void)G_GNUC_CONST;

void            gtr_show_preferences_dialog             (GtrWindow *window);
int             gtr_prefs_get_remove_obsolete           (void);

G_END_DECLS
