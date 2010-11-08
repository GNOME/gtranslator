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

#include <gtk/gtk.h>
#include <libxml/tree.h>
#include <gio/gio.h>

gchar **gtr_utils_split_string_in_words (const gchar * string);

xmlDocPtr gtr_xml_new_doc (const gchar * name);

xmlDocPtr gtr_xml_open_file (const gchar * filename);

GtkWidget *gtr_gtk_button_new_with_stock_icon (const gchar * label,
                                               const gchar * stock_id);

void gtr_utils_menu_position_under_widget (GtkMenu * menu,
                                           gint * x,
                                           gint * y,
                                           gboolean * push_in,
                                           gpointer user_data);

void gtr_utils_menu_position_under_tree_view (GtkMenu * menu,
                                              gint * x,
                                              gint * y,
                                              gboolean * push_in,
                                              gpointer user_data);

gboolean
gtr_utils_get_ui_objects (const gchar * filename,
                          gchar ** root_objects,
                          GtkWidget ** error_widget,
                          const gchar * object_name, ...)
  G_GNUC_NULL_TERMINATED;

     GSList *gtr_utils_drop_get_locations (GtkSelectionData * selection_data);

     gchar *gtr_utils_escape_search_text (const gchar * text);

     gchar *gtr_utils_unescape_search_text (const gchar * text);

     gboolean g_utf8_caselessnmatch (const gchar * s1,
                                     const gchar * s2, gssize n1, gssize n2);

     void gtr_utils_help_display (GtkWindow * parent,
                                  const gchar * doc_id,
                                  const gchar * file_name);

     gchar *gtr_utils_get_user_config_dir (void);

     gchar *gtr_utils_get_current_date (void);

     gchar *gtr_utils_get_current_time (void);

     gchar *gtr_utils_get_current_year (void);

     void gtr_utils_scan_dir (GFile * dir,
                              GSList ** list, const gchar * po_name);

     gchar *gtr_utils_reduce_path (const gchar * path);

     gchar *gtr_utils_escape_underscores (const gchar * text, gssize length);

     gchar *gtr_utils_get_file_from_pixmapsdir (const gchar * filename);

     gchar *gtr_utils_get_file_from_pkgdatadir (const gchar * filename);

     gchar *gtr_utils_get_datadir (void);

     gchar *gtr_utils_get_win32_plugindir (void);
#endif
