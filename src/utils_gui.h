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

#ifndef GTR_UTILS_GUI_H
#define GTR_UTILS_GUI_H 1

#include "parse.h"
#include "stylistics.h"

#include <gtk/gtkwidget.h>
#include <gtk/gtksignal.h>

/*
 * GUI related utility functions for gtranslator.
 */

/*
 * Show an error messagebox to the user. 
 */
GtkWidget *gtranslator_utils_error_dialog(gchar *format, ...);

/*
 * Show the homepage of gtranslator.
 */
void gtranslator_utils_show_home_page(GtkWidget  *widget, gpointer useless);

/*
 * Invert the dots in the message.
 */
gchar *gtranslator_utils_invert_dot(gchar *str);

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
	gboolean editable, GCallback callback, gpointer user_data);

GtkWidget *gtranslator_utils_attach_entry_with_label(GtkWidget * table, 
	gint row, const char *label_text, const char *value,
	GCallback callback);

GtkWidget *gtranslator_utils_attach_toggle_with_label(GtkWidget * table, 
	gint row, const char *label_text, gboolean value,
	GCallback callback);

GtkWidget *gtranslator_utils_attach_text_with_label(GtkWidget * table, 
	gint row, const char *label_text, const char *value,
	GCallback callback);

GtkWidget *gtranslator_utils_attach_spin_with_label(GtkWidget *table,
	gint row, const gchar *label_text, gfloat minimum, gfloat maximum,
	gfloat value, GCallback callback);
	
GtkWidget *gtranslator_utils_attach_font_with_label(GtkWidget *table,
	gint row, const gchar *label_text, const gchar *title_text,
	const gchar *fontspec, GCallback callback);
	
GtkWidget *gtranslator_utils_attach_color_with_label(GtkWidget *table,
	gint row, const gchar *label_text, const gchar *title_text,
	ColorType color_type, GCallback callback);

/*
 * Checks the given file for the right permissions for
 *  open/writing the files.
 */
gboolean gtranslator_utils_check_file_permissions(GtrPo *po_file);

/*
 * Checks if we've been started with the given file yet.
 */
gboolean gtranslator_utils_check_file_being_open(const gchar *filename);

/*
 * Check for a given program being in the user's path -- should be useful
 *  for the "differently" opening/saving functions...
 */
gboolean gtranslator_utils_check_program(const gchar *program_name,
	const gint type_int);

#endif
