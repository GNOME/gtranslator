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

#ifndef GTR_TRANSLATION_MEMORY_H
#define GTR_TRANSLATION_MEMORY_H 1

#include "messages.h"

/*
 * The general header informations for the translation memories.
 */
typedef struct
{
	gchar 	*tool;
	gchar	*tool_version;
	gchar	*format;
	gchar	*encoding;

	gchar	*data_type;
	gchar	*seg_type;
	gchar	*adminlang;
	gchar	*srclang;

	gchar	*id;
	gchar	*date;
} GtrTMHeader;

/*
 * The TM content.
 */
typedef struct
{
	gchar		*filename;
	GtrTMHeader	*header;
	GList		*units;
} GtrTM;

/*
 * A gtranslator-specific ID container -- is generated out oif the specs for
 *  the po-file and the message.
 */
typedef struct
{
	gchar	*po_file;
	gchar	*po_language;
	gchar	*po_version;
	gchar	*po_date;

	gint	 po_position;
} GtrID;

/*
 * Should be an interface to append a memory entry to the currently used TM.
 */
void gtranslator_tm_add(const gchar *message, const gchar *translation,
	const gchar *language_code, const gchar *eventual_id);

/*
 * Return the pos if there's already such an entry named "message" in the TM,
 *  or -1 if not.
 */
gint gtranslator_tm_query_for_message(const gchar *message, 
	const gchar *eventual_id);

/*
 * Query the TM for the ID gtranslator generates internally for the entries.
 */
gint gtranslator_tm_query_for_id(const gchar *eventual_id);

#endif
