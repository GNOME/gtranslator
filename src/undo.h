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

#ifndef GTR_UNDO_H
#define GTR_UNDO_H 1

#include "undo-core.h"

/*
 * The commonly-used GtrUndo structure. 
 */
typedef struct {
	GtrUndoCore 	*core;
	gchar 		*description;
} GtrUndo;

#define GTR_UNDO(x) ((GtrUndo *) x)

#define GTR_UNDO_FUNC(x) \
	(GTR_UNDO_CORE(GTR_UNDO(x)->core)->function)

/*
 * Almost the same like the "gtranslator_undo_new_register" function
 *  but doesn't return any GtrUndo and handles the data locally.
 */
void gtranslator_undo_add(const gchar *description,
	const gchar *actionname, GFunc function, gpointer data);

void gtranslator_redo_add(const gchar *description,
	const gchar *actionname, GFunc function, gpointer data);

/*
 * Undo/Redo the last performed action -- if there's any.
 */
void gtranslator_undo(void);
void gtranslator_redo(void);

/*
 * Lists up all GtrUndo elements in our internal lists.
 */
void gtranslator_undo_get_undo_list(void);
void gtranslator_redo_get_redo_list(void);

/*
 * Free up all the internal list data for Undo/Redo.
 */
void gtranslator_undo_free_lists(void);

#endif
