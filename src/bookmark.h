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

#ifndef GTR_BOOKMARK_H
#define GTR_BOOKMARK_H 1

#include <glib.h>

/*
 * A gtranslator-specific ID container -- is generated out of the specs for
 *  the po-file and the message.
 *
 * General format: gtranslator_bookmark:po_file#po_language/po_version/po_date/po_position
 *
 */
typedef struct
{
	gchar	*po_file;
	gchar	*po_language;
	gchar	*po_version;
	gchar	*po_date;

	gint	 po_position;
} GtrBookmark;

#define GTR_BOOKMARK(x) ((GtrBookmark *) x)

/*
 * Create and return a GtrBookmark from the current position & po file -- 
 *  if a file is opened yet.
 */
GtrBookmark *gtranslator_bookmark_new(void);

/*
 * Creates the bookmark from a given gchar-string formatted by gtranslator.
 */
GtrBookmark *gtranslator_bookmark_new_from_string(const gchar *string);

/*
 * Create and convert the generated bookmark to a string.
 */
gchar *gtranslator_bookmark_new_bookmark_string(void);

/*
 * The otherway conversion: convert a GtrBookmark into a plain string.
 */
gchar *gtranslator_bookmark_string_from_bookmark(GtrBookmark *bookmark);

/*
 * Open the bookmark -- file & position => version and date should be only some
 *  kind of aside-factors.
 */
void gtranslator_bookmark_open(GtrBookmark *bookmark);

/*
 * Is the given GtrBookmark resolvable at all -- is the file present and do the
 *  other specs also match?!
 */
gboolean gtranslator_bookmark_resolvable(GtrBookmark *bookmark);

/*
 * Return whether the two GtrBookmark's are equal.
 */
gboolean gtranslator_bookmark_equal(GtrBookmark *one, GtrBookmark *two);

/*
 * Smally different: compare the GtrBookmark and the bookmark string.
 */
gboolean gtranslator_bookmark_string_equal(GtrBookmark *bookmark, const gchar *string); 

/*
 * Add the given GtrBookmark to the IDs list.
 */
void gtranslator_bookmark_add(GtrBookmark *bookmark);

/*
 * Remove the given GtrBookmark from the IDs list -- the result is
 *  passed as the return-value.
 */
gboolean gtranslator_bookmark_remove(GtrBookmark *bookmark);

/*
 * Check whether the given GtrBookmark is already included in 
 *  our IDs list -- simple return TRUE in this case.
 */
gboolean gtranslator_bookmark_search(GtrBookmark *bookmark);

/*
 * Read in our stored GtrBookmarks list -- restored the list from the
 *  preferences.
 */
void gtranslator_bookmark_load_list(void);

/*
 * Saves the GtrBookmarks list in our preferences.
 */
void gtranslator_bookmark_save_list(void);

/*
 * Copy the given GtrBookmark safely.
 */
GtrBookmark *gtranslator_bookmark_copy(GtrBookmark *bookmark);

/*
 * Free the given GtrBookmark.
 */
void gtranslator_bookmark_free(GtrBookmark *bookmark);

#endif
