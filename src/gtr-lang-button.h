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

G_BEGIN_DECLS

#define GTR_TYPE_LANG_BUTTON (gtr_lang_button_get_type())

G_DECLARE_FINAL_TYPE (GtrLangButton, gtr_lang_button, GTR, LANG_BUTTON, GtkMenuButton)

GtrLangButton*  gtr_lang_button_new       ();
const gchar *   gtr_lang_button_get_lang  (GtrLangButton *self);
void            gtr_lang_button_set_lang  (GtrLangButton *self,
                                           const gchar   *name);

G_END_DECLS

