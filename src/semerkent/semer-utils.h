/*
 * (C) 2001-2003 	Fatih Demir <kabalak@kabalak.net>
 *
 * semerkent is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU Library General Public License as published by the
 *   Free Software Foundation; either version 2 of the License, or (at your
 *    option) any later version.
 *    
 * semerkent is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 *    License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * semerkent is being developed around gtranslator, so that any wishes or own
 *  requests should be mailed to me, as I will naturally keep semerkent very
 *   close to gtranslator. This shouldn't avoid any use of semerkent from other
 *    applications/packages.
 *
 */

#ifndef SEMER_UTILS_H
#define SEMER_UTILS_H 1

#include <glib.h>

G_BEGIN_DECLS

/*
 * Small utility functions for use within Semerkent.
 */
gchar *semer_utils_tmx_date_new(void);
gchar *semer_utils_po_date_new(void);
gchar *semer_utils_umtf_date_new(void);

/*
 * Test if the given file/directory is a regular file/directory and existing.
 */
gboolean semer_utils_test_file(const gchar *filename);
gboolean semer_utils_test_directory(const gchar *directoryname);

/*
 * Free the data safely.
 */
void semer_utils_free(gpointer freedata);

/*
 * Copy the string safely.
 */
gchar *semer_utils_strcopy(const gchar *string); 

/*
 * Set the given string to the also given string -- including free, fallback 
 *  etc. ralated ussues/actions.
 */
void semer_utils_strset(gchar **string, 
	const gchar *set_string,
	const gchar *fallback_string);

G_END_DECLS

#endif /* SEMER_UTILS_H */
