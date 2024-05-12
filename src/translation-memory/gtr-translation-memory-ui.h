/*
 * Copyright (C) 2008 Igalia  
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
 */

#pragma once

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <adwaita.h>

G_BEGIN_DECLS

#define GTR_TYPE_TRANSLATION_MEMORY_UI          (gtr_translation_memory_ui_get_type ())

G_DECLARE_FINAL_TYPE (GtrTranslationMemoryUi, gtr_translation_memory_ui, GTR, TRANSLATION_MEMORY_UI, AdwBin)

GType            gtr_translation_memory_ui_register_type        (GTypeModule            *module);
GtkWidget       *gtr_translation_memory_ui_new                  (GtkWidget              *tab,
                                                                 GtrTranslationMemory   *translation_memory);

G_END_DECLS
