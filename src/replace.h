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

#ifndef GTR_REPLACE_H
#define GTR_REPLACE_H 1

#include <glib.h>

/*
 * The Replace structure with all the informations about a replace request.
 */
typedef struct
{
	gchar 		*string;
	gchar 		*replace_string;

	gboolean 	replace_all;
	gint 		start_offset;

	gboolean	replace_in_comments;
	gboolean	replace_in_english;
	gboolean	replace_in_translation;
} GtrReplace;

#define GTR_REPLACE(x) ((GtrReplace *) x)

/*
 * Creation/deletion of a GtrReplace structure.
 */
GtrReplace *gtranslator_replace_new(const gchar *find, const gchar *replace,
	gboolean do_it_for_all, gint start, gboolean replace_in_comments,
	gboolean replace_in_english, gboolean replace_in_translation);

void gtranslator_replace_free(GtrReplace **replace);

/*
 * Run the replace action on the given GtrReplace object.
 */
void gtranslator_replace_run(GtrReplace *replace);

#endif
