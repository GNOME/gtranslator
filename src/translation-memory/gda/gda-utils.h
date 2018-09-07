/*
 * (C) 2001     Fatih Demir <kabalak@kabalak.net>
 *     2012     Ignacio Casal Quinteiro <icq@gnome.org>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or   
 * (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Fatih Demir <kabalak@kabalak.net>
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 */

#ifndef GDA_UTILS_H
#define GDA_UTILS_H 1

#include <gtk/gtk.h>
#include <gio/gio.h>

gchar **gtr_gda_utils_split_string_in_words (const gchar *string);

#endif
