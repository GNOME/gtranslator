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
	gpointer	data;

	gchar		*name;
} GtrUndoCore;

#define GTR_UNDO_CORE(x) ((GtrUndoCore *) x)

/*
 * Create a new GtrUndoCore and register it already within our internal
 *  mechanism.
 */
GtrUndoCore *gtranslator_undo_core_new(const gchar *name, GFunc func);

/*
 * Free the given GtrUndoCore.
 */
void gtranslator_undo_core_free(GtrUndoCore **core);

/*
 * Sets the data field of the GtrUndoCore.
 */
void gtranslator_undo_core_set_data(GtrUndoCore **core, gpointer data);

/*
 * Gets the data field string from the GtrUndoCore structure.
 */
gpointer gtranslator_undo_core_get_data(GtrUndoCore **core);

/*
 * Execute the function for a real Undo/Redo -- core function for our Undo
 *  stuff -- performs the undo function with the data in GtrUndoCore.
 */
void gtranslator_undo_core_run(GtrUndoCore **core, gpointer additional_data);

#endif
