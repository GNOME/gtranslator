/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef GTR_CONVERT_H
#define GTR_CONVERT_H 1

#include <glib.h>

/*
 * Small conversion helper functions to enable re-coding.
 */

/*
 * Converts from_encoding (per default "iso-8859-1") to to_encoding using
 *  iconv.
 */
gchar *gtranslator_convert_string(const gchar *string,
	const gchar *from_encoding, const gchar *to_encoding);

/*
 * Simple wrappers for converting from/to UTF-8.
 */
gchar *gtranslator_convert_string_to_utf8(const gchar *string,
	const gchar *from_encoding);

gchar *gtranslator_convert_string_from_utf8(const gchar *string,
	const gchar *to_encoding);

#endif
