/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include <gtk/gtk.h>
#include <gio/gio.h>
#include "window.h"

G_BEGIN_DECLS

/*File*/
void       gtranslator_open_file_dialog         (GtkAction * action,
						 GtranslatorWindow *window);

void       gtranslator_save_current_file_dialog (GtkWidget * widget,
						 GtranslatorWindow *window);

void       gtranslator_save_file_as_dialog      (GtkAction * action,
						 GtranslatorWindow *window);

gboolean   gtranslator_open                     (GFile *location,
						 GtranslatorWindow *window,
						 GError **error);

void       gtranslator_close_tab                (GtranslatorTab *tab,
						 GtranslatorWindow *window);

void       gtranslator_file_close               (GtkAction * widget,
						 GtranslatorWindow *window);

void       gtranslator_file_quit                (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_actions_load_locations   (GtranslatorWindow *window,
						 const GSList      *locations);

/*Edit*/
void       gtranslator_actions_edit_undo        (GtkAction   *action,
						 GtranslatorWindow *window);

void       gtranslator_actions_edit_redo        (GtkAction   *action,
						 GtranslatorWindow *window);

void       gtranslator_actions_edit_cut         (GtkAction   *action,
						 GtranslatorWindow *window);

void       gtranslator_actions_edit_copy        (GtkAction   *action,
						 GtranslatorWindow *window);

void       gtranslator_actions_edit_paste       (GtkAction   *action,
						 GtranslatorWindow *window);

void 	   gtranslator_actions_edit_header 	(GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_edit_message_comment     (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_copy_to_translation
                                                (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_status_toggle_fuzzy
                                                (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_actions_edit_preferences (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_actions_edit_clear       (GtkAction *action,
						 GtranslatorWindow *window);

/* View */
void       gtranslator_actions_view_context     (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_actions_view_translation_memory
						(GtkAction *action,
						 GtranslatorWindow *window);

/*Go*/
void       gtranslator_message_go_to_first      (GtkAction  * action,
						 GtranslatorWindow *window);

void       gtranslator_message_go_to_previous   (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_go_to_next       (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_go_to_last       (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_go_to_next_fuzzy (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_go_to_prev_fuzzy (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_go_to_next_untranslated
                                                (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_go_to_prev_untranslated
                                                (GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_go_to_next_fuzzy_or_untranslated
						(GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_go_to_prev_fuzzy_or_untranslated
						(GtkAction *action,
						 GtranslatorWindow *window);

void       gtranslator_message_jump             (GtkAction *action,
						 GtranslatorWindow *window);

/*Search*/
void       _gtranslator_actions_search_find     (GtkAction   *action,
						 GtranslatorWindow *window);

void       _gtranslator_actions_search_replace  (GtkAction   *action,
						 GtranslatorWindow *window);

/*Documents*/
void       gtranslator_actions_documents_next_document
						(GtkAction  * action,
						 GtranslatorWindow *window);

void       gtranslator_actions_documents_previous_document
						(GtkAction  * action,
						 GtranslatorWindow *window);

/*Help*/
void       gtranslator_window_show_home_page    (GtkAction *action,
						 gpointer useless);

void       gtranslator_cmd_help_contents        (GtkAction *action,
                                                 GtranslatorWindow *window);

void       gtranslator_about_dialog             (GtkAction *action,
						 GtranslatorWindow *window);



G_END_DECLS

#endif
