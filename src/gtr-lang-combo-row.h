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

#define GTR_TYPE_LANG_COMBO_ROW (gtr_lang_combo_row_get_type())

G_DECLARE_FINAL_TYPE (GtrLangComboRow, gtr_lang_combo_row, GTR, LANG_COMBO_ROW, AdwComboRow)

GtrLangComboRow*  gtr_lang_combo_row_new       (void);

const gchar *     gtr_lang_combo_row_get_lang  (GtrLangComboRow *self);
void              gtr_lang_combo_row_set_lang  (GtrLangComboRow *self,
                                                const gchar     *name);

G_END_DECLS

