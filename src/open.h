/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
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

#include "parse.h"
#include "gui.h"

/*
 * Open up a compiled gettext po file.
 */ 
void gtranslator_open_compiled_po_file(gchar *file);

/*
 * Open up a gzipped plain gettext po file.
 */
void gtranslator_open_gzipped_po_file(gchar *file);

#endif
