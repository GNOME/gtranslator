/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __IO_ERROR_MESSAGE_AREA_H__
#define __IO_ERROR_MESSAGE_AREA_H__

#include <glib.h>
#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

GtkWidget    *create_error_message_area              (const gchar *primary_text,
						      const gchar *secondary_text);
G_END_DECLS

#endif
