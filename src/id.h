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

#ifndef GTR_ID_H
#define GTR_ID_H 1

#include <glib.h>

/*
 * A gtranslator-specific ID container -- is generated out of the specs for
 *  the po-file and the message.
 *
 * General format: gtranslator_id:po_file#po_language/po_version/po_date/po_position
 *
 */
typedef struct
{
	gchar	*po_file;
	gchar	*po_language;
	gchar	*po_version;
	gchar	*po_date;

	gint	 po_position;
} GtrID;

#define GTR_ID(x) ((GtrID *) x)

/*
 * The used GList for the GtrID's -- a general way to handle with this
 *  list is supplied by the gtranslator_id_* methods.
 */
GList *gtranslator_ids;

/*
 * Create and return a GtrID from the current position & po file -- 
 *  if a file is opened yet.
 */
GtrID *gtranslator_id_new(void);

/*
 * Creates the id from a given gchar-string formatted by gtranslator.
 */
GtrID *gtranslator_id_new_from_string(const gchar *string);

/*
 * Create and convert the generated id to a string.
 */
gchar *gtranslator_id_new_id_string(void);

/*
 * The otherway conversion: convert a GtrID into a plain string.
 */
gchar *gtranslator_id_string_from_id(GtrID *id);

/*
 * Is the given GtrID resolvable at all -- is the file present and do the
 *  other specs also match?!
 */
gboolean gtranslator_id_resolvable(GtrID *id);

/*
 * Return whether the two GtrID's are equal.
 */
gboolean gtranslator_id_equal(GtrID *one, GtrID *two);

/*
 * Smally different: compare the GtrID and the id string.
 */
gboolean gtranslator_id_string_equal(GtrID *id, const gchar *string); 

/*
 * Add the given GtrID to the IDs list.
 */
void gtranslator_id_add(GtrID *id);

/*
 * Remove the given GtrID from the IDs list -- the result is
 *  passed as the return-value.
 */
gboolean gtranslator_id_remove(GtrID *id);

/*
 * Check whether the given GtrID is already included in 
 *  our IDs list -- simple return TRUE in this case.
 */
gboolean gtranslator_id_search(GtrID *id);

/*
 * Copy the given GtrID safely.
 */
GtrID *gtranslator_id_copy(GtrID *id);

/*
 * Free the given GtrID.
 */
void gtranslator_id_free(GtrID *id); 

#endif
