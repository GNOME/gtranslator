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

/*
 * Utility function for use within gtranslator.
 */
 
/*
 * Returns the non-prefixed and non-suffixed filename back.
 */
gchar *gtranslator_utils_get_raw_file_name(gchar *filename);

/*
 * Show the homepage of gtranslator.
 */
void gtranslator_utils_show_home_page(GtkWidget  *widget, gpointer useless);

/*
 * Invert the dots in the message.
 */
void gtranslator_utils_invert_dot(gchar *str);

#endif
