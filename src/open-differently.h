/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
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

#ifndef GTR_OPEN_DIFFERENTLY_H
#define GTR_OPEN_DIFFERENTLY_H 1

/*
 * These functions are good for opening up compiled gettext/gzipped
 *  po files.
 */

/*
 * Detects if we can open it with our "special" open functions.
 */
gboolean gtranslator_open_po_file(gchar *file); 

/*
 * Open up a compiled gettext po file.
 */ 
void gtranslator_open_compiled_po_file(gchar *file);

/*
 * Open up a gzip'ed plain gettext po file (.po.gz).
 */
void gtranslator_open_gzipped_po_file(gchar *file);

/*
 * Open up a bzip2'ed plain gettext po file (.po.bz2).
 */
void gtranslator_open_bzip2ed_po_file(gchar *file);

/*
 * Open up a compress'ed plain gettext po file (.po.Z).
 */ 
void gtranslator_open_compressed_po_file(gchar *file);

#endif
