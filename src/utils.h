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
#include <gtk/gtkwidget.h>
#include <gtk/gtksignal.h>

#include "parse.h"

/*
 * Utility function for use within gtranslator.
 */
 
/*
 * Returns the non-prefixed and non-suffixed filename back.
 */
gchar *gtranslator_utils_get_raw_file_name(gchar *filename);

/*
 * Returns a list of (full) filenames matching the given parameters in ths
 *  given directory; extension means extension _with_ point (".xml").
 */
GList *gtranslator_utils_file_names_from_directory(const gchar *directory,
	const gchar *extension, gboolean sort, gboolean strip_extension);

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
 * Show the homepage of gtranslator.
 */
void gtranslator_utils_show_home_page(GtkWidget  *widget, gpointer useless);

/*
 * Invert the dots in the message.
 */
void gtranslator_utils_invert_dot(gchar *str);

/*
 * Routines for saving/restoring/setting geometry of the main window.
 */
void gtranslator_utils_save_geometry(void);
void gtranslator_utils_restore_geometry(gchar *gstr);

/*
 * Convenience functions for adding items 
 */
GtkWidget *gtranslator_utils_attach_combo_with_label(GtkWidget * table, 
	gint row, const char *label_text, GList * list, const char *value,
	GtkSignalFunc callback, gpointer user_data);

GtkWidget *gtranslator_utils_attach_entry_with_label(GtkWidget * table, 
	gint row, const char *label_text, const char *value,
	GtkSignalFunc callback);

GtkWidget *gtranslator_utils_attach_toggle_with_label(GtkWidget * table, 
	gint row, const char *label_text, gboolean value,
	GtkSignalFunc callback);

GtkWidget *gtranslator_utils_attach_text_with_label(GtkWidget * table, 
	gint row, const char *label_text, const char *value,
	GtkSignalFunc callback);

/*
 * Returns the added GtkTable to the preferences box.
 */ 
GtkWidget *gtranslator_utils_append_page_to_preferences_dialog(
	GtkWidget * probox, gint rows, gint cols, const char *label_text);

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
 * Checks the given file for the right permissions for
 *  open/writing the files.
 */
gboolean gtranslator_utils_check_file_permissions(GtrPo *po_file);

/*
 * Convert the fg/bg settings from < 2001-03-03 to the new places.
 */
void gtranslator_utils_old_colors_to_new_location(void);

char * gtranslator_utils_getline (FILE* stream);
	
#endif
