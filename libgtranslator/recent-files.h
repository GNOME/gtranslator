/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
 *
 * libgtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * libgtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef LIBGTRANSLATOR_RECENT_FILES_H
#define LIBGTRANSLATOR_RECENT_FILES_H 1

/*
 * These functions are libgtranslator's recent files' menu stub.
 */

#include <libgtranslator/preferences.h> 

/*
 * Returns the list of the recent files.
 */
GList *gtranslator_get_recent_files(gboolean delete_nonexistent);

/*
 * Saves the given list of (probably) recent filenames.
 */
void gtranslator_set_recent_files(GList *filenames);

/*
 * Returns the length of the "private" recent files' list.
 */
gint gtranslator_get_recent_files_length();

/*
 * Returns the requested nth element in the recent files' list.
 */
gchar *gtranslator_get_recent_files_entry(gint n);

#endif
