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

#include "messages.h"

/*
 * The primitive Undo types we do support -- direct function calls are needed
 *  here as there isn't any general handling integrated/possible yet.
 */

/*
 * Register and get the undo stuff for view changes (the previous/current view can
 *  be simply get via the gtranslator_views_* interface).
 */
void gtranslator_undo_register_view_change(void);

/*
 * Replaces should point to revert, a single replace in the current message
 *  should be undoable at place.
 */
void gtranslator_undo_register_replace(const gchar *original, const gchar *replace_string);
void gtranslator_undo_register_replaces(void);

/*
 * Register type changes (untranslated <-> fuzzy <-> translated) -- must get "hidden"
 *  when the message is changed.
 */
void gtranslator_undo_register_type_change(GtrMsgStatus previous_status);

/*
 * Register text deletion/insertion into the translation here.
 */
void gtranslator_undo_register_insert_text(const gint start_pos, const gchar *new_text);
void gtranslator_undo_register_delete_text(const gint start_pos, const gint length);

/*
 * Get if there are any undos remaining.
 */
gboolean gtranslator_undo_is_any_item_remaining(void);

/*
 * Handle the undos/redos opaquely here.
 */
void gtranslator_undo_run(void);
void gtranslator_redo_run(void);

#endif
