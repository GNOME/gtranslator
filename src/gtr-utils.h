/*
 * Copyright (C) 2001  Fatih Demir <kabalak@kabalak.net>
 *               2008  Igalia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

xmlDocPtr gtr_xml_new_doc (const gchar * name);

xmlDocPtr gtr_xml_open_file (const gchar * filename);

     gchar *gtr_utils_escape_search_text (const gchar * text);

     gchar *gtr_utils_unescape_search_text (const gchar * text);

     gboolean g_utf8_caselessnmatch (const gchar * s1,
                                     const gchar * s2, gssize n1, gssize n2);

     void gtr_utils_help_display (GtkWindow * parent);

     gchar *gtr_utils_get_user_config_dir (void);

     gchar *gtr_utils_get_current_date (void);

     gchar *gtr_utils_get_current_time (void);

     gchar *gtr_utils_get_current_year (void);

     gchar *gtr_utils_reduce_path (const gchar * path);

     gchar *gtr_utils_escape_underscores (const gchar * text, gssize length);

     gchar *gtr_utils_get_file_from_pixmapsdir (const gchar * filename);

     gchar *gtr_utils_get_file_from_pkgdatadir (const gchar * filename);

     gchar *gtr_utils_get_datadir (void);

     gchar *gtr_utils_get_win32_plugindir (void);

     gchar * gtr_utils_get_filename (const gchar * filename);

     gchar * pango_font_description_to_css (PangoFontDescription *desc);

     int parse_nplurals_header (const gchar * plurals_header);
#endif
