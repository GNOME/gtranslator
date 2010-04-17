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

#include "gtr-window.h"

G_BEGIN_DECLS
/*File*/
void gtr_open_file_dialog (GtkAction * action, GtrWindow * window);

void gtr_save_current_file_dialog (GtkWidget * widget, GtrWindow * window);

void gtr_save_file_as_dialog (GtkAction * action, GtrWindow * window);

gboolean gtr_open (GFile * location, GtrWindow * window, GError ** error);

void gtr_close_tab (GtrTab * tab, GtrWindow * window);

void gtr_file_close (GtkAction * widget, GtrWindow * window);

void gtr_file_quit (GtkAction * action, GtrWindow * window);

void gtr_actions_load_locations (GtrWindow * window,
                                 const GSList * locations);

void _gtr_actions_file_close_all (GtkAction * action, GtrWindow * window);

void _gtr_actions_file_save_all (GtkAction * action, GtrWindow * window);

/*Edit*/
void gtr_actions_edit_undo (GtkAction * action, GtrWindow * window);

void gtr_actions_edit_redo (GtkAction * action, GtrWindow * window);

void gtr_actions_edit_cut (GtkAction * action, GtrWindow * window);

void gtr_actions_edit_copy (GtkAction * action, GtrWindow * window);

void gtr_actions_edit_paste (GtkAction * action, GtrWindow * window);

void gtr_actions_edit_header (GtkAction * action, GtrWindow * window);

void gtr_message_copy_to_translation (GtkAction * action, GtrWindow * window);

void gtr_message_status_toggle_fuzzy (GtkAction * action, GtrWindow * window);

void gtr_actions_edit_preferences (GtkAction * action, GtrWindow * window);

void gtr_actions_edit_clear (GtkAction * action, GtrWindow * window);

/* View */
void gtr_actions_view_context (GtkAction * action, GtrWindow * window);

/*Go*/
void gtr_message_go_to_first (GtkAction * action, GtrWindow * window);

void gtr_message_go_to_previous (GtkAction * action, GtrWindow * window);

void gtr_message_go_to_next (GtkAction * action, GtrWindow * window);

void gtr_message_go_to_last (GtkAction * action, GtrWindow * window);

void gtr_message_go_to_next_fuzzy (GtkAction * action, GtrWindow * window);

void gtr_message_go_to_prev_fuzzy (GtkAction * action, GtrWindow * window);

void gtr_message_go_to_next_untranslated
  (GtkAction * action, GtrWindow * window);

void gtr_message_go_to_prev_untranslated
  (GtkAction * action, GtrWindow * window);

void gtr_message_go_to_next_fuzzy_or_untranslated
  (GtkAction * action, GtrWindow * window);

void gtr_message_go_to_prev_fuzzy_or_untranslated
  (GtkAction * action, GtrWindow * window);

void gtr_message_jump (GtkAction * action, GtrWindow * window);

/*Search*/
void _gtr_actions_search_find (GtkAction * action, GtrWindow * window);

void _gtr_actions_search_replace (GtkAction * action, GtrWindow * window);

/*Documents*/
void gtr_actions_documents_next_document
  (GtkAction * action, GtrWindow * window);

void gtr_actions_documents_previous_document
  (GtkAction * action, GtrWindow * window);

/*Help*/
void gtr_window_show_home_page (GtkAction * action, gpointer useless);

void gtr_cmd_help_contents (GtkAction * action, GtrWindow * window);

void gtr_about_dialog (GtkAction * action, GtrWindow * window);



G_END_DECLS
#endif
