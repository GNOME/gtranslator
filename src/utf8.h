/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
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
gboolean gtranslator_utf8_po_file_is_utf8(GtrPo *po);

/*
 * Convert the msgid & msgstr's to/from UTF-8.
 */
void gtranslator_utf8_convert_message_to_utf8(GtrMsg *msg, const gchar *orig_enc);
void gtranslator_utf8_convert_message_from_utf8(GtrMsg *msg, const gchar *orig_enc);

/*
 * Convert the whole messages to/from UTF-8.
 */
void gtranslator_utf8_convert_po_to_utf8(GtrPo *po);
void gtranslator_utf8_convert_po_from_utf8(GtrPo *po);

/*
 * Simple typing castration is shown in here...
 */
gchar *gtranslator_utf8_convert_to_utf8(const gchar *encoding, const gchar *str);

#endif
