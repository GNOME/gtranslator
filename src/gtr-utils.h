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

#pragma once

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <libxml/tree.h>

xmlDocPtr gtr_xml_new_doc (const char *name);

xmlDocPtr gtr_xml_open_file (const char *filename);

     char *gtr_utils_escape_search_text (const char *text);

     char *gtr_utils_unescape_search_text (const char *text);

     gboolean g_utf8_caselessnmatch (const char *s1,
                                     const char *s2,
                                     ssize_t     n1,
                                     ssize_t     n2);

     void gtr_utils_help_display (GtkWindow * parent);

     char *gtr_utils_get_current_date (void);

     char *gtr_utils_get_current_time (void);

     char *gtr_utils_get_current_year (void);

     char *gtr_utils_get_filename (const char *filename);

     char *pango_font_description_to_css (PangoFontDescription *desc);

     int parse_nplurals_header (const char *plurals_header);
