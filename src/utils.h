/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *		Gediminas Paulauskas <menesis@gtranslator.org>
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

#ifndef GTR_UTILS_H
#define GTR_UTILS_H 1

#include <glib.h>

#include "parse.h"

/*
 * Non-GUI utility functions for gtranslator.
 */
 
/*
 * Returns the non-prefixed and non-suffixed filename back.
 */
gchar *gtranslator_utils_get_raw_file_name(gchar *filename);

/*
 * Remove the possibly generated temporary file of gtranslator.
 */
void gtranslator_utils_remove_temp_file(void); 

/*
 * Return the filename of our generally used temporary filename.
 */
gchar *gtranslator_utils_get_temp_file_name(void);

/*
 * Return the name of our crash-security filename.
 */
gchar *gtranslator_utils_get_crash_file_name(void);

/*
 * Initialize and create the gtranslator directory ~/.gtranslator in the user's
 *  home directory.
 */
void gtranslator_utils_create_gtranslator_directory(void); 

/*
 * The autosave function.
 */
gboolean gtranslator_utils_autosave(gpointer foo_me_or_die);

/*
 * Returns whether the given file_uri's prefix is supported.
 */
gboolean gtranslator_utils_uri_supported(const gchar *file_uri);

/*
 * Returns a list of (full) filenames matching the given parameters in ths
 *  given directory; extension means extension _with_ point (".xml").
 */
GList *gtranslator_utils_file_names_from_directory(const gchar *directory,
	const gchar *extension, gboolean sort, gboolean strip_extension,
	gboolean with_full_path);

/*
 * Check for matching of an entry of the list entry and the string -- returns
 *  '-1' on non-matching, else the position in the list.
 */
gint gtranslator_utils_stringlist_strcasecmp(GList *list, const gchar *string);

/*
 * Get the right localename/charset for the language.
 */
gchar *gtranslator_utils_get_locale_name(void); 
gchar *gtranslator_utils_get_locale_charset(void);

/*
 * The lists for the language informations -- are all changed 
 *  accordingly automatically.
 */
GList *languages_list, *encodings_list, *lcodes_list, *group_emails_list,
	*bits_list;
	

/*
 * Creates the lists above and deletes/frees it.
 */
void gtranslator_utils_language_lists_create(void);
gboolean gtranslator_utils_language_lists_free(GtkWidget *widget, 
	gpointer useless);

/*
 * Convert the fg/bg settings from < 2001-03-03 to the new places.
 */
void gtranslator_utils_old_colors_to_new_location(void);

/*
 * Setup the real language names ala "tr_TR" to get the localized values
 *  for the given "halfwise" language name.
 */
gchar *gtranslator_utils_get_full_language_name(gchar *lang);

/*
 * Own getline implementation to handle very long comments.
 */
char * gtranslator_utils_getline (FILE* stream);
	
#endif
