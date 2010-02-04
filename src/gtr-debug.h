/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

/**
 * SECTION:debug
 * @title: Debugging
 * @short_description: Debug functions
 * @include: gtranslator/debug.h
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <glib.h>

G_BEGIN_DECLS
/**
 * DEBUG_PRINT:
 *
 * Equivalent to g_message(), except it has only effect when DEBUG is
 * defined. Used for printing debug messages.
 */
#ifdef DEBUG
#  define DEBUG_PRINT g_message
#else
#  define DEBUG_PRINT(...)
#endif
  G_END_DECLS
#endif /* __DEBUG_H__ */
