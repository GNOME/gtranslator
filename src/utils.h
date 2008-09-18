/*
 * (C) 2001 	Fatih Demir <kabalak@kabalak.net>
 *     2008     Igalia
 *
 * gtranslator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or   
 * (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *   Fatih Demir <kabalak@kabalak.net>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifndef GTR_UTILS_H
#define GTR_UTILS_H 1

#include <gtk/gtkwidget.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkaboutdialog.h>
#include <gtk/gtkwindow.h>
#include <libxml/tree.h>
#include <gio/gio.h>

gchar **       gtranslator_utils_split_string_in_words (const gchar *string);

xmlDocPtr      gtranslator_xml_new_doc (const gchar *name);

xmlDocPtr      gtranslator_xml_open_file (const gchar *filename);

GtkWidget     *gtranslator_gtk_button_new_with_stock_icon (const gchar *label,
				      const gchar *stock_id);

void           gtranslator_utils_menu_position_under_widget (GtkMenu  *menu,
					gint     *x,
					gint     *y,
					gboolean *push_in,
					gpointer  user_data);
					
void           gtranslator_utils_menu_position_under_tree_view (GtkMenu  *menu,
					   gint     *x,
					   gint     *y,
					   gboolean *push_in,
					   gpointer  user_data);

gboolean       gtranslator_utils_get_glade_widgets    (const gchar *filename,
						       const gchar *root_node,
						       GtkWidget **error_widget,
						       const gchar *widget_name,
						       ...)G_GNUC_NULL_TERMINATED;

GSList        *gtranslator_utils_drop_get_locations   (GtkSelectionData *selection_data);

gchar         *gtranslator_utils_escape_search_text   (const gchar* text);

gchar         *gtranslator_utils_unescape_search_text (const gchar *text);

gboolean       g_utf8_caselessnmatch                  (const gchar *s1,
						       const gchar *s2,
						       gssize n1,
						       gssize n2);

void           gtranslator_utils_activate_url         (GtkAboutDialog *dialog,
						       const gchar *url,
						       gpointer data);

void           gtranslator_utils_activate_email       (GtkAboutDialog *dialog,
						       const gchar *email,
						       gpointer data);
						       
void           gtranslator_utils_help_display         (GtkWindow   *parent,
						       const gchar *doc_id,
						       const gchar *file_name);
						       
gchar         *gtranslator_utils_get_user_config_dir  (void);

gchar         *gtranslator_utils_get_current_date (void);

gchar         *gtranslator_utils_get_current_time (void);

gchar         *gtranslator_utils_get_current_year (void);

void           gtranslator_utils_scan_dir              (GFile *dir,
							GSList **list,
							const gchar *po_name);

#endif
