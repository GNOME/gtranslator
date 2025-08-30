/*
 * Copyright (C) 2001  Fatih Demir <kabalak@kabalak.net>
 *               2012  Ignacio Casal Quinteiro <icq@gnome.org>
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
 * Authors:
 *   Fatih Demir <kabalak@kabalak.net>
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 */

#ifndef GDA_UTILS_H
#define GDA_UTILS_H 1

#include <gtk/gtk.h>
#include <gio/gio.h>

GStrv gtr_gda_utils_split_string_in_words (const gchar *string);

#endif
