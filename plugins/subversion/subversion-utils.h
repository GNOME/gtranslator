/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 *     Based on anjuta subversion plugin.
 *     Copyright (C) James Liggett 2007 <jrliggett@cox.net>
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

#ifndef GTR_SUBVERSION_UTILS_H
#define GTR_SUBVERSION_UTILS_H 1

#include <gtk/gtk.h>
#include <gio/gio.h>
#include "window.h"
#include "command.h"

void subversion_utils_report_errors (GtranslatorWindow * window,
				     GtranslatorCommand * command,
				     guint error_code);

gchar *subversion_utils_get_log_from_textview (GtkWidget * textview);

void subversion_utils_from_file_to_file (GInputStream * istream,
					 GOutputStream * ostream);

gchar *subversion_utils_get_changelog_entry_from_view (GtkWidget * view);

#endif
