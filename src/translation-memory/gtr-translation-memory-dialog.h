/*
 * Copyright (C) 2012 Ignacio Casal Quinteiro <icq@gnome.org>
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <gtk/gtk.h>
#include <adwaita.h>
#include "gtr-translation-memory.h"

G_BEGIN_DECLS

#define GTR_TYPE_TRANSLATION_MEMORY_DIALOG             (gtr_translation_memory_dialog_get_type ())
G_DECLARE_FINAL_TYPE (GtrTranslationMemoryDialog,
                      gtr_translation_memory_dialog, GTR,
                      TRANSLATION_MEMORY_DIALOG, AdwDialog)

GtkWidget       *gtr_translation_memory_dialog_new          (GtrTranslationMemory *translation_memory);

G_END_DECLS
