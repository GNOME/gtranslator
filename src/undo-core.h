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

#ifndef GTR_UNDO_CORE_H
#define GTR_UNDO_CORE_H 1

#include <glib.h>

/*
 * The GtrUndoCore structure -- not used directly but as the base for all the
 *  Undo/Redo stuff it's needed here.
 */
typedef struct {
	GFunc		function;
	GFunc		reverse_function;
	
	gchar		*name;
	
	gpointer	once;
	gpointer	twice;
	gpointer	trice;
} GtrUndoCore;

#define GTR_UNDO_CORE(x) ((GtrUndoCore *) x)

/*
 * Creation/deletion functions.
 */
GtrUndoCore *gtranslator_undo_core_new(GFunc function, GFunc reverse_function,
	const gchar *name, gconstpointer once, gconstpointer twice,
	gconstpointer trice);

void gtranslator_undo_core_free(GtrUndoCore **core);

#endif
