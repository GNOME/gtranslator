/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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
#include <sys/types.h>
#include <libxml/tree.h>

#include "parse.h"

/*
 * Non-GUI utility functions for gtranslator.
 */

/*
 * Really nasty wrapper macros which should expand to really common constructs.
 */
#define GTR_FREE(x) g_free((x)); (x)=NULL;
#define GTR_ITER(x) if((x)->next) { (x)=(x)->next; } else { break; }

/*
 * Returns the non-prefixed and non-suffixed filename back.
 */
gchar *gtranslator_utils_get_raw_file_name(const gchar *filename);

/*
 * Removes all punctuation characters from the given string.
 */
gchar *gtranslator_utils_strip_all_punctuation_chars(const gchar *str);

/*
 * Search the "environment path" in "envpath" for a non-NULL value and sets
 *  the "value" correspondingly.
 */
void gtranslator_utils_get_environment_value(const gchar *envpath, gchar **value);

/*
 * Remove the non-localizeed name of the given language -- if possible.
 */
const gchar *gtranslator_utils_get_english_language_name(const gchar *lang);

/*
 * Return the corresponding language name for the given locale.
 */
gchar *gtranslator_utils_get_language_name_by_locale_code(const gchar *locale_code);

/*
 * Sets the prefs values corresponding to the given language.
 */
void gtranslator_utils_set_language_values_by_language(const gchar *language);

/*
 * Remove the possibly generated temporary files of gtranslator.
 */
void gtranslator_utils_remove_temp_files(void); 

/*
 * Set up our temporary file names for the test compile run.
 */
void gtranslator_utils_get_compile_file_names(gchar **test_file, 
	gchar **output_file, gchar **result_file);

/*
 * Clean up the temporary files for the test compile run -- also free the variables.
 */
void gtranslator_utils_remove_compile_files(gchar **test_file,
	gchar **output_file, gchar **result_file);

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
 * Free's the list data and the list itself safely.
 */
void gtranslator_utils_free_list(GList *list, gboolean free_contents);

/*
 * Return the language environmental variable.
 */
gchar *gtranslator_utils_get_environment_locale(void);

/*
 * Returns a list of (full) filenames matching the given parameters in ths
 *  given directory; extension means extension _with_ point (".xml").
 */
GList *gtranslator_utils_file_names_from_directory(const gchar *directory,
	const gchar *extension, gboolean sort, gboolean strip_extension,
	gboolean with_full_path);

/*
 * Return the similarity percentage between the two given strings --
 *  this doesn't only sound like voodoo; it IS voodoo!
 */
gfloat gtranslator_utils_calculate_similarity(const gchar *a, const gchar *b);

/*
 * Check for matching of an entry of the list entry and the string -- returns
 *  '-1' on non-matching, else the position in the list.
 */
gint gtranslator_utils_stringlist_strcasecmp(GList *list, const gchar *string);

/*
 * Get the right localename/charset for the language.
 */
gchar *gtranslator_utils_get_locale_name(GtrPo *po);
gchar *gtranslator_utils_get_locale_charset(GtrPo *po);

/*
 * The lists for the language informations -- are all changed 
 *  accordingly automatically.
 */
extern GList *languages_list, *encodings_list, *lcodes_list, *group_emails_list,
	*bits_list;

/*
 * Creates the lists above and deletes/frees it.
 */
void gtranslator_utils_language_lists_create(void);
gboolean gtranslator_utils_language_lists_free(GtkWidget *widget, 
	gpointer useless);

/*
 * Setup the real language names ala "tr_TR" to get the localized values
 *  for the given "halfwise" language name.
 */
gchar *gtranslator_utils_get_full_language_name(gchar *lang);

/*
 * Own getline implementation to handle very long comments.
 */
gchar *gtranslator_utils_getline (FILE* stream);

/*
 * These functions are all taken from GAL and integrated simply to here.
 */
gchar *gtranslator_xml_get_string_prop_by_name_with_default(const xmlNode *parent,
	const xmlChar *prop_name, const gchar *default_string);

void  gtranslator_xml_set_string_prop_by_name(xmlNode *parent,
	const xmlChar *prop_name, const gchar *value);
						      			
gint gtranslator_xml_get_integer_prop_by_name_with_default(const xmlNode *parent,
	const xmlChar *prop_name, int default_int);
						      		
void gtranslator_xml_set_integer_prop_by_name(xmlNode *parent, const xmlChar *prop_name,
	gint value);

int  gtranslator_mkdir_hier (const char *path, mode_t mode);
	
#endif
