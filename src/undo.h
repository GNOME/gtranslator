/*
 * (C) 2001 	Fatih Demir <kabalak@kabalak.net>
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

#include "messages.h"

/*
 * The primitive Undo types we do support -- direct function calls are needed
 *  here as there isn't any general handling integrated/possible yet.
 */

void gtranslator_undo_register_deletion(const gchar *text, const GtkTextIter *position, const GtkTextIter *endposition);
void gtranslator_undo_register_insertion(const gchar *text, const GtkTextIter *position);

/*
 * Return the status of the register variables used internally for this
 *  piece of enormous insanity.
 */
gboolean gtranslator_undo_get_if_registered_undo(void);

/*
 * These functions are quite clearly named I guess .-)
 */
void gtranslator_undo_clean_register(void);
void gtranslator_undo_run_undo(void);

#endif
