/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <icq@gnome.org>
 *               2022  Daniel Garcia Moreno <danigm@gnome.org>
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

#pragma once

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-msg.h"
#include "translation-memory/gtr-translation-memory.h"

G_BEGIN_DECLS
#define GTR_TYPE_CONTEXT_PANEL  (gtr_context_panel_get_type ())

G_DECLARE_FINAL_TYPE (GtrContextPanel, gtr_context_panel, GTR, CONTEXT_PANEL, GtkBox)

/*
 * Public methods
 */
GtkWidget        *gtr_context_panel_new                   (void);
void              gtr_context_init_tm                     (GtrContextPanel *panel, GtrTranslationMemory *tm);
void              gtr_context_add_path                    (GtrContextPanel *panel, const char *filename, int line);

G_END_DECLS
