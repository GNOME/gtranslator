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

#ifndef GTR_UTF8_H
#define GTR_UTF8_H 1

#include "parse.h"

/*
 * Return TRUE/FALSE if the current file is a UTF-8 file.
 */
gboolean gtranslator_utf8_po_file_is_utf8(void);

/*
 * Convert the msgid & msgstr's to/from UTF-8.
 */
void gtranslator_utf8_convert_message_to_utf8(GtrMsg **msg);
void gtranslator_utf8_convert_message_from_utf8(GtrMsg **msg);

/*
 * Convert the whole messages to/from UTF-8.
 */
void gtranslator_utf8_convert_po_to_utf8(void);
void gtranslator_utf8_convert_po_from_utf8(void);

/*
 * Gets the UTF-8 part of the current string/message and ports it into "plain"
 *  form.
 */
gchar *gtranslator_utf8_get_plain_string(gchar **string); 
gchar *gtranslator_utf8_get_plain_msgstr(GtrMsg **message); 

/*
 * Get me the back to UTF-8!
 */
gchar *gtranslator_utf8_get_utf8_string(gchar **string); 

/*
 * Reads the content for the current GtrMsg from the text boxes and
 *  sets up the right UTF-8 text-parts.
 */
void gtranslator_utf8_get_utf8_for_current_message(void); 

#endif
